/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/tvga8900/t89_driver.c,v 3.66.2.11 1997/05/28 13:12:53 dawes Exp $ */
/*
 * Copyright 1992 by Alan Hourihane, Wigan, England.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Alan Hourihane not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Alan Hourihane makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ALAN HOURIHANE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ALAN HOURIHANE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Alan Hourihane, alanh@logitek.co.uk, version 0.1beta
 * 	    David Wexelblat, added ClockSelect logic. version 0.2.
 *	    Alan Hourihane, tweaked Init code (5 reg hack). version 0.2.1.
 *	    Alan Hourihane, removed ugly tweak code. version 0.3
 *          		    changed vgaHW.c to accomodate changes.
 * 	    Alan Hourihane, fix Restore called incorrectly. version 0.4
 *
 *	    Alan Hourihane, sent to x386beta team, version 1.0
 *
 *	    David Wexelblat, edit for comments.  Support 8900C only, dual
 *			     bank mode.  version 2.0
 *
 *	    Alan Hourihane, move vgaHW.c changes here for now. version 2.1
 *	    David Wexelblat, fix bank restoration. version 2.2
 *	    David Wexelblat, back to single bank mode.  version 2.3
 *	    Alan Hourihane, fix monochrome text restoration. version 2.4
 *	    Gertjan Akkerman, add TVGA9000 hacks.  version 2.5
 *
 *	    David Wexelblat, massive rewrite to support 8800CS, 8900B,
 *			     8900C, 8900CL, 9000.  Support 512k and 1M.
 *			     Version 3.0.
 *
 *          Alan Hourihane, support for TGUI9440 and compatibles. Version 3.1
 *
 *	    Alan Hourihane, rewrote to support all cards with appropriate 
 * 			    speedups. Linear access, hw cursor etc... v4.0
 *			    Needs more work yet though !
 * 
 *	    Massimiliano Ghilardi, max@Linuz.sns.it, some fixes to the
 *				   clockchip programming code.
 */
/* $XConsortium: t89_driver.c /main/33 1996/10/28 05:31:03 kaleb $ */

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
#define INITIALIZE_LIMITS
#include "t89_driver.h"
#include "tgui_ger.h"
#include "vgaPCI.h"

#ifdef XFreeXDGA
#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

#ifdef XF86VGA16
#define MONOVGA
#endif

#ifndef MONOVGA
#include "tgui_drv.h"
#include "tgui_mmio.h"
#include "vga256.h"
extern vgaHWCursorRec vgaHWCursor;
#endif

typedef struct {
	vgaHWRec std;          		/* std IBM VGA register 	*/
	unsigned char ConfPort;		/* For memory selection 	*/
	unsigned char OldMode2;		/* To enable memory banks 	*/
	unsigned char OldMode1;		/* For clock select		*/
	unsigned char NewMode2;		/* For clock select 		*/
	unsigned char NewMode1;		/* For write bank select 	*/
	unsigned char CRTCModuleTest;	/* For interlace mode 		*/
	unsigned char LinearAddReg;	/* For Linear Register TGUI	*/
	unsigned char VCLK_O;		/* For MiscOutReg		*/
	unsigned char VCLK_A;		/* For Programmable Clock (low) */
	unsigned char VCLK_B;		/* For Programmable Clock (hi)  */
	unsigned char MCLK_A;		/* For Programmable MCLK	*/
	unsigned char MCLK_B;		/* For Programmable MCLK	*/
	unsigned char VLBusReg;		/* For VL Bus and 32bit mode 	*/
	unsigned char MiscExtFunc;	/* For Misc. Ext. Functions     */
	unsigned char GraphEngReg;	/* For Graphic Engine Control   */
	unsigned char PCIReg;		/* For PCI Bursts		*/
	unsigned char AddColReg;	/* For Address/Colour Register  */
	unsigned char CommandReg;	/* For DAC Command Register     */
	unsigned char TRDReg;		/* For DAC Setup 		*/
	unsigned char MiscIntContReg;	/* For Misc. Int. Cont. Reg.    */
	unsigned char PixelBusReg;	/* For Pixel Bus Register	*/
	unsigned char CRTHiOrd;		/* For CRTC bit 10, Start Add.  */
	unsigned char AltClock;		/* For Alternate Clock Selection*/
	unsigned char CurConReg;	/* For HW Cursor Control	*/
	unsigned char CursorRegs[16];	/* For Cursor Registers 	*/
	unsigned char CyberCont;	/* For Cyber Control LCD	*/
	unsigned char CyberVExp;	/* For Cyber VDisplay Control   */
	unsigned char CyberHExp;	/* For Cyber HDisplay Control   */
	unsigned char CyberEnhance;	/* For Cyber Enhancement	*/
	unsigned char DRAMTiming;	/* For 9400/9420/9430 DRAM 	*/
	unsigned char FIFOControl;	/* For 9400/9420/9430 FIFO 	*/
	unsigned char Performance;	/* For 968x FIFO		*/
} vgaTVGA8900Rec, *vgaTVGA8900Ptr;

static Bool TVGA8900ClockSelect();
static char *TVGA8900Ident();
static Bool TVGA8900Probe();
static void TVGA8900EnterLeave();
static Bool TVGA8900Init();
static int  TVGA8900ValidMode();
static void *TVGA8900Save();
static void TVGA8900Restore();
static void TVGA8900FbInit();
static void TVGA8900Adjust();
extern void TVGA8900SetRead();
extern void TVGA8900SetWrite();
extern void TVGA8900SetReadWrite();
static void TVGA8900DisplayPowerManagementSet();
extern void TGUISetRead();
extern void TGUISetWrite();
extern void TGUISetReadWrite();
static int  TGUIPitchAdjust();

vgaVideoChipRec TVGA8900 = {
  TVGA8900Probe,
  TVGA8900Ident,
  TVGA8900EnterLeave,
  TVGA8900Init,
  TVGA8900ValidMode,
  TVGA8900Save,
  TVGA8900Restore,
  TVGA8900Adjust,
  vgaHWSaveScreen,
  (void (*)())NoopDDA,
  TVGA8900FbInit,
  TVGA8900SetRead,
  TVGA8900SetWrite,
  TVGA8900SetReadWrite,
  0x10000,
  0x10000,
  16,
  0xffff,
  0x00000, 0x10000,
  0x00000, 0x10000,
  FALSE,                               /* Uses a single bank */
  VGA_DIVIDE_VERT,
  {0,},
  8,				/* Set to 16 for 512k cards in Probe() */
  FALSE,			/* Linear Addressing */
  0,
  0,
  FALSE,
  FALSE,
  FALSE,
  NULL,
  1,            /* ChipClockMulFactor */
  1             /* ChipClockDivFactor */
};

#define new ((vgaTVGA8900Ptr)vgaNewVideoState)

int TVGAchipset;
static int numClocks;
int tridentHWCursorType = 0;
int tridentDisplayWidth;
int tridentDACtype = -1;
int GE_OP;
Bool tridentHasAcceleration = FALSE;
Bool tridentUseLinear = FALSE;
Bool tridentTGUIProgrammableClocks = FALSE;
Bool tridentIsTGUI = FALSE;
Bool tridentLinearOK = FALSE;
Bool IsCyber = FALSE;
static int CyberLCDHeight, CyberLCDWidth;
static unsigned char DRAMspeed;
static int TridentDisplayableMemory;
unsigned char *tguiMMIOBase = NULL;

static unsigned TGUI_ExtPorts[] = {
	0x210A, 0x210C, 0x210D, 0x210E, 0x210F,		/* For Old GE */
	0x211A, 0x211C, 0x211D, 0x211E, 0x211F,
	0x212A, 0x212C, 0x212D, 0x212E, 0x212F,
	0x213A, 0x213C, 0x213D, 0x213E, 0x213F,
	0x2120, 0x2122, 0x2124, 0x2127, 0x2128,		/* For New GE */
	0x212C, 0x2130, 0x2134, 0x2138, 0x213A,
	0x213C, 0x213E, 0x2140, 0x2142, 0x2148,
	0x214A, 0x214C, 0x214E,
	0x43C6, 0x43C7, 0x43C8, 0x43C9,};		/* For MCLK/VCLK */

static int Num_TGUI_ExtPorts =
	(sizeof(TGUI_ExtPorts)/sizeof(TGUI_ExtPorts[0]));

int TGUIRops_alu[16] = {
	TGUIROP_0,
	TGUIROP_AND,
	TGUIROP_SRC_AND_NOT_DST,
	TGUIROP_SRC,
	TGUIROP_NOT_SRC_AND_DST,
	TGUIROP_DST,
	TGUIROP_XOR,
	TGUIROP_OR,
	TGUIROP_NOR,
	TGUIROP_XNOR,
	TGUIROP_NOT_DST,
	TGUIROP_SRC_OR_NOT_DST,
	TGUIROP_NOT_SRC,
	TGUIROP_NOT_SRC_OR_DST,
	TGUIROP_NAND,
	TGUIROP_1
};

int TGUIRops_Pixalu[16] = {
	TGUIROP_0,
	TGUIROP_AND_PAT,
	TGUIROP_PAT_AND_NOT_DST,
	TGUIROP_PAT,
	TGUIROP_NOT_PAT_AND_DST,
	TGUIROP_DST,
	TGUIROP_XOR_PAT,
	TGUIROP_OR_PAT,
	TGUIROP_NOR_PAT,
	TGUIROP_XNOR_PAT,
	TGUIROP_NOT_DST,
	TGUIROP_PAT_OR_NOT_DST,
	TGUIROP_NOT_PAT,
	TGUIROP_NOT_PAT_OR_DST,
	TGUIROP_NAND_PAT,
	TGUIROP_1
};

#ifdef PC98_TGUI
#include "pc98_tgui.h"
extern Bool BoardInit(void);
extern PC98TGUiTable *pc98TGUi;
#endif

/*
 * TGUISetClock -- Set programmable clock for TGUI cards !
 */
static
TGUISetClock(no)
	int no;
{
	int clock_diff = 750;
	int freq, ffreq;
	int m, n, k;
	int p, q, r, s; 
	int startn, endn;
	int endm;
	unsigned char temp;

	p = q = r = s = 0;

	if (IsCyber)
	{
		startn = 64;
		endn = 255;
		endm = 64;
	}
	else
	{
		startn = 1;
		endn = 122;
		endm = 32;
	}

	freq = vga256InfoRec.clock[no];

	if ((vgaBitsPerPixel == 16) && (TVGAchipset <= TGUI9440AGi))
		freq *= 2; 
	if (vgaBitsPerPixel == 32)
		freq *= 2;
	if (vgaBitsPerPixel == 24)
		freq *= 3;

	for (k=1;k<3;k++)
	  for (n=startn;n<endn;n++)
	    for (m=1;m<endm;m++)
	    {
		ffreq = ((( (n + 8) * 14.31818) / ((m + 2) * k)) * 1000);
		if ((ffreq > freq - clock_diff) && (ffreq < freq + clock_diff)) 
		{
			if ( (n+8)*100/(m+2) < 978 && (n+8)*100/(m+2) > 349 ) {
				clock_diff = (freq > ffreq) ? freq - ffreq : ffreq - freq;
				p = n; q = m; r = k; s = ffreq;
			}
		}
	    }

	if (s == 0)
	{
		TVGA8900EnterLeave(LEAVE);
		FatalError("Unable to set programmable clock.\n"
			   "Frequency %d is not a valid clock.\n"
			   "Please modify XF86Config for a new clock.\n",	
			   freq);
	}

	temp = inb(0x3CC);
	new->VCLK_O = (temp & 0xF3) | 0x08;
	if (IsCyber)
	{
		new->VCLK_A = p;
		new->VCLK_B = (q & 0x2F) | ((r - 1) << 6);
	}
	else
	{
		new->VCLK_A = ((1 & q) << 7) | p;
		new->VCLK_B = (((q & 0xFE) >> 1) | ((r - 1) << 4));
	}
}

/*
 * TVGA8900Ident --
 */
static char *
TVGA8900Ident(n)
	int n;
{
	static char *chipsets[] = {"tvga8200lx", "tvga8800cs", "tvga8900b", 
				   "tvga8900c", 
			     	   "tvga8900cl", "tvga8900d", "tvga9000",  
				   "tvga9000i", "tvga9100b",
				   "tvga9200cxr", "tgui9320lcd",
				   "tgui9400cxi", "tgui9420",
				   "tgui9420dgi", "tgui9430dgi",
				   "tgui9440agi", "tgui96xx",
				   "cyber938x",
				  };

	if (n + 1 > sizeof(chipsets) / sizeof(char *))
		return(NULL);
	else
		return(chipsets[n]);
}

/*
 * TVGA8900ClockSelect --
 * 	select one of the possible clocks ...
 */
static Bool
TVGA8900ClockSelect(no)
	int no;
{
	static unsigned char save1, save2, save3;
	unsigned char temp;

	/*
	 * CS0 and CS1 are in MiscOutReg
	 *
	 * For 8900B, 8900C, 8900CL and 9000, CS2 is bit 0 of
	 * New Mode Control Register 2.
	 *
	 * For 8900CL, CS3 is bit 4 of Old Mode Control Register 1.
	 *
	 * For 9000, CS3 is bit 6 of New Mode Control Register 2.
	 *
	 * For TGUI, we don't use the ClockSelect function at all.
	 */
	switch(no)
	{
	case CLK_REG_SAVE:
		save1 = inb(0x3CC);
		if (TVGAchipset != TVGA8800CS)
		{
			outw(0x3C4, 0x000B);	/* Switch to Old Mode */
			inb(0x3C5);		/* Now to New Mode */
			outb(0x3C4, 0x0D); save2 = inb(0x3C5);
			if ( (numClocks == 16) && 
			     (TVGAchipset != TVGA9000) &&
			     (TVGAchipset != TVGA9000i) )
			{
				outw(0x3C4, 0x000B);	/* Switch to Old Mode */
				outb(0x3C4, 0x0E); save3 = inb(0x3C5);
			}
		}
		break;
	case CLK_REG_RESTORE:
		outb(0x3C2, save1);
		if (TVGAchipset != TVGA8800CS)
		{
			outw(0x3C4, 0x000B);	/* Switch to Old Mode */
			inb(0x3C5);		/* Now to New Mode */
			outw(0x3C4, (save2 << 8) | 0x0D); 
			if ( (numClocks == 16) && 
			     (TVGAchipset != TVGA9000) &&
			     (TVGAchipset != TVGA9000i) )
			{
				outw(0x3C4, 0x000B);	/* Switch to Old Mode */
				outw(0x3C4, (save3 << 8) | 0x0E);
			}
		}
		break;
	default:
		/*
	 	 * Do CS0 and CS1
	 	 */
		temp = inb(0x3CC);
		outb(0x3C2, (temp & 0xF3) | ((no << 2) & 0x0C));
		if (TVGAchipset != TVGA8800CS)
		{
			/* 
	 	 	 * Go to New Mode for CS2 and TVGA9000 CS3.
	 	 	 */
			outw(0x3C4, 0x000B);	/* Switch to Old Mode */
			inb(0x3C5);		/* Now to New Mode */
			outb(0x3C4, 0x0D);
			/*
			 * Bits 1 & 2 are dividers - set to 0 to get no
			 * clock division.
			 */
			temp = inb(0x3C5) & 0xF8;
			temp |= (no & 0x04) >> 2;
			if ( (TVGAchipset == TVGA9000) ||
			     (TVGAchipset == TVGA9000i) )
			{
				temp &= ~0x40;
				temp |= (no & 0x08) << 3;
			}
			outb(0x3C5, temp);
			if ( (numClocks == 16) && 
			     (TVGAchipset != TVGA9000) &&
			     (TVGAchipset != TVGA9000i) )
			{
				/* 
				 * Go to Old Mode for CS3.
			 	 */
				outw(0x3C4, 0x000B);	/* Switch to Old Mode */
				outb(0x3C4, 0x0E);
				temp = inb(0x3C5) & 0xEF;
				temp |= (no & 0x08) << 1;
				outb(0x3C5, temp);
			}
		}
	}
	return(TRUE);
}

/* 
 * TVGA8900Probe --
 * 	check up whether a Trident based board is installed
 */
static Bool
TVGA8900Probe()
{
  	unsigned char temp;
	char *REV, *LCD, *SIZE;
	int i;

#ifdef PC98_TGUI
	OFLG_SET(OPTION_PC98TGUI, &TVGA8900.ChipOptionFlags);
	if( BoardInit() == FALSE )
		return(FALSE);
#endif
	/*
         * Set up I/O ports to be used by this card
	 */
	xf86ClearIOPortList(vga256InfoRec.scrnIndex);
	xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);

  	if (vga256InfoRec.chipset)
    	{
		/*
		 * If chipset from XF86Config matches...
		 */
		if (!StrCaseCmp(vga256InfoRec.chipset, "tvga8900"))
		{
			ErrorF("\ntvga8900 is no longer valid.  Use one of\n");
			ErrorF("the names listed by the -showconfig option\n");
			return(FALSE);
		}
		if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(0)))
			TVGAchipset = TVGA8200LX;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(1)))
			TVGAchipset = TVGA8800CS;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(2)))
			TVGAchipset = TVGA8900B;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(3)))
			TVGAchipset = TVGA8900C;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(4)))
			TVGAchipset = TVGA8900CL;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(5)))
			TVGAchipset = TVGA8900D;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(6)))
			TVGAchipset = TVGA9000;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(7)))
			TVGAchipset = TVGA9000i;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(8)))
			TVGAchipset = TVGA9100B;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(9)))
			TVGAchipset = TVGA9200CXr;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(10)))
			TVGAchipset = TGUI9320LCD;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(11)))
			TVGAchipset = TGUI9400CXi;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(12)))
			TVGAchipset = TGUI9420;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(13)))
			TVGAchipset = TGUI9420DGi;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(14)))
			TVGAchipset = TGUI9430DGi;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(15)))
			TVGAchipset = TGUI9440AGi;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(16)))
			TVGAchipset = TGUI96xx;
		else if (!StrCaseCmp(vga256InfoRec.chipset, TVGA8900Ident(17)))
		{
			TVGAchipset = TGUI96xx;
			IsCyber = TRUE;
		}
		else
			return(FALSE);
		TVGA8900EnterLeave(ENTER);
    	}
  	else
    	{
      		unsigned char origVal, newVal;
      		char *TVGAName;
		char *TreatAs = NULL;
	
      		TVGA8900EnterLeave(ENTER);

      		/* 
       		 * Check first that we have a Trident card.
       		 */
		outb(0x3C4, 0x0B);
		temp = inb(0x3C5);	/* Save old value */
		outb(0x3C4, 0x0B);	/* Switch to Old Mode */
		outb(0x3C5, 0x00);
		inb(0x3C5);		/* Now to New Mode */
      		outb(0x3C4, 0x0E);
      		origVal = inb(0x3C5);
      		outb(0x3C5, 0x00);
      		newVal = inb(0x3C5) & 0x0F;
      		outb(0x3C5, (origVal ^ 0x02));

		/* 
		 * Is it a Trident card ?? 
		 */
      		if (newVal != 2)
		{
			/*
			 * Nope, so quit
			 */
			outb(0x3C4, 0x0B);	/* Restore value of 0x0B */
			outb(0x3C5, temp);
			outb(0x3C4, 0x0E);
			outb(0x3C5, origVal);
	  		TVGA8900EnterLeave(LEAVE);
	  		return(FALSE);
		}

      		/* 
		 * We've found a Trident card, now check the version.
		 */
		TVGAchipset = -1;
      		outb(0x3C4, 0x0B);
		outb(0x3C5, 0x00);
      		temp = inb(0x3C5);
      		switch (temp)
      		{
		case 0x01:
			TVGAName = "TVGA8800BR";
			break;
      		case 0x02:
			TVGAchipset = TVGA8800CS;
      			TVGAName = "TVGA8800CS";
      			break;
      		case 0x03:
			TVGAchipset = TVGA8900B;
      			TVGAName = "TVGA8900B";
      			break;
      		case 0x04:
      		case 0x13:
			TVGAchipset = TVGA8900C;
      			TVGAName = "TVGA8900C";
      			break;
      		case 0x23:
			TVGAchipset = TVGA9000;
      			TVGAName = "TVGA9000";
      			break;
      		case 0x33:
			TVGAchipset = TVGA8900CL;
      			TVGAName = "TVGA8900CL/D";
      			break;
		case 0x43:
			TVGAchipset = TVGA9000i;
			TVGAName = "TVGA9000i";
			break;
		case 0x53:
			TVGAchipset = TVGA9200CXr;
			TVGAName = "TVGA9200CXr";
			break;
		case 0x63:
			TVGAchipset = TVGA9100B;
			TVGAName = "TVGA9100B";
			break;
		case 0x73:
			TVGAchipset = TGUI9420;
			TVGAName = "TGUI9420";
			break;
		case 0xC3:
			TVGAchipset = TGUI9420DGi;
			TVGAName = "TGUI9420DGi";		
			break;
		case 0x83:
			TVGAchipset = TVGA8200LX;
			TVGAName = "TVGA8200LX";
			break;
		case 0x93:
			TVGAchipset = TGUI9400CXi;
			TVGAName = "TGUI9400CXi";
			break;
		case 0xA3:
			TVGAchipset = TGUI9320LCD;
			TVGAName = "Cyber9320";
			break;
		case 0xD3:
			TVGAchipset = TGUI96xx;		
			TVGAName = "TGUI96xx";
			break;
		case 0xE3:
			TVGAchipset = TGUI9440AGi;
			TVGAName = "TGUI9440AGi";
			break;
		case 0xF3:
			TVGAchipset = TGUI9430DGi;
			TVGAName = "TGUI9430DGi";
			break;
      		default:
      			TVGAName = "UNKNOWN";
      		}
      		ErrorF("%s Trident chipset version: 0x%02x (%s)\n", 
                       XCONFIG_PROBED, temp, TVGAName);
		if (TreatAs != (char *)NULL)
		{
			ErrorF("%s \tDriver will treat chipset as: %s\n",
			       XCONFIG_PROBED, TreatAs);
		}
		if (TVGAchipset == -1)
		{
			if (temp == 0x01)
			{
				ErrorF("Cannot support 8800BR, sorry\n");
			}
			else
			{
				ErrorF("Unknown Trident chipset.\n");
			}
	  		TVGA8900EnterLeave(LEAVE);
	  		return(FALSE);
		}
    	}

	/* Enable Trident enhancements according to chipset */
	outb(0x3C4, 0x09); temp = inb(0x3C5); /* Get Revision ID */

     	switch (TVGAchipset)
      	{
      	case TVGA8900D:
		tridentLinearOK = TRUE;
		tridentDACtype = TKD8001;
		TVGA8900.ChipHas16bpp = TRUE;	/* Has HiColor DAC */
      		break;
	case TVGA9200CXr:
		tridentIsTGUI = FALSE;			/* Not a TGUI */
		tridentTGUIProgrammableClocks = FALSE;
		tridentLinearOK = TRUE;
		tridentDACtype = TKD8001;
		TVGA8900.ChipHas16bpp = TRUE;
		break;
	case TGUI9320LCD:
		tridentIsTGUI = TRUE;		/* Reports of this works */
		tridentLinearOK = TRUE;
		tridentDACtype = TGUIDAC;
#ifndef MONOVGA
		TVGA8900.ChipUse2Banks = TRUE;
#endif
		tridentTGUIProgrammableClocks = TRUE;
#if 0
		tridentHasAcceleration = TRUE;	/* NEEDS CHECKING ! */
#endif
		break;
	case TGUI9400CXi:
		tridentIsTGUI = TRUE;	
		tridentLinearOK = TRUE;
		tridentTGUIProgrammableClocks = FALSE;	/* Not programmable */
		tridentDACtype = TKD8001;
		TVGA8900.ChipHas16bpp = TRUE;
#ifndef MONOVGA
		TVGA8900.ChipUse2Banks = TRUE;
#endif
		break;
	case TGUI9430DGi:
		tridentHWCursorType = 2;		/* HW cursor */
		/* Fall through to 9420 - it is the same apart from cursor */
	case TGUI9420:					/* CHECK ME ! */
		TVGAchipset = TGUI9420DGi;
	case TGUI9420DGi:
		tridentIsTGUI = TRUE;			
		tridentLinearOK = TRUE;
		tridentTGUIProgrammableClocks = FALSE;	/* Not programmable */
		tridentDACtype = TKD8001;
#if 0	/* Disabled for 3.3 */
		tridentHasAcceleration = TRUE;
#endif
		TVGA8900.ChipHas16bpp = TRUE;
#ifndef MONOVGA
		TVGA8900.ChipUse2Banks = TRUE;
#endif
		break;
	case TGUI9440AGi:
		tridentIsTGUI = TRUE;
		tridentTGUIProgrammableClocks = TRUE;
		tridentLinearOK = TRUE;
		tridentHWCursorType = 1;
		tridentDACtype = TGUIDAC;
		if (vgaBitsPerPixel == 24)
			tridentHasAcceleration = FALSE;
		else
			tridentHasAcceleration = TRUE;
		TVGA8900.ChipHas16bpp = TRUE;
		TVGA8900.ChipHas24bpp = TRUE;
#ifndef MONOVGA
		TVGA8900.ChipUse2Banks = TRUE;
#endif
		break;
	case TGUI96xx:
		/* We've found a 96xx graphics engine */
		/* Let's probe furthur */
		switch (temp) {
			case 0x00:
				REV = "9660";
				break;
			case 0x01:
				REV = "9680";
				break;
			case 0x10:
				REV = "ProVidia 9682";
				break;
			case 0x21:
				REV = "ProVidia 9685";
				break;
			case 0x30:
			case 0x33: /* Guessing */
			case 0x34:
			case 0xB3:
				REV = "9385";
				IsCyber = TRUE;
				break;
			case 0x38:
				REV = "9385-1";
				IsCyber = TRUE;
				break;
			case 0x40:
			case 0x42: /* Guessing */
				REV = "9382";
				IsCyber = TRUE;
				break;
			case 0x50:
				REV = "ProVidia 9692";
				break;
			default:
				REV = "Unknown ID - Please report to trident@xfree86.org";
				break;
		}
		ErrorF("%s %s: Detected a Trident %s.\n",
			XCONFIG_PROBED, vga256InfoRec.name, REV);
		tridentIsTGUI = TRUE;
		tridentTGUIProgrammableClocks = TRUE;
		tridentLinearOK = TRUE;
		tridentHWCursorType = 1;
		tridentDACtype = TGUIDAC;
		tridentHasAcceleration = TRUE;
		TVGA8900.ChipHas16bpp = TRUE;
		TVGA8900.ChipHas32bpp = TRUE;
#ifndef MONOVGA
		TVGA8900.ChipUse2Banks = TRUE;
#endif
		if (IsCyber)
		{
			/* Allow stretch mode on LCD */
			OFLG_SET(OPTION_LCD_STRETCH, &TVGA8900.ChipOptionFlags);
			/* Allow LCD centering */
			OFLG_SET(OPTION_LCD_CENTER, &TVGA8900.ChipOptionFlags);
			outb(0x3CE, 0x42);
			temp = inb(0x3CF);
			if (temp & 0x80) 
				LCD = "TFT";
			else
			{
				outb(0x3CE, 0x43); temp = inb(0x3CF);
				if (temp & 0x20)
					LCD = "DSTN";
				else
					LCD = "STN";
			}
			outb(0x3CE, 0x52);
			temp = inb(0x3CF);
			switch ((temp & 0x30)>>4) {
				case 1:
					SIZE = " 640x480";
					CyberLCDHeight = 480;
					CyberLCDWidth = 640;
					break;
				case 3:
					SIZE = " 800x600";
					CyberLCDHeight = 600;
					CyberLCDWidth = 800;
					break;
				case 2:
					SIZE = " 1024x768";
					CyberLCDHeight = 768;
					CyberLCDWidth = 1024;
					break;
				case 0:
					SIZE = " 1280x1024";
					CyberLCDHeight = 1024;
					CyberLCDWidth = 1280;
					break;
			}	
			ErrorF("%s %s: Detected an %s %s Display\n",
				XCONFIG_PROBED, vga256InfoRec.name, LCD, SIZE);
		}
		break;
      	}

	ErrorF("%s %s: Revision %d.\n", XCONFIG_PROBED, vga256InfoRec.name,
					temp);

	/* 
	 * Set up 2 bank registers 
	 */

	if (TVGA8900.ChipUse2Banks == TRUE)
	{
		TVGA8900.ChipSetRead = TGUISetRead;
		TVGA8900.ChipSetWrite = TGUISetWrite;
		TVGA8900.ChipSetReadWrite = TGUISetReadWrite;
	}
	
 	/* 
	 * How much Video Ram have we got?
	 */
    	if (!vga256InfoRec.videoRam)
    	{
		unsigned char temp;

		outb(vgaIOBase + 4, 0x1F); 
		temp = inb(vgaIOBase + 5);

		switch (temp & 0x07) 
		{
		case 0: 
		case 4:
			vga256InfoRec.videoRam = 256; 
			break;
		case 1: 
		case 5:	/* New TGUI's don't support less than 1MB */
			if (TVGAchipset == TGUI96xx)
				vga256InfoRec.videoRam = 4096;
			else
				vga256InfoRec.videoRam = 512; 
			break;
		case 2: 
		case 6:
			vga256InfoRec.videoRam = 768; 
			break;
		case 3: 
			vga256InfoRec.videoRam = 1024; 
			break;
		case 7:
			vga256InfoRec.videoRam = 2048;
			break;
		}
     	}

	if ((vga256InfoRec.videoRam < 1024) && (!tridentTGUIProgrammableClocks))
		TVGA8900.ChipRounding = 16;

	if (!tridentTGUIProgrammableClocks)
	{
		if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE,
			&vga256InfoRec.clockOptions))
		{
			TVGA8900EnterLeave(LEAVE);
			FatalError("%s %s: Please Remove ClockChip Entry "
				   "from XF86Config file.\n", XCONFIG_GIVEN,
				   vga256InfoRec.name);
		}
	}

	if (tridentIsTGUI)
	{
		/* Enable extra IO ports for the TGUI */
		xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_TGUI_ExtPorts,
			       TGUI_ExtPorts);
		TVGA8900EnterLeave(LEAVE); /* force update of IO ports */
		TVGA8900EnterLeave(ENTER);
	}

	if (tridentTGUIProgrammableClocks) 
	{
		OFLG_SET(OPTION_TGUI_MCLK_66, &TVGA8900.ChipOptionFlags);
		if (OFLG_ISSET(OPTION_TGUI_MCLK_66, &vga256InfoRec.options))
			ErrorF("%s %s: Forcing MCLK to 66MHz\n", XCONFIG_GIVEN,
				vga256InfoRec.name);

		OFLG_SET(OPTION_NO_PROGRAM_CLOCKS, &TVGA8900.ChipOptionFlags);

		/* Do some sanity checking first ! */
		if (vga256InfoRec.clocks)
		{
			TVGA8900EnterLeave(LEAVE);
			FatalError("%s %s: Please Remove Clocks Line from "
				"your XF86Config file.\n",
				XCONFIG_GIVEN, vga256InfoRec.name);
		}
		if (!OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE,
			&vga256InfoRec.clockOptions))
		{
			ErrorF("%s %s: Using Trident programmable clocks\n",
				XCONFIG_PROBED, vga256InfoRec.name);
			OFLG_SET(CLOCK_OPTION_TRIDENT, 
					&vga256InfoRec.clockOptions);
			OFLG_SET(CLOCK_OPTION_PROGRAMABLE, 
					&vga256InfoRec.clockOptions);
		}
		else
		if (OFLG_ISSET(CLOCK_OPTION_TRIDENT,
			&vga256InfoRec.clockOptions))
		{
			ErrorF("%s %s: Using Trident programmable clocks\n",
				XCONFIG_GIVEN, vga256InfoRec.name);
		}
		else
		{
			ErrorF("%s %s: Ignoring unrecognised ClockChip\n",
		   		XCONFIG_GIVEN, vga256InfoRec.name);
			OFLG_SET(CLOCK_OPTION_TRIDENT,
				 &vga256InfoRec.clockOptions);
			ErrorF("%s %s: Using Trident programmable clocks\n",
		       		XCONFIG_PROBED, vga256InfoRec.name);
		}
		if (OFLG_ISSET(OPTION_NO_PROGRAM_CLOCKS,
			&vga256InfoRec.options))
		{
			ErrorF("%s %s: Turning off programmable clock.\n",
				XCONFIG_GIVEN, vga256InfoRec.name);
			tridentTGUIProgrammableClocks = FALSE;
		}
	}

	/*
	 * If clocks are not specified in XF86Config file, probe for them
	 */
    	if ((!vga256InfoRec.clocks) && (!tridentTGUIProgrammableClocks))
	{
		switch (TVGAchipset)
		{
		case TVGA8800CS:
			numClocks = 4;
			break;
		case TVGA8900B:
		case TVGA8900C:
			if (OFLG_ISSET(OPTION_16CLKS,&vga256InfoRec.options))
				numClocks = 16;
			else
				numClocks = 8;
			break;
		default:
			if (OFLG_ISSET(OPTION_NO_PROGRAM_CLOCKS,
				&vga256InfoRec.options))
			{
				numClocks = 4;
			} 
			else
			{
				numClocks = 16;
			}
			break;
		}
		vgaGetClocks(numClocks, TVGA8900ClockSelect);
	}

	/*
	 * Get to New Mode.
	 */
      	outb(0x3C4, 0x0B);
      	inb(0x3C5);	

	vga256InfoRec.chipset = TVGA8900Ident(TVGAchipset);
	vga256InfoRec.bankedMono = TRUE;

	/* Initialize option flags allowed for this driver */
	if ((TVGAchipset == TVGA8900B) || (TVGAchipset == TVGA8900C))
	{
		OFLG_SET(OPTION_16CLKS, &TVGA8900.ChipOptionFlags);
	}

#ifndef MONOVGA
	/* For 512k in 256 colour, the pixel clock is half the raw clock */
	if ((vga256InfoRec.videoRam < 1024) && (!tridentIsTGUI))
		TVGA8900.ChipClockMulFactor = 2;

	if (tridentLinearOK)
	{
		OFLG_SET(OPTION_LINEAR, &TVGA8900.ChipOptionFlags);
		OFLG_SET(OPTION_NOLINEAR_MODE, &TVGA8900.ChipOptionFlags);
	}

#if 0 /* Disabled for 3.3 */
	if (tridentHWCursorType)
	{
		OFLG_SET(OPTION_HW_CURSOR, &TVGA8900.ChipOptionFlags);
	}
#endif

#ifdef XFreeXDGA
	/* We support Direct Video Access */
	vga256InfoRec.directMode = XF86DGADirectPresent;
#endif

	if (tridentHasAcceleration)
	{
		/* TGUI Accelerator stuff */
		OFLG_SET(OPTION_NOACCEL, &TVGA8900.ChipOptionFlags);
		OFLG_SET(OPTION_NO_MMIO, &TVGA8900.ChipOptionFlags);
		if (TVGAchipset >= TGUI9440AGi)
		    if (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options))
			vgaSetPitchAdjustHook(TGUIPitchAdjust);
#ifdef __alpha__
		OFLG_SET(OPTION_NO_MMIO, &vga256InfoRec.options);
#endif
	}
#endif

	if (vgaPCIInfo && vgaPCIInfo->Vendor == PCI_VENDOR_TRIDENT)
	{
		OFLG_SET(OPTION_TGUI_PCI_READ_ON,
			&TVGA8900.ChipOptionFlags);
		OFLG_SET(OPTION_TGUI_PCI_WRITE_ON,
			&TVGA8900.ChipOptionFlags);
	}

	if ( (OFLG_ISSET(OPTION_NOLINEAR_MODE, &vga256InfoRec.options)) &&
	     (OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options)) )
	{
		TVGA8900EnterLeave(LEAVE);
		FatalError("%s %s: Can't have 'nolinear' and 'linear' "
			   "defined. Remove one !\n", XCONFIG_GIVEN,
			   vga256InfoRec.name);
	}
	
#ifndef PC98_TGUI
	switch (vgaBitsPerPixel) {
		case 8:
			vga256InfoRec.maxClock = 
				tridentClockLimit[TVGAchipset];
			break;
		case 16:
			vga256InfoRec.maxClock = 
				tridentClockLimit16bpp[TVGAchipset];
			if (!tridentTGUIProgrammableClocks)
			{
				TVGA8900.ChipClockMulFactor = 2;
				vga256InfoRec.maxClock *= 2;
			}
			break;
		case 24:
		case 32:
			vga256InfoRec.maxClock = 
				tridentClockLimit32bpp[TVGAchipset];
			if (!tridentTGUIProgrammableClocks)
			{
				TVGA8900.ChipClockMulFactor = 3;
				vga256InfoRec.maxClock *= 3;
			}
			break;
	}
#else
	switch (vgaBitsPerPixel) {
			case 8:
				vga256InfoRec.maxClock = 
					pc98TGUi->Bpp_Clocks[0];
				break;
			case 16:
				vga256InfoRec.maxClock = 
					pc98TGUi->Bpp_Clocks[1];
				break;
			case 24:
			case 32:
				vga256InfoRec.maxClock = 
					pc98TGUi->Bpp_Clocks[3];
				break;
	}
#endif /* PC98_TGUI */

#ifdef DPMSExtension
	if (TVGAchipset >= TGUI9440AGi)
		vga256InfoRec.DPMSSet = TVGA8900DisplayPowerManagementSet;
#endif

    	return(TRUE);
}

/* 
 * TVGA8900DisplayPowerManagementSet --
 * 
 * Sets VESA Display Power Management Signalling (DPMS) Mode.
 */
#ifdef DPMSExtension
static void TVGA8900DisplayPowerManagementSet(PowerManagementMode)
int PowerManagementMode;
{
	unsigned char Cont;
	if (!xf86VTSema) return;
	outb(0x3CE, 0x23); /* Read DPMS Control */
	Cont = inb(0x3CF) & 0xFC;
	switch (PowerManagementMode)
	{
	case DPMSModeOn:
		/* Screen: On, HSync: On, VSync: On */
		Cont |= 0x00;
		break;
	case DPMSModeStandby:
		/* Screen: Off, HSync: Off, VSync: On */
		Cont |= 0x01;
		break;
	case DPMSModeSuspend:
		/* Screen: Off, HSync: On, VSync: Off */
		Cont |= 0x02;
		break;
	case DPMSModeOff:
		/* Screen: Off, HSync: Off, VSync: Off */
		Cont |= 0x03;
		break;
	}
	outb(0x3CF, Cont);
}
#endif

/*
 * TVGA8900FbInit --
 *	enable speedups for the chips that support it
 */
static void
TVGA8900FbInit()
{
	int offscreen_available;
	unsigned char temp;
	tridentUseLinear = FALSE;

#ifndef MONOVGA
	/* If not a TGUI then chipsets don't have any speedups - so exit */
	/* Exception is the 8900CL/D and the 9200CXr as they have linear */
	if (!tridentIsTGUI)
		if ( (TVGAchipset != TVGA8900CL) && 
		     (TVGAchipset != TVGA8900D)  &&
		     (TVGAchipset != TVGA9200CXr) )
			return;

	TVGA8900.ChipLinearSize = vga256InfoRec.videoRam * 1024;

	if (vgaPCIInfo && vgaPCIInfo->Vendor == PCI_VENDOR_TRIDENT)
	{
		if (vgaPCIInfo->MemBase != 0) {
		  TVGA8900.ChipLinearBase = vgaPCIInfo->MemBase;
		  tridentUseLinear = TRUE;
		} else {
		  ErrorF("%s %s: Unable to locate valid FrameBuffer,"
			" Linear Addressing Disabled\n",
		  XCONFIG_PROBED, vga256InfoRec.name);
		  tridentUseLinear = FALSE;
		}
	} 
	else /* VLBus, ISA, EISA */
	{
		/* We disable Linear for boards that aren't PCI */
		/* Unless specifically requested 		*/
		if ( (TVGAchipset == TVGA8900CL) ||
		     (TVGAchipset == TVGA8900D) )
 		{
		  /* This is for the 8900CL/D Linear Buffer */
		  /* 8900CL/D only has 1MB Ram, therefore.. */
		  TVGA8900.ChipLinearBase = (15 * 1024 * 1024);
		}
		else
		{
		  /* set a default of 60MB, must be on 2MB boundary */
		  TVGA8900.ChipLinearBase = (60 * 1024 * 1024);
		}
	}

	if (OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options))
 	 	tridentUseLinear = TRUE;

	/* Use Membase when told to, then align on 2MB boundary */
	if (vga256InfoRec.MemBase != 0)
		TVGA8900.ChipLinearBase = vga256InfoRec.MemBase & 0xFFE00000;

	if (OFLG_ISSET(OPTION_NOLINEAR_MODE, &vga256InfoRec.options))
		tridentUseLinear = FALSE;

	if (!xf86LinearVidMem())
		tridentUseLinear = FALSE;

	if (tridentUseLinear) 
		ErrorF("%s %s: Using Linear Frame Buffer at 0x0%x, Size %dMB\n",
			XCONFIG_PROBED, vga256InfoRec.name,
			TVGA8900.ChipLinearBase, 
			TVGA8900.ChipLinearSize/1048576);

	if (tridentUseLinear)
		TVGA8900.ChipUseLinearAddressing = TRUE;

	TridentDisplayableMemory = vga256InfoRec.displayWidth 
					* vga256InfoRec.virtualY
					* (vgaBitsPerPixel / 8);

	offscreen_available = vga256InfoRec.videoRam * 1024 -
				TridentDisplayableMemory;

#if 0 /* Disabled for 3.3 */
	if (tridentHWCursorType)
	{
	  if (OFLG_ISSET(OPTION_HW_CURSOR, &vga256InfoRec.options))
	  {
		if (offscreen_available < (IsCyber ? 4096 : 1024))
			ErrorF("%s %s: Not enough off-screen video"
				" memory for hw cursor, using sw cursor.\n",
				XCONFIG_PROBED, vga256InfoRec.name);
		else {
			TridentCursorWidth = 32;
			TridentCursorHeight = 32;
			vgaHWCursor.Initialized = TRUE;
			vgaHWCursor.Init = TridentCursorInit;
			vgaHWCursor.Restore = TridentRestoreCursor;
			vgaHWCursor.Warp = TridentWarpCursor;
			vgaHWCursor.QueryBestSize = TridentQueryBestSize;
			ErrorF("%s %s: Using hardware cursor\n",
				XCONFIG_GIVEN, vga256InfoRec.name);
		}
	  }
	  else
	  {
		ErrorF("%s %s: Using software cursor\n", XCONFIG_PROBED,
							vga256InfoRec.name);
	  }
	}
#endif

	if (tridentHasAcceleration)
	{
	   if (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options))
	   {
		if (TVGAchipset >= TGUI9440AGi)
		{
		   if (!OFLG_ISSET(OPTION_NO_MMIO, &vga256InfoRec.options))
			TGUIAccelInitMMIO();
		   else
			TGUIAccelInit();
		}
		else
		{
		   /* Should only get here for 9420/9430 boards */
		   if (!OFLG_ISSET(OPTION_NO_MMIO, &vga256InfoRec.options))
			OLDTGUIAccelInitMMIO();
		   else
			OLDTGUIAccelInit();
		}
		ErrorF("%s %s: Using Graphics Engine.\n",
			XCONFIG_PROBED, vga256InfoRec.name);
		ErrorF("%s %s: Using %d byte display width.\n",
			XCONFIG_PROBED, vga256InfoRec.name,
			vga256InfoRec.displayWidth);
	   }
	   else
	   {
		ErrorF("%s %s: Disabled Graphics Engine.\n",
			XCONFIG_GIVEN, vga256InfoRec.name);
	   }
	}
#endif /* MONOVGA */
}

/*
 * TVGA8900EnterLeave --
 * 	enable/disable io-mapping
 */
static void
TVGA8900EnterLeave(enter)
	Bool enter;
{
  	unsigned char temp;

#ifndef MONOVGA
#ifdef XFreeXDGA
	if (vga256InfoRec.directMode & XF86DGADirectGraphics && !enter)
	{
		if (tridentHWCursorType)
			TridentHideCursor();
		return;
	}
#endif
#endif
  	if (enter)
    	{
      		xf86EnableIOPorts(vga256InfoRec.scrnIndex);
		vgaIOBase = (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;
      		outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);
      		outb(vgaIOBase + 5, temp & 0x7F);
#ifdef PC98_TGUI
		pc98TGUi->crtsw(1);
#endif
    	}
  	else
    	{
#ifdef PC98_TGUI
		pc98TGUi->crtsw(0);
#endif
      		xf86DisableIOPorts(vga256InfoRec.scrnIndex);
    	}
}

/*
 * TVGA8900Restore --
 *      restore a video mode
 */
static void
TVGA8900Restore(restore)
     	vgaTVGA8900Ptr restore;
{
	unsigned char temp, tmp;
	int i;

	vgaProtect(TRUE);

	/*
	 * Go to Old Mode.
	 */
	outw(0x3C4, 0x000B);
	
	/*
	 * Restore Old Mode Control Registers 1 & 2.
	 * Not needed for TGUI - we have a programmable clock.
	 */
	if (!tridentTGUIProgrammableClocks) 
	{
		if ( (numClocks == 16) &&
		     (TVGAchipset != TVGA9000) &&
		     (TVGAchipset != TVGA9000i) )
		{
			if (restore->std.NoClock >= 0)
			{
				outb(0x3C4, 0x0E);
				temp = inb(0x3C5) & 0xEF;
				outb(0x3C5, temp | (restore->OldMode1 & 0x10));
			}
		}
		outw(0x3C4, ((restore->OldMode2) << 8) | 0x0D);
	}

	/*
	 * Now go to New Mode
	 */
	outb(0x3C4, 0x0B);
	inb(0x3C5);

	/*
	 * Unlock Configuration Port Register, then restore:
	 *	Configuration Port Register 1
	 *	New Mode Control Register 2
	 *	New Mode Control Register 1
	 *	CRTC Module Testing Register
	 * 
	 * 	For the TGUI's, Set the Linear Address, 16/32Bpp etc.
	 */
	tmp = 0x82;
	if ((tridentIsTGUI) && (TVGAchipset >= TGUI9440AGi)) tmp |= 0x40;
	outw(0x3C4, (tmp << 8) | 0x0E);

	if (TVGAchipset < TGUI9440AGi) {
		outb(0x3C4, 0x0C);
		temp = inb(0x3C5) & 0xDF;
		outb(0x3C5, temp | (restore->ConfPort & 0x20));
        	if (restore->std.NoClock >= 0)
			outw(0x3C4, ((restore->NewMode2) << 8) | 0x0D);
	}

	outw(vgaIOBase + 4, ((restore->CRTCModuleTest) << 8) | 0x1E);

	if (tridentUseLinear)
		outw(vgaIOBase + 4, ((restore->LinearAddReg) << 8) | 0x21);

	if (tridentHWCursorType == 1)
	{
		outw(vgaIOBase + 4, ((restore->CurConReg) << 8) | 0x50);
		for (i=0;i<16;i++)
			outw(vgaIOBase + 4, 
				((restore->CursorRegs[i]) << 8) | 0x40 | i);
	}

#ifndef MONOVGA
	if ( (tridentDACtype == TKD8001) && (TVGAchipset != TVGA8900D) )
			outb(0x3C7, restore->TRDReg); 
#endif

	if (TVGAchipset < TGUI96xx)
		vgaHWRestore((vgaHWPtr)restore);

#ifndef MONOVGA
	/* Do the DAC */
	if (tridentDACtype != -1)
	{
		inb(0x3C8);
		inb(0x3C6); inb(0x3C6); inb(0x3C6); inb(0x3C6);
		outb(0x3C6, restore->CommandReg);
		inb(0x3C8);
	}
#endif

	if (tridentTGUIProgrammableClocks)
	{
		if (OFLG_ISSET(OPTION_TGUI_MCLK_66, &vga256InfoRec.options))
		{
			outb(0x43C6, restore->MCLK_A);
			outb(0x43C7, restore->MCLK_B);
		}
		/* UGLY HACK for the 9320lcd chip 
		 * This shouldn't be needed and if it isn't specified
		 * then the programmable clock isn't set properly, but
		 * mode switching is now screwed too. FIX ME!
		 */
		if (TVGAchipset != TGUI9320LCD)
			outb(0x3C2, restore->VCLK_O);
		outb(0x43C8, restore->VCLK_A);
		outb(0x43C9, restore->VCLK_B);
	}

	outw(vgaIOBase + 4, ((restore->CRTHiOrd) << 8) | 0x27);
	outw(vgaIOBase + 4, ((restore->AddColReg) << 8) | 0x29);

	if (tridentIsTGUI)
	{
		if (IsCyber) 
		{
			outw(0x3CE, ((restore->CyberCont) << 8) | 0x30);
			outw(0x3CE, ((restore->CyberVExp) << 8) | 0x52);
			outw(0x3CE, ((restore->CyberHExp) << 8) | 0x53);
			outw(0x3CE, ((restore->CyberEnhance) << 8) | 0x31);
		}
#ifndef MONOVGA

		outw(0x3CE, ((restore->MiscExtFunc) << 8) | 0x0F);
#endif

		outw(vgaIOBase + 4, ((restore->VLBusReg) << 8) | 0x2A);
		
#ifndef MONOVGA
		if (TVGAchipset >= TGUI9440AGi)
		{
			outw(vgaIOBase + 4, ((restore->GraphEngReg) << 8) | 0x36);
			if (TVGAchipset >= TGUI96xx)
				outw(vgaIOBase + 4, ((restore->Performance)<<8)
						| 0x2F);
			outw(vgaIOBase + 4, ((restore->PixelBusReg) << 8) | 0x38);
			outw(0x3CE, ((restore->MiscIntContReg) << 8) | 0x2F);
		}
		else
		{
			outw(vgaIOBase + 4, ((restore->DRAMTiming)<<8)| 0x23);
			outw(vgaIOBase + 4, ((restore->FIFOControl)<<8)| 0x20);
			if (tridentHasAcceleration)
			{
			  /* This is only for 9420/9430 boards */
			  outb(GER_INDEX, OLDGER_STATUS);
			  outb(GER_BYTE0, restore->GraphEngReg); 
			  if (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options))			{
				if (!OFLG_ISSET(OPTION_NO_MMIO, 
						&vga256InfoRec.options))
				{
					outb(GER_INDEX, MMIOBASE);
					outl(GER_BYTE0, 0xBF000);
				}
			  }
			}
		}
#endif
	}

	if (vgaPCIInfo && vgaPCIInfo->Vendor == PCI_VENDOR_TRIDENT)
		outw(vgaIOBase + 4, ((restore->PCIReg) << 8) | 0x39);

	outw(0x3C4, ((restore->NewMode1 ^ 0x02) << 8) | 0x0E);

	if (TVGAchipset >= TGUI96xx) 
		vgaHWRestore((vgaHWPtr)restore);

	vgaProtect(FALSE);
}

/*
 * TVGA8900Save --
 *      save the current video mode
 */
static void *
TVGA8900Save(save)
     	vgaTVGA8900Ptr save;
{
	unsigned char temp, tmp;
	int i;

	/*
	 * Get current bank
	 */
	outb(0x3C4, 0x0e); temp = inb(0x3C5);

	/*
	 * Save generic VGA registers
	 */

  	save = (vgaTVGA8900Ptr)vgaHWSave((vgaHWPtr)save, sizeof(vgaTVGA8900Rec));

	/*
	 * Go to Old Mode.
	 */
	outw(0x3C4, 0x000B);

	/*
	 * Save Old Mode Control Registers 1 & 2.
	 * Not needed for TGUI - we have a programmable clock.
	 */
	if (!tridentTGUIProgrammableClocks)
	{
		if ( (numClocks == 16) &&
		     (TVGAchipset != TVGA9000) &&
		     (TVGAchipset != TVGA9000i) )
		{
			outb(0x3C4, 0x0E); save->OldMode1 = inb(0x3C5); 
		}
		outb(0x3C4, 0x0D); save->OldMode2 = inb(0x3C5); 
	}
	
	/*
	 * Now go to New Mode
	 */
	outb(0x3C4, 0x0B);
	inb(0x3C5);

	/*
	 * Unlock Configuration Port Register, then save:
	 *	Configuration Port Register 1
	 *	New Mode Control Register 2
	 *	New Mode Control Register 1
	 *	CRTC Module Testing Register
	 */
	tmp = 0x82;
	if ((tridentIsTGUI) && (TVGAchipset >= TGUI9440AGi)) tmp |= 0x40;
	outw(0x3C4, (tmp << 8) | 0x0E);

	if (tridentUseLinear)
	{
		outb(vgaIOBase + 4, 0x21); 
		save->LinearAddReg = inb(vgaIOBase + 5);
	}

	if (TVGAchipset < TGUI9440AGi)
	{
		outb(0x3C4, 0x0C); save->ConfPort = inb(0x3C5);
		outb(0x3C4, 0x0D); save->NewMode2 = inb(0x3C5);
	}

	if (tridentHWCursorType == 1)
	{
		outb(vgaIOBase + 4, 0x50); save->CurConReg = inb(vgaIOBase + 5);
		for (i=0;i<16;i++)
		{
			outb(vgaIOBase + 4, 0x40 | i);
			save->CursorRegs[i] = inb(vgaIOBase + 5);
		}
	}

	outb(vgaIOBase + 4, 0x1E); save->CRTCModuleTest = inb(vgaIOBase + 5);
	save->NewMode1 = temp;

	/* Do the DAC */
	if (tridentDACtype != -1)
	{
		inb(0x3C8);
		inb(0x3C6); inb(0x3C6); inb(0x3C6); inb(0x3C6);
		save->CommandReg = inb(0x3C6);
		inb(0x3C8);
	}

	outb(vgaIOBase + 4, 0x27); save->CRTHiOrd = inb(vgaIOBase + 5);
	outb(vgaIOBase + 4, 0x29); save->AddColReg = inb(vgaIOBase + 5);

	if (tridentIsTGUI)
	{
		if (IsCyber) 
		{
			outb(0x3CE, 0x31);
			save->CyberEnhance = inb(0x3CF);
			outb(0x3CE, 0x30);
			save->CyberCont = inb(0x3CF);
			outb(0x3CE, 0x52);
			save->CyberVExp = inb(0x3CF);
			outb(0x3CE, 0x53);
			save->CyberHExp = inb(0x3CF);
		}
#ifndef MONOVGA
		outb(0x3CE, 0x0F); save->MiscExtFunc = inb(0x3CF);
#endif
		outb(vgaIOBase + 4, 0x2A); save->VLBusReg = inb(vgaIOBase + 5);

		if (tridentTGUIProgrammableClocks)
		{
			save->VCLK_O = inb(0x3CC);
			save->VCLK_A = inb(0x43C8);
			save->VCLK_B = inb(0x43C9);
			save->MCLK_A = inb(0x43C6);
			save->MCLK_B = inb(0x43C7);
		}

#ifndef MONOVGA
		if (TVGAchipset >= TGUI9440AGi)
		{
			outb(vgaIOBase + 4, 0x36); 
			save->GraphEngReg = inb(vgaIOBase + 5);
			outb(vgaIOBase + 4, 0x38); 
			save->PixelBusReg = inb(vgaIOBase + 5);
			outb(0x3CE, 0x2F);
			save->MiscIntContReg = inb(0x3CF);
			if (TVGAchipset >= TGUI96xx) 
			{
				outb(vgaIOBase + 4, 0x2F);
				save->Performance = inb(vgaIOBase + 5);
			}
		}
		else
		{
			outb(vgaIOBase + 4, 0x23);
			save->DRAMTiming = inb(vgaIOBase + 5);
			outb(vgaIOBase + 4, 0x20);
			save->FIFOControl = inb(vgaIOBase + 5);
		}
#endif
	}

	if (vgaPCIInfo && vgaPCIInfo->Vendor == PCI_VENDOR_TRIDENT)
	{
		outb(vgaIOBase + 4, 0x39);
		save->PCIReg = inb(vgaIOBase + 5);
	}

#ifndef MONOVGA
	if ( (tridentDACtype == TKD8001) && (TVGAchipset != TVGA8900D) )
			save->TRDReg = inb(0x3C7); 
#endif

  	return ((void *) save);
}

/*
 * TVGA8900Init --
 *      Handle the initialization, etc. of a screen.
 */
static Bool
TVGA8900Init(mode)
    	DisplayModePtr mode;
{
	unsigned char temp;
	int i;
	int offset;

#ifndef MONOVGA
	/*
	 * In 256-color mode, with less than 1M memory, the horizontal
	 * timings and the dot-clock must be doubled.  We can (and
	 * should) do the former here.  The latter must, unfortunately,
	 * be handled by the user in the XF86Config file.
	 */
	if (vga256InfoRec.videoRam < 1024)
	{
		/*
		 * Double horizontal timings.
		 */
		if (!mode->CrtcHAdjusted)
		{
			mode->CrtcHTotal <<= 1;
			mode->CrtcHDisplay <<= 1;
			mode->CrtcHSyncStart <<= 1;
			mode->CrtcHSyncEnd <<= 1;
			mode->CrtcHSkew <<= 1;
			mode->CrtcHAdjusted = TRUE;
		}
		/*
		 * Initialize generic VGA registers.
		 */
		if (!vgaHWInit(mode, sizeof(vgaTVGA8900Rec)))
			return(FALSE);
  
		/*
		 * Now do Trident-specific stuff.  This one is also
		 * affected by the x2 requirement.
		 */
		offset = vga256InfoRec.displayWidth >> 
			(mode->Flags & V_INTERLACE ? 2 : 3);
	} else {
#endif
		/*
		 * Initialize generic VGA Registers
		 */
		if (!vgaHWInit(mode, sizeof(vgaTVGA8900Rec)))
			return(FALSE);

		/*
		 * Now do Trident-specific stuff.
		 */
		offset = vga256InfoRec.displayWidth >>
				(mode->Flags & V_INTERLACE ? 3 : 4);

#ifndef MONOVGA
		switch (tridentDACtype)
		{
		case TGUIDAC:
			if (vgaBitsPerPixel == 8)
			offset = vga256InfoRec.displayWidth >> 3;
			if (vgaBitsPerPixel == 16)
			offset = vga256InfoRec.displayWidth >> 2;
			if (vgaBitsPerPixel == 24)
			offset = (vga256InfoRec.displayWidth * 3) >> 3;
			if (vgaBitsPerPixel == 32)
			offset = vga256InfoRec.displayWidth >> 1;
			break;
		case TKD8001:
			if (vgaBitsPerPixel == 16)
			offset = vga256InfoRec.displayWidth >> 3;
			if (vgaBitsPerPixel == 32)
			offset = vga256InfoRec.displayWidth >> 2;
			break;
		}
	}

	new->std.CRTC[20] = 0x40;
	if (TVGAchipset < TGUI9440AGi)
	{
		new->std.CRTC[23] = 0xA3;
		if (vga256InfoRec.videoRam > 512)
			new->OldMode2 = 0x10;
		else
			new->OldMode2 = 0x00;
	}
	new->NewMode1 = 0x02;
#endif
	if (TVGAchipset < TGUI9440AGi)
	{
		if (vga256InfoRec.videoRam > 512)
			new->ConfPort = 0x20;
		else
			new->ConfPort = 0x00;
	}

	new->CRTCModuleTest = (mode->Flags & V_INTERLACE ? 0x84 : 0x80); 

	if (tridentUseLinear) 
	{
		new->LinearAddReg = 
			  ((TVGA8900.ChipLinearBase >> 24) << 6) |
			  ((TVGA8900.ChipLinearBase >> 20) & 0x0F);
		new->LinearAddReg |= 0x20;	/* Enable Linear */
		if (TVGAchipset < TGUI9440AGi)
			if (TVGA8900.ChipLinearSize == (2048*1024))
				new->LinearAddReg |= 0x10;
	}

	new->std.CRTC[19] = offset & 0xFF;
	outb(vgaIOBase + 4, 0x29);
	new->AddColReg = inb(vgaIOBase + 5) | ((offset & 0x100) >> 4);

	/* Anything less than a 9440 doesn't have 10 CRTC address bits */
	if (TVGAchipset >= TGUI9440AGi)
 		new->CRTHiOrd = ((mode->CrtcVSyncStart & 0x400) >> 4) |
 			(((mode->CrtcVTotal - 2) & 0x400) >> 3) |
 			((mode->CrtcVSyncStart & 0x400) >> 5) |
 			(((mode->CrtcVDisplay - 1) & 0x400) >> 6) |
 			0x08;
#ifndef MONOVGA
	if (tridentIsTGUI)
	{
		/* Turn on 32 bit mode - applies to VLBus and PCI */
		outb(vgaIOBase + 4, 0x2A);
		new->VLBusReg = inb(vgaIOBase + 5) | 0x40; /* 32bit mode */
		outb(0x3CE, 0x0F);
		new->MiscExtFunc = inb(0x3CF) | 0x07; /* Set Dual Banks */
	}
	new->CommandReg = 0x00;		/* DAC Standard colourmap */

#if 0 /* Disabled for 3.3 */
	if (tridentHWCursorType)
	  if (OFLG_ISSET(OPTION_HW_CURSOR, &vga256InfoRec.options))
		new->std.Attribute[17] = 0x00; /* Black overscan */
#endif

	if ( (TVGAchipset == TVGA8900D) ||
	     (TVGAchipset == TVGA9200CXr) ||
	     (TVGAchipset == TGUI9400CXi) ||
	     (TVGAchipset == TGUI9420DGi) ||
	     (TVGAchipset == TGUI9430DGi) )
	{
		if (TVGAchipset != TVGA8900D)
		{
			temp = inb(0x3C7);
			new->TRDReg = temp & 0xBF; 	/* Sierra DAC */
		}
		if (vgaBitsPerPixel == 16)
		{
			new->std.Attribute[17] = 0x00;
			new->CommandReg = 0xE0;
			new->MiscExtFunc |= 0x08;
		}
		if (vgaBitsPerPixel == 32)
		{
			new->std.Attribute[17] = 0x00;
			if (TVGAchipset == TVGA8900D)
				new->CommandReg = 0x42;
			else
				new->CommandReg = 0xC0;
			new->MiscExtFunc |= 0x08;
		}
		if ( (TVGAchipset == TGUI9400CXi) ||
		     (TVGAchipset == TGUI9420DGi) ||
		     (TVGAchipset == TGUI9430DGi) )
		{
			outb(vgaIOBase + 4, 0x23);
			new->DRAMTiming = inb(vgaIOBase + 5) | 0x04;
			outb(vgaIOBase + 4, 0x20);
			new->FIFOControl = inb(vgaIOBase + 5) | 0x01;
		}
		if (tridentHasAcceleration)
		{
			/* Only for 9420/9430 boards */
			new->GraphEngReg = 0x00;
			if (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options))
			{
				if (!OFLG_ISSET(OPTION_NO_MMIO, 
						&vga256InfoRec.options))
				{
					/* Enable MMIO */
					new->GraphEngReg |= 0x10;
#ifdef __alpha__
					tguiMMIOBase = xf86MapVidMemSparse(0,MMIO_REGION, (pointer)0xBF000, 0x1000);
#else
					tguiMMIOBase = xf86MapVidMem(0,MMIO_REGION, (pointer)0xBF000, 0x1000);
#endif
				}
				switch (vgaBitsPerPixel) {
					case 8:
						GE_OP = 0x03;
						break;
					case 16:
						GE_OP = 0x04;
						break;
				}
				/* Enable the GE */
				new->AddColReg |= 0x80;
			}
		}
	}
#endif

	if (vgaPCIInfo && vgaPCIInfo->Vendor == PCI_VENDOR_TRIDENT)
	{
		outb(vgaIOBase + 4, 0x39);
		new->PCIReg = inb(vgaIOBase + 5);
		/* Turn PCI Burst Read and Write OFF - By Default ! */
		/* Otherwise the Graphics Engine flakes out !	    */
		/* But we still allow it to be turned on, if poss.  */
		new->PCIReg &= 0xF9;
		if (OFLG_ISSET(OPTION_TGUI_PCI_READ_ON,
					&vga256InfoRec.options))
			new->PCIReg |= 0x02;
		if (OFLG_ISSET(OPTION_TGUI_PCI_WRITE_ON,
					&vga256InfoRec.options))
			new->PCIReg |= 0x04;
	}

	if (TVGAchipset >= TGUI9440AGi)
	{
		if (IsCyber) 
		{
			outb(0x3CE, 0x31);
			new->CyberEnhance = inb(0x3CF) & 0x8F;
			if (mode->CrtcVDisplay > 768)
				new->CyberEnhance |= 0x30;
			else
			if (mode->CrtcVDisplay > 600)
				new->CyberEnhance |= 0x20;
			else
			if (mode->CrtcVDisplay > 480)
				new->CyberEnhance |= 0x10;
			outb(0x3CE, 0x30);
			new->CyberCont = inb(0x3CF) & 0xEE;
			outb(0x3CE, 0x52);
			new->CyberVExp = inb(0x3CF);
			outb(0x3CE, 0x53);
			new->CyberHExp = inb(0x3CF);
			/* turn off stretch: */
			new->CyberVExp &= 0xFE; 
			new->CyberHExp &= 0xFE;
			if (!OFLG_ISSET(OPTION_LCD_STRETCH, &vga256InfoRec.options))
			{
				/* Stretch Width */
				if (mode->CrtcHDisplay < CyberLCDWidth)
					new->CyberVExp |= 0x01;
				/* Stretch Height */
				if (mode->CrtcVDisplay < CyberLCDHeight)
					new->CyberHExp |= 0x01;
			}
			if (OFLG_ISSET(OPTION_LCD_CENTER, &vga256InfoRec.options))
			{
				/* Center */
				new->CyberVExp |= 0x80;
				new->CyberHExp |= 0x80;
			}
			else
			{
				/* Don't Center */
				new->CyberVExp &= 0x7F;
				new->CyberHExp &= 0x7F;
			}
		}

		if (TVGAchipset >= TGUI96xx)
		{
			new->AddColReg |= ((offset & 0x200) >> 4);
			outb(vgaIOBase + 4, 0x2F);
#ifndef PC98_TGUI
			new->Performance = inb(vgaIOBase + 5) | 0x10;
#else
			new->Performance = inb(vgaIOBase + 5) & ~0x10;
			/* disable 12depth FIFO */
#endif
		}

		if (vgaBitsPerPixel >= 8) {
		if ((!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options)) &&
			tridentHasAcceleration)
		{
			if (!OFLG_ISSET(OPTION_NO_MMIO, &vga256InfoRec.options))
			{
#ifdef PC98_TGUI
				new->GraphEngReg = 0x83; /* Enable MMIO, GER */
				tguiMMIOBase = xf86MapVidMem(0,MMIO_REGION, (pointer)pc98TGUi->mmioBase, 0x1000);
#else
				new->GraphEngReg = 0x82; /* Enable MMIO, GER */
#ifdef __alpha__
				tguiMMIOBase = xf86MapVidMemSparse(0,MMIO_REGION, (pointer)0xBF000, 0x1000);
#else
				tguiMMIOBase = xf86MapVidMem(0,MMIO_REGION, (pointer)0xBF000, 0x1000);
#endif
				tguiMMIOBase += 0xF00;
#endif /* PC98_TGUI */
			}
			else
			{
#ifdef PC98_TGUI
				new->GraphEngReg = 0x83; /* Enable MMIO, GER */
#else
				new->GraphEngReg = 0x80; /* Enable 0x21XX, GER */
#endif /* PC98_TGUI */
			      }
			GE_OP = 0x0000;		/* Use XY */
			switch (vga256InfoRec.displayWidth * vgaBitsPerPixel / 8) {
				case 512:
					GE_OP |= 0x00;
					break;
				case 1024:
					GE_OP |= 0x04;
					break;
				case 2048:
					GE_OP |= 0x08;
					break;
				case 4096:
					GE_OP |= 0x0C;
					break;
			}
		}
		outb(0x3CE, 0x2F);
		new->MiscIntContReg = inb(0x3CF) | 0x04; /* double line width */
		new->PixelBusReg = 0x00;
		if (vgaBitsPerPixel == 16)
		{
			new->std.Attribute[17] = 0x00;
			if (TVGAchipset <= TGUI9440AGi)
				new->MiscExtFunc |= 0x08; /* Clock Div. by 2 */
			new->CommandReg = 0x30;	 /* 16bpp */
			new->PixelBusReg |= 0x04;
			if (TVGAchipset >= TGUI96xx)
				new->PixelBusReg |= 0x01; /* 16bit bus */
			GE_OP |= 0x01; /* 16bpp in GE */
		}
		if (vgaBitsPerPixel == 24)
		{
			new->std.Attribute[17] = 0x00;
			new->CommandReg = 0xD0; /* 24bpp */
			new->MiscExtFunc |= 0x40; /* Clock Division by 3 */
			new->PixelBusReg |= 0x08;
			GE_OP |= 0x03; /* 24bpp in GE */
		}
		if (vgaBitsPerPixel == 32)
		{
			new->std.Attribute[17] = 0x00;
			new->CommandReg = 0xD0; /* 32bpp */
			new->MiscExtFunc |= 0x08; /* Clock Division by 2 */
			new->PixelBusReg |= 0x09; /* 16bit bus */
			GE_OP |= 0x02; /* 32bpp in GE */
		}
		}
	}

	if (new->std.NoClock >= 0)
	{
		if (tridentTGUIProgrammableClocks)
		{
			TGUISetClock(new->std.NoClock);
			if (OFLG_ISSET(OPTION_TGUI_MCLK_66, &vga256InfoRec.options))
			{
				if (IsCyber)
				{
					new->MCLK_A = 0xBD;
					new->MCLK_B = 0x58;
				}
				else
				{
					new->MCLK_A = 0x8F;
					new->MCLK_B = 0x00;
				}
			}
		}
		else
		{
  			new->NewMode2 = (new->std.NoClock & 0x04) >> 2;
			if ( (TVGAchipset == TVGA9000) ||
			     (TVGAchipset == TVGA9000i) )
				new->NewMode2 |= (new->std.NoClock & 0x08) << 3;
			else
			if (numClocks == 16)
				new->OldMode1 = (new->std.NoClock & 0x08) << 1;
		}
	}

        return(TRUE);
}

/*
 * TVGA8900Adjust --
 *      adjust the current video frame to display the mousecursor
 */
static void 
TVGA8900Adjust(x, y)
	int x, y;
{
	unsigned char temp;
	int base = y * vga256InfoRec.displayWidth + x;
	int shift = 0;

	if (vgaBitsPerPixel >= 8) {
	   if ((TVGAchipset >= TGUI96xx) && (vgaBitsPerPixel == 8))
		base &= 0xFFFFFFF8;
	   if (vgaBitsPerPixel == 16)
		shift = 1;
	   if (vgaBitsPerPixel == 24)
		base = ((base + 1) & ~0x03) * 3;
	   if (vgaBitsPerPixel == 32)
		shift = 2;
	/* 
	 * Go see the comments in the Init function.
	 */
	   if (tridentIsTGUI)
		base >>= (2 - shift);
	   else
	   {
		if (vga256InfoRec.videoRam < 1024) 
			base = (y * vga256InfoRec.displayWidth + x + 1)
								>> (2 - shift);
		else
			base = (y * vga256InfoRec.displayWidth + x + 3) 
								>> (3 - shift);
	   }
	} else {
	   base = (y * vga256InfoRec.displayWidth + x + 3) >> 3;
	}

  	outw(vgaIOBase + 4, (base & 0x00FF00) | 0x0C);
	outw(vgaIOBase + 4, ((base & 0x00FF) << 8) | 0x0D);

	/* CRT bit 16 */
	outb(vgaIOBase + 4, 0x1E); 
	temp = inb(vgaIOBase + 5) & 0xDF;
	temp |= (base & 0x10000) >> 11;
	outb(vgaIOBase + 5, temp);

	/* CRT bits 17-19 */
	outb(vgaIOBase + 4, 0x27); 
	temp = inb(vgaIOBase + 5) & 0xF8;
	temp |= (base & 0xE0000) >> 17;
	outb(vgaIOBase + 5, temp);

#ifdef XFreeXDGA
	if (vgaBitsPerPixel >= 8) {
	if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
		/* Wait until vertical retrace is in progress. */
		while (inb(vgaIOBase + 0xA) & 0x08);
		while (!(inb(vgaIOBase + 0xA) & 0x08));
	}
	}
#endif
}

/*
 * TVGA8900ValidMode --
 *
 */
static int
TVGA8900ValidMode(mode, verbose,flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
	if (vgaBitsPerPixel < 8) {
	if ( (TVGAchipset == TVGA8900C) ||
	     (TVGAchipset == TVGA8900B) ||
	     (TVGAchipset == TVGA8800CS) ||
	     (TVGAchipset == TVGA9000) ||
	     (TVGAchipset == TVGA9000i) )
	{
		if (mode->HDisplay > 1152)
		{
		    if (verbose)
			ErrorF("%s %s: Chipset supports a max. width"
			       " of 1152, Adjust Modes in XF86Config.\n",
			       XCONFIG_PROBED, vga256InfoRec.name);
		    return(MODE_BAD);
		}
	}
	}

	if (IsCyber)
	{
		if (mode->VDisplay > 1024)
		{
		   if (verbose)
			ErrorF("%s %s: Chipset supports a max. height"
			       "of 1024, Adjust Modes in XF86Conig.\n",
			       XCONFIG_PROBED, vga256InfoRec.name);
		   return(MODE_BAD);
		}
	}

	if ( (tridentDACtype == TKD8001) && (vgaBitsPerPixel > 8) )
	{
		if (mode->Flags & V_INTERLACE)
		{
		    if (verbose)
			ErrorF("%s %s: Chipset does not support Interlaced "
			       "modes at >8bpp\n", XCONFIG_PROBED, 
			       vga256InfoRec.name);
		    return MODE_BAD;
		}
	}
	return MODE_OK;
}

/*
 * TGUIPitchAdjust
 * 
 * This function adjusts the display width (pitch) once the virtual
 * width is known. It returns the display width.
 */
static int
TGUIPitchAdjust()
{
	int pitch = 0;
	int memory;
	int X;

	X = vga256InfoRec.virtualX;

	if (X <= 4096)
		pitch = 4096;
	if (X <= 2048)
		pitch = 2048;
	if (X <= 1024)
		pitch = 1024;
	if (X <= 512)
		pitch = 512;

	memory = ((pitch * vga256InfoRec.virtualY) / 1024) * vgaBitsPerPixel/8;

	if (memory > vga256InfoRec.videoRam)
	{
		TVGA8900EnterLeave(LEAVE);
		FatalError("%s %s: Too Little VideoRam for Accelerator Engine\n"
			   "%s %s: Required resolution with accelerator is %dx%d.\n"
			   "%s %s: Reduce your Virtual X resolution to %d, or use Option \"noaccel\".\n",
			XCONFIG_PROBED, vga256InfoRec.name,
			XCONFIG_PROBED, vga256InfoRec.name,
			pitch, vga256InfoRec.virtualY,
			XCONFIG_PROBED, vga256InfoRec.name,
			pitch/2);
	}

	return pitch;
}
