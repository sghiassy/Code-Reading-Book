#define PSZ 8
#include "vga256.h"

#ifdef	__arm32__
/*#include <machine/sysarch.h>*/
#define	arm32_drain_writebuf()	sysarch(1, 0)

static int ctHiQVBank = -1;
#endif


void CHIPSSetRead(int bank)
{
}


void CHIPSSetWrite(int bank)
{
}


void CHIPSSetReadWrite(int bank)
{
}


void CHIPSWINSetRead(int bank)
{
}


void CHIPSWINSetWrite(int bank)
{
}


void CHIPSWINSetReadWrite(int bank)
{
}


void CHIPSHiQVSetRead(int bank)
{
    outw(0x3D6, (((bank & 0x7F) << 8) | 0x0E));

#ifdef	__arm32__
    /* Must drain StrongARM write buffer on bank switch! */
    if (bank != ctHiQVBank) {
	arm32_drain_writebuf();
	ctHiQVBank = bank;
    }
#endif
}


void CHIPSHiQVSetWrite(int bank)
{
    outw(0x3D6, (((bank & 0x7F) << 8) | 0x0E));

#ifdef	__arm32__
    /* Must drain StrongARM write buffer on bank switch! */
    if (bank != ctHiQVBank) {
	arm32_drain_writebuf();
	ctHiQVBank = bank;
    }
#endif
}


void CHIPSHiQVSetReadWrite(int bank)
{
    outw(0x3D6, (((bank & 0x7F) << 8) | 0x0E));

#ifdef	__arm32__
    /* Must drain StrongARM write buffer on bank switch! */
    if (bank != ctHiQVBank) {
	arm32_drain_writebuf();
	ctHiQVBank = bank;
    }
#endif
}
