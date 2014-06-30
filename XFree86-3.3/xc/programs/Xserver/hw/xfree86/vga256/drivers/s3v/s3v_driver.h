/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/s3v/s3v_driver.h,v 1.1.2.2 1997/06/01 12:33:40 dawes Exp $ */

/* Header file for ViRGE server */

/* Declared in s3v_driver.c */
extern vgaVideoChipRec S3V;
extern vgaCRIndex, vgaCRReg;

/* Driver variables */

/* Driver data structure; this should contain all neeeded info for a mode */
typedef struct {     
   vgaHWRec std;
   unsigned char SR10, SR11, SR12, SR13, SR15, SR18; /* SR9-SR1C, ext seq. */
   unsigned char Clock;
   unsigned char s3DacRegs[0x101];
   unsigned char CR31, CR33, CR34, CR36, CR3A, CR3B, CR3C;
   unsigned char CR42, CR43;
   unsigned char CR51, CR53, CR54, CR58, CR5D, CR5E;
   unsigned char CR63, CR65, CR66, CR67, CR68, CR69; /* Video attrib. */
   unsigned char CR86;
   unsigned char CR90;
   unsigned char ColorStack[8]; /* S3 hw cursor color stack CR4A/CR4B */
   unsigned int  STREAMS[22];   /* Streams regs */
   unsigned int  MMPR0, MMPR1, MMPR2, MMPR3;   /* MIU regs */
} vgaS3VRec, *vgaS3VPtr;

/*
 * This is a convenience macro, so that entries in the driver structure
 * can simply be dereferenced with 'new->xxx'.
 */

#define new ((vgaS3VPtr)vgaNewVideoState)


/* PCI info structure */

typedef struct S3PCIInformation {
   int DevID;
   int ChipType;
   int ChipRev;
   unsigned long MemBase;
} S3PCIInformation;

/* Private data structure used for storing all variables needed in driver */
/* This is not exported outside of here, so no need to worry about packing */

typedef struct {
   int chip;
   unsigned short ChipId;
   pointer MmioMem;
   int MemOffScreen;
   int HorizScaleFactor;
   Bool STREAMSRunning;
   Bool NeedSTREAMS;
   int Width, Bpp,Bpl, ScissB;
   unsigned PlaneMask;
   int bltbug_width1, bltbug_width2;
   int MCLK;
   Bool NoPCIRetry;
} S3VPRIV;


/* Function prototypes */

S3PCIInformation * s3vGetPCIInfo();
extern Bool S3VCursorInit();
extern void S3VRestoreCursor();
extern void S3VWarpCursor();
extern void S3VQueryBestSize();


/* Various defines which are used to pass flags between the Setup and 
 * Subsequent functions. 
 */

#define NO_MONO_FILL      0x00
#define NEED_MONO_FILL    0x01
#define MONO_TRANSPARENCY 0x02


/* This function was taken from accel/s3v.h. It adjusts the width
 * of transfers for mono images to works around some bugs.
 */

extern S3VPRIV s3vPriv;

static __inline__ int S3VCheckLSPN(int w, int dir)
{
   int lspn = (w * s3vPriv.Bpp) & 63;  /* scanline width in bytes modulo 64*/

   if (s3vPriv.Bpp == 1) {
      if (lspn <= 8*1)
	 w += 16;
      else if (lspn <= 16*1)
	 w += 8;
   } else if (s3vPriv.Bpp == 2) {
      if (lspn <= 4*2)
	 w += 8;
      else if (lspn <= 8*2)
	 w += 4;
   } else {  /* s3vPriv.Bpp == 3 */
      if (lspn <= 3*3) 
	 w += 6;
      else if (lspn <= 6*3)
	 w += 3;
   }
   if (dir && w >= s3vPriv.bltbug_width1 && w <= s3vPriv.bltbug_width2) {
      w = s3vPriv.bltbug_width2 + 1;
   }

   return w;
}

/* And this adjusts color bitblts widths to work around GE bugs */

static __inline__ int S3VCheckBltWidth(int w)
{
   if (w >= s3vPriv.bltbug_width1 && w <= s3vPriv.bltbug_width2) {
      w = s3vPriv.bltbug_width2 + 1;
   }
   return w;
}

/* This next function determines if the Source operand is present in the
 * given ROP. The rule is that both the lower and upper nibble of the rop
 * have to be neither 0x00, 0x05, 0x0a or 0x0f. If a CPU-Screen blit is done
 * with a ROP which does not contain the source, the virge will hang when
 * data is written to the image transfer area. 
 */

static __inline__ Bool S3VROPHasSrc(shifted_rop)
int shifted_rop;
{
    int rop = (shifted_rop & (0xff << 17)) >> 17;

    if ((((rop & 0x0f) == 0x0a) | ((rop & 0x0f) == 0x0f) 
        | ((rop & 0x0f) == 0x05) | ((rop & 0x0f) == 0x00)) &
       (((rop & 0xf0) == 0xa0) | ((rop & 0xf0) == 0xf0) 
        | ((rop & 0xf0) == 0x50) | ((rop & 0xf0) == 0x00)))
            return FALSE;
    else 
            return TRUE;
}

/* This next function determines if the Destination operand is present in the
 * given ROP. The rule is that both the lower and upper nibble of the rop
 * have to be neither 0x00, 0x03, 0x0c or 0x0f. 
 */

static __inline__ Bool S3VROPHasDst(shifted_rop)
int shifted_rop;
{
    int rop = (shifted_rop & (0xff << 17)) >> 17;

    if ((((rop & 0x0f) == 0x0c) | ((rop & 0x0f) == 0x0f) 
        | ((rop & 0x0f) == 0x03) | ((rop & 0x0f) == 0x00)) &
       (((rop & 0xf0) == 0xc0) | ((rop & 0xf0) == 0xf0) 
        | ((rop & 0xf0) == 0x30) | ((rop & 0xf0) == 0x00)))
            return FALSE;
    else 
            return TRUE;
}
