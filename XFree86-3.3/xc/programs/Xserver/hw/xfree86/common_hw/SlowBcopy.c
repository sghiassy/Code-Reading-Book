/* $XConsortium: SlowBcopy.c /main/1 1996/05/07 17:14:10 kaleb $ */
/*******************************************************************************
  for Alpha Linux
*******************************************************************************/

/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/SlowBcopy.c,v 3.3 1997/01/05 11:58:24 dawes Exp $ */
 
/* 
 *   Create a dependency that should be immune from the effect of register
 *   renaming as is commonly seen in superscalar processors.  This should
 *   insert a minimum of 100-ns delays between reads/writes at clock rates
 *   up to 100 MHz---GGL
 *   
 *   Slowbcopy(char *src, char *dst, int count)   
 *   
 */ 

/* The outb() isn't needed on my machine, but who nows ... -- ost */
void
SlowBcopy(src, dst, len)
unsigned char *dst, *src;
int len;
{
    while(len--)
    {
	*dst++ = *src++;
	outb(0x80, 0x00);
    }
}

#ifdef __alpha__
/*
 * The Jensen lacks dense memory, thus we have to address the bus via
 * the sparse addressing scheme. Time critical code uses routines from
 * BUSmemcpy.c
 *
 * Martin Ostermann (ost@comnets.rwth-aachen.de) - Apr.-Sep. 1996
 */

unsigned long _bus_base(void);

#ifdef TEST_JENSEN_CODE /* define to test the Sparse addressing on a non-Jensen */
#define SPARSE (5)
#else
#define SPARSE (7)
#endif

void
SlowBCopyFromBus(src, dst, count)
     unsigned char *src, *dst;
     int count;
{
    if (!_bus_base()) /* Jensen */
    {
	unsigned long addr;
	long result;

	addr = (unsigned long) src;
	while( count ){
	    result = *(volatile int *) addr;
	    result >>= ((addr>>SPARSE) & 3) * 8;
	    *dst++ = (unsigned char) (0xffUL & result);
	    addr += 1<<SPARSE;
	    count--;
	    outb(0x80, 0x00);
	}
    }
    else
	SlowBcopy(src,dst,count);
}
  
void
SlowBCopyToBus(src, dst, count)
     unsigned char *src, *dst;
     int count;
{
    if (!_bus_base()) /* Jensen */
    {
	unsigned long addr;

	addr = (unsigned long) dst;
	while(count) {
	    *(volatile unsigned int *) addr = (unsigned short)(*src) * 0x01010101;
	    src++;
	    addr += 1<<SPARSE;
	    count--;
	    outb(0x80, 0x00);
	}
    }
    else
	SlowBcopy(src,dst,count);    
}
#endif
