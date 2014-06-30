/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3init.c,v 3.110.2.5 1997/05/21 15:02:31 dawes Exp $ */
/*
 * Written by Jake Richter Copyright (c) 1989, 1990 Panacea Inc.,
 * Londonderry, NH - All Rights Reserved
 * 
 * This code may be freely incorporated in any program without royalty, as long
 * as the copyright notice stays intact.
 * 
 * Additions by Kevin E. Martin (martin@cs.unc.edu)
 * 
 * KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * 
 */

/*
 * Modified by Amancio Hasty and Jon Tombs
 * 
 */
/* $XConsortium: s3init.c /main/28 1996/10/28 04:55:18 kaleb $ */


#define USE_VGAHWINIT

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "servermd.h"
#include "scrnintstr.h"
#include "site.h"

#include "xf86Procs.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "vga.h"

#include "s3.h"
#include "regs3.h"
#include "s3Bt485.h"
#include "Ti302X.h"
#include "IBMRGB.h"
#define XCONFIG_FLAGS_ONLY 
#include "xf86_Config.h"

typedef struct {
   vgaHWRec std;                /* good old IBM VGA */
   unsigned char s3reg[10];     /* Video Atribute (CR30-34, CR38-3C) */
   unsigned char s3sysreg[46];  /* Video Atribute (CR40-6D)*/
   unsigned char ColorStack[8]; /* S3 hw cursor color stack CR4A/CR4B */
}
vgaS3Rec, *vgaS3Ptr;

extern int   vgaIOBase;
extern int   vgaCRIndex;
extern int   vgaCRReg;
int   s3InitCursorFlag = TRUE;
int   s3HDisplay;
extern xf86InfoRec xf86Info;

static vgaS3Ptr oldS3 = NULL;

static short savedVgaIOBase;

pointer vgaNewVideoState = NULL;
static LUTENTRY oldlut[256];
static Bool LUTInited = FALSE;
static short s3Initialised = 0;
static int   old_clock;
extern Bool (*s3ClockSelectFunc) (
#if NeedNestedPrototypes
	int	/* clock */
#endif
	);
extern int s3DisplayWidth;
extern Bool s3Localbus;
extern Bool s3Mmio928;
extern Bool s3NewMmio;
extern Bool s3PixelMultiplexing;
extern pointer vgaBase;
extern pointer vgaBaseLow;
extern pointer vgaBaseHigh;
int currents3dac_border = 0xff;

static void InitLUT(
#if NeedFunctionPrototypes
	void
#endif
	);

#ifdef PC98
extern void crtswitch(short);
#endif


#define new ((vgaHWPtr)vgaNewVideoState)

unsigned short s3ChipId = 0;
unsigned int  s3ChipRev = 0;

#define	cebank() do {							\
   	if (S3_801_928_SERIES(s3ChipId)) {				\
		unsigned char tmp2;					\
		outb(vgaCRIndex, 0x51);					\
		tmp2 = inb(vgaCRReg);					\
		outb(vgaCRReg, (tmp2 & 0xf3));				\
	}								\
} while (1 == 0)

/*
 * Registers to save/restore in the 0x50 - 0x5f control range
 */

static short reg50_mask = 0x673b; /* was 0x6733; */ /* was 0x4023 */

extern unsigned char s3Port54;
extern unsigned char s3Port51;
extern unsigned char s3Port40;
extern unsigned char s3Port59;
extern unsigned char s3Port5A;
extern unsigned char s3Port31;

void
s3CleanUp(void)
{
   int   i;
   unsigned char tmp;

   UNLOCK_SYS_REGS;
   
   vgaProtect(TRUE);

   if (OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options) &&
       !OFLG_ISSET(OPTION_NOLINEAR_MODE, &s3InfoRec.options) &&
       s3Mmio928) {
     /*
       Clear bit 7 of CRTC register 5C to map video memory normally.
     */
     int CR5C;
     outb(vgaCRIndex, 0x5C);
     CR5C = inb(vgaCRReg);
     outb(vgaCRIndex, 0x5C);
     outb(vgaCRReg, CR5C & ~0x80);
     vgaBase = vgaBaseLow;
   }
   /* BL */
   if (s3NewMmio)	{
      outb (vgaCRIndex, 0x58);
      outb (vgaCRReg, s3SAM256); /* disable linear mode */
   } /* end BL */


   WaitQueue(8);
   outb(vgaCRIndex, 0x35);
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, (tmp & 0xf0));
   cebank();

   outw(ADVFUNC_CNTL, 0);
   if (s3Mmio928 || s3NewMmio) {
      outb(vgaCRIndex, 0x53);
      outb(vgaCRReg, 0x00);
   }

 /* (s3ClockSelectFunc)(restore->std.NoClock); */
  

 /* Restore Ramdac registers */
  (s3Ramdacs[s3RamdacType].DacRestore)();

   if (DAC_IS_TI3025) {
      outb(vgaCRIndex, 0x5C);
      outb(vgaCRReg, oldS3->s3sysreg[0x0C + 16]);
   }

 /* restore s3 special bits */
   if (S3_801_928_SERIES(s3ChipId)) {
    /* restore 801 specific registers */

      for (i = 32; i < (S3_x64_SERIES(s3ChipId) ? 46 : 
                      S3_805_I_SERIES(s3ChipId) ? 40 : 38) ; i++) {
	 outb(vgaCRIndex, 0x40 + i);
	 outb(vgaCRReg, oldS3->s3sysreg[i]);

      }
      for (i = 0; i < 16; i++) {
	 if (!((1 << i) & reg50_mask))
	   continue;
	 outb(vgaCRIndex, 0x50 + i);
	 outb(vgaCRReg, oldS3->s3sysreg[i + 16]);
      }
   }
   for (i = 0; i < 5; i++) {
      outb(vgaCRIndex, 0x30 + i);
      outb(vgaCRReg, oldS3->s3reg[i]);
      outb(vgaCRIndex, 0x38 + i);
      outb(vgaCRReg, oldS3->s3reg[5 + i]);
   }

   for (i = 0; i < 16; i++) {
      outb(vgaCRIndex, 0x40 + i);
      outb(vgaCRReg, oldS3->s3sysreg[i]);
   }

   outb(vgaCRIndex, 0x45);
   inb(vgaCRReg);         /* reset color stack pointer */
   outb(vgaCRIndex, 0x4A);
   for (i = 0; i < 4; i++)
      outb(vgaCRReg, oldS3->ColorStack[i]);

   outb(vgaCRIndex, 0x45);
   inb(vgaCRReg);         /* reset color stack pointer */
   outb(vgaCRIndex, 0x4B);
   for (i = 4; i < 8; i++)
      outb(vgaCRReg, oldS3->ColorStack[i]);


   if (OFLG_ISSET(CLOCK_OPTION_ICS2595, &s3InfoRec.clockOptions)){
      outb(vgaCRIndex, 0x42);
      outb(vgaCRReg, (oldS3->s3sysreg[2] & 0xf0) | 0x01);
      outb(vgaCRIndex, 0x5c);	/* switch back to 28MHz clock */
      outb(vgaCRReg,   0x20);
      outb(vgaCRReg,   0x00);
   }

   vgaHWRestore((vgaHWPtr)oldS3);

   outb(0x3c2, old_clock);

   i = inb(0x3CC);
   if (savedVgaIOBase == 0x3B0)
      i &= 0xFE;
   else
      i |= 0x01;
   outb(0x3C2, i);

   vgaIOBase = savedVgaIOBase;
   vgaCRIndex = vgaIOBase + 4;
   vgaCRReg = vgaIOBase + 5;
      
   vgaProtect(FALSE);

#ifdef PC98
	crtswitch(0);
#endif

   xf86DisableIOPorts(s3InfoRec.scrnIndex);
}

Bool
s3Init(mode)
     DisplayModePtr mode;
{
   short i, m, n;
   int   interlacedived = mode->Flags & V_INTERLACE ? 2 : 1;
   unsigned char tmp, CR5C;
   unsigned int itmp;
   extern Bool s3DAC8Bit;
   int pixMuxShift = 0;

   s3HDisplay = mode->HDisplay;

   UNLOCK_SYS_REGS;

   /* Force use of colour I/O address */
   if (!s3Initialised) {
      savedVgaIOBase = vgaIOBase;
   }
   i = inb(0x3CC);
   outb(0x3C2, i | 0x01);
   vgaIOBase = 0x3D0;
   vgaCRIndex = 0x3D4;
   vgaCRReg = 0x3D5;
      
   if (!s3Initialised) {
    /* blanket save of state */
    /* unlock */
      outb(vgaCRIndex, 0x38);
      outb(vgaCRReg, 0x48);
      old_clock = inb(0x3CC);

      outb(vgaCRIndex, 0x35);	/* select segment 0 */
      i = inb(vgaCRReg);
      outb(vgaCRReg, i & 0xf0);
      cebank();

      oldS3 = vgaHWSave((vgaHWPtr)oldS3, sizeof(vgaS3Rec));

      /*
       * Set up the Serial Access Mode 256 Words Control
       *   (bit 6 in CR58)
       */
      outb(vgaCRIndex, 0x58);
      s3SAM256 = inb(vgaCRReg) & 0x80;

      if (S3_968_SERIES(s3ChipId) || (S3_964_SERIES(s3ChipId) &&
	  !OFLG_ISSET(OPTION_NUMBER_NINE, &s3InfoRec.options)))
         s3SAM256 |= 0x40;
      else if ((OFLG_ISSET(OPTION_SPEA_MERCURY, &s3InfoRec.options) &&
               S3_928_ONLY(s3ChipId)) ||
	       OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options))
         s3SAM256 |= 0x80; /* set 6 MCLK cycles for R/W time on Mercury */

      if (OFLG_ISSET(OPTION_EARLY_RAS_PRECHARGE, &s3InfoRec.options))
	 s3SAM256 |= 0x80;
      if (OFLG_ISSET(OPTION_LATE_RAS_PRECHARGE, &s3InfoRec.options))
	 s3SAM256 &= 0x7f;

      if (DAC_IS_TI3025) {
 	  outb(vgaCRIndex, 0x5C);
	  CR5C = inb(vgaCRReg);
      }

/* Save Ramdac registers */
     (s3Ramdacs[s3RamdacType].DacSave)();

 
      for (i = 0; i < 5; i++) {
	 outb(vgaCRIndex, 0x30 + i);
	 oldS3->s3reg[i] = inb(vgaCRReg);
#ifdef REG_DEBUG
	 ErrorF("CR%X = 0x%02x\n", 0x30 + i, oldS3->s3reg[i]);
#endif
	 outb(vgaCRIndex, 0x38 + i);
	 oldS3->s3reg[5 + i] = inb(vgaCRReg);
#ifdef REG_DEBUG
	 ErrorF("CR%X = 0x%02x\n", 0x38 + i, oldS3->s3reg[i + 5]);
#endif
      }

      outb(vgaCRIndex, 0x11);	/* allow writting to CR0-7 */
      tmp = inb(vgaCRReg);
      outb(vgaCRReg, tmp & 0x7f);
      for (i = 0; i < 16; i++) {
	 outb(vgaCRIndex, 0x40 + i);
	 oldS3->s3sysreg[i] = inb(vgaCRReg);
#ifdef REG_DEBUG
	 ErrorF("CR%X = 0x%02x\n", 0x40 + i, oldS3->s3sysreg[i]);
#endif
      }

      outb(vgaCRIndex, 0x45);
      inb(vgaCRReg);         /* reset color stack pointer */
      outb(vgaCRIndex, 0x4A);
      for (i = 0; i < 4; i++) {
	 oldS3->ColorStack[i] = inb(vgaCRReg);
	 outb(vgaCRReg,oldS3->ColorStack[i]);  /* advance stack pointer */
      }
      
      outb(vgaCRIndex, 0x45);
      inb(vgaCRReg);         /* reset color stack pointer */
      outb(vgaCRIndex, 0x4B);
      for (i = 4; i < 8; i++) {
	 oldS3->ColorStack[i] = inb(vgaCRReg);
	 outb(vgaCRReg,oldS3->ColorStack[i]);  /* advance stack pointer */
      }

      if (S3_801_928_SERIES(s3ChipId)) 
         for (i = 0; i < 16; i++) {
#ifdef REG_DEBUG
	     outb(vgaCRIndex, 0x50 + i);
	     ErrorF("CR%X = 0x%02x\n", 0x50 + i, inb(vgaCRReg));
#endif
	     if (!((1 << i) & reg50_mask))
	       continue;
	     outb(vgaCRIndex, 0x50 + i);
	     oldS3->s3sysreg[i + 16] = inb(vgaCRReg);
          }
      if (DAC_IS_TI3025)  /* restore 5C from above */
         oldS3->s3sysreg[0x0C + 16] = CR5C;

      for (i = 32; i < (S3_x64_SERIES(s3ChipId) ? 46 : 
                      S3_805_I_SERIES(s3ChipId) ? 40 : 38); i++) {
	 outb(vgaCRIndex, 0x40 + i);
	 oldS3->s3sysreg[i] = inb(vgaCRReg);
#ifdef REG_DEBUG
	 ErrorF("CR%X = 0x%02x\n", 0x40 + i, oldS3->s3sysreg[i]);
#endif
      }

      s3Initialised = 1;
      vgaNewVideoState = vgaHWSave(vgaNewVideoState, sizeof(vgaS3Rec));
      outb(DAC_MASK, 0);

   } else if (DAC_IS_TI3025) {
      /* switch the ramdac from bt485 to ti3020 mode clearing RS4 */
      outb(vgaCRIndex, 0x5C);
      CR5C = inb(vgaCRReg);
      outb(vgaCRReg, CR5C & 0xDF);

      /* clear TI_PLANAR_ACCESS bit */
      s3OutTiIndReg(TI_CURS_CONTROL, 0x7F, 0x00);
   }

   if (s3UsingPixMux && (mode->Flags & V_PIXMUX))
      s3PixelMultiplexing = TRUE;
   else
      s3PixelMultiplexing = FALSE;

   if (OFLG_ISSET(OPTION_ELSA_W2000PRO, &s3InfoRec.options))
      pixMuxShift = s3InfoRec.clock[mode->Clock] > 120000 ? 2 : 
		      s3InfoRec.clock[mode->Clock] > 60000 ? 1 : 0 ;
   else if (OFLG_ISSET(OPTION_MIRO_80SV,  &s3InfoRec.options))
      pixMuxShift = ((mode->Flags & V_DBLCLK && s3Bpp == 1)
		     || s3Bpp == 4) ? 1 : 0;
   else if (DAC_IS_IBMRGB528)
      pixMuxShift = (s3InfoRec.clock[mode->Clock] > 220000 && s3Bpp <= 2) ? 2 :
	             s3InfoRec.clock[mode->Clock] > 110000 ? 1 : 0 ;
   else if ((mode->Flags & V_DBLCLK)
	    && (DAC_IS_TI3026) 
	    && (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &s3InfoRec.clockOptions)))
      pixMuxShift =  (s3Bpp <= 2) ? 2 : 1;
   else if ((mode->Flags & V_DBLCLK) && DAC_IS_TI3030)
      pixMuxShift =  1;
   else if (S3_964_SERIES(s3ChipId) && DAC_IS_IBMRGB)
      pixMuxShift = mode->Flags & V_DBLCLK ? 1 : 0;
   else if (S3_964_SERIES(s3ChipId) && DAC_IS_TI3025)
      pixMuxShift =  mode->Flags & V_DBLCLK ? 1 : 0;
   else if (S3_964_SERIES(s3ChipId) && DAC_IS_BT485_SERIES)
      /* Stealth64 and Miro Crystal 20SV */
      pixMuxShift =  mode->Flags & V_DBLCLK ? 1 : 0;
   else if (S3_801_928_SERIES(s3ChipId) && DAC_IS_SC15025)
      pixMuxShift = -(s3Bpp>>1);  /* for 16/32 bpp */
   else if (S3_864_SERIES(s3ChipId) || S3_805_I_SERIES(s3ChipId))
	    /* && (DAC_IS_ATT498 || DAC_IS_STG1700) */
      pixMuxShift = -(s3Bpp>>1);  /* for 16/32 bpp */
   else if (S3_TRIOxx_SERIES(s3ChipId))
      pixMuxShift = -(s3Bpp == 2);
   else if (S3_x64_SERIES(s3ChipId)) /* XXXX Better to test the DAC type? */
      pixMuxShift = 0;
   else if ((S3_928_SERIES(s3ChipId) && 
	     (DAC_IS_TI3020 || DAC_IS_BT485_SERIES)) &&
            s3PixelMultiplexing) {
      if (s3Bpp == 4)      pixMuxShift = 0;  /* 32 bit */
      else if (s3Bpp == 2) pixMuxShift = 1;  /* 16 bit */
      else                 pixMuxShift = 2;  /*  8 bit */
   } else if (s3PixelMultiplexing)  
      pixMuxShift = 2; /* old default for   if (s3PixelMultiplexing) shifting */
   else 
      pixMuxShift = 0;

   if (!mode->CrtcHAdjusted) {
      if (s3Bpp == 3 && 
	  (   (S3_968_SERIES(s3ChipId) && DAC_IS_TI3026)
	   || (S3_968_SERIES(s3ChipId) && DAC_IS_TI3030))) {	 
	 mode->CrtcHTotal     = (mode->CrtcHTotal     * 3) / 4;
	 mode->CrtcHDisplay   = (mode->CrtcHDisplay   * 3) / 4;
	 mode->CrtcHSyncStart = (mode->CrtcHSyncStart * 3) / 4;
	 mode->CrtcHSyncEnd   = (mode->CrtcHSyncEnd   * 3) / 4;
	 mode->CrtcHSkew      = (mode->CrtcHSkew      * 3) / 4;
      }
      if (pixMuxShift > 0) {
	 /* now divide the horizontal timing parameters as required */
	 mode->CrtcHTotal     >>= pixMuxShift;
	 mode->CrtcHDisplay   >>= pixMuxShift;
	 mode->CrtcHSyncStart >>= pixMuxShift;
	 mode->CrtcHSyncEnd   >>= pixMuxShift;
	 mode->CrtcHSkew      >>= pixMuxShift;
      }
      else if (pixMuxShift < 0) {
	 /* now multiply the horizontal timing parameters as required */
	 mode->CrtcHTotal     <<= -pixMuxShift;
	 mode->CrtcHDisplay   <<= -pixMuxShift;
	 mode->CrtcHSyncStart <<= -pixMuxShift;
	 mode->CrtcHSyncEnd   <<= -pixMuxShift;
	 mode->CrtcHSkew      <<= -pixMuxShift;
      }
      mode->CrtcHAdjusted = TRUE;
   }

   /* 
    * do some sanity checks on the horizontal timing parameters 
    */
   { 
      Bool changed=FALSE;
      int oldCrtcHSyncStart, oldCrtcHSyncEnd, oldCrtcHTotal;
      int p24_fact = 4;

      oldCrtcHSyncStart = mode->CrtcHSyncStart;
      oldCrtcHSyncEnd   = mode->CrtcHSyncEnd;
      oldCrtcHTotal     = mode->CrtcHTotal;
      if (mode->CrtcHTotal > 4096) {  /*  CrtcHTotal/8  is a 9 bit value */
	 mode->CrtcHTotal = 4096;
	 changed = TRUE;
      }
      if (mode->CrtcHSyncEnd >= mode->CrtcHTotal) {
	 mode->CrtcHSyncEnd = mode->CrtcHTotal - 1;
	          changed = TRUE;
      }
      if (mode->CrtcHSyncStart >= mode->CrtcHSyncEnd) {
	 mode->CrtcHSyncStart = mode->CrtcHSyncEnd - 1;
         changed = TRUE;
      }
      if ((DAC_IS_TI3030 || DAC_IS_IBMRGB528) && s3Bpp==1) {
	 /* for 128bit bus we need multiple of 16 8bpp pixels... */
	 if (mode->CrtcHTotal & 0x0f) {
	    mode->CrtcHTotal = (mode->CrtcHTotal + 0x0f) & ~0x0f;
	    changed = TRUE;
	 }
      }
      if (s3Bpp == 3) {
	 /* for packed 24bpp CrtcHTotal must be multiple of 3*8... */
	 if ((mode->CrtcHTotal >> 3) % 3 != 0) {
	    mode->CrtcHTotal >>= 3;
	    mode->CrtcHTotal += 3 - mode->CrtcHTotal % 3;
	    mode->CrtcHTotal <<= 3;
	    changed = TRUE;
	    p24_fact = 3;
	 }
      }
      if (changed) {
	 ErrorF("%s %s: mode line has to be modified ...\n",
		XCONFIG_PROBED, s3InfoRec.name);
	 ErrorF("\t\tfrom   %4d %4d %4d %4d   %4d %4d %4d %4d\n"
		,mode->HDisplay, mode->HSyncStart, mode->HSyncEnd, mode->HTotal
		,mode->VDisplay, mode->VSyncStart, mode->VSyncEnd, mode->VTotal
		);
	 ErrorF("\t\tto     %4d %4d %4d %4d   %4d %4d %4d %4d\n"
		,((mode->CrtcHDisplay   << 8) >> (8-pixMuxShift)) * 4 / p24_fact
		,((mode->CrtcHSyncStart << 8) >> (8-pixMuxShift)) * 4 / p24_fact
		,((mode->CrtcHSyncEnd   << 8) >> (8-pixMuxShift)) * 4 / p24_fact
		,((mode->CrtcHTotal     << 8) >> (8-pixMuxShift)) * 4 / p24_fact
		,mode->VDisplay, mode->VSyncStart, mode->VSyncEnd, mode->VTotal
		);
      }
   }
   if (!vgaHWInit(mode, sizeof(vgaS3Rec)))
      return(FALSE);

   new->MiscOutReg |= 0x0C;		/* enable CR42 clock selection */
   new->Sequencer[0] = 0x03;		/* XXXX shouldn't need this */
   new->CRTC[19] = s3BppDisplayWidth >> 3;
   new->CRTC[23] = 0xE3;		/* XXXX shouldn't need this */
   new->Attribute[0x11] = currents3dac_border; /* The overscan colour AR11 gets */
					/* disabled anyway */

   if ((S3_TRIO64V_SERIES(s3ChipId) && (s3ChipRev <= 0x53) && (s3Bpp==1)) ^
       !!OFLG_ISSET(OPTION_TRIO64VP_BUG2, &s3InfoRec.options)) {
      /* set correct blanking for broken Trio64V+ to avoid bright left border:
	 blank signal needs to go off ~400 usec before video signal starts 
	 w/o border:  blank_shift = 0 */
      int blank_shift = 400 * s3InfoRec.clock[mode->Clock] / 1000000 / 8;
      new->CRTC[2]  = mode->CrtcHDisplay >> 3;
      new->CRTC[3] &= ~0x1f;
      new->CRTC[3] |=  ((mode->CrtcHTotal >> 3) - 2 - blank_shift) & 0x1f;
      new->CRTC[5] &= ~0x80;
      new->CRTC[5] |= (((mode->CrtcHTotal >> 3) - 2 - blank_shift) & 0x20) << 2;
      
      new->CRTC[21] =  (mode->CrtcVDisplay - 1) & 0xff; 
      new->CRTC[7] &= ~0x08;
      new->CRTC[7] |= ((mode->CrtcVDisplay - 1) & 0x100) >> 5;
      new->CRTC[9] &= ~0x20;
      new->CRTC[9] |= ((mode->CrtcVDisplay - 1) & 0x200) >> 4;

      new->CRTC[22] = (mode->CrtcVTotal - 2) & 0xFF;
   }

   vgaProtect(TRUE);

   if (vgaIOBase == 0x3B0)
      new->MiscOutReg &= 0xFE;
   else
      new->MiscOutReg |= 0x01;

   if (OFLG_ISSET(OPTION_SPEA_MERCURY, &s3InfoRec.options) && 
       S3_928_ONLY(s3ChipId)) {
      /*
       * Make sure that parallel option is already set correctly before
       * changing the clock doubler state.
       * XXXX maybe the !s3PixelMultiplexing bit is not required?
       */
      if (s3PixelMultiplexing) {
	 outb(vgaCRIndex, 0x5C);
	 outb(vgaCRReg, 0x20);
	 outb(0x3C7, 0x21);
	 /* set s3 reg53 to parallel addressing by or'ing 0x20		*/
	 outb(vgaCRIndex, 0x53);
	 tmp = inb(vgaCRReg);
	 outb(vgaCRReg, tmp | 0x20);  
	 outb(vgaCRIndex, 0x5C);
	 outb(vgaCRReg, 0x00);
      } else {
	 outb(vgaCRIndex, 0x5C);
	 outb(vgaCRReg, 0x20);
	 outb(0x3C7, 0x00); 
	 /* set s3 reg53 to non-parallel addressing by and'ing 0xDF	*/
	 outb(vgaCRIndex, 0x53);
	 tmp = inb(vgaCRReg);
	 outb(vgaCRReg, tmp & 0xDF);
	 outb(vgaCRIndex, 0x5C);
	 outb(vgaCRReg, 0x00);
      }
      if (s3Bpp == 4) 
         s3OutTi3026IndReg(TI_LATCH_CONTROL, ~1, 1);  /* 0x06 -> 0x07 */
      else
         s3OutTi3026IndReg(TI_LATCH_CONTROL, ~1, 0);  /* 0x06 */
   }

   if(    (DAC_IS_TI3026 || DAC_IS_TI3030) 
       && (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &s3InfoRec.clockOptions))){
      /*
       * for the boards with Ti3026 and external ICD2061A clock chip we
       * need to enable clock doubling, if necessary
       */
      if( mode->Flags & V_DBLCLK ) {
#ifdef EXTENDED_DEBUG
	 ErrorF("Putting Ti3026 into external double clock mode\n");
#endif
         s3OutTi3026IndReg(TI_INPUT_CLOCK_SELECT,0x00,0x08);
      }
      else {
#ifdef EXTENDED_DEBUG
	 ErrorF("Putting Ti3026 into external clock mode\n");
#endif
         s3OutTi3026IndReg(TI_INPUT_CLOCK_SELECT,0x00,0x00);
      }
      if (s3Bpp == 4) 
         s3OutTi3026IndReg(TI_LATCH_CONTROL, ~1, 1);  /* 0x06 -> 0x07 */
      else
         s3OutTi3026IndReg(TI_LATCH_CONTROL, ~1, 0);  /* 0x06 */
   }

   if (OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options) &&
       !OFLG_ISSET(OPTION_NOLINEAR_MODE, &s3InfoRec.options) &&
       s3Mmio928) {
     /*
       Set bit 7 of CRTC register 5C to map video memory with
       LAW31-26 = 011111 rather than 000000.  Note that this remaps
       all addresses seen by the 928, including the VGA Base Address.
     */
     int CR5C;
     outb(vgaCRIndex, 0x5C);
     CR5C = inb(vgaCRReg);
     outb(vgaCRIndex, 0x5C);
     outb(vgaCRReg, CR5C | 0x80);
     vgaBase = vgaBaseHigh;
   }

   if (OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options) &&
       !OFLG_ISSET(OPTION_NOLINEAR_MODE, &s3InfoRec.options) &&
       s3Mmio928) {
     int CR5C;
     outb(vgaCRIndex, 0x5C);
     CR5C = inb(vgaCRReg);
     outb(vgaCRIndex, 0x5C);
       switch(s3InfoRec.depth) {
       case 24:
       case 32:
	 outb(vgaCRReg, CR5C | 0xf0);
	 break;
       case 16:
       case 15:
	 outb(vgaCRReg, CR5C | 0x70);
	 break;
       default:
	 outb(vgaCRReg, CR5C | 0xa0);
       }
       vgaBase = vgaBaseHigh;
}

   /* Don't change the clock bits when using an external clock program */

   if (new->NoClock < 0) {
      tmp = inb(0x3CC);
      new->MiscOutReg = (new->MiscOutReg & 0xF3) | (tmp & 0x0C);
   } else {
      /* XXXX Should we really do something about the return value? */
      if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions))
	 (void) (s3ClockSelectFunc)(mode->SynthClock);
      else
         (void) (s3ClockSelectFunc)(mode->Clock);
     
      if ((mode->Flags & V_DBLCLK)
	 && (DAC_IS_TI3026 || DAC_IS_TI3030) 
	 && (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &s3InfoRec.clockOptions))){
	 if (s3Bpp <= 2)
	    Ti3026SetClock(mode->SynthClock / 2, 2, s3Bpp, TI_LOOP_CLOCK);
	 else 
	    Ti3026SetClock(mode->SynthClock, 2, s3Bpp, TI_LOOP_CLOCK);	    
	 s3OutTi3026IndReg(TI_MCLK_LCLK_CONTROL, ~0x20, 0x20);
      }
      if ((DAC_IS_TI3026 || DAC_IS_TI3030) && s3Bpp == 3) {
	 s3OutTi3026IndReg(TI_MCLK_LCLK_CONTROL, ~0x20, 0x20);
      }
   }



/* Initialize Ramdac */
     if(!(s3Ramdacs[s3RamdacType].DacInit)(mode)) {
	/* blah */
     }


   s3InitCursorFlag = TRUE;  /* turn on the cursor during the next load */

   outb(0x3C2, new->MiscOutReg);

   for (i = 1; i < 5; i++)
      outw(0x3C4, (new->Sequencer[i] << 8) | i);

   for (i = 0; i < 25; i++)
      outw(vgaCRIndex, (new->CRTC[i] << 8) | i);

   for (i = 0; i < 9; i++)
      outw(0x3CE, (new->Graphics[i] << 8) | i);

   i = inb(vgaIOBase + 0x0A);	/* reset flip-flop */
   for (i = 0; i < 16; i++) {
      outb(0x3C0, i);
      outb(0x3C0, new->Attribute[i]);
   }
   for (i = 16; i < 21; i++) {
      outb(0x3C0, i | 0x20);
      outb(0x3C0, new->Attribute[i]);
   }

   if (s3DisplayWidth == 2048)
      s3Port31 = 0x8f;
   else
      s3Port31 = 0x8d;

   outb(vgaCRIndex, 0x31);
   outb(vgaCRReg, s3Port31);
   outb(vgaCRIndex, 0x32);
   outb(vgaCRReg, 0x00);
   outb(vgaCRIndex, 0x33);
   if (OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options))
     /* Blank border comes earlier than display enable. */
     outb(vgaCRReg, 0x00);
   else if (!S3_TRIOxx_SERIES(s3ChipId))
      outb(vgaCRReg, 0x20);
   outb(vgaCRIndex, 0x34);
   outb(vgaCRReg, 0x10);		/* 1024 */
   outb(vgaCRIndex, 0x35);
   outb(vgaCRReg, 0x00);
   cebank();
#if 0  /* x64: set to 1 if PCI read bursts should be enabled
        * NOTE: there are known problems with PCI burst mode in SATURN
        * chipset rev. 2 so this is commented out, maybe a new XF86Config
        * option should be used
        */
   if (S3_x64_SERIES(s3ChipId)) {
      outb(vgaCRIndex, 0x3a);
      outb(vgaCRReg, 0xb5 & 0x7f);
   } else
#endif
      {
	 int pci_disc;
	 if (OFLG_ISSET(OPTION_NO_PCI_DISC, &s3InfoRec.options))
	    pci_disc = 0x00;
	 else
	    pci_disc = 0x80;

	 outb(vgaCRIndex, 0x66);  /* set CR66_7 before CR3A_7 */
	 tmp = inb(vgaCRReg) & 0x7f;
	 outb(vgaCRReg, tmp | pci_disc);
	 
	 outb(vgaCRIndex, 0x3a);
	 if (OFLG_ISSET(OPTION_SLOW_DRAM_REFRESH, &s3InfoRec.options))
	    outb(vgaCRReg, 0x37 | pci_disc);
	 else
	    outb(vgaCRReg, 0x35 | pci_disc);
      }

   outb(vgaCRIndex, 0x3b);
   outb(vgaCRReg, (new->CRTC[0] + new->CRTC[4] + 1) / 2);
   outb(vgaCRIndex, 0x3c);
   outb(vgaCRReg, new->CRTC[0]/2);	/* Interlace mode frame offset */

   /* x64: CR40 changed a lot for 864/964; wait and see if this still works */
   outb(vgaCRIndex, 0x40);
   if (S3_911_SERIES (s3ChipId)) {
      i = (inb(vgaCRReg) & 0xf2);
      s3Port40 = (i | 0x09);
      outb(vgaCRReg, s3Port40);
   } else {
      if (s3Localbus) {
	 i = (inb(vgaCRReg) & 0xf2);
	 if (OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options) ||
	     OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options))
	   /* Set no wait states on STB Pegasus. */
	   s3Port40 = (i | 0x01);
	 else s3Port40 = (i | 0x05);
	 outb(vgaCRReg, s3Port40);
      } else {
	 i = (inb(vgaCRReg) & 0xf6);
	 s3Port40 = (i | 0x01);
	 outb(vgaCRReg, s3Port40);
      }
   }

   outb(vgaCRIndex, 0x43);
   switch (s3InfoRec.depth) {
   case 24:
   case 32:
      if (S3_864_SERIES(s3ChipId))
	 outb(vgaCRReg, 0x08);  /* 0x88 can't be used for 864/964 */
      else if (S3_801_928_SERIES(s3ChipId) && DAC_IS_SC15025)
	 outb(vgaCRReg, 0x01);  /* ELSA Winner 1000 */
      else if (DAC_IS_BT485_SERIES && S3_928_SERIES(s3ChipId))
	 outb(vgaCRReg, 0x00);
      break;
   case 15:
   case 16:
      if (DAC_IS_ATT490 || DAC_IS_GENDAC || DAC_IS_SC1148x_SERIES /* JON */
		|| DAC_IS_SS2410 ) /*??? I'm not sure - but the 490 does it */  
	 outb(vgaCRReg, 0x80);
      else if (DAC_IS_TI3025)
	 outb(vgaCRReg, 0x10);
      else if (DAC_IS_TI3026 || DAC_IS_TI3030)
	 outb(vgaCRReg, 0x10);
      else if (DAC_IS_IBMRGB)
	 outb(vgaCRReg, 0x10);
      else if (S3_864_SERIES(s3ChipId))
	 outb(vgaCRReg, 0x08);  /* 0x88 can't be used for 864/964 */
      else if (S3_928_SERIES(s3ChipId)) {
	 if (DAC_IS_SC15025)
	    outb(vgaCRReg, 0x01);  /* ELSA Winner 1000 */
	 else if (DAC_IS_BT485_SERIES || DAC_IS_TI3020)
	    outb(vgaCRReg, 0x00);
	 else
	    outb(vgaCRReg, 0x09);  /* who uses this ? */
      }
      else if (DAC_IS_ATT498 && S3_805_I_SERIES(s3ChipId))
	 outb(vgaCRReg, 0x00);
      else
	 outb(vgaCRReg, 0x09);
      break;
   case 8:
   default:
      outb(vgaCRReg, 0x00); /* DON'T enable XOR addresses */
      break;
   }

   outb(vgaCRIndex, 0x44);
   outb(vgaCRReg, 0x00);

   outb(vgaCRIndex, 0x45);
   i = inb(vgaCRReg) & 0xf2;
   /* hi/true cursor color enable */
   switch (s3InfoRec.bitsPerPixel) {
   case 16:
      if (!S3_x64_SERIES(s3ChipId) && !S3_805_I_SERIES(s3ChipId) &&
          !DAC_IS_TI3020)
	 i = i | 0x04;
      break;
   case 32:
      if (S3_x64_SERIES(s3ChipId) || S3_805_I_SERIES(s3ChipId) ||
	  DAC_IS_TI3020)
	 i = i | 0x04; /* for 16bit RAMDAC, 0x0c for 8bit RAMDAC */
      else
	 i = i | 0x08;
      break;
   }
   outb(vgaCRReg, i);

   if (S3_801_928_SERIES(s3ChipId) || S3_964_SERIES(s3ChipId)) {

      outb(vgaCRIndex, 0x50);
      i = inb(vgaCRReg);
      i &= ~0xf1;
      switch (s3InfoRec.bitsPerPixel) {
	case 8:
           break;
	case 16:
	   i |= 0x10;
	   break;
	case 24:
	   i |= 0x20;
	   break;
	case 32:
	   i |= 0x30;
	   break;
      }
      switch (s3DisplayWidth) { 
	case 640:
	   i |= 0x40;
	   break;
	case 800:
	   i |= 0x80;
	   break;
	case 1152:
	   i |= 0x01;
	   break;
	case 1280:
	   i |= 0xc0;
	   break;
	case 1600:
	   i |= 0x81;
	   break;
	default: /* 1024 and 2048 */
	   ;
      }
      outb(vgaCRReg, i);

      outb(vgaCRIndex, 0x51);
      s3Port51 = (inb(vgaCRReg) & 0xC0) | ((s3BppDisplayWidth >> 7) & 0x30);

      if (OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options) ||
	  OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options)) {
	if (s3PixelMultiplexing)
	  /* In Pixel Multiplexing mode, disable split transfers. */
	  s3Port51 |= 0x40;
	else
	  /* In VGA mode, enable split transfers. */
	  s3Port51 &= ~0x40;
      }

      outb(vgaCRReg, s3Port51);

      outb(vgaCRIndex, 0x58);
      outb(vgaCRReg, s3SAM256);

#ifdef DEBUG
      ErrorF("Writing CR59 0x%02x, CR5A 0x%02x\n", s3Port59, s3Port5A);
#endif

      outb(vgaCRIndex, 0x59);
      outb(vgaCRReg, s3Port59);
      outb(vgaCRIndex, 0x5A);
      outb(vgaCRReg, s3Port5A);
      
      outb(vgaCRIndex, 0x53);
      tmp = inb(vgaCRReg) & ~0x18;
      if (s3Mmio928)
	 tmp |= 0x10;
      if (s3NewMmio) {
	 if (s3InfoRec.MemBase != 0) {
	    s3Port59 = (s3InfoRec.MemBase >> 24) & 0xfc;
	    s3Port5A = 0;
	    outb(vgaCRIndex, 0x59);
	    outb(vgaCRReg, s3Port59);
	    outb(vgaCRIndex, 0x5a);
	    outb(vgaCRReg, s3Port5A);
	    outb(vgaCRIndex, 0x53);
	 }
	 tmp |= 0x18;
      }

      /*
       * Now the DRAM interleaving bit for the 801/805 chips
       * Note, we don't touch this bit for 928 chips because they use it
       * for pixel multiplexing control.
       */
      if (S3_801_SERIES(s3ChipId)) {
	 if (S3_805_I_SERIES(s3ChipId) && s3InfoRec.videoRam == 2048)
	    tmp |= 0x20;
	 else
	    tmp &= ~0x20;
      }
      outb(vgaCRReg, tmp);

      if (s3NewMmio) {      
	 outb (vgaCRIndex, 0x58);
	 outb (vgaCRReg, (s3LinApOpt & ~0x04) | s3SAM256);  /* window size for linear mode */
      }

      n = 255;
      outb(vgaCRIndex, 0x54);
      if (S3_x64_SERIES(s3ChipId) || S3_805_I_SERIES(s3ChipId)) {
	 int clock2,mclk;
	 clock2 = s3InfoRec.clock[mode->Clock] * s3Bpp;
	 if (s3InfoRec.s3MClk > 0) 
	    mclk = s3InfoRec.s3MClk;
	 else if (S3_805_I_SERIES(s3ChipId))
	    mclk = 50000;  /* 50 MHz, guess for 805i limit */
	 else
	    mclk = 60000;  /* 60 MHz, limit for 864 */
	 if (s3InfoRec.videoRam < 2048 || S3_TRIO32_SERIES(s3ChipId))
	    clock2 *= 2;
	 m = (int)((mclk/1000.0*.72+16.867)*89.736/(clock2/1000.0+39)-21.1543);
	 if (s3InfoRec.videoRam < 2048 || S3_TRIO32_SERIES(s3ChipId))
	    m /= 2;
	 m -= s3InfoRec.s3Madjust;
	 if (m > 31) m = 31;
	 else if (m < 0) {
	    m = 0;
	    n = 16;
	 }
      }
      else if (s3InfoRec.videoRam == 512 || mode->HDisplay > 1200) /* XXXX */
	 m = 0;
      else if (s3InfoRec.videoRam == 1024)
         m = 2;
      else
	 m = 20;
      if (OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options))
 	s3Port54 = 0x7F;
      else if (OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options))
	s3Port54 = 0;
      else s3Port54 = m << 3;
      outb(vgaCRReg, s3Port54);
      
      n -= s3InfoRec.s3Nadjust;
      if (n < 0) n = 0;
      else if (n > 255) n = 255;
      outb(vgaCRIndex, 0x60);
      outb(vgaCRReg, n);

      if(!OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options)) {
	outb(vgaCRIndex, 0x55);
	tmp = inb(vgaCRReg) & 0x08;       /* save the external serial bit  */
	outb(vgaCRReg, tmp | 0x40);	/* remove mysterious dot at 60Hz */
      }

      /* This shouldn't be needed -- they should be set by vgaHWInit() */
      if (!mode->CrtcVAdjusted) {
	 mode->CrtcVTotal /= interlacedived;
	 mode->CrtcVDisplay /= interlacedived;
	 mode->CrtcVSyncStart /= interlacedived;
	 mode->CrtcVSyncEnd /= interlacedived;
	 mode->CrtcVAdjusted = TRUE;
      }

      if ((S3_TRIO64V_SERIES(s3ChipId) && (s3ChipRev <= 0x53) && (s3Bpp==1)) ^
	  !!OFLG_ISSET(OPTION_TRIO64VP_BUG2, &s3InfoRec.options))
	 i = (((mode->CrtcVTotal - 2) & 0x400) >> 10)  |
	     (((mode->CrtcVDisplay - 1) & 0x400) >> 9) |
	     (((mode->CrtcVDisplay - 1) & 0x400) >> 8) |
	     (((mode->CrtcVSyncStart) & 0x400) >> 6)   | 0x40;
      else
	 i = (((mode->CrtcVTotal - 2) & 0x400) >> 10)  |
	     (((mode->CrtcVDisplay - 1) & 0x400) >> 9) |
	     (((mode->CrtcVSyncStart) & 0x400) >> 8)   |
	     (((mode->CrtcVSyncStart) & 0x400) >> 6)   | 0x40;
	  
      outb(vgaCRIndex, 0x5e);
      outb(vgaCRReg, i);

      if ((S3_TRIO64V_SERIES(s3ChipId) && (s3ChipRev <= 0x53) && (s3Bpp==1)) ^
	  !!OFLG_ISSET(OPTION_TRIO64VP_BUG2, &s3InfoRec.options)) {
	 i = ((((mode->CrtcHTotal >> 3) - 5) & 0x100) >> 8) |
	     ((((mode->CrtcHDisplay >> 3) - 1) & 0x100) >> 7) |
	     (((mode->CrtcHDisplay >> 3) & 0x100) >> 6) |
	     ((mode->CrtcHSyncStart & 0x800) >> 7);
	 if ((mode->CrtcHTotal >> 3) - (mode->CrtcHDisplay >> 3) > 64)
	    i |= 0x08;   /* add another 64 DCLKs to blank pulse width */
      }
      else {
	 i = ((((mode->CrtcHTotal >> 3) - 5) & 0x100) >> 8) |
	     ((((mode->CrtcHDisplay >> 3) - 1) & 0x100) >> 7) |
	     ((((mode->CrtcHSyncStart >> 3) - 1) & 0x100) >> 6) |
	     ((mode->CrtcHSyncStart & 0x800) >> 7);
	 if ((mode->CrtcHSyncEnd >> 3) - (mode->CrtcHSyncStart >> 3) > 64)
	    i |= 0x08;   /* add another 64 DCLKs to blank pulse width */
      }

      if ((mode->CrtcHSyncEnd >> 3) - (mode->CrtcHSyncStart >> 3) > 32)
	 i |= 0x20;   /* add another 32 DCLKs to hsync pulse width */


      outb(vgaCRIndex, 0x3b);
      if (DAC_IS_IBMRGB528) {
	 if (OFLG_ISSET(OPTION_MIRO_80SV,  &s3InfoRec.options)) {
	    itmp = (( (new->CRTC[0] + ((i&0x01)<<8)) * 3
		     + new->CRTC[4] + ((i&0x10)<<4) + 1) / 4) & ~1;
	    if (itmp-(new->CRTC[4] + ((i&0x10)<<4)) < 4)
	       itmp = (  new->CRTC[0] + ((i&0x01)<<8) - 2) & ~1;
	 }
	 else if (s3Bpp==1)
	    itmp = ((new->CRTC[4] + ((i&0x10)<<4) + 2) + 1) & ~1;
	 else
	    itmp = ((new->CRTC[4] + ((i&0x10)<<4) + 4) + 1) & ~1;
      }
      else {
	 itmp = (  new->CRTC[0] + ((i&0x01)<<8)
		 + new->CRTC[4] + ((i&0x10)<<4) + 1) / 2;
	 if (itmp-(new->CRTC[4] + ((i&0x10)<<4)) < 4)
	    if (new->CRTC[4] + ((i&0x10)<<4) + 4 <= new->CRTC[0]+ ((i&0x01)<<8))
	       itmp = new->CRTC[4] + ((i&0x10)<<4) + 4;
	    else
	       itmp = new->CRTC[0]+ ((i&0x01)<<8) + 1;
      }
      outb(vgaCRReg, itmp & 0xff);
      i |= (itmp&0x100) >> 2;
      outb(vgaCRIndex, 0x3c);
      outb(vgaCRReg, (new->CRTC[0] + ((i&0x01)<<8)) /2);	/* Interlace mode frame offset */

      outb(vgaCRIndex, 0x5d);
      tmp = inb(vgaCRReg);
      outb(vgaCRReg, (tmp & 0x80) | i);

      if (s3InfoRec.videoRam > 1024 && S3_x64_SERIES(s3ChipId)) 
	 i = mode->HDisplay * s3Bpp / 8 + 1;
      else
	 i = mode->HDisplay * s3Bpp / 4 + 1; /* XXX should be checked for 801/805 */
      
      outb(vgaCRIndex, 0x61);
      tmp = inb(vgaCRReg);
      outb(vgaCRReg, 0x80 | (tmp & 0x60) | (i >> 8));
      outb(vgaCRIndex, 0x62);
      outb(vgaCRReg, i & 0xff);
   }

   if ((mode->Flags & V_INTERLACE) != 0) {
      outb(vgaCRIndex, 0x42);
      tmp = inb(vgaCRReg);
      outb(vgaCRReg, 0x20 | tmp);
   }
   else {
      outb(vgaCRIndex, 0x42);
      tmp = inb(vgaCRReg);
      outb(vgaCRReg, ~0x20 & tmp);
   }

   if (mode->Private) {
      if (mode->Private[0] & (1 << S3_INVERT_VCLK)) {
	outb(vgaCRIndex, 0x67);
	tmp = inb(vgaCRReg) & 0xfe;
	if (mode->Private[S3_INVERT_VCLK])
	  tmp |= 1;
	outb(vgaCRReg, tmp);
      }
      if (mode->Private[0] & (1 << S3_BLANK_DELAY)) {
	 outb(vgaCRIndex, 0x6d);
	 outb(vgaCRReg, mode->Private[S3_BLANK_DELAY]);
      }
      if (mode->Private[0] & (1 << S3_EARLY_SC)) {
	 outb(vgaCRIndex, 0x65);
	 tmp = inb(vgaCRReg) & ~2;
	 if (mode->Private[S3_EARLY_SC])
	    tmp |= 2;
	 outb(vgaCRReg, tmp);
      }
   }


   if (OFLG_ISSET(OPTION_FAST_VRAM, &s3InfoRec.options)) {
      outb(vgaCRIndex, 0x39);
      outb(vgaCRReg, 0xa5);
      outb(vgaCRIndex, 0x68);
      tmp = inb(vgaCRReg);
      /* -RAS low timing 3.5 MCLKs, -RAS precharge timing 2.5 MCLKs */
      outb(vgaCRReg, tmp | 0xf0);
   }

   if (OFLG_ISSET(OPTION_SLOW_VRAM, &s3InfoRec.options)) {
      /* 
       * some Diamond Stealth 64 VRAM cards have a problem with VRAM timing,
       * increase -RAS low timing from 3.5 MCLKs to 4.5 MCLKs 
       */ 
      outb(vgaCRIndex, 0x39);
      outb(vgaCRReg, 0xa5);
      outb(vgaCRIndex, 0x68);
      tmp = inb(vgaCRReg);
      if ((tmp & 0x30) == 0x30) 		/* 3.5 MCLKs */
	 outb(vgaCRReg, tmp & 0xef);		/* 4.5 MCLKs */
   }

   if (OFLG_ISSET(OPTION_SLOW_DRAM, &s3InfoRec.options)) {
      /* 
       * fixes some pixel errors for a SPEA Trio64V+ card
       * increase -RAS precharge timing from 2.5 MCLKs to 3.5 MCLKs 
       */ 
      outb(vgaCRIndex, 0x39);
      outb(vgaCRReg, 0xa5);
      outb(vgaCRIndex, 0x68);
      tmp = inb(vgaCRReg);
      outb(vgaCRReg, tmp & 0xf7);		/* 3.5 MCLKs */
   }

   if (OFLG_ISSET(OPTION_SLOW_EDODRAM, &s3InfoRec.options)) {
      /* 
       * fixes some pixel errors for a SPEA Trio64V+ card
       * increase from 1-cycle to 2-cycle EDO mode
       */ 
      outb(vgaCRIndex, 0x39);
      outb(vgaCRReg, 0xa5);
      outb(vgaCRIndex, 0x36);
      tmp = inb(vgaCRReg);
      if ((tmp & 0x0c) == 0x00) 		/* 1-cycle EDO */
	 outb(vgaCRReg, tmp | 0x08);		/* 2-cycle EDO */
   }


   if (S3_964_SERIES(s3ChipId) && DAC_IS_BT485_SERIES) {
      if (OFLG_ISSET(OPTION_S3_964_BT485_VCLK, &s3InfoRec.options)) {
	 /*
	  * This is the design alert from S3 with Bt485A and Vision 964. 
	  */
	 int i1, last, tmp1, cr55, cr67;
	 int port, bit;

#define VerticalRetraceWait() \
	 { \
	      while ((inb(vgaIOBase + 0x0A) & 0x08) == 0x00) ; \
	      while ((inb(vgaIOBase + 0x0A) & 0x08) == 0x08) ; \
	      while ((inb(vgaIOBase + 0x0A) & 0x08) == 0x00) ; \
	}

	 if (OFLG_ISSET(OPTION_DIAMOND, &s3InfoRec.options)) {
	    port = 0x3c2;
	    bit  = 0x10;
	 }
	 else {  /* MIRO 20SV Rev.2 */
	    port = 0x3c8;
	    bit  = 0x04;
	 }

	 if (port == 0x3c8) {
	    outb(vgaCRIndex, 0x55);
	    cr55 = inb(vgaCRReg);
	    outb(vgaCRReg, cr55 | 0x04); /* enable rad of general input */
	 }
	 outb(vgaCRIndex, 0x67);
	 cr67 = inb(vgaCRReg);

	 for(last=i1=30; --i1;) {
	    VerticalRetraceWait();
	    VerticalRetraceWait();
	    if ((inb(port) & bit) == 0) { /* if GD2 is low then */
	       last = i1;
	       outb(vgaCRIndex, 0x67);
	       tmp1 = inb(vgaCRReg);
	       outb(vgaCRReg, tmp1 ^ 0x01); /* clock should be inverted */
#if 0
	       ErrorF("inverted VCLK %d  to 0x%02x\n",i1,tmp1 ^ 0x01);
#endif
	    }
	    if (last-i1 > 4) break;
	 }
	 if (!i1) {  /* didn't get stable input, restore original CR67 value */
	    outb(vgaCRIndex, 0x67);
	    outb(vgaCRReg, cr67);
	 }
	 if (port == 0x3c8) {
	    outb(vgaCRIndex, 0x55);
	    outb(vgaCRReg, cr55); 
	 }
#if 0
	 outb(vgaCRIndex, 0x67);
	 tmp1 = inb(vgaCRReg);
	 /* if ((cr67 ^ tmp1) & 0x01)  */ 
	 ErrorF("VCLK has been inverted %d times from 0x%02x to 0x%02x now\n",i1,cr67,tmp1);
#endif
      }
   }

   if (OFLG_ISSET(OPTION_MIRO_80SV,  &s3InfoRec.options)) {
      outb(vgaCRIndex, 0x42);
      tmp = inb(vgaCRReg) & 0xf0;
      outb(vgaCRReg, tmp | 0x06);
   }      

   if (OFLG_ISSET(OPTION_ELSA_W2000PRO_X8,  &s3InfoRec.options)) {
      /* check LCLK/SCLK phase */
      unsigned char cr5c, cr42;

      outb(vgaCRIndex, 0x42);
      cr42 = inb(vgaCRReg);

      if (inb(0x3cc) & 0x40)   /* hsync polarity */
	 cr42 &= 0xfb;
      else
	 cr42 |= 0x04;
      outb(vgaCRReg, cr42);

      outb(vgaCRIndex, 0x5c);
      cr5c = inb(vgaCRReg);
      outb(vgaCRReg, cr5c | 0xa0);  /* set GD7 & GD5 */

      usleep(100000);   /* wait at least 2 VSYNCs to latch clock phase */

      if (inb(0x3c2) & 0x10)   /* query SENSE */
	 cr42 &= 0xf7;
      else
	 cr42 |= 0x08;

      outb(vgaCRIndex, 0x42);
      outb(vgaCRReg, cr42);

      outb(vgaCRIndex, 0x5c);
      outb(vgaCRReg, cr5c & 0x7f | 0x20);
   }

   if (OFLG_ISSET(CLOCK_OPTION_ICS2595, &s3InfoRec.clockOptions))
         (void) (s3ClockSelectFunc)(mode->SynthClock);
         /* fixes the ICS2595 initialisation problems */

   s3AdjustFrame(s3InfoRec.frameX0, s3InfoRec.frameY0);

   if ( S3_TRIO64V2_SERIES(s3ChipId) ) {
      /* disable DAC power saving to avoid bright left edge */
      outb(0x3d4,0x86);
      outb(0x3d5,0x80);
      /* disable the stream display fetch length control */
      outb(0x3d4,0x90);
      outb(0x3d5,0x00);
   }     

#ifdef REG_DEBUG
   for (i=0; i<10; i++) {
	 outb(vgaCRIndex, i);
	 tmp = inb(vgaCRReg);
	 ErrorF("CR%X = 0x%02x\n", i, tmp);
   }
   for (i=0x10; i<0x19; i++) {
	 outb(vgaCRIndex, i);
	 tmp = inb(vgaCRReg);
	 ErrorF("CR%X = 0x%02x\n", i, tmp);
   }
   outb(vgaCRIndex, 0x3b);
   tmp = inb(vgaCRReg);
   ErrorF("CR%X = 0x%02x\n", 0x3b, tmp);
   outb(vgaCRIndex, 0x5d);
   tmp = inb(vgaCRReg);
   ErrorF("CR%X = 0x%02x\n", 0x5d, tmp);
   outb(vgaCRIndex, 0x5e);
   tmp = inb(vgaCRReg);
   ErrorF("CR%X = 0x%02x\n", 0x5e, tmp);
   for (i=0; i<0x40; i++) {
	 tmp = s3InTiIndReg(i);
	 ErrorF("TI%X = 0x%02x\n", i, tmp);
   }
#endif
   vgaProtect(FALSE);

   if (s3DisplayWidth == 1024) /* this is unclear Jon */
      outw(ADVFUNC_CNTL, 0x0007);
    else 
      outw(ADVFUNC_CNTL, 0x0003);

 /*
  * Blank the screen temporarily to display color 0 by turning the display of
  * all planes off
  */
   outb(DAC_MASK, 0x00);

 /* Reset the 8514/A, and disable all interrupts. */
   outw(SUBSYS_CNTL, GPCTRL_RESET | CHPTEST_NORMAL);
   outw(SUBSYS_CNTL, GPCTRL_ENAB | CHPTEST_NORMAL);

   i = inw(SUBSYS_STAT);

   outw(MULTIFUNC_CNTL, MEM_CNTL | VRTCFG_4 | HORCFG_8);

   outb(DAC_MASK, 0xff);

   LOCK_SYS_REGS;

#ifdef PC98
   crtswitch(1);
#endif
   return TRUE;
}

/* InitLUT() */

/*
 * Loads the Look-Up Table with all black. Assumes 8-bit board is in use.
 */
static void
InitLUT()
{
   short i;

   outb(DAC_R_INDEX, 0);
   for (i = 0; i < 256; i++) {
      oldlut[i].r = inb(DAC_DATA);
      oldlut[i].g = inb(DAC_DATA);
      oldlut[i].b = inb(DAC_DATA);
   }
      
   outb(DAC_W_INDEX, 0);

   /* Load all 256 LUT entries */
   for (i = 0; i < 256; i++) {
      outb(DAC_DATA, 0);
      outb(DAC_DATA, 0);
      outb(DAC_DATA, 0);
   }

   if (s3InfoRec.bitsPerPixel > 8 &&
       (DAC_IS_SC15025 || DAC_IS_TI3020_SERIES || DAC_IS_TI3026 
	|| DAC_IS_TI3030 || DAC_IS_IBMRGB)) {
      int r,g,b;
      int mr,mg,mb;
      int nr=5, ng=5, nb=5;
      extern unsigned char xf86rGammaMap[], xf86gGammaMap[], xf86bGammaMap[];
      extern LUTENTRY currents3dac[];

      if (!LUTInited) {
	 if (s3Weight == RGB32_888 || DAC_IS_TI3020_SERIES || DAC_IS_TI3026 
	     || DAC_IS_TI3030 || DAC_IS_IBMRGB) {
	    for(i=0; i<256; i++) {
	       currents3dac[i].r = xf86rGammaMap[i];
	       currents3dac[i].g = xf86gGammaMap[i];
	       currents3dac[i].b = xf86bGammaMap[i];
	    }
	 }
	 else {
	    if (s3Weight == RGB16_565) ng = 6;
	    mr = (1<<nr)-1;
	    mg = (1<<ng)-1;
	    mb = (1<<nb)-1;
	    
	    for(i=0; i<256; i++) {
	       r = (i >> (6-nr)) & mr;
	       g = (i >> (6-ng)) & mg;
	       b = (i >> (6-nb)) & mb;
	       currents3dac[i].r = xf86rGammaMap[(r*255+mr/2)/mr];
	       currents3dac[i].g = xf86gGammaMap[(g*255+mg/2)/mg];
	       currents3dac[i].b = xf86bGammaMap[(b*255+mb/2)/mb];
	    }
	 }
      }

      i = xf86getdaccomm();
      xf86setdaccomm(i | 0x08);
      
      outb(DAC_W_INDEX, 0);
      for(i=0; i<256; i++) {
	 outb(DAC_DATA, currents3dac[i].r);
	 outb(DAC_DATA, currents3dac[i].g);
	 outb(DAC_DATA, currents3dac[i].b);
      }
   }
   LUTInited = TRUE;
}

/*
 * s3InitEnvironment()
 * 
 * Initializes the 8514/A's drawing environment and clears the display.
 */
void
s3InitEnvironment()
{
 /* Current mixes, src, foreground active */

   WaitQueue(6);
   outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
   outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);

 /* Clipping rectangle to full drawable space */
   outw(MULTIFUNC_CNTL, SCISSORS_T | 0x000);
   outw(MULTIFUNC_CNTL, SCISSORS_L | 0x000);
   outw(MULTIFUNC_CNTL, SCISSORS_R | (s3DisplayWidth-1));
   outw(MULTIFUNC_CNTL, SCISSORS_B | s3ScissB);

 /* Enable writes to all planes and reset color compare */
   WaitQueue16_32(3,5);

   outw32(WRT_MASK, ~0);
   outw32(RD_MASK, 0);
   outw(MULTIFUNC_CNTL, PIX_CNTL | 0x0000);

 /*
  * Clear the display.  Need to set the color, origin, and size. Then draw.
  */
   WaitQueue16_32(6,7);

   if (xf86FlipPixels && s3Bpp == 1)
      outw32(FRGD_COLOR, 1);
   else
      outw32(FRGD_COLOR, 0);

   outw(CUR_X, 0);
   outw(CUR_Y, 0);
   outw(MAJ_AXIS_PCNT, s3InfoRec.virtualX - 1);
   outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | s3ScissB);
   outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW | PLANAR | WRTDATA);

   WaitQueue16_32(4,6);

 /* Reset current draw position */
   outw(CUR_X, 0);
   outw(CUR_Y, 0);

 /* Reset current colors, foreground is all on, background is 0. */
   outw32(FRGD_COLOR, ~0);
   outw32(BKGD_COLOR,  0);

 /* Load the LUT */
   InitLUT();

 /* fix a bug in early Trio64 chips (with Trio32 labels) */
   outb(DAC_MASK, 0xff); 
}

void
s3Unlock()
{
   unsigned char tmp;

   xf86EnableIOPorts(s3InfoRec.scrnIndex);

 /* unlock */
   outb(vgaCRIndex, 0x38);
   outb(vgaCRReg, 0x48);
   outb(vgaCRIndex, 0x39);
   outb(vgaCRReg, 0xa5);

   outb(vgaCRIndex, 0x35);		/* select segment 0 */
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, tmp & 0xf0);
   cebank();

   outb(vgaCRIndex, 0x11);		/* allow writting to CR0-7 */
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, tmp & 0x7f);

}
