/*	$NetBSD: machdep.c,v 1.1.1.1 1997/10/03 17:24:11 christos Exp $	*/

/*
 * Copyright (c) 1996
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <sys/cdefs.h>
#ifndef lint
#if 0
static const char rcsid[] =
    "@(#) Header: machdep.c,v 1.3 96/12/10 23:24:28 leres Exp  (LBL)";
#else
__RCSID("$NetBSD: machdep.c,v 1.1.1.1 1997/10/03 17:24:11 christos Exp $");
#endif
#endif

#include <sys/types.h>
#ifdef __osf__
#include <sys/sysinfo.h>
#include <sys/proc.h>
#endif

#include <pcap.h>

#include "machdep.h"

int
abort_on_misalignment(char *ebuf)
{
#ifdef __osf__
	static int buf[2] = { SSIN_UACPROC, UAC_SIGBUS };

	if (setsysinfo(SSI_NVPAIRS, (caddr_t)buf, 1, 0, 0) < 0) {
		(void)sprintf(ebuf, "setsysinfo: %s", pcap_strerror(errno));
		return (-1);
	}
#endif
	return (0);
}
