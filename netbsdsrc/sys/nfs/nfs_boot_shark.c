/*	$NetBSD: nfs_boot.c,v 1.30 1997/01/31 02:57:31 thorpej Exp $	*/

/*
 * Copyright (c) 1995 Adam Glass, Gordon Ross
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the authors may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/conf.h>
#include <sys/device.h>
#include <sys/ioctl.h>
#include <sys/proc.h>
#include <sys/mount.h>
#include <sys/mbuf.h>
#include <sys/reboot.h>
#include <sys/socket.h>
#include <sys/socketvar.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_arp.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/if_inarp.h>

#include <nfs/rpcv2.h>
#include <nfs/nfsproto.h>
#include <nfs/nfs.h>
#include <nfs/nfsdiskless.h>
#include <nfs/krpc.h>
#include <nfs/xdr_subs.h>
#include <nfs/nfs_var.h>
#include <nfs/bootdata.h>

#include "arp.h"
#if NARP == 0

int 
nfs_boot_init(nd, procp)
	struct nfs_diskless *nd;
	struct proc *procp;
{
	panic("nfs_boot_init: NARP == 0");
}

#else /* NARP */

/*
 * Support for NFS diskless booting, specifically getting information
 * about where to boot from, what pathnames, etc.
 *
 * This implememtation uses RARP and the bootparam RPC.
 * We are forced to implement RPC anyway (to get file handles)
 * so we might as well take advantage of it for bootparam too.
 *
 * The diskless boot sequence goes as follows:
 * (1) Use RARP to get our interface address
 * (2) Use RPC/bootparam/whoami to get our hostname,
 *     our IP address, and the server's IP address.
 * (3) Use RPC/bootparam/getfile to get the root path
 * (4) Use RPC/mountd to get the root file handle
 * (5) Use RPC/bootparam/getfile to get the swap path
 * (6) Use RPC/mountd to get the swap file handle
 *
 * (This happens to be the way Sun does it too.)
 */

/* mountd RPC */
static int md_mount __P((struct sockaddr_in *mdsin, char *path,
	struct nfs_args *argp));

/*
 * IP address to dotted quad
 */

char *
ip2dotted(struct in_addr ip) {
    static char dotted[24];

    sprintf(dotted, "%d.%d.%d.%d", (ip.s_addr & 0x000000ff),
				   (ip.s_addr & 0x0000ff00) >>  8,
				   (ip.s_addr & 0x00ff0000) >> 16,
				   (ip.s_addr & 0xff000000) >> 24);
    return(dotted);
}


struct	bootdata	bootdata;

/*
 * Called with an empty nfs_diskless struct to be filled in.
 */
int
nfs_boot_init(nd, procp)
	struct nfs_diskless *nd;
	struct proc *procp;
{
	struct ifreq ireq;
	struct sockaddr_in *sin;
	struct ifnet *ifp;
	struct socket *so;
	int error;

	/*
	 * Find an interface, find its ip address, stuff it, the
	 * implied broadcast addr, and netmask into a nfs_diskless struct.
	 *
	 * This was moved here from nfs_vfsops.c because this procedure
	 * would be quite different if someone decides to write (i.e.) a
	 * BOOTP version of this file (might not use RARP, etc.)
	 */

	/*
	 * Find a network interface.
	 */
	ifp = ifunit(root_device->dv_xname);
	if (ifp == NULL) {
		printf("nfs_boot: no suitable interface");
		return (ENXIO);
	}
	bcopy(ifp->if_xname, ireq.ifr_name, IFNAMSIZ);
	printf("nfs_boot: using network interface '%s'\n",
	    ireq.ifr_name);

	/*
	 * Bring up the interface.
	 *
	 * Get the old interface flags and or IFF_UP into them; if
	 * IFF_UP set blindly, interface selection can be clobbered.
	 */
	if ((error = socreate(AF_INET, &so, SOCK_DGRAM, 0)) != 0)
		panic("nfs_boot: socreate, error=%d", error);
	error = ifioctl(so, SIOCGIFFLAGS, (caddr_t)&ireq, procp);
	if (error)
		panic("nfs_boot: GIFFLAGS, error=%d", error);
	ireq.ifr_flags |= IFF_UP;
	error = ifioctl(so, SIOCSIFFLAGS, (caddr_t)&ireq, procp);
	if (error)
		panic("nfs_boot: SIFFLAGS, error=%d", error);

	/*
	 * Get our IP address.  If we're booting using DHCP, then
	 * either the firmware has a DHCP packet for us or we'll
	 * contact the server and get the packet.  If we're using
	 * BOOTPARAM, then do a reverse ARP for the IP address.
	 */
#if	BOOT_DHCP
	get_dhcp_data(&bootdata);
	printf("DHCP data: IP %s, mask 0x%x, root \"%s\"\n",
	    ip2dotted(bootdata.ip_address), ntohl(bootdata.ip_mask.s_addr),
	    bootdata.root);
	printf("\thost \"%s\" domain \"%s\" swap \"%s\"\n", bootdata.hostname,
	    bootdata.domainname, bootdata.swap);
#endif	/* BOOT_DHCP */
#if	BOOT_BOOTPARAM
	if ((error = revarpwhoami(&bootdata.ip_address, ifp)) != 0) {
		printf("revarp failed, error=%d", error);
		return (EIO);
	}
#endif	/* BOOT_BOOTPARAM */
	printf("nfs_boot: client_addr=0x%x\n",
				(u_int32_t)ntohl(bootdata.ip_address.s_addr));

	/*
	 * Do enough of ifconfig(8) so that the chosen interface
	 * can talk to the servers.  Always set the address; set
	 * the netmask if we know it.
	 *
	 * XXX default route yet to do
	 */
	sin = (struct sockaddr_in *)&ireq.ifr_addr;
	bzero((caddr_t)sin, sizeof(*sin));
	sin->sin_len = sizeof(*sin);
	sin->sin_family = AF_INET;
	if(bootdata.ip_mask.s_addr != 0) {
	    sin->sin_addr = bootdata.ip_mask;
	    error = ifioctl(so, SIOCSIFNETMASK, (caddr_t)&ireq, procp);
	    if (error)
		panic("nfs_boot: set if mask, error=%d", error);
	    printf("nfs_boot: netmask set to 0x%x\n",
				(u_int32_t)ntohl(bootdata.ip_mask.s_addr));
	}
	sin->sin_addr = bootdata.ip_address;
	error = ifioctl(so, SIOCSIFADDR, (caddr_t)&ireq, procp);
	if (error)
		panic("nfs_boot: set if addr, error=%d", error);

	soclose(so);

#if	BOOT_BOOTPARAM
	/*
	 * Now that the interface is ready, we can do BOOTPARAM requests.
	 * The IP address is already in the bootdata structure.
	 */
	if(error = get_bootparam_data(&bootdata))
	    return(error);
#endif	/* BOOT_BOOTPARAM */

	if (bootdata.router_ip.s_addr != 0) {
	    struct sockaddr dst;
	    struct sockaddr gw;
	    struct sockaddr mask;

	    /*
	     * If we know the default router to use, add
	     * a route through it.
	     */
	    bzero((caddr_t)&dst, sizeof(dst));
	    dst.sa_len = sizeof(dst);
	    dst.sa_family = AF_INET;

	    bzero((caddr_t)&gw, sizeof(gw));
	    sin = (struct sockaddr_in *)&gw;
	    sin->sin_len = sizeof(gw);
	    sin->sin_family = AF_INET;
	    sin->sin_addr = bootdata.router_ip;

	    bzero(&mask, sizeof(mask));

	    printf("nfs_boot: gateway=0x%x\n",
				(u_int32_t)ntohl(bootdata.router_ip.s_addr));
	    /* add, dest, gw, mask, flags, 0 */
	    error = rtrequest(RTM_ADD, &dst, &gw, &mask,
			    RTF_UP | RTF_GATEWAY | RTF_STATIC, NULL);
	    if (error)
		printf("nfs_boot: add route, error=%d\n", error);
	}
	/*
	 * Remember the hostname and domain if we got them from the
	 * server and they're not already set (for domain, this is wrong,
	 * really, since the kernel "domainname" should be the NIS
	 * domain, not the DNS domain, but never mind that).
	 */
	if(hostname[0] == '\0' && bootdata.hostname[0] != '\0')
	    strncpy(hostname, bootdata.hostname, MAXHOSTNAMELEN - 1);
	if(domainname[0] == '\0' && bootdata.domainname[0] != '\0')
	    strncpy(domainname, bootdata.domainname, MAXHOSTNAMELEN - 1);
	return (0);
}

/*
 * Mount either the root filesystem or swap file.  The server IP address
 * and pathname to mount were retrieved earlier via either DHCP or
 * BOOTPARAM.
 */

void
nfs_boot_getfh(bpsin, key, ndmntp)
	struct sockaddr_in *bpsin;	/* bootparam server */
	char *key;			/* root or swap */
	struct nfs_dlmount *ndmntp;	/* output */
{
	struct nfs_args *args;
	char *sp, *dp, *endp;
	struct sockaddr_in *sin;
	int error;
	struct in_addr *ip;
	char *path;

	args = &ndmntp->ndm_args;

	/* Initialize mount args. */
	bzero((caddr_t) args, sizeof(*args));
	args->addr     = &ndmntp->ndm_saddr;
	args->addrlen  = args->addr->sa_len;
#ifdef NFS_BOOT_TCP
	args->sotype   = SOCK_STREAM;
#else
	args->sotype   = SOCK_DGRAM;
#endif
	args->fh       = ndmntp->ndm_fh;
	args->hostname = ndmntp->ndm_host;
	args->flags    = NFSMNT_RESVPORT | NFSMNT_NFSV3;

#ifdef	NFS_BOOT_OPTIONS
	args->flags    |= NFS_BOOT_OPTIONS;
#endif
#ifdef	NFS_BOOT_RWSIZE
	/*
	 * Reduce rsize,wsize for interfaces that consistently
	 * drop fragments of long UDP messages.  (i.e. wd8003).
	 * You can always change these later via remount.
	 */
	args->flags   |= NFSMNT_WSIZE | NFSMNT_RSIZE;
	args->wsize    = NFS_BOOT_RWSIZE;
	args->rsize    = NFS_BOOT_RWSIZE;
#endif

	sin = (void*)&ndmntp->ndm_saddr;

	/*
	 * Cons up a struct sockaddr_in for mounting the {root,swap}
	 * based on key.
	 */
	bzero((caddr_t)sin, sizeof(struct sockaddr_in));
	sin->sin_len = sizeof(struct sockaddr_in);
	sin->sin_family = AF_INET;
	if (strcmp(key, "root") == 0) {
	    ip = &bootdata.root_ip;
	    path = bootdata.root;
	} else {
	    ip = &bootdata.swap_ip;
	    path = bootdata.swap;
	}
	if (ip->s_addr == 0 || path == NULL || *path == '\0')
	    panic("nfs_boot: can't find my %sfs", key);

	sin->sin_addr = *ip;
	/*
	 * Get file handle for "key" (root or swap)
	 * using RPC to mountd/mount
	 */
	error = md_mount(sin, path, args);
	if (error)
		panic("nfs_boot: mountd %s, error=%d", key, error);

	/* Set port number for NFS use. */
	/* XXX: NFS port is always 2049, right? */
#ifdef NFS_BOOT_TCP
retry:
#endif
	error = krpc_portmap(sin, NFS_PROG,
		    (args->flags & NFSMNT_NFSV3) ? NFS_VER3 : NFS_VER2,
		    (args->sotype == SOCK_STREAM) ? IPPROTO_TCP : IPPROTO_UDP,
		    &sin->sin_port);

	if (error || (sin->sin_port == htons(0))) {
#ifdef NFS_BOOT_TCP
		if (args->sotype == SOCK_STREAM) {
			args->sotype = SOCK_DGRAM;
			goto retry;
		}
#endif
		panic("nfs_boot: portmap NFS, error=%d", error);
	}

	/* Construct remote path (for getmntinfo(3)) */
	dp = ndmntp->ndm_host;
	endp = dp + MNAMELEN - 1;
	strcpy(dp, ip2dotted(*ip));
	dp += strlen(dp);
	*dp++ = ':';
	for (sp = path; *sp && dp < endp;)
		*dp++ = *sp++;
	*dp = '\0';

}

int nfs_boot_setrecvtimo(so)
struct socket *so;
{
	struct mbuf *m;
	struct timeval *tv;

	m = m_get(M_WAIT, MT_SOOPTS);
	tv = mtod(m, struct timeval *);
	m->m_len = sizeof(*tv);
	tv->tv_sec = 1;
	tv->tv_usec = 0;
	return(sosetopt(so, SOL_SOCKET, SO_RCVTIMEO, m));
}

int nfs_boot_enbroadcast(so)
struct socket *so;
{
	struct mbuf *m;
	int32_t *on;

	m = m_get(M_WAIT, MT_SOOPTS);
	on = mtod(m, int32_t *);
	m->m_len = sizeof(*on);
	*on = 1;
	return(sosetopt(so, SOL_SOCKET, SO_BROADCAST, m));
}

int nfs_boot_sobind_ipport(so, port)
struct socket *so;
u_int16_t port;
{
	struct mbuf *m;
	struct sockaddr_in *sin;
	int error;

	m = m_getclr(M_WAIT, MT_SONAME);
	sin = mtod(m, struct sockaddr_in *);
	sin->sin_len = m->m_len = sizeof(*sin);
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = INADDR_ANY;
	sin->sin_port = htons(port);
	error = sobind(so, m);
	m_freem(m);
	return(error);
}

/*
 * What is the longest we will wait before re-sending a request?
 * Note this is also the frequency of "timeout" messages.
 * The re-send loop counts up linearly to this maximum, so the
 * first complaint will happen after (1+2+3+4+5)=15 seconds.
 */
#define	MAX_RESEND_DELAY 5	/* seconds */
#define TOTAL_TIMEOUT   30	/* seconds */

int nfs_boot_sendrecv(so, nam, sndproc, snd, rcvproc, rcv,
		      from_p, context)
struct socket *so;
struct mbuf *nam;
int (*sndproc) __P((struct mbuf*, void*, int));
struct mbuf *snd;
int (*rcvproc) __P((struct mbuf*, void*));
struct mbuf **rcv, **from_p;
void *context;
{
	int error, rcvflg, timo, secs, waited;
	struct mbuf *m, *from;
	struct uio uio;

	/* Free at end if not null. */
	from = NULL;

	/*
	 * Send it, repeatedly, until a reply is received,
	 * but delay each re-send by an increasing amount.
	 * If the delay hits the maximum, start complaining.
	 */
	waited = timo = 0;
send_again:
	waited += timo;
	if (waited >= TOTAL_TIMEOUT)
		return(ETIMEDOUT);

	/* Determine new timeout. */
	if (timo < MAX_RESEND_DELAY)
		timo++;
	else
		printf("nfs_boot: timeout...\n");

	if (sndproc) {
		error = (*sndproc)(snd, context, waited);
		if (error)
			goto out;
	}

	/* Send request (or re-send). */
	m = m_copypacket(snd, M_WAIT);
	if (m == NULL) {
		error = ENOBUFS;
		goto out;
	}
	error = sosend(so, nam, NULL, m, NULL, 0);
	if (error) {
		printf("nfs_boot: sosend: %d\n", error);
		goto out;
	}
	m = NULL;

	/*
	 * Wait for up to timo seconds for a reply.
	 * The socket receive timeout was set to 1 second.
	 */

	secs = timo;
	for (;;) {
		if (from) {
			m_freem(from);
			from = NULL;
		}
		if (m) {
			m_freem(m);
			m = NULL;
		}
		uio.uio_resid = 1 << 16; /* ??? */
		rcvflg = 0;
		error = soreceive(so, &from, &uio, &m, NULL, &rcvflg);
		if (error == EWOULDBLOCK) {
			if (--secs <= 0)
				goto send_again;
			continue;
		}
		if (error)
			goto out;
#ifdef DIAGNOSTIC
		if (!m || !(m->m_flags & M_PKTHDR)
		    || (1 << 16) - uio.uio_resid != m->m_pkthdr.len)
			panic("nfs_boot_sendrecv: return size");
#endif

		if ((*rcvproc)(m, context))
			continue;

		if (rcv)
			*rcv = m;
		else
			m_freem(m);
		if (from_p) {
			*from_p = from;
			from = NULL;
		}
		break;
	}
out:
	if (from) m_freem(from);
	return(error);
}
/*
 * RPC: mountd/mount
 * Given a server pathname, get an NFS file handle.
 * Also, sets sin->sin_port to the NFS service port.
 */
static int
md_mount(mdsin, path, argp)
	struct sockaddr_in *mdsin;		/* mountd server address */
	char *path;
	struct nfs_args *argp;
{
	/* The RPC structures */
	struct rdata {
		u_int32_t errno;
		union {
			u_int8_t  v2fh[NFSX_V2FH];
			struct {
				u_int32_t fhlen;
				u_int8_t  fh[1];
			} v3fh;
		} fh;
	} *rdata;
	struct mbuf *m;
	u_int8_t *fh;
	int minlen, error;

retry:
	/* Get port number for MOUNTD. */
	error = krpc_portmap(mdsin, RPCPROG_MNT,
		     (argp->flags & NFSMNT_NFSV3) ? RPCMNT_VER3 : RPCMNT_VER1,
		     IPPROTO_UDP, &mdsin->sin_port);
	if (error)
		return error;

	m = xdr_string_encode(path, strlen(path));
	if (m == NULL)
		return ENOMEM;

	/* Do RPC to mountd. */
	error = krpc_call(mdsin, RPCPROG_MNT, (argp->flags & NFSMNT_NFSV3) ?
			  RPCMNT_VER3 : RPCMNT_VER1, RPCMNT_MOUNT, &m, NULL);
	if (error) {
		if ((error==RPC_PROGMISMATCH) && (argp->flags & NFSMNT_NFSV3)) {
			argp->flags &= ~NFSMNT_NFSV3;
			goto retry;
		}
		return error;	/* message already freed */
	}

	/* The reply might have only the errno. */
	if (m->m_len < 4)
		goto bad;
	/* Have at least errno, so check that. */
	rdata = mtod(m, struct rdata *);
	error = fxdr_unsigned(u_int32_t, rdata->errno);
	if (error)
		goto out;

	 /* Have errno==0, so the fh must be there. */
	if (argp->flags & NFSMNT_NFSV3){
		argp->fhsize   = fxdr_unsigned(u_int32_t, rdata->fh.v3fh.fhlen);
		if (argp->fhsize > NFSX_V3FHMAX)
			goto bad;
		minlen = 2 * sizeof(u_int32_t) + argp->fhsize;
	} else {
		argp->fhsize   = NFSX_V2FH;
		minlen = sizeof(u_int32_t) + argp->fhsize;
	}

	if (m->m_len < minlen) {
		m = m_pullup(m, minlen);
		if (m == NULL)
			return(EBADRPC);
		rdata = mtod(m, struct rdata *);
	}

	fh = (argp->flags & NFSMNT_NFSV3) ? rdata->fh.v3fh.fh : rdata->fh.v2fh;
	bcopy(fh, argp->fh, argp->fhsize);

	goto out;

bad:
	error = EBADRPC;

out:
	m_freem(m);
	return error;
}

#endif /* NARP */
