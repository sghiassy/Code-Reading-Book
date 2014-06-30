/*
 * some bits copied from mprof by Ben Zorn
 *
 * Copyright (c) 1995 Jeffrey Hsu
 */

/* $XFree86: xc/util/memleak/getreti386.c,v 3.2 1996/10/16 14:46:28 dawes Exp $ */

#define get_current_fp(first_local)	((unsigned)&(first_local) + 4)
#define prev_fp_from_fp(fp)		*((unsigned *) fp)
#define ret_addr_from_fp(fp)		*((unsigned *) (fp+4))

#ifdef __FreeBSD__
#define CRT0_ADDRESS		0x10d3
#endif
#if defined(__NetBSD__) || defined(__OpenBSD__)
#define CRT0_ADDRESS            0x109a
#endif

void
getStackTrace(unsigned long *results, int max)
{

	int first_local;
	unsigned long fp, ret_addr;

	fp = get_current_fp(first_local);
	ret_addr = ret_addr_from_fp(fp);

	while (ret_addr > CRT0_ADDRESS && max--) {
	  *results++ = ret_addr;
	  fp = prev_fp_from_fp(fp);
	  ret_addr = ret_addr_from_fp(fp);
	}

}
