/* $XConsortium: ct_BltHiQV.h /main/2 1996/10/25 10:28:43 kaleb $ */





/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/chips/ct_BltHiQV.h,v 3.4.2.1 1997/05/03 09:47:58 dawes Exp $ */

/* Definitions for the Chips and Technology BitBLT engine communication. */
/* These are done using Memory Mapped IO, of the registers */
/* BitBLT modes for register 93D0. */

#define ctPATCOPY               0xF0
#define ctLEFT2RIGHT            0x000
#define ctRIGHT2LEFT            0x100
#define ctTOP2BOTTOM            0x000
#define ctBOTTOM2TOP            0x200
#define ctSRCSYSTEM             0x400
#define ctSRCMONO               0x1000
#define ctBGTRANSPARENT         0x22000
#define ctCOLORTRANSENABLE      0x4000
#define ctCOLORTRANSDISABLE     0x0
#define ctCOLORTRANSDST         0x8000
#define ctCOLORTRANSROP         0x0
#define ctCOLORTRANSEQUAL       0x10000L
#define ctCOLORTRANSNEQUAL      0x0
#define ctPATMONO               0x40000L
#define ctPATSOLID              0x80000L
#define ctPATSTART0             0x000000L
#define ctPATSTART1             0x100000L
#define ctPATSTART2             0x200000L
#define ctPATSTART3             0x300000L
#define ctPATSTART4             0x400000L
#define ctPATSTART5             0x500000L
#define ctPATSTART6             0x600000L
#define ctPATSTART7             0x700000L
#define ctSRCFG                 0x000000L	/* Where is this for the 65550?? */

/* The Monochrome expansion register setup */
#define ctCLIPLEFT(clip)        ((clip)&0x3F)
#define ctCLIPRIGHT(clip)       (((clip)&0x3F) << 8)
#define ctSRCDISCARD(clip)      (((clip)&0x3F) << 16)
#define ctBITALIGN              0x1000000L
#define ctBYTEALIGN             0x2000000L
#define ctWORDALIGN             0x3000000L
#define ctDWORDALIGN            0x4000000L
#define ctQWORDALIGN            0x5000000L
/* This shouldn't be used because not all chip rev's
 * have BR09 and BR0A, and I haven't even defined
 * macros to write to these registers 
 */
#define ctEXPCOLSEL             0x8000000L

/* Macros to do useful things with the C&T BitBLT engine */
#if 0
/* Chips and technologies released an application note saying that
 * with certain batches of chips you couldn't read the blitter registers
 * properly. This could cause some drawing anolomies, use XR20[0] instead
 */
#define ctBLTWAIT \
  while(*(volatile unsigned int *)(ctMMIOBase + BR(0x4)) & \
	(0x80000000)){}
#else
#if 0
#define ctBLTWAIT \
  outb(0x3D6,0x20); while(inb(0x3D7)&0x1){}
#else
/*
 * This version resets the blitter if a timeout occurs
 */
#define ctBLTWAIT \
  outb(0x3D6,0x20); {int timeout; \
                     timeout = 0; \
		     for (;;) { \
                         if (!(inb(0x3D7)&0x1)) break; \
                         timeout++; \
                         if (timeout == 10000000) { \
			    unsigned char tmp; \
                            ErrorF("CHIPS: BitBlt Engine Timeout\n"); \
			    tmp = inb(0x3D7); \
			    outb(0x3D7, ((tmp & 0xFD) | 0x2)); \
			    break; \
                         } \
		      } \
		    }
#endif
#endif

#define ctSETROP(op) \
  *(unsigned int *)(ctMMIOBase + BR(0x4)) = (op)

#define ctSETMONOCTL(op) \
  *(unsigned int *)(ctMMIOBase + BR(0x3)) = (op)

#define ctSETSRCADDR(srcAddr) \
  *(unsigned int *)(ctMMIOBase + BR(0x6)) = (srcAddr)&0x7FFFFFL

#define ctSETDSTADDR(dstAddr) \
  *(unsigned int *)(ctMMIOBase + BR(0x7)) = (dstAddr)&0x7FFFFFL

#define ctSETPITCH(srcPitch,dstPitch) \
  *(unsigned int *)(ctMMIOBase + BR(0x0)) = (((dstPitch)&0xFFFF)<<16)| \
      ((srcPitch)&0xFFFF)

#define ctSETHEIGHTWIDTHGO(Height,Width)\
  *(unsigned int *)(ctMMIOBase + BR(0x8)) = (((Height)&0xFFFF)<<16)| \
      ((Width)&0xFFFF)

#define ctSETPATSRCADDR(srcAddr)\
  *(unsigned int *)(ctMMIOBase + BR(0x5)) = (srcAddr)&0x1FFFFFL

#define ctSETBGCOLOR8(bgColor)\
  *(unsigned int *)(ctMMIOBase + BR(0x1)) = ((bgColor)&0xFF)

#define ctSETBGCOLOR16(bgColor)\
  *(unsigned int *)(ctMMIOBase + BR(0x1)) = ((bgColor)&0xFFFF)

#define ctSETBGCOLOR24(bgColor)\
  *(unsigned int *)(ctMMIOBase + BR(0x1)) = ((bgColor)&0xFFFFFF)

#define ctSETFGCOLOR8(fgColor)\
  *(unsigned int *)(ctMMIOBase + BR(0x2)) = ((fgColor)&0xFF)

#define ctSETFGCOLOR16(fgColor)\
  *(unsigned int *)(ctMMIOBase + BR(0x2)) = ((fgColor)&0xFFFF)

#define ctSETFGCOLOR24(fgColor)\
  *(unsigned int *)(ctMMIOBase + BR(0x2)) = ((fgColor)&0xFFFFFF)

