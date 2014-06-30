/* $XFree86: xc/programs/Xserver/hw/xfree86/mono/drivers/apollo/apollodriv.c,v 3.5 1996/12/23 06:47:55 dawes Exp $ */
/*
 * MONO: Driver family for interlaced and banked monochrome video adaptors
 * Pascal Haible 8/93, 3/94, 4/94 haible@IZFM.Uni-Stuttgart.DE
 *
 * mono/driver/apollo/apollodriv.c
 * Hamish Coleman 11/93 hamish@zot.apana.org.au
 *
 * derived from:
 * bdm2/hgc1280/...
 * Pascal Haible 8/93, haible@izfm.uni-stuttgart.de
 * hga2/...
 * Author:  Davor Matic, dmatic@athena.mit.edu
 * and
 * vga256/...
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * Thanks to Herb Peyerl (hpeyerl@novatel.ca) for the information on 
 * programming this card.
 *
 * see mono/COPYRIGHT for copyright and disclaimers.
 */
/* $XConsortium: apollodriv.c /main/4 1996/02/21 17:49:23 kaleb $ */

#include "X.h"
#include "input.h"
#include "screenint.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"
#include "mono.h"
#include "apolloHW.h"
#ifdef NOTYET
#include "mfbfuncs.h"
#endif

static int apollo_Current_mode = apollo_Textmode;

/*
 * Define the Apollo I/O Ports
 */
unsigned Apollo_IOBASE = AP_PORT_BASE;

static unsigned Apollo_IOPorts[16] =
	{AP_PORT_BASE, AP_PORT_BASE + 1, AP_PORT_BASE + 2,
	 AP_PORT_BASE + 3, AP_PORT_BASE + 4, AP_PORT_BASE + 5,
	 AP_PORT_BASE + 6, AP_PORT_BASE + 7, AP_PORT_BASE + 8,
	 AP_PORT_BASE + 9, AP_PORT_BASE + 0x0a, AP_PORT_BASE + 0x0b,
	 AP_PORT_BASE + 0x0c, AP_PORT_BASE + 0x0d, AP_PORT_BASE + 0x0e,
	 AP_PORT_BASE + 0x0f};

static int Num_Apollo_IOPorts = (sizeof(Apollo_IOPorts)/sizeof(Apollo_IOPorts[0]));

Bool ApolloProbe(
#if NeedFunctionPrototypes
		void
#endif
);
char *ApolloIdent(
#if NeedFunctionPrototypes
		int
#endif
);
pointer ApolloInit(
#if NeedFunctionPrototypes
		DisplayModePtr
#endif
);
void ApolloEnterLeave(
#if NeedFunctionPrototypes
		Bool /* enter */
#endif
);
pointer ApolloSave(
#if NeedFunctionPrototypes
		pointer
#endif
);
void ApolloRestore(
#if NeedFunctionPrototypes
		pointer
#endif
);
void ApolloAdjust(
#if NeedFunctionPrototypes
		int /* x */,
		int /* y*/
#endif
);
Bool ApolloSaveScreen(
#if NeedFunctionPrototypes
		ScreenPtr /* pScreen */,
		Bool      /* on */
#endif
);
void ApolloFbInit(
#if NeedFunctionPrototypes
		void
#endif
);
void ApolloClearScreen(
#if NeedFunctionPrototypes
		void
#endif
);
PixelType * ApolloScanlineOffsetA(
#if NeedFunctionPrototypes
		PixelType * /* pointer */,
		int         /* offset */
#endif
);
PixelType * ApolloScanlineOffsetB(
#if NeedFunctionPrototypes
		PixelType * /* pointer */,
		int         /* offset */
#endif
);

#ifdef NOTYET
extern int ApolloDoBitblt();
#endif

/*
 * structure for accessing the video chip`s functions
 */

monoVideoChipRec APOLLO = {
  /* Functions */
  ApolloProbe,
  ApolloIdent,
  ApolloInit,
  ApolloFbInit,			/* Initialize acclerated functions */
  ApolloEnterLeave,
  ApolloSave,
  ApolloRestore,
  ApolloAdjust,
  ApolloSaveScreen,
  ApolloClearScreen,		/* ClearScreen */
  ApolloScanlineOffsetA,	/* SetBankA */
  ApolloScanlineOffsetB,	/* SetBankB */
  (unsigned char *)AP_BANK_BOTTOM,	/* BankABottom */
  (unsigned char *)AP_BANK_TOP,		/* BankATop */
  (unsigned char *)AP_BANK_BOTTOM,	/* BankBBottom */
  (unsigned char *)AP_BANK_TOP,		/* BankBTop */
  (unsigned char *)AP_MAP_BASE,	/* MapBase */
  AP_MAP_SIZE,			/* MapSize */
  AP_HDISPLAY,			/* HDisplay */
  AP_VDISPLAY,			/* VDisplay */
  AP_SCAN_LINE_WIDTH,		/* ScanLineWidth */
  {0,},				/* ChipOptionFlags */
};

/*
 * ApolloIdent
 */
char *
ApolloIdent(n)
    int n;
{
static char *chipsets[] = {"apollo9"};
if (n >= sizeof(chipsets) / sizeof(char *))
	return(NULL);
else return(chipsets[n]);
}

/*
 * ApolloProbe --
 *      check whether an Apollo ID-9 board is installed
 */
Bool
ApolloProbe()
{
  /*
   * Set up I/O ports to be used by this card
   */
  if (OFLG_ISSET(OPTION_SECONDARY, &monoInfoRec.options)) {
  	int i;
  	
  	/* option "secondary" */
  	Apollo_IOBASE = AP_PORT_ALTERNATE;
  	APOLLO.ChipMapBase = (unsigned char *)AP_MAP_ALTERNATE;
  	
  	for (i=0;i<Num_Apollo_IOPorts;i++)
  		Apollo_IOPorts[i] = AP_PORT_ALTERNATE + i;
  } else {
  	/* assume primary */
  	Apollo_IOBASE = AP_PORT_BASE;
  }

  xf86ClearIOPortList(monoInfoRec.scrnIndex);
  xf86AddIOPorts(monoInfoRec.scrnIndex, Num_Apollo_IOPorts, Apollo_IOPorts);

  if (monoInfoRec.chipset) {
	/* Chipset preset */
	if (strcmp(monoInfoRec.chipset, ApolloIdent(0)))
		/* desired chipset != this one */
		return (FALSE);
	else {
		ApolloEnterLeave(ENTER);
		/* go on with videoram etc. below */
	}
  }
  else {
	unsigned char id;
	Bool found=FALSE;

	ApolloEnterLeave(ENTER);
	/*
	 * Checks if there is a Apollo ID 09 board in the system.
	 *
	 *
	 */
	id = inb(AP_PORT_DEVICE_ID);

	if (id == AP_MONO_HIRES)
		found=TRUE;
	
	if ( !found ) {
		ApolloEnterLeave(LEAVE);
		return(FALSE);
	}

	monoInfoRec.chipset = ApolloIdent(0);

  } /* else (monoInfoRec.chipset) */
  if (Apollo_IOBASE != AP_PORT_BASE) {
	ErrorF("%s %s: %s: Using secondary address\n",
	XCONFIG_GIVEN, monoInfoRec.name, monoInfoRec.chipset);
  } else {
	ErrorF("%s %s: %s: Using primary address\n",
	XCONFIG_PROBED, monoInfoRec.name, monoInfoRec.chipset);
  }
  if (!monoInfoRec.videoRam) {
	/* videoram not given in XF86Config */
	monoInfoRec.videoRam=256;
  }
  /* We do 'virtual' handling here as it is highly chipset specific */
  /* Screen size (pixels) is fixed, virtual size can be larger up to
   * ChipMaxVirtualX and ChipMaxVirtualY */
  /* Real display size is given by AP_HDISPLAY and AP_VDISPLAY,
   * desired virtual size is monoInfoRec.virtualX and monoInfoRec.virtualY.
   * Think they can be -1 at this point.
   * Maximum virtual size as done by the driver is
   * AP_MAX_VIRTUAL_X and ..._Y
   */
   if (!(monoInfoRec.virtualX < 0)) {
	/* virtual set in XF86Config */
	ErrorF("%s %s: %s: Virtual not allowed for this chipset\n",
		XCONFIG_PROBED, monoInfoRec.name, monoInfoRec.chipset);
   }
   /* Set virtual to real size */
   monoInfoRec.virtualX = AP_HDISPLAY;
   monoInfoRec.virtualY = AP_VDISPLAY;
   
   /* Initialize option flags allowed for this driver */
   OFLG_SET(OPTION_SECONDARY, &APOLLO.ChipOptionFlags);
   OFLG_SET(OPTION_NOACCEL, &APOLLO.ChipOptionFlags);

   /* Apollo framebuffer is linear */
   monoInfoRec.bankedMono = FALSE;

   /* Must return real display size */
   /* hardcoded in APOLLO */
   return(TRUE);
}

/*
 * ApolloFbInit --
 *	enable speedups
 */
void
ApolloFbInit()
{
#ifdef NOTYET
  if (!OFLG_ISSET(OPTION_NOACCEL, &monoInfoRec.options))
    {
      if (xf86Verbose)
        {
          ErrorF ("%s %s: Using accelerator functions\n",
          	  XCONFIG_PROBED, monoInfoRec.chipset);
        }
        
      mfbLowlevFuncs.doBitblt = ApolloDoBitblt;
    }
#endif
}

/*
 * ApolloEnterLeave --
 *      enable/disable io permissions
 */
void 
ApolloEnterLeave(enter)
    Bool enter;
{
  if (enter)
	xf86EnableIOPorts(monoInfoRec.scrnIndex);
  else
	xf86DisableIOPorts(monoInfoRec.scrnIndex);
}

/*
 * ApolloInit --
 *      Handle the initialization of the Apollo's registers
 */
pointer
ApolloInit(mode)
    DisplayModePtr mode;
{
return((pointer)apollo_Graphmode);
}

/*
 * ApolloSave --
 *      save the current video mode
 */
pointer
ApolloSave(save)
    pointer save;
{
return((pointer)apollo_Current_mode);
}

/*
 * ApolloRestore --
 *      restore a video mode
 */
void
ApolloRestore(restore)
    pointer restore;
{
    if ((int)restore==apollo_Textmode) {

	/* Blank the screen */
	
	/* Set text mode */

	/* Unblank the screen */

        apollo_Current_mode=apollo_Textmode;
        
    } else if ((int)restore==apollo_Graphmode) {

	outb(AP_PORT_CONTROL_3,AP_RESET_CREG);
	outw(AP_PORT_ROP, AP_ROP_SRC|(AP_ROP_SRC<<4)|(AP_ROP_SRC<<8)|(AP_ROP_SRC<<12) );
	outb(AP_PORT_CONTROL_0,AP_MODE_NORMAL);
	outb(AP_PORT_CONTROL_1,(AP_BLT_NORMAL | AP_ROP_DISABLE | AP_CREG1_NORMAL));
	outb(AP_PORT_CONTROL_2,AP_DATA_PLN);
	outw(AP_PORT_WRITE_ENABLE,0x0);

        apollo_Current_mode=apollo_Graphmode;

    } else ErrorF("Warning: ApolloRestore called with invalid arg.\n");
}

/*
 * ApolloSaveScreen();
 *	Disable the video on the frame buffer (screensaver)
 */
Bool
ApolloSaveScreen(pScreen,on)
    ScreenPtr pScreen;
	Bool      on;
{
if (on == SCREEN_SAVER_FORCER)
	SetTimeSinceLastInputEvent();
if (xf86VTSema) {
	if (on) { /* Grrr! SaveScreen(on=TRUE) means turn ScreenSaver off */
		outb(AP_PORT_CONTROL_3,AP_BIT_DISP|AP_ON);
/*		outb(AP_PORT_CONTROL_1,(AP_BLT_AD | AP_ROP_DISABLE | AP_CREG1_NORMAL)); */
	} else {
		outb(AP_PORT_CONTROL_3,AP_BIT_DISP|AP_OFF);
/*		outb(AP_PORT_CONTROL_1,(AP_BLT_AD | AP_CREG1_NORMAL) & ~AP_DISP_ENABLE); */
	}
} /* if we are not on the active VT, don't do anything - the screen
   * will be visible as soon as we switch back anyway (?) */
return(TRUE);
}

/* ApolloAdjust --
 *      adjust the current video frame to display the mousecursor
 *      (x,y) is the upper left corner to be displayed.
 */
void
ApolloAdjust(x,y)
    int x, y;
{
}

void
ApolloClearScreen()
{
    /* Chipset specific ClearScreen */
    /* needed as banks are not set up 'correctly' */
    int i, lines;

    lines = AP_SEGMENT_SIZE / ( AP_SCAN_LINE_WIDTH/8 /*Pixels/Byte*/ );
    for (i=0;i<lines;i++) {
	memset((unsigned char *)monoBase + i*(AP_SCAN_LINE_WIDTH/8),
		0, (unsigned int)(AP_HDISPLAY/8));
    }
    /* ... */
}

PixelType * ApolloScanlineOffsetA(p, offset)
    PixelType * p;
    int offset;
{
return(p+offset);
}

PixelType * ApolloScanlineOffsetB(p, offset)
    PixelType * p;
    int offset;
{
return(p+offset);
}
