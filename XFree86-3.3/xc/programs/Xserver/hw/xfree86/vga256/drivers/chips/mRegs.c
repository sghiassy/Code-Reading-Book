/* $XConsortium: mRegs.c /main/2 1996/10/27 11:49:43 kaleb $ */





/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/chips/mRegs.c,v 3.2 1996/12/27 07:05:14 dawes Exp $ */

#ifdef __NetBSD__
/*
 * For NetBSD compile with -li386
 */
# include <sys/types.h>
# include <machine/pio.h>
# include <machine/sysarch.h>
# define iopl i386_iopl
#else /* NetBSD */
# include "AsmMacros.h"*/
#endif /* NetBSD */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int hex2int(char *str);

int
main(int argc, char **argv)
{
    int i, value, index;
    char c, cport;
    char *str;
    unsigned int port;

    if (argc < 2) {
	printf("usage: %s Cvvxx [Cvvxx]\n", argv[0]);
	printf("     where C = A|a write vv to ARxx\n");
	printf("             = C|c write vv to CRxx\n");
	printf("             = F|f write vv to FRxx (6555x only)\n");
	printf("             = G|g write vv to GRxx\n");
	printf("             = M|m write vv to MRxx (6555x only)\n");
	printf("             = S|s write vv to SRxx\n");
	printf("             = X|x write vv to XRxx\n");
	printf("     Both vv and xx are in hexadecimal\n");
    }
    if (iopl(3))
	return -1;

    for (i = 1; i < argc; i++) {
	value = 0;
	str = argv[i];
	c = *str++;
	switch (c) {
	case 'f':
	case 'F':
	    cport = 'F';
	    port = 0x3D0;
	    break;
	case 'c':
	case 'C':
	    cport = 'C';
	    port = 0x3D4;
	    break;
	case 'x':
	case 'X':
	    cport = 'X';
	    port = 0x3D6;
	    break;
	case 'g':
	case 'G':
	    cport = 'G';
	    port = 0x3CE;
	    break;
	case 'a':
	case 'A':
	    cport = 'A';
	    port = 0x3C0;
	    break;
	case 's':
	case 'S':
	    cport = 'S';
	    port = 0x3C4;
	    break;
	case 'm':
	case 'M':
	    cport = 'M';
	    port = 0x3D2;
	    break;
	default:
	    continue;
	    break;
	}
	index = inb(port);
	while (c = *str++) {
	    if (c >= '0' && c <= '9')
		value = (value << 4) | (c - '0');	/*ASCII assumed */
	    else if (c >= 'A' && c < 'G')
		value = (value << 4) | (c - 'A' + 10);	/*ASCII assumed */
	    else if (c >= 'a' && c < 'g')
		value = (value << 4) | (c - 'a' + 10);	/*ASCII assumed */
	}
	outb(port, value & 0xFF);
	printf("%cR%X: 0x%X -> 0x%X\n", cport, value & 0xFF,
	    inb(port + 1) & 0xFF, (value & 0xFF00) >> 8);
	outw(port, value);
	outb(port, index & 0xFF);
    }
    iopl(0);
    return 0;
}
