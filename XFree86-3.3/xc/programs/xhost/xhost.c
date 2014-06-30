/* $XConsortium: xhost.c,v 11.63 95/04/03 20:56:49 mor Exp $ */
/* $XFree86: xc/programs/xhost/xhost.c,v 3.6 1997/01/18 07:02:59 dawes Exp $ */
/*

Copyright (c) 1985, 1986, 1987  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

#if defined(TCPCONN) || defined(STREAMSCONN) || defined(AMTCPCONN)
#define NEEDSOCKETS
#endif
#ifdef UNIXCONN
#define NEEDSOCKETS
#endif
#ifdef DNETCONN
#define NEEDSOCKETS
#endif

#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xproto.h>
#include <X11/Xfuncs.h>
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <ctype.h>
#include <X11/Xauth.h>
#include <X11/Xmu/Error.h>

#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#else
char *malloc();
#endif

#ifdef NEEDSOCKETS
#ifdef att
typedef unsigned short unsign16;
typedef unsigned long unsign32;
typedef short sign16;
typedef long sign32;
#include <interlan/socket.h>
#include <interlan/netdb.h>
#include <interlan/in.h>
#else
#ifndef AMOEBA
#ifndef Lynx
#include <sys/socket.h>
#else
#include <socket.h>
#endif
#include <netdb.h>
#include <netinet/in.h>
#else
#include <server/ip/gen/socket.h>
#include <server/ip/types.h>
#include <server/ip/gen/in.h>
#include <server/ip/gen/inet.h>
#include <server/ip/gen/netdb.h>
#endif
#endif
#endif /* NEEDSOCKETS */

#ifdef notdef
#include <arpa/inet.h>
	bogus definition of inet_makeaddr() in BSD 4.2 and Ultrix
#else
#if !defined(hpux) && !defined(NCR) && !defined(__EMX__)
extern unsigned long inet_makeaddr();
#endif
#endif
#ifdef DNETCONN
#include <netdnet/dn.h>
#include <netdnet/dnetdb.h>
#endif

#ifdef SECURE_RPC
#include <pwd.h>
#include <rpc/rpc.h>
#ifdef X_POSIX_C_SOURCE
#define _POSIX_C_SOURCE X_POSIX_C_SOURCE
#include <limits.h>
#undef _POSIX_C_SOURCE
#else
#if defined(X_NOT_POSIX) || defined(_POSIX_SOURCE)
#include <limits.h>
#else
#define _POSIX_SOURCE
#include <limits.h>
#undef _POSIX_SOURCE
#endif
#endif
#ifndef NGROUPS_MAX
#include <sys/param.h>
#define NGROUPS_MAX NGROUPS
#endif
#endif
 
static int local_xerror();
static char *get_hostname();

#ifdef SIGNALRETURNSINT
#define signal_t int
#else
#define signal_t void
#endif
static signal_t nameserver_lost();

#define NAMESERVER_TIMEOUT 5	/* time to wait for nameserver */

int nameserver_timedout;
 
char *ProgramName;

#ifdef NEEDSOCKETS
static int XFamily(af)
    int af;
{
    int i;
    static struct _familyMap {
	int af, xf;
    } familyMap[] = {
#ifdef	AF_DECnet
        { AF_DECnet, FamilyDECnet },
#endif
#ifdef	AF_CHAOS
        { AF_CHAOS, FamilyChaos },
#endif
#ifdef	AF_INET
        { AF_INET, FamilyInternet },
#endif
};

#define FAMILIES ((sizeof familyMap)/(sizeof familyMap[0]))

    for (i = 0; i < FAMILIES; i++)
	if (familyMap[i].af == af) return familyMap[i].xf;
    return -1;
}
#endif /* NEEDSOCKETS */

Display *dpy;

main(argc, argv)
    int argc;
    char **argv;
{
    register char *arg;
    int i, nhosts;
    char *hostname;
    int nfailed = 0;
    XHostAddress *list;
    Bool enabled = False;
#ifdef DNETCONN
    char *dnet_htoa();
    struct nodeent *np;
    struct dn_naddr *nlist, dnaddr, *dnaddrp, *dnet_addr();
    char *cp;
#endif
 
    ProgramName = argv[0];

    if ((dpy = XOpenDisplay(NULL)) == NULL) {
	fprintf(stderr, "%s:  unable to open display \"%s\"\n",
		ProgramName, XDisplayName (NULL));
	exit(1);
    }

    XSetErrorHandler(local_xerror);
 
 
    if (argc == 1) {
#ifdef DNETCONN
	setnodeent(1);		/* keep the database accessed */
#endif
	sethostent(1);		/* don't close the data base each time */
	list = XListHosts(dpy, &nhosts, &enabled);
	if (enabled)
	    printf ("access control enabled, only authorized clients can connect\n");
	else
	    printf ("access control disabled, clients can connect from any host\n");

	if (nhosts != 0) {
	    for (i = 0; i < nhosts; i++ )  {
		hostname = get_hostname(&list[i]);
		if (hostname) {
		    switch (list[i].family) {
		    case FamilyInternet:
			printf("INET:");
			break;
		    case FamilyDECnet:
			printf("DNET:");
			break;
		    case FamilyNetname:
			printf("NIS:");
			break;
		    case FamilyKrb5Principal:
			printf("KRB:");
			break;
		    case FamilyLocalHost:
			printf("LOCAL:");
			break;
		    }
		    printf ("%s", hostname);
		} else {
		    printf ("<unknown address in family %d>",
			    list[i].family);
		}
		if (nameserver_timedout) {
		    printf("\t(no nameserver response within %d seconds)\n",
			   NAMESERVER_TIMEOUT);
		    nameserver_timedout = 0;
		} else
		    printf("\n");
	    }
	    free(list);
	    endhostent();
	}
	exit(0);
    }
 
    if (argc == 2 && !strcmp(argv[1], "-help")) {
	fprintf(stderr, "usage: %s [[+-]hostname ...]\n", argv[0]);
	exit(1);
    }

    for (i = 1; i < argc; i++) {
	arg = argv[i];
	if (*arg == '-') {
	    
	    if (!argv[i][1] && ((i+1) == argc)) {
		printf ("access control enabled, only authorized clients can connect\n");
		XEnableAccessControl(dpy);
	    } else {
		arg = argv[i][1]? &argv[i][1] : argv[++i];
		if (!change_host (dpy, arg, False)) {
		    fprintf (stderr, "%s:  bad hostname \"%s\"\n",
			     ProgramName, arg);
		    nfailed++;
		}
	    }
	} else {
	    if (*arg == '+' && !argv[i][1] && ((i+1) == argc)) {
		printf ("access control disabled, clients can connect from any host\n");
		XDisableAccessControl(dpy);
	    } else {
		if (*arg == '+') {
		    arg = argv[i][1]? &argv[i][1] : argv[++i];
		}
		if (!change_host (dpy, arg, True)) {
		    fprintf (stderr, "%s:  bad hostname \"%s\"\n",
			     ProgramName, arg);
		    nfailed++;
		}
	    }
	}
    }
    XCloseDisplay (dpy);	/* does an XSync first */
    exit(nfailed);
}

 

/*
 * change_host - edit the list of hosts that may connect to the server;
 * it parses DECnet names (expo::), Internet addresses (18.30.0.212), or
 * Internet names (expo.lcs.mit.edu); if 4.3bsd macro h_addr is defined
 * (from <netdb.h>), it will add or remove all addresses with the given
 * address.
 */

int change_host (dpy, name, add)
    Display *dpy;
    char *name;
    Bool add;
{
    struct hostent *hp;
    XHostAddress ha;
    char *lname;
    int namelen, i, family = FamilyWild;
#ifdef K5AUTH
    krb5_principal princ;
    krb5_data kbuf;
#endif
#ifdef NEEDSOCKETS
#ifndef AMTCPCONN
    static struct in_addr addr;	/* so we can point at it */
#else
    static ipaddr_t addr;
#endif
#endif
    char *cp;
#ifdef DNETCONN
    struct dn_naddr *dnaddrp;
    struct nodeent *np;
    static struct dn_naddr dnaddr;
#endif				/* DNETCONN */
    static char *add_msg = "being added to access control list";
    static char *remove_msg = "being removed from access control list";

    namelen = strlen(name);
    if ((lname = (char *)malloc(namelen+1)) == NULL) {
	fprintf (stderr, "%s: malloc bombed in change_host\n", ProgramName);
	exit (1);
    }
    for (i = 0; i < namelen; i++) {
	lname[i] = tolower(name[i]);
    }
    lname[namelen] = '\0';
    if (!strncmp("inet:", lname, 5)) {
#if defined(TCPCONN) || defined(STREAMSCONN) || defined(AMTCPCONN)
	family = FamilyInternet;
	name += 5;
#else
	fprintf (stderr, "%s: not compiled for TCP/IP\n", ProgramName);
	return 0;
#endif
    }
    if (!strncmp("dnet:", lname, 5)) {
#ifdef DNETCONN
	family = FamilyDECnet;
	name += 5;
#else
	fprintf (stderr, "%s: not compiled for DECnet\n", ProgramName);
	return 0;
#endif
    }
    if (!strncmp("nis:", lname, 4)) {
#ifdef SECURE_RPC
	family = FamilyNetname;
	name += 4;
#else
	fprintf (stderr, "%s: not compiled for Secure RPC\n", ProgramName);
	return 0;
#endif
    }
    if (!strncmp("krb:", lname, 4)) {
#ifdef K5AUTH
	family = FamilyKrb5Principal;
	name +=4;
#else
	fprintf (stderr, "%s: not compiled for Kerberos 5\n", ProgramName);
	return 0;
#endif
    }
    if (!strncmp("local:", lname, 6)) {
	family = FamilyLocalHost;
    }
    if (family == FamilyWild && (cp = strchr(lname, ':'))) {
	*cp = '\0';
	fprintf (stderr, "%s: unknown address family \"%s\"\n",
		 ProgramName, lname);
	return 0;
    }
    free(lname);

#ifdef DNETCONN
    if (family == FamilyDECnet ||
	(cp = strchr(name, ':')) && (*(cp + 1) == ':') &&
	!(*cp = '\0')) {
	ha.family = FamilyDECnet;
	if (dnaddrp = dnet_addr(name)) {
	    dnaddr = *dnaddrp;
	} else {
	    if ((np = getnodebyname (name)) == NULL) {
		fprintf (stderr, "%s:  unable to get node name for \"%s::\"\n",
			 ProgramName, name);
		return 0;
	    }
	    dnaddr.a_len = np->n_length;
	    memmove( dnaddr.a_addr, np->n_addr, np->n_length);
	}
	ha.length = sizeof(struct dn_naddr);
	ha.address = (char *)&dnaddr;
	if (add) {
	    XAddHost (dpy, &ha);
	    printf ("%s:: %s\n", name, add_msg);
	} else {
	    XRemoveHost (dpy, &ha);
	    printf ("%s:: %s\n", name, remove_msg);
	}
	return 1;
    }
#endif				/* DNETCONN */
#ifdef K5AUTH
    if (family == FamilyKrb5Principal) {
	krb5_error_code retval;

	retval = krb5_parse_name(name, &princ);
	if (retval) {
	    krb5_init_ets();	/* init krb errs for error_message() */
	    fprintf(stderr, "%s: cannot parse Kerberos name: %s\n",
		    ProgramName, error_message(retval));
	    return 0;
	}
	XauKrb5Encode(princ, &kbuf);
	ha.length = kbuf.length;
	ha.address = kbuf.data;
	ha.family = family;
	if (add)
	    XAddHost(dpy, &ha);
	else
	    XRemoveHost(dpy, &ha);
	krb5_free_principal(princ);
	free(kbuf.data);
	printf( "%s %s\n", name, add ? add_msg : remove_msg);
	return 1;
    }
#endif
    if (family == FamilyLocalHost) {
	ha.length = 0;
	ha.address = "";
	ha.family = family;
	if (add)
	    XAddHost(dpy, &ha);
	else
	    XRemoveHost(dpy, &ha);
	printf( "non-network local connections %s\n", add ? add_msg : remove_msg);
	return 1;
    }
    /*
     * If it has an '@', it's a netname
     */
    if ((family == FamilyNetname && (cp = strchr(name, '@'))) ||
	(cp = strchr(name, '@'))) {
        char *netname = name;
#ifdef SECURE_RPC
	static char username[MAXNETNAMELEN];

	if (!cp[1]) {
	    struct passwd *pwd;
	    static char domainname[128];

	    *cp = '\0';
	    pwd = getpwnam(name);
	    if (!pwd) {
		fprintf(stderr, "no such user \"%s\"\n", name);
		return 0;
	    }
	    getdomainname(domainname, sizeof(domainname));
	    if (!user2netname(username, pwd->pw_uid, domainname)) {
		fprintf(stderr, "failed to get netname for \"%s\"\n", name);
		return 0;
	    }
	    netname = username;
	}
#endif
	ha.family = FamilyNetname;
	ha.length = strlen(netname);
	ha.address = netname;
	if (add)
	    XAddHost (dpy, &ha);
	else
	    XRemoveHost (dpy, &ha);
	if (netname != name)
	    printf ("%s@ (%s) %s\n", name, netname, add ? add_msg : remove_msg);
	else
	    printf ("%s %s\n", netname, add ? add_msg : remove_msg);
        return 1;
    }
#ifdef NEEDSOCKETS
    /*
     * First see if inet_addr() can grok the name; if so, then use it.
     */
#ifndef AMTCPCONN
    if ((addr.s_addr = inet_addr(name)) != -1) {
#else
    if ((addr = inet_addr(name)) != -1) {
#endif
	ha.family = FamilyInternet;
	ha.length = 4;		/* but for Cray would be sizeof(addr.s_addr) */
	ha.address = (char *)&addr; /* but for Cray would be &addr.s_addr */
	if (add) {
	    XAddHost (dpy, &ha);
	    printf ("%s %s\n", name, add_msg);
	} else {
	    XRemoveHost (dpy, &ha);
	    printf ("%s %s\n", name, remove_msg);
	}
	return 1;
    } 
    /*
     * Is it in the namespace?
     */
    else if (((hp = gethostbyname(name)) == (struct hostent *)NULL)
	     || hp->h_addrtype != AF_INET) {
	return 0;
    } else {
	ha.family = XFamily(hp->h_addrtype);
	ha.length = hp->h_length;
#ifdef h_addr			/* new 4.3bsd version of gethostent */
    {
	char **list;

	/* iterate over the hosts */
	for (list = hp->h_addr_list; *list; list++) {
	    ha.address = *list;
	    if (add) {
		XAddHost (dpy, &ha);
	    } else {
		XRemoveHost (dpy, &ha);
	    }
	}
    }
#else
	ha.address = hp->h_addr;
	if (add) {
	    XAddHost (dpy, &ha);
	} else {
	    XRemoveHost (dpy, &ha);
	}
#endif
	printf ("%s %s\n", name, add ? add_msg : remove_msg);
	return 1;
    }
#else /* NEEDSOCKETS */
    return 0;
#endif /* NEEDSOCKETS */
}


/*
 * get_hostname - Given an internet address, return a name (CHARON.MIT.EDU)
 * or a string representing the address (18.58.0.13) if the name cannot
 * be found.
 */

jmp_buf env;

static char *get_hostname (ha)
    XHostAddress *ha;
{
#if defined(TCPCONN) || defined(STREAMSCONN) || defined(AMTCPCONN)
    struct hostent *hp = NULL;
    char *inet_ntoa();
#endif
#ifdef DNETCONN
    struct nodeent *np;
    static char nodeaddr[5 + 2 * DN_MAXADDL];
#endif				/* DNETCONN */
#ifdef K5AUTH
    krb5_principal princ;
    krb5_data kbuf;
    char *kname;
    static char kname_out[255];
#endif

#if defined(TCPCONN) || defined(STREAMSCONN) || defined(AMTCPCONN)
    if (ha->family == FamilyInternet) {
#ifdef CRAY
	struct in_addr t_addr;
	bzero((char *)&t_addr, sizeof(t_addr));
	bcopy(ha->address, (char *)&t_addr, 4);
	ha->address = (char *)&t_addr;
#endif
	/* gethostbyaddr can take a LONG time if the host does not exist.
	   Assume that if it does not respond in NAMESERVER_TIMEOUT seconds
	   that something is wrong and do not make the user wait.
	   gethostbyaddr will continue after a signal, so we have to
	   jump out of it. 
	   */
	signal(SIGALRM, nameserver_lost);
	alarm(4);
	if (setjmp(env) == 0) {
	    hp = gethostbyaddr (ha->address, ha->length, AF_INET);
	}
	alarm(0);
	if (hp)
	    return (hp->h_name);
#ifndef AMTCPCONN
	else return (inet_ntoa(*((struct in_addr *)(ha->address))));
#else
	else return (inet_ntoa(*((ipaddr_t *)(ha->address))));
#endif
    }
#endif
    if (ha->family == FamilyNetname) {
	static char netname[512];
	int len;
#ifdef SECURE_RPC
	int uid, gid, gidlen, gidlist[NGROUPS_MAX];
#endif

	if (ha->length < sizeof(netname) - 1)
	    len = ha->length;
	else
	    len = sizeof(netname) - 1;
	memmove( netname, ha->address, len);
	netname[len] = '\0';
#ifdef SECURE_RPC
	if (netname2user(netname, &uid, &gid, &gidlen, gidlist)) {
	    struct passwd *pwd;
	    char *cp;

	    pwd = getpwuid(uid);
	    if (pwd)
		sprintf(netname, "%s@ (%*.*s)", pwd->pw_name,
			ha->length, ha->length, ha->address);
	}
#endif
	return (netname);
    }
#ifdef DNETCONN
    if (ha->family == FamilyDECnet) {
	struct dn_naddr *addr_ptr = (struct dn_naddr *) ha->address;

	if (np = getnodebyaddr(addr_ptr->a_addr, addr_ptr->a_len, AF_DECnet)) {
	    sprintf(nodeaddr, "%s", np->n_name);
	} else {
	    sprintf(nodeaddr, "%s", dnet_htoa(ha->address));
	}
	return(nodeaddr);
    }
#endif
#ifdef K5AUTH
    if (ha->family == FamilyKrb5Principal) {
	kbuf.data = ha->address;
	kbuf.length = ha->length;
	XauKrb5Decode(kbuf, &princ);
	krb5_unparse_name(princ, &kname);
	krb5_free_principal(princ);
	strncpy(kname_out, kname, sizeof (kname_out));
	free(kname);
	return kname_out;
    }
#endif
    if (ha->family == FamilyLocalHost) {
	return "";
    }
    return (NULL);
}

static signal_t nameserver_lost()
{
    nameserver_timedout = 1;
    longjmp(env, -1);
}

/*
 * local_xerror - local non-fatal error handling routine. If the error was
 * that an X_GetHosts request for an unknown address format was received, just
 * return, otherwise print the normal error message and continue.
 */
static int local_xerror (dpy, rep)
    Display *dpy;
    XErrorEvent *rep;
{
    if ((rep->error_code == BadAccess) && (rep->request_code == X_ChangeHosts)) {
	fprintf (stderr, 
		 "%s:  must be on local machine to add or remove hosts.\n",
		 ProgramName);
	return 1;
    } else if ((rep->error_code == BadAccess) && 
	       (rep->request_code == X_SetAccessControl)) {
	fprintf (stderr, 
	"%s:  must be on local machine to enable or disable access control.\n",
		 ProgramName);
	return 1;
    } else if ((rep->error_code == BadValue) && 
	       (rep->request_code == X_ListHosts)) {
	return 1;
    }

    XmuPrintDefaultErrorMessage (dpy, rep, stderr);
    return 0;
}
