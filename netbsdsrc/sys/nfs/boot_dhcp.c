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
**  FACILITY
**
**     Data gathering for diskless booting, when data is provided
**     via DHCP.  Currently, the data is collected from the OFW;
**     a place for a complete kernel implementation of DHCP is provided.
**
**  AUTHORS
**
**     Ed Gould		 IAG
**
**  CREATION DATE:
**
**     July, 1997
*/

/*
 * boot_dhcp.c
 *
 * Gather bootstrap data from DHCP
 */

#include <nfs/bootdata.h>
#include "ether.h"

#if	NETHER && BOOT_DHCP

void
get_dhcp_data(struct bootdata *bdp) {
#if	BOOT_FW_DHCP
	get_fw_dhcp_data(bdp);
#else
	/*
	 * XXX machine-independant in-kernel DHCP goes here
	 */
#error	kernel DHCP not implemented
#endif
}

/*
 * Decimal ASCII to integer
 */

static int
char2int(ptr)
    char **ptr;
{
    char *p = *ptr;
    int ret=0;

    if ((*p < '0') || (*p > '9'))
	return(-1);
    while ((*p >= '0') && (*p <= '9')) {
	ret = ret*10 + (*p - '0');
	p++;
    }
    *ptr = p;
    return(ret);
}

/*
 * Dotted quad to IP address, result in network order.
 */

int
dotted2ip(p, i)
    char *p;
    unsigned *i;
{
    u_int32_t ip = 0;
    u_int32_t val;

    if (((val = char2int(&p)) < 0) || (val > 255))
	return(0);
    if (*p != '.')
	return(0);
    p++;
    ip = val;
    if (((val = char2int(&p)) < 0) || (val > 255))
	return(0);
    if (*p != '.')
	return(0);
    p++;
    ip = (ip << 8) | val;
    if (((val = char2int(&p)) < 0) || (val > 255))
	return(0);
    if (*p != '.')
	return(0);
    p++;
    ip = (ip << 8) | val;
    if (((val = char2int(&p)) < 0) || (val > 255))
	return(0);
    *i = htonl((ip << 8) | val);
    return(1);
}

extern char *strchr         __P((char *s1, int c));

void
parse_server_path(
    char *p,
    struct in_addr *in,
    char *path
) {
    char *cp;
    struct in_addr ip;

    *path = '\0';
    if(strncasecmp(p, "nfs://", 6) == 0) {
	/*
	 * A URL format path
	 */
	p += 6;
	/*
	 * Give up on malformed URL
	 */
	if((cp = strchr(p, '/')) == NULL)
	    return;
    } else {
	/*
	 * host_ip:path or just path
	 */
	if((cp = strchr(p, ':')) != NULL)
	    cp++;
    }
    if (dotted2ip(p, &ip.s_addr))
	*in = ip;
    strncpy(path, cp, MAXHOSTNAMELEN);
}

void
parse_dhcp_options(
    struct dhcp_packet *dpp,
    u_char *opt,
    u_char *limit,
    struct bootdata *bdp,
    const char *ip
) {
    int overload;
    char scratch[MAXHOSTNAMELEN + 1];	/* XXX enough stack for these?? */
    char scratch2[MAXHOSTNAMELEN + 1];

    overload = 0;
    while(opt < limit && *opt != DHO_END) {
	int optlen;
	u_char *cp;

	switch(*opt) {

	case DHO_PAD:
	    opt++;
	    continue;

	case DHO_ROOT_PATH:
	    optlen = *++opt;
	    bcopy(++opt, scratch, optlen);
	    scratch[optlen] = '\0';
	    sprintf(scratch2, scratch, ip);
	    parse_server_path(scratch2, &bdp->root_ip, bdp->root);
	    opt += optlen;
	    continue;

	case DHO_EXTENSIONS_PATH:		/* XXX hack for swap path */
	    optlen = *++opt;
	    bcopy(++opt, scratch, optlen);
	    scratch[optlen] = '\0';
	    sprintf(scratch2, scratch, ip);
	    parse_server_path(scratch2, &bdp->swap_ip, bdp->swap);
	    opt += optlen;
	    continue;

	case DHO_VENDOR_ENCAPSULATED_OPTIONS:
	    /*
	     * According to JDD's spec, this option should contain
	     * NFS URLs for root, swap, and tmp.  Per RFC 2132, it may
	     * either be an unstructured string or parsed the same
	     * was as the options buffer.  If parsed, the option numbers
	     * may be redefined.  For now, it is an unstructured string
	     * containing the swap path.
	     */
	    optlen = *++opt;
#if 1
	    bcopy(++opt, scratch, optlen);
	    scratch[optlen] = '\0';
	    sprintf(scratch2, scratch, ip);
	    parse_server_path(scratch2, &bdp->swap_ip, bdp->swap);
#else
	    /* If/when JDDs spec gets fleshed out */
	    parse_dhcp_options(NULL, opt, opt + optlen, bdp, ip);
#endif
	    opt += optlen;
	    continue;

	case DHO_SUBNET_MASK:
	    optlen = *++opt;
	    SANITY(optlen == 4, "Bad DHCP packet (DHO_SUBNET_MASK)");
	    bcopy(++opt, (char *)&bdp->ip_mask.s_addr, 4);
	    opt += 4;
	    continue;

	case DHO_ROUTERS:
	    optlen = *++opt;
	    SANITY(optlen >= 4, "Bad DHCP packet (DHO_ROUTERS)");
	    /*
	     * Take only the first one, for now
	     */
	    bcopy(++opt, (char *)&bdp->router_ip.s_addr, 4);
	    opt += optlen;
	    continue;

	case DHO_SWAP_SERVER:
	    optlen = *++opt;
	    SANITY(optlen == 4, "Bad DHCP packet (DHO_SWAP_SERVER)");
	    opt++;
	    /*
	     * If the swap server address is already set, it's because
	     * it was explicitly specified with the swap path.  Don't
	     * override that.
	     */
	    if(bdp->swap_ip.s_addr == 0)
		bcopy(opt, (char *)&bdp->swap_ip.s_addr, 4);
	    opt += 4;
	    continue;

	case DHO_HOST_NAME:
	    optlen = *++opt;
	    bcopy(++opt, scratch, optlen);
	    scratch[optlen] = '\0';
	    sprintf(scratch2, scratch, ip);
	    strncpy(bdp->hostname, scratch2, MAXHOSTNAMELEN);
	    opt += optlen;
	    continue;

	case DHO_DOMAIN_NAME:
	    optlen = *++opt;
	    bcopy(++opt, scratch, optlen);
	    scratch[optlen] = '\0';
	    sprintf(scratch2, scratch, ip);
	    strncpy(bdp->domainname, scratch2, MAXHOSTNAMELEN);
	    opt += optlen;
	    continue;

	case DHO_DHCP_OPTION_OVERLOAD:
	    optlen = *++opt;
	    SANITY(optlen == 1, "Bad DHCP packet (DHO_DHCP_OPTION_OVERLOAD)");
	    overload = *++opt;
	    opt++;
	    continue;

	default:
	    optlen = *++opt;
	    opt += optlen + 1;
	    continue;
	}
    }
    SANITY(opt < limit && *opt == DHO_END,
			"Unexpected end of DHCP packet or malformed options");
    if(dpp != NULL) {
	if(overload & 01)
	    parse_dhcp_options(
		NULL,
		dpp->file,
		dpp->file + sizeof(dpp->file),
		bdp,
		ip
	    );
	if(overload & 02)
	    parse_dhcp_options(
		NULL,
		dpp->sname,
		dpp->sname + sizeof(dpp->sname),
		bdp,
		ip
	    );
    }
}

#endif	/* NETHER && BOOT_DHCP */
