/* $XConsortium: ct_BlitMM.h /main/2 1996/10/25 10:28:31 kaleb $ */





/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/chips/ct_BlitMM.h,v 3.4.2.1 1997/05/03 09:47:56 dawes Exp $ */

/* Definitions for the Chips and Technology BitBLT engine communication. */
/* These are done using Memory Mapped IO, of the registers */
/* BitBLT modes for register 93D0. */

#define ctPATCOPY               0xF0
#define ctTOP2BOTTOM            0x100
#define ctBOTTOM2TOP            0x000
#define ctLEFT2RIGHT            0x200
#define ctRIGHT2LEFT            0x000
#define ctSRCFG                 0x400
#define ctSRCMONO               0x800
#define ctPATMONO               0x1000
#define ctBGTRANSPARENT         0x2000
#define ctSRCSYSTEM             0x4000
#define ctPATSOLID              0x80000L
#define ctPATSTART0             0x00000L
#define ctPATSTART1             0x10000L
#define ctPATSTART2             0x20000L
#define ctPATSTART3             0x30000L
#define ctPATSTART4             0x40000L
#define ctPATSTART5             0x50000L
#define ctPATSTART6             0x60000L
#define ctPATSTART7             0x70000L

/* Macros to do useful things with the C&T BitBLT engine */
#define ctBLTWAIT \
  {HW_DEBUG(0x4); \
   while(*(volatile unsigned int *)(ctMMIOBase + MR(0x4)) & \
   0x00100000){};}

#define ctSETROP(op) \
  {HW_DEBUG(0x4); *(unsigned int *)(ctMMIOBase + MR(0x4)) = (op);}

#define ctSETSRCADDR(srcAddr) \
  {HW_DEBUG(0x5); \
   *(unsigned int *)(ctMMIOBase + MR(0x5)) = (srcAddr)&0x7FFFFFL;}

#define ctSETDSTADDR(dstAddr) \
{HW_DEBUG(0x6); \
  *(unsigned int *)(ctMMIOBase + MR(0x6)) = (dstAddr)&0x7FFFFFL;}

#define ctSETPITCH(srcPitch,dstPitch) \
{HW_DEBUG(0x0); \
  *(unsigned int *)(ctMMIOBase + MR(0x0)) = (((dstPitch)&0xFFFF)<<16)| \
      ((srcPitch)&0xFFFF);}

#define ctSETHEIGHTWIDTHGO(Height,Width)\
{HW_DEBUG(0x7); \
  *(unsigned int *)(ctMMIOBase + MR(0x7)) = (((Height)&0xFFFF)<<16)| \
      ((Width)&0xFFFF);}

#define ctSETPATSRCADDR(srcAddr)\
{HW_DEBUG(0x1); \
  *(unsigned int *)(ctMMIOBase + MR(0x1)) = (srcAddr)&0x1FFFFFL;}

#define ctSETBGCOLOR8(bgColor)\
{HW_DEBUG(0x2); \
  *(unsigned int *)(ctMMIOBase + MR(0x2)) = \
           ((((((bgColor)&0xFF)<<8)|((bgColor)&0xFF))<<16) | \
	   ((((bgColor)&0xFF)<<8)|((bgColor)&0xFF)));}

#define ctSETBGCOLOR16(bgColor)\
{HW_DEBUG(0x2); \
  *(unsigned int *)(ctMMIOBase + MR(0x2)) = \
           ((((bgColor)&0xFFFF)<<16)|((bgColor)&0xFFFF));}

/* As the 6554x doesn't support 24bpp colour expansion this doesn't work,
 * It is here only for later use with the 65550 */
#define ctSETBGCOLOR24(bgColor)\
{HW_DEBUG(0x2); \
  *(unsigned int *)(ctMMIOBase + MR(0x2)) = \
           ((bgColor)&0xFFFFFF);}

#define ctSETFGCOLOR8(fgColor)\
{HW_DEBUG(0x3); \
  *(unsigned int *)(ctMMIOBase + MR(0x3)) = \
           ((((((fgColor)&0xFF)<<8)|((fgColor)&0xFF))<<16) | \
	   ((((fgColor)&0xFF)<<8)|((fgColor)&0xFF)));}

#define ctSETFGCOLOR16(fgColor)\
{HW_DEBUG(0x3); \
  *(unsigned int *)(ctMMIOBase + MR(0x3)) = \
           ((((fgColor)&0xFFFF)<<16)|((fgColor)&0xFFFF));}

/* As the 6554x doesn't support 24bpp colour expansion this doesn't work,
 * It is here only for later use with the 65550 */
#define ctSETFGCOLOR24(fgColor)\
{HW_DEBUG(0x3); \
  *(unsigned int *)(ctMMIOBase + MR(0x3)) = \
           ((fgColor)&0xFFFFFF);}

#define ctGETHWCUR(status) \
{HW_DEBUG(0x8); status = MMIOmeml(MR(0x8));}
#define ctPUTHWCUR(status) \
{HW_DEBUG(0x8); MMIOmemw(MR(0x8)) = (status);}
