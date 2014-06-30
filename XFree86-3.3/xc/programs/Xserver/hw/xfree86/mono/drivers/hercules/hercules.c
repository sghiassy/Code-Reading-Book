/* $XFree86: xc/programs/Xserver/hw/xfree86/mono/drivers/hercules/hercules.c,v 3.7 1996/12/23 06:48:06 dawes Exp $ */
/*
 * MONO: Driver family for interlaced and banked monochrome video adaptors
 * Pascal Haible 8/93, 3/94, 4/94 haible@IZFM.Uni-Stuttgart.DE
 *
 * mono/drivers/hercules
 *
 * original:
 * hga2/drivers/hga6845.c
 *
 * Author:  Davor Matic, dmatic@athena.mit.edu
 *
 * heavily edited for R6 by
 * Pascal Haible 4/94 haible@IZFM.Uni-Stuttgart.DE
 *
 */
/* $XConsortium: hercules.c /main/8 1996/10/22 10:35:33 kaleb $ */

#define _NEED_SYSI86

#include "X.h"
#include "input.h"
#include "screenint.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"

#include "mono.h"

/*
 * Define the generic HGA I/O Ports
 */
#define HGA_INDEX	0x3B4
#define HGA_DATA	0x3B5
#define HGA_MODE	0x3B8
#define HGA_STATUS	0x3BA
#define HGA_CONFIG	0x3BF
static unsigned HGA_IOPorts[] = { HGA_INDEX, HGA_DATA, HGA_MODE, HGA_STATUS,
				HGA_CONFIG };
static int Num_HGA_IOPorts = (sizeof(HGA_IOPorts)/sizeof(HGA_IOPorts[0]));

/*
 * Since the conf and mode registers are write only, we need to keep 
 * a local copy of the state here.  The initial state is assumed to be:
 * conf: enable setting of graphics mode, and disable page one 
 *       (allows coexistence with a color graphics board)
 * mode: text, deactivate screen, enable text blink, and page zero at 0xB0000
 */
static unsigned char static_config = 0x01;
static unsigned char static_mode = 0x20;

/*
 * Since the table of 6845 registers is write only, we need to keep
 * a local copy of the state here.  The initial state is assumed to 
 * be 80x25 text mode.
 */
static unsigned char 
  static_tbl[] = {0x61, 0x50, 0x52, 0x0F, 0x19, 0x06, 0x19, 0x19,
		  0x02, 0x0D, 0x0B, 0x0C, 0x00, 0x00, 0x00, 0x28};

static unsigned char init_conf = 0x03;
static unsigned char init_mode = 0x0A;
static unsigned char       /* 720x348 graphics mode parameters */
  init_tbl[] = {0x35, 0x2D, 0x2E, 0x07, 0x5B, 0x02, 0x57, 0x57,
		  0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A};

typedef struct {
  unsigned char config; /* write only conf register at port 0x3BF */
  unsigned char mode; /* write only mode register at port 0x3B8 */
  unsigned char tbl[16];
  } hga6845Rec, *hga6845Ptr;

static Bool     HGA6845Probe(
#if NeedFunctionPrototypes
void
#endif
);
static char *   HGA6845Ident(
#if NeedFunctionPrototypes
int
#endif
);
static pointer  HGA6845Init(
#if NeedFunctionPrototypes
DisplayModePtr /* mode */
#endif
);
static void     HGA6845EnterLeave(
#if NeedFunctionPrototypes
Bool /* enter */
#endif
);
static pointer  HGA6845Save(
#if NeedFunctionPrototypes
pointer
#endif
);
static void     HGA6845Restore(
#if NeedFunctionPrototypes
pointer
#endif
);
static Bool	HGA6845SaveScreen(
#if NeedFunctionPrototypes
ScreenPtr /* pScreen */,
Bool      /* on */
#endif
);
static void	HGA6845ClearScreen(
#if NeedFunctionPrototypes
void
#endif
);
static PixelType *HGA6845ScanlineOffset(
#if NeedFunctionPrototypes
PixelType * /* p */,
int         /* offset */
#endif
);
static int      HGA6845MapFrameBuffer(
#if NeedFunctionPrototypes
void
#endif
);

#define		HGA6845MapBase		((unsigned char *)0xB0000)
#define		HGA6845MapSize		0x8000
#define		HGA6845ScanLineWidth	736
#define		HGA6845HDisplay		720
#define		HGA6845VDisplay		348

monoVideoChipRec HERCULES = {
  HGA6845Probe,
  HGA6845Ident,
  HGA6845Init,
  (void (*)())NoopDDA,		/* FbInit */
  HGA6845EnterLeave,
  HGA6845Save,
  HGA6845Restore,
  (void (*)())NoopDDA,		/* Adjust */
  HGA6845SaveScreen,
  HGA6845ClearScreen,
  HGA6845ScanlineOffset,	/* ScanlineOffsetA */
  HGA6845ScanlineOffset,	/* ScanlineOffsetB */
  0,				/* BankABottom */
  (unsigned char *)HGA6845MapSize,	/* BankATop */
  0,				/* BankBBottom */
  0,				/* BankBTop */
  HGA6845MapBase,
  HGA6845MapSize,
  HGA6845HDisplay,
  HGA6845VDisplay,
  HGA6845ScanLineWidth,
  {0,},				/* OptionFlags */
};

/*
 * HGA6845Ident
 */

char *
HGA6845Ident(n)
    int n;
{
    static char *chipsets[] = {"hercules"};
    if (n >= sizeof(chipsets) / sizeof(char *))
        return(NULL);
    else
        return(chipsets[n]);
}

/*
 * HGA6845Probe --
 *      check whether an HGA6845 based board is installed
 */

static Bool
HGA6845Probe()
{
    /*
     * Set up I/O ports to be used by this card
     */
    xf86ClearIOPortList(monoInfoRec.scrnIndex);
    xf86AddIOPorts(monoInfoRec.scrnIndex, Num_HGA_IOPorts, HGA_IOPorts);

    if (monoInfoRec.chipset) {
	if (strcmp(monoInfoRec.chipset, HGA6845Ident(0)))
	    return (FALSE);
	else
	    HGA6845EnterLeave(ENTER);
    } else {
#define DSP_VSYNC_MASK  0x80
#define DSP_ID_MASK  0x70
	unsigned char dsp, dsp_old;
	int i, cnt;

	HGA6845EnterLeave(ENTER);
	/*
	 * Checks if there is a HGA 6845 based bard in the system.
	 * The following loop tries to see if the Hercules display 
	 * status port register is counting vertical syncs (50Hz). 
	 */
	cnt = 0;
	dsp_old = inb(HGA_STATUS) & DSP_VSYNC_MASK;
	for (i = 0; i < 0x10000; i++) {
	    dsp = inb(HGA_STATUS) & DSP_VSYNC_MASK;
	    if (dsp != dsp_old)
		cnt++;
	    dsp_old = dsp;
	}

	/* If there are active sync changes, we found a Hercules board. */
	if (cnt) {
	    monoInfoRec.videoRam = 64;

	    /* The Plus and InColor versions have an ID code as well. */
	    dsp = inb(HGA_STATUS) & DSP_ID_MASK;
	    switch(dsp) {
		case 0x10:        /* Plus */
			monoInfoRec.videoRam = 64;
			break;
		case 0x50:        /* InColor */
			monoInfoRec.videoRam = 256;
			break;
	    }
	} else { /* there is no hga card */
	    HGA6845EnterLeave(LEAVE);
	    return(FALSE);
	}

    }
    /* The following is done for both probed and preset chipset */

    if (!monoInfoRec.videoRam) {
	/* videoram not given in XF86Config */
	monoInfoRec.videoRam=32;
    }

    /* Set the chipset here so we don't ErrorF() a NULL string below. */
    monoInfoRec.chipset = HGA6845Ident(0);

    /* We do 'virtual' handling here as it is highly chipset specific */
    if (!(monoInfoRec.virtualX < 0)) {
	/* virtual set in XF86Config */
	ErrorF("%s %s: %s: Virtual not allowed for this chipset\n",
		XCONFIG_PROBED, monoInfoRec.name, monoInfoRec.chipset);
    }

    monoInfoRec.virtualX = HGA6845HDisplay;
    monoInfoRec.virtualY = HGA6845VDisplay;

    /* Inform the code above that the framebuffer is banked. H.J. */
    monoInfoRec.bankedMono = TRUE;

    return(TRUE);
}

/*
 * HGA6845EnterLeave --
 *      enable/disable io-mapping
 */
static void 
HGA6845EnterLeave(enter)
     Bool enter;
{
    if (enter)
	xf86EnableIOPorts(monoInfoRec.scrnIndex);
    else
	xf86DisableIOPorts(monoInfoRec.scrnIndex);
}

/*
 * HGA6845Restore --
 *      restore a video mode
 */
static void
HGA6845Restore(restore)
     pointer restore;
{
    unsigned char i;

    /* Clear screen first */
    HGA6845ClearScreen();

    outb(HGA_CONFIG, static_config = ((hga6845Ptr)restore)->config);
    outb(HGA_MODE, static_mode = ((hga6845Ptr)restore)->mode);

    for (i = 0; i < 16; i++) {
	outb(HGA_INDEX, i);
	outb(HGA_DATA, static_tbl[i] = ((hga6845Ptr)restore)->tbl[i]);
    }
    outb(HGA_MODE,static_tbl[15]);
}

/*
 * HGA6845Save --
 *      save the current video mode
 */
static pointer
HGA6845Save(save)
     pointer save;
{
    unsigned char i;

    if (NULL==save)
	save = (pointer)xcalloc(1,sizeof(hga6845Rec));
    ((hga6845Ptr)save)->config = static_config;
    ((hga6845Ptr)save)->mode   = static_mode;
    for (i = 0; i < 16; i++)
	((hga6845Ptr)save)->tbl[i] = static_tbl[i];

    return (save);
}

/*
 * HGA6845Init --
 *      Handle the initialization of the HGAs registers
 */
static pointer
HGA6845Init(mode)
     DisplayModePtr mode;
{
    unsigned char i;
    hga6845Ptr new_mode;

    if (NULL==(new_mode = (hga6845Ptr)xcalloc(1,sizeof(hga6845Rec))))
	return(NULL);
    new_mode->config = init_conf;
    new_mode->mode = init_mode;
    for (i = 0; i < 16; i++)
	new_mode->tbl[i] = init_tbl[i];
    if (0!=HGA6845MapFrameBuffer())
	return(NULL);
    return(new_mode);
}

/*
 * HGA6845SaveScreen -- 
 *      Disable the video on the frame buffer to save the screen.
 */
Bool
HGA6845SaveScreen (pScreen, on)
     ScreenPtr     pScreen;
     Bool          on;
{
    if (on == SCREEN_SAVER_FORCER) {
	SetTimeSinceLastInputEvent();
    } else {
	if (xf86VTSema) { /* our VT is active */
	    if (on)
		outb(HGA_MODE, static_mode |= 0x08); /*   activate screen */
	    else
		outb(HGA_MODE, static_mode &= 0xF7); /* deactivate screen */
	} else {
	    /* if we are not on the active VT, don't do anything - the screen
	     * will be visible as soon as we switch back anyway (?) */
#if 0
	    if (on)
		((hgaHWPtr)hgaNewVideoState)->mode |= 0x08;
	    else
		((hgaHWPtr)hgaNewVideoState)->mode &= 0xF7;
#endif
	}
    }
    return(TRUE);
}

static unsigned long *hga6845Mapping=NULL;
static unsigned long *hga6845RevMapping=NULL;

/*
 * HGA6845MapFrameBuffer --
 *     maps the frame buffer
 */
static int HGA6845MapFrameBuffer()
{
    unsigned long voffset;
    unsigned long roffset;

    if ( (NULL!=hga6845Mapping) && (NULL!=hga6845RevMapping) )
	return(0); /* already done */

    if (NULL!=hga6845Mapping)
	xfree(hga6845Mapping);
    hga6845Mapping    = (unsigned long *)xalloc(
				HGA6845MapSize * sizeof(unsigned long));
    if (NULL!=hga6845RevMapping)
	xfree(hga6845RevMapping);
    hga6845RevMapping = (unsigned long *)xalloc(
				HGA6845MapSize * sizeof(unsigned long));
    if ( (NULL==hga6845Mapping) || (NULL==hga6845RevMapping) ) {
	if (NULL!=hga6845Mapping)
	    xfree(hga6845Mapping);
	if (NULL!=hga6845RevMapping)
	    xfree(hga6845RevMapping);
	return(-1);
    }

    /* voffset: virtual offset in byte */
    /* roffset: real offset in byte, not PixelType aligned */
    for (voffset = 0; voffset < HGA6845MapSize; voffset++) {
	roffset = ((voffset/92)%4)*8192
		  + ((voffset/92)/4)*90
		  + voffset%92 ;
	hga6845Mapping[voffset] = roffset;
	hga6845RevMapping[roffset] = voffset;
    }
    return(0);
}

static PixelType *HGA6845ScanlineOffset(p, offset)
	PixelType *p;
	int offset;
{
    /* offset is in PixelType units !! */
    /* index and output of hga6845Mapping is in byte units */
    /* index and output of hga6845RevMapping is in byte units */
    if ((unsigned char *)p >= (unsigned char *)MONOBASE) {
	return((PixelType *)(monoBankABottom
			     + hga6845Mapping[(unsigned char *)p
					      - (unsigned char *)MONOBASE
					      + offset*sizeof(PixelType)]));
    }
    if ((unsigned char *)p >= monoBankABottom)
	if ((unsigned char *)p < monoBankATop) {
	    return((PixelType *)(monoBankABottom +
			hga6845Mapping[hga6845RevMapping[(unsigned char *)p
							- monoBankABottom]
					+ offset*sizeof(PixelType)]));
	}
    /* no framebuffer address */
    return(p + offset);
}

static void HGA6845ClearScreen()
{
    if (NULL!=monoBankABottom)
	memset(monoBankABottom,0,HGA6845MapSize);
}
