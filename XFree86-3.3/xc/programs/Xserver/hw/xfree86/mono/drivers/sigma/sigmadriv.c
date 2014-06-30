/* $XFree86: xc/programs/Xserver/hw/xfree86/mono/drivers/sigma/sigmadriv.c,v 3.5 1996/12/23 06:48:30 dawes Exp $ */
/*
 * MONO: Driver family for interlaced and banked monochrome video adaptors
 * Pascal Haible 8/93, 3/94, 4/94 haible@IZFM.Uni-Stuttgart.DE
 *
 * mono/driver/sigma/sigmadriv.c
 *
 * Parts derived from:
 * hga2/...
 * Author:  Davor Matic, dmatic@athena.mit.edu
 * and
 * vga256/...
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * see mono/COPYRIGHT for copyright and disclaimers.
 */
/* $XConsortium: sigmadriv.c /main/4 1996/10/23 18:46:02 kaleb $ */


#include "X.h"
#include "input.h"
#include "screenint.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"

#include "mono.h"
#include "sigmaHW.h"

static int sigma_Current_mode = sigma_Textmode;
static int sigmaMemBase = SIGMA_DEF_MEM_BASE;

/*
 * Define the SIGMA I/O Ports
 */
static unsigned SIGMA_IOPorts[] = { SLV_EN1, SLV_W16, SLV_W17, SLV_W18,
			SLV_BLANK, SLV_ZOOM, SLV_GR0, SLV_GR1,
			SLV_BANK0, SLV_BANK1, SLV_BANK2, SLV_BANK3,
			SLV_HIRES /* = MONOEN */, SLV_BOLD /* = WOB */ };
static int Num_SIGMA_IOPorts = (sizeof(SIGMA_IOPorts)/sizeof(SIGMA_IOPorts[0]));

#define SIGMA_DELAY	{ volatile int delay_dummy=0; delay_dummy++; }

Bool SIGMAProbe(
#if NeedFunctionPrototypes
		void
#endif
);
char *SIGMAIdent(
#if NeedFunctionPrototypes
		int
#endif
);
pointer	SIGMAInit(
#if NeedFunctionPrototypes
		DisplayModePtr
#endif
);
void SIGMAEnterLeave(
#if NeedFunctionPrototypes
		Bool /* enter */
#endif
);
pointer SIGMASave(
#if NeedFunctionPrototypes
		pointer
#endif
);
void SIGMARestore(
#if NeedFunctionPrototypes
		pointer
#endif
);
void SIGMAAdjust(
#if NeedFunctionPrototypes
		int /* x */,
		int /* y */
#endif
);
Bool SIGMASaveScreen(
#if NeedFunctionPrototypes
		ScreenPtr /* pScreen */,
		Bool      /* on */
#endif
);
void SIGMAClearScreen(
#if NeedFunctionPrototypes
		void
#endif
);

/* banking functions below */
PixelType * SIGMAScanlineOffsetA(
#if NeedFunctionPrototypes
                PixelType * /* pointer */,
                int         /* offset */
#endif
);
PixelType * SIGMAScanlineOffsetB(
#if NeedFunctionPrototypes
                PixelType * /* pointer */,
                int         /* offset */
#endif
);

/*
 * structure for accessing the video chip`s functions
 *
 * We are doing a total encapsulation of the driver's functions.
 * Banking (monoSetReadWrite(p) etc.) is done in monoBank.c
 *   using the chip's function pointed to by
 *   bmpSetReadWriteFunc(bank) etc.
 */
monoVideoChipRec SIGMA = {
  /* Functions */
  SIGMAProbe,
  SIGMAIdent,
  SIGMAInit,
  (void (*)())NoopDDA,		/* FbInit */
  SIGMAEnterLeave,
  SIGMASave,
  SIGMARestore,
  SIGMAAdjust,
  SIGMASaveScreen,
  SIGMAClearScreen,		/* ClearScreen */
  SIGMAScanlineOffsetA,
  SIGMAScanlineOffsetB,
  (unsigned char *)SIGMA_BANK1_BOTTOM,	/* ReadBottom */
  (unsigned char *)SIGMA_BANK1_TOP,	/* ReadTop */
  (unsigned char *)SIGMA_BANK2_BOTTOM,	/* WriteBottom */
  (unsigned char *)SIGMA_BANK2_TOP,	/* WriteTop */
  (unsigned char *)SIGMA_DEF_MAP_BASE,	/* MapBase */
  SIGMA_MAP_SIZE,		/* MapSize */
  SIGMA_HDISPLAY,		/* HDisplay */
  SIGMA_VDISPLAY,		/* VDisplay */
  SIGMA_SCAN_LINE_WIDTH,	/* ScanLineWidth */
  {0,},				/* ChipOptionFlags */
};

/*
 * SIGMAIdent
 */
char *
SIGMAIdent(n)
    int n;
{
/* "Sigma L-View (LVA-PC-00SP0)", "Sigma LaserView PLUS (LDA-1200)" */
/* But this must be the chipset name optionally preset (lowercase!?)*/
/* using only one as I don't know how to tell them apart */
static char *chipsets[] = {"sigmalview"};
if (n >= sizeof(chipsets) / sizeof(char *))
	return(NULL);
else return(chipsets[n]);
}

/*
 * SIGMAProbe --
 *      check whether an SIGMA based board is installed
 */
Bool
SIGMAProbe()
{
  /*
   * Set up I/O ports to be used by this card
   */
  xf86ClearIOPortList(monoInfoRec.scrnIndex);
  xf86AddIOPorts(monoInfoRec.scrnIndex, Num_SIGMA_IOPorts, SIGMA_IOPorts);

  if (monoInfoRec.chipset) {
	/* Chipset preset */
	if (strcmp(monoInfoRec.chipset, SIGMAIdent(0)))
		/* desired chipset != this one */
		return (FALSE);
	else {
		SIGMAEnterLeave(ENTER);
		/* go on with videoram etc. below */
	}
  }
  else {
	Bool found=FALSE;
	unsigned char zoom_save;

	/* Enable I/O Ports */
	SIGMAEnterLeave(ENTER);

	/*
	 * Check if there is a Sigma L-View or LaserView PLUS board
	 * in the system.
	 */

	/* Test if ZOOM bit (bit 7 on extended port 0x4649) is r/w */
	/* save it first */
	zoom_save = inb(SLV_ZOOM);
	SIGMA_DELAY;
	outb(SLV_ZOOM,0);
	SIGMA_DELAY;
	found=((inb(SLV_ZOOM)&0x80)==0);
	SIGMA_DELAY;
	outb(SLV_ZOOM,0x80);
	SIGMA_DELAY;
	found=found && ((inb(SLV_ZOOM)&0x80)==0x80);
	SIGMA_DELAY;
	/* write back */
	if (found)
		outb(SLV_ZOOM, (zoom_save & 0x80));
					/* write only 0x00 or 0x80 */

	/* There seems to be no easy way to tell if it is an PLUS or not
	 * (apart perhaps from writing to both planes) */
	if (found) {
		monoInfoRec.chipset = SIGMAIdent(0);
		ErrorF("%s %s: %s detected\n",
			XCONFIG_PROBED,
			monoInfoRec.name,
			monoInfoRec.chipset);
	} else {
	/* there is no Sigma L-View card */
		SIGMAEnterLeave(LEAVE);
		return(FALSE);
	}
  } /* else (monoInfoRec.chipset) -- monoInfoRec.chipset is already set */

/* The following is done for both probed and preset chipset */

  if (!monoInfoRec.videoRam) {
	/* videoram not given in XF86Config */
	monoInfoRec.videoRam=256;
  }
  if (monoInfoRec.MemBase!=0) {
	if ( (monoInfoRec.MemBase!=0xA0000L) &&
		(monoInfoRec.MemBase!=0xB0000L) &&
		(monoInfoRec.MemBase!=0xC0000L) &&
		(monoInfoRec.MemBase!=0xD0000L) &&
		(monoInfoRec.MemBase!=0xE0000L) ) {
	    /* Invalid MemBase */
	    ErrorF("%s %s: %s: Invalid MemBase 0x%x (must be 0x{ABCDE}0000),\n\t\tusing default\n",
		XCONFIG_GIVEN,
		monoInfoRec.name,
		monoInfoRec.chipset,
		sigmaMemBase);
	    sigmaMemBase=SIGMA_DEF_MEM_BASE;
	    ErrorF("%s %s: %s using mem base 0x%x\n",
		XCONFIG_PROBED,
		monoInfoRec.name,
		monoInfoRec.chipset,
		sigmaMemBase);
	} else {
	    /* Valid MemBase */
	    sigmaMemBase=monoInfoRec.MemBase;
	    SIGMA.ChipMapBase=(unsigned char *)sigmaMemBase;
	    ErrorF("%s %s: %s using mem base 0x%x\n",
		XCONFIG_GIVEN,
		monoInfoRec.name,
		monoInfoRec.chipset,
		sigmaMemBase);
	}
  } else {
	/* Default MemBase */
	sigmaMemBase=SIGMA_DEF_MEM_BASE;
	ErrorF("%s %s: %s using mem base 0x%x\n",
		XCONFIG_PROBED,
		monoInfoRec.name,
		monoInfoRec.chipset,
		sigmaMemBase);
  }
  /* We do 'virtual' handling here as it is highly chipset specific */
  /* Screen size (pixels) is fixed, virtual size can be larger up to
   * ChipMaxVirtualX and ChipMaxVirtualY */
  /* Real display size is given by SIGMA_HDISPLAY and SIGMA_VDISPLAY,
   * desired virtual size is monoInfoRec.virtualX and monoInfoRec.virtualY.
   * Think they can be -1 at this point.
   * Maximum virtual size as done by the driver is
   * SIGMA_MAX_VIRTUAL_X and ..._Y
   */
   if (!(monoInfoRec.virtualX < 0)) {
	/* virtual set in XF86Config */
	ErrorF("%s %s: %s: Virtual not allowed for this chipset\n",
		XCONFIG_PROBED, monoInfoRec.name, monoInfoRec.chipset);
   }
   /* Set virtual to real size */
   monoInfoRec.virtualX = SIGMA_HDISPLAY;
   monoInfoRec.virtualY = SIGMA_VDISPLAY;
   /* Must return real display size */
   /* hardcoded in SIGMA */

   /* N.B.: Inform the code above that the framebuffer is banked */
   monoInfoRec.bankedMono = TRUE;

   return(TRUE);
}

/*
 * SIGMAEnterLeave --
 *      enable/disable io permissions
 */
void 
SIGMAEnterLeave(enter)
    Bool enter;
{
  if (enter)
	xf86EnableIOPorts(monoInfoRec.scrnIndex);
  else
	xf86DisableIOPorts(monoInfoRec.scrnIndex);
}

/*
 * SIGMAInit --
 *      Handle the initialization of the SIGMAs registers
 */
pointer
SIGMAInit(mode)
    DisplayModePtr mode;
{
    return((pointer)sigma_Graphmode);
}

/*
 * SIGMASave --
 *      save the current video mode
 */
pointer
SIGMASave(save)
    pointer save;
{
    return((pointer)sigma_Current_mode);
}

/*
 * SIGMARestore --
 *      restore a video mode
 * The action done here may be overkill - but as I don't have this
 * card in front of me - never change working code...
 */
void
SIGMARestore(restore)
    pointer restore;
{
    if ((int)restore==sigma_Textmode) {
	/* Blank the screen to black */
	outb(SLV_GR0,0);
	SIGMA_DELAY;
	outb(SLV_GR1,0);
	SIGMA_DELAY;
	outb(SLV_BLANK,0);
	SIGMA_DELAY;
	/* Disable adapter memory */
	outb(SLV_EN1,0);
	SIGMA_DELAY;

	/* deselect hires */
	outb(SLV_HIRES,0x0);
	SIGMA_DELAY;
	/* Unblank the screen */
	outb(SLV_GR0,0x80);
	SIGMA_DELAY;
	outb(SLV_GR1,0x80);
	SIGMA_DELAY;
	outb(SLV_BLANK,0x80);
	SIGMA_DELAY;

	sigma_Current_mode=sigma_Textmode;

    } else if ((int)restore==sigma_Graphmode) {
	/* Blank the screen to black */
	outb(SLV_GR0,0);
	SIGMA_DELAY;
	outb(SLV_GR1,0);
	SIGMA_DELAY;
	outb(SLV_BLANK,0);
	SIGMA_DELAY;
	/* Disable adapter memory */
	outb(SLV_EN1,0);
	SIGMA_DELAY;

	/* Set page frame */
	outb(SLV_W16,((sigmaMemBase & 0x10000L) ? 0 : 0x80));
	SIGMA_DELAY;
	outb(SLV_W17,((sigmaMemBase & 0x20000L) ? 0 : 0x80));
	SIGMA_DELAY;
	outb(SLV_W18,((sigmaMemBase & 0x40000L) ? 0 : 0x80));
	SIGMA_DELAY;
	outb(SLV_ZOOM,0x80);
	SIGMA_DELAY;

	/* */
	outb(SLV_HIRES,0x80);
	SIGMA_DELAY;
	/* Unblank the screen */
	outb(SLV_GR0,0x80);
	SIGMA_DELAY;
	outb(SLV_GR1,0x80);
	SIGMA_DELAY;
	outb(SLV_BLANK,0x80);
	SIGMA_DELAY;
	/* Set enable state */
	outb(SLV_EN1,0x80);

	sigma_Current_mode=sigma_Graphmode;

    } else ErrorF("Warning: SIGMARestore called with invalid arg.\n");
}

/*
 * SIGMASaveScreen();
 *	Disable the video on the frame buffer (screensaver)
 */
Bool
SIGMASaveScreen(pScreen,on)
    ScreenPtr pScreen;
    Bool      on;
{
if (on == SCREEN_SAVER_FORCER)
	SetTimeSinceLastInputEvent();
if (xf86VTSema) {
	if (on) { /* Grrr! SaveScreen(on=TRUE) means turn ScreenSaver off */
		/* Unblank to 4 gray levels */
		outb(SLV_BLANK,0x80);
		SIGMA_DELAY;
		outb(SLV_GR0,0x80);
		SIGMA_DELAY;
		outb(SLV_GR1,0x80);
	} else {
		/* Blank to black */
		outb(SLV_BLANK,0);
		SIGMA_DELAY;
		outb(SLV_GR0,0);
		SIGMA_DELAY;
		outb(SLV_GR1,0);
	}
} /* if we are not on the active VT, don't do anything - the screen
   * will be visible as soon as we switch back anyway (?) */
return(TRUE);
}

/* SIGMAAdjust --
 *      adjust the current video frame to display the mousecursor
 *      (x,y) is the upper left corner to be displayed.
 *      The Sigma L-View / LaserView PLUS can't pan.
 */
void
SIGMAAdjust(x,y)
    int x, y;
{
}

/*
 * Special hack for Sigma LaserView [PLUS]
 * - logical bank size 16k
 * - real bank size 16k
 * To bank logical bank e.g n=1 [starting at 0]
 *    (line 64 to 127, mem 16k to 32k-1) to e.g. (logical) BANK1:
 * bank real bank n+13=14 to the first 16k window,
 * bank real bank n+14=15 to the second 16k window.
 * top and bottom are set to include last 12k of the first window and the
 * first 4k of the second window.
 */

static int SIGMABankAseg;
static int SIGMABankBseg;

#define BANK_A(_bank)			\
	{ register int real_bank;	\
	  real_bank = _bank;		\
	  real_bank += 13;		\
	  real_bank |= 0x80;		\
	  outb(SLV_BANK0,real_bank);	\
	  real_bank ++;			\
	  outb(SLV_BANK1,real_bank);	\
	}
#define BANK_B(_bank)			\
	{ register int real_bank;	\
	  real_bank = _bank;		\
	  real_bank += 13;		\
	  real_bank |= 0x80;		\
	  outb(SLV_BANK2,real_bank);	\
	  real_bank ++;			\
	  outb(SLV_BANK3,real_bank);	\
	}

void SIGMAClearScreen()
{
    int lines_per_bank, nr_full_banks, i, j;
    lines_per_bank = SIGMA_SEGMENT_SIZE/(SIGMA_SCAN_LINE_WIDTH/8 /*Pixels/Byte*/ );
    nr_full_banks = SIGMA_VDISPLAY/lines_per_bank;
    for (i=0; i<nr_full_banks; i++) {
	BANK_A(i);
	/* Don't touch the gap at the end of the line */
	for (j=0; j<lines_per_bank; j++) {
	    memset(monoBankABottom + j*(SIGMA_SCAN_LINE_WIDTH/8),
		   0, (unsigned int)(SIGMA_HDISPLAY/8));
	}
    }
    /* the sigma is not done with this */
    BANK_A(i);
    for (j=0; j<(SIGMA_VDISPLAY%lines_per_bank); j++) {
	memset(monoBankABottom + j*(SIGMA_SCAN_LINE_WIDTH/8),
		0, (unsigned int)(SIGMA_HDISPLAY/8));
    }
}

PixelType * SIGMAScanlineOffsetA(p, offset)
    PixelType *p;
    int offset;
{
register /*signed*/ int delta;
register /*signed*/ int deltabank;
  if ((unsigned int)(p)>=MONOBASE) {
     /* virtual framebuffer address */
     /* p - MONOBASE is not necessarily a valid pointer within the bank */
     /* -> do absolute banking */
     p += offset;
     p = (PixelType *)((unsigned int)(p) - (unsigned int)MONOBASE);
     deltabank = (signed int)p >> SIGMA_SEGMENT_SHIFT;
     SIGMABankAseg = deltabank;
     BANK_A(SIGMABankAseg);
     p = (PixelType *)( (unsigned int)p + monoBankABottom
                        - (deltabank << SIGMA_SEGMENT_SHIFT) );
     return(p);
  }
  /* At least on Linux, the mapped Area is at 0x40000000, so test >= first */
  if ((unsigned int)(p)>=(unsigned int)monoBankABottom) {
     if ((unsigned int)(p)<(unsigned int)monoBankATop) {
        /* within the framebuffer */
        (p) += (offset);
        /* outside of the bank now ? */
        delta = (signed int)p - (signed int)monoBankABottom;
        deltabank = delta >> SIGMA_SEGMENT_SHIFT;
        if (0!=deltabank) {
                SIGMABankAseg += deltabank;
                BANK_A(SIGMABankAseg);
                p = (PixelType *)( (unsigned int)p
                                   - (deltabank << SIGMA_SEGMENT_SHIFT) );
        }
        return(p);
     }
  }
  /* no framebuffer address */
  return((p)+(offset));
}

PixelType * SIGMAScanlineOffsetB(p, offset)
    PixelType *p;
    int offset;
{
register /*signed*/ int delta;
register /*signed*/ int deltabank;
  if ((unsigned int)(p)>=MONOBASE) {
     p += offset;
     p = (PixelType *)((unsigned int)(p) - (unsigned int)MONOBASE);
     deltabank = (signed int)p >> SIGMA_SEGMENT_SHIFT;
     SIGMABankBseg = deltabank;
     BANK_B(SIGMABankBseg);
     p = (PixelType *)( (unsigned int)p + monoBankBBottom
                        - (deltabank << SIGMA_SEGMENT_SHIFT) );
     return(p);
  }
  if ((unsigned int)(p)>=(unsigned int)monoBankBBottom) {
     if ((unsigned int)(p)<(unsigned int)monoBankBTop) {
        (p) += (offset);
        delta = (signed int)p - (signed int)monoBankBBottom;
        deltabank = delta >> SIGMA_SEGMENT_SHIFT;
        if (0!=deltabank) {
                SIGMABankBseg += deltabank;
                BANK_B(SIGMABankBseg);
                p = (PixelType *)( (unsigned int)p
                                   - (deltabank << SIGMA_SEGMENT_SHIFT) );
        }
        return(p);
     }
  }
  return((p)+(offset));
}
