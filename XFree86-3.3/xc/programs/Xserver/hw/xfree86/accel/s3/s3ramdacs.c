/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3ramdacs.c,v 3.10.2.4 1997/05/19 08:06:55 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Thomas Roell makes no
 * representations about the suitability of this software for any purpose. It
 * is provided "as is" without express or implied warranty.
 * 
 * THOMAS ROELL AND KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL THOMAS ROELL OR KEVIN E. MARTIN BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 * 
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * 
 * Header: /home/src/xfree86/mit/server/ddx/xf86/accel/s3/RCS/s3.c,v 2.0
 * 1993/02/22 05:58:13 jon Exp
 * 
 * Modified by Amancio Hasty and Jon Tombs
 *
 * Rather severely reorganized by MArk Vojkovich (mvojkovi@ucsd.edu)
 * 
 */

#include "misc.h"
#include "cfb.h"
#include "pixmapstr.h"
#include "fontstruct.h"
#include "s3.h"
#include "regs3.h"
#include "xf86_HWlib.h"
#include "xf86_PCI.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
#include "s3linear.h"
#include "s3Bt485.h"
#include "Ti302X.h"
#include "IBMRGB.h"
#include "s3ELSA.h"

extern char *clockchip_probed;		/* in s3.c */
extern Bool pixMuxPossible;
extern Bool allowPixMuxInterlace;
extern Bool allowPixMuxSwitching;
extern int nonMuxMaxClock;
extern int nonMuxMaxMemory;
extern int pixMuxMinWidth;
extern int pixMuxMinClock;
extern Bool pixMuxLimitedWidths;
extern Bool clockDoublingPossible;
extern Bool s3PixelMultiplexing;
extern Bool s3DACSyncOnGreen;
extern Bool pixMuxNeeded;
extern int s3BiosVendor;
extern int maxRawClock;
extern int numClocks;
extern int s3MaxClock;

extern unsigned char *find_bios_string(int, char *, char *);
extern void (* dacOutTi3026IndReg)(unsigned char,unsigned char,unsigned char);
extern unsigned char (* dacInTi3026IndReg)(unsigned char);
 

static Bool NORMAL_Probe(void);
static Bool S3_TRIO32_Probe(void);
static Bool S3_TRIO64_Probe(void);
static Bool S3_TRIO64V2_Probe(void);
static Bool TI3026_Probe(void);
static Bool TI3030_Probe(void);
static Bool TI3020_Probe(void);
static Bool TI3025_Probe(void);
static Bool BT485_Probe(void);
static Bool ATT20C505_Probe(void);
static Bool ATT22C498_Probe(void);
static Bool ATT498_Probe(void);
static Bool ATT20C409_Probe(void);
static Bool SC15025_Probe(void);
static Bool STG1700_Probe(void);
static Bool STG1703_Probe(void);
static Bool IBMRGB524_Probe(void);
static Bool IBMRGB525_Probe(void);
static Bool IBMRGB528_Probe(void);
static Bool S3_SDAC_Probe(void);
static Bool S3_GENDAC_Probe(void);
static Bool ATT20C490_Probe(void);
static Bool SS2410_Probe(void);
static Bool SC1148x_Probe(void);

static Bool Null_Probe(void) {return FALSE;}

static int BT485_SERIES_PreInit(void);
static int TI3020_3025_PreInit(void);
static int ATT409_498_PreInit(void);
static int SC15025_PreInit(void);
static int STG17xx_PreInit(void);
static int S3_SDAC_GENDAC_PreInit(void);
static int S3_TRIO_PreInit(void);
static int TI3030_3026_PreInit(void);
static int IBMRGB52x_PreInit(void);
static int MISC_HI_COLOR_PreInit(void);
static int NORMAL_PreInit(void);
static int Null_PreInit(void) {return 0;}


static void Null_Restore(void) {}
static void S3_TRIO_Restore(void);
static void TI3030_3026_Restore(void);
static void TI3020_3025_Restore(void);
static void BT485_Restore(void);
static void ATT409_498_Restore(void);
static void SC15025_Restore(void);
static void STG17xx_Restore(void);
static void IBMRGB52x_Restore(void);
static void S3_SDAC_GENDAC_Restore(void);
static void SC1148x_Restore(void);
static void SS2410_Restore(void);
static void ATT20C490_Restore(void);

static void Null_Save(void) {}
static void S3_TRIO_Save(void);
static void TI3030_3026_Save(void);
static void TI3020_3025_Save(void);
static void BT485_Save(void);
static void ATT409_498_Save(void);
static void SC15025_Save(void);
static void STG17xx_Save(void);
static void IBMRGB52x_Save(void);
static void S3_SDAC_GENDAC_Save(void);
static void SC1148x_Save(void);
static void SS2410_Save(void);
static void ATT20C490_Save(void);

static int Null_Init(DisplayModePtr mode){return 1;}
static int SC1148x_Init(DisplayModePtr);
static int ATT20C490_Init(DisplayModePtr);
static int ATT409_498_Init(DisplayModePtr);
static int STG17xx_Init(DisplayModePtr);
static int S3_SDAC_Init(DisplayModePtr);
static int S3_GENDAC_Init(DisplayModePtr);
static int S3_TRIO_Init(DisplayModePtr);
static int BT485_Init(DisplayModePtr);
static int TI3020_3025_Init(DisplayModePtr);
static int TI3030_3026_Init(DisplayModePtr);
static int IBMRGB52x_Init(DisplayModePtr);
static int SC15025_Init(DisplayModePtr);
static int SS2410_Init(DisplayModePtr);


extern pointer vgaNewVideoState;
#define new ((vgaHWPtr)vgaNewVideoState)


Bool  (*s3ClockSelectFunc) (int);
static Bool LegendClockSelect(int);
static Bool s3ClockSelect(int);
static Bool icd2061ClockSelect(int);
static Bool s3GendacClockSelect(int);
static Bool ti3025ClockSelect(int);
static Bool ti3026ClockSelect(int);
static Bool IBMRGBClockSelect(int);

static void s3ProgramTi3025Clock(
#if NeedFunctionPrototypes
	int clk,
	unsigned char n,
	unsigned char m,
	unsigned char p
#endif
);

static Bool ch8391ClockSelect(int);
static Bool att409ClockSelect(int);
static Bool STG1703ClockSelect(int);
static Bool Gloria8ClockSelect(int);

static unsigned char s3DacRegs[0x101];

/* NOTE:  This order must be the same as the #define order in
	s3.h !!!!!!!! */
s3RamdacInfo s3Ramdacs[] = {
	   /*   DacName,  DacSpeed, DacProbe(), PreInit(), DacRestore(),
			DacSave(),  DacInit() */
/* 0 */		{"normal", 110000, NORMAL_Probe, NORMAL_PreInit,Null_Restore,
			Null_Save, Null_Init},
/* 1 */		{"s3_trio32", 135000, S3_TRIO32_Probe, S3_TRIO_PreInit, 	
			S3_TRIO_Restore,S3_TRIO_Save,S3_TRIO_Init},
/* 2 */		{"s3_trio64", 135000, S3_TRIO64_Probe, S3_TRIO_PreInit, 	
			S3_TRIO_Restore,S3_TRIO_Save,S3_TRIO_Init},
/* 3 */		{"ti3026", 135000, TI3026_Probe, TI3030_3026_PreInit,
			TI3030_3026_Restore,TI3030_3026_Save,TI3030_3026_Init},
/* 4 */		{"ti3030", 175000, TI3030_Probe, TI3030_3026_PreInit,
			TI3030_3026_Restore,TI3030_3026_Save,TI3030_3026_Init},
/* 5 */		{"ti3020", 135000, TI3020_Probe, TI3020_3025_PreInit, 
			TI3020_3025_Restore,TI3020_3025_Save,TI3020_3025_Init},
/* 6 */		{"ti3025", 135000, TI3025_Probe, TI3020_3025_PreInit,
			TI3020_3025_Restore,TI3020_3025_Save,TI3020_3025_Init},
/* 7 */		{"Bt485", 135000, BT485_Probe, BT485_SERIES_PreInit,
			BT485_Restore, BT485_Save, BT485_Init},
/* 8 */		{"att20c505", 135000, ATT20C505_Probe, BT485_SERIES_PreInit,
			BT485_Restore, BT485_Save, BT485_Init},
/* 9 */		{"att22c498", 135000, ATT22C498_Probe, ATT409_498_PreInit,
			ATT409_498_Restore,ATT409_498_Save,ATT409_498_Init},
/* 10 */	{"att20c498", 135000, ATT498_Probe, ATT409_498_PreInit,
			ATT409_498_Restore, ATT409_498_Save, ATT409_498_Init},
/* 11 */	{"att20c409", 135000, ATT20C409_Probe, ATT409_498_PreInit,
			ATT409_498_Restore, ATT409_498_Save, ATT409_498_Init},
/* 12 */	{"sc15025", 110000, SC15025_Probe, SC15025_PreInit,
			SC15025_Restore,SC15025_Save,SC15025_Init},
/* 13 */	{"stg1700", 135000, STG1700_Probe, STG17xx_PreInit,
			STG17xx_Restore,STG17xx_Save,STG17xx_Init},
/* 14 */	{"stg1703", 135000, STG1703_Probe, STG17xx_PreInit,
			STG17xx_Restore,STG17xx_Save,STG17xx_Init},
/* 15 */	{"ibm_rgb524", 170000, IBMRGB524_Probe, IBMRGB52x_PreInit,
			IBMRGB52x_Restore,IBMRGB52x_Save,IBMRGB52x_Init},
/* 16 */	{"ibm_rgb525", 170000, IBMRGB525_Probe, IBMRGB52x_PreInit,
			IBMRGB52x_Restore,IBMRGB52x_Save,IBMRGB52x_Init},
/* 17 */	{"ibm_rgb528", 170000, IBMRGB528_Probe, IBMRGB52x_PreInit,
			IBMRGB52x_Restore,IBMRGB52x_Save,IBMRGB52x_Init},
/* 18 */	{"s3_sdac", 135000, S3_SDAC_Probe, S3_SDAC_GENDAC_PreInit,
			S3_SDAC_GENDAC_Restore,S3_SDAC_GENDAC_Save,
			S3_SDAC_Init},
/* 19 */	{"s3_gendac", 110000, S3_GENDAC_Probe, S3_SDAC_GENDAC_PreInit,
			S3_SDAC_GENDAC_Restore,S3_SDAC_GENDAC_Save,
			S3_GENDAC_Init},
/* 20 */	{"att20c490", 110000, ATT20C490_Probe, MISC_HI_COLOR_PreInit,
			ATT20C490_Restore,ATT20C490_Save,ATT20C490_Init},
/* 21 */	{"ss2410", 110000, SS2410_Probe, MISC_HI_COLOR_PreInit,
			SS2410_Restore,SS2410_Save,SS2410_Init},
/* 22 */	{"sc1148x", 110000, SC1148x_Probe, MISC_HI_COLOR_PreInit,
			SC1148x_Restore,SC1148x_Save,SC1148x_Init},
/* 23 */	{"s3_trio64v2", 170000, S3_TRIO64V2_Probe, S3_TRIO_PreInit, 	
			S3_TRIO_Restore,S3_TRIO_Save,S3_TRIO_Init},
/* 24 */	{NULL, 0, Null_Probe, Null_PreInit,Null_Restore,Null_Save,
			Null_Init}
}; 

#if 0

static Bool TEMPLATE_PreInit()
{
    /* Verify that depth is supported by ramdac */

    /* Set cursor options */
   
    /* Check if PixMux is supported and set the PixMux
	related flags and variables */

    /* If there is an internal clock, set s3ClockSelectFunc, maxRawClock
	numClocks and whatever options need to be set. For external
	clocks, pass the job to OtherClocksSetup() */

    /* Make any necessary clock alterations due to multiplexing,
	clock doubling, etc...  s3Probe will do some last minute
	clock sanity checks when we return */

	return 1;
}

#endif

static void OtherClocksSetup(void);


/*************************************************************\
			
			BT485_DAC

\*************************************************************/

static Bool BT485_Probe()
{	
   /*
    * Probe for the bloody thing.  Set 0x3C6 to a bogus value, then
    * try to get the Bt485 status register.  If it's there, then we will
    * get something else back from this port.
    */

    Bool found = FALSE;
    unsigned char tmp,tmp2,saveCR43;

    /*quick check*/
    if (!S3_928_ONLY(s3ChipId) && !S3_964_SERIES(s3ChipId) && 
			!S3_968_SERIES(s3ChipId))
	return FALSE;

    outb(vgaCRIndex, 0x43);
    saveCR43 = inb(vgaCRReg);
    outb(vgaCRReg, saveCR43 & ~0x02);

    tmp = inb(0x3C6);
    outb(0x3C6, 0x0F);
    if (((tmp2 = s3InBtStatReg()) & 0x80)) {
          /*
           * Found either a BrookTree Bt485 or AT&T 20C505.
           */
          if ((tmp2 & 0xF0) != 0xD0) {
             found = TRUE;
             ErrorF("%s %s: Detected a BrookTree Bt485 RAMDAC\n",
                    XCONFIG_PROBED, s3InfoRec.name);

             /* If it is a Bt485 and no clockchip is specified in the 
                XF86Config, set clockchips for SPEA Mercury / Mercury P64 */

             if (!OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions))
              if (OFLG_ISSET(OPTION_SPEA_MERCURY, &s3InfoRec.options)) {
               if (S3_964_SERIES(s3ChipId)) {
                   OFLG_SET(CLOCK_OPTION_ICD2061A, &s3InfoRec.clockOptions);
                   OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
                   clockchip_probed = XCONFIG_PROBED;
               } else if (S3_928_ONLY(s3ChipId)) {
                   OFLG_SET(CLOCK_OPTION_SC11412, &s3InfoRec.clockOptions);
                   OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
                   clockchip_probed = XCONFIG_PROBED;
               }
              }    
          }
     }
     outb(0x3C6, tmp);

     outb(vgaCRIndex, 0x43);
     outb(vgaCRReg, saveCR43);

     return found;
}


static int BT485_SERIES_PreInit()
{
    /* Verify that depth is supported by ramdac */
	/* all are supported */

    /* Set cursor options */
   if (OFLG_ISSET(OPTION_BT485_CURS, &s3InfoRec.options)) {
	 ErrorF("%s %s: Using hardware cursor from Bt485/20C505 RAMDAC\n",
		XCONFIG_GIVEN, s3InfoRec.name);
   }
   
    /* Check if PixMux is supported and set the PixMux
	related flags and variables */
   if ( OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options) ||
	OFLG_ISSET(OPTION_NUMBER_NINE, &s3InfoRec.options) ||
	OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options) ||
	OFLG_ISSET(OPTION_SPEA_MERCURY, &s3InfoRec.options) ||
	S3_964_SERIES(s3ChipId) || S3_968_SERIES(s3ChipId)) {
      s3Bt485PixMux = TRUE;
      /* XXXX Are the defaults for the other parameters correct? */
      pixMuxPossible = TRUE;
      allowPixMuxInterlace = FALSE;	/* It doesn't work right (yet) */
      allowPixMuxSwitching = FALSE;	/* XXXX Is this right? */
      if (OFLG_ISSET(OPTION_SPEA_MERCURY, &s3InfoRec.options) &&
          S3_928_ONLY(s3ChipId)) {
	 nonMuxMaxClock = 67500;	/* Doubling only works in mux mode */
	 nonMuxMaxMemory = 1024;	/* Can't access more without mux */
	 allowPixMuxSwitching = FALSE;
	 pixMuxLimitedWidths = FALSE;
	 pixMuxMinWidth = 1024;
	 if (s3Bpp == 2) {
	    nonMuxMaxMemory = 0;	/* Only 2:1MUX works (yet)!     */
	    pixMuxMinWidth = 800;
	 } else if (s3Bpp==4) {
	    nonMuxMaxMemory = 0;
	    pixMuxMinWidth = 640;
	 }
      } else if (OFLG_ISSET(OPTION_NUMBER_NINE, &s3InfoRec.options)) {
	 nonMuxMaxClock = 67500;
	 allowPixMuxSwitching = TRUE;
	 pixMuxLimitedWidths = TRUE;
	 pixMuxMinWidth = 800;
      } else if (OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options)) {
	allowPixMuxSwitching = TRUE;
	pixMuxLimitedWidths = TRUE;
	/* For 8bpp mode, allow PIXMUX selection based on Clock and Width. */
	if (s3Bpp == 1) {
	  nonMuxMaxClock = 85000;
	  pixMuxMinWidth = 1024;
	} else {
	  /* For 16bpp and 32bpp modes, require PIXMUX. */
	  nonMuxMaxClock = 0;
	  pixMuxMinWidth = 0;
	}
      } else if (OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options)) {
	allowPixMuxSwitching = FALSE;
	pixMuxLimitedWidths = TRUE;
 	/* For 8bpp mode, allow PIXMUX selection based on Clock and Width. */
 	if (s3Bpp == 1) {
 	  nonMuxMaxClock = 85000;
 	  pixMuxMinWidth = 1024;
 	} else {
	  /* For 16bpp and 32bpp modes, require PIXMUX. */
	  nonMuxMaxClock = 0;
	  pixMuxMinWidth = 0;
 	}
      } else if (S3_964_SERIES(s3ChipId) || S3_968_SERIES(s3ChipId)) {
         nonMuxMaxClock = 0;  /* 964 can only be in pixmux mode when */
         pixMuxMinWidth = 0;  /* working in enhanced mode */  
	 pixMuxLimitedWidths = FALSE;
	 pixMuxNeeded = TRUE;
      } else {
	 nonMuxMaxClock = 85000;
      }

   }


    /* If there is an internal clock, set s3ClockSelectFunc, maxRawClock
	numClocks and whatever options need to be set. For external
	clocks, pass the job to OtherClocksSetup() */

   /* Diamond Stealth 64 VRAM uses an ICD2061A */
   if (!OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions) &&
       (s3BiosVendor == DIAMOND_BIOS) && S3_964_ONLY(s3ChipId)) {
      OFLG_SET(CLOCK_OPTION_ICD2061A, &s3InfoRec.clockOptions);
      OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
      clockchip_probed = XCONFIG_PROBED;
   }

   OtherClocksSetup();	/* setup clock */

    /* Make any necessary clock alterations due to multiplexing,
	clock doubling, etc...  s3Probe will do some last minute
	clock sanity checks when we return */

   if(s3RamdacType == BT485_DAC) {
      if (maxRawClock > 67500)
	 clockDoublingPossible = TRUE;
      /* These limits are based on the LCLK rating, and may be too high */
      if (s3Bt485PixMux && s3Bpp < 4)
	 s3InfoRec.maxClock = s3InfoRec.dacSpeeds[0];
      else {
	 if (s3InfoRec.dacSpeeds[0] < 150000)    /* 110 and 135 */
	    s3InfoRec.maxClock = 90000;
	 else				      /* 150 and 170 (if they exist) */
	    s3InfoRec.maxClock = 110000;
      }
   } else { /* ATT20C505_DAC */
      if (maxRawClock > 90000)
	 clockDoublingPossible = TRUE;
      /* These limits are based on the LCLK rating, and may be too high */
      if (s3Bt485PixMux && s3Bpp < 4)
	 s3InfoRec.maxClock = s3InfoRec.dacSpeeds[0];
      else {
	 if (s3InfoRec.dacSpeeds[0] < 110000)	  /* 85 */
	    s3InfoRec.maxClock = 85000;
	 else if (s3InfoRec.dacSpeeds[0] < 135000)	  /* 110 */
	    s3InfoRec.maxClock = 90000;
	 else					  /* 135, 150, 170 */
	    s3InfoRec.maxClock = 110000;
      }
   }

   return 1;
}

static void BT485_Restore()
{
      unsigned char tmp;

      /* Turn off parallel mode explicitly here */
      if (s3Bt485PixMux) {
         if (OFLG_ISSET(OPTION_SPEA_MERCURY, &s3InfoRec.options) &&
             S3_928_ONLY(s3ChipId))
	 {
	    outb(vgaCRIndex, 0x5C);
	    outb(vgaCRReg, 0x20);
	    outb(0x3C7, 0x00);
	    /* set s3 reg53 to non-parallel addressing by and'ing 0xDF     */
            outb(vgaCRIndex, 0x53);
            tmp = inb(vgaCRReg);
            outb(vgaCRReg, tmp & 0xDF);
	    outb(vgaCRIndex, 0x5C);
	    outb(vgaCRReg, 0x00);
         }
         if (OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options) &&
             S3_928_ONLY(s3ChipId))
	 {
	    outb(vgaCRIndex, 0x5C);
	    outb(vgaCRReg, 0x40); /* XXXXXXXXXXXXXXXXXXXXX */
	    outb(0x3C7, 0x00);
	    /* set s3 reg53 to non-parallel addressing by and'ing 0xDF     */
            outb(vgaCRIndex, 0x53);
            tmp = inb(vgaCRReg);
            outb(vgaCRReg, tmp & 0xDF);
         }
      }
	 
      s3OutBtReg(BT_COMMAND_REG_0, 0xFE, 0x01);
      s3OutBtRegCom3(0x00, s3DacRegs[3]);
      if (s3Bt485PixMux) {
	 s3OutBtReg(BT_COMMAND_REG_2, 0x00, s3DacRegs[2]);
	 s3OutBtReg(BT_COMMAND_REG_1, 0x00, s3DacRegs[1]);
      }
      s3OutBtReg(BT_COMMAND_REG_0, 0x00, s3DacRegs[0]);
 }

static void BT485_Save()
{         
	 s3DacRegs[0] = s3InBtReg(BT_COMMAND_REG_0);
	 if (s3Bt485PixMux) {
	    s3DacRegs[1] = s3InBtReg(BT_COMMAND_REG_1);
	    s3DacRegs[2] = s3InBtReg(BT_COMMAND_REG_2);
	 }
	 s3DacRegs[3] = s3InBtRegCom3();
}

static int BT485_Init(DisplayModePtr mode)
{
   register unsigned char tmp, tmp2 = 0;

   outb(0x3C4, 1);
   tmp = inb(0x3C5);
   outb(0x3C5, tmp | 0x20); /* blank the screen */
   s3OutBtReg(BT_COMMAND_REG_0, 0xFE, 0x01); /* sleep mode */
   
   if (s3Bt485PixMux) {
      if (s3PixelMultiplexing) {
         /* fun timing mods for pixel-multiplexing!  

	   Pixel Multiplexing is selected for 16bpp, 32bpp, or 8bpp
	   with Width > 1024.  Pixel Multiplexing requires we also
	   Select Parallel VRAM Addressing (CR53.5), and Parallel
	   VRAM Addressing also requires a line width of 1024 or
	   2048, external SID enabled (CR55.3), and split transfers
	   disabled (CR51.6).
	 */

	 if (OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options) ||
	     OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options)) {
	   outb(vgaCRIndex, 0x53);
	   tmp = inb(vgaCRReg);
	   outb(vgaCRReg, tmp | 0x20);
	 }

	 if (OFLG_ISSET(OPTION_SPEA_MERCURY, &s3InfoRec.options) &&
              S3_928_ONLY(s3ChipId))	
	 {
	    outb(vgaCRIndex, 0x5C);
	    outb(vgaCRReg, 0x20);
	    outb(0x3C7, 0x21);

            /* set s3 reg53 to parallel addressing by or'ing 0x20          */
            outb(vgaCRIndex, 0x53);
            tmp = inb(vgaCRReg);
            outb(vgaCRReg, tmp | 0x20);

	    outb(vgaCRIndex, 0x5C);
	    outb(vgaCRReg, 0x00);
	 }

         /* set s3 reg55 to external serial by or'ing 0x08              */
         outb(vgaCRIndex, 0x55);
         tmp = inb(vgaCRReg); /* XXXX Something should be masked here */
	 if (s3InfoRec.bitsPerPixel == 32 &&
	     !OFLG_ISSET(OPTION_MIRO_MAGIC_S4,&s3InfoRec.options))  
	/* 24bpp truecolor */
 	   tmp |= 0x48;
	 else
	   tmp |= 0x08;
         outb(vgaCRReg, tmp);
	 if (S3_964_SERIES(s3ChipId) || S3_968_SERIES(s3ChipId)) {
	    /* Stealth 64 and Miro Crystal 20SV */
	    outb(vgaCRIndex, 0x66);
	    tmp = inb(vgaCRReg) & 0xc0;
	    if (mode->Flags & V_DBLCLK) {
	       /* Set VCLK = DCLCK/2 */
	       /* And set up a 32 bit interleaved bus */
	       if (s3Bpp == 1)
		  tmp |= 0x11;
	       else
		  tmp |= 0x10; /* 16bpp */
            } else {
	       if (s3Bpp == 1)
		  tmp |= 0x12;
	       else if (s3Bpp == 2)
		  tmp |= 0x11;
	       else
		  tmp |= 0x10;	/* for 20SV, Stealth needs 0x10 ? */
            }
	    outb(vgaCRReg, tmp);

	    /* blank_delay = 0 (at least for Miro Crystal 20SV) */
	    outb(vgaCRIndex, 0x6d);
	    if ((mode->Flags & V_DBLCLK) || s3Bpp > 1) 
	       outb(vgaCRReg, 0);
	    else
	       outb(vgaCRReg, 1);  /* or 2; needed for 20SV with ATT 20C505 */
         }
	 outb(vgaCRIndex, 0x65);
	 tmp = inb(vgaCRReg);

         if (OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options))
	   /*
	     Setting this register non-zero on the Pegasus causes a wrap of
	     the rightmost pixels back to the left of the display.
	   */
	   outb(vgaCRReg, 0x00);
         else if (!(OFLG_ISSET(OPTION_SPEA_MERCURY, &s3InfoRec.options) &&
                    S3_928_ONLY(s3ChipId))) {
	    outb(vgaCRReg, tmp | 0x20);
 	    /* set s3 reg65 for some unknown reason                      */
	    /* Setting this for the SPEA Mercury affects clocks > 120MHz */
	  } else if (OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options)) {
	     /* do nothing */ ;
	  } else if ((s3DisplayWidth >= 1024) || (s3InfoRec.depth == 24)
		     || (s3InfoRec.depth == 32)) {
#ifndef PC98_PW
	    outb(vgaCRReg, tmp | 0x40);
#else
	    outb(vgaCRReg, tmp | 0x08);
#endif
	    /* remove horizontal stripes in 1600/8bpp and 1152/16bpp      */
	    /* 800/32bpp linewidth pixmux modes                           */
	    /* someone should check this for other 928 + Bt485 cards      */
	  } else outb(vgaCRReg, tmp & 0xBF);

         /*
          * set output clocking to 4:1 multiplexing
          */
	 if (s3InfoRec.depth == 24 || s3InfoRec.depth == 32)  /* 24bpp */
	    tmp = 0x10;
	 else if (s3InfoRec.depth == 16)               /* 5-6-5 */
	    tmp = 0x38;
	 else if (s3InfoRec.depth == 15)               /* 5-5-5 */
	    tmp = 0x30;
	 else
	    tmp = 0x40;                                /* 8bpp */
         s3OutBtReg(BT_COMMAND_REG_1, 0x00, tmp);

	 /* SCLK enable,pclk1,pixport	                           */
	 if (mode->Flags & V_INTERLACE)
	    s3OutBtReg(BT_COMMAND_REG_2, 0x00, 0x30 | 0x08);
	 else
	    s3OutBtReg(BT_COMMAND_REG_2, 0x00, 0x30);

      } else {

	 if (OFLG_ISSET(OPTION_SPEA_MERCURY, &s3InfoRec.options) &&
             S3_928_ONLY(s3ChipId))
	 {
	    outb(vgaCRIndex, 0x5C);
	    outb(vgaCRReg, 0x20);
	    outb(0x3C7, 0x00);
	 }
	 if (OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options) &&
             S3_928_ONLY(s3ChipId))
	 {
	    outb(vgaCRIndex, 0x5C);
	    outb(vgaCRReg, 0x20);
	    outb(0x3C7, 0x00);
	 }

         /* set s3 reg53 to non-parallel addressing by and'ing 0xDF     */
         outb(vgaCRIndex, 0x53);
         tmp = inb(vgaCRReg);
	 if (OFLG_ISSET(OPTION_SPEA_MERCURY, &s3InfoRec.options) && 
	     S3_928_ONLY(s3ChipId) && (s3Bpp != 1)) {
            outb(vgaCRReg, tmp | 0x20);
	 } else {
            outb(vgaCRReg, tmp & 0xDF);
	 }

         /* set s3 reg65 for some unknown reason                        */
         outb(vgaCRIndex, 0x65);
         tmp = inb(vgaCRReg);
         outb(vgaCRReg, tmp & 0xDF);

	 if (OFLG_ISSET(OPTION_SPEA_MERCURY, &s3InfoRec.options) &&
             S3_928_ONLY(s3ChipId))
	 {
	    outb(vgaCRIndex, 0x5C);
	    outb(vgaCRReg, 0x00);
	 }

	 if (OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options))
	 {
 	    outb(vgaCRIndex, 0x5C);
	    outb(vgaCRReg, 0x00);
	    outb(vgaCRIndex, 0x55);
	    outb(vgaCRReg, 0x20);
	 } else {
	   /* set s3 reg55 to non-external serial by and'ing 0xF7         */
	   outb(vgaCRIndex, 0x55);
	   tmp = inb(vgaCRReg);
	   outb(vgaCRReg, tmp & 0xF7);
	 }
	 
	 if (s3InfoRec.depth == 24 || s3InfoRec.depth == 32)	/* 24bpp */
	    tmp = 0x10;
	 else if (s3InfoRec.depth == 16)		/* 5-6-5 */
	    tmp = 0x3c;					/* 1:1 MUX */
	 else if (s3InfoRec.depth == 15)		/* 5-5-5 */
	    tmp = 0x34;					/* 1:1 MUX */
	 else
	    tmp = 0x00;
         s3OutBtReg(BT_COMMAND_REG_1, 0x00, tmp);

	 if (s3InfoRec.bitsPerPixel > 8)
	    tmp = 0x30;
	 else
	    tmp = 0x10;

	 /* pclk1,vgaport                                               */
	 if (mode->Flags & V_INTERLACE)
	    s3OutBtReg(BT_COMMAND_REG_2, 0x00, tmp | 0x08);
	 else
	    s3OutBtReg(BT_COMMAND_REG_2, 0x00, tmp);

      }  /* end of s3PixelMultiplexing */
   }

   /* Set 6/8 bit mode and sync-on-green if required */
      s3OutBtReg(BT_COMMAND_REG_0, 0x00, 0x01 |
		 (s3DAC8Bit ? 0x02 : 0) | (s3DACSyncOnGreen ? 0x08 : 0x00));
#ifdef CLOCKDEBUG
      if (mode->Flags & V_DBLCLK) {
	 ErrorF("Setting clock doubler in s3Init(), freq = %.3f\n",
		s3InfoRec.clock[mode->Clock] / 1000.0);
      }
#endif
      /* Use Bt485 clock doubler - Bit 3 of Command Reg 3 */
      s3OutBtRegCom3(0xF7, (mode->Flags & V_DBLCLK ? 0x08 : 0x00));
      s3OutBtReg(BT_COMMAND_REG_0, 0xFE, 0x00); /* wake up    */
      outb(0x3C4, 1);
      outb(0x3C5, tmp2); /* unblank the screen */
	
      return 1;
}

/************************************************************\

			ATT20C505_DAC

\************************************************************/

static Bool ATT20C505_Probe()
{
    Bool found = FALSE;
    unsigned char tmp,tmp2;

    /*quick check*/
    if (!S3_928_ONLY(s3ChipId) && !S3_964_SERIES(s3ChipId))
	return FALSE;

    tmp = inb(0x3C6);
    outb(0x3C6, 0x0F);
    if (((tmp2 = s3InBtStatReg()) & 0x80) == 0x80) {
          /*
           * Found either a BrookTree Bt485 or AT&T 20C505.
           */
          if ((tmp2 & 0xF0) == 0xD0) {
             found = TRUE;
             ErrorF("%s %s: Detected an AT&T 20C505 RAMDAC\n",
                    XCONFIG_PROBED, s3InfoRec.name);
          }
    }
    outb(0x3C6, tmp);
    return found;
}


/*******************************************************\
 
		TI3020_DAC  
		TI3025_DAC 

\*******************************************************/

static Bool TI3020_3025_Probe(int type)
{
    int found = 0;
    unsigned char saveCR55, saveCR45, saveCR43, saveCR5C;
    unsigned char saveTIndx,saveTIndx2,saveTIdata;

    outb(vgaCRIndex, 0x43);
    saveCR43 = inb(vgaCRReg);
    outb(vgaCRReg, saveCR43 & ~0x02);

    outb(vgaCRIndex, 0x45);
    saveCR45 = inb(vgaCRReg);
    outb(vgaCRReg, saveCR45 & ~0x20);

    outb(vgaCRIndex, 0x55);
    saveCR55 = inb(vgaCRReg);
	 
    /* toggle to upper 4 direct registers */
    outb(vgaCRReg, (saveCR55 & 0xFC) | 0x01);

    saveTIndx = inb(TI_INDEX_REG);
    outb(TI_INDEX_REG, TI_ID);
    if (inb(TI_DATA_REG) == TI_VIEWPOINT20_ID) {
	    /*
	     * Found TI ViewPoint 3020 DAC
	     */
	    found = TI3020_DAC;
	    saveCR43 &= ~0x02;
	    saveCR45 &= ~0x20;
    } else {
	    outb(vgaCRIndex, 0x5C);
	    saveCR5C = inb(vgaCRReg);
	    /* clear 0x20 (RS4) for 3020 mode */
	    outb(vgaCRReg, saveCR5C & 0xDF);
	    saveTIndx2 = inb(TI_INDEX_REG);
	    /* already twiddled CR55 above */
	    outb(TI_INDEX_REG, TI_CURS_CONTROL);
	    saveTIdata = inb(TI_DATA_REG);
	    /* clear TI_PLANAR_ACCESS bit */
	    outb(TI_DATA_REG, saveTIdata & 0x7F);

	    outb(TI_INDEX_REG, TI_ID);
	    if (inb(TI_DATA_REG) == TI_VIEWPOINT25_ID) {
	       /*
	        * Found TI ViewPoint 3025 DAC
	        */
	       found = TI3025_DAC;
	       saveCR43 &= ~0x02;
	       saveCR45 &= ~0x20;
	    }

	    /* restore this mess */
	    outb(TI_INDEX_REG, TI_CURS_CONTROL);
	    outb(TI_DATA_REG, saveTIdata);
	    outb(TI_INDEX_REG, saveTIndx2);
	    outb(vgaCRIndex, 0x5C);
	    outb(vgaCRReg, saveCR5C);
    }

    outb(TI_INDEX_REG, saveTIndx);
    outb(vgaCRIndex, 0x55);
    outb(vgaCRReg, saveCR55);

    outb(vgaCRIndex, 0x45);
    outb(vgaCRReg, saveCR45);

    outb(vgaCRIndex, 0x43);
    outb(vgaCRReg, saveCR43);

    return (found == type);
}

static Bool TI3020_Probe()
{
  if (!S3_928_ONLY(s3ChipId) && !S3_964_SERIES(s3ChipId))
	return FALSE;
  else if (TI3020_3025_Probe(TI3020_DAC)) {
	ErrorF("%s %s: Detected a TI ViewPoint 3020 RAMDAC\n",
	           XCONFIG_PROBED, s3InfoRec.name);
	return TRUE;
  } else return FALSE;	
}

static Bool TI3025_Probe()
{
  if (!S3_964_SERIES(s3ChipId))
	return FALSE;
  else if(TI3020_3025_Probe(TI3025_DAC)) {
	ErrorF("%s %s: Detected a TI ViewPoint 3025 RAMDAC\n",
	            XCONFIG_PROBED, s3InfoRec.name);
	return TRUE;
  } else return FALSE;
}

static int TI3020_3025_PreInit()
{
    /* Verify that depth is supported by ramdac */
	/* all are supported */

    /* Set cursor options */
    if (OFLG_ISSET(OPTION_NO_TI3020_CURS, &s3InfoRec.options)) {
         ErrorF("%s %s: Use of Ti3020/25 cursor disabled in XF86Config\n",
	        XCONFIG_GIVEN, s3InfoRec.name);
	 OFLG_CLR(OPTION_TI3020_CURS, &s3InfoRec.options);
    } else {
	 /* use the ramdac cursor by default */
	 ErrorF("%s %s: Using hardware cursor from Ti3020/25 RAMDAC\n",
	        OFLG_ISSET(OPTION_TI3020_CURS, &s3InfoRec.options) ?
		XCONFIG_GIVEN : XCONFIG_PROBED, s3InfoRec.name);
	 OFLG_SET(OPTION_TI3020_CURS, &s3InfoRec.options);
    }


    /* Check if PixMux is supported and set the PixMux
	related flags and variables */
    pixMuxPossible = TRUE;
    allowPixMuxInterlace = FALSE;
    allowPixMuxSwitching = FALSE;
    nonMuxMaxClock = 70000;
    pixMuxLimitedWidths = FALSE;
    if (S3_964_SERIES(s3ChipId)) {
         nonMuxMaxClock = 0;  /* 964 can only be in pixmux mode when */
         pixMuxMinWidth = 0;  /* working in enhanced mode */  
    }

    /* If there is an internal clock, set s3ClockSelectFunc, maxRawClock
	numClocks and whatever options need to be set. For external
	clocks, pass the job to OtherClocksSetup() */

  if (DAC_IS_TI3025 && 
       !OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions)) {
      OFLG_SET(CLOCK_OPTION_TI3025, &s3InfoRec.clockOptions);
      OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
      clockchip_probed = XCONFIG_PROBED;
   }
   if (OFLG_ISSET(CLOCK_OPTION_TI3025, &s3InfoRec.clockOptions)) {
        int mclk, m, n, p, mcc, cr5c;

        s3ClockSelectFunc = ti3025ClockSelect;
      	numClocks = 3;
        maxRawClock = s3InfoRec.dacSpeeds[0]; 

      	outb(vgaCRIndex, 0x5c);
      	cr5c = inb(vgaCRReg);
      	outb(vgaCRReg, cr5c & 0xdf);     /* clear RS4 - use 3020 mode */

      	s3OutTiIndReg(TI_PLL_CONTROL, 0x00, 0x00);
      	n = s3InTiIndReg(TI_MCLK_PLL_DATA) & 0x7f;
      	s3OutTiIndReg(TI_PLL_CONTROL, 0x00, 0x01);
      	m = s3InTiIndReg(TI_MCLK_PLL_DATA) & 0x7f;
      	s3OutTiIndReg(TI_PLL_CONTROL, 0x00, 0x02);
      	p = s3InTiIndReg(TI_MCLK_PLL_DATA) & 0x03;
      	mcc = s3InTiIndReg(TI_MCLK_DCLK_CONTROL);
      	if (mcc & 0x08) 
	 	mcc = (mcc & 0x07) * 2 + 2;
      	else 
	 	mcc = 1;
      	mclk = ((1431818 * ((m+2) * 8)) / (n+2) / (1 << p) / mcc + 50) / 100;
      	if (xf86Verbose)
	 ErrorF("%s %s: Using TI 3025 programmable clock (MCLK %1.3f MHz)\n",
		clockchip_probed, s3InfoRec.name, mclk / 1000.0);
      	if (OFLG_ISSET(OPTION_NUMBER_NINE, &s3InfoRec.options)) {
	   mclk = 55000;
	   if (xf86Verbose)
	    ErrorF("%s %s: Setting MCLK to %1.3f MHz for #9GXE64 Pro\n",
		   XCONFIG_PROBED, s3InfoRec.name, mclk / 1000.0);
	   Ti3025SetClock(2 * mclk, TI_MCLK_PLL_DATA, s3ProgramTi3025Clock);
	   mcc = s3InTiIndReg(TI_MCLK_DCLK_CONTROL);
	   s3OutTiIndReg(TI_MCLK_DCLK_CONTROL,0x00, (mcc & 0xf0) | 0x08);
      	}
        if (!s3InfoRec.s3MClk)
	 	s3InfoRec.s3MClk = mclk;
      	outb(vgaCRIndex, 0x5c);
      	outb(vgaCRReg, cr5c);

   }	/* Ti3020 will have an external clock */
   else 
      OtherClocksSetup();	/* external clocks */

    /* Make any necessary clock alterations due to multiplexing,
	clock doubling, etc...  s3Probe will do some last minute
	clock sanity checks when we return */
   clockDoublingPossible = TRUE;
   s3InfoRec.maxClock = s3InfoRec.dacSpeeds[0];

   return 1;
}


static void TI3020_3025_Restore()
{
   s3OutTiIndReg(TI_MUX_CONTROL_1, 0x00, s3DacRegs[TI_MUX_CONTROL_1]);
   s3OutTiIndReg(TI_MUX_CONTROL_2, 0x00, s3DacRegs[TI_MUX_CONTROL_2]);
   s3OutTiIndReg(TI_INPUT_CLOCK_SELECT, 0x00,
		    s3DacRegs[TI_INPUT_CLOCK_SELECT]);
   s3OutTiIndReg(TI_OUTPUT_CLOCK_SELECT, 0x00,
		    s3DacRegs[TI_OUTPUT_CLOCK_SELECT]);
   s3OutTiIndReg(TI_GENERAL_CONTROL, 0x00,
		    s3DacRegs[TI_GENERAL_CONTROL]);
   s3OutTiIndReg(TI_AUXILIARY_CONTROL, 0x00,
		    s3DacRegs[TI_AUXILIARY_CONTROL]);
   s3OutTiIndReg(TI_GENERAL_IO_CONTROL, 0x00, 0x1f);
   s3OutTiIndReg(TI_GENERAL_IO_DATA, 0x00,
		    s3DacRegs[TI_GENERAL_IO_DATA]);
   if (DAC_IS_TI3025) {
     s3OutTiIndReg(TI_PLL_CONTROL, 0x00, 0x00);
     s3OutTiIndReg(TI_PIXEL_CLOCK_PLL_DATA, 0x00, s3DacRegs[0 + 0x40]);  /* N */
     s3OutTiIndReg(TI_PIXEL_CLOCK_PLL_DATA, 0x00, s3DacRegs[1 + 0x40]);  /* M */
     s3OutTiIndReg(TI_PIXEL_CLOCK_PLL_DATA, 0x00, s3DacRegs[2 + 0x40]);  /* P */

     s3OutTiIndReg(TI_MCLK_PLL_DATA, 0x00, s3DacRegs[3 + 0x40]);         /* N */
     s3OutTiIndReg(TI_MCLK_PLL_DATA, 0x00, s3DacRegs[4 + 0x40]);         /* M */
     s3OutTiIndReg(TI_MCLK_PLL_DATA, 0x00, s3DacRegs[5 + 0x40] | 0x80 ); /* P */

     s3OutTiIndReg(TI_LOOP_CLOCK_PLL_DATA, 0x00, s3DacRegs[6 + 0x40]);  /* N */
     s3OutTiIndReg(TI_LOOP_CLOCK_PLL_DATA, 0x00, s3DacRegs[7 + 0x40]);  /* M */
     s3OutTiIndReg(TI_LOOP_CLOCK_PLL_DATA, 0x00, s3DacRegs[8 + 0x40]);  /* P */

     s3OutTiIndReg(TI_TRUE_COLOR_CONTROL, 0x00,
                    s3DacRegs[TI_TRUE_COLOR_CONTROL]);
     s3OutTiIndReg(TI_MISC_CONTROL, 0x00, s3DacRegs[TI_MISC_CONTROL]);
   }
   
   s3OutTiIndReg(TI_CURS_CONTROL, 0x00, s3DacRegs[TI_CURS_CONTROL]);
}


static void TI3020_3025_Save()
{
      if (DAC_IS_TI3025) {
          unsigned char CR5C;

          /* switch the ramdac from bt485 to ti3020 mode clearing RS4 */
	  outb(vgaCRIndex, 0x5C);
	  CR5C = inb(vgaCRReg);
	  outb(vgaCRReg, CR5C & 0xDF);

          s3DacRegs[TI_CURS_CONTROL] = s3InTiIndReg(TI_CURS_CONTROL);
          /* clear TI_PLANAR_ACCESS bit */
	  s3OutTiIndReg(TI_CURS_CONTROL, 0x7F, 0x00);
      }

      s3DacRegs[TI_MUX_CONTROL_1] = s3InTiIndReg(TI_MUX_CONTROL_1);
      s3DacRegs[TI_MUX_CONTROL_2] = s3InTiIndReg(TI_MUX_CONTROL_2);
      s3DacRegs[TI_INPUT_CLOCK_SELECT] =
                 s3InTiIndReg(TI_INPUT_CLOCK_SELECT);
      s3DacRegs[TI_OUTPUT_CLOCK_SELECT] =
                 s3InTiIndReg(TI_OUTPUT_CLOCK_SELECT);
      s3DacRegs[TI_GENERAL_CONTROL] = s3InTiIndReg(TI_GENERAL_CONTROL);
      s3DacRegs[TI_AUXILIARY_CONTROL] =
		 s3InTiIndReg(TI_AUXILIARY_CONTROL);
      s3OutTiIndReg(TI_GENERAL_IO_CONTROL, 0x00, 0x1f);
      s3DacRegs[TI_GENERAL_IO_DATA] = s3InTiIndReg(TI_GENERAL_IO_DATA);

      if (DAC_IS_TI3025) {
          s3DacRegs[TI_TRUE_COLOR_CONTROL] =
                 s3InTiIndReg(TI_TRUE_COLOR_CONTROL);
          s3DacRegs[TI_MISC_CONTROL] = s3InTiIndReg(TI_MISC_CONTROL);

          s3OutTiIndReg(TI_PLL_CONTROL, 0x00, 0x00);
          s3DacRegs[0 + 0x40] = s3InTiIndReg(TI_PIXEL_CLOCK_PLL_DATA);  /* N */
          s3OutTiIndReg(TI_PIXEL_CLOCK_PLL_DATA, 0x00, s3DacRegs[0 + 0x40]);
          s3DacRegs[1 + 0x40] = s3InTiIndReg(TI_PIXEL_CLOCK_PLL_DATA);  /* M */
          s3OutTiIndReg(TI_PIXEL_CLOCK_PLL_DATA, 0x00, s3DacRegs[1 + 0x40]);
          s3DacRegs[2 + 0x40] = s3InTiIndReg(TI_PIXEL_CLOCK_PLL_DATA);  /* P */
          s3OutTiIndReg(TI_PIXEL_CLOCK_PLL_DATA, 0x00, s3DacRegs[2 + 0x40]);

          s3DacRegs[3 + 0x40] = s3InTiIndReg(TI_MCLK_PLL_DATA);  /* N */
          s3OutTiIndReg(TI_MCLK_PLL_DATA, 0x00, s3DacRegs[3 + 0x40]);
          s3DacRegs[4 + 0x40] = s3InTiIndReg(TI_MCLK_PLL_DATA);  /* M */
          s3OutTiIndReg(TI_MCLK_PLL_DATA, 0x00, s3DacRegs[4 + 0x40]);
          s3DacRegs[5 + 0x40] = s3InTiIndReg(TI_MCLK_PLL_DATA);  /* P */
          s3OutTiIndReg(TI_MCLK_PLL_DATA, 0x00, s3DacRegs[5 + 0x40]);

          s3DacRegs[6 + 0x40] = s3InTiIndReg(TI_LOOP_CLOCK_PLL_DATA);  /* N */
          s3OutTiIndReg(TI_LOOP_CLOCK_PLL_DATA, 0x00, s3DacRegs[6 + 0x40]);
          s3DacRegs[7 + 0x40] = s3InTiIndReg(TI_LOOP_CLOCK_PLL_DATA);  /* M */
          s3OutTiIndReg(TI_LOOP_CLOCK_PLL_DATA, 0x00, s3DacRegs[7 + 0x40]);
          s3DacRegs[8 + 0x40] = s3InTiIndReg(TI_LOOP_CLOCK_PLL_DATA);  /* P */
          s3OutTiIndReg(TI_LOOP_CLOCK_PLL_DATA, 0x00, s3DacRegs[8 + 0x40]);

      }
}

static int TI3020_3025_Init(DisplayModePtr mode)
{
      unsigned char tmp, tmp1, tmp2;

      outb(0x3C4, 1);
      tmp2 = inb(0x3C5);
      outb(0x3C5, tmp2 | 0x20); /* blank the screen */

      /* change polarity on S3 to pass through control to the 3020      */
      tmp = new->MiscOutReg; /* BUG tmp = inb(0x3CC); */
      new->MiscOutReg |= 0xC0;
      tmp1 = 0x00;
      if (!(tmp & 0x80)) tmp1 |= 0x02; /* invert bits for the 3020      */
      if (!(tmp & 0x40)) tmp1 |= 0x01;
      if (s3DACSyncOnGreen) tmp1 |= 0x20;  /* add IOG sync              */
      s3OutTiIndReg(TI_GENERAL_CONTROL, 0x00, tmp1);
      s3OutTiIndReg(TI_TRUE_COLOR_CONTROL, 0x00, 0x00);

      if (DAC_IS_TI3020) {
	 /* the 3025 clock programming code sets the input clock select */
         if (mode->Flags & V_DBLCLK)
	    s3OutTiIndReg(TI_INPUT_CLOCK_SELECT, 0x00, TI_ICLK_CLK1_DOUBLE);
         else
	    s3OutTiIndReg(TI_INPUT_CLOCK_SELECT, 0x00, TI_ICLK_CLK1);
      }

      outb(vgaCRIndex, 0x65);
      if (DAC_IS_TI3025) {
	 if (OFLG_ISSET(OPTION_NUMBER_NINE,&s3InfoRec.options)) {
	    outb(vgaCRReg, 0x82);
	 } else {
	    outb(vgaCRReg, 0);
	 }
      } else {
	 /* set s3 reg65 for some unknown reason			*/
	 if (s3InfoRec.bitsPerPixel == 32)
	    outb(vgaCRReg, 0x80);
	 else if (s3InfoRec.bitsPerPixel == 16)
	    outb(vgaCRReg, 0x40);
	 else
	    outb(vgaCRReg, 0x00);
      }

      if (s3PixelMultiplexing) {
	 /* fun timing mods for pixel-multiplexing!                     */

	 if (OFLG_ISSET(OPTION_ELSA_W2000PRO,&s3InfoRec.options)) {
	    /* set CR40 acording to Bernhard Bender */
	    outb(vgaCRIndex, 0x40);
	    outb(vgaCRReg, 0xd1);
	 } else if (DAC_IS_TI3025) {
	    outb(vgaCRIndex, 0x40);
	    outb(vgaCRReg, 0x11);
	    outb(vgaCRIndex, 0x55);
	    outb(vgaCRReg, 0x00);
	 } else {
            /* set s3 reg53 to parallel addressing by or'ing 0x20          */
            outb(vgaCRIndex, 0x53);
            tmp = inb(vgaCRReg);
            outb(vgaCRReg, tmp | 0x20);

            /* set s3 reg55 to external serial by or'ing 0x08              */
            outb(vgaCRIndex, 0x55);
            tmp = inb(vgaCRReg);
            outb(vgaCRReg, tmp | 0x08);
	 }
	 /* the input clock is already set to clk1 or clk1double (s3.c) */

	 if (DAC_IS_TI3025) {
	    if (s3InfoRec.bitsPerPixel > 8)
	       s3OutTiIndReg(TI_AUXILIARY_CONTROL, 0, 0x00);
            else
	       s3OutTiIndReg(TI_AUXILIARY_CONTROL, 0, TI_AUX_W_CMPL);
         } else {
	    /* set aux control to self clocked, window function complement */
	    if (s3InfoRec.bitsPerPixel > 8)
	       s3OutTiIndReg(TI_AUXILIARY_CONTROL, 0, TI_AUX_SELF_CLOCK);
            else
	       s3OutTiIndReg(TI_AUXILIARY_CONTROL, 0,
		          TI_AUX_SELF_CLOCK | TI_AUX_W_CMPL);
         }
	 if (OFLG_ISSET(OPTION_ELSA_W2000PRO,&s3InfoRec.options)) {
   	    int vclock,rclock;

	    /*
	     * The 964 needs different VCLK division depending on the 
	     * clock frequenzy used. VCLK/1 for 0-60MHz, VCLK/2 for
	     * 60-120MHz and VCLK/4 for 120-175MHz (or -200MHz, depending
	     * on the RAMDAC actually used)
	     * the RCLK output is tied to the LCLK input which is the same
	     * as SCLK but with no blanking.  SCLK is the actual pixel
	     * shift clock for the pixel bus.
	     * RCLK/8 is used because of the 8:1 pixel-multiplexing below.
	     * (964 uses always 8:1 in 256 color modes)
	     */
	    if (s3InfoRec.clock[mode->Clock] > 120000) {
	       vclock = TI_OCLK_V4;
	    } else if (s3InfoRec.clock[mode->Clock] > 60000){
	       vclock = TI_OCLK_V2;
            } else {
	       vclock = TI_OCLK_V1;
            }
	    if (s3InfoRec.bitsPerPixel == 32) {           /* 24bpp */
               rclock = TI_OCLK_R2;
            } else if ((s3InfoRec.bitsPerPixel == 16) ||
                       (s3InfoRec.bitsPerPixel == 15)) {  /* 15/16bpp */
               rclock = TI_OCLK_R4;
            } else {
               rclock = TI_OCLK_R8;
            }
            s3OutTiIndReg(TI_OUTPUT_CLOCK_SELECT, 0x00,
			  TI_OCLK_S | vclock | rclock);
            outb(vgaCRIndex, 0x66);
            tmp = inb(vgaCRReg);
            outb(vgaCRReg, (tmp & 0xf8) | ((rclock - (vclock >> 3)) & 7));
	 } else if (DAC_IS_TI3025) {
	    if (s3InfoRec.bitsPerPixel == 32) {           /* 24bpp */
	       s3OutTiIndReg(TI_OUTPUT_CLOCK_SELECT, 0x00, TI_OCLK_S_V2_R2);
	       outb(vgaCRIndex, 0x66);
	       tmp = inb(vgaCRReg);
               if (mode->Flags & V_DBLCLK)
	          outb(vgaCRReg, (tmp & 0xf8) | 0x00);
               else
	          outb(vgaCRReg, (tmp & 0xf8) | 0x01);
	    } else if (s3InfoRec.bitsPerPixel == 16) {      /* 5-6-5 */
	       s3OutTiIndReg(TI_OUTPUT_CLOCK_SELECT, 0x00, TI_OCLK_S_V2_R4);
	       outb(vgaCRIndex, 0x66);
	       tmp = inb(vgaCRReg);
               if (mode->Flags & V_DBLCLK)
	          outb(vgaCRReg, (tmp & 0xf8) | 0x01);
               else
	          outb(vgaCRReg, (tmp & 0xf8) | 0x02);
	    } else {
	       s3OutTiIndReg(TI_OUTPUT_CLOCK_SELECT, 0x00, TI_OCLK_S_V2_R8);
	       outb(vgaCRIndex, 0x66);
	       tmp = inb(vgaCRReg);
               if (mode->Flags & V_DBLCLK)
	          outb(vgaCRReg, (tmp & 0xf8) | 0x02);
               else
	          outb(vgaCRReg, (tmp & 0xf8) | 0x03);
	    }
	 } else {
	    /*
	     * for all other boards with Ti3020 (only #9 level 14/16 ?)
	     * set output clocking to VCLK/4, RCLK/8 like the fixed Bt485.
	     * RCLK/8 is used because of the 8:1 pixel-multiplexing below.
	     */
	    if (s3InfoRec.bitsPerPixel == 32) {           /* 24bpp */
	       s3OutTiIndReg(TI_OUTPUT_CLOCK_SELECT, 0x00, TI_OCLK_S_V1_R2);
	    } else if (s3InfoRec.bitsPerPixel == 16) {      /* 5-6-5 */
	       s3OutTiIndReg(TI_OUTPUT_CLOCK_SELECT, 0x00, TI_OCLK_S_V2_R4);
	    } else {
	       s3OutTiIndReg(TI_OUTPUT_CLOCK_SELECT, 0x00, TI_OCLK_S_V4_R8);
	    }
	 }

         /*
          * set the serial access mode 256 words control
          */
         outb(vgaCRIndex, 0x58);
         tmp = inb(vgaCRReg);
         outb(vgaCRReg, (tmp & 0xbf) | s3SAM256);

	 if (s3InfoRec.depth == 24 || s3InfoRec.depth == 32) {  /* 24bpp */
            if (DAC_IS_TI3025) {
               s3OutTiIndReg(TI_MUX_CONTROL_1, 0x00, TI_MUX1_3025T_888);
               s3OutTiIndReg(TI_MUX_CONTROL_2, 0x00, TI_MUX2_BUS_TC_D24P64);
               s3OutTiIndReg(TI_COLOR_KEY_CONTROL, 0x00, 0x01);
            } else {
               s3OutTiIndReg(TI_MUX_CONTROL_1, 0x00, TI_MUX1_DIRECT_888);
               s3OutTiIndReg(TI_MUX_CONTROL_2, 0x00, TI_MUX2_BUS_DC_D24P64);
               s3OutTiIndReg(TI_COLOR_KEY_CONTROL, 0x00, 0x00);
            }
	 } else if (s3InfoRec.depth == 16) {                    /* 5-6-5 */
            if (DAC_IS_TI3025) {
               s3OutTiIndReg(TI_MUX_CONTROL_1, 0x00, TI_MUX1_3025T_565);
               s3OutTiIndReg(TI_MUX_CONTROL_2, 0x00, TI_MUX2_BUS_TC_D16P64);
               s3OutTiIndReg(TI_COLOR_KEY_CONTROL, 0x00, 0x01);
            } else {
               s3OutTiIndReg(TI_MUX_CONTROL_1, 0x00, TI_MUX1_DIRECT_565);
               s3OutTiIndReg(TI_MUX_CONTROL_2, 0x00, TI_MUX2_BUS_DC_D16P64);
               s3OutTiIndReg(TI_COLOR_KEY_CONTROL, 0x00, 0x00);
            }
	 } else if (s3InfoRec.depth == 15) {                     /* 5-5-5 */
            if (DAC_IS_TI3025) {
               s3OutTiIndReg(TI_MUX_CONTROL_1, 0x00, TI_MUX1_3025T_555);
               s3OutTiIndReg(TI_MUX_CONTROL_2, 0x00, TI_MUX2_BUS_TC_D15P64);
               s3OutTiIndReg(TI_COLOR_KEY_CONTROL, 0x00, 0x01);
            } else {
               s3OutTiIndReg(TI_MUX_CONTROL_1, 0x00, TI_MUX1_DIRECT_555);
               s3OutTiIndReg(TI_MUX_CONTROL_2, 0x00, TI_MUX2_BUS_DC_D15P64);
               s3OutTiIndReg(TI_COLOR_KEY_CONTROL, 0x00, 0x00);
            }
	 } else {
            /* set mux control 1 and 2 to provide pseudocolor sub-mode 4   */
            /* this provides a 64-bit pixel bus with 8:1 multiplexing      */
            s3OutTiIndReg(TI_MUX_CONTROL_1, 0x00, TI_MUX1_PSEUDO_COLOR);
            s3OutTiIndReg(TI_MUX_CONTROL_2, 0x00, TI_MUX2_BUS_PC_D8P64);
	 }
         /* change to 8-bit DAC and re-route the data path and clocking */
         s3OutTiIndReg(TI_GENERAL_IO_CONTROL, 0x00, TI_GIC_ALL_BITS);
         if (s3DAC8Bit) {
            if (DAC_IS_TI3025) {
               s3OutTiIndReg(TI_GENERAL_IO_DATA , 0x00, TI_GID_N9_964);
               s3OutTiIndReg(TI_GENERAL_IO_CONTROL, 0x00, 0x00);
               s3OutTiIndReg(TI_MISC_CONTROL , 0xF0,
                             TI_MC_INT_6_8_CONTROL | TI_MC_8_BPP);
            } else if(OFLG_ISSET(OPTION_ELSA_W2000PRO,&s3InfoRec.options)) 
               s3OutTiIndReg(TI_GENERAL_IO_DATA , 0x00 , TI_GID_W2000_8BIT);
            else
               s3OutTiIndReg(TI_GENERAL_IO_DATA, 0x00, TI_GID_TI_DAC_8BIT);
         } else {
            if (DAC_IS_TI3025) {
               s3OutTiIndReg(TI_GENERAL_IO_DATA , 0x00, TI_GID_N9_964);
               s3OutTiIndReg(TI_GENERAL_IO_CONTROL, 0x00, 0x00);
               s3OutTiIndReg(TI_MISC_CONTROL , 0xF0, TI_MC_INT_6_8_CONTROL);
            } else if(OFLG_ISSET(OPTION_ELSA_W2000PRO,&s3InfoRec.options)) 
	       s3OutTiIndReg( TI_GENERAL_IO_DATA , 0x00 , TI_GID_W2000_6BIT );
            else
               s3OutTiIndReg(TI_GENERAL_IO_DATA, 0x00, TI_GID_TI_DAC_6BIT);
         }
         if (DAC_IS_TI3025) {
	    outb(vgaCRIndex, 0x6D);
	    if (s3Bpp == 1)
	       if (mode->Flags & V_DBLCLK) 
		  outb(vgaCRReg, 0x02);
	       else
		  outb(vgaCRReg, 0x03);
	    else if (s3Bpp == 2)
	       if (mode->Flags & V_DBLCLK) 
		  outb(vgaCRReg, 0x00);
	       else
		  outb(vgaCRReg, 0x01);
	    else /* (s3Bpp == 4) */
	       outb(vgaCRReg, 0x00);
	 }
      } else {
         /* set s3 reg53 to non-parallel addressing by and'ing 0xDF     */
         outb(vgaCRIndex, 0x53);
         tmp = inb(vgaCRReg);
         outb(vgaCRReg, tmp & 0xDF);

         /* set s3 reg55 to non-external serial by and'ing 0xF7         */
         outb(vgaCRIndex, 0x55);
         tmp = inb(vgaCRReg);
         outb(vgaCRReg, tmp & 0xF7);

         /* the input clock is already set to clk1 or clk1double (s3.c) */

	 if (DAC_IS_TI3025) {
	    if (s3InfoRec.bitsPerPixel > 8)
	       s3OutTiIndReg(TI_AUXILIARY_CONTROL, 0, 0);
	    else
	       s3OutTiIndReg(TI_AUXILIARY_CONTROL, 0, TI_AUX_W_CMPL);
	 }
	 else {
            /* set aux control to self clocked only                        */
            s3OutTiIndReg(TI_AUXILIARY_CONTROL, 0, TI_AUX_SELF_CLOCK);
	 }
         /*
          * set output clocking to default of VGA.
          */
         s3OutTiIndReg(TI_OUTPUT_CLOCK_SELECT, 0x00, TI_OCLK_VGA);

         /* set mux control 1 and 2 to provide pseudocolor VGA          */
         s3OutTiIndReg(TI_MUX_CONTROL_1, 0x00, TI_MUX1_PSEUDO_COLOR);
         s3OutTiIndReg(TI_MUX_CONTROL_2, 0x00, TI_MUX2_BUS_VGA);

         /* change to 8-bit DAC and re-route the data path and clocking */
         s3OutTiIndReg(TI_GENERAL_IO_CONTROL, 0x00, TI_GIC_ALL_BITS);
         if (s3DAC8Bit)
            s3OutTiIndReg(TI_GENERAL_IO_DATA, 0x00, TI_GID_S3_DAC_8BIT);
         else
            s3OutTiIndReg(TI_GENERAL_IO_DATA, 0x00, TI_GID_S3_DAC_6BIT);
      }  /* end of s3PixelMultiplexing */

      /* for some reason the bios doesn't set this properly          */
      s3OutTiIndReg(TI_SENSE_TEST, 0x00, 0x00);

      outb(0x3C4, 1);
      outb(0x3C5, tmp2);        /* unblank the screen */

      return 1;
}

/***********************************************************\

			ATT498_DAC 
			ATT20C498_DAC			
			ATT22C498_DAC 
			ATT20C409_DAC

\***********************************************************/

static Bool ATT409_498_Probe(int type)
{
    int found = 0;
    int dir, mir, olddaccomm;

    /*quick check*/
    if (!S3_86x_SERIES(s3ChipId) && !S3_805_I_SERIES(s3ChipId))
	return FALSE;

    xf86dactopel();
    xf86dactocomm();
    (void)inb(0x3C6);
    mir = inb(0x3C6);
    dir = inb(0x3C6);
    xf86dactopel();

    if ((mir == 0x84) && (dir == 0x98)) {
	olddaccomm = xf86getdaccomm();
	xf86setdaccomm(0x0a);
	if (xf86getdaccomm() == 0) 
		found = ATT22C498_DAC;
	else
		found = ATT498_DAC;
	
	xf86setdaccomm(olddaccomm);
     } else if ((mir == 0x84) && (dir == 0x09)) {
	found = ATT20C409_DAC;
	if (!OFLG_ISSET(CLOCK_OPTION_ATT409, &s3InfoRec.clockOptions)) {
		OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
		OFLG_SET(CLOCK_OPTION_ATT409, &s3InfoRec.clockOptions);
		clockchip_probed = XCONFIG_PROBED;
	}
     } else if ((mir == 0x84) && (dir == 0x99)) {
	/*
	 * according to the 21C499 data sheet it is fully compatible
	 * with the 22C409. So we will only miss its new features
	 * this way, but in theory things might work.
	 */
	found = ATT20C409_DAC;
	if(type == ATT20C409_DAC) {
	    ErrorF("%s %s: Detected an ATT 21C499 RAMDAC\n",
		XCONFIG_PROBED, s3InfoRec.name);
	    ErrorF("%s %s:    support for this RAMDAC is untested. "
		"Please report to XFree86@XFree86.Org\n",
		XCONFIG_PROBED, s3InfoRec.name);
   	}
	if (!OFLG_ISSET(CLOCK_OPTION_ATT409, &s3InfoRec.clockOptions)) {
		OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
		OFLG_SET(CLOCK_OPTION_ATT409, &s3InfoRec.clockOptions);
		clockchip_probed = XCONFIG_PROBED;
	}
      }

     return (found == type);
}

static Bool ATT498_Probe()
{
    if(ATT409_498_Probe(ATT498_DAC)) {
	ErrorF("%s %s: Detected an ATT 20C498/21C498 RAMDAC\n",
			XCONFIG_PROBED, s3InfoRec.name);
	return TRUE;
    } else return FALSE;
}

static Bool ATT22C498_Probe()
{
    if(ATT409_498_Probe(ATT22C498_DAC)) {
	ErrorF("%s %s: Detected an ATT 22C498 RAMDAC\n",
			XCONFIG_PROBED, s3InfoRec.name);
	return TRUE;
    } else return FALSE;
}

static Bool ATT20C409_Probe()
{
    if(ATT409_498_Probe(ATT20C409_DAC)) {
	ErrorF("%s %s: Detected an ATT 20C409 RAMDAC\n",
		XCONFIG_PROBED, s3InfoRec.name);
	return TRUE;
    } else return FALSE;
}


static int ATT409_498_PreInit()
{
    /* Verify that depth is supported by ramdac */
	/* all are supported */
	
    /* Set cursor options */
   	/* none */

    /* Check if PixMux is supported and set the PixMux
	related flags and variables */
    if((xf86bpp <= 8) && 
	(S3_x64_SERIES(s3ChipId) || S3_805_I_SERIES(s3ChipId))) {
    	s3ATT498PixMux = TRUE;
       	pixMuxPossible = TRUE;

        if (DAC_IS_ATT20C498) {  
	  if (S3_866_SERIES(s3ChipId) || S3_868_SERIES(s3ChipId)) {
	    nonMuxMaxClock = 100000; /* 866/868 DCLK limit */
	    pixMuxMinClock =  67500;
	  }
	  else if (S3_864_SERIES(s3ChipId)) {
	    nonMuxMaxClock =  95000; /* 864 DCLK limit */
	    pixMuxMinClock =  67500;
	  }
	  else if (S3_805_I_SERIES(s3ChipId)) {
	    nonMuxMaxClock = 80000;  /* XXXX just a guess, who has 805i docs? */
	    pixMuxMinClock = 67500;
	  }
	  else {
	    nonMuxMaxClock = 67500;
	    pixMuxMinClock = 67500;
	  }
        }
        else {
	 nonMuxMaxClock = 67500;
	 pixMuxMinClock = 67500;
        }
        allowPixMuxInterlace = TRUE;
        allowPixMuxSwitching = TRUE;
        pixMuxLimitedWidths = FALSE;
        pixMuxMinWidth = 0;

    }		

    /* If there is an internal clock, set s3ClockSelectFunc, maxRawClock
	numClocks and whatever options need to be set. For external
	clocks, pass the job to OtherClocksSetup() */
   if (DAC_IS_ATT20C409 && 
       !OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions)) {
      OFLG_SET(CLOCK_OPTION_ATT409, &s3InfoRec.clockOptions);
      OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
      clockchip_probed = XCONFIG_PROBED;
   }
   if (OFLG_ISSET(CLOCK_OPTION_ATT409, &s3InfoRec.clockOptions)) {
      s3ClockSelectFunc = att409ClockSelect;
      numClocks = 3;
      maxRawClock = s3InfoRec.dacSpeeds[0]; /* Is this right?? */
      if (xf86Verbose)
	 ErrorF("%s %s: Using ATT20C409/ATT20C499 programmable clock\n",
		clockchip_probed, s3InfoRec.name);
   }
   else 
       OtherClocksSetup();   


   if (s3ATT498PixMux) {
	 s3InfoRec.maxClock = s3InfoRec.dacSpeeds[0];
	 if (s3Bpp == 1)	/* XXXX is this right?? */
	    clockDoublingPossible = TRUE;
   } else {
	 if (s3InfoRec.dacSpeeds[0] >= 135000) /* 20C498 -13, -15, -17 */
	    s3InfoRec.maxClock = 110000;
	 else				   /* 20C498 -11 */
	    s3InfoRec.maxClock = 80000;
	 /* Halve it for 32bpp */
	 if (s3Bpp == 4) {
	    s3InfoRec.maxClock /= 2;
	    maxRawClock /= 2;
	 }
   }

   return 1;
}

static void ATT409_498_Restore()
{
   xf86setdaccomm(s3DacRegs[0]);
}

static void ATT409_498_Save()
{
   s3DacRegs[0] = xf86getdaccomm();
}

static int ATT409_498_Init(DisplayModePtr mode)
{
      unsigned char tmp,tmp2;

      if (s3DAC8Bit) 
         xf86setdaccommbit(0x02);
      else 
	 xf86clrdaccommbit(0x02);
     

      outb(0x3C4, 1);
      tmp2 = inb(0x3C5);
      outb(0x3C5, tmp2 | 0x20); /* blank the screen */

      if (s3PixelMultiplexing) { /* x64:pixmux */
	 /* pixmux with 16/32 bpp not possible for 864 ==> only 8bit mode  */
	 int daccomm;
	 tmp = xf86getdaccomm();
	 
	 if (DAC_IS_ATT22C498) {
	    if (s3InfoRec.clock[mode->Clock]/2 < 22500) daccomm = 0x20;
	    else if (s3InfoRec.clock[mode->Clock]/2 < 45000) daccomm = 0x21;
	    else daccomm = 0x24;
#if 0
	    /* using digital clock doubler; 20C498 compatible */
	    daccomm = 0x25;
#endif
	 } else daccomm = 0x20;

#ifdef EXTENDED_DEBUG
	 ErrorF("Putting AT&T 2xC4[09][89] RAMDAC into pixmux\n");
#endif
	 xf86setdaccomm( (tmp&0x02) | daccomm );  /* set mode 2,
						  pixel multiplexing on */

	 if ( ! DAC_IS_ATT20C409 ) {
	    outb(vgaCRIndex, 0x33);	/* set VCLK = -DCLK */
	    tmp = inb(vgaCRReg);
	    outb(vgaCRReg, tmp | 0x08 );
	 }
	 
	 if (S3_x64_SERIES(s3ChipId) || S3_805_I_SERIES(s3ChipId)) {
	    outb(vgaCRIndex, 0x67);	/* set Mode 8: Two 8-bit color
	    				   1 VCLK / 2 pixels */
	    if (    OFLG_ISSET(OPTION_NUMBER_NINE, &s3InfoRec.options)
	         || DAC_IS_ATT20C409 )
	       outb(vgaCRReg, 0x10 );	/* VCLK is out of phase with DCLK */
	    else
	       outb(vgaCRReg, 0x11 );	/* VCLK is in phase with DCLK */

	    outb(vgaCRIndex, 0x6d);
	    outb(vgaCRReg, 2 );     /* delay -BLANK pulse by 2 DCLKs */
	 }
	 else {
	    /* don't know */
	 }
      } else { /* !s3PixelMultiplexing */
	 outb(vgaCRIndex, 0x33);
	 tmp = inb(vgaCRReg);
	 outb(vgaCRReg, tmp &  ~0x08 );

	 tmp = xf86getdaccomm() & 0x0f;

	 if (S3_x64_SERIES(s3ChipId) || S3_805_I_SERIES(s3ChipId)) {
	    int invert_vclk = 0;
	    int delay_blank = 0;
	    outb(vgaCRIndex, 0x67);

	    switch (s3InfoRec.bitsPerPixel) {
	    case 8: /* set Mode  0:  8-bit color, 1 VCLK/pixel */
	       outb(vgaCRReg, 0x00 | invert_vclk); 
	       xf86setdaccomm(tmp | 0x00);  /* set mode 0 */
	       break;
	    case 16: 
	       if (s3Weight == RGB16_555) {
		  outb(vgaCRReg, 0x30 | invert_vclk); /* set Mode 9:
						15-bit color, 1 VCLK/pixel */
		  xf86setdaccomm(tmp | 0x10);  /* set mode 1 */
	       }
	       else {
		  outb(vgaCRReg, 0x50 | invert_vclk); /* set Mode 10:
						16-bit color, 1 VCLK/pixel */
		  xf86setdaccomm(tmp | 0x30);  /* set mode 3 */
	       }
	       delay_blank = 2;
	       break;
	    case 32: /* set Mode 11: 24/32-bit color, 2 VCLK/pixel */
	       outb(vgaCRReg, 0x70 | invert_vclk);  
	       xf86setdaccomm(tmp | 0x50);  /* set mode 5 */
	       if (mode->HDisplay > 640)   /* why not for 640 ?  tsk */
		  delay_blank = 2;
	       break;
	    default:
	       ;
	    }
	    outb(vgaCRIndex, 0x6d);
	    outb(vgaCRReg, delay_blank);
	 }
	 else {
	    /* don't know */
	 }
      }  /* end of s3PixelMultiplexing */

      outb(0x3C4, 1);
      outb(0x3C5, tmp2);        /* unblank the screen */


     return 1;
}

/********************************************************\

		 SC15025_DAC 

\********************************************************/

static Bool SC15025_Probe()
{
    /* What chipsets use this? so we can do a quick check */

    Bool found = FALSE;
    int i;
    unsigned char c,id[4];

    c = xf86getdaccomm();
    xf86setdaccomm(c | 0x10);
    for (i=0; i<4; i++) {
	    outb(0x3C7, 0x9+i); 
	    id[i] = inb(0x3C8);
    }
    xf86setdaccomm(c);
    xf86dactopel();
    if (id[0] == 'S' && ((id[1]<<8)|id[2]) == 15025) {  
        /* unique for the SC 15025/26 */
	if (id[3] != 'A') {           /* version number */
	   ErrorF(
 	     "%s %s: ==> New Sierra SC 15025/26 version (0x%x) found,\n",
		XCONFIG_PROBED, s3InfoRec.name, id[3]);
	   ErrorF("\tplease report!\n");
	}
	ErrorF("%s %s: Detected a Sierra SC 15025/26 RAMDAC\n",
	           XCONFIG_PROBED, s3InfoRec.name);
	found = TRUE;
    }
    return found;    
}

static int SC15025_PreInit()
{
    int doubleEdgeLimit;
    /* Verify that depth is supported by ramdac */
	/* all are supported */

    /* Set cursor options */
   	/* none */

    /* Check if PixMux is supported and set the PixMux
	related flags and variables */
	/* no PixMux */

    /* let OtherClocksSetup() take care of the clock options */
    OtherClocksSetup();
     
    /* Make any necessary clock alterations due to multiplexing,
	clock doubling, etc...  s3Probe will do some last minute
	clock sanity checks when we return */

    if (s3InfoRec.dacSpeeds[0] >= 125000)	/* -125 */
	  doubleEdgeLimit = 85000;
    else if (s3InfoRec.dacSpeeds[0] >= 110000)	/* -110 */
	  doubleEdgeLimit = 65000;
    else					/* -80, -66 */
	  doubleEdgeLimit = 50000;
    switch (s3Bpp) {
	  case 1:
	    s3InfoRec.maxClock = s3InfoRec.dacSpeeds[0];
	    break;
	  case 2:
	    s3InfoRec.maxClock = doubleEdgeLimit;
	    maxRawClock /= 2;
	    break;
	  case 4:
	    s3InfoRec.maxClock = doubleEdgeLimit / 2;
	    maxRawClock /= 4;
	    break;
    }

    return 1;
}

static void SC15025_Restore()
{
      unsigned char c;

      c=xf86getdaccomm();
      xf86setdaccomm( c | 0x10 );  /* set internal register access */
      (void)xf86dactocomm();
      outb(0x3c7, 0x8);   /* Auxiliary Control Register */
      outb(0x3c8, s3DacRegs[1]);
      outb(0x3c7, 0x10);  /* Pixel Repack Register */
      outb(0x3c8, s3DacRegs[2]);
      xf86setdaccomm( c );
      xf86setdaccomm(s3DacRegs[0]);
}

static void SC15025_Save()
{
      LOCK_SYS_REGS;
         s3DacRegs[0] = xf86getdaccomm();
	 xf86setdaccomm((s3DacRegs[0] | 0x10));
         (void)xf86dactocomm();
	 outb(0x3c7,0x8);   /* Auxiliary Control Register */
	 s3DacRegs[1] = inb(0x3c8);
	 outb(0x3c7,0x10);  /* Pixel Repack Register */
	 s3DacRegs[2] = inb(0x3c8);
	 xf86setdaccomm(s3DacRegs[0]);
      UNLOCK_SYS_REGS;
}

static int SC15025_Init(DisplayModePtr mode)
{
      unsigned char aux=0, comm=0, prr=0;

      LOCK_SYS_REGS;
      if (s3DAC8Bit || s3InfoRec.bitsPerPixel > 8) aux=1;
      switch (s3InfoRec.bitsPerPixel) {
      case 8: 
	 comm = 0;  /* repack mode 0, color mode 0 */
	 break;
      case 16:
	 if (s3Weight == RGB16_555) {
	    comm = 0x80;  /* repack mode 1a using both clock edges */
	 }
	 else {  /* RGB16_565 */
	    comm = 0xc0;  /* repack mode 1a using both clock edges */
	 }
	 break;
      case 32:
	 comm = 0x40;  /* repack mode 3a using both clock edges */
	 prr = 1;
	 break;
      default:
	 return 0;
      }
      
      comm |= 0x08;  /* enable LUT for gamma correction */

      xf86setdaccomm(comm | 0x10);
      outb(0x3c7,0x8);
      outb(0x3c8,aux);
      outb(0x3c7,0x10);
      outb(0x3c8,prr);
      xf86setdaccomm(comm);
	
      return 1;
}

/*********************************************************\

 			STG1700_DAC  
			STG1703_DAC 

\*********************************************************/

static Bool STG17xx_Probe(int type)
{
    int found = 0;
    int cid, did, daccomm, readmask;

    if (!S3_86x_SERIES(s3ChipId) && !S3_805_I_SERIES(s3ChipId))
	return FALSE;

    readmask = inb(0x3c6);
    xf86dactopel();
    daccomm = xf86getdaccomm();
    xf86setdaccommbit(0x10);
    xf86dactocomm();
    inb(0x3c6);
    outb(0x3c6, 0x00);
    outb(0x3c6, 0x00);
    cid = inb(0x3c6);     /* company ID */
    did = inb(0x3c6);     /* device ID */
    xf86dactopel(); 
    outb(0x3c6,readmask);

    if ((cid == 0x44) && (did == 0x00))
    {
	found = STG1700_DAC;
    }  else if ((cid == 0x44) && (did == 0x03)) {
	found = STG1703_DAC;
	if (!OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE,&s3InfoRec.clockOptions)) {
	       OFLG_SET(CLOCK_OPTION_STG1703,    &s3InfoRec.clockOptions);
	       OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
	       clockchip_probed = XCONFIG_PROBED;
	}
    }

    xf86setdaccomm(daccomm);

    return (found == type);
}

static Bool STG1700_Probe()
{
    if(STG17xx_Probe(STG1700_DAC)) {
	ErrorF("%s %s: Detected an STG1700 RAMDAC\n",
	           XCONFIG_PROBED, s3InfoRec.name);
	return TRUE;
    } else return FALSE;
}

static Bool STG1703_Probe()
{
    if(STG17xx_Probe(STG1703_DAC)) {
	ErrorF("%s %s: Detected an STG1703 RAMDAC\n",
	           XCONFIG_PROBED, s3InfoRec.name);
	return TRUE;
    } else return FALSE;
}



static int STG17xx_PreInit()
{
    /* Verify that depth is supported by ramdac */
	/* all are supported */

    /* Set cursor options */
	/* none */
   
    /* Check if PixMux is supported and set the PixMux
	related flags and variables */
    if((xf86bpp <= 8) &&  (S3_x64_SERIES(s3ChipId) || 
			S3_805_I_SERIES(s3ChipId))) {
	s3ATT498PixMux = TRUE;
	nonMuxMaxClock = 67500;
	pixMuxMinClock = 67500;
        allowPixMuxInterlace = TRUE;
        allowPixMuxSwitching = TRUE;
        pixMuxLimitedWidths = FALSE;
        pixMuxMinWidth = 0;

    }   

    /* If there is an internal clock, set s3ClockSelectFunc, maxRawClock
	numClocks and whatever options need to be set. For external
	clocks, pass the job to OtherClocksSetup() */
    if (OFLG_ISSET(CLOCK_OPTION_STG1703, &s3InfoRec.clockOptions)) {
      unsigned char mi, ml, mh, tmp;
      int mclk;
      
      outb(vgaCRIndex, 0x43);
      tmp = inb(vgaCRReg);
      outb(vgaCRReg, tmp & ~0x02);
      
      outb(vgaCRIndex, 0x55);
      tmp = inb(vgaCRReg) & ~0x03;
      outb(vgaCRReg, tmp | 1);  /* set RS2 */
      
      outb(0x3c7, 0x00);  /* index high */
      outb(0x3c8, 0x48);  /* index low  */
      mi = (inb(0x3c9) >> 4) & 0x03;
      
      outb(0x3c8, 0x40 + 2*mi);  /* index low  */
      ml = inb(0x3c9);
      mh = inb(0x3c9);
      
      outb(vgaCRReg, tmp);  /* reset RS2 */
      
      mclk = ((((1431818 * ((ml&0x7f) + 2)) / ((mh&0x1f) + 2)) 
	       >> ((mh>>5)&0x03)) + 50) / 100;
      
      s3ClockSelectFunc = STG1703ClockSelect;
      numClocks = 3;
      maxRawClock = 135000;

      if (xf86Verbose)
	 ErrorF("%s %s: Using STG1703 programmable clock(MCLK%d %02x %02x "
	   "%1.3f MHz)\n",XCONFIG_GIVEN, s3InfoRec.name, mi, ml,mh, mclk/1e3);
      if (s3InfoRec.s3MClk > 0) {
	 if (xf86Verbose)
	    ErrorF("%s %s: using specified MCLK value of %1.3f MHz for DRAM "
 	"timings\n",XCONFIG_GIVEN, s3InfoRec.name, s3InfoRec.s3MClk / 1000.0);
      }
      else
	 s3InfoRec.s3MClk = mclk;
      
   }
   else
       OtherClocksSetup();

    /* Make any necessary clock alterations due to multiplexing,
	clock doubling, etc...  s3Probe will do some last minute
	clock sanity checks when we return */
   if (s3ATT498PixMux) {
	 s3InfoRec.maxClock = s3InfoRec.dacSpeeds[0];
	 if (s3Bpp == 1)	/* XXXX is this right?? */
	    clockDoublingPossible = TRUE;
   }
   else {
	 if (s3InfoRec.dacSpeeds[0] >= 135000) /* 20C498 -13, -15, -17 */
	    s3InfoRec.maxClock = 110000;
	 else				   /* 20C498 -11 */
	    s3InfoRec.maxClock = 80000;
	 /* Halve it for 32bpp */
	 if (s3Bpp == 4) {
	    s3InfoRec.maxClock /= 2;
	    maxRawClock /= 2;
	 }
   }

   return 1;
}

static void STG17xx_Restore()
{
      xf86dactopel();
      xf86setdaccommbit(0x10);   /* enable extended registers */
      xf86dactocomm();
      inb(0x3c6);                /* command reg */

      outb(0x3c6, 0x03);         /* index low */
      outb(0x3c6, 0x00);         /* index high */
      
      outb(0x3c6, s3DacRegs[1]);  /* primary pixel mode */
      outb(0x3c6, s3DacRegs[2]);  /* secondary pixel mode */
      outb(0x3c6, s3DacRegs[3]);  /* PLL control */
      usleep(500);		       /* PLL settling time */

      xf86dactopel();
      xf86setdaccomm(s3DacRegs[0]);
}


static void STG17xx_Save()
{
         xf86dactopel();
         s3DacRegs[0] = xf86getdaccomm();

         xf86setdaccommbit(0x10);   /* enable extended registers */
         xf86dactocomm();
         inb(0x3c6);                /* command reg */

         outb(0x3c6, 0x03);         /* index low */
         outb(0x3c6, 0x00);         /* index high */

         s3DacRegs[1] = inb(0x3c6);  /* primary pixel mode */
         s3DacRegs[2] = inb(0x3c6);  /* secondary pixel mode */
         s3DacRegs[3] = inb(0x3c6);  /* PLL control */

         xf86dactopel();
}

static int STG17xx_Init(DisplayModePtr mode)
{
      unsigned char tmp, tmp2;
      int daccomm;

      if (s3DAC8Bit) 
         xf86setdaccommbit(0x02);
      else 
	 xf86clrdaccommbit(0x02);

      daccomm = (xf86getdaccomm() & 0x06) | 0x10;

      outb(0x3C4, 1);
      tmp2 = inb(0x3C5);
      outb(0x3C5, tmp2 | 0x20); /* blank the screen */

      if (s3PixelMultiplexing) {
	 /* x64:pixmux */
	 /* pixmux with 16/32 bpp not possible for 864 ==> only 8bit mode  */
         daccomm |= 0x08;           /* enable extended pixel modes */
	 xf86setdaccomm(daccomm);   /* pixel multiplexing on */

         xf86dactocomm();
         inb(0x3c6);                /* command reg */

         outb(0x3c6, 0x03);         /* index low */
         outb(0x3c6, 0x00);         /* index high */
      
         outb(0x3c6, 0x05);         /* primary pixel mode */
         outb(0x3c6, 0x05);         /* secondary pixel mode */
         outb(0x3c6, 0x02);         /* PLL control for 64-135 MHz pixclk */
         usleep(500);		    /* PLL settling time before LUT access */

         xf86dactopel();

	 outb(vgaCRIndex, 0x33);
	 tmp = inb(vgaCRReg);
	 outb(vgaCRReg, tmp | 0x08 );
	 
	 if (S3_x64_SERIES(s3ChipId) || S3_805_I_SERIES(s3ChipId)) {
	    outb(vgaCRIndex, 0x67);
	    outb(vgaCRReg, 0x11 );   /* set Mode 5: double 8-bit indexed color,
					  1 VCLK/2 pixels */
	    outb(vgaCRIndex, 0x6d);
	    outb(vgaCRReg, 2 );     /* delay -BLANK pulse by 2 DCLKs */
	 }
	 else {
	    /* don't know */
	 }
      }
      else 
      { /* !s3PixelMultiplexing */
	 outb(vgaCRIndex, 0x33);
	 tmp = inb(vgaCRReg);
	 outb(vgaCRReg, tmp &  ~0x08 );

	 if (S3_x64_SERIES(s3ChipId) || S3_805_I_SERIES(s3ChipId)) {
	    int invert_vclk = 0;
	    int delay_blank = 0;
            int pixmode     = 0;
            int s3mux       = 0;

	    outb(vgaCRIndex, 0x67);

	    switch (s3InfoRec.bitsPerPixel) 
            {
	       case 8: /* set Mode  0:  8-bit indexed color, 1 VCLK/pixel */
	          daccomm |= 0x00;
                  s3mux    = 0x00 | invert_vclk; 
	          break;

       	       case 16: 
	          if (s3Weight == RGB16_555) 
                  {  /* set Mode 2: 15-bit direct color */
	             daccomm |= 0xa8;
                     pixmode  = 0x02;
                     s3mux    = 0x30 | invert_vclk; 
	          }
	          else 
                  {  /* set Mode 3: 16-bit (565) direct color */
	             daccomm |= 0xc8;
                     pixmode  = 0x03;
                     s3mux    = 0x50 | invert_vclk; 
	          }
	          delay_blank = 2;
	          break;

	       case 32: /* set Mode 4: 24-bit direct color, 2 VCLK/pixel */
	          daccomm |= 0xe8;
                  pixmode  = 0x04;
                  s3mux    = 0x70 | invert_vclk; 
	          delay_blank = 2;
	          break;

	       default:
	          ErrorF("default switch 2\n");
	    }
	    outb(vgaCRReg, s3mux);  
	    xf86setdaccomm(daccomm);

            xf86dactocomm();
            inb(0x3c6);                /* command reg */

            outb(0x3c6, 0x03);         /* index low */
            outb(0x3c6, 0x00);         /* index high */
      
            outb(0x3c6, pixmode);      /* primary pixel mode */
            outb(0x3c6, pixmode);      /* secondary pixel mode */

            xf86dactopel();

	    outb(vgaCRIndex, 0x6d);
	    outb(vgaCRReg, delay_blank);
	 }
	 else {
	    /* don't know */
	 }
      }  /* end of s3PixelMultiplexing */

      outb(0x3C4, 1);
      outb(0x3C5, tmp2);        /* unblank the screen */
   
      return 1;
}

/*********************************************************\

 			S3_SDAC_DAC  
			S3_GENDAC_DAC 

\*********************************************************/

static Bool S3_SDAC_GENDAC_Probe(int type)
{
   /* probe for S3 GENDAC or SDAC */
   /*
    * S3 GENDAC and SDAC have two fixed read only PLL clocks
    *     CLK0 f0: 25.255MHz   M-byte 0x28  N-byte 0x61
    *     CLK0 f1: 28.311MHz   M-byte 0x3d  N-byte 0x62
    * which can be used to detect GENDAC and SDAC since there is no chip-id
    * for the GENDAC.
    *
    * NOTE: for the GENDAC on a MIRO 10SD (805+GENDAC) reading PLL values
    * for CLK0 f0 and f1 always returns 0x7f (but is documented "read only")
    */
      
   unsigned char saveCR55, saveCR45, saveCR43, savelut[6];
   unsigned int i;		/* don't use signed int, UW2.0 compiler bug */
   long clock01, clock23;
   int found = 0;

   outb(vgaCRIndex, 0x43);
   saveCR43 = inb(vgaCRReg);
   outb(vgaCRReg, saveCR43 & ~0x02);

   outb(vgaCRIndex, 0x45);
   saveCR45 = inb(vgaCRReg);
   outb(vgaCRReg, saveCR45 & ~0x20);

   outb(vgaCRIndex, 0x55);
   saveCR55 = inb(vgaCRReg);
   outb(vgaCRReg, saveCR55 & ~1);

   outb(0x3c7,0);
   for(i=0; i<2*3; i++)		/* save first two LUT entries */
      savelut[i] = inb(0x3c9);
   outb(0x3c8,0);
   for(i=0; i<2*3; i++)		/* set first two LUT entries to zero */
      outb(0x3c9,0);

   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, saveCR55 | 1);

   outb(0x3c7,0);
   for(i=clock01=0; i<4; i++)
      clock01 = (clock01 << 8) | (inb(0x3c9) & 0xff);
   for(i=clock23=0; i<4; i++)
      clock23 = (clock23 << 8) | (inb(0x3c9) & 0xff);

   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, saveCR55 & ~1);

   outb(0x3c8,0);
   for(i=0; i<2*3; i++)		/* restore first two LUT entries */
      outb(0x3c9,savelut[i]);

   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, saveCR55);

   if ( clock01 == 0x28613d62 ||
       (clock01 == 0x7f7f7f7f && clock23 != 0x7f7f7f7f)) {

	
	 xf86dactopel();
	 inb(0x3c6);
	 inb(0x3c6);
	 inb(0x3c6);

	 /* the fourth read will show the SDAC chip ID and revision */
	 if (((i=inb(0x3c6)) & 0xf0) == 0x70) {
	    found = S3_SDAC_DAC;
	    saveCR43 &= ~0x02;
	    saveCR45 &= ~0x20;
	 }
	 else {
	    found = S3_GENDAC_DAC;
	    saveCR43 &= ~0x02;
	    saveCR45 &= ~0x20;
	 }
         xf86dactopel();
   }

   outb(vgaCRIndex, 0x45);
   outb(vgaCRReg, saveCR45);

   outb(vgaCRIndex, 0x43);
   outb(vgaCRReg, saveCR43);

   if((found==type) && !OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, 
					&s3InfoRec.clockOptions)) {
	OFLG_SET(CLOCK_OPTION_S3GENDAC,    &s3InfoRec.clockOptions);
	OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
	clockchip_probed = XCONFIG_PROBED;
   }


   return (found == type);
}

static Bool S3_SDAC_Probe()
{
   if (!S3_86x_SERIES(s3ChipId) && !S3_805_I_SERIES(s3ChipId))
	return FALSE;
   else if(S3_SDAC_GENDAC_Probe(S3_SDAC_DAC)) {
   ErrorF("%s %s: Detected an S3 SDAC 86C716 RAMDAC and programmable clock\n", 
		XCONFIG_PROBED, s3InfoRec.name);
	return TRUE;
   } else return FALSE;
}

static Bool S3_GENDAC_Probe()
{
   if (!S3_801_SERIES(s3ChipId))
	return FALSE;
   else if (S3_SDAC_GENDAC_Probe(S3_GENDAC_DAC)) {
  ErrorF("%s %s: Detected an S3 GENDAC 86C708 RAMDAC and programmable clock\n", 
			XCONFIG_PROBED, s3InfoRec.name);
	return TRUE;
   } else return FALSE;
}

static int S3_SDAC_GENDAC_PreInit()
{
    /* Verify that depth is supported by ramdac */
	/* all are supported */

    /* Set cursor options */
	/* none */
   
    /* Check if PixMux is supported and set the PixMux
	related flags and variables */
    if(DAC_IS_SDAC && (xf86bpp <= 8) && 
	(S3_x64_SERIES(s3ChipId) || S3_805_I_SERIES(s3ChipId))) {
	s3ATT498PixMux = TRUE;
        pixMuxPossible = TRUE;
	nonMuxMaxClock = 67500;
	pixMuxMinClock = 67500;
        allowPixMuxInterlace = TRUE;
        allowPixMuxSwitching = TRUE;
        pixMuxLimitedWidths = FALSE;
        pixMuxMinWidth = 0;
    }

    OtherClocksSetup();   /* S3_GENDAC clock is in OtherClocksSetup()
			     since it is compatible with the ICS5342 */

    /* Make any necessary clock alterations due to multiplexing,
	clock doubling, etc...  s3Probe will do some last minute
	clock sanity checks when we return */
    if(DAC_IS_SDAC) {
    	if (s3ATT498PixMux) {
	  s3InfoRec.maxClock = s3InfoRec.dacSpeeds[0];
	  if (s3Bpp == 1)	/* XXXX is this right?? */
	    clockDoublingPossible = TRUE;
        } else {
	  if (s3InfoRec.dacSpeeds[0] >= 135000) /* 20C498 -13, -15, -17 */
	    s3InfoRec.maxClock = 110000;
	  else				   /* 20C498 -11 */
	    s3InfoRec.maxClock = 80000;
	 /* Halve it for 32bpp */
	  if (s3Bpp == 4) {
	    s3InfoRec.maxClock /= 2;
	    maxRawClock /= 2;
	  }
        }
     } else  /* DAC_IS_GENDAC */
        s3InfoRec.maxClock = s3InfoRec.dacSpeeds[0] / s3Bpp;

      return 1;
}


static void S3_SDAC_GENDAC_Restore()
{
#if !defined(PC98_PW) && !defined(PC98_PWLB)
      unsigned char tmp;

      outb(vgaCRIndex, 0x55);
      tmp = inb(vgaCRReg);
      outb(vgaCRReg, tmp | 1);

      outb(0x3c6, s3DacRegs[0]);      /* Enhanced command register */
      outb(0x3c8, 2);                   /* index to f2 reg */
      outb(0x3c9, s3DacRegs[3]);      /* f2 PLL M divider */
      outb(0x3c9, s3DacRegs[4]);      /* f2 PLL N1/N2 divider */
      outb(0x3c8, 0x0e);                /* index to PLL control */
      outb(0x3c9, s3DacRegs[5]);      /* PLL control */
      outb(0x3c8, s3DacRegs[2]);      /* PLL write index */
      outb(0x3c7, s3DacRegs[1]);      /* PLL read index */

      outb(vgaCRReg, tmp & ~1);
#endif
}

static void S3_SDAC_GENDAC_Save()
{
#if !defined(PC98_PW) && !defined(PC98_PWLB)
         unsigned char tmp;

         outb(vgaCRIndex, 0x55);
	 tmp = inb(vgaCRReg);
         outb(vgaCRReg, tmp | 1);

         s3DacRegs[0] = inb(0x3c6);      /* Enhanced command register */
         s3DacRegs[2] = inb(0x3c8);      /* PLL write index */
         s3DacRegs[1] = inb(0x3c7);      /* PLL read index */
         outb(0x3c7, 2);                   /* index to f2 reg */
         s3DacRegs[3] = inb(0x3c9);      /* f2 PLL M divider */
         s3DacRegs[4] = inb(0x3c9);      /* f2 PLL N1/N2 divider */
         outb(0x3c7, 0x0e);                /* index to PLL control */
         s3DacRegs[5] = inb(0x3c9);      /* PLL control */

         outb(vgaCRReg, tmp & ~1);
#endif
}

static int S3_SDAC_Init(DisplayModePtr mode)
{
      int pixmux = 0;           /* SDAC command and CR67 */
      int blank_delay = 0;      /* CR6D */
      int invert_vclk = 0;      /* CR66 bit 0 */
      unsigned char tmp2, tmp3;

      outb(0x3C4, 1);
      tmp2 = inb(0x3C5);
      outb(0x3C5, tmp2 | 0x20); /* blank the screen */

      if (s3PixelMultiplexing)
      {
	 /* x64:pixmux */
	 /* pixmux with 16/32 bpp not possible for 864 ==> only 8bit mode  */
         pixmux = 0x10;         /* two 8bit pixels per clock */
	 if (!S3_866_SERIES(s3ChipId) && !S3_868_SERIES(s3ChipId))
	    invert_vclk = 1;
         blank_delay = 2;
      }
      else
      {
         switch (s3InfoRec.bitsPerPixel) 
         {
            case 8:  /* 8-bit color, 1 VCLK/pixel */
               break;

            case 16: /* 15/16-bit color, 1VCLK/pixel */
               if (s3Weight == RGB16_555)
                  pixmux = 0x30;
               else
                  pixmux = 0x50;
               blank_delay = 2;
               break;

            case 32: /* 32-bit color, 2VCLK/pixel */
               pixmux = 0x70;
               blank_delay = 2;
         }
      }

      outb(vgaCRIndex, 0x55);
      tmp3 = inb(vgaCRReg);
      outb(vgaCRReg, tmp3 | 1);

      outb(vgaCRIndex, 0x67);
      outb(vgaCRReg, pixmux | invert_vclk);    /* set S3 mux mode */
      outb(0x3c6, pixmux);                     /* set SDAC mux mode */

      outb(vgaCRIndex, 0x6D);
      outb(vgaCRReg, blank_delay);             /* set blank delay */

      outb(vgaCRIndex, 0x55);
      tmp3 = inb(vgaCRReg);
      outb(vgaCRReg, tmp3 & ~1);

      outb(0x3C4, 1);
      outb(0x3C5, tmp2);        /* unblank the screen */

      return 1;
}

static int S3_GENDAC_Init(DisplayModePtr mode)
{
      int daccomm = 0;           /* GENDAC command */
      unsigned char tmp2, tmp3;

      outb(0x3C4, 1);
      tmp2 = inb(0x3C5);
      outb(0x3C5, tmp2 | 0x20); /* blank the screen */

      switch (s3InfoRec.bitsPerPixel) 
      {
         case 8:  /* 8-bit color, 1 VCLK/pixel */
            break;

         case 16: /* 15/16-bit color, 2VCLK/pixel */
            if (s3Weight == RGB16_555)
               daccomm = 0x20;
            else
               daccomm = 0x60;
            break;

         case 32: /* 32-bit color, 3VCLK/pixel */
            daccomm = 0x40;
      }

      outb(vgaCRIndex, 0x55);
      tmp3 = inb(vgaCRReg);
      outb(vgaCRReg, tmp3 | 1);

      outb(0x3c6, daccomm);                     /* set GENDAC mux mode */

      outb(vgaCRIndex, 0x55);
      tmp3 = inb(vgaCRReg);
      outb(vgaCRReg, tmp3 & ~1);

      outb(0x3C4, 1);
      outb(0x3C5, tmp2);        /* unblank the screen */

      return 1;
}

/***********************************************************\

 			S3_TRIO32_DAC  
			S3_TRIO64_DAC 

\***********************************************************/


static Bool S3_TRIO_Probe(int type) 
{
   int found = 0;

   if (S3_TRIOxx_SERIES(s3ChipId)) {
	 if (S3_TRIO32_SERIES(s3ChipId))
		found = S3_TRIO32_DAC;
	 else if (S3_TRIO64V2_SERIES(s3ChipId))
		found = S3_TRIO64V2_DAC;
	 else 
		found = S3_TRIO64_DAC;
   }

   if(found == type)
   {
      if (S3_AURORA64VP_SERIES(s3ChipId)) {
	 if ( OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions) &&
	     !OFLG_ISSET(CLOCK_OPTION_S3TRIO, &s3InfoRec.clockOptions) &&
	     !OFLG_ISSET(CLOCK_OPTION_S3AURORA, &s3InfoRec.clockOptions)) {
	    ErrorF("%s %s: for Aurora64 chips you shouldn't specify any Clockchip\n"
		   "\t other than \"s3_aurora64\" or maybe \"s3_trio64\"\n",
		   XCONFIG_PROBED, s3InfoRec.name);
	    /* Clear the other clock options */
	    OFLG_ZERO(&s3InfoRec.clockOptions);
	 }
	 if (S3_AURORA64VP_SERIES(s3ChipId) &&
	     !OFLG_ISSET(CLOCK_OPTION_S3TRIO, &s3InfoRec.clockOptions)) {
	    OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
	    OFLG_SET(CLOCK_OPTION_S3AURORA, &s3InfoRec.clockOptions);
	    clockchip_probed = XCONFIG_PROBED;
	 }
      }
      else if (S3_TRIO64V2_SERIES(s3ChipId)) {
	 if ( OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions) &&
	     !OFLG_ISSET(CLOCK_OPTION_S3TRIO, &s3InfoRec.clockOptions) &&
	     !OFLG_ISSET(CLOCK_OPTION_S3TRIO64V2, &s3InfoRec.clockOptions)) {
	    ErrorF("%s %s: for Trio64V2 chips you shouldn't specify any Clockchip\n"
		   "\t other than \"s3_trio64v2\" or maybe \"s3_trio64\"\n",
		   XCONFIG_PROBED, s3InfoRec.name);
	    /* Clear the other clock options */
	    OFLG_ZERO(&s3InfoRec.clockOptions);
	 }
	 if (S3_TRIO64V2_SERIES(s3ChipId) &&
	     !OFLG_ISSET(CLOCK_OPTION_S3TRIO, &s3InfoRec.clockOptions)) {
	    OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
	    OFLG_SET(CLOCK_OPTION_S3TRIO64V2, &s3InfoRec.clockOptions);
	    clockchip_probed = XCONFIG_PROBED;
	 }
      }
      else {
	 if ( OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions) &&
	     !OFLG_ISSET(CLOCK_OPTION_S3TRIO, &s3InfoRec.clockOptions)) {
	    ErrorF("%s %s: for Trio32/64 chips you shouldn't specify a Clockchip\n",
		   XCONFIG_PROBED, s3InfoRec.name);
	    /* Clear the other clock options */
	    OFLG_ZERO(&s3InfoRec.clockOptions);
	 }
	 if (!OFLG_ISSET(CLOCK_OPTION_S3TRIO, &s3InfoRec.clockOptions)) {
	    OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
	    OFLG_SET(CLOCK_OPTION_S3TRIO, &s3InfoRec.clockOptions);
	    clockchip_probed = XCONFIG_PROBED;
	 }
      }
   }
  
   return (found == type);
}

static Bool S3_TRIO64V2_Probe()
{
    return S3_TRIO_Probe(S3_TRIO64V2_DAC);
}

static Bool S3_TRIO64_Probe()
{
    return S3_TRIO_Probe(S3_TRIO64_DAC);
}

static Bool S3_TRIO32_Probe()
{
    return S3_TRIO_Probe(S3_TRIO32_DAC);
}

static int S3_TRIO_PreInit()
{
    unsigned char sr8, sr27, sr28;
    int m,n,n1,n2, mclk;

    /* Verify that depth is supported by ramdac */
	/* all are supported */

    /* Set cursor options */
	/* none */
   
    /* Check if PixMux is supported and set the PixMux
	related flags and variables */
    if(xf86bpp <= 8) {
	s3ATT498PixMux = TRUE;
       	pixMuxPossible = TRUE;
    	if (OFLG_ISSET(OPTION_TRIO64VP_BUG3, &s3InfoRec.options)) {
	    nonMuxMaxClock = 0;   /* need CR67=10 and DCLK/2 all the time */
	    pixMuxMinClock = 0;   /* to avoid problems with blank signal */
    	} else {
	    nonMuxMaxClock = 80000;
	    pixMuxMinClock = 80000;
    	}
    	allowPixMuxInterlace = TRUE;
    	allowPixMuxSwitching = TRUE;
    	pixMuxLimitedWidths = FALSE;
    	pixMuxMinWidth = 0;
   
	s3InfoRec.maxClock = s3InfoRec.dacSpeeds[0];
    } else if (s3Bpp < 4)
       if (OFLG_ISSET(CLOCK_OPTION_S3TRIO64V2, &s3InfoRec.clockOptions))
	  s3InfoRec.maxClock = 170000;
       else
	  s3InfoRec.maxClock = 80000;
    else
       if (OFLG_ISSET(CLOCK_OPTION_S3TRIO64V2, &s3InfoRec.clockOptions))
	  s3InfoRec.maxClock = 135000;
       else
	  s3InfoRec.maxClock = 50000;
 

    /* If there is an internal clock, set s3ClockSelectFunc, maxRawClock
	numClocks and whatever options need to be set. For external
	clocks, pass the job to OtherClocksSetup() */
    s3ClockSelectFunc = s3GendacClockSelect;
    numClocks = 3;
    if (OFLG_ISSET(CLOCK_OPTION_S3TRIO64V2, &s3InfoRec.clockOptions))
       maxRawClock = 170000;
    else
       maxRawClock = 135000;
      
    outb(0x3c4, 0x08);
    sr8 = inb(0x3c5);
    outb(0x3c5, 0x06);
      
    outb(0x3c4, 0x11);
    m = inb(0x3c5);
    outb(0x3c4, 0x10);
    n = inb(0x3c5);
      
    m &= 0x7f;
    n1 = n & 0x1f;
    n2 = (n>>5) & 0x03;
    mclk = ((1431818 * (m+2)) / (n1+2) / (1 << n2) + 50) / 100;
    if (OFLG_ISSET(CLOCK_OPTION_S3AURORA, &s3InfoRec.clockOptions)) {       
       outb(0x3c4, 0x27);
       sr27 = inb(0x3c5);
       outb(0x3c4, 0x28);
       sr28 = inb(0x3c5);
       mclk /= ((sr27 >> 2) & 0x03) + 1;
    }

    outb(0x3c4, 0x08);
    outb(0x3c5, sr8);
      
    if (xf86Verbose)
       if (OFLG_ISSET(CLOCK_OPTION_S3AURORA, &s3InfoRec.clockOptions))
	  ErrorF("%s %s: Using Aurora64 programmable clock (MCLK %1.3f MHz, SR27=%02x, SR28=%02x)\n"
		 ,clockchip_probed, s3InfoRec.name
		 ,mclk / 1000.0, sr27, sr28);
       else if (OFLG_ISSET(CLOCK_OPTION_S3TRIO64V2, &s3InfoRec.clockOptions))
	  ErrorF("%s %s: Using Trio64V2 programmable clock (MCLK %1.3f MHz)\n"
		 ,clockchip_probed, s3InfoRec.name
		 ,mclk / 1000.0);
       else
	  ErrorF("%s %s: Using Trio32/64 programmable clock (MCLK %1.3f MHz)\n"
		 ,clockchip_probed, s3InfoRec.name
		 ,mclk / 1000.0);
    if (s3InfoRec.s3MClk > 0) {
      if (xf86Verbose)
	ErrorF("%s %s: using specified MCLK value of %1.3f MHz for DRAM "
	 "timings\n",XCONFIG_GIVEN, s3InfoRec.name, s3InfoRec.s3MClk/1000.0);
    } else {
	 s3InfoRec.s3MClk = mclk;
    }
    if (s3InfoRec.MemClk > 0) {
       /* some sanity checks */
       if (s3InfoRec.MemClk < 40000 || s3InfoRec.MemClk > 100000) {
	  ErrorF("%s %s: MCLK %1.3f MHz too low/high, not changed!\n",
		 OFLG_ISSET(XCONFIG_DACSPEED, &s3InfoRec.xconfigFlag) ?
		 XCONFIG_GIVEN : XCONFIG_PROBED, s3InfoRec.name, 
		 s3InfoRec.MemClk / 1000.0);
	  s3InfoRec.MemClk = 0;
       }
       else if (xf86Verbose)
	  ErrorF("%s %s: set MCLK to %1.3f MHz\n",
		 XCONFIG_GIVEN, s3InfoRec.name, s3InfoRec.MemClk / 1000.0);
    }

 
    return 1;  
}

static void S3_TRIO_Restore()
{
      unsigned char tmp;

      outb(0x3c2, s3DacRegs[0]);
      outb(0x3c4, 0x08); outb(0x3c5, 0x06);

      outb(0x3c4, 0x09); outb(0x3c5, s3DacRegs[2]);
      outb(0x3c4, 0x0a); outb(0x3c5, s3DacRegs[3]);
      outb(0x3c4, 0x0b); outb(0x3c5, s3DacRegs[4]);
      outb(0x3c4, 0x0d); outb(0x3c5, s3DacRegs[5]);

      outb(0x3c4, 0x10); outb(0x3c5, s3DacRegs[8]); 
      outb(0x3c4, 0x11); outb(0x3c5, s3DacRegs[9]); 
      outb(0x3c4, 0x12); outb(0x3c5, s3DacRegs[10]); 
      outb(0x3c4, 0x13); outb(0x3c5, s3DacRegs[11]); 
      outb(0x3c4, 0x1a); outb(0x3c5, s3DacRegs[12]); 
      outb(0x3c4, 0x1b); outb(0x3c5, s3DacRegs[13]); 
      outb(0x3c4, 0x15);
      tmp = inb(0x3c5) & ~0x21;
      outb(0x3c5, tmp | 0x03);
      outb(0x3c5, tmp | 0x23);
      outb(0x3c5, tmp | 0x03);

      outb(0x3c4, 0x15); outb(0x3c5, s3DacRegs[6]); 
      outb(0x3c4, 0x18); outb(0x3c5, s3DacRegs[7]);

      /* if (S3_AURORA64VP_SERIES(s3ChipId)) { */
      if (OFLG_ISSET(CLOCK_OPTION_S3AURORA, &s3InfoRec.clockOptions)) {
	 int i;
	 for (i=0x1a; i<= 0x6f; i++) {
	    outb(0x3c4, i); outb(0x3c5, s3DacRegs[i]);
	 }
      }

      outb(0x3c4, 0x08); outb(0x3c5, s3DacRegs[1]);

} 

static void S3_TRIO_Save()
{
	 s3DacRegs[0] = inb(0x3cc);

	 outb(0x3c4, 0x08); s3DacRegs[1] = inb(0x3c5);
	 outb(0x3c5, 0x06);

	 outb(0x3c4, 0x09); s3DacRegs[2]  = inb(0x3c5);
	 outb(0x3c4, 0x0a); s3DacRegs[3]  = inb(0x3c5);
	 outb(0x3c4, 0x0b); s3DacRegs[4]  = inb(0x3c5);
	 outb(0x3c4, 0x0d); s3DacRegs[5]  = inb(0x3c5);
	 outb(0x3c4, 0x15); s3DacRegs[6]  = inb(0x3c5) & 0xfe; 
	 outb(0x3c5, s3DacRegs[6]);
	 outb(0x3c4, 0x18); s3DacRegs[7]  = inb(0x3c5);

	 outb(0x3c4, 0x10); s3DacRegs[8]  = inb(0x3c5);
	 outb(0x3c4, 0x11); s3DacRegs[9]  = inb(0x3c5);
	 outb(0x3c4, 0x12); s3DacRegs[10] = inb(0x3c5);
	 outb(0x3c4, 0x13); s3DacRegs[11] = inb(0x3c5);
	 outb(0x3c4, 0x1a); s3DacRegs[12] = inb(0x3c5);
	 outb(0x3c4, 0x1b); s3DacRegs[13] = inb(0x3c5);

      /* if (S3_AURORA64VP_SERIES(s3ChipId)) { */
	 if (OFLG_ISSET(CLOCK_OPTION_S3AURORA, &s3InfoRec.clockOptions)) {
	    int i;
	    for (i=0x1a; i<= 0x6f; i++) {
	       outb(0x3c4, i); s3DacRegs[i] = inb(0x3c5);
	    }
	 }

	 outb(0x3c4, 8);
	 outb(0x3c5, 0x00);
}

static int S3_TRIO_Init(DisplayModePtr mode)
{
      int pixmux = 0;           /* SDAC command and CR67 */
      int invert_vclk = 0;      /* CR66 bit 0 */
      int sr8, sr15, sr18, cr33;
      unsigned char tmp, tmp2;
      
      outb(0x3C4, 1);
      tmp2 = inb(0x3C5);
      outb(0x3C5, tmp2 | 0x20); /* blank the screen */

      outb(0x3c4, 0x08);
      sr8 = inb(0x3c5);
      outb(0x3c5, 0x06);

      outb(0x3c4, 0x0d);        /* fix for VideoLogic GrafixStar cards: */
      tmp = inb(0x3c5);    /* disable feature connector to use 64bit DRAM bus */
      outb(0x3c5, tmp & ~1);

      outb(0x3c4, 0x15);
      sr15 = inb(0x3c5) & ~0x10;  /* XXXX ~0x40 and SynthClock /= 2 in s3.c 
						might be better... */
      outb(0x3c4, 0x18);
      sr18 = inb(0x3c5) & ~0x80;
      outb(vgaCRIndex, 0x33);
      cr33 = inb(vgaCRReg) & ~0x28;

      /* for Trio64+ we need corrected blank signal timing */
      if (!(S3_TRIO64V_SERIES(s3ChipId) && (s3ChipRev <= 0x53)
	    || (S3_TRIO64V2_SERIES(s3ChipId))) ^ 
	  !!OFLG_ISSET(OPTION_TRIO64VP_BUG1, &s3InfoRec.options)) {
	 cr33 |= 0x20;
      }

      if (s3PixelMultiplexing)
      {
	 /* x64:pixmux */
	 /* pixmux with 16/32 bpp not possible for 864 ==> only 8bit mode  */
         pixmux = 0x10;         /* two 8bit pixels per clock */
         invert_vclk = 2;       /* XXXX strange: reserved bit which helps! */
	 sr15 |= 0x10;  /* XXXX 0x40? see above! */
	 sr18 |= 0x80;
      }
      else
      {
         switch (s3InfoRec.bitsPerPixel) 
         {
            case 8:  /* 8-bit color, 1 VCLK/pixel */
               break;

            case 16: /* 15/16-bit color, 1VCLK/pixel */
	       cr33 |= 0x08;
               if (s3Weight == RGB16_555)
                  pixmux = 0x30;
               else
                  pixmux = 0x50;
               break;

            case 32: /* 32-bit color, 2VCLK/pixel */
               pixmux = 0xd0;
         }
      }

      outb(vgaCRReg, cr33);

      outb(vgaCRIndex, 0x67);
      outb(vgaCRReg, pixmux | invert_vclk);    /* set S3 mux mode */

      outb(0x3c4, 0x15);
      outb(0x3c5, sr15);
      outb(0x3c4, 0x18);
      outb(0x3c5, sr18);

      if (OFLG_ISSET(CLOCK_OPTION_S3AURORA, &s3InfoRec.clockOptions)) {
	 outb(0x3c4, 0x28);
	 outb(0x3c5, 0);
      }

      outb(0x3c4, 0x08);
      outb(0x3c5, sr8);

      outb(0x3C4, 1);
      outb(0x3C5, tmp2);        /* unblank the screen */

      return 1;
}

/******************************************************\

  			TI3030_DAC  
			TI3026_DAC 

\******************************************************/

static Bool TI3030_3026_Probe(int type)
{ 
    int found = 0;
    unsigned char saveCR55, saveCR45, saveCR43, saveID, saveTIndx;

    if (!S3_964_SERIES(s3ChipId) && !S3_968_SERIES(s3ChipId))
	return FALSE;

    outb(vgaCRIndex, 0x43);
    saveCR43 = inb(vgaCRReg);
    outb(vgaCRReg, saveCR43 & ~0x02);

    outb(vgaCRIndex, 0x45);
    saveCR45 = inb(vgaCRReg);
    outb(vgaCRReg, saveCR45 & ~0x20);

    outb(vgaCRIndex, 0x55);
    saveCR55 = inb(vgaCRReg);

    outb(vgaCRReg, (saveCR55 & 0xFC) | 0x00);
    saveTIndx = inb(0x3c8);
    outb(0x3c8, 0x3f);

    outb(vgaCRReg, (saveCR55 & 0xFC) | 0x02);
    saveID = inb(0x3c6);
    if (saveID == 0x26 || saveID == 0x30) {
	 outb(0x3c6, ~saveID);    /* check if ID reg. is read-only */
	 if (inb(0x3c6) != saveID) {
	       outb(0x3c6, saveID);
	 }
	 else {
	     /*
	      * Found TI ViewPoint 3026/3030 DAC
	      */
	      int rev;
	      outb(vgaCRReg, (saveCR55 & 0xFC) | 0x00);
	      saveTIndx = inb(0x3c8);
	      outb(0x3c8, 0x01);
	      outb(vgaCRReg, (saveCR55 & 0xFC) | 0x02);
	      rev = inb(0x3c6);
	       
	      if (saveID == 0x26)
		  found = TI3026_DAC;
	      else  /* saveID == 0x30 */
		  found = TI3030_DAC;
	      
 	      if(found == type)
	       ErrorF("%s %s: Detected a TI ViewPoint 30%02x RAMDAC rev. %x\n",
		      XCONFIG_PROBED, s3InfoRec.name, saveID, rev);
	      saveCR43 &= ~0x02;
	      saveCR45 &= ~0x20;
	 }
    }

    /* restore this mess */
    outb(vgaCRReg, (saveCR55 & 0xFC) | 0x00);
    outb(0x3c8, saveTIndx);

    outb(vgaCRIndex, 0x55);
    outb(vgaCRReg, saveCR55);

    outb(vgaCRIndex, 0x45);
    outb(vgaCRReg, saveCR45);

    outb(vgaCRIndex, 0x43);
    outb(vgaCRReg, saveCR43);

    return (found == type);
}

static Bool TI3030_Probe()
{
    return TI3030_3026_Probe(TI3030_DAC);
}

static Bool TI3026_Probe()
{
    return TI3030_3026_Probe(TI3026_DAC);
}

static int TI3030_3026_PreInit()
{
    int mclk, m, n, p;

    /* Verify that depth is supported by ramdac */
	/* all are supported */

    /* Set cursor options */
    if (OFLG_ISSET(OPTION_SW_CURSOR, &s3InfoRec.options)) {
         ErrorF("%s %s: Use of Ti3026/3030 cursor disabled in XF86Config\n",
	        XCONFIG_GIVEN, s3InfoRec.name);
	 OFLG_CLR(OPTION_TI3026_CURS, &s3InfoRec.options);
    } else {
	 /* use the ramdac cursor by default */
	 ErrorF("%s %s: Using hardware cursor from Ti3026/3030 RAMDAC\n",
	        OFLG_ISSET(OPTION_TI3026_CURS, &s3InfoRec.options) ?
		XCONFIG_GIVEN : XCONFIG_PROBED, s3InfoRec.name);
	 OFLG_SET(OPTION_TI3026_CURS, &s3InfoRec.options);
    }
   
    /* Check if PixMux is supported and set the PixMux
	related flags and variables */
    pixMuxPossible = TRUE;
    allowPixMuxInterlace = TRUE;
    allowPixMuxSwitching = FALSE;
    nonMuxMaxClock = 70000;
    pixMuxLimitedWidths = FALSE;
    if (S3_964_SERIES(s3ChipId)) {
         nonMuxMaxClock = 0;  /* 964 can only be in pixmux mode when */
         pixMuxMinWidth = 0;  /* working in enhanced mode */  
    }

    /* If there is an internal clock, set s3ClockSelectFunc, maxRawClock
	numClocks and whatever options need to be set. For external
	clocks, pass the job to OtherClocksSetup() */
   
   if(!OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions)) {
   	OFLG_SET(CLOCK_OPTION_TI3026, &s3InfoRec.clockOptions);
   	OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
   	clockchip_probed = XCONFIG_PROBED;
   }  /* there are cards with Ti3026 and EXTERNAL clock! */

   s3OutTi3026IndReg(TI_PLL_CONTROL, 0x00, 0x00);
   n = s3InTi3026IndReg(TI_MCLK_PLL_DATA) & 0x3f;
   s3OutTi3026IndReg(TI_PLL_CONTROL, 0x00, 0x04);
   m = s3InTi3026IndReg(TI_MCLK_PLL_DATA) & 0x3f;
   s3OutTi3026IndReg(TI_PLL_CONTROL, 0x00, 0x08);
   p = s3InTi3026IndReg(TI_MCLK_PLL_DATA) & 0x03;
   mclk = ((1431818 * ((65-m) * 8)) / (65-n) / (1 << p) + 50) / 100;
   if (xf86Verbose)
	ErrorF("%s %s: Using Ti3026/3030 programmable MCLK (MCLK %1.3f MHz)\n",
		clockchip_probed, s3InfoRec.name, mclk / 1000.0);
   numClocks = 3;
   if (!s3InfoRec.s3MClk)
	s3InfoRec.s3MClk = mclk;

   if (OFLG_ISSET(CLOCK_OPTION_TI3026, &s3InfoRec.clockOptions)) {
      s3ClockSelectFunc = ti3026ClockSelect;
      maxRawClock = s3InfoRec.dacSpeeds[0]; /* Is this right?? */
      OFLG_SET(CLOCK_OPTION_TI3026, &s3InfoRec.clockOptions);
      OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
      if (xf86Verbose)
	 ErrorF("%s %s: Using Ti3026/3030 programmable DCLK\n",
		clockchip_probed, s3InfoRec.name);
   }  /* there are cards with Ti3026 and EXTERNAL clock! */
   else
      OtherClocksSetup();

   clockDoublingPossible = TRUE;
   s3InfoRec.maxClock = s3InfoRec.dacSpeeds[0];
   dacOutTi3026IndReg = s3OutTi3026IndReg;
   dacInTi3026IndReg = s3InTi3026IndReg;

   return 1;
}

static void TI3030_3026_Restore()
{
      int i;

      /* select pixel clock PLL as dot clock soure */
      s3OutTi3026IndReg(TI_INPUT_CLOCK_SELECT, 0x00, TI_ICLK_PLL);

      /* programm dot clock PLL to new MCLK frequency */
      s3OutTi3026IndReg(TI_PLL_CONTROL, 0x00, 0x00);
      s3OutTi3026IndReg(TI_PIXEL_CLOCK_PLL_DATA, 0x00, s3DacRegs[3 + 0x40]);
      s3OutTi3026IndReg(TI_PIXEL_CLOCK_PLL_DATA, 0x00, s3DacRegs[4 + 0x40]);
      s3OutTi3026IndReg(TI_PIXEL_CLOCK_PLL_DATA, 0x00, s3DacRegs[5 + 0x40]);

      /* wait until PLL is locked */
      for (i=0; i<10000; i++)
	 if (s3InTi3026IndReg(TI_PIXEL_CLOCK_PLL_DATA) & 0x40) 
	    break;

      /* switch to output dot clock on the MCLK terminal */
      s3OutTi3026IndReg(0x39, 0xe7, 0x00);
      s3OutTi3026IndReg(0x39, 0xe7, 0x08);
      
      /* Set MCLK */
      s3OutTi3026IndReg(TI_PLL_CONTROL, 0x00, 0x00);
      s3OutTi3026IndReg(TI_MCLK_PLL_DATA, 0x00, s3DacRegs[3 + 0x40]);
      s3OutTi3026IndReg(TI_MCLK_PLL_DATA, 0x00, s3DacRegs[4 + 0x40]);
      s3OutTi3026IndReg(TI_MCLK_PLL_DATA, 0x00, s3DacRegs[5 + 0x40]);

      /* wait until PLL is locked */
      for (i=0; i<10000; i++) 
	 if (s3InTi3026IndReg(TI_MCLK_PLL_DATA) & 0x40) 
	    break;

      /* switch to output MCLK on the MCLK terminal */
      s3OutTi3026IndReg(0x39, 0xe7, 0x10);
      s3OutTi3026IndReg(0x39, 0xe7, 0x18);


      /* restore dot clock PLL */

      s3OutTi3026IndReg(TI_PIXEL_CLOCK_PLL_DATA, 0x00, s3DacRegs[0 + 0x40]);
      s3OutTi3026IndReg(TI_PIXEL_CLOCK_PLL_DATA, 0x00, s3DacRegs[1 + 0x40]);
      s3OutTi3026IndReg(TI_PIXEL_CLOCK_PLL_DATA, 0x00, s3DacRegs[2 + 0x40]);

      s3OutTi3026IndReg(TI_LOOP_CLOCK_PLL_DATA, 0x00, s3DacRegs[6 + 0x40]);
      s3OutTi3026IndReg(TI_LOOP_CLOCK_PLL_DATA, 0x00, s3DacRegs[7 + 0x40]);
      s3OutTi3026IndReg(TI_LOOP_CLOCK_PLL_DATA, 0x00, s3DacRegs[8 + 0x40]);

      s3OutTi3026IndReg(TI_CURS_CONTROL, 0x00, 
			s3DacRegs[TI_CURS_CONTROL]);
      s3OutTi3026IndReg(TI_INPUT_CLOCK_SELECT, 0x00,
			s3DacRegs[TI_INPUT_CLOCK_SELECT]);
      s3OutTi3026IndReg(TI_MUX_CONTROL_1, 0x00,
			s3DacRegs[TI_MUX_CONTROL_1]);
      s3OutTi3026IndReg(TI_MUX_CONTROL_2, 0x00,
			s3DacRegs[TI_MUX_CONTROL_2]);
      s3OutTi3026IndReg(TI_COLOR_KEY_CONTROL, 0x00, 
			s3DacRegs[TI_COLOR_KEY_CONTROL]);
      s3OutTi3026IndReg(TI_GENERAL_CONTROL, 0x00, 
			s3DacRegs[TI_GENERAL_CONTROL]);
      s3OutTi3026IndReg(TI_MISC_CONTROL, 0x00, 
			s3DacRegs[TI_MISC_CONTROL]);
      s3OutTi3026IndReg(TI_MCLK_LCLK_CONTROL, 0x00, 
			s3DacRegs[TI_MCLK_LCLK_CONTROL]);
      s3OutTi3026IndReg(TI_GENERAL_IO_CONTROL, 0x00, 
			s3DacRegs[TI_GENERAL_IO_CONTROL]);
      s3OutTi3026IndReg(TI_GENERAL_IO_DATA, 0x00, 
			s3DacRegs[TI_GENERAL_IO_DATA]);
}

static void TI3030_3026_Save()
{
	int i;

	  for (i=0; i<0x40; i++) 
	     s3DacRegs[i] = s3InTi3026IndReg(i);

          s3OutTi3026IndReg(TI_PLL_CONTROL, 0x00, 0x00);
          s3DacRegs[0 + 0x40] = s3InTi3026IndReg(TI_PIXEL_CLOCK_PLL_DATA);
          s3OutTi3026IndReg(TI_PLL_CONTROL, 0x00, 0x01);
          s3DacRegs[1 + 0x40] = s3InTi3026IndReg(TI_PIXEL_CLOCK_PLL_DATA);
          s3OutTi3026IndReg(TI_PLL_CONTROL, 0x00, 0x02);
          s3DacRegs[2 + 0x40] = s3InTi3026IndReg(TI_PIXEL_CLOCK_PLL_DATA);

          s3OutTi3026IndReg(TI_PLL_CONTROL, 0x00, 0x00);
          s3DacRegs[3 + 0x40] = s3InTi3026IndReg(TI_MCLK_PLL_DATA);
          s3OutTi3026IndReg(TI_PLL_CONTROL, 0x00, 0x04);
          s3DacRegs[4 + 0x40] = s3InTi3026IndReg(TI_MCLK_PLL_DATA);
          s3OutTi3026IndReg(TI_PLL_CONTROL, 0x00, 0x08);
          s3DacRegs[5 + 0x40] = s3InTi3026IndReg(TI_MCLK_PLL_DATA);

          s3OutTi3026IndReg(TI_PLL_CONTROL, 0x00, 0x00);
          s3DacRegs[6 + 0x40] = s3InTi3026IndReg(TI_LOOP_CLOCK_PLL_DATA);
          s3OutTi3026IndReg(TI_PLL_CONTROL, 0x00, 0x10);
          s3DacRegs[7 + 0x40] = s3InTi3026IndReg(TI_LOOP_CLOCK_PLL_DATA);
          s3OutTi3026IndReg(TI_PLL_CONTROL, 0x00, 0x20);
          s3DacRegs[8 + 0x40] = s3InTi3026IndReg(TI_LOOP_CLOCK_PLL_DATA);
          s3OutTi3026IndReg(TI_PLL_CONTROL, 0x00, 0x00);

 }

static int TI3030_3026_Init(DisplayModePtr mode)
{
      unsigned char tmp, tmp1, tmp2;

      outb(0x3C4, 1);
      tmp2 = inb(0x3C5);
      outb(0x3C5, tmp2 | 0x20); /* blank the screen */

      /* change polarity on S3 to pass through control to the 3020      */
      tmp = new->MiscOutReg;
      new->MiscOutReg |= 0xC0;
      tmp1 = 0x00;
      if (!(tmp & 0x80)) tmp1 |= 0x02; /* invert bits for the 3020      */
      if (!(tmp & 0x40)) tmp1 |= 0x01;
      if (s3DACSyncOnGreen) tmp1 |= 0x30;  /* add IOG sync  & 7.5 IRE   */
      s3OutTi3026IndReg(TI_GENERAL_CONTROL, 0x00, tmp1);

      if (s3DACSyncOnGreen) {  /* needed for ELSA Winner 2000PRO/X */
	 s3OutTi3026IndReg(TI_GENERAL_IO_CONTROL, 0x00, TI_GIC_ALL_BITS);
         s3OutTi3026IndReg(TI_GENERAL_IO_DATA, ~TI_GID_ELSA_SOG, 0);
      }

      outb(vgaCRIndex, 0x65);
      if (DAC_IS_TI3030)
	 outb(vgaCRReg, 0x80);
      else
	 outb(vgaCRReg, 0);

      if (s3PixelMultiplexing) {
	 int vclock,rclock;
	 /* fun timing mods for pixel-multiplexing!                     */

	 outb(vgaCRIndex, 0x40);
	 outb(vgaCRReg, 0x11);
	 outb(vgaCRIndex, 0x55);
	 outb(vgaCRReg, 0x00);

	 if (s3InfoRec.clock[mode->Clock] > 120000) {
	    vclock = TI_OCLK_V4;
	 } else if (s3InfoRec.clock[mode->Clock] > 60000){
	    vclock = TI_OCLK_V2;
	 } else {
	    vclock = TI_OCLK_V1;
	 }
	 if (s3InfoRec.bitsPerPixel >= 24) {   /* 32bpp or packed 24bpp */
	    rclock = TI_OCLK_R2;
	 } else if ((s3InfoRec.bitsPerPixel == 16) ||
		    (s3InfoRec.bitsPerPixel == 15)) {  /* 15/16bpp */
	    rclock = TI_OCLK_R4;
	 } else {
	    rclock = TI_OCLK_R8;
	 }

	 outb(vgaCRIndex, 0x66);
	 tmp = inb(vgaCRReg);
	 if (DAC_IS_TI3030)
	    tmp |= 0x60;
	 if ((mode->Flags & V_DBLCLK)
	     && OFLG_ISSET(CLOCK_OPTION_ICD2061A, &s3InfoRec.clockOptions))
	    if (s3Bpp <= 2)
	       outb(vgaCRReg, (tmp & 0xf8) | (rclock-2));
	    else
	       outb(vgaCRReg, (tmp & 0xf8) | (rclock-1));
	 else if ((mode->Flags & V_DBLCLK) && DAC_IS_TI3030)
	    outb(vgaCRReg, (tmp & 0xf8) | (rclock-1));
	 else 
		    outb(vgaCRReg, (tmp & 0xf8) | (rclock-0));

         /*
          * set the serial access mode 256 words control
          */
         outb(vgaCRIndex, 0x58);
         tmp = inb(vgaCRReg);
         outb(vgaCRReg, (tmp & 0xbf) | s3SAM256);

	 if (xf86bpp == 24) {                        /* packed 24bpp */
	    s3OutTi3026IndReg(TI_MUX_CONTROL_1, 0x00, TI_MUX1_3026T_888_P8);
	    if (DAC_IS_TI3030)
	       s3OutTi3026IndReg(TI_MUX_CONTROL_2, 0x00, 
			TI_MUX2_BUS_3030TC_D24P128);
	    else
	       s3OutTi3026IndReg(TI_MUX_CONTROL_2, 0x00, 
			TI_MUX2_BUS_3026TC_D24P64);
	    s3OutTi3026IndReg(TI_COLOR_KEY_CONTROL, 0x00, 0x01);
	 } else if (s3InfoRec.bitsPerPixel == 32) {               /* 32bpp */
	    s3OutTi3026IndReg(TI_MUX_CONTROL_1, 0x00, TI_MUX1_3026T_888);
	    if (DAC_IS_TI3030)
	       s3OutTi3026IndReg(TI_MUX_CONTROL_2, 0x00,
			 TI_MUX2_BUS_3030TC_D24P128);
	    else
	       s3OutTi3026IndReg(TI_MUX_CONTROL_2, 0x00, 
				TI_MUX2_BUS_3026TC_D24P64);
	    s3OutTi3026IndReg(TI_COLOR_KEY_CONTROL, 0x00, 0x01);
	 } else if (s3InfoRec.depth == 16) {                    /* 5-6-5 */
	    s3OutTi3026IndReg(TI_MUX_CONTROL_1, 0x00, TI_MUX1_3026T_565);
	    if (DAC_IS_TI3030)
	       s3OutTi3026IndReg(TI_MUX_CONTROL_2, 0x00, 
				TI_MUX2_BUS_3030TC_D16P128);
	    else
	       s3OutTi3026IndReg(TI_MUX_CONTROL_2, 0x00, 
				TI_MUX2_BUS_3026TC_D16P64);
	    s3OutTi3026IndReg(TI_COLOR_KEY_CONTROL, 0x00, 0x01);
	 } else if (s3InfoRec.depth == 15) {                     /* 5-5-5 */
	    s3OutTi3026IndReg(TI_MUX_CONTROL_1, 0x00, TI_MUX1_3026T_555);
	    if (DAC_IS_TI3030)
	       s3OutTi3026IndReg(TI_MUX_CONTROL_2, 0x00, 
				TI_MUX2_BUS_3030TC_D15P128);
	    else
	       s3OutTi3026IndReg(TI_MUX_CONTROL_2, 0x00, 
				TI_MUX2_BUS_3026TC_D15P64);
	    s3OutTi3026IndReg(TI_COLOR_KEY_CONTROL, 0x00, 0x01);
	 } else {
            /* set mux control 1 and 2 to provide pseudocolor sub-mode 4   */
            /* this provides a 64-bit pixel bus with 8:1 multiplexing      */
            s3OutTi3026IndReg(TI_MUX_CONTROL_1, 0x00, TI_MUX1_PSEUDO_COLOR);
	    if (DAC_IS_TI3030)
	       s3OutTi3026IndReg(TI_MUX_CONTROL_2, 0x00, 
			TI_MUX2_BUS_3030PC_D8P128);
	    else
	       s3OutTi3026IndReg(TI_MUX_CONTROL_2, 0x00, 
				TI_MUX2_BUS_3026PC_D8P64);
	 }

         /* change to 8-bit DAC and re-route the data path and clocking */
         s3OutTi3026IndReg(TI_GENERAL_IO_CONTROL, 0x00, TI_GIC_ALL_BITS);
         if (s3DAC8Bit) {
	    s3OutTi3026IndReg(TI_MISC_CONTROL , 0xF0,
			      TI_MC_INT_6_8_CONTROL | TI_MC_8_BPP);
         } else {
	    s3OutTi3026IndReg(TI_MISC_CONTROL , 0xF0, TI_MC_INT_6_8_CONTROL);
         }
	 if (OFLG_ISSET(OPTION_DIAMOND, &s3InfoRec.options)) {
	    outb(vgaCRIndex, 0x67);
	    outb(vgaCRReg, 0x01);
	    outb(vgaCRIndex, 0x6D);
	    if (s3Bpp == 1)
	       outb(vgaCRReg, 0x72);
	    else if (s3Bpp == 2)
	       outb(vgaCRReg, 0x73);
	    else /* if (s3Bpp == 4) */
	       outb(vgaCRReg, 0x75);
	 }
	 else {
	    outb(vgaCRIndex, 0x67);
	    outb(vgaCRReg, 0x00);
	    outb(vgaCRIndex, 0x6d);
	    if (s3Bpp == 1)
	       outb(vgaCRReg, 0x00);
	    else if (s3Bpp == 2)
	       outb(vgaCRReg, 0x01);
	    else /* if (s3Bpp == 4) */
	       outb(vgaCRReg, 0x00);
	 }
	 if (DAC_IS_TI3030) {
	    /* set s3 reg53 to parallel addressing by or'ing 0x20     */
	    outb(vgaCRIndex, 0x53);
	    tmp = inb(vgaCRReg);
	    outb(vgaCRReg, tmp | 0x20);
	 }
      } else {
         outb(vgaCRIndex, 0x53);
         tmp = inb(vgaCRReg);
	 if (DAC_IS_TI3030)
	    /* set s3 reg53 to parallel addressing by or'ing 0x20     */
	    outb(vgaCRReg, tmp | 0x20);
	 else
	    /* set s3 reg53 to non-parallel addressing by and'ing 0xDF     */
	    outb(vgaCRReg, tmp & ~0x20);
         /* set s3 reg55 to non-external serial by and'ing 0xF7         */
         outb(vgaCRIndex, 0x55);
         tmp = inb(vgaCRReg);
         outb(vgaCRReg, tmp & 0xF7);

         /* set mux control 1 and 2 to provide pseudocolor VGA          */
         s3OutTi3026IndReg(TI_MUX_CONTROL_1, 0x00, TI_MUX1_PSEUDO_COLOR);
         s3OutTi3026IndReg(TI_MUX_CONTROL_2, 0x00, TI_MUX2_BUS_VGA);
      }  /* end of s3PixelMultiplexing */

      /* for some reason the bios doesn't set this properly          */
      s3OutTi3026IndReg(TI_SENSE_TEST, 0x00, 0x00);

      if (OFLG_ISSET(OPTION_TI3026_CURS, &s3InfoRec.options)) {
	 /* enable interlaced cursor;
	    not very useful without CR45 bit 5 set, but anyway */
	 if (mode->Flags & V_INTERLACE) {
	    static int already = 0;
	    if (!already) {
	       already++;
	       ErrorF("%s %s: Ti3026 hardware cursor in interlaced modes "
		      "doesn't work correctly,\n"
		      "\tplease use Option \"sw_cursor\" when using "
		      "interlaced modes!\n"
		      ,XCONFIG_PROBED, s3InfoRec.name);
	    }
	    s3OutTi3026IndReg(TI_CURS_CONTROL, ~0x60, 0x60);
	 }
	 else
	    s3OutTi3026IndReg(TI_CURS_CONTROL, ~0x60, 0x00);
      }

      outb(0x3C4, 1);
      outb(0x3C5, tmp2);        /* unblank the screen */

      return 1;

}

/*********************************************************\

 			IBMRGB524_DAC 
			IBMRGB525_DAC 
			IBMRGB528_DAC 

\*********************************************************/


static Bool IBMRGB52x_Probe(int type)
{
    int ibm_id;
	 
    if (!S3_964_SERIES(s3ChipId) && !S3_968_SERIES(s3ChipId))
	return FALSE;

    if ((ibm_id = s3IBMRGB_Probe())) {
	s3IBMRGB_Init();
	switch(ibm_id >> 8) {
	    case 1:
	       if(type == IBMRGB525_DAC) {
	         ErrorF("%s %s: Detected an IBM RGB525 ramdac rev. %x\n",
		      XCONFIG_PROBED, s3InfoRec.name, ibm_id&0xff);
	         return TRUE;
               }
	       break;
	    case 2:
	       if(type == IBMRGB524_DAC) {
		  ErrorF("%s %s: Detected an IBM RGB524 ramdac rev. %x\n",
			 XCONFIG_PROBED, s3InfoRec.name, ibm_id&0xff);
		  return TRUE;
	       }
	       break;
	    case 0x102:
	       if(type == IBMRGB528_DAC) {
	          ErrorF("%s %s: Detected an IBM RGB528 ramdac rev. %x\n",
		      XCONFIG_PROBED, s3InfoRec.name, ibm_id&0xff);
		  return TRUE;
               }
	       break;
	    default:
	       break;
	}
    }

    return FALSE;
}

static Bool IBMRGB524_Probe()
{
    return IBMRGB52x_Probe(IBMRGB524_DAC);
}

static Bool IBMRGB525_Probe()
{
    return IBMRGB52x_Probe(IBMRGB525_DAC);
}

static Bool IBMRGB528_Probe()
{
    return IBMRGB52x_Probe(IBMRGB528_DAC);
}

static int IBMRGB52x_PreInit()
{
    /* Verify that depth is supported by ramdac */
	/* all are supported */

    /* Set cursor options */
    if (OFLG_ISSET(OPTION_SW_CURSOR, &s3InfoRec.options)) {
         ErrorF("%s %s: Use of IBM RGB52x cursor disabled in XF86Config\n",
	        XCONFIG_GIVEN, s3InfoRec.name);
	 OFLG_CLR(OPTION_IBMRGB_CURS, &s3InfoRec.options);
    } else {
	 /* use the ramdac cursor by default */
	 ErrorF("%s %s: Using hardware cursor from IBM RGB52x RAMDAC\n",
	        OFLG_ISSET(OPTION_IBMRGB_CURS, &s3InfoRec.options) ?
		XCONFIG_GIVEN : XCONFIG_PROBED, s3InfoRec.name);
	 OFLG_SET(OPTION_IBMRGB_CURS, &s3InfoRec.options);
    }
   
    /* Check if PixMux is supported and set the PixMux
	related flags and variables */
    pixMuxPossible = TRUE;
    allowPixMuxInterlace = TRUE;
    allowPixMuxSwitching = TRUE;
    nonMuxMaxClock = 70000;
    pixMuxLimitedWidths = FALSE;
    if (S3_964_SERIES(s3ChipId)) {
         nonMuxMaxClock = 0;  /* 964 can only be in pixmux mode when */
         pixMuxMinWidth = 0;  /* working in enhanced mode */  
    }

    /* Set Clock options, s3ClockSelectFunc, & maxRawClock 
	for internal clocks. Pass the job to the OtherClocksSetup()
	function for external clocks*/
   if (DAC_IS_IBMRGB && 
       !OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions)) {
      OFLG_SET(CLOCK_OPTION_IBMRGB, &s3InfoRec.clockOptions);
      OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
      clockchip_probed = XCONFIG_PROBED;
   }

   if (OFLG_ISSET(CLOCK_OPTION_IBMRGB, &s3InfoRec.clockOptions)) {
      char *refclock_probed;
      int mclk=0, m, n, df;
      int m0,m1,n0,n1;
      double f0,f1,fdiff;
	 
      maxRawClock = s3InfoRec.dacSpeeds[0]; /* Is this right?? */
      s3ClockSelectFunc = IBMRGBClockSelect;
      numClocks = 3;

      OFLG_SET(CLOCK_OPTION_IBMRGB, &s3InfoRec.clockOptions);
      OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);

      if (s3InfoRec.s3RefClk) 
	 refclock_probed = XCONFIG_GIVEN;
      else 
	 refclock_probed = XCONFIG_PROBED;

      if (s3InIBMRGBIndReg(IBMRGB_pll_ctrl1) & 1) {
	 m0 = s3InIBMRGBIndReg(IBMRGB_m0+0*2);
	 n0 = s3InIBMRGBIndReg(IBMRGB_n0+0*2) & 0x1f;
	 m1 = s3InIBMRGBIndReg(IBMRGB_m0+1*2);
	 n1 = s3InIBMRGBIndReg(IBMRGB_n0+1*2) & 0x1f;
      }
      else {
	 m0 = s3InIBMRGBIndReg(IBMRGB_f0+0);
	 m1 = s3InIBMRGBIndReg(IBMRGB_f0+1);
	 n0 = s3InIBMRGBIndReg(IBMRGB_pll_ref_div_fix) & 0x1f;
	 n1 = n0;
      }
      f0 = 25.175 / ((m0&0x3f)+65.0) * n0 * (8 >> (m0>>6));
      f1 = 28.322 / ((m1&0x3f)+65.0) * n1 * (8 >> (m1>>6));      
      if (f1>f0) fdiff = f1-f0;
      else       fdiff = f0-f1;


      /* refclock defaults should not depend on vendor options
       * but only on probed BIOS tags; it's too dangerous
       * when users play with options */

      if (find_bios_string(s3InfoRec.BIOSbase,"VideoBlitz III AV",
			   "Genoa Systems Corporation") != NULL) {
         if (s3BiosVendor == UNKNOWN_BIOS) 
	    s3BiosVendor = GENOA_BIOS;
	 if (xf86Verbose)
	    ErrorF("%s %s: Genoa VideoBlitz III AV BIOS found\n",
		   XCONFIG_PROBED, s3InfoRec.name);
	 if (!s3InfoRec.s3RefClk) 
	    s3InfoRec.s3RefClk = 50000;
      }
      else if (find_bios_string(s3InfoRec.BIOSbase,"STB Systems, Inc.", NULL) 
	       != NULL) {
	 if (s3BiosVendor == UNKNOWN_BIOS) 
	    s3BiosVendor = STB_BIOS;
	 if (xf86Verbose)
	    ErrorF("%s %s: STB Velocity 64 BIOS found\n",
		   XCONFIG_PROBED, s3InfoRec.name);
	 if (!s3InfoRec.s3RefClk)
	    s3InfoRec.s3RefClk = 24000;
      }
      else if (find_bios_string(s3InfoRec.BIOSbase,
				"Number Nine Visual Technology","Motion 771")
	       != NULL) {
	 if (s3BiosVendor == UNKNOWN_BIOS) 
	    s3BiosVendor = NUMBER_NINE_BIOS;
	 if (xf86Verbose)
	    ErrorF("%s %s: #9 Motion 771 BIOS found\n",
		   XCONFIG_PROBED, s3InfoRec.name);
	 if (!s3InfoRec.s3RefClk)
	    s3InfoRec.s3RefClk = 16000;
      }
      else if (OFLG_ISSET(OPTION_ELSA_W2000PRO_X8,  &s3InfoRec.options)) {
	 if (!s3InfoRec.s3RefClk)
	    s3InfoRec.s3RefClk = 28322;
      }
      else if (OFLG_ISSET(OPTION_MIRO_80SV,  &s3InfoRec.options)) {
	 if (!s3InfoRec.s3RefClk)
	    s3InfoRec.s3RefClk = 16000;
      }
      else if (find_bios_string(s3InfoRec.BIOSbase,
				"Hercules Graphite Terminator",NULL) != NULL) {
	 if (s3BiosVendor == UNKNOWN_BIOS) 
	    s3BiosVendor = HERCULES_BIOS;
	 if (xf86Verbose)
	    ErrorF("%s %s: Hercules Graphite Terminator BIOS found\n",
		   XCONFIG_PROBED, s3InfoRec.name);
	 if (!s3InfoRec.s3RefClk)
	    if (S3_968_SERIES(s3ChipId))
	       /* Hercules Graphite Terminator Pro(tm) BIOS. */
	       s3InfoRec.s3RefClk = 16000;
	    else		/* S3 964 */
	       /* Hercules Graphite Terminator 64(tm) BIOS. */
	       s3InfoRec.s3RefClk = 50000;
      }
      else if (!s3InfoRec.s3RefClk) {
	 int f, i, rclks[] = { 14318, 16000, 24000, 28322, 50000, 0 };
	 if (fdiff > f0*0.02)
	    ErrorF("%s %s: WARNING: probed refclocks are too different"
		   "\tused refclock value might be wrong...\n",
		   XCONFIG_PROBED, s3InfoRec.name);

	 /* 28.322 MHz clock seems to be more acurate then 25.175 */
	 /* f = (f0+f1)/2 * 1e3 + 0.5; */ 
	 f = f1 * 1e3 + 0.5;

	 /* try to match some known reclock values */
	 for (i=0; rclks[i]; i++)
	    if (abs((int)(f - rclks[i])) <= 100) {
	       s3InfoRec.s3RefClk = rclks[i];
	       break;
	    }
	 if (!s3InfoRec.s3RefClk)
	    s3InfoRec.s3RefClk = (f/500)*500;
      }
      
      if (!DAC_IS_IBMRGB525) {
	 m = s3InIBMRGBIndReg(IBMRGB_sysclk_vco_div);
	 n = s3InIBMRGBIndReg(IBMRGB_sysclk_ref_div) & 0x1f;
	 df = m>>6;
	 m &= 0x3f;
	 if (!n) { m=0; n=1; }
	 mclk = ((s3InfoRec.s3RefClk*100 * (m+65)) / n / (8 >> df) + 50) / 100;
      }
      if (xf86Verbose) {
	 ErrorF("%s %s: Using IBM RGB52x programmable clock",
		clockchip_probed, s3InfoRec.name);
	 if (mclk)
	    ErrorF(" (MCLK %1.3f MHz)", mclk / 1000.0);
	 ErrorF("\n");	
	 ErrorF("%s %s: with refclock %1.3f MHz (probed %1.3f & %1.3f)\n",
		refclock_probed,s3InfoRec.name,s3InfoRec.s3RefClk/1e3,f0,f1);
      }
      if (!s3InfoRec.s3MClk)
	 s3InfoRec.s3MClk = mclk;
   } 
   else 
       OtherClocksSetup();

   clockDoublingPossible = FALSE;
      /* LCLK & SCLK limit is 100 MHz */
   if ((   OFLG_ISSET(OPTION_ELSA_W2000PRO_X8,  &s3InfoRec.options)
	|| OFLG_ISSET(OPTION_MIRO_80SV,  &s3InfoRec.options))
       && s3Bpp > 2)
         s3InfoRec.maxClock = 220000;
   else if ((s3InfoRec.dacSpeeds[0] * s3Bpp) / 8 > 100000)
	 s3InfoRec.maxClock = (100000 * 8) / s3Bpp; 
   else
	 s3InfoRec.maxClock = s3InfoRec.dacSpeeds[0];

   return 1;
}

static void IBMRGB52x_Restore()
{
      int i;

      if (DAC_IS_IBMRGB525) {
	 s3OutIBMRGBIndReg(IBMRGB_vram_mask_0, 0, 3);
	 s3OutIBMRGBIndReg(IBMRGB_misc1, ~0x40, 0x40);
	 usleep (1000);
	 s3OutIBMRGBIndReg(IBMRGB_misc2, ~1, 0);
      }
      for (i=0; i<0x100; i++)
	 s3OutIBMRGBIndReg(i, 0, s3DacRegs[i]);
      outb(vgaCRIndex, 0x22);
      outb(vgaCRReg, s3DacRegs[0x100]);   
}

static void IBMRGB52x_Save()
{
	int i;

	for (i=0; i<0x100; i++)
	    s3DacRegs[i] = s3InIBMRGBIndReg(i);
	outb(vgaCRIndex, 0x22);
	s3DacRegs[0x100] = inb(vgaCRReg);
}

static int IBMRGB52x_Init(DisplayModePtr mode)
{
      unsigned char tmp, tmp2;

      outb(0x3C4, 1);
      tmp2 = inb(0x3C5);
      outb(0x3C5, tmp2 | 0x20); /* blank the screen */

      if (DAC_IS_IBMRGB528) {
	 if (OFLG_ISSET(OPTION_MIRO_80SV,  &s3InfoRec.options)) {
	    if (xf86bpp == 32)
	       s3OutIBMRGBIndReg(IBMRGB_misc_clock,    0, 0x43);
	    else if (xf86bpp == 8)
	       if (mode->Flags & V_DBLCLK)
		  s3OutIBMRGBIndReg(IBMRGB_misc_clock, 0, 0x47);
	       else
		  s3OutIBMRGBIndReg(IBMRGB_misc_clock, 0, 0x45);
	    else /* 15/16 bpp */
	       if (mode->Flags & V_DBLCLK)
		  s3OutIBMRGBIndReg(IBMRGB_misc_clock, 0, 0x45);
	       else
		  s3OutIBMRGBIndReg(IBMRGB_misc_clock, 0, 0x43);
	 }
	 else {
	    if (s3InfoRec.clock[mode->Clock] <=  110000)
	       s3OutIBMRGBIndReg(IBMRGB_misc_clock, 0xf0, 0x01);
	    else if (s3InfoRec.clock[mode->Clock] <=  220000)
	       s3OutIBMRGBIndReg(IBMRGB_misc_clock, 0xf0, 0x03);
	    else
	       s3OutIBMRGBIndReg(IBMRGB_misc_clock, 0xf0, 0x05);
	 }
      }
      else if (mode->Flags & V_DBLCLK)
	 s3OutIBMRGBIndReg(IBMRGB_misc_clock, 0xf0, 0x03);
      else
	 s3OutIBMRGBIndReg(IBMRGB_misc_clock, 0xf0, 0x01);

      s3OutIBMRGBIndReg(IBMRGB_sync, 0, 0);
      if ((mode->Private[0] & (1 << S3_BLANK_DELAY))
	  && S3_968_SERIES(s3ChipId)) {
	 int pixels = (mode->Private[S3_BLANK_DELAY] & 0x07) * 8 / s3Bpp;
	 if (pixels > 15) pixels = 15;
	 s3OutIBMRGBIndReg(IBMRGB_hsync_pos, 0, pixels);
      }
      else
	 s3OutIBMRGBIndReg(IBMRGB_hsync_pos, 0, 0);
      s3OutIBMRGBIndReg(IBMRGB_pwr_mgmt, 0, 0);
      s3OutIBMRGBIndReg(IBMRGB_dac_op, ~8, s3DACSyncOnGreen ? 8 : 0);
      s3OutIBMRGBIndReg(IBMRGB_dac_op, ~2, 1 /* fast slew */ ? 2 : 0);
      s3OutIBMRGBIndReg(IBMRGB_pal_ctrl, 0, 0);
      /* set VRAM size to 128/64 bit and disable VRAM mask */
      if (DAC_IS_IBMRGB528)
	 s3OutIBMRGBIndReg(IBMRGB_misc1, ~0x43, 3);
      else
	 s3OutIBMRGBIndReg(IBMRGB_misc1, ~0x43, 1);
      if (s3DAC8Bit)
	 s3OutIBMRGBIndReg(IBMRGB_misc2, 0, 0x47);
      else
	 s3OutIBMRGBIndReg(IBMRGB_misc2, 0, 0x43);

#if 0  /* this will lock up the S3 chip & PC sometimes */
      outb(vgaCRIndex, 0x22); /* don't know why it's important    */
      outb(vgaCRReg, 0xff);   /* to set a "read only" register ?? */
#else
      outb(vgaCRIndex, 0x22);
      tmp = inb(vgaCRReg);
      if (OFLG_ISSET(OPTION_MIRO_80SV,  &s3InfoRec.options))
	 if (s3Bpp == 1)
	    outb(vgaCRReg, tmp | 8);
	 else
	    outb(vgaCRReg, tmp & ~8);
      else if (s3Bpp == 1 && S3_968_SERIES(s3ChipId) && !DAC_IS_IBMRGB528)
	 outb(vgaCRReg, tmp | 8);
      else 
	 outb(vgaCRReg, tmp & ~8);
#endif

      outb(vgaCRIndex, 0x65);
      if (DAC_IS_IBMRGB528)
	 if (OFLG_ISSET(OPTION_MIRO_80SV,  &s3InfoRec.options))
	    outb(vgaCRReg, 0x82);
	 else
	    outb(vgaCRReg, 0x80);
      else
	 outb(vgaCRReg, 0);

      if (s3PixelMultiplexing) {
	 outb(vgaCRIndex, 0x40);
	 outb(vgaCRReg, 0x11);
	 outb(vgaCRIndex, 0x55);
	 outb(vgaCRReg, 0x00);

	 if (s3InfoRec.depth == 24 || s3InfoRec.depth == 32) { /* 24 bpp */
	    s3OutIBMRGBIndReg(IBMRGB_pix_fmt, 0xf8, 6);
	    s3OutIBMRGBIndReg(IBMRGB_32bpp, 0, 0);
	 } else if (s3InfoRec.depth == 16) {             /* 16 bpp */
	    s3OutIBMRGBIndReg(IBMRGB_pix_fmt, 0xf8, 4);
	    s3OutIBMRGBIndReg(IBMRGB_16bpp, 0, 0x02);
	 } else if (s3InfoRec.depth == 15) {             /* 15 bpp */
	    s3OutIBMRGBIndReg(IBMRGB_pix_fmt, 0xf8, 4);
	    s3OutIBMRGBIndReg(IBMRGB_16bpp, 0, 0x00);
	 } else {                                        /*  8 bpp */
	    s3OutIBMRGBIndReg(IBMRGB_pix_fmt, 0xf8, 3);
	    s3OutIBMRGBIndReg(IBMRGB_8bpp, 0, 0);
	 }
	 /* if (DAC_IS_IBMRGB528) tmp++; */

	 outb(vgaCRIndex, 0x66);
	 tmp = inb(vgaCRReg) & 0xf8;
	 if (DAC_IS_IBMRGB528) {
	    int tmp2;
	    tmp = tmp & 0x80 | 0x60;   /* 128 bit SID mode */
	    if (OFLG_ISSET(OPTION_MIRO_80SV,  &s3InfoRec.options)) {
	       if (s3Bpp < 4 && !(mode->Flags & V_DBLCLK)) tmp++;
	    }
	    else {
	       if (s3InfoRec.clock[mode->Clock] <= 110000) tmp2 = 3;
	       else if (s3InfoRec.clock[mode->Clock] <= 220000) tmp2 = 2;
	       else tmp2 = 1;
	       if (s3Bpp == 2) tmp2--;
	       else if (s3Bpp == 4) {
		  if (tmp2 >= 2) tmp2 -= 2;
		  else tmp2 = 0;
	       }
	       tmp |= tmp2;
	    }
	 }
	 else if (!S3_968_SERIES(s3ChipId)) {
	   if (s3Bpp == 1) tmp |= 3;
	   else if (s3Bpp == 2) tmp |= 2;
	   else /* if (s3Bpp == 4) */ tmp |= 1;
	   if (mode->Flags & V_DBLCLK) tmp--;
	 }

	 outb(vgaCRReg, tmp);

         /*
          * set the serial access mode 256 words control
          */
         outb(vgaCRIndex, 0x58);
         tmp = inb(vgaCRReg);
         outb(vgaCRReg, (tmp & 0xbf) | s3SAM256);

	 outb(vgaCRIndex, 0x67);
	 if (DAC_IS_IBMRGB528)
	    if (OFLG_ISSET(OPTION_MIRO_80SV,  &s3InfoRec.options)) {
	       if (s3Bpp == 2) 
		  if (mode->Flags & V_DBLCLK)
		     outb(vgaCRReg, 0x18);
		  else
		     outb(vgaCRReg, 0x10);
	       else
		  outb(vgaCRReg, 0x00);
	    }
	    else if (s3Bpp == 1)
	       outb(vgaCRReg, 0x01);
	    else
	       outb(vgaCRReg, 0x00);
	 else if (s3Bpp == 4)
	    outb(vgaCRReg, 0x00);
	 else
	    if (S3_968_SERIES(s3ChipId))
	       outb(vgaCRReg, 0x11);
	    else
	       outb(vgaCRReg, 0x01);

	 outb(vgaCRIndex, 0x6d);
	 if (s3Bpp == 1)
	    outb(vgaCRReg, 0x21);
	 else if (s3Bpp == 2)
	    outb(vgaCRReg, 0x10);
	 else /* if (s3Bpp == 4) */
	    outb(vgaCRReg, 0x00);

      } else {
         /* set s3 reg53 to non-parallel addressing by and'ing 0xDF     */
         outb(vgaCRIndex, 0x53);
         tmp = inb(vgaCRReg);
         outb(vgaCRReg, tmp & 0xDF);

         /* set s3 reg55 to non-external serial by and'ing 0xF7         */
         outb(vgaCRIndex, 0x55);
         tmp = inb(vgaCRReg);
         outb(vgaCRReg, tmp & 0xF7);

         /* provide pseudocolor VGA          */
         s3OutIBMRGBIndReg(IBMRGB_misc2, 0, 0);
      }  /* end of s3PixelMultiplexing */

      if (DAC_IS_IBMRGB528) {
         /* set s3 reg53 to parallel addressing   */
         outb(vgaCRIndex, 0x53);
         tmp = inb(vgaCRReg);
         outb(vgaCRReg, tmp | 0x20);
      }

#if 0
      if (OFLG_ISSET(OPTION_IBMRGB_CURS, &s3InfoRec.options)) {
	 /* enable interlaced cursor;
	    not very useful without CR45 bit 5 set, but anyway */
	 if (mode->Flags & V_INTERLACE) {
	    static int already = 0;
	    if (!already) {
	       already++;
	       ErrorF("%s %s: IBMRGB hardware cursor in interlaced modes "
		      "doesn't work correctly,\n"
		      "\tplease use Option \"sw_cursor\" when using "
		      "interlaced modes!\n"
		      ,XCONFIG_PROBED, s3InfoRec.name);
	    }
	    s3OutIBMRGBIndReg(TI_CURS_CONTROL, ~0x60, 0x60);
	 }
	 else
	    s3OutIBMRGBIndReg(TI_CURS_CONTROL, ~0x60, 0x00);
      }
#endif

      outb(0x3C4, 1);
      outb(0x3C5, tmp2);        /* unblank the screen */

      return 1;
}

/***********************************************************\

  			ATT20C490_DAC  
			SS2410_DAC  
			SC1148x_M2_DAC  

\***********************************************************/

#define Setcomm(v)        (xf86dactocomm(),outb(0x3C6,v),\
                        xf86dactocomm(),inb(0x3C6))

static Bool MISC_HI_COLOR_Probe(int type)
{
    int found = 0;
    unsigned char tmp, olddacpel, olddaccomm, notdaccomm;

    /*quick check*/
    if(!S3_8XX_9XX_SERIES(s3ChipId)) 
	return FALSE;
	
    (void) xf86dactocomm();
    olddaccomm = inb(0x3C6);
    xf86dactopel();
    olddacpel = inb(0x3C6);

    notdaccomm = ~olddaccomm;
    outb(0x3C6, notdaccomm);
    (void) xf86dactocomm();

    if (inb(0x3C6) != notdaccomm) {
            /* Looks like a HiColor RAMDAC */
            if ((Setcomm(0xE0) & 0xE0) == 0xE0) {
               if ((Setcomm(0x60) & 0xE0) == 0x00) {
                  if ((Setcomm(0x02) & 0x02) != 0x00) {
                     found = ATT20C490_DAC;
                  }
               } else {
                  tmp = Setcomm(olddaccomm);
                  if (inb(0x3C6) != notdaccomm) {
                     (void) inb(0x3C6);
                     (void) inb(0x3C6);
                     (void) inb(0x3C6);
                     if (inb(0x3C6) != notdaccomm) {
                        xf86dactopel();
                        outb(0x3C6, 0xFF);
                        (void) inb(0x3C6);
                        (void) inb(0x3C6);
                        (void) inb(0x3C6);
                        switch (inb(0x3C6)) {
                        case 0x44:
                        case 0x82:
			    break;
                        case 0x8E:
			    found = SS2410_DAC;		
			    break;
                        default:
                            xf86dactopel();
                            outb(0x3C6, olddacpel & 0xFB);
                            xf86dactocomm();
                            outb(0x3C6, olddaccomm & 0x04);
                            tmp = inb(0x3C6);
                            outb(0x3C6, tmp & 0xFB);
                            if ((tmp & 0x04) == 0) {
                               found = SC1148x_M2_DAC;
                            }
                        }
                     }
                  }
               }
            }
    }
    (void) xf86dactocomm();
    outb(0x3C6, olddaccomm);
    xf86dactopel();
    outb(0x3C6, olddacpel);

    return (found == type);
}
#undef Setcomm

static Bool SC1148x_Probe()
{
    return MISC_HI_COLOR_Probe(SC1148x_M2_DAC);
}

static Bool SS2410_Probe()
{
    return MISC_HI_COLOR_Probe(SS2410_DAC);
}

static Bool ATT20C490_Probe()
{
    return MISC_HI_COLOR_Probe(ATT20C490_DAC);
}

static int MISC_HI_COLOR_PreInit()
{
    /* Verify that depth is supported by ramdac */
    switch(s3RamdacType) {
	case SC1148x_M2_DAC:
    	    if ( s3InfoRec.depth > 16 ) {
		ErrorF("Depths greater than 16bpp are not supported for "
		   	"the Sierra 1148x RAMDAC.\n");
	        return 0;
    	    }
	    break;
	case SS2410_DAC:
    	    if ( s3InfoRec.depth > 24 ) {
		ErrorF("Depths greater than 24bpp are not supported for "
		    "the Diamond SS2410 RAMDAC.\n");
		return 0;
    	     }
	    break;
	case ATT20C490_DAC:
    	    if (s3Bpp > 2) {
		ErrorF("Depths greater than 16bpp are not supported for "
		    "the ATT20C490 RAMDAC.\n");
		return 0;
    	     }
	    break;
	default:
	    ErrorF("Internal error in MISC_HI_COLOR_PreInit().\n");
		return 0;
     }	


    /* Set cursor options */
   	/* none */

    /* Check if PixMux is supported and set the PixMux
	related flags and variables */
	/* No PixMux */

    /* Set Clock options, s3ClockSelectFunc, & maxRawClock 
	for internal clocks. Pass the job to the OtherClocksSetup()
	function for external clocks*/
    OtherClocksSetup();

    s3InfoRec.maxClock = s3InfoRec.dacSpeeds[0];
    /* Halve it for 16bpp (32bpp not supported) */
    if (s3Bpp > 1) {
	 s3InfoRec.maxClock /= 2;
	 maxRawClock /= 2;
    }

    return 1;
}


static void SC1148x_Restore()
{
   xf86setdaccomm(s3DacRegs[0]);
}

static void ATT20C490_Restore()
{
    xf86setdaccomm(s3DacRegs[0]);
}

static void SS2410_Restore()
{
    unsigned char tmp;

    outb( vgaCRIndex, 0x55 );
    tmp = inb( vgaCRReg );
    outb( vgaCRReg, tmp | 1 ); 
    xf86setdaccomm(s3DacRegs[0]);
    outb( vgaCRReg, tmp );
}

static void SC1148x_Save()
{
   s3DacRegs[0] = xf86getdaccomm();
}

static void ATT20C490_Save()
{
   s3DacRegs[0] = xf86getdaccomm();
}

static void SS2410_Save()
{
	unsigned char tmp;

 	outb( vgaCRIndex, 0x55 );
	tmp = inb( vgaCRReg );
	outb( vgaCRReg, tmp | 1 ); 
	s3DacRegs[0] = xf86getdaccomm( );
	outb( vgaCRReg, tmp );
}

static int SC1148x_Init(DisplayModePtr mode)
{
   if (s3InfoRec.bitsPerPixel == 8) {
      xf86clrdaccommbit(0xe0);
   } else {
      if (s3InfoRec.depth == 16) {
	xf86setdaccommbit(0xe0);
      } else if (s3InfoRec.depth == 15) {
        xf86setdaccommbit(0xa0);
      } else {
	ErrorF("Depths greater than 16bpp are not supported by the SC1148x!\n");
	return 0;
      }
   }
   return 1;
}

static int ATT20C490_Init(DisplayModePtr mode)
{
      if (s3InfoRec.bitsPerPixel == 8) {
	if (s3DAC8Bit) {
         xf86setdaccomm(0x02);
	} else {
	 xf86setdaccomm(0x00);
	}
      } else {
	switch (s3InfoRec.depth) {
	  case 15:
		xf86setdaccomm(0xa0);
                break;
          case 16:
		xf86setdaccomm(0xc0);
                break;
          case 24:
          case 32:
		xf86setdaccomm(0xe0);  /* XXXX just a guess, check !!! */
                break;
          default:
		ErrorF("Bad depth %i!\n",s3InfoRec.depth);
		return 0;
	}
      }  
      return 1;  
}

static int SS2410_Init(DisplayModePtr mode)
{
    register unsigned char tmp;

	if ( s3InfoRec.bitsPerPixel == 8 ) {
 		outb( vgaCRIndex, 0x55 );
		tmp = inb( vgaCRReg );
		outb( vgaCRReg, tmp | 1 ); 
		xf86setdaccomm( 0 );
		outb( vgaCRReg, tmp );
	} else {
	     switch ( s3InfoRec.depth ) { 
		   case 15:
 			outb( vgaCRIndex, 0x55 );
			tmp = inb( vgaCRReg );
			outb( vgaCRReg, tmp | 1 ); 
			xf86setdaccomm( 0xA0 );
			outb( vgaCRReg, tmp );
			break;
		   case 16:
 			outb( vgaCRIndex, 0x55 );
			tmp = inb( vgaCRReg );
			outb( vgaCRReg, tmp | 1 ); 
			xf86setdaccomm( 0xA6 );
			outb( vgaCRReg, tmp );
			break;
		   case 24:
 			outb( vgaCRIndex, 0x55 );
			tmp = inb( vgaCRReg );
			outb( vgaCRReg, tmp | 1 ); 
			xf86setdaccomm( 0x9E );
			outb( vgaCRReg, tmp );
			break;
		   default:
			ErrorF("Bad depth %i!\n",s3InfoRec.depth);
			return 0;
		
	     }
     	}

	return 1;
}

/*****************************************************\

  			NORMAL_DAC 

\*****************************************************/

static Bool NORMAL_Probe()
{
    return TRUE;
}

static int NORMAL_PreInit()
{
    /* Verify that depth is supported by ramdac */
    if(s3Bpp > 1) {
	ErrorF("Depths greater than 8bpp are not supported for a "
	"\"normal\" RAMDAC.\n");
	return 0;	/* BAD_DEPTH */
    }

    /* Set cursor options */
   	/* none */

    /* Check if PixMux is supported and set the PixMux
	related flags and variables */
	/* of course not */

    /* If there is an internal clock, set s3ClockSelectFunc, maxRawClock
	numClocks and whatever options need to be set. For external
	clocks, pass the job to OtherClocksSetup() */
    OtherClocksSetup();
    
    /* anything else */
    s3InfoRec.maxClock = s3MaxClock;

    return 1;
}



/**********************************************************\

	THE CLOCKS

\**********************************************************/

static
void OtherClocksSetup()
{
	/*******************************************\
	|  Clocks that aren't built into the ramdac |
	\*******************************************/

   if (OFLG_ISSET(OPTION_LEGEND, &s3InfoRec.options)) {
      s3ClockSelectFunc = LegendClockSelect;
      numClocks = 32;
/*      maxRawClock = ;		(MArk) what to do here? */
      if (xf86Verbose)
         ErrorF("%s %s: Using Sigma Legend ICS 1394-046 clock generator\n",
		clockchip_probed, s3InfoRec.name);
   } else if (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &s3InfoRec.clockOptions)) {
      s3ClockSelectFunc = icd2061ClockSelect;
      numClocks = 3;
      maxRawClock = 120000;
      if (xf86Verbose)
         ErrorF("%s %s: Using ICD2061A programmable clock\n",
		clockchip_probed, s3InfoRec.name);
   } else if (OFLG_ISSET(CLOCK_OPTION_GLORIA8, &s3InfoRec.clockOptions)) {
      s3ClockSelectFunc = Gloria8ClockSelect;
      numClocks = 3;
/*      maxRawClock = ;		(MArk) what to do here? */
      if (xf86Verbose)
         ErrorF("%s %s: Using ELSA Gloria-8 ICS9161/TVP3030 programmable " 	
		"clock\n",clockchip_probed, s3InfoRec.name);
   } else if (OFLG_ISSET(CLOCK_OPTION_SC11412, &s3InfoRec.clockOptions)) {
      s3ClockSelectFunc = icd2061ClockSelect;
      numClocks = 3;
      if (OFLG_ISSET(CLOCK_OPTION_SC11412, &s3InfoRec.clockOptions)) {
	 if (!OFLG_ISSET(OPTION_SPEA_MERCURY, &s3InfoRec.options)) {
	    switch (s3RamdacType) {
	    case BT485_DAC:
	       maxRawClock = 67500;
	       break;
	    case ATT20C505_DAC:
	       maxRawClock = 90000;
	       break;
	    default:
	       maxRawClock = 100000;
	       break;
	    }
	 } else {
	    maxRawClock = 100000;
	 }
      }
      if (xf86Verbose)
	 ErrorF("%s %s: Using Sierra SC11412 programmable clock\n",
		clockchip_probed, s3InfoRec.name);
   } else if (OFLG_ISSET(CLOCK_OPTION_ICS2595, &s3InfoRec.clockOptions)) {
      s3ClockSelectFunc = icd2061ClockSelect;
      numClocks = 3;
      maxRawClock = 145000; /* This is what is in common_hw/ICS2595.h */
      if (xf86Verbose)
	 ErrorF("%s %s: Using ICS2595 programmable clock\n",
		XCONFIG_GIVEN, s3InfoRec.name);
   } else if (OFLG_ISSET(CLOCK_OPTION_CH8391, &s3InfoRec.clockOptions)) {
      s3ClockSelectFunc = ch8391ClockSelect;
      numClocks = 3;
      maxRawClock = 135000;
      if (xf86Verbose)
	 ErrorF("%s %s: Using Chrontel 8391 programmable clock\n",
		XCONFIG_GIVEN, s3InfoRec.name);
   } else if (OFLG_ISSET(CLOCK_OPTION_S3GENDAC, &s3InfoRec.clockOptions) ||
	      OFLG_ISSET(CLOCK_OPTION_ICS5342,  &s3InfoRec.clockOptions)) {
      unsigned char saveCR55;
      int m,n,n1,n2, mclk;
      
      s3ClockSelectFunc = s3GendacClockSelect;
      numClocks = 3;
      maxRawClock = 110000;
      
      outb(vgaCRIndex, 0x55);
      saveCR55 = inb(vgaCRReg);
      outb(vgaCRReg, saveCR55 | 1);
      
      outb(0x3C7, 10); /* read MCLK */
      m = inb(0x3C9);
      n = inb(0x3C9);
      
      outb(vgaCRIndex, 0x55);
      outb(vgaCRReg, saveCR55);	 
      
      m &= 0x7f;
      n1 = n & 0x1f;
      n2 = (n>>5) & 0x03;
      mclk = ((1431818 * (m+2)) / (n1+2) / (1 << n2) + 50) / 100;

      if (xf86Verbose)
	 ErrorF("%s %s: Using %s programmable clock (MCLK %1.3f MHz)\n"
		,clockchip_probed, s3InfoRec.name
		,OFLG_ISSET(CLOCK_OPTION_ICS5342, &s3InfoRec.clockOptions)
		? "ICS5342" : "S3 Gendac/SDAC"
		,mclk / 1000.0);
      if (s3InfoRec.s3MClk > 0) {
       if (xf86Verbose)
	 ErrorF("%s %s: using specified MCLK value of %1.3f MHz for DRAM "
 	"timings\n",XCONFIG_GIVEN, s3InfoRec.name, s3InfoRec.s3MClk/1000.0);
      } else {
	 s3InfoRec.s3MClk = mclk;
      }
   } else {
      s3ClockSelectFunc = s3ClockSelect;
      numClocks = 16;
      if (!s3InfoRec.clocks) 
         vgaGetClocks(numClocks, s3ClockSelectFunc);
   }

}



static Bool
s3ClockSelect(no)
     int   no;

{
   unsigned char temp;
   static unsigned char save1, save2;
 
   UNLOCK_SYS_REGS;
   
   switch(no)
   {
   case CLK_REG_SAVE:
      save1 = inb(0x3CC);
      outb(vgaCRIndex, 0x42);
      save2 = inb(vgaCRReg);
      break;
   case CLK_REG_RESTORE:
      outb(0x3C2, save1);
      outb(vgaCRIndex, 0x42);
      outb(vgaCRReg, save2);
      break;
   default:
      no &= 0xF;
      if (no == 0x03)
      {
	 /*
	  * Clock index 3 is a 0Hz clock on all the S3-recommended 
	  * synthesizers (except the Chrontel).  A 0Hz clock will lock up 
	  * the chip but good (requiring power to be cycled).  Nuke that.
	  */
         LOCK_SYS_REGS;
	 return(FALSE);
      }
      temp = inb(0x3CC);
      outb(0x3C2, temp | 0x0C);
      outb(vgaCRIndex, 0x42);
      temp = inb(vgaCRReg) & 0xf0;
      outb(vgaCRReg, temp | no);
      usleep(150000);
   }
   LOCK_SYS_REGS;
   return(TRUE);
}



static Bool
LegendClockSelect(no)
     int   no;
{

 /*
  * Sigma Legend special handling
  * 
  * The Legend uses an ICS 1394-046 clock generator.  This can generate 32
  * different frequencies.  The Legend can use all 32.  Here's how:
  * 
  * There are two flip/flops used to latch two inputs into the ICS clock
  * generator.  The five inputs to the ICS are then
  * 
  * ICS     ET-4000 ---     --- FS0     CS0 FS1     CS1 FS2     ff0 flip/flop 0
  * outbut FS3     CS2 FS4     ff1     flip/flop 1 outbut
  * 
  * The flip/flops are loaded from CS0 and CS1.  The flip/flops are latched by
  * CS2, on the rising edge. After CS2 is set low, and then high, it is then
  * set to its final value.
  * 
  */
   static unsigned char save1, save2;
   unsigned char temp = inb(0x3CC);

   switch(no)
   {
   case CLK_REG_SAVE:
      save1 = inb(0x3CC);
      outb(vgaCRIndex, 0x34);
      save2 = inb(vgaCRReg);
      break;
   case CLK_REG_RESTORE:
      outb(0x3C2, save1);
      outw(vgaCRIndex, 0x34 | (save2 << 8));
      break;
   default:
      outb(0x3C2, (temp & 0xF3) | ((no & 0x10) >> 1) | (no & 0x04));
      outw(vgaCRIndex, 0x0034);
      outw(vgaCRIndex, 0x0234);
      outw(vgaCRIndex, ((no & 0x08) << 6) | 0x34);
      outb(0x3C2, (temp & 0xF3) | ((no << 2) & 0x0C));
   }
   return(TRUE);
}




static Bool
icd2061ClockSelect(freq)
     int   freq;
{
   Bool result = TRUE;

   UNLOCK_SYS_REGS;
   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = s3ClockSelect(freq);
      break;
   default:
      {
	 /* Convert freq to Hz */
	 freq *= 1000;
	 /* Use the "Alt" version always since it is more reliable */
	 if (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &s3InfoRec.clockOptions)) {
	    /* setting exactly 120 MHz doesn't work all the time */
	    if (freq > 119900000) freq = 119900000;
#if defined(PC98_PW) || defined(PC98_PWLB)
	    AltICD2061SetClock(freq, 0);
#else
	    AltICD2061SetClock(freq, 2);
	    AltICD2061SetClock(freq, 2);
	    AltICD2061SetClock(freq, 2);
#endif
	    if (DAC_IS_TI3026 || DAC_IS_TI3030) {
	       /* 
	        * then we need to setup the loop clock
	        */
	       Ti3026SetClock(freq/1000, 2, s3Bpp, TI_LOOP_CLOCK);
	       s3OutTi3026IndReg(TI_MCLK_LCLK_CONTROL, ~0x20, 0x20);
            }
	 } else if (OFLG_ISSET(CLOCK_OPTION_SC11412, &s3InfoRec.clockOptions)) {
	    result = SC11412SetClock((long)freq/1000);
	 } else if (OFLG_ISSET(CLOCK_OPTION_ICS2595, &s3InfoRec.clockOptions)) {
	    result = ICS2595SetClock((long)freq/1000);
	    result = ICS2595SetClock((long)freq/1000);
	 } else { /* Should never get here */
	    result = FALSE;
	    break;
	 }

	 if (!OFLG_ISSET(CLOCK_OPTION_ICS2595, &s3InfoRec.clockOptions)) {
	    unsigned char tmp;
	    outb(vgaCRIndex, 0x42);/* select the clock */
	    tmp = inb(vgaCRReg) & 0xf0;
	    if (OFLG_ISSET(OPTION_SPEA_MERCURY, &s3InfoRec.options) &&
                S3_964_SERIES(s3ChipId)) /* SPEA Mercury P64 uses bit2/3  */
                 outb(vgaCRReg, tmp | 0x06);   /* for synchronizing reasons (?) */
            else outb(vgaCRReg, tmp | 0x02); 
            usleep(150000);
	 }
	 /* Do the clock doubler selection in s3Init() */
      }
   }
   LOCK_SYS_REGS;
   return(result);
}




/* the ELSA Gloria-8 uses a TVP3030 with ICS9161 as refclock */

static Bool
Gloria8ClockSelect(freq)
     int   freq;
{
   Bool result = TRUE;
   unsigned char tmp;
   int p;

   UNLOCK_SYS_REGS;
   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = s3ClockSelect(freq);
      break;
   default:
      for(p=0; p<4; p++)
	 if ((freq << p) >= 120000) break;

      AltICD2061SetClock((freq * 1000) >> (3-p), 2);

      Ti3030SetClock(14318 << (3-p), 2, s3Bpp, TI_BOTH_CLOCKS);
      Ti3030SetClock(freq, 2, s3Bpp, TI_LOOP_CLOCK);
      s3OutTi3026IndReg(TI_MCLK_LCLK_CONTROL, ~0x38, 0x30);
      s3OutTi3026IndReg(TI_MCLK_LCLK_CONTROL, ~0x38, 0x38);

      outb(vgaCRIndex, 0x42);/* select the clock */
      tmp = inb(vgaCRReg) & 0xf0;
      outb(vgaCRReg, tmp | 0x06);
   }
   LOCK_SYS_REGS;
   return(result);
}






/* The GENDAC code also works for the SDAC, used for Trio32/Trio64 too */

static Bool
s3GendacClockSelect(freq)
     int   freq;

{
   Bool result = TRUE;
   unsigned char tmp;
 
   UNLOCK_SYS_REGS;
   
   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = s3ClockSelect(freq);
      break;
   default:
      {
#if defined(PC98_PW)
	(void) S3gendacSetClock(freq, 7);  /* PW805i can't use reg 2 */
#else

	 if (S3_TRIOxx_SERIES(s3ChipId)) {
	    if (OFLG_ISSET(CLOCK_OPTION_S3AURORA, &s3InfoRec.clockOptions))
	       (void) S3AuroraSetClock(freq, 2); /* can't fail */
	    else if (OFLG_ISSET(CLOCK_OPTION_S3TRIO64V2, &s3InfoRec.clockOptions))
	       (void) S3Trio64V2SetClock(freq, 2); /* can't fail */
	    else 
	       (void) S3TrioSetClock(freq, 2); /* can't fail */
	    if (s3InfoRec.MemClk > 0) {
	       if (OFLG_ISSET(CLOCK_OPTION_S3AURORA, &s3InfoRec.clockOptions))
		  (void) S3AuroraSetClock(s3InfoRec.MemClk, 10); /* can't fail */
	       else if (OFLG_ISSET(CLOCK_OPTION_S3TRIO64V2, &s3InfoRec.clockOptions))
		  (void) S3Trio64V2SetClock(s3InfoRec.MemClk, 10); /* can't fail */
	       else 
		  (void) S3TrioSetClock(s3InfoRec.MemClk, 10); /* can't fail */
	    }
	 }
	 else {
	    if (OFLG_ISSET(CLOCK_OPTION_ICS5342, &s3InfoRec.clockOptions))
	       (void) ICS5342SetClock(freq, 2); /* can't fail */
	    else
	       (void) S3gendacSetClock(freq, 2); /* can't fail */
	    if (s3InfoRec.MemClk > 0) {
	       if (OFLG_ISSET(CLOCK_OPTION_ICS5342, &s3InfoRec.clockOptions))
		  (void) ICS5342SetClock(s3InfoRec.MemClk, 10); /* can't fail */
	       else
		  (void) S3gendacSetClock(s3InfoRec.MemClk, 10); /* can't fail */
	    }
#endif
	    outb(vgaCRIndex, 0x42);/* select the clock */
#if defined(PC98_PW)
	    tmp = inb(vgaCRReg) & 0xf0;
	    outb(vgaCRReg, tmp | 0x07);
#else
	    tmp = inb(vgaCRReg) & 0xf0;
	    outb(vgaCRReg, tmp | 0x02);
#endif
	    usleep(150000);
#if !defined(PC98_PW)
	 }
#endif
      }
   }
   LOCK_SYS_REGS;
   return(result);
}



static void
#if NeedFunctionPrototypes
s3ProgramTi3025Clock(
int clk,
unsigned char n,
unsigned char m,
unsigned char p)
#else
s3ProgramTi3025Clock(clk, n, m, p)
int clk;
unsigned char n;
unsigned char m;
unsigned char p;
#endif
{
   /*
    * Reset the clock data index
    */
   s3OutTiIndReg(TI_PLL_CONTROL, 0x00, 0x00);

   if (clk != TI_MCLK_PLL_DATA) {
      /*
       * Now output the clock frequency
       */
      s3OutTiIndReg(TI_PIXEL_CLOCK_PLL_DATA, 0x00, n);
      s3OutTiIndReg(TI_PIXEL_CLOCK_PLL_DATA, 0x00, m);
      s3OutTiIndReg(TI_PIXEL_CLOCK_PLL_DATA, 0x00, p | TI_PLL_ENABLE);

      /*
       * And now set up the loop clock for RCLK
       */
      s3OutTiIndReg(TI_LOOP_CLOCK_PLL_DATA, 0x00, 0x01);
      s3OutTiIndReg(TI_LOOP_CLOCK_PLL_DATA, 0x00, 0x01);
      s3OutTiIndReg(TI_LOOP_CLOCK_PLL_DATA, 0x00, p>0 ? p : 1);
      s3OutTiIndReg(TI_MISC_CONTROL, 0x00,
		    TI_MC_LOOP_PLL_RCLK | TI_MC_LCLK_LATCH | TI_MC_INT_6_8_CONTROL);

      /*
       * And finally enable the clock
       */
      s3OutTiIndReg(TI_INPUT_CLOCK_SELECT, 0x00, TI_ICLK_PLL);
   } else {
      /*
       * Set MCLK
       */
      s3OutTiIndReg(TI_MCLK_PLL_DATA, 0x00, n);
      s3OutTiIndReg(TI_MCLK_PLL_DATA, 0x00, m);
      s3OutTiIndReg(TI_MCLK_PLL_DATA, 0x00, p | 0x80);
   }
}




static Bool
ti3025ClockSelect(freq)
     int   freq;

{
   Bool result = TRUE;
   unsigned char tmp;
 
   UNLOCK_SYS_REGS;
   
   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = s3ClockSelect(freq);
      break;
   default:
      {
	 /* Check if clock frequency is within range */
	 /* XXXX Check this elsewhere */
	 if (freq < 20000) {
	    ErrorF("%s %s: Specified dot clock (%.3f) too low for TI 3025",
		   XCONFIG_PROBED, s3InfoRec.name, freq / 1000.0);
	    result = FALSE;
	    break;
	 }
	 (void) Ti3025SetClock(freq, 2, s3ProgramTi3025Clock); /* can't fail */
	 outb(vgaCRIndex, 0x42);/* select the clock */
	 tmp = inb(vgaCRReg) & 0xf0;
	 outb(vgaCRReg, tmp | 0x02);
	 usleep(150000);
      }
   }
   LOCK_SYS_REGS;
   return(result);
}




static Bool
ti3026ClockSelect(freq)
     int   freq;

{
   Bool result = TRUE;
   unsigned char tmp;
 
   UNLOCK_SYS_REGS;
   
   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = s3ClockSelect(freq);
      break;
   default:
      {
	 /* Check if clock frequency is within range */
	 /* XXXX Check this elsewhere */
	 if (freq < 13750) {
	    ErrorF("%s %s: Specified dot clock (%.3f) too low for Ti3026/3030",
		   XCONFIG_PROBED, s3InfoRec.name, freq / 1000.0);
	    result = FALSE;
	    break;
	 }
	 (void) Ti3026SetClock(freq, 2, s3Bpp, TI_BOTH_CLOCKS); /* can't fail */
	 outb(vgaCRIndex, 0x42);/* select the clock */
	 tmp = inb(vgaCRReg) & 0xf0;
	 outb(vgaCRReg, tmp | 0x02);
      }
   }
   LOCK_SYS_REGS;
   return(result);
}




static Bool
IBMRGBClockSelect(freq)
     int   freq;

{
   Bool result = TRUE;
 
   UNLOCK_SYS_REGS;
   
   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = s3ClockSelect(freq);
      break;
   default:
      {
	 /* Check if clock frequency is within range */
	 /* XXXX Check this elsewhere */
	 if (freq < 16250) {
	    ErrorF("%s %s: Specified dot clock (%.3f) too low for IBM RGB52x",
		   XCONFIG_PROBED, s3InfoRec.name, freq / 1000.0);
	    result = FALSE;
	    break;
	 }
	 (void)IBMRGBSetClock(freq, 2, s3InfoRec.dacSpeeds[0], s3InfoRec.s3RefClk);
      }
   }
   LOCK_SYS_REGS;
   return(result);
}




static Bool
ch8391ClockSelect(freq)
     int   freq;

{
   Bool result = TRUE;
   unsigned char tmp;
 
   UNLOCK_SYS_REGS;
   
   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = s3ClockSelect(freq);
      break;
   default:
      {
	 /* Check if clock frequency is within range */
	 /* XXXX Check this elsewhere */
	 if (freq < 8500 || freq > 135000) {
	    ErrorF("%s %s: Specified dot clock (%.3f) out of range for Chrontel 8391",
		   XCONFIG_PROBED, s3InfoRec.name, freq / 1000.0);
	    result = FALSE;
	    break;
	 }
	 (void) Chrontel8391SetClock(freq, 2); /* can't fail */
	 outb(vgaCRIndex, 0x42);/* select the clock */
	 tmp = inb(vgaCRReg) & 0xf0;
	 outb(vgaCRReg, tmp | 0x02);
	 usleep(150000);
      }
   }
   LOCK_SYS_REGS;
   return(result);
}




static Bool
att409ClockSelect(freq)
     int   freq;

{
   Bool result = TRUE;
   unsigned char tmp;
 
   UNLOCK_SYS_REGS;
   
   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = s3ClockSelect(freq);
      break;
   default:
      {
	 /* Check if clock frequency is within range */
	 /* XXXX Check this elsewhere */
	 if (freq < 15000 || freq > 240000) {
	    ErrorF("%s %s: Specified dot clock (%.3f) out of range for ATT20C409",
		   XCONFIG_PROBED, s3InfoRec.name, freq / 1000.0);
	    result = FALSE;
	    break;
	 }
	 (void) Att409SetClock(freq, 2); /* can't fail */
	 outb(vgaCRIndex, 0x42);/* select the clock */
	 tmp = inb(vgaCRReg) & 0xf0;
	 outb(vgaCRReg, tmp | 0x02);
	 usleep(150000);
      }
   }
   LOCK_SYS_REGS;
   return(result);
}




static Bool
STG1703ClockSelect(freq)
     int   freq;

{
   Bool result = TRUE;
   unsigned char tmp;
 
   UNLOCK_SYS_REGS;
   
   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = s3ClockSelect(freq);
      break;
   default:
      {
	 /* Check if clock frequency is within range */
	 /* XXXX Check this elsewhere */
	 if (freq < 8500 || freq > 135000) {
	    ErrorF("%s %s: Specified dot clock (%.3f) out of range for STG1703",
		   XCONFIG_PROBED, s3InfoRec.name, freq / 1000.0);
	    result = FALSE;
	    break;
	 }
	 (void) STG1703SetClock(freq, 2); /* can't fail */
	 outb(vgaCRIndex, 0x42);/* select the clock */
	 tmp = inb(vgaCRReg) & 0xf0;
	 outb(vgaCRReg, tmp | 0x02);
	 usleep(150000);
      }
   }
   LOCK_SYS_REGS;
   return(result);
}


