/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/s3init.c,v 3.15.2.8 1997/05/24 14:31:32 dawes Exp $ */
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
/* $XConsortium: s3init.c /main/9 1996/10/27 18:07:06 kaleb $ */


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

#include "s3v.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

/* void s3InitSTREAMS( int, int ); */

#define VerticalRetraceWait() \
{ \
   outb(vgaCRIndex, 0x17); \
   if ( inb(vgaCRReg) & 0x80 ) { \
       while ((inb(vgaIOBase + 0x0A) & 0x08) == 0x00) ; \
       while ((inb(vgaIOBase + 0x0A) & 0x08) == 0x08) ; \
       while ((inb(vgaIOBase + 0x0A) & 0x08) == 0x00) ; \
       }\
}

typedef struct {
   vgaHWRec std;                /* good old IBM VGA */
   unsigned char Trio[14];      /* Trio32/64 ext. sequenzer (PLL) registers */
   unsigned char s3reg[11];     /* Video Atribute (CR30-34, CR38-3C) */
   unsigned char s3sysreg[46];  /* Video Atribute (CR40-6D)*/
   unsigned char ColorStack[8]; /* S3 hw cursor color stack CR4A/CR4B */
   unsigned int  fifo[2];     /* MM8200 & MM8202 for streams FIFO control */
}
vgaS3Rec, *vgaS3Ptr;

extern int   vgaIOBase; /* These defaults are overriden in s3Probe() */
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
extern Bool (*s3ClockSelectFunc) ();
extern int s3DisplayWidth;
extern Bool s3Localbus;
extern Bool s3NewMmio;
extern Bool s3PixelMultiplexing;
extern pointer vgaBase;
extern pointer vgaBaseLow;
extern pointer vgaBaseHigh;
int currents3dac_border = 0xff;

#ifdef PC98
extern void crtswitch(short);
#endif


#define new ((vgaHWPtr)vgaNewVideoState)

unsigned short s3ChipId = 0;
unsigned int  s3ChipRev = 0;

#define	cebank() do {							\
		unsigned char tmp;					\
		outb(vgaCRIndex, 0x51);					\
		tmp = inb(vgaCRReg);					\
		outb(vgaCRReg, (tmp & 0xf3));				\
} while (1 == 0)

/*
 * Registers to save/restore in the 0x50 - 0x5f control range
 */

static short reg50_mask = 0x673b;

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

   outb(vgaCRIndex, 0x39);
   outb(vgaCRReg, 0xa5);

   if (s3MmioMem != NULL ) {
      WaitIdle();
      VerticalRetraceWait();
      ((mmtr)s3MmioMem)->memport_regs.regs.fifo_control = oldS3->fifo[0];
      WaitIdle();                  /* Don't ask... */
      ((mmtr)s3MmioMem)->memport_regs.regs.streams_timeout = oldS3->fifo[1];
   }

   #if 1			   
   			/* This seems to help, but maybe a wait for */
			/* VSYNC would be better than WaitIdleEmpty(). */
   				/* STREAMS disable code, for 24 & 32 Bpp */
   if ( ( s3InfoRec.bitsPerPixel == 32 || 
          s3InfoRec.bitsPerPixel == 24 ) &&
          s3MmioMem != NULL ) {
	  			/* temp - KJB */
       ((mmtr)s3MmioMem)->streams_regs.regs.prim_fbaddr0 = 0;
	      /*  ((y * s3DisplayWidth + (x & ~3)) * s3Bpp) */ /* & ~3 */;
	      			/* end temp */
       
       outb(vgaCRIndex, 0x67);
       tmp = inb(vgaCRReg);
       /* WaitIdleEmpty(); */
 				/* Disable STREAMS processor */
       outb( vgaCRReg, tmp & ~0x0C );
   }	
   #endif

   vgaProtect(TRUE);

   if (s3NewMmio)	{
      outb (vgaCRIndex, 0x58);
      outb (vgaCRReg, s3SAM256); /* disable linear mode */
   } /* end BL */


   WaitQueue(8);
   outb(vgaCRIndex, 0x35);
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, (tmp & 0xf0));
   cebank();

   if (s3NewMmio) {
      outb(vgaCRIndex, 0x53);
      outb(vgaCRReg, 0x00);
   }

   /* Restore S3 Trio32/64 ext. sequenzer (PLL) registers */
   if (DAC_IS_TRIO)
   {
      outb(0x3c2, oldS3->Trio[0]);
      outb(0x3c4, 0x08); outb(0x3c5, 0x06);

      outb(0x3c4, 0x09); outb(0x3c5, oldS3->Trio[2]);
      outb(0x3c4, 0x0a); outb(0x3c5, oldS3->Trio[3]);
      outb(0x3c4, 0x0b); outb(0x3c5, oldS3->Trio[4]);
      outb(0x3c4, 0x0d); outb(0x3c5, oldS3->Trio[5]);

      outb(0x3c4, 0x10); outb(0x3c5, oldS3->Trio[8]);
      outb(0x3c4, 0x11); outb(0x3c5, oldS3->Trio[9]);
      outb(0x3c4, 0x12); outb(0x3c5, oldS3->Trio[10]);
      outb(0x3c4, 0x13); outb(0x3c5, oldS3->Trio[11]);
      outb(0x3c4, 0x1a); outb(0x3c5, oldS3->Trio[12]);
      outb(0x3c4, 0x1b); outb(0x3c5, oldS3->Trio[13]);
      outb(0x3c4, 0x15);
      tmp = inb(0x3c5) & ~0x21;
      outb(0x3c5, tmp | 0x03);
      outb(0x3c5, tmp | 0x23);
      outb(0x3c5, tmp | 0x03);

      outb(0x3c4, 0x15); outb(0x3c5, oldS3->Trio[6]);
      outb(0x3c4, 0x18); outb(0x3c5, oldS3->Trio[7]);

      outb(0x3c4, 0x08); outb(0x3c5, oldS3->Trio[1]);

   }



   /* restore s3 special bits */

   /* restore 801 specific registers */

   for (i = 32; i < 46; i++) {
      outb(vgaCRIndex, 0x40 + i);
      outb(vgaCRReg, oldS3->s3sysreg[i]);

   }
   for (i = 0; i < 16; i++) {
      if (!((1 << i) & reg50_mask))
	 continue;
      outb(vgaCRIndex, 0x50 + i);
      outb(vgaCRReg, oldS3->s3sysreg[i + 16]);
   }
   for (i = 0; i < 5; i++) {
      outb(vgaCRIndex, 0x30 + i);
      outb(vgaCRReg, oldS3->s3reg[i]);
      outb(vgaCRIndex, 0x38 + i);
      outb(vgaCRReg, oldS3->s3reg[5 + i]);
   }
   outb(vgaCRIndex, 0x36);
   outb(vgaCRReg, oldS3->s3reg[10]);


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
   unsigned char tmp, tmp2;
   unsigned int itmp;
   extern Bool s3DAC8Bit;
   int pixMuxShift = 0;

   s3HDisplay = mode->HDisplay;

   outb(vgaCRIndex, 0x39);
   outb(vgaCRReg, 0xa5);

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

      outb(vgaCRIndex, 0x58);
      s3SAM256 = inb(vgaCRReg) & 0x80;
      if (S3_ViRGE_VX_SERIES(s3ChipId))
	 s3SAM256 |= 0x40;

      if (OFLG_ISSET(OPTION_EARLY_RAS_PRECHARGE, &s3InfoRec.options))
	 s3SAM256 |= 0x80;
      if (OFLG_ISSET(OPTION_LATE_RAS_PRECHARGE, &s3InfoRec.options))
	 s3SAM256 &= 0x7f;

      /* Save S3 Trio32/64 ext. sequenzer (PLL) registers */
      if (DAC_IS_TRIO) {
	 oldS3->Trio[0] = inb(0x3cc);

	 outb(0x3c4, 0x08); oldS3->Trio[1] = inb(0x3c5);
	 outb(0x3c5, 0x06);

	 outb(0x3c4, 0x09); oldS3->Trio[2]  = inb(0x3c5);
	 outb(0x3c4, 0x0a); oldS3->Trio[3]  = inb(0x3c5);
	 outb(0x3c4, 0x0b); oldS3->Trio[4]  = inb(0x3c5);
	 outb(0x3c4, 0x0d); oldS3->Trio[5]  = inb(0x3c5);
	 outb(0x3c4, 0x15); oldS3->Trio[6]  = inb(0x3c5) & 0xfe;
	 outb(0x3c5, oldS3->Trio[6]);
	 outb(0x3c4, 0x18); oldS3->Trio[7]  = inb(0x3c5);

	 outb(0x3c4, 0x10); oldS3->Trio[8]  = inb(0x3c5);
	 outb(0x3c4, 0x11); oldS3->Trio[9]  = inb(0x3c5);
	 outb(0x3c4, 0x12); oldS3->Trio[10] = inb(0x3c5);
	 outb(0x3c4, 0x13); oldS3->Trio[11] = inb(0x3c5);
	 outb(0x3c4, 0x1a); oldS3->Trio[12] = inb(0x3c5);
	 outb(0x3c4, 0x1b); oldS3->Trio[13] = inb(0x3c5);

	 outb(0x3c4, 8);
	 outb(0x3c5, 0x00);
      }

      for (i = 0; i < 5; i++) {
	 outb(vgaCRIndex, 0x30 + i);
	 oldS3->s3reg[i] = inb(vgaCRReg);
	 outb(vgaCRIndex, 0x38 + i);
	 oldS3->s3reg[5 + i] = inb(vgaCRReg);
      }
      outb(vgaCRIndex, 0x36);
      oldS3->s3reg[10] = inb(vgaCRReg);

      outb(vgaCRIndex, 0x11);	/* allow writing to CR0-7 */
      tmp = inb(vgaCRReg);
      outb(vgaCRReg, tmp & 0x7f);
      for (i = 0; i < 16; i++) {
	 outb(vgaCRIndex, 0x40 + i);
	 oldS3->s3sysreg[i] = inb(vgaCRReg);
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

      for (i = 0; i < 16; i++) {
	 if (!((1 << i) & reg50_mask))
	    continue;
	 outb(vgaCRIndex, 0x50 + i);
	 oldS3->s3sysreg[i + 16] = inb(vgaCRReg);
      }
      for (i = 32; i < 46; i++) {
	 outb(vgaCRIndex, 0x40 + i);
	 oldS3->s3sysreg[i] = inb(vgaCRReg);
      }

      s3Initialised = 1;
      vgaNewVideoState = vgaHWSave(vgaNewVideoState, sizeof(vgaS3Rec));
      outb(DAC_MASK, 0);
   }

   if (s3UsingPixMux && (mode->Flags & V_PIXMUX))
      s3PixelMultiplexing = TRUE;
   else
      s3PixelMultiplexing = FALSE;

   if (S3_ViRGE_VX_SERIES(s3ChipId))
      pixMuxShift = 0;
   else
      pixMuxShift = -(s3Bpp == 2);

   if (!mode->CrtcHAdjusted) {	 
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
      if (DAC_IS_TI3030 && s3Bpp==1) {
	 /* for 128bit bus we need multiple of 16 8bpp pixels... */
	 if (mode->CrtcHTotal & 0x0f) {
	    mode->CrtcHTotal = (mode->CrtcHTotal + 0x0f) & ~0x0f;
	    changed = TRUE;
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

   if (s3MmioMem != NULL ) {
      WaitIdle();
      VerticalRetraceWait();
      ((mmtr)s3MmioMem)->memport_regs.regs.fifo_control = 0x17000;
      WaitIdle();                  /* Don't ask... */
      ((mmtr)s3MmioMem)->memport_regs.regs.streams_timeout = 0x8000;
   }

   #if 1
   			/* This seems to help, but maybe a wait for */
			/* VSYNC would be better than WaitIdleEmpty(). */
   				/* STREAMS disable code, for 24 & 32 Bpp */
				/* Disable the STREAMS processor BEFORE */
				/* any mode switching... */
   if ( ( s3InfoRec.bitsPerPixel == 32 || 
          s3InfoRec.bitsPerPixel == 24 ) &&
          s3MmioMem != NULL ) {
       outb(vgaCRIndex, 0x67);
       tmp = inb(vgaCRReg);	   
       /* WaitIdleEmpty(); *cep*/
 				/* Disable STREAMS processor */
       outb( vgaCRReg, tmp & ~0x0C );
   }	
   #endif
      
   new->MiscOutReg |= 0x0C;		/* enable CR42 clock selection */
   new->Sequencer[0] = 0x03;		/* XXXX shouldn't need this */
   new->CRTC[19] = s3BppDisplayWidth >> 3;
   new->CRTC[23] = 0xE3;		/* XXXX shouldn't need this */
   new->Attribute[0x11] = currents3dac_border; /* The overscan colour AR11 gets */
					/* disabled anyway */

   vgaProtect(TRUE);

   if (vgaIOBase == 0x3B0)
      new->MiscOutReg &= 0xFE;
   else
      new->MiscOutReg |= 0x01;
     
      
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
   }

   if (DAC_IS_TRIO)
   {
      int pixmux = 0;           /* SDAC command and CR67 */
      int invert_vclk = 0;      /* CR66 bit 0 */
      int sr8, sr15, sr18, cr33;

      outb(0x3C4, 1);
      tmp2 = inb(0x3C5);
      outb(0x3C5, tmp2 | 0x20); /* blank the screen */

      outb(0x3c4, 0x08);
      sr8 = inb(0x3c5);
      outb(0x3c5, 0x06);

      outb(0x3c4, 0x0d);        /* fix for VideoLogic GrafixStar cards: */
      tmp = inb(0x3c5);         /* disable feature connector to use 64bit DRAM bus */
      outb(0x3c5, tmp & ~1);

      outb(0x3c4, 0x15);
      sr15 = inb(0x3c5) & ~0x10;  /* XXXX ~0x40 and SynthClock /= 2 in s3.c might be better... */
      outb(0x3c4, 0x18);
      sr18 = inb(0x3c5) & ~0x80;
      outb(vgaCRIndex, 0x33);
      cr33 = inb(vgaCRReg) & ~0x28;

      if (s3PixelMultiplexing) {
         switch (s3InfoRec.bitsPerPixel)  {
	 case 8:
	    pixmux = 0x10;
	    break;

	 case 16:
	    cr33 |= 0x08;
	    if (s3Weight == RGB16_555)
	       pixmux = 0x30;
	    else
	       pixmux = 0x50;
	    break;
         }
	 if (!S3_ViRGE_VX_SERIES(s3ChipId)){
	    invert_vclk = 2;   /* XXXX strange: reserved bit which helps! */
	    sr15 |= 0x10;  /* XXXX 0x40? see above! */
	    sr18 |= 0x80;
	 }
      }
      else
      {
         switch (s3InfoRec.bitsPerPixel)
         {
            case 8:  /* 8-bit color, 1 VCLK/pixel */
	       pixmux = 0;
               break;

            case 16: /* 15/16-bit color, 1VCLK/pixel */
	       cr33 |= 0x08;
	       if (S3_ViRGE_VX_SERIES(s3ChipId))
		  if (s3Weight == RGB16_555)
		     pixmux = 0x20;
		  else
		     pixmux = 0x40;
	       else
		  if (s3Weight == RGB16_555)
		     pixmux = 0x30;
		  else
		     pixmux = 0x50;
               break;

            case 24: /* packed 24-bit color */
            case 32: /* 32-bit color, 2VCLK/pixel */
               pixmux = 0xd0;
         }
      }

      outb(vgaCRReg, cr33);

      if (S3_ViRGE_VX_SERIES(s3ChipId)) {
	 outb(vgaCRIndex, 0x55);
	 tmp = inb(vgaCRReg) & ~0x40;
	 if (s3DAC8Bit) {
	    tmp |= 0x40;
	    pixmux |= 0x02;  /* enable "gamma correction" */
	 }
	 outb(vgaCRReg, tmp);
      }

      outb(vgaCRIndex, 0x67);	
      		/* KJB - temp */
      tmp = inb(vgaCRReg) & 0xf;
      outb(vgaCRReg, 0x50 | tmp);  /* ask S3 why this is needed for ViRGE/VX ... */
      #if 0
      outb(vgaCRReg, 0x50);  /* ask S3 why this is needed for ViRGE/VX ... */
	/* KJB - temp */
      usleep(10000);
      outb(vgaCRIndex, 0x67);
	/* KJB - temp */
      #endif
      outb(vgaCRReg, pixmux | invert_vclk);    /* set S3 mux mode */

      outb(0x3c4, 0x15);
      outb(0x3c5, sr15);
      outb(0x3c4, 0x18);
      outb(0x3c5, sr18);
      outb(0x3c4, 0x08);
      outb(0x3c5, sr8);

      outb(0x3C4, 1);
      outb(0x3C5, tmp2);        /* unblank the screen */

      if (s3InfoRec.MemClk > 0) {
	 if (OFLG_ISSET(CLOCK_OPTION_S3TRIO64V2, &s3InfoRec.clockOptions))
	    S3Trio64V2SetClock(s3InfoRec.MemClk, 10);
	 else if (S3_ViRGE_VX_SERIES(s3ChipId))
	    S3ViRGE_VXSetClock(s3InfoRec.MemClk, 10);
	 else
	    S3TrioSetClock(s3InfoRec.MemClk, 10);
      }
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
   if (S3_ViRGE_SERIES(s3ChipId)) {
      outb(vgaCRIndex, 0x3a);
      outb(vgaCRReg, 0xb5 & 0x7f);
   } else
#endif
      {
	 outb(vgaCRIndex, 0x66);  /* set CR66_7 before CR3A_7 */
	 tmp = inb(vgaCRReg);
	 outb(vgaCRReg, tmp | 0x80);
	 
	 outb(vgaCRIndex, 0x3a);
	 if (OFLG_ISSET(OPTION_SLOW_DRAM_REFRESH, &s3InfoRec.options))
	    outb(vgaCRReg, 0xb7);
	 else
	    outb(vgaCRReg, 0xb5);
      }

   outb(vgaCRIndex, 0x3b);
   outb(vgaCRReg, (new->CRTC[0] + new->CRTC[4] + 1) / 2);
   outb(vgaCRIndex, 0x3c);
   outb(vgaCRReg, new->CRTC[0]/2);	/* Interlace mode frame offset */

   outb(vgaCRIndex, 0x40);
   if (s3Localbus) {
      i = (inb(vgaCRReg) & 0xf2);
      s3Port40 = (i | 0x05);
      outb(vgaCRReg, s3Port40);
   } else {
      i = (inb(vgaCRReg) & 0xf6);
      s3Port40 = (i | 0x01);
      outb(vgaCRReg, s3Port40);
   }

   outb(vgaCRIndex, 0x43);
   outb(vgaCRReg, 0x00);

   outb(vgaCRIndex, 0x44);
   outb(vgaCRReg, 0x00);

   outb(vgaCRIndex, 0x45);
   i = inb(vgaCRReg) & 0xf2;

   /* hi/true cursor color enable */
   switch (s3InfoRec.bitsPerPixel) {
   case 24:
   case 32:
      i = i | 0x04; /* for 16bit RAMDAC, 0x0c for 8bit RAMDAC */
      break;
   }
   outb(vgaCRReg, i);

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
   case 32:			/* actually 24 bpp */
      i |= 0x20;
      break;
#if 0
   case 32:
      i |= 0x30;
      break;
#endif
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
   default:			/* 1024 and 2048 */
      ;
   }
   outb(vgaCRReg, i);

   outb(vgaCRIndex, 0x51);
   s3Port51 = (inb(vgaCRReg) & 0xC0) | ((s3BppDisplayWidth >> 7) & 0x30);

   /* ensure split vram xfer is enabled, per S3 doc -hu */
   if (S3_ViRGE_VX_SERIES(s3ChipId)) s3Port51 &= ~0x40;
   outb(vgaCRReg, s3Port51);

   outb(vgaCRIndex, 0x58);	/* disable linear mode */
   outb(vgaCRReg, s3SAM256);

#ifdef DEBUG
   ErrorF("Writing CR59 0x%02x, CR5A 0x%02x\n", s3Port59, s3Port5A);
#endif

   outb(vgaCRIndex, 0x53);	/* disable mmio mode */
   tmp = inb(vgaCRReg) & ~0x18;
   outb(vgaCRReg, tmp);

   if (s3InfoRec.MemBase != 0) {
      s3Port59 = (s3InfoRec.MemBase >> 24) & 0xfc;
      s3Port5A = 0;
   }
   outb(vgaCRIndex, 0x59);
   outb(vgaCRReg, s3Port59);
   outb(vgaCRIndex, 0x5A);
   outb(vgaCRReg, s3Port5A);

   outb(vgaCRIndex, 0x53);	/* enable new mmio mode */
   outb(vgaCRReg, tmp | 0x08);

   outb (vgaCRIndex, 0x58);	/* enable linear mode */
   outb (vgaCRReg, (s3LinApOpt & ~0x04) | s3SAM256); /* window size for linear mode */

   /* XXX m/n calculation should be adopted to EDO-DRAMs */
   n = 255;
   outb(vgaCRIndex, 0x54);
   if (S3_ViRGE_VX_SERIES(s3ChipId)) 
      m = 0;
   else {
      int clock,mclk;
      clock = s3InfoRec.clock[mode->Clock] * realS3Bpp /*s3Bpp*/;
      if (s3InfoRec.s3MClk > 0)
	 mclk = s3InfoRec.s3MClk;
      else
	 mclk = 60000;  /* 60 MHz, limit for 864 */
      m = (int)((mclk/1000.0*.72+16.867)*89.736/(clock/1000.0+39)-21.1543);
   }
   m -= s3InfoRec.s3Madjust;
   if (m > 31) m = 31;
   else if (m < 0) {
      m = 0;
      n = 16;
   }

   s3Port54 = m << 3;
   outb(vgaCRReg, s3Port54);

   n -= s3InfoRec.s3Nadjust;
   if (n < 0) n = 0;
   else if (n > 255) n = 255;
   outb(vgaCRIndex, 0x60);
   outb(vgaCRReg, n);

   /* enable enhanced functions */
   if (S3_ViRGE_VX_SERIES(s3ChipId))
      outb(vgaCRIndex, 0x63);
   else
      outb(vgaCRIndex, 0x66);
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, tmp | 1);

   /* This shouldn't be needed -- they should be set by vgaHWInit() */
   if (!mode->CrtcVAdjusted) {
      mode->CrtcVTotal /= interlacedived;
      mode->CrtcVDisplay /= interlacedived;
      mode->CrtcVSyncStart /= interlacedived;
      mode->CrtcVSyncEnd /= interlacedived;
      mode->CrtcVAdjusted = TRUE;
   }

   i = (((mode->CrtcVTotal - 2) & 0x400) >> 10)  |
      (((mode->CrtcVDisplay - 1) & 0x400) >> 9) |
	 (((mode->CrtcVSyncStart) & 0x400) >> 8)   |
	    (((mode->CrtcVSyncStart) & 0x400) >> 6)   | 0x40;
   outb(vgaCRIndex, 0x5e);
   outb(vgaCRReg, i);

   i = ((((mode->CrtcHTotal >> 3) - 5) & 0x100) >> 8) |
      ((((mode->CrtcHDisplay >> 3) - 1) & 0x100) >> 7) |
	 ((((mode->CrtcHSyncStart >> 3) - 1) & 0x100) >> 6) |
	    ((mode->CrtcHSyncStart & 0x800) >> 7);
   if ((mode->CrtcHSyncEnd >> 3) - (mode->CrtcHSyncStart >> 3) > 64)
      i |= 0x08;		/* add another 64 DCLKs to blank pulse width */

   if ((mode->CrtcHSyncEnd >> 3) - (mode->CrtcHSyncStart >> 3) > 32)
      i |= 0x20;   /* add another 32 DCLKs to hsync pulse width */


   outb(vgaCRIndex, 0x3b);
   itmp = (  new->CRTC[0] + ((i&0x01)<<8)
	   + new->CRTC[4] + ((i&0x10)<<4) + 1) / 2;
   if (itmp-(new->CRTC[4] + ((i&0x10)<<4)) < 10)
      if (new->CRTC[4] + ((i&0x10)<<4) + 10 <= new->CRTC[0]+ ((i&0x01)<<8))
	 itmp = new->CRTC[4] + ((i&0x10)<<4) + 10;
      else
	 itmp = new->CRTC[0]+ ((i&0x01)<<8) + 1;
   outb(vgaCRReg, itmp & 0xff);
   i |= (itmp&0x100) >> 2;
   outb(vgaCRIndex, 0x3c);
   outb(vgaCRReg, (new->CRTC[0] + ((i&0x01)<<8)) /2);	/* Interlace mode frame offset */

   outb(vgaCRIndex, 0x5d);
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, (tmp & 0x80) | i);

   if (s3InfoRec.videoRam > 1024)
      i = mode->HDisplay * realS3Bpp /*s3Bpp*/ / 8 + 1;

   outb(vgaCRIndex, 0x61);
   outb(vgaCRReg, (i >> 8) | 0x80);
   outb(vgaCRIndex, 0x62);
   outb(vgaCRReg, i & 0xff);

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

   if (S3_ViRGE_DXGX_SERIES(s3ChipId)) {
      /* disable DAC power saving to avoid bright left edge */
      outb(0x3d4,0x86);
      outb(0x3d5,0x80);
      /* disable the stream display fetch length control */
      outb(0x3d4,0x90);
      outb(0x3d5,0x00);
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
	 if (S3_ViRGE_VX_SERIES(s3ChipId)) {
	    outb(vgaCRIndex, 0x6d);
	    outb(vgaCRReg, mode->Private[S3_BLANK_DELAY]);
	 } else {
	    outb(vgaCRIndex, 0x65);
	    tmp = inb(vgaCRReg) & 0xc7;
	    outb(vgaCRReg,tmp | ((mode->Private[S3_BLANK_DELAY] & 0x07) << 3));
	 }
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
      outb(vgaCRReg, tmp | 0x04 | 0x08);
   }

   if (OFLG_ISSET(OPTION_SLOW_VRAM, &s3InfoRec.options)) {
      /*
       * increase -RAS low timing from 3.5 MCLKs to 4.5 MCLKs
       */
      outb(vgaCRIndex, 0x39);
      outb(vgaCRReg, 0xa5);
      outb(vgaCRIndex, 0x68);
      tmp = inb(vgaCRReg);
      outb(vgaCRReg, tmp & ~0x04);
   }

   if (OFLG_ISSET(OPTION_SLOW_DRAM, &s3InfoRec.options)) {
      /*
       * increase -RAS precharge timing from 2.5 MCLKs to 3.5 MCLKs
       */
      outb(vgaCRIndex, 0x39);
      outb(vgaCRReg, 0xa5);
      outb(vgaCRIndex, 0x68);
      tmp = inb(vgaCRReg);
      outb(vgaCRReg, tmp & ~0x08);
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

   if (S3_ViRGE_VX_SERIES(s3ChipId)) {
      outb(vgaCRIndex, 0x36);            /* ViRGE/VX requires 1-cycle EDO */
      tmp = inb(vgaCRReg);               /* for GE operations (or FPM) */
      if (OFLG_ISSET(OPTION_FPM_VRAM, &s3InfoRec.options))
	 outb(vgaCRReg, tmp |  0x0c);
      else
	 outb(vgaCRReg, tmp & ~0x0c);

      outb(vgaCRIndex, 0x67);
      tmp = inb(vgaCRReg);
      outb(vgaCRReg, 0x50);  /* ask S3 why this is needed for ViRGE/VX ... */
      usleep(10000);
      outb(vgaCRIndex, 0x67);
      outb(vgaCRReg, tmp);
   }
			/* Start the STREAMS processor, only active */
			/* for 24 or 32 bpp modes. */
   /* s3InitSTREAMS( mode->HDisplay, mode->VDisplay ); KJB */

   if (s3MmioMem != NULL)
      s3AdjustFrame(s3InfoRec.frameX0, s3InfoRec.frameY0);

   vgaProtect(FALSE);


 /*
  * Blank the screen temporarily to display color 0 by turning the display of
  * all planes off
  */
   outb(DAC_MASK, 0x00);

   outb(DAC_MASK, 0xff);

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

   if (s3InfoRec.bitsPerPixel > 8 && S3_ViRGE_VX_SERIES(s3ChipId)) {
      extern unsigned char xf86rGammaMap[], xf86gGammaMap[], xf86bGammaMap[];
      extern LUTENTRY currents3dac[];

      if (!LUTInited) {
	 if (s3Weight == RGB32_888 || S3_ViRGE_VX_SERIES(s3ChipId)) {
	    for(i=0; i<256; i++) {
	       currents3dac[i].r = xf86rGammaMap[i];
	       currents3dac[i].g = xf86gGammaMap[i];
	       currents3dac[i].b = xf86bGammaMap[i];
	    }
	 }
	 else {
	    int r,g,b;
	    int mr,mg,mb;
	    int nr=5, ng=5, nb=5;

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
 * s3InitSTREAMS( mode )
 *
 * Enables and sets up the STREAMS processor for 24/32 bpp
 */
void
s3InitSTREAMS( mode )
   DisplayModePtr mode;
#if 0
   int	RequestWidth;
   int	RequestHeight;
#endif
{

   unsigned char tmp;

   if (s3MmioMem != NULL && oldS3->fifo[0] == 0) {
      WaitIdle();
      VerticalRetraceWait();
      oldS3->fifo[0] = ((mmtr)s3MmioMem)->memport_regs.regs.fifo_control;
      WaitIdle();                  /* Don't ask... */
      oldS3->fifo[1] = ((mmtr)s3MmioMem)->memport_regs.regs.streams_timeout;
   }

    				/* Only for 24 or 32 bpp!  Also needs */
				/* s3MmioMem to be mapped . */
    if (!( (s3InfoRec.bitsPerPixel == 32 || 
            s3InfoRec.bitsPerPixel == 24 ) &&
           s3MmioMem != NULL)) return;

	   			/* Wait for VSYNC */
       VerticalRetraceWait();
       outb(vgaCRIndex, 0x67);
       tmp = inb(vgaCRReg);
 				/* Enable full STREAMS processor */
       outb( vgaCRReg, tmp | 0x0C );

/* virge is packed 24 only  -hu
       if ( s3InfoRec.bitsPerPixel == 24 ) {
*/
 				/* data format 8.8.8 (24 bpp) */
         ((mmtr)s3MmioMem)->streams_regs.regs.prim_stream_cntl = 0x06000000;
#if 0
       } else {
                                /* one more bit for X.8.8.8, 32 bpp */
         ((mmtr)s3MmioMem)->streams_regs.regs.prim_stream_cntl = 0x07000000;
       }
#endif
 				/* NO chroma keying... */
       ((mmtr)s3MmioMem)->streams_regs.regs.col_chroma_key_cntl = 0x0;
 				/* Secondary stream format KRGB-16 */
                                /* data book suggestion... */
       ((mmtr)s3MmioMem)->streams_regs.regs.second_stream_cntl = 0x03000000;
 
       ((mmtr)s3MmioMem)->streams_regs.regs.chroma_key_upper_bound = 0x0;
 
       ((mmtr)s3MmioMem)->streams_regs.regs.second_stream_stretch = 0x0;
				/* use 0x01000000 for primary over second. */
				/* use 0x0 for second over prim. */ 
       ((mmtr)s3MmioMem)->streams_regs.regs.blend_cntl = 0x01000000;
 
       ((mmtr)s3MmioMem)->streams_regs.regs.prim_fbaddr0 = 0x0;
 
       ((mmtr)s3MmioMem)->streams_regs.regs.prim_fbaddr1 = 0x0;
 
                                /* Stride is 3 bytes for 24 bpp mode and */
                                /* 4 bytes for 32 bpp. */
/* virge is packed 24 only  -hu
       if ( s3InfoRec.bitsPerPixel == 24 ) {
*/
         ((mmtr)s3MmioMem)->streams_regs.regs.prim_stream_stride = s3InfoRec.virtualX * 3;
         /*((mmtr)s3MmioMem)->streams_regs.regs.prim_stream_stride = mode->HDisplay * 3;*/
#if 0
       } else {
         ((mmtr)s3MmioMem)->streams_regs.regs.prim_stream_stride = s3InfoRec.virtualX * 4;
         /*((mmtr)s3MmioMem)->streams_regs.regs.prim_stream_stride = mode->HDisplay * 4;*/
       }
#endif
 				/* Choose fbaddr0 as stream source. */
       ((mmtr)s3MmioMem)->streams_regs.regs.double_buffer = 0x0;
 
       ((mmtr)s3MmioMem)->streams_regs.regs.second_fbaddr0 = 0x0;
 
       ((mmtr)s3MmioMem)->streams_regs.regs.second_fbaddr1 = 0x0;
 
       ((mmtr)s3MmioMem)->streams_regs.regs.second_stream_stride = 0x1;
 				/* Set primary stream on top of secondary */
 				/* stream. */
       ((mmtr)s3MmioMem)->streams_regs.regs.opaq_overlay_cntl = 0x40000000;
 				/* Vertical scale factor. */
       ((mmtr)s3MmioMem)->streams_regs.regs.k1 = 0x0;
 
       ((mmtr)s3MmioMem)->streams_regs.regs.k2 = 0x0;
 				/* Vertical accum. initial value. */
       ((mmtr)s3MmioMem)->streams_regs.regs.dda_vert = 0x0;
 				/* X and Y start coords + 1. */
       ((mmtr)s3MmioMem)->streams_regs.regs.prim_start_coord =  0x00010001;
/*         (s3InfoRec.frameX0 + 1) << 16 | (s3InfoRec.frameY0 + 1);  KJB - temp */
 				/* Specify window Width -1 and Height of */
 				/* stream. */
 				/* ScreenHeight ??? */
				/* Set the STREAM source to the frame. */
       ((mmtr)s3MmioMem)->streams_regs.regs.prim_window_size = 
       #if 0  	/* temp - KJB */
           (s3InfoRec.frameX1 - s3InfoRec.frameX0) << 16 | 
           (s3InfoRec.frameY1 - s3InfoRec.frameY0 + 1);
	   
	   ( (RequestWidth << 16) | (RequestHeight + 1) );
	#else
	   (mode->HDisplay-1) << 16 | (mode->VDisplay);
	#endif
				/* Book says 0x07ff07ff. */ 
       ((mmtr)s3MmioMem)->streams_regs.regs.second_start_coord = 0x07ff07ff;
 
       ((mmtr)s3MmioMem)->streams_regs.regs.second_window_size = 0x00010001;
 
                                /* STREAMS enable after? */
				/* NO, YOU DON't want to do this! */
       /* outb( vgaCRReg, tmp | 0x0C ); */
 
    /* end STREAMS processor. */
 
} /*s3InitSTREAMS()*/

/*
 * s3InitEnvironment()
 *
 * Initializes the drawing environment and clears the display.
 */
void
s3InitEnvironment()
{
		   /* Set up the STREAMS processor */
   /* s3InitSTREAMS( s3InfoRec.width, s3InfoRec.height ); KJB */

   /* Current mixes, src, foreground active */

   if (s3MmioMem != NULL) {   /* wait until s3MmioMem is initialzied */
      s3_gcmd = DRAW | CMD_ITA_DWORD | CMD_HWCLIP;

      if (s3Bpp == 1) {
	 s3_gcmd |= DST_8BPP;
	 s3bltbug_width1 = 51;
	 s3bltbug_width2 = 64;
      }
      else if (s3Bpp == 2) {
	 s3_gcmd |= DST_16BPP;
	 s3bltbug_width1 = 26;
	 s3bltbug_width2 = 32;
      }
      else {
	 s3_gcmd |= DST_24BPP;
	 s3bltbug_width1 = 16;
	 s3bltbug_width2 = 22;
      }

      /* reset S3 graphics engine and disable all interrupts. */
      SET_SUBSYS_CRTL(GPCTRL_RESET);
      SET_SUBSYS_CRTL(GPCTRL_ENAB);
      usleep(10000);  /* wait a little bit... */

      if (IN_SUBSYS_STAT() != 0x3000) {  /* 2nd try */
	int tmp;
	if (S3_ViRGE_VX_SERIES(s3ChipId))
	   outb(vgaCRIndex, 0x63);
	else 
	   outb(vgaCRIndex, 0x66);
	tmp = inb(vgaCRReg);
	outb(vgaCRReg, tmp |  0x02);
	outb(vgaCRReg, tmp & ~0x02);
	usleep(10000);  /* wait a little bit... */
      }
      WaitIdleEmpty();

      SETB_CMD_SET(CMD_NOP);

      WaitQueue(10);
      SETB_SRC_BASE(0);
      SETB_DEST_BASE(0);
      SETL_SRC_BASE(0);
      SETL_DEST_BASE(0);

      /* Clipping rectangle to full drawable space */
      SETB_CLIP_L_R(0, s3DisplayWidth-1);
      SETB_CLIP_T_B(0, s3ScissB);
      SETB_DEST_SRC_STR(s3BppDisplayWidth, s3BppDisplayWidth);
      SETL_CLIP_L_R(0, s3DisplayWidth-1);
      SETL_CLIP_T_B(0, s3ScissB);
      SETL_DEST_SRC_STR(s3BppDisplayWidth, s3BppDisplayWidth);

      WaitQueue(7);
      /* Enable writes to all planes and reset color compare */
      ;SET_WRT_MASK(~0);

      /*
       * Clear the display.  Need to set the color, origin, and size. Then draw.
       */
      if (xf86FlipPixels && s3Bpp == 1)
	 SETB_PAT_FG_CLR(1);
      else
	 SETB_PAT_FG_CLR(0);

      SETB_RSRC_XY(0,0);
      SETB_RDEST_XY(0,0);
      SETB_RWIDTH_HEIGHT(s3InfoRec.virtualX-1, s3ScissB+1);
      SETB_MONO_PAT0(~0);
      SETB_MONO_PAT1(~0);
      SETB_CMD_SET((s3_gcmd & ~CMD_HWCLIP) | CMD_BITBLT | MIX_MONO_PATT | INC_X | INC_Y | ROP_P);

      WaitIdleEmpty();

      /* Reset current draw position */
      SETB_RSRC_XY(0,0);
      SETB_RDEST_XY(0,0);

      /* Reset current colors, foreground is all on, background is 0. */
      SETB_PAT_FG_CLR(~0);
      SETB_PAT_BG_CLR(0);
   }
   else {
      int tmp;
      /* reset S3 graphics engine */
      if (S3_ViRGE_VX_SERIES(s3ChipId))
	 outb(vgaCRIndex, 0x63);
      else 
	 outb(vgaCRIndex, 0x66);
      tmp = inb(vgaCRReg);
      outb(vgaCRReg, tmp |  0x02);
      outb(vgaCRReg, tmp & ~0x02);
      usleep(10000);  /* wait a little bit... */
   }

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

   outb(vgaCRIndex, 0x11);		/* allow writing to CR0-7 */
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, tmp & 0x7f);

}
