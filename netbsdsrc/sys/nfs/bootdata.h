/*
 * Copyright 1997
 * Digital Equipment Corporation. All rights reserved.
 *
 * This software is furnished under license and may be used and
 * copied only in accordance with the following terms and conditions.
 * Subject to these conditions, you may download, copy, install,
 * use, modify and distribute this software in source and/or binary
 * form. No title or ownership is transferred hereby.
 *
 * 1) Any source code used, modified or distributed must reproduce
 *    and retain this copyright notice and list of conditions as
 *    they appear in the source file.
 *
 * 2) No right is granted to use any trade name, trademark, or logo of
 *    Digital Equipment Corporation. Neither the "Digital Equipment
 *    Corporation" name nor any trademark or logo of Digital Equipment
 *    Corporation may be used to endorse or promote products derived
 *    from this software without the prior written permission of
 *    Digital Equipment Corporation.
 *
 * 3) This software is provided "AS-IS" and any express or implied
 *    warranties, including but not limited to, any implied warranties
 *    of merchantability, fitness for a particular purpose, or
 *    non-infringement are disclaimed. In no event shall DIGITAL be
 *    liable for any damages whatsoever, and in particular, DIGITAL
 *    shall not be liable for special, indirect, consequential, or
 *    incidental damages or damages for lost profits, loss of
 *    revenue or loss of use, whether such damages arise in contract,
 *    negligence, tort, under statute, in equity, at law or otherwise,
 *    even if advised of the possibility of such damage.
 */

/*
 * bootdata.h
 *
 * Data needed to boot a diskless machine.  This is obtained by either
 * DHCP or BOOTPARAM
 */

#ifndef	_NFS_BOOTDATA_H_
#define	_NFS_BOOTDATA_H_

#include <sys/param.h>
#include <sys/mbuf.h>
#include <netinet/in.h>
#include <netinet/dhcp.h>

#if	(BOOT_DHCP + BOOT_BOOTPARAM) != 1
#error	Exactly one of BOOT_DHCP and BOOT_BOOTPARAM must be defined
#endif

struct	bootdata	{
    struct	in_addr		ip_address;	/* my IP address */
    struct	in_addr		ip_mask;	/* my netmask */
    char	hostname[MAXHOSTNAMELEN + 1];	/* my name */
    char	domainname[MAXHOSTNAMELEN + 1];	/* my domain */

    struct	in_addr		root_ip;	/* IP address of root server */
    char	root[MAXPATHLEN + 1];		/* path to root, on server */

    struct	in_addr		swap_ip;	/* IP address of swap server */
    char	swap[MAXPATHLEN + 1];		/* path to swap, on server */

    struct	in_addr		router_ip;	/* default router */

    struct	dhcp_packet	dhcp_packet;	/* the DHCP packet */
};

#define SANITY(c, m)    if (!(c)) panic(m)

#endif
