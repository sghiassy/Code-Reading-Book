/* $XFree86: xc/programs/Xserver/hw/xfree86/mono/drivers/hgc1280/hgc1280driv.c,v 3.6 1996/12/23 06:48:20 dawes Exp $ */
/*
 * MONO: Driver family for interlaced and banked monochrome video adaptors
 * Pascal Haible 8/93, 3/94, 4/94 haible@IZFM.Uni-Stuttgart.DE
 *
 * mono/driver/hgc1280/hgc1280driv.c
 *
 * derived from:
 * hga2/...
 * Author:  Davor Matic, dmatic@athena.mit.edu
 * and
 * vga256/...
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * see mono/COPYRIGHT for copyright and disclaimers.
 */
/* $XConsortium: hgc1280driv.c /main/5 1996/10/23 18:45:55 kaleb $ */


#include "X.h"
#include "input.h"
#include "screenint.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"
#include "xf86Procs.h"

#include "mono.h"
#include "hgc1280HW.h"

static int HGC_Current_mode = HGC_Textmode;

static Bool HGC_Primary = TRUE;

/* #define HGC1280_DEBUG */

/*
 * Define the HGC I/O Ports
 * We take the ports for both primary and secondary 
 */
static unsigned HGC1280_IOPorts[] = {
	HGC_PRIM_PORT_INDEX, HGC_PRIM_PORT_DATA, HGC_PRIM_PORT_CONTROL,
	HGC_PRIM_PORT_CRT_STATUS, HGC_PRIM_PORT_CONFIG,
	HGC_SEC_PORT_INDEX, HGC_SEC_PORT_DATA, HGC_SEC_PORT_CONTROL,
	HGC_SEC_PORT_CRT_STATUS, HGC_SEC_PORT_CONFIG };
static int Num_HGC1280_IOPorts = (sizeof(HGC1280_IOPorts)/sizeof(HGC1280_IOPorts[0]));

Bool HGC1280Probe(
#if NeedFunctionPrototypes
		void
#endif
);
char *HGC1280Ident(
#if NeedFunctionPrototypes
		int
#endif
);
pointer HGC1280Init(
#if NeedFunctionPrototypes
		DisplayModePtr
#endif
);
void HGC1280EnterLeave(
#if NeedFunctionPrototypes
		Bool /* enter */
#endif
);
pointer HGC1280Save(
#if NeedFunctionPrototypes
		pointer
#endif
);
void HGC1280Restore(
#if NeedFunctionPrototypes
		pointer
#endif
);
void HGC1280Adjust(
#if NeedFunctionPrototypes
		int /* x */,
		int /* y*/
#endif
);
Bool HGC1280SaveScreen(
#if NeedFunctionPrototypes
		ScreenPtr /* pScreen */,
		Bool      /* on */
#endif
);
void HGC1280ClearScreen(
#if NeedFunctionPrototypes
		void
#endif
);

/* banking functions below */
PixelType * HGC1280pScanlineOffsetA(
#if NeedFunctionPrototypes
		PixelType * /* pointer */,
		int         /* offset */
#endif
);
PixelType * HGC1280pScanlineOffsetB(
#if NeedFunctionPrototypes
		PixelType * /* pointer */,
		int         /* offset */
#endif
);
PixelType * HGC1280sScanlineOffsetA(
#if NeedFunctionPrototypes
                PixelType * /* pointer */,
		int         /* offset */
#endif
);
PixelType * HGC1280sScanlineOffsetB(
#if NeedFunctionPrototypes
		PixelType * /* pointer */,
		int         /* offset */
#endif
);

/*
 * structure for accessing the video chip`s functions
 */

monoVideoChipRec HGC1280 = {
  /* Functions */
  HGC1280Probe,
  HGC1280Ident,
  HGC1280Init,
  (void (*)()) NoopDDA,		/* FbInit */
  HGC1280EnterLeave,
  HGC1280Save,
  HGC1280Restore,
  HGC1280Adjust,
  HGC1280SaveScreen,
  HGC1280ClearScreen,
  HGC1280pScanlineOffsetA,
  HGC1280pScanlineOffsetB,
  (unsigned char *)HGC_BANK1_BOTTOM,	/* BankABottom */
  (unsigned char *)HGC_BANK1_TOP,	/* BankATop */
  (unsigned char *)HGC_BANK2_BOTTOM,	/* BankBBottom */
  (unsigned char *)HGC_BANK2_TOP,	/* BankBTop */
  (unsigned char *)HGC_PRIM_MAP_BASE,	/* MapBase */
  HGC_MAP_SIZE,			/* MapSize */
  HGC_HDISPLAY,			/* HDisplay */
  HGC_VDISPLAY,			/* VDisplay */
  HGC_SCAN_LINE_WIDTH,		/* ScanLineWidth */
  {0,},				/* ChipOptionFlags */
};

/*
 * HGC1280Ident
 */
char *
HGC1280Ident(n)
    int n;
{
    static char *chipsets[] = {"hgc1280"};
    if (n >= sizeof(chipsets) / sizeof(char *))
	return(NULL);
    else
	return(chipsets[n]);
}

/*
 * HGC1280Probe --
 *      check whether an HGC1280 based board is installed
 */
Bool
HGC1280Probe()
{
    /*
     * Set up I/O ports to be used by this card
     */
    xf86ClearIOPortList(monoInfoRec.scrnIndex);
    xf86AddIOPorts(monoInfoRec.scrnIndex, Num_HGC1280_IOPorts, HGC1280_IOPorts);

    if (monoInfoRec.chipset) {
	/* Chipset preset */
	if (strcmp(monoInfoRec.chipset, HGC1280Ident(0)))
	    /* desired chipset != this one */
	    return (FALSE);
	else {
	    /* chipset "hgc1280" */
	    if (OFLG_ISSET(OPTION_SECONDARY, &monoInfoRec.options)) {
		/* option "secondary" */
		HGC_Primary=FALSE;
		ErrorF("%s %s: %s: Using secondary address\n",
			XCONFIG_GIVEN, monoInfoRec.name, monoInfoRec.chipset);
	    } else {
		/* assume primary */
		HGC_Primary=TRUE;
		ErrorF("%s %s: %s: Assuming primary address\n",
		XCONFIG_PROBED, monoInfoRec.name, monoInfoRec.chipset);
	    }
	    HGC1280EnterLeave(ENTER);
	    /* go on with videoram etc. below */
	}
  }
  else {
	unsigned char dsp, dsp_old;
	int i;
	int cnt=0;
	short val=0;
	Bool found=FALSE;

	HGC1280EnterLeave(ENTER);
	/*
	 * Checks if there is a HGC-1280 based board in the system.
	 *
	 *
	 */
	/*************************************************************
	 * If anybody has any 'real' infos about the HGC-1280 Hardware
	 * - Please please contact me!
	 *************************************************************/
	/* Check for Hercules-like HSYNC on BASE+0xA, Bit 1 */ 
#define HSYNC_MASK  0x01
#define MIN_COUNT 2000
	/* First check the primary address */
#ifdef HGC1280_DEBUG
ErrorF("Checking primary address: ");
#endif
	dsp_old = inb(HGC_PRIM_PORT_CRT_STATUS) & HSYNC_MASK;
	for (i = 0; i < 5000; i++) {
		dsp = inb(HGC_PRIM_PORT_CRT_STATUS) & HSYNC_MASK;
		if (dsp != dsp_old) cnt++;
		dsp_old = dsp;
	}
	/* This cnt is about 3500 to 4500 with a 'virgin' hardware,
	 * and might be exactly 5000 if the card had been switched
	 * to 'my' graphics mode before */
#ifdef HGC1280_DEBUG
ErrorF("count=%d\n",cnt);
#endif
	if (cnt>MIN_COUNT) {
		monoInfoRec.chipset = HGC1280Ident(0);
#ifdef HGC1280_DEBUG
		ErrorF("%s %s: %s: cnt=%d, ", XCONFIG_PROBED,
			monoInfoRec.name, monoInfoRec.chipset, cnt);
#endif
		/* Test a r/w register */
		HGC_PRIM_SET_REG(HGC_PROBE_REG_RW,HGC_PROBE_VAL_WRITE);
		HGC_PRIM_GET_REG(HGC_PROBE_REG_RW,&val);
#ifdef HGC1280_DEBUG
		ErrorF("r/w-val=%d, ",val);
#endif
		found = (val==HGC_PROBE_VAL_READ);
		/* this register can't be set, depends on other registers */
		HGC_PRIM_GET_REG(HGC_PROBE_REG_FIX,&val);
#ifdef HGC1280_DEBUG
		ErrorF("fix-val1=%d, ",val);
#endif
		found = found && (val==HGC_PROBE_VAL_FIX1);
		HGC_PRIM_SET_REG(HGC_PROBE_REG_RW,HGC_PROBE_VAL_RESET);
		HGC_PRIM_GET_REG(HGC_PROBE_REG_FIX,&val);
#ifdef HGC1280_DEBUG
		ErrorF("fix-val2=%d\n",val);
#endif
		found = found && (val==HGC_PROBE_VAL_FIX2);
		if (found) {
			HGC_Primary=TRUE;
			ErrorF("%s %s: %s found on primary address\n",
				XCONFIG_PROBED, monoInfoRec.name,
				monoInfoRec.chipset);
		}
		else {
#ifdef HGC1280_DEBUG
		    ErrorF("==> not found\n");
#endif
		    monoInfoRec.chipset = "\0";
		}
	};
	if ( !(cnt>MIN_COUNT) || !found ) {
#ifdef HGC1280_DEBUG
ErrorF("Checking secondary address: ");
#endif
	    /* Check the secondary address */
	    dsp_old = inb(HGC_SEC_PORT_CRT_STATUS) & HSYNC_MASK;
	    for (i = 0; i < 5000; i++) {
		dsp = inb(HGC_SEC_PORT_CRT_STATUS) & HSYNC_MASK;
		if (dsp != dsp_old) cnt++;
		dsp_old = dsp;
	    }
#ifdef HGC1280_DEBUG
ErrorF("count=%d\n",cnt);
#endif
	    if (cnt>MIN_COUNT) {
		monoInfoRec.chipset = HGC1280Ident(0);
#ifdef HGC1280_DEBUG
		ErrorF("%s %s: %s: cnt=%d, ", XCONFIG_PROBED,
		       monoInfoRec.name, monoInfoRec.chipset, cnt);
#endif
		/* Test a r/w register */
		HGC_SEC_SET_REG(HGC_PROBE_REG_RW,HGC_PROBE_VAL_WRITE);
		HGC_SEC_GET_REG(HGC_PROBE_REG_RW,&val);
#ifdef HGC1280_DEBUG
		ErrorF("r/w-val=%d, ",val);
#endif
		found = (val==HGC_PROBE_VAL_READ);
		/* this register can't be set, depends on other registers */
		HGC_SEC_GET_REG(HGC_PROBE_REG_FIX,&val);
#ifdef HGC1280_DEBUG
		ErrorF("fix-val1=%d, ",val);
#endif
		found = found && (val==HGC_PROBE_VAL_FIX1);
		HGC_SEC_SET_REG(HGC_PROBE_REG_RW,HGC_PROBE_VAL_RESET);
		HGC_SEC_GET_REG(HGC_PROBE_REG_FIX,&val);
#ifdef HGC1280_DEBUG
		ErrorF("fix-val2=%d\n",val);
#endif
		found = found && (val==HGC_PROBE_VAL_FIX2);
		if (found) {
			HGC_Primary=FALSE;
			ErrorF("%s %s: %s found on secondary address\n",
				XCONFIG_PROBED, monoInfoRec.name,
				monoInfoRec.chipset);
		} else {
#ifdef HGC1280_DEBUG
			ErrorF("==> not found\n");
#endif
			monoInfoRec.chipset = "\0";
		}
	    }
	} /* if ( !(cnt>MIN_COUNT) || !found ) */
	if ( !(cnt>MIN_COUNT) || !found ) {
	/* there is no HGC-1280 card */
		HGC1280EnterLeave(LEAVE);
		return(FALSE);
	}
  } /* else (monoInfoRec.chipset) -- monoInfoRec.chipset is already set */

  /* The following is done for both probed and preset chipset */

  if (!monoInfoRec.videoRam) {
	/* videoram not given in XF86Config */
	monoInfoRec.videoRam=192;
  }
  if (!HGC_Primary) {
	/* Fill in consts and functions for secondary I/O / Mem Base */
	HGC1280.ChipMapBase=(unsigned char *)HGC_SEC_MAP_BASE;
	HGC1280.ChipScanlineOffsetA=HGC1280sScanlineOffsetA;
	HGC1280.ChipScanlineOffsetB=HGC1280sScanlineOffsetB;
  }
  /* We do 'virtual' handling here as it is highly chipset specific */
  /* Screen size (pixels) is fixed, virtual size can be larger up to
   * ChipMaxVirtualX and ChipMaxVirtualY */
  /* Real display size is given by HGC_HDISPLAY and HGC_VDISPLAY,
   * desired virtual size is monoInfoRec.virtualX and monoInfoRec.virtualY.
   * Think they can be -1 at this point.
   * Maximum virtual size as done by the driver is
   * HGC_MAX_VIRTUAL_X and ..._Y
   */
   if (monoInfoRec.virtualX < 0) {
	/* No virtual set in XF86Config */
	/* Set virtual to real size */
	monoInfoRec.virtualX = HGC_HDISPLAY;
	monoInfoRec.virtualY = HGC_VDISPLAY;
   } else {
	int Xrounding = 16;
        if (monoInfoRec.virtualX > HGC_MAX_VIRTUAL_X) {
		ErrorF("%s %s: %s: Virtual width too large, reset to %d\n",
			XCONFIG_PROBED,
			monoInfoRec.name, monoInfoRec.chipset,
			HGC_MAX_VIRTUAL_X);
		monoInfoRec.virtualX = HGC_MAX_VIRTUAL_X;
	} else if (monoInfoRec.virtualY > HGC_MAX_VIRTUAL_Y) {
		ErrorF("%s %s: %s: Virtual height too large, reset to %d\n",
			XCONFIG_PROBED,
			monoInfoRec.name, monoInfoRec.chipset,
			HGC_MAX_VIRTUAL_Y);
                monoInfoRec.virtualY = HGC_MAX_VIRTUAL_Y;
	} else if (monoInfoRec.virtualX < HGC_HDISPLAY) {
		ErrorF("%s %s: %s: Virtual width too small, reset to %d\n",
			XCONFIG_PROBED,
			monoInfoRec.name, monoInfoRec.chipset,
			HGC_HDISPLAY);
		monoInfoRec.virtualX = HGC_HDISPLAY;
	} else if (monoInfoRec.virtualY < HGC_VDISPLAY) {
		ErrorF("%s %s: %s: Virtual height too small, reset to %d\n",
			XCONFIG_PROBED,
			monoInfoRec.name, monoInfoRec.chipset,
			HGC_VDISPLAY);
		monoInfoRec.virtualY = HGC_VDISPLAY;
	};
	if (monoInfoRec.virtualX % Xrounding) {
		monoInfoRec.virtualX -= monoInfoRec.virtualX % Xrounding;
		ErrorF("%s: %s: Virtual width rounded down to %d\n",
			monoInfoRec.name, monoInfoRec.chipset,
			monoInfoRec.virtualX);
	}
   }

   /* Initialize option flags allowed for this driver */
   OFLG_SET(OPTION_SECONDARY, &HGC1280.ChipOptionFlags);

   /* Must return real display size */
   /* hardcoded in HGC1280 */

   monoInfoRec.bankedMono = TRUE;

   return(TRUE);
}

/*
 * HGC1280EnterLeave --
 *      enable/disable io permissions
 */
void 
HGC1280EnterLeave(enter)
    Bool enter;
{
  if (enter)
	xf86EnableIOPorts(monoInfoRec.scrnIndex);
  else
	xf86DisableIOPorts(monoInfoRec.scrnIndex);
}

/*
 * HGC1280Init --
 *      Handle the initialization of the HGCs registers
 */
pointer
HGC1280Init(mode)
    DisplayModePtr mode;
{
return((pointer)HGC_Graphmode);
}

/*
 * HGC1280Save --
 *      save the current video mode
 */
pointer
HGC1280Save(save)
    pointer save;
{
return((pointer)HGC_Current_mode);
}

/*
 * HGC1280Restore --
 *      restore a video mode
 */
void
HGC1280Restore(restore)
    pointer restore;
{
  unsigned char i;
  if (HGC_Primary) {
	if ((int)restore==HGC_Textmode) {
		for (i = 0; i < HGC_NUM_REGS; i++) {
			HGC_PRIM_SET_REG(i,hgcRegsText[i]);
		}
		HGC_Current_mode = HGC_Textmode;
	} else if ((int)restore==HGC_Graphmode) {
		for (i = 0; i < HGC_NUM_REGS; i++) {
			HGC_PRIM_SET_REG(i,hgcRegsGraf1280x1024[i]);
		}
		HGC_Current_mode = HGC_Graphmode;
	} else ErrorF("Warning: HGC1280Restore called with invalid arg.\n");
  } else {
  /* secondary */ 
	if ((int)restore==HGC_Textmode) {
		for (i = 0; i < HGC_NUM_REGS; i++) {
			HGC_SEC_SET_REG(i,hgcRegsText[i]);
		}
		HGC_Current_mode = HGC_Textmode;
	} else if ((int)restore==HGC_Graphmode) {
		for (i = 0; i < HGC_NUM_REGS; i++) {
			HGC_SEC_SET_REG(i,hgcRegsGraf1280x1024[i]);
		}
		HGC_Current_mode = HGC_Graphmode;
	} else ErrorF("Warning: HGC1280Restore called with invalid arg.\n");
  }
}

/*
 * HGC1280SaveScreen();
 *	Disable the video on the frame buffer (screensaver)
 */
Bool
HGC1280SaveScreen(pScreen,on)
    ScreenPtr pScreen;
    Bool      on;
{
if (on == SCREEN_SAVER_FORCER)
	SetTimeSinceLastInputEvent();
if (xf86VTSema) {
	/* Kind of hack: to get the screen dark, I set the
	 * "begin [end] of display within scanline" to middle of the screen
	 */
	if (on) { /* Grrr! SaveScreen(on=TRUE) means turn ScreenSaver off */
		if (HGC_Primary) {
			HGC_PRIM_SET_REG(HGC_REG_LEFT_BORDER,138);
			HGC_PRIM_SET_REG(HGC_REG_RIGHT_BORDER,90);
		} else {
			HGC_SEC_SET_REG(HGC_REG_LEFT_BORDER,138);
			HGC_SEC_SET_REG(HGC_REG_RIGHT_BORDER,90);
		}
	} else {
		if (HGC_Primary) {
			HGC_PRIM_SET_REG(HGC_REG_LEFT_BORDER,170);
			HGC_PRIM_SET_REG(HGC_REG_RIGHT_BORDER,50);
		} else {
			HGC_SEC_SET_REG(HGC_REG_LEFT_BORDER,170);
			HGC_SEC_SET_REG(HGC_REG_RIGHT_BORDER,50);
		}
	}
} /* if we are not on the active VT, don't do anything - the screen
   * will be visible as soon as we switch back anyway (?) */
return(TRUE);
}

/* HGC1280Adjust --
 *      adjust the current video frame to display the mousecursor
 *      (x,y) is the upper left corner to be displayed.
 *      This needs some special handling with hgc1280: the card can only pan
 *      in steps of 16 pixels.
 */
void
HGC1280Adjust(x,y)
    int x, y;
{
static int oldx=0;
int val;
#if 0
ErrorF("(%d,%d)",x,y);
if (x>=192)
	ErrorF("hgc1280: HGC1280Adjust(%d,%d) ",x,y);
#endif
/* make shure the pointer is always on the screen */
if (x>oldx)
	/* right side */
	val=16-((x+15)>>4);
else
	/* left side */
	val=16-(x>>4);
oldx=x;
if (HGC_Primary) {
	HGC_PRIM_SET_REG(HGC_REG_SHIFT_DISPLAY,val);
} else {
	HGC_SEC_SET_REG(HGC_REG_SHIFT_DISPLAY,val);
}
}

#define BANK_PRIM_A(_bank) \
		outb(HGC_PRIM_PORT_INDEX,HGC_REG_BANK1); \
		outb(HGC_PRIM_PORT_DATA,_bank);
#define BANK_PRIM_B(_bank) \
		outb(HGC_PRIM_PORT_INDEX,HGC_REG_BANK2); \
		outb(HGC_PRIM_PORT_DATA,_bank);
#define BANK_SEC_A(_bank) \
		outb(HGC_SEC_PORT_INDEX,HGC_REG_BANK1); \
		outb(HGC_SEC_PORT_DATA,_bank);
#define BANK_SEC_B(_bank) \
		outb(HGC_SEC_PORT_INDEX,HGC_REG_BANK2); \
		outb(HGC_SEC_PORT_DATA,_bank);

void HGC1280ClearScreen()
{
    int bank;
    for (bank=0;
         bank<(HGC_MAX_VIRTUAL_Y*HGC_SCAN_LINE_WIDTH/8/HGC_BANK_SIZE);
         bank++) {
	if (HGC_Primary) {
		BANK_PRIM_A(bank);
	} else {
		BANK_SEC_A(bank);
	}
	memset((unsigned char *)monoBankABottom, 0, HGC_BANK_SIZE);
    }
}

static int hgcBankAseg;
static int hgcBankBseg;

PixelType * HGC1280pScanlineOffsetA(p, offset)
    PixelType *p;
    int offset;
{
register /*signed*/ long delta;
register /*signed*/ long deltabank;
  if ((unsigned long)p >= MONOBASE) {
     /* virtual framebuffer address */
     /* p - MONOBASE is not necessarily a valid pointer within the bank */
     /* -> do absolute banking */
     p += offset;
     p = (PixelType *)((unsigned long)p - (unsigned long)MONOBASE);
     deltabank = (/*signed*/ long)p >> HGC_SEGMENT_SHIFT;
     hgcBankAseg = deltabank;
     BANK_PRIM_A(hgcBankAseg);
     p = (PixelType *)( (unsigned long)p + monoBankABottom
                        - (deltabank << HGC_SEGMENT_SHIFT) );
     return(p);
  }
  /* At least on Linux, the mapped Area is at 0x40000000, so test >= first */
  if ((unsigned long)p >= (unsigned long)monoBankABottom) {
     if ((unsigned long)p < (unsigned long)monoBankATop) {
        /* within the framebuffer */
        p += offset;
        /* outside of the bank now ? */
        delta = (/*signed*/ long)p - (/*signed*/ long)monoBankABottom;
        deltabank = delta >> HGC_SEGMENT_SHIFT;
        if (0!=deltabank) {
                hgcBankAseg += deltabank;
                BANK_PRIM_A(hgcBankAseg);
                p = (PixelType *)( (unsigned long)p
                                   - (deltabank << HGC_SEGMENT_SHIFT) );
        }
        return(p);
     }
  }
  /* no framebuffer address */
  return(p+offset);
}

PixelType * HGC1280pScanlineOffsetB(p, offset)
    PixelType *p;
    int offset;
{
register /*signed*/ long delta;
register /*signed*/ long deltabank;
  if ((unsigned long)p >= MONOBASE) {
     p += offset;
     p = (PixelType *)((unsigned long)p - (unsigned long)MONOBASE);
     deltabank = (/*signed*/ long)p >> HGC_SEGMENT_SHIFT;
     hgcBankBseg = deltabank;
     BANK_PRIM_B(hgcBankBseg);
     p = (PixelType *)( (unsigned long)p + monoBankBBottom
                        - (deltabank << HGC_SEGMENT_SHIFT) );
     return(p);
  }
  if ((unsigned long)p >= (unsigned long)monoBankBBottom) {
     if ((unsigned long)p < (unsigned long)monoBankBTop) {
        p += offset;
        delta = (/*signed*/ long)p - (/*signed*/ long)monoBankBBottom;
        deltabank = delta >> HGC_SEGMENT_SHIFT;
        if (0!=deltabank) {
                hgcBankBseg += deltabank;
                BANK_PRIM_B(hgcBankBseg);
                p = (PixelType *)( (unsigned long)p
                                   - (deltabank << HGC_SEGMENT_SHIFT) );
        }
        return(p);
     }
  }
  return(p+offset);
}

PixelType * HGC1280sScanlineOffsetA(p, offset)
    PixelType *p;
    int offset;
{
register /*signed*/ long delta;
register /*signed*/ long deltabank;
  if ((unsigned long)p >= MONOBASE) {
     p += offset;
     p = (PixelType *)((unsigned long)p - (unsigned long)MONOBASE);
     deltabank = (/*signed*/ long)p >> HGC_SEGMENT_SHIFT;
     hgcBankAseg = deltabank;
     BANK_SEC_A(hgcBankAseg);
     p = (PixelType *)( (unsigned long)p + monoBankABottom
                        - (deltabank << HGC_SEGMENT_SHIFT) );
     return(p);
  }
  if ((unsigned long)p >= (unsigned long)monoBankABottom) {
     if ((unsigned long)p < (unsigned long)monoBankATop) {
        p += offset;
        delta = (/*signed*/ long)p - (/*signed*/ long)monoBankABottom;
        deltabank = delta >> HGC_SEGMENT_SHIFT;
        if (0!=deltabank) {
                hgcBankAseg += deltabank;
                BANK_SEC_A(hgcBankAseg);
                p = (PixelType *)( (unsigned long)p
                                   - (deltabank << HGC_SEGMENT_SHIFT) );
        }
        return(p);
     }
  }
  return(p+offset);
}

PixelType * HGC1280sScanlineOffsetB(p, offset)
    PixelType *p;
    int offset;
{
register /*signed*/ long delta;
register /*signed*/ long deltabank;
  if ((unsigned long)p >= MONOBASE) {
     p += offset;
     p = (PixelType *)((unsigned long)p - (unsigned long)MONOBASE);
     deltabank = (/*signed*/ long)p >> HGC_SEGMENT_SHIFT;
     hgcBankBseg = deltabank;
     BANK_SEC_B(hgcBankBseg);
     p = (PixelType *)( (unsigned long)p + monoBankBBottom
                        - (deltabank << HGC_SEGMENT_SHIFT) );
     return(p);
  }
  if ((unsigned long)p >= (unsigned long)monoBankBBottom) {
     if ((unsigned long)p < (unsigned long)monoBankBTop) {
        p += offset;
        delta = (/*signed*/ long)p - (/*signed*/ long)monoBankBBottom;
        deltabank = delta >> HGC_SEGMENT_SHIFT;
        if (0!=deltabank) {
                hgcBankBseg += deltabank;
                BANK_SEC_B(hgcBankBseg);
                p = (PixelType *)( (unsigned long)p
                                   - (deltabank << HGC_SEGMENT_SHIFT) );
        }
        return(p);
     }
  }
  return(p+offset);
}
