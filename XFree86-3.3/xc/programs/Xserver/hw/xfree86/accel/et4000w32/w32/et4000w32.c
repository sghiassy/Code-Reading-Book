/*
 * $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/w32/et4000w32.c,v 3.22.2.2 1997/05/09 07:15:12 hohndel Exp $
 *
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 */
/* $XConsortium: et4000w32.c /main/13 1996/10/24 07:10:28 kaleb $ */

 /*
  *  Modified by Glenn G. Lai for the et4000/w32 series accelerators
  *  ET6000 support by Koen Gadeyne
  */

#include "X.h"
#include "input.h"
#include "screenint.h"
#include "dix.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
#include "vga.h"
#include "w32.h"

static Bool     ET4000W32Probe();
static char *   ET4000W32Ident();
static void     ET4000W32EnterLeave();
static Bool     ET4000W32Init();
static int      ET4000W32ValidMode();
static void *   ET4000W32Save();
static void     ET4000W32Restore();
static void     ET4000W32Adjust();
static void     ET4000W32SaveScreen();
extern void     ET4000SetRead();
extern void     ET4000SetWrite();
extern void     ET4000SetReadWrite();

extern vgaVideoChipRec ET4000;
extern Bool (*ClockSelect)();

vgaVideoChipRec ET4000W32 = {
    ET4000W32Probe,
    ET4000W32Ident,
    ET4000W32EnterLeave,
    ET4000W32Init,
    ET4000W32ValidMode,
    ET4000W32Save,
    ET4000W32Restore,
    ET4000W32Adjust,
    ET4000W32SaveScreen,
    (void (*)())NoopDDA,	/* ChipGetMode */
    (void (*)())NoopDDA,	/* ChipFbInit */
    (void (*)())NoopDDA,	/* ChipSetRead */
    (void (*)())NoopDDA,	/* ChipSetWrite */
    (void (*)())NoopDDA,	/* ChipSetReadWrite */
    0x20000,			/* ChipMapSize */
    0x10000,			/* ChipSegmentSize */
    16,				/* ChipSegmentShift */
    0xFFFF,			/* ChipSegmentMask */
    0x00000, 0x10000,		/* ChipReadBottom, ChipReadTop */
    0x00000, 0x10000,		/* ChipWriteBottom, ChipWriteTop */
    TRUE,			/* ChipUse2Banks */
    VGA_NO_DIVIDE_VERT,		/* ChipInterlaceType */
    {0,},			/* ChipOptionFlags */
    8,				/* ChipRounding */
    FALSE,			/* ChipUseLinearAddressing */
    0,				/* ChipLinearBase */
    0,				/* ChipLinearSize*/
    FALSE,			/* ChipHas16bpp */
    FALSE,			/* ChipHas24bpp */
    FALSE,			/* ChipHas32bpp */
    NULL,			/* ChipBuiltinModes */
    1,				/* ChipClockMulFactor */
    1				/* ChipClockDivFactor */
};

static et4000w32_initted = FALSE;
static unsigned ET4000W32_ExtPorts[] = {0x3cb, 0x217a, 0x217b};
static int Num_ET4000W32_ExtPorts = 
	(sizeof(ET4000W32_ExtPorts)/sizeof(ET4000W32_ExtPorts[0]));


static char * w32ChipNames[] = {
	"et4000w32",
	"et4000w32i",
	"et4000w32p_rev_a",
	"et4000w32i_rev_b",
	"reserved",
	"et4000w32p_rev_b",
	"et4000w32p_rev_d",
	"et4000w32p_rev_c",
	"reserved",
	"reserved",
	"reserved",
	"et4000w32i_rev_c",
	"reserved",
	"reserved",
	"reserved",
	"reserved"
};

/*
 * ET4000W32Ident
 */

static char *
ET4000W32Ident(n)
    int n;
{
    static char *
    w32_ids[] = {
	"et4000w32",
	"et4000w32i",
	"et4000w32p_rev_a",
	"et4000w32i_rev_b",
	"et4000w32i_rev_c",
	"et4000w32p_rev_b",
	"et4000w32p_rev_c",
	"et4000w32p_rev_d",
	"et6000",
    };

    if (n + 1 > sizeof(w32_ids) / sizeof(char *))
	return(NULL);
    else
	return(w32_ids[n]);
}


/*
 * ET4000W32Probe --
 */

static Bool
ET4000W32Probe()
{
    static char *et4000w32_id = NULL;
    char *et4000 = "et4000";
    char *save_chipset;
    int save_videoram;
    int i;

    if (et4000w32_id)
	return TRUE;

    if (save_chipset = vga256InfoRec.chipset)
    {
	if (strcmp(save_chipset, et4000) == 0)
	    return FALSE;
#if 0
	vga256InfoRec.chipset = et4000;
#endif
    }

    save_videoram = vga256InfoRec.videoRam;

    if (!ET4000.ChipProbe())
    {
	vga256InfoRec.chipset = save_chipset;
	vga256InfoRec.videoRam = save_videoram;
	return FALSE;
    }

    /* we have to know NOW if it's an ET6000, because W32EnterLeave depends on it */
    if (vga256InfoRec.chipset)
       W32et6000 = strcmp(vga256InfoRec.chipset, "et6000") == 0;
    else
       W32et6000 = FALSE;
    
    if (W32et6000==FALSE)
    {
      vga256InfoRec.chipset = save_chipset; 
      vga256InfoRec.videoRam = save_videoram;
    }

    /*
     *  Set up those I/O ports not in the ET4000 
     */
    ET4000.ChipEnterLeave(LEAVE);
    xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_ET4000W32_ExtPorts,
		   ET4000W32_ExtPorts);
    ET4000W32EnterLeave(ENTER);

    if (vga256InfoRec.chipset)
    {
	i = 0;
	while (et4000w32_id = ET4000W32Ident(i++))
	    if (StrCaseCmp(et4000w32_id, vga256InfoRec.chipset) == 0)
		break;
	if (!et4000w32_id)
	{
	    ET4000W32EnterLeave(LEAVE);
	    return FALSE;
	}
    }
    else
    {
	/*
	 *  Hope this doesn't cause any trouble
	 *  Use a delay loop if it does--GGL
	 */ 
	for (i = 0; i < 4; i++)
	{
	    outb(0x3cb, i);
	    if (i != inb(0x3cb))
	    {
		ErrorF("w32:  failed the segment test\n");
		ET4000W32EnterLeave(LEAVE);
		return FALSE;
	    }
	}
	for (i = 16; i < 64; i += 16)
	{
	    outb(0x3cb, i);
	    if (i != inb(0x3cb))
	    {
		ErrorF("w32:  failed the segment test\n");
		ET4000W32EnterLeave(LEAVE);
		return FALSE;
	    }
	}
	outb(0x217a, 0xec);
	vga256InfoRec.chipset = et4000w32_id = w32ChipNames[inb(0x217b) >> 4];
	if (strcmp(et4000w32_id, "reserved") == 0)
	{
	    vga256InfoRec.chipset = et4000w32_id = NULL;
	    ET4000W32EnterLeave(LEAVE);
	    return FALSE;
	}
    }

    W32 = strcmp(et4000w32_id, "et4000w32") == 0;
    W32i = strcmp(et4000w32_id, "et4000w32i") == 0 ||
	   strcmp(et4000w32_id, "et4000w32i_rev_b") == 0 ||
	   strcmp(et4000w32_id, "et4000w32i_rev_c") == 0;
    W32OrW32i = W32 || W32i;
    W32p = !(W32OrW32i || W32et6000);
    W32pa = strcmp(et4000w32_id, "et4000w32_rev_a") == 0;
    W32pCAndLater = W32p && strcmp(et4000w32_id, "et4000w32p_rev_a") != 0
			 && strcmp(et4000w32_id, "et4000w32p_rev_b") != 0;

#ifdef SHOW_CHIPSET_FAMILIES
    ErrorF("W32=%d; W32i=%d, W32OrW32i=%d, W32p=%d, W32pa=%d, W32pCAndLater=%d, W32et6000=%d\n",
            W32, W32i, W32OrW32i, W32p, W32pa, W32pCAndLater, W32et6000);
#endif

    if (W32et6000)
    {
      /* no ET6000 specific option flags (yet) */
      
      /* get PCI IOBase -- this is a bit of a kludge:
       * it should be done using ordinary PCI scanning
       */
      outb(vgaIOBase + 4, 0x21); ET6Kbase = (inb(vgaIOBase + 5)<<8);
      outb(vgaIOBase + 4, 0x22); ET6Kbase += (inb(vgaIOBase + 5)<<16);
      outb(vgaIOBase + 4, 0x23); ET6Kbase += (inb(vgaIOBase + 5)<<24);

      vga256InfoRec.videoRam -= 1; /* spare mem needed for accel stuff (?) */
      setup_et6000_ramdac();
      return TRUE;
    }

    /* set ET4000/W32 specific options */
    OFLG_SET(OPTION_LEGEND, &ET4000W32.ChipOptionFlags);
    OFLG_SET(OPTION_HIBIT_HIGH, &ET4000W32.ChipOptionFlags);
    OFLG_SET(OPTION_HIBIT_LOW, &ET4000W32.ChipOptionFlags);
    OFLG_SET(OPTION_PCI_BURST_ON, &ET4000W32.ChipOptionFlags);
    OFLG_SET(OPTION_PCI_BURST_OFF, &ET4000W32.ChipOptionFlags);
    OFLG_SET(OPTION_W32_INTERLEAVE_ON, &ET4000W32.ChipOptionFlags);
    OFLG_SET(OPTION_W32_INTERLEAVE_OFF, &ET4000W32.ChipOptionFlags);
#ifndef MONOVGA
    OFLG_SET(OPTION_FAST_DRAM, &ET4000W32.ChipOptionFlags);
    OFLG_SET(OPTION_SLOW_DRAM, &ET4000W32.ChipOptionFlags);
#endif

    if (vga256InfoRec.videoRam == 0)
    {
	outb(vgaIOBase + 0x04, 0x37);
	switch (inb(vgaIOBase + 0x05) & 0x9)
	{
	    case 0: i = 2048; break;
	    case 1: i = 4096; break;
	    case 8: i = 512; break;
	    case 9: i = 1024;
		    if (!W32)
		    {
			outb(vgaIOBase+0x04, 0x32);
			if (inb(vgaIOBase+0x05) & 0x80)
			    i = 2048;
		    }
		    break;
	}
	vga256InfoRec.videoRam = i; 
    }

    vga256InfoRec.videoRam -= 1;
    SetupRamdac();

    return TRUE;
}


/*
 * ET4000W32EnterLeave --
 *      enable/disable io-mapping
 */

static void 
ET4000W32EnterLeave(enter)
    Bool enter;
{
    unsigned char tmp;

    if (enter == ENTER)
    {
	ET4000.ChipEnterLeave(ENTER);

        /* enable w32 mapping */
        if (W32et6000==TRUE)
        {
          tmp = inb(ET6Kbase + 0x40);
          outb(ET6Kbase + 0x40, tmp | 0x06);
        }
        else
        {
          outb(vgaIOBase + 0x4, 0x36);
          tmp = inb(vgaIOBase + 0x5);
          outb(vgaIOBase + 0x5, tmp | 0x28);
        }
    }
    else
    {
	/* force w32 mapping off */
        if (W32et6000==TRUE)
        {
          tmp = inb(ET6Kbase + 0x40);
          outb(ET6Kbase + 0x40, tmp & ~0x06);
        }
        else
        {
          outb(vgaIOBase + 0x4, 0x36);
          tmp = inb(vgaIOBase + 0x5);
          outb(vgaIOBase + 0x5, tmp & ~0x28);
        }

	/* WAIT_XY if strong optimizations performed (in the future)--GGL*/
	ET4000.ChipEnterLeave(LEAVE);
    }
}


static void 
ET4000W32Restore(mode)
    DisplayModePtr mode;
{
    ET4000.ChipRestore(mode);
}


static void *
ET4000W32Save(mode)
    DisplayModePtr mode;
{
    static BOOL first_time = TRUE;
    static unsigned char fix_626_1995;
    void *tmp;

    if (first_time)
    {
	first_time = FALSE;
	outb(vgaIOBase + 4, 0x34); fix_626_1995 = inb(vgaIOBase + 5);
    }

    tmp = ET4000.ChipSave(mode);
    outb(vgaIOBase + 4, 0x34);
    outb(vgaIOBase + 5, fix_626_1995);

    return tmp;
}


/*
 */
static Bool
ET4000W32Init(mode)
    DisplayModePtr mode;
{
    int i;


    if (!ET4000.ChipInit(mode))
	return FALSE;

    if (et4000w32_initted)
	return TRUE;

    W32Buffer			= (ByteP)vgaBase + 96 * 1024;  /* pointer to MMU buffer 0 */
    ACL				= W32Buffer + 16384;           /* pointer to MMU buffer 2 */
#define MMR_BASE (W32Buffer + 0x7f00)  /* memory mapped ACL registers' base address */
    MBP0                        = (LongP) (MMR_BASE);
    MBP1                        = (LongP) (MMR_BASE + 0x4);
    MBP2                        = (LongP) (MMR_BASE + 0x8);

    MMU_CONTROL			= (ByteP) (MMR_BASE + 0x13);

    ACL_SUSPEND_TERMINATE	= (ByteP) (MMR_BASE + 0x30); 
    ACL_OPERATION_STATE		= (ByteP) (MMR_BASE + 0x31);

    ACL_SYNC_ENABLE		= (ByteP) (MMR_BASE + 0x32);
    /* for ET6000, ACL_SYNC_ENABLE becomes ACL_6K_CONFIG */

    ACL_INTERRUPT_MASK		= (ByteP) (MMR_BASE + 0x34);
    ACL_INTERRUPT_STATUS	= (ByteP) (MMR_BASE + 0x35);
    ACL_ACCELERATOR_STATUS	= (ByteP) (MMR_BASE + 0x36);

    /* non-queued for w32p's and ET6000 */
    ACL_NQ_X_POSITION		= (WordP) (MMR_BASE + 0x38);
    ACL_NQ_Y_POSITION		= (WordP) (MMR_BASE + 0x3A);
    /* queued for w32 and w32i */
    ACL_X_POSITION		= (WordP) (MMR_BASE + 0x94);
    ACL_Y_POSITION		= (WordP) (MMR_BASE + 0x96);

    ACL_PATTERN_ADDRESS 	= (LongP) (MMR_BASE + 0x80);
    ACL_SOURCE_ADDRESS		= (LongP) (MMR_BASE + 0x84);

    ACL_PATTERN_Y_OFFSET	= (WordP) (MMR_BASE + 0x88);
    ACL_SOURCE_Y_OFFSET		= (WordP) (MMR_BASE + 0x8A);
    ACL_DESTINATION_Y_OFFSET	= (WordP) (MMR_BASE + 0x8C);

    ACL_VIRTUAL_BUS_SIZE 	= (ByteP) (MMR_BASE + 0x8E);
    /* w32p's */
    ACL_PIXEL_DEPTH 		= (ByteP) (MMR_BASE + 0x8E);

    /* w32 and w32i */
    ACL_XY_DIRECTION 		= (ByteP) (MMR_BASE + 0x8F);


    ACL_PATTERN_WRAP		= (ByteP) (MMR_BASE + 0x90);
    ACL_SOURCE_WRAP		= (ByteP) (MMR_BASE + 0x92);

    ACL_X_COUNT			= (WordP) (MMR_BASE + 0x98);
    ACL_Y_COUNT			= (WordP) (MMR_BASE + 0x9A);

    ACL_ROUTING_CONTROL		= (ByteP) (MMR_BASE + 0x9C);
    /* for ET6000, ACL_ROUTING_CONTROL becomes ACL_MIX_CONTROL */
    ACL_RELOAD_CONTROL		= (ByteP) (MMR_BASE + 0x9D);
    /* for ET6000, ACL_RELOAD_CONTROL becomes ACL_STEPPING_INHIBIT */

    ACL_BACKGROUND_RASTER_OPERATION	= (ByteP) (MMR_BASE + 0x9E); 
    ACL_FOREGROUND_RASTER_OPERATION	= (ByteP) (MMR_BASE + 0x9F);

    ACL_DESTINATION_ADDRESS 	= (LongP) (MMR_BASE + 0xA0);

    /* the following is for the w32p's only */
    ACL_MIX_ADDRESS 		= (LongP) (MMR_BASE + 0xA4);

    ACL_MIX_Y_OFFSET 		= (WordP) (MMR_BASE + 0xA8);
    ACL_ERROR_TERM 		= (WordP) (MMR_BASE + 0xAA);
    ACL_DELTA_MINOR 		= (WordP) (MMR_BASE + 0xAC);
    ACL_DELTA_MAJOR 		= (WordP) (MMR_BASE + 0xAE);

    /* and this is only for the ET6000 */
    ACL_POWER_CONTROL		= (ByteP) (MMR_BASE + 0x37);

    W32BltCount = 8192/vga256InfoRec.displayWidth;
    W32BltHop = vga256InfoRec.displayWidth * W32BltCount;
    W32BoxCount = 16384/vga256InfoRec.displayWidth;
    W32BoxHop = vga256InfoRec.displayWidth * W32BoxCount;
    W32PointCount = 16383/vga256InfoRec.displayWidth + 1;
    W32PointHop = vga256InfoRec.displayWidth * W32PointCount;
    W32Foreground = vga256InfoRec.virtualX * vga256InfoRec.virtualY;
    W32Background = W32Foreground + 8;
    W32Pattern = W32Foreground + 16;
    W32Mix = W32Foreground + 24;

    W32MixPong = W32Mix + 504;
    MixDstPing = W32Mix << 3;
    MixDstPong = W32MixPong << 3;

    RESET_ACL

    return et4000w32_initted = TRUE;
}


/*
 * ET4000Adjust --
 *      adjust the current video frame to display the mouse cursor
 */

static void 
ET4000W32Adjust(x, y)
    int x, y;
{
    int Base = (y * vga256InfoRec.displayWidth + x + 1) >> 2;

    outw(vgaIOBase + 4, (Base & 0x00FF00) | 0x0C);
    outw(vgaIOBase + 4, ((Base & 0x00FF) << 8) | 0x0D);
    outw(vgaIOBase + 4, ((Base & 0x0F0000) >> 8) | 0x33);
}


static void 
ET4000W32SaveScreen(start_finish)
    Bool start_finish; 
{
    unsigned char tmp;

    vgaHWSaveScreen(start_finish);

    if (start_finish == SS_FINISH)
    {
	/* set KEY */
	outb(0x3BF, 0x03);
	outb(vgaIOBase + 8, 0xA0);

	/* enable w32 */
	if (W32et6000)
	{
          tmp = inb(ET6Kbase + 0x40);
          outb(ET6Kbase + 0x40, tmp | 0x06);
	}
	else
	{
	  outb(vgaIOBase + 0x4, 0x36);
	  tmp = inb(vgaIOBase + 0x5);
	  outb(vgaIOBase + 0x5, tmp | 0x28);
	}
	if (et4000w32_initted) RESET_ACL;
    }
}

/*
 * ET4000W32ValidMode --
 *
 */
static int
ET4000W32ValidMode(mode, verbose, flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
return MODE_OK;
}

