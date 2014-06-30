/* $XConsortium: sis86c201.c /main/11 1996/10/27 13:24:11 kaleb $ */
/*
 * Copyright 1995 by Alan Hourihane, Wigan, England.
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
 * Author:  Alan Hourihane, alanh@fairlite.demon.co.uk
 *
 * Modified 1996 by Xavier Ducoin <xavier@rd.lectra.fr>
 *
 */
/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/sis/sis86c201.c,v 3.17.2.3 1997/05/09 07:15:48 hohndel Exp $ */

/*#define DEBUG*/
/*#define IO_DEBUG*/

/*#define USE_XAA*/
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

#if !defined(MONOVGA) && !defined(XF86VGA16)
#include "vga256.h"
#include "sis_driver.h"
#else
#define SIS86C201 0
#define SIS86C202 1
#define SIS86C205 2
#endif


#ifndef MONOVGA
extern vgaHWCursorRec vgaHWCursor;
#endif

/* Blitter related */
/*address in video ram of tile/stipple pattern*/
unsigned int sisBLTPatternAddress = -1; 
int sisBLTPatternOffscreenSize = 0 ;
unsigned char *sisBltDataWindow = NULL;
Bool sisAvoidImageBLT = FALSE;

int sisReg32MMIO[]={0x8280,0x8284,0x8288,0x828C,0x8290,0x8294,0x8298,0x829C,
		    0x82A0,0x82A4,0x82A8,0x82AC};
#ifndef MONOVGA
extern GCOps cfb16TEOps1Rect, cfb16TEOps, cfb16NonTEOps1Rect, cfb16NonTEOps;
extern GCOps cfb24TEOps1Rect, cfb24TEOps, cfb24NonTEOps1Rect, cfb24NonTEOps;
#endif

Bool sisHWCursor = FALSE ;

/* Clock related */
typedef struct {
    unsigned char msr;
    unsigned char xr2A;
    unsigned char xr2B;
    int Clock;
} sisClockReg, *sisClockPtr;

typedef struct {
	vgaHWRec std;          		/* std IBM VGA register 	*/
	unsigned char Port_3C4[0x38]; 
	unsigned char ClockReg2;
	sisClockReg   sisClock;
} vgaSISRec, *vgaSISPtr;

/* alias for specific extended registers   */
#define ClockReg	Port_3C4[0x07]
#define DualBanks	Port_3C4[0x0B]
#define BankReg		Port_3C4[0x06]
#define CRTCOff		Port_3C4[0x0A]
#define DispCRT		Port_3C4[0x27]
#define Unknown		Port_3C4[0x08]
#define LinearAddr0	Port_3C4[0x20]
#define LinearAddr1	Port_3C4[0x21]

static Bool SISClockSelect();
static char *SISIdent();
static Bool SISProbe();
static void SISEnterLeave();
static Bool SISInit();
static int  SISValidMode();
static void *SISSave();
static void SISRestore();
static void SISFbInit();
static void SISAdjust();
static void SISDisplayPowerManagementSet();
extern void SISSetRead();
extern void SISSetWrite();
extern void SISSetReadWrite();

extern int SISCursorHotX;
extern int SISCursorHotY;
extern int SISCursorWidth;
extern int SISCursorHeight;

extern Bool SISCursorInit();
extern void SISRestoreCursor();
extern void SISWarpCursor();
extern void SISQueryBestSize();

vgaVideoChipRec SIS = {
  SISProbe,
  SISIdent,
  SISEnterLeave,
  SISInit,
  SISValidMode,
  SISSave,
  SISRestore,
  SISAdjust,
  vgaHWSaveScreen,
  (void (*)())NoopDDA,
  SISFbInit,
  SISSetRead,
  SISSetWrite,
  SISSetReadWrite,
  0x10000,
  0x10000,
  16,
  0xffff,
  0x00000, 0x10000,
  0x00000, 0x10000,
  TRUE,                           
  VGA_DIVIDE_VERT,
  {0,},
  16,				
  FALSE,
  0,
  0,
  TRUE,
  TRUE,
  FALSE,
  NULL,
  1,            /* ClockMulFactor */
  1             /* ClockDivFactor */
};

#define new ((vgaSISPtr)vgaNewVideoState)

int SISchipset;
/* default is enhanced mode (use option to disable)*/
Bool sisUseLinear = TRUE;
Bool sisUseMMIO = TRUE;
Bool sisUseXAAcolorExp = TRUE;
static int SISDisplayableMemory;
unsigned char *sisMMIOBase = NULL;
unsigned int PCIMMIOBase=0 ;

/*
 * SISIdent --
 */
static char *
SISIdent(n)
	int n;
{
	static char *chipsets[] = {"sis86c201", "sis86c202", "sis86c205", };

	if (n + 1 > sizeof(chipsets) / sizeof(char *))
		return(NULL);
	else
		return(chipsets[n]);
}

/*
 * SISClockSelect --
 * 	select one of the possible clocks ...
 */
static Bool
SISClockSelect(no)
	int no;
{
	static unsigned char save1, save2;
	unsigned char temp;
#ifdef DEBUG
    ErrorF("SISClockSelect(%d)\n",no);
#endif

	/*
	 * CS0 and CS1 are in MiscOutReg
	 *
	 * CS2,CS3,CS4 are in 0x3C4 index 7
	 * But - only active when CS0/CS1 are set.
	 */
	switch(no)
	{
	case CLK_REG_SAVE:
		save1 = inb(0x3CC);
		outb(0x3C4, 0x07);
		save2 = inb(0x3C5);
		break;
	case CLK_REG_RESTORE:
		outb(0x3C2, save1);
		outw(0x3C4, (save2 << 8) | 0x07);
		break;
	default:
		/*
		 * Do CS0 and CS1 and set them - makes index 7 valid
		 */
		temp = inb(0x3CC);
		temp &= ~0x0C ;
		if ( no >= 2 ) {
		    outb(0x3C2, temp | 0x0C);
		    outw(0x3C4, (no << 8) | 0x07);
		}
		else /* using vga clock */
		   outb(0x3C2, temp | ( (no<<2) & 0x0C) ); 
		    
	}
	return(TRUE);
}

#define write_xr(num,val) {outb(0x3C4, num);outb(0x3C5, val);}
#define read_xr(num,var) {outb(0x3C4, num);var=inb(0x3C5);}

static Bool
ClockProgramable()
{
#if defined(MONOVGA) || defined(XF86VGA16)
    return FALSE;
#else
    return (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, 
		       &vga256InfoRec.clockOptions)) ;
#endif
}

static void
sisCalcClock(int Clock, unsigned int *vclk)
{
    int M, N, P, PSN, VLD, PSNx;

    int bestM, bestN, bestP, bestPSN, bestVLD;
    double bestError, abest = 42, bestFout;
    double target;

    double Fvco, Fout;
    double error, aerror;



    /*
     *	fd = fref*(Numerator/Denumerator)*(Divider/PostScaler)
     *
     *	M 	= Numerator [1:128] 
     *  N 	= DeNumerator [1:32]
     *  VLD	= Divider (Vco Loop Divider) : divide by 1, 2
     *  P	= Post Scaler : divide by 1, 2, 3, 4
     *  PSN     = Pre Scaler (Reference Divisor Select) 
     * 
     * result in vclk[]
     */
#define Midx 	0
#define Nidx 	1
#define VLDidx 	2
#define Pidx 	3
#define PSNidx 	4
#define Fref 14318180
/* stability constraints for internal VCO -- MAX_VCO also determines 
 * the maximum Video pixel clock */
#define MIN_VCO Fref
#define MAX_VCO 135000000
#define MAX_VLD 2
#define MAX_PSN 0 /* no pre scaler for this chip */

    int M_min = 2;
    int M_max = 128;

    target = Clock * 1000;

    for (PSNx = 0; PSNx <= MAX_PSN ; PSNx++) {
	int low_N, high_N;
	double FrefVLDPSN;

	PSN = !PSNx ? 1 : 4;

	low_N = 2;
	high_N = 32;

	for ( VLD = 1 ; VLD <= MAX_VLD ; VLD++ ) {

	    FrefVLDPSN = (double)Fref * VLD / PSN;
	    for (N = low_N; N <= high_N; N++) {
		double tmp = FrefVLDPSN / N;

		for (P = 1; P <= 4; P++) {	
		    double Fvco_desired = target * ( P );
		    double M_desired = Fvco_desired / tmp;

		    /* Which way will M_desired be rounded?  
		     *  Do all three just to be safe.  
		     */
		    int M_low = M_desired - 1;
		    int M_hi = M_desired + 1;

		    if (M_hi < M_min || M_low > M_max)
			continue;

		    if (M_low < M_min)
			M_low = M_min;
		    if (M_hi > M_max)
			M_hi = M_max;

		    for (M = M_low; M <= M_hi; M++) {
			Fvco = tmp * M;
			if (Fvco <= MIN_VCO)
			    continue;
			if (Fvco > MAX_VCO)
			    break;

			Fout = Fvco / ( P );

			error = (target - Fout) / target;
			aerror = (error < 0) ? -error : error;
			if (aerror < abest) {
			    abest = aerror;
			    bestError = error;
			    bestM = M;
			    bestN = N;
			    bestP = P;
			    bestPSN = PSN;
			    bestVLD = VLD;
			    bestFout = Fout;
			}
#ifdef DEBUG1
			ErrorF("Freq. selected: %.2f MHz, M=%d, N=%d, VLD=%d,"
			       " P=%d, PSN=%d\n",
			       (float)(Clock / 1000.), M, N, P, VLD, PSN);
			ErrorF("Freq. set: %.2f MHz\n", Fout / 1.0e6);
#endif
		    }
		}
	    }
	}
    }
    vclk[Midx]    = bestM;
    vclk[Nidx]    = bestN;
    vclk[VLDidx]  = bestVLD;
    vclk[Pidx]    = bestP;
    vclk[PSNidx]  = bestPSN;
#ifdef DEBUG
    ErrorF("Freq. selected: %.2f MHz, M=%d, N=%d, VLD=%d, P=%d, PSN=%d\n",
	(float)(Clock / 1000.), vclk[Midx], vclk[Nidx], vclk[VLDidx], 
	   vclk[Pidx], vclk[PSNidx]);
    ErrorF("Freq. set: %.2f MHz\n", bestFout / 1.0e6);
#endif
}

static void
sisClockSave(Clock)
    sisClockPtr Clock;
{
#ifdef DEBUG
    ErrorF("sisClockSave(Clock)\n");
#endif
    Clock->msr = (inb(0x3CC) & 0xFE);	/* save the standard VGA clock 
					 * registers */
    read_xr(0x2A, Clock->xr2A);
    read_xr(0x2B, Clock->xr2B);
    
}

static void
sisClockRestore(Clock)
    sisClockPtr Clock;
{
#ifdef DEBUG
    ErrorF("sisClockRestore(Clock)\n");
#endif
    outb(0x3C2, Clock->msr);
    write_xr(0x2A, Clock->xr2A);
    write_xr(0x2B, Clock->xr2B);

}

static Bool
sisClockFind(no, Clock)
    int no;
    sisClockPtr Clock;
{
    int clock ;

    clock = vga256InfoRec.clock[no] ;

#ifdef DEBUG
    ErrorF("sisClockFind(%d %d)\n",no,clock);
#endif

    if (no > (vga256InfoRec.clocks - 1))
	return (FALSE);

    Clock->Clock = clock;

    return (TRUE);
}

static void
sisClockLoad(Clock)
    sisClockPtr Clock;
{
    unsigned int 	vclk[5];
    unsigned char 	temp, xr2a, xr2b;
#ifdef DEBUG
    ErrorF("sisClockLoad(Clock)\n");
#endif

    if (!Clock->Clock) {       /* Hack to load saved console clock */
	sisClockRestore(Clock) ;
    }
    else {
	sisCalcClock(Clock->Clock, vclk);

	xr2a = (vclk[Midx] - 1) & 0x7f ;
	xr2a |= ((vclk[VLDidx] == 2 ) ? 1 : 0 ) << 7 ;
	xr2b  = (vclk[Nidx] -1) & 0x1f ;
	xr2b |= (vclk[Pidx] -1 ) << 5 ;
	/*xr2b |= 0x80 ;*/   /* gain for high frequency */

	write_xr(0x2A, xr2a );
	write_xr(0x2B, xr2b );
#ifdef DEBUG
	ErrorF("xr2a=%2x xr2b=%2x\n",xr2a, xr2b);
#endif
    }
}


/* 
 * SISProbe --
 * 	check up whether a SIS 86C201 based board is installed
 */
static Bool
SISProbe()
{
  	int numClocks;
  	unsigned char temp;

	/*
         * Set up I/O ports to be used by this card
	 */
	xf86ClearIOPortList(vga256InfoRec.scrnIndex);
	xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);

	SISchipset = -1;

  	if (vga256InfoRec.chipset)
    	{
		/*
		 * If chipset from XF86Config doesn't match...
		 */
		if (!StrCaseCmp(vga256InfoRec.chipset, SISIdent(0)))
			SISchipset = SIS86C201;
		else if (!StrCaseCmp(vga256InfoRec.chipset, SISIdent(1)))
			SISchipset = SIS86C202;
		else if (!StrCaseCmp(vga256InfoRec.chipset, SISIdent(2)))
			SISchipset = SIS86C205;
		else
			return(FALSE);
    	}
	else
	{
		/* Aparently there are only PCI based 86C201's */

		if (vgaPCIInfo && vgaPCIInfo->Vendor == PCI_VENDOR_SIS)
		{
			switch(vgaPCIInfo->ChipType)
			{
			case PCI_CHIP_SG86C201: 	/* 86C201 */
				SISchipset = SIS86C201;
				break;
			case PCI_CHIP_SG86C202:		/* 86C202 */
				SISchipset = SIS86C202;
				break;
			case PCI_CHIP_SG86C205:		/* 86C205 */
				SISchipset = SIS86C205;
				break;
			}
		}
		if (SISchipset == -1)
			return (FALSE);
		vga256InfoRec.chipset = SISIdent(SISchipset);
	}

	SISEnterLeave(ENTER);
	
 	/* 
	 * How much Video Ram have we got?
	 */
    	if (!vga256InfoRec.videoRam)
    	{
		unsigned char temp;

		outb(0x3C4, 0x0F); 
		temp = inb(0x3C5);

		switch (temp & 0x03) 
		{
		case 0: 
			vga256InfoRec.videoRam = 1024;
			break;
		case 1:
			vga256InfoRec.videoRam = 2048;
			break;
		case 2: 
			vga256InfoRec.videoRam = 4096;
			break;
		}
     	}

#if defined(MONOVGA) || defined(XF86VGA16)
	if (!vga256InfoRec.clocks) {
	    numClocks = 4;
	    vgaGetClocks(numClocks, SISClockSelect);
	}
#else
	/*
	 * If clocks are not specified in XF86Config file, probe for them
	 */
	if ( (OFLG_ISSET(OPTION_HW_CLKS, &vga256InfoRec.options)) ||
	     (SISchipset == SIS86C201) )  {
	    /* if sis86c201 force to use the hw clock 
	     * if programmable clock works with the sis86c201
	     * let us know
	     */
	    if (!vga256InfoRec.clocks) {
		numClocks = 32;
		vgaGetClocks(numClocks, SISClockSelect);
	    }
	}
	else {
	    OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions);
	    ErrorF("%s %s: using programmable clocks.\n",
		   XCONFIG_PROBED, vga256InfoRec.name);
	    if(!vga256InfoRec.clockprog)
		vga256InfoRec.clocks = 0;
	}
	    
	/* maximal clock */
	if ( (SISchipset == SIS86C205) ||  (SISchipset == SIS86C202) )
	    vga256InfoRec.maxClock = 135000;
#endif	/* XF86VGA16*/

	vga256InfoRec.bankedMono = TRUE;
#ifndef MONOVGA
	/* We support Direct Video Access */
	vga256InfoRec.directMode = XF86DGADirectPresent;

	/* MaxClock set at 90MHz for 256 - ??? */

	OFLG_SET(OPTION_HW_CURSOR, &SIS.ChipOptionFlags);
	OFLG_SET(OPTION_SW_CURSOR, &SIS.ChipOptionFlags);
	OFLG_SET(OPTION_HW_CLKS, &SIS.ChipOptionFlags);
	OFLG_SET(OPTION_LINEAR, &SIS.ChipOptionFlags);
	OFLG_SET(OPTION_MMIO, &SIS.ChipOptionFlags);
	OFLG_SET(OPTION_NOLINEAR_MODE, &SIS.ChipOptionFlags);
	OFLG_SET(OPTION_NO_BITBLT, &SIS.ChipOptionFlags);
	OFLG_SET(OPTION_NO_IMAGEBLT, &SIS.ChipOptionFlags);
	OFLG_SET(OPTION_NOACCEL, &SIS.ChipOptionFlags);
#else
	/* Set to 130MHz at 16 colours */
	vga256InfoRec.maxClock = 130000;
#endif
#ifdef DPMSExtension
	vga256InfoRec.DPMSSet = SISDisplayPowerManagementSet;
#endif

    	return(TRUE);
}

static int 
sisPCIMemBase()
{

    if (vgaPCIInfo && vgaPCIInfo->Vendor == PCI_VENDOR_SIS) {
	if (vgaPCIInfo->MemBase != 0) {
	    return vgaPCIInfo->MemBase & 0xFFF80000;
	} else {
	    ErrorF("%s %s: %s: Can't find valid PCI "
		"Base Address\n", XCONFIG_PROBED,
		vga256InfoRec.name, vga256InfoRec.chipset);
	    return -1;
	}
    } else {
	ErrorF("%s %s: %s: Can't find PCI device in "
	    "configuration space\n", XCONFIG_PROBED,
	    vga256InfoRec.name, vga256InfoRec.chipset);
	return -1;
    }
}

static unsigned int
sisPCIMMIOBase()
{
    
    if ( vgaPCIInfo->IOBase ) 
	return vgaPCIInfo->IOBase ;
    else 
	return -1;
}

/*
 * SISScrnInit --
 *
 * Sets some accelerated functions
 */		
static int
SISScrnInit(pScreen, LinearBase, virtualX, virtualY, res1, res2, width)
ScreenPtr pScreen;
char *LinearBase;
int virtualX, virtualY, res1, res2, width;
{
#ifdef DEBUG
    ErrorF("SISScrnInit\n");
#endif
#if !defined(MONOVGA) && !defined(XF86VGA16)
    pScreen->CopyWindow = siscfbCopyWindow;
    pScreen->PaintWindowBackground = sisPaintWindow;
    pScreen->PaintWindowBorder = sisPaintWindow;
#endif	
    return(TRUE);
}

/*
 * SISFbInit --
 *	enable speedups for the chips that support it
 */
static void
SISFbInit()
{
#ifndef MONOVGA
	unsigned long j;
	unsigned long i;
	pointer sisVideoMem;
	long *poker;
	int offscreen_available;
	unsigned char tmp1,tmp2;
#ifdef DEBUG
	ErrorF("SISFbInit()\n");
#endif

	if (OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options))
	{
	    ErrorF("%s %s: Enabling Linear Addressing\n",
		   XCONFIG_GIVEN, vga256InfoRec.name);
		sisUseLinear = TRUE;
	}
	if (OFLG_ISSET(OPTION_NOLINEAR_MODE, &vga256InfoRec.options))
	{
	    ErrorF("%s %s: Disabling Linear Addressing\n",
		   XCONFIG_GIVEN, vga256InfoRec.name);
		sisUseLinear = FALSE;
	}

	if ( sisUseLinear ) {
	    if (vga256InfoRec.MemBase != 0) {
		SIS.ChipLinearBase = vga256InfoRec.MemBase;
		ErrorF("%s %s: base address is set at 0x%X.\n",
		       XCONFIG_GIVEN, vga256InfoRec.name, SIS.ChipLinearBase);
	    }	
	    else {
		SIS.ChipLinearBase = sisPCIMemBase();
		if (SIS.ChipLinearBase == -1) {
		    unsigned long addr,addr2 ;

		    outb(0x3C4, 0x21);
		    addr = inb(0x3C5) & 0x1f ;
		    addr <<= 27 ;
		    outb(0x3C4, 0x20);
		    addr2 = inb(0x3C5) ;
		    addr2 <<= 19 ;
		    addr |= addr2 ;
		    if ( addr == 0 )  {
			ErrorF("%s %s: Disabling Linear Addressing\n",
			       XCONFIG_PROBED, vga256InfoRec.name);
			ErrorF("%s %s:   Try to set MemBase in XF86Config\n",
			       XCONFIG_PROBED, vga256InfoRec.name);
			sisUseLinear = FALSE;
		    }
		    else {
			SIS.ChipLinearBase = addr ;
			ErrorF("%s %s: Trying Linear Addressing at 0x0%x\n",
			       XCONFIG_PROBED, vga256InfoRec.name,
			       SIS.ChipLinearBase);
		    }
		}
	    }
	}

	if ( sisUseLinear && xf86LinearVidMem() )
	{
	    SIS.ChipLinearSize = vga256InfoRec.videoRam * 1024;
	    ErrorF("%s %s: Using Linear Frame Buffer at 0x0%x, Size %dMB\n"
		   ,XCONFIG_PROBED, vga256InfoRec.name,
		   SIS.ChipLinearBase, SIS.ChipLinearSize/1048576);
	}	

	if (sisUseLinear) 
	    SIS.ChipUseLinearAddressing = TRUE;
	else 
	    SIS.ChipUseLinearAddressing = FALSE;

	if (sisUseMMIO && OFLG_ISSET(OPTION_NO_BITBLT,&vga256InfoRec.options)){
	    sisUseMMIO = FALSE ;
	    ErrorF("%s %s: SIS: Bit Block Transfert disabled\n",
		   OFLG_ISSET(OPTION_NO_BITBLT, &vga256InfoRec.options) ?
		   XCONFIG_GIVEN : XCONFIG_PROBED, vga256InfoRec.name); 
	}

	if ( sisUseMMIO ){
	    PCIMMIOBase = sisPCIMMIOBase() ;
	    sisUseMMIO = TRUE ;
	    if ( PCIMMIOBase == -1 ) {
		/* use default base */
		if ( sisUseLinear) 
		    /* sisMMIOBase = vgaBase , but not yet mapped here */
		    PCIMMIOBase = 0xA0000 ;
		else {
		    PCIMMIOBase = 0xB0000 ;
		    sisMMIOBase = xf86MapVidMem(vga256InfoRec.scrnIndex, 
					    MMIO_REGION,
					    (pointer)(PCIMMIOBase), 0x10000L);
		}
	    } else {
		sisMMIOBase = xf86MapVidMem(vga256InfoRec.scrnIndex, 
					    MMIO_REGION,
					    (pointer)(PCIMMIOBase), 0x10000L);
	    }
	    ErrorF("%s %s: SIS: Memory mapped I/O selected at 0x0%x\n",
		   OFLG_ISSET(OPTION_MMIO, &vga256InfoRec.options) ?
		   XCONFIG_GIVEN : XCONFIG_PROBED, 
		   vga256InfoRec.name,PCIMMIOBase);
	}


	SISDisplayableMemory = vga256InfoRec.displayWidth
				* vga256InfoRec.virtualY
				* (vgaBitsPerPixel / 8);

	offscreen_available = vga256InfoRec.videoRam * 1024 - 
					SISDisplayableMemory;

	if (OFLG_ISSET(OPTION_HW_CURSOR, &vga256InfoRec.options) ||
	    !OFLG_ISSET(OPTION_SW_CURSOR, &vga256InfoRec.options) )
	{
	    OFLG_SET(OPTION_HW_CURSOR, &vga256InfoRec.options) ;
	    /* SiS needs upper 16K for hardware cursor */
	    if (offscreen_available < 16384)
		ErrorF("%s %s: Not enough off-screen video"
		       " memory for hardware cursor,"
		       " using software cursor.\n",
		       XCONFIG_PROBED, vga256InfoRec.name);
	    else {
		SISCursorWidth = 64;
		SISCursorHeight = 64;
		vgaHWCursor.Initialized = TRUE;
		vgaHWCursor.Init = SISCursorInit;
		vgaHWCursor.Restore = SISRestoreCursor;
		vgaHWCursor.Warp = SISWarpCursor;
		vgaHWCursor.QueryBestSize = SISQueryBestSize;
		sisHWCursor = TRUE;
		ErrorF("%s %s: Using hardware cursor\n",
		       XCONFIG_GIVEN, vga256InfoRec.name);
		/* new offscreen_available */
		offscreen_available -= 16384 ;
	    }
	}

	if (OFLG_ISSET(OPTION_NO_IMAGEBLT, &vga256InfoRec.options)) {
	  ErrorF("%s %s: SIS: Not using mono expand system-to-video BitBLT.\n",
		 XCONFIG_GIVEN, vga256InfoRec.name);
	    sisAvoidImageBLT = TRUE;
	}

	if (sisUseMMIO) {
	   if (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options)) {
	       if ( !sisUseLinear || 
		   OFLG_ISSET(OPTION_XAA_NO_COL_EXP, &vga256InfoRec.options))
		   sisUseXAAcolorExp = FALSE;
	       SISAccelInit();
	   }
	   else if ( sisUseLinear ) {
	       ErrorF("%s %s: SIS: using old accelerated functions.\n",
		 XCONFIG_PROBED, vga256InfoRec.name);
	    switch (vgaBitsPerPixel) {
	      case 8:

		vga256LowlevFuncs.doBitbltCopy = siscfbDoBitbltCopy;
		vga256LowlevFuncs.vgaBitblt = sisMMIOBitBlt;
		
		vga256LowlevFuncs.fillRectSolidCopy = sisMMIOFillRectSolid;

		vga256LowlevFuncs.fillBoxSolid = siscfbFillBoxSolid;
		vga256TEOps1Rect.FillSpans = sisMMIOFillSolidSpansGeneral;
		vga256TEOps.FillSpans = sisMMIOFillSolidSpansGeneral;
		vga256LowlevFuncs.fillSolidSpans =sisMMIOFillSolidSpansGeneral;

		/* Setup the address of the tile/stipple in vram. 
		 * be aligned on a 64 bytes value. Size of the space
		 * is 1024 */
		/* in the future it might be better to keep all the offscreen
		   memory for cache pixmap/bitmap
		 */
		if (offscreen_available < 1024) {
			ErrorF("%s %s: Not enough off-screen video"
			       " memory for expand color."
			       " using builin pattern reg for 512 pixels\n",
			       XCONFIG_PROBED, vga256InfoRec.name);
		    sisBLTPatternOffscreenSize = 
			(SISchipset == SIS86C205) ? 512 : 256  ;
		    }			
		else {
		    int CursorSize = sisHWCursor?16384:0 ;
		    sisBLTPatternAddress = vga256InfoRec.videoRam * 1024 
			- CursorSize - 1024;
		    sisBLTPatternOffscreenSize = 1024 ;
		}
		/* Hook special op. fills (and tiles): */
		vga256TEOps1Rect.PolyFillRect = siscfbPolyFillRect;
		vga256NonTEOps1Rect.PolyFillRect = siscfbPolyFillRect;
		vga256TEOps.PolyFillRect = siscfbPolyFillRect;
		vga256NonTEOps.PolyFillRect = siscfbPolyFillRect;

		if (!OFLG_ISSET(OPTION_NO_IMAGEBLT,
				&vga256InfoRec.options)) {
		    vga256TEOps1Rect.PolyGlyphBlt = sisMMIOPolyGlyphBlt;
		    vga256TEOps.PolyGlyphBlt = sisMMIOPolyGlyphBlt;
		    vga256LowlevFuncs.teGlyphBlt8 = sisMMIOImageGlyphBlt;
		    vga256TEOps1Rect.ImageGlyphBlt = sisMMIOImageGlyphBlt;
		    vga256TEOps.ImageGlyphBlt = sisMMIOImageGlyphBlt;
		}

		break;
	      case 16:
		/* There are no corresponding structures to vga256LowlevFuncs
		 * for 16/24bpp. Hence we have to hook to the cfb functions
		 * in a similar way to the cirrus driver. For now I've just
		 * implemented the most basic of blits */

		cfb16TEOps1Rect.CopyArea = siscfb16CopyArea;
		cfb16TEOps.CopyArea = siscfb16CopyArea;
		cfb16NonTEOps1Rect.CopyArea = siscfb16CopyArea;
		cfb16NonTEOps.CopyArea = siscfb16CopyArea;

		cfb16TEOps1Rect.FillSpans = sisMMIOFillSolidSpansGeneral;
		cfb16TEOps.FillSpans = sisMMIOFillSolidSpansGeneral;
		cfb16NonTEOps1Rect.FillSpans = sisMMIOFillSolidSpansGeneral;
		cfb16NonTEOps.FillSpans = sisMMIOFillSolidSpansGeneral;
		
		/* Setup the address of the tile/stipple in vram. 
		 * be aligned on a 64 bytes value. Size of the space
		 * is 1024 */
		/* in the future it might be better to keep all the offscreen
		   memory for cache pixmap/bitmap
		 */
		if (offscreen_available < 1024) {
			ErrorF("%s %s: Not enough off-screen video"
			       " memory for expand color."
			       " using builin pattern reg for 512 pixels\n",
			       XCONFIG_PROBED, vga256InfoRec.name);
			sisBLTPatternOffscreenSize = 
			    (SISchipset == SIS86C205) ? 512 : 256  ;
		    }
		else {
		    int CursorSize = sisHWCursor?16384:0 ;
		    sisBLTPatternAddress = vga256InfoRec.videoRam * 1024 
			- CursorSize - 1024;
		    sisBLTPatternOffscreenSize = 1024 ;
		}
		cfb16TEOps1Rect.PolyFillRect = siscfbPolyFillRect;
		cfb16TEOps.PolyFillRect = siscfbPolyFillRect;
		cfb16NonTEOps1Rect.PolyFillRect = siscfbPolyFillRect;
		cfb16NonTEOps.PolyFillRect = siscfbPolyFillRect;

		if (!OFLG_ISSET(OPTION_NO_IMAGEBLT,
				&vga256InfoRec.options)) {
		    cfb16TEOps1Rect.PolyGlyphBlt = sisMMIOPolyGlyphBlt;
		    cfb16TEOps.PolyGlyphBlt = sisMMIOPolyGlyphBlt;
		    cfb16TEOps1Rect.ImageGlyphBlt = sisMMIOImageGlyphBlt;
		    cfb16TEOps.ImageGlyphBlt = sisMMIOImageGlyphBlt;
		}

		break ;
	      case 24:

		cfb24TEOps1Rect.CopyArea = siscfb24CopyArea;
		cfb24TEOps.CopyArea = siscfb24CopyArea;
		cfb24NonTEOps1Rect.CopyArea = siscfb24CopyArea;
		cfb24NonTEOps.CopyArea = siscfb24CopyArea;
		
		cfb24TEOps1Rect.FillSpans = sisMMIOFillSolidSpansGeneral;
		cfb24TEOps.FillSpans = sisMMIOFillSolidSpansGeneral;
		cfb24NonTEOps1Rect.FillSpans = sisMMIOFillSolidSpansGeneral;
		cfb24NonTEOps.FillSpans = sisMMIOFillSolidSpansGeneral;
		
		/* Setup the address of the tile/stipple in vram. 
		 * be aligned on a 64 bytes value. Size of the space
		 * is 1024 */
		/* in the future it might be better to keep all the offscreen
		   memory for cache pixmap/bitmap
		 */
		if (offscreen_available < 1024) {
			ErrorF("%s %s: Not enough off-screen video"
			       " memory for expand color."
			       " using builin pattern reg for 512 pixels\n",
			       XCONFIG_PROBED, vga256InfoRec.name);
			sisBLTPatternOffscreenSize = 
			    (SISchipset == SIS86C205) ? 512 : 256  ;
		    }
		else {
		    int CursorSize = sisHWCursor?16384:0 ;
		    sisBLTPatternAddress = vga256InfoRec.videoRam * 1024 
			- CursorSize - 1024;
		    sisBLTPatternOffscreenSize = 1024 ;
		}
		cfb24TEOps1Rect.PolyFillRect = siscfbPolyFillRect;
		cfb24TEOps.PolyFillRect = siscfbPolyFillRect;
		cfb24NonTEOps1Rect.PolyFillRect = siscfbPolyFillRect;
		cfb24NonTEOps.PolyFillRect = siscfbPolyFillRect;

		/* the enhanced color expansion is not supported
		 * by the engine in 16M-color graphic mode.
		 */
		sisAvoidImageBLT = TRUE;

		break;
	    }
	    vgaSetScreenInitHook(SISScrnInit);
	   }
       }		

#endif /* MONOVGA */
}

/*
 * SISEnterLeave --
 * 	enable/disable io-mapping
 */
static void
SISEnterLeave(enter)
	Bool enter;
{
  	unsigned char temp;
#ifdef DEBUG
    ErrorF("SISEnterLeave(");
    if (enter)
	ErrorF("Enter)\n");
    else
	ErrorF("Leave)\n");
#endif

#ifndef MONOVGA
#ifdef XFreeXDGA
	if (vga256InfoRec.directMode & XF86DGADirectGraphics && !enter)
	if (OFLG_ISSET(OPTION_HW_CURSOR, &vga256InfoRec.options))
		SISHideCursor();
#endif
#endif

  	if (enter)
    	{
      		xf86EnableIOPorts(vga256InfoRec.scrnIndex);
		vgaIOBase = (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;
      		outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);
      		outb(vgaIOBase + 5, temp & 0x7F);

		outw(0x3C4, 0x8605); 	/* Unlock Specials */
    	}
  	else
    	{
		outw(0x3C4, 0x0005);	/* Lock Specials */

      		xf86DisableIOPorts(vga256InfoRec.scrnIndex);
    	}
}

/*
 * SISRestore --
 *      restore a video mode
 */
static void
SISRestore(restore)
     	vgaSISPtr restore;
{
    int i;

#ifdef DEBUG
    ErrorF("SISRestore\n");
#endif
    vgaProtect(TRUE);

    for (i = 5 ; i <= 0x37; i++) {
	outb(0x3C4, i);
	if (inb(0x3C5) != restore->Port_3C4[i])
	    outb(0x3C5,restore->Port_3C4[i]);
    }

    /* set the clock */
    if ( ClockProgramable() ) {
	if (restore->std.NoClock >= 0)
	    sisClockLoad(&restore->sisClock);
    }
    else
	outw(0x3C4, ((restore->ClockReg) << 8) | 0x07);

    /*
     * Now restore generic VGA Registers
     */
    vgaHWRestore((vgaHWPtr)restore);

     outb(0x3C2, restore->ClockReg2);

#ifdef IO_DEBUG
     for (i = 5 ; i <= 0x37; i++) {
	outb(0x3C4, i);
	ErrorF("XR%X - %X\n", i, inb(0x3C5));
 	}
#endif
	vgaProtect(FALSE);
}

/*
 * SISSave --
 *      save the current video mode
 */
static void *
SISSave(save)
     	vgaSISPtr save;
{
    int	i;
#ifdef DEBUG
    ErrorF("SISSave\n");
#endif
  	save = (vgaSISPtr)vgaHWSave((vgaHWPtr)save, sizeof(vgaSISRec));

      for (i = 5 ; i <= 0x37; i++) {
	outb(0x3C4, i);
	save->Port_3C4[i] = inb(0x3C5) ;
#ifdef IO_DEBUG
	ErrorF("XS%X - %X\n", i, inb(0x3C5));
#endif
	}

	save->ClockReg2 = inb(0x3CC);

	/* save clock */
	if ( ClockProgramable() )
	    sisClockSave(&save->sisClock);

  	return ((void *) save);
}

/*
 *	In graphic mode: data ---------> to selector switch
 *
 * Now, let's look at the selector switch
 *
 *                                         FIFO
 *                             MCLK    ________________   VCLK
 * cpu/engine  <---o        o-------->|________________|---------> CRT
 *                 ^       ^             ^         ^
 *                  \     /              |         |
 *                   \   /               |         |
 *                    \ /                |         |
 *             selector switch   Threshold low Threshold high
 *
 * CRT consumes the data in the fifo. When data in FIFO reaches the 
 * level of  threshold low, the selector will switch to right so that the 
 * FIFO can be filled by data. When the data in FIFO  reaches the threshold 
 * high level, the selector will switch back to left.
 * The threshold low must  be set to a minimum level or the snow 
 * phenomenon (flicker noise) will be found on the screen.
 *
 * The threshold low should increase if  bpp increased, cause it means the 
 * required data for CRT is increased when bpp increased. When the threshold 
 * low  increased, the distance between threshold high and thereshold low 
 * should not be too near, else it will have the selector switch frequently, 
 * a bad performance result.
 */
static int
FindCRT_CPUthreshold(dotClock,bpp,thresholdLow,thresholdHigh)
int dotClock ;
int bpp;
int *thresholdLow;
int *thresholdHigh;
{
    /* here is an hack to set the CRT/CPU threshold register.
       the value in the three arrays come from Dump Registers in W95
     */
    struct ThresholdREC {
	int freq;
	int thresholdHigh;
	int thresholdLow;
    } ;
#if 0
    static struct ThresholdREC threshold8[]={{25250,0x6,0x3},{31500,0x6,0x3},
					     {40000,0x7,0x4},{44900,0x7,0x4},
					     {56250,0x7,0x4},
					     {65500,0x8,0x5},{78750,0x9,0x6},
					     {95000,0xa,0x7},{110000,0xd,0xb},
					     {135000,0xc,0xa}};
#else /* adjusted for X11 */
    /* this is the best values I've found.
     * there is no glitch but with intensive generation snow still appears 
     * on screen  especially well seen on the tiled root window.
     */
    static struct ThresholdREC threshold8[]={{25250,0x6,0x3},{31500,0x6,0x3},
					     {40000,0x7,0x4},{44900,0x7,0x4},
					     {56250,0x9,0x5},
					     {65000,0xe,0x5},/* ??? */
					     {78750,0xd,0x6},{85000,0xd,0x6},
					     {95000,0xb,0x7},{110000,0xc,0xa},
					     {135000,0xc,0xa}};
#endif
#if 0
    static struct ThresholdREC threshold16[]={{25250,0x8,0x5},{31500,0x8,0x5},
					      {40000,0xa,0x7},{44900,0xa,0x7},
					      {56250,0xb,0x8},
					      {65500,0xc,0xa},{78750,0xe,0xc},
					      {95000,0xf,0xd}};
#else
    static struct ThresholdREC threshold16[]={{25250,0x8,0x5},{31500,0x8,0x5},
					      {40000,0xa,0x7},{44900,0xa,0x7},
					      {56250,0xf,0x7},
					      {65000,0xf,0x7},{78750,0xf,0x8},
					      {95000,0xf,0xd}};
#endif
    static struct ThresholdREC threshold24[]={{25250,0xa,0x7},{31500,0xa,0x7},
					      {40000,0xc,0x9},{56250,0xe,0xc}};

    int nfreq ;
    int i;
    struct ThresholdREC *thresholdTab;

    switch ( bpp ) {
      case 8:
	thresholdTab = threshold8 ;
	nfreq = sizeof(threshold8)/sizeof(struct ThresholdREC);
	break;
      case 16:
	thresholdTab = threshold16 ;
	nfreq = sizeof(threshold16)/sizeof(struct ThresholdREC);
	break;
      case 24:
	thresholdTab = threshold24 ;
	nfreq = sizeof(threshold24)/sizeof(struct ThresholdREC);
	break;
      default:
	thresholdTab = threshold8 ;
	nfreq = sizeof(threshold8)/sizeof(struct ThresholdREC);
    }	    

    for ( i = 0 ; i < nfreq ; i++ ) 
	if ( thresholdTab[i].freq >= dotClock ) break ;
    if ( i == 0 ) {
	 *thresholdLow = thresholdTab[0].thresholdLow ;
	 *thresholdHigh = thresholdTab[0].thresholdHigh ;
	 return ;
     }
    if ( i == nfreq ) { /* nothing found */
	*thresholdLow = thresholdTab[nfreq -1].thresholdLow ;
	*thresholdHigh = thresholdTab[nfreq -1].thresholdHigh ;
    }
    else {
	*thresholdLow = thresholdTab[i-1].thresholdLow + 
	    ((thresholdTab[i].thresholdLow - thresholdTab[i-1].thresholdLow) *
	     (dotClock - thresholdTab[i-1].freq)) / 
		 ( thresholdTab[i].freq - thresholdTab[i-1].freq) ;
	*thresholdHigh = thresholdTab[i-1].thresholdHigh + 
	    ((thresholdTab[i].thresholdHigh - thresholdTab[i-1].thresholdHigh)*
	     (dotClock - thresholdTab[i-1].freq)) / 
		 ( thresholdTab[i].freq - thresholdTab[i-1].freq) ;
    }

#ifdef DEBUG
    ErrorF("FindCRT_CPUthreshold(%d, %d) = 0x%x 0x%x\n", dotClock, bpp,
	   *thresholdLow,*thresholdHigh);
#endif

}


/*
 * SISInit --
 *      Handle the initialization, etc. of a screen.
 */
static Bool
SISInit(mode)
    	DisplayModePtr mode;
{
	unsigned char temp;
	int offset;
	int i;
	unsigned int	CRT_CPUthresholdLow ;
	unsigned int	CRT_CPUthresholdHigh ;
	unsigned char	CRT_ENGthreshold ;
#ifdef DEBUG
    ErrorF("SISInit\n");
#endif

	/*
	 * Initialize generic VGA registers.
	 */
	vgaHWInit(mode, sizeof(vgaSISRec));

	/* get SIS Specific Registers */
	for (i = 5 ; i <= 0x37; i++) {
	    outb(0x3C4, i);
	    new->Port_3C4[i] = inb(0x3C5);
	}

#if !defined(MONOVGA) && !defined(XF86VGA16)
	offset = vga256InfoRec.displayWidth >>
#ifdef MONOVGA
		(mode->Flags & V_INTERLACE ? 3 : 4);
#else
		(mode->Flags & V_INTERLACE ? 2 : 3);

	new->std.Attribute[16] = 0x01;
	new->std.CRTC[20] = 0x40;
	new->std.CRTC[23] = 0xA3;
#endif

	/* some generic settings */
	new->std.Attribute[0x10] = 0x01;   /* mode */
	new->std.Attribute[0x11] = 0x00;   /* overscan (border) color */
	new->std.Attribute[0x12] = 0x0F;   /* enable all color planes */
	new->std.Attribute[0x13] = 0x00;   /* horiz pixel panning 0 */

	if ( (vgaBitsPerPixel == 16) || (vgaBitsPerPixel == 24) )
	    new->std.Graphics[0x05] = 0x00;    /* normal read/write mode */

	if (vgaBitsPerPixel == 16) {
	    offset <<= 1;	       /* double the width of the buffer */
	} else if (vgaBitsPerPixel == 24) {
	    offset += offset << 1;
	} 

	new->BankReg = 0x02;
	new->DualBanks = 0x00; 

	if ( sisUseLinear ) {
	    new->BankReg |= 0x80;  	/* enable linear mode addressing */
	    new->LinearAddr0 = (SIS.ChipLinearBase & 0x07f80000) >> 19 ; 
	    new->LinearAddr1 = ((SIS.ChipLinearBase & 0xf8000000) >> 27) |
		                (0x60) ; /* Enable Linear with max 4 mb*/
	}	
	else
	    new->DualBanks |= 0x08;

	if (vgaBitsPerPixel == 16) 
	    if (xf86weight.green == 5)
		new->BankReg |= 0x04;	/* 16bpp = 5-5-5             */
	    else
		new->BankReg |= 0x08;	/* 16bpp = 5-6-5             */

	if (vgaBitsPerPixel == 24) {
	    new->BankReg |= 0x10;
	    new->DualBanks |= 0x80;
	}

	new->std.CRTC[0x13] = offset & 0xFF;
	new->CRTCOff = ((offset & 0xF00) >> 4) | 
	    (((mode->CrtcVTotal-2) & 0x400) >> 10 ) |
		(((mode->CrtcVDisplay-1) & 0x400) >> 9 ) |
		    ((mode->CrtcVSyncStart & 0x400) >> 8 ) |
			(((mode->CrtcVSyncStart) & 0x400) >> 7 ) ;
	
	if (mode->Flags & V_INTERLACE)
		new->BankReg |= 0x20;


	if ( ClockProgramable() ){
	    /* init clock */
	    if (!sisClockFind(new->std.NoClock, &new->sisClock)) {
		ErrorF("Can't find desired clock\n");
		return (FALSE);
	    }
	}

	if (new->std.NoClock >= 0) {
	    new->ClockReg = new->std.NoClock;   /* not used in programmable  */
	    new->ClockReg2 = inb(0x3CC) | 0x0C; /* set internal/external clk */
	}

	/*  
	 *  this is function of the bandwidth
	 *  (pixelsize, displaysize, dotclock)
         *  worst case is not optimal
	 */
	CRT_ENGthreshold = 0x0f ;	
	FindCRT_CPUthreshold(vga256InfoRec.clock[new->std.NoClock], 
			     vgaBitsPerPixel,
			     &CRT_CPUthresholdLow, &CRT_CPUthresholdHigh);
	new->Port_3C4[0x08] = (CRT_ENGthreshold & 0x0F) | 
	    (CRT_CPUthresholdLow & 0x0F)<<4 ;
	new->Port_3C4[0x09] = (CRT_CPUthresholdHigh & 0x0F) ;
#if 0
	/* Graphics Modes seem to need a Higher MClk, than at Console
	 * Force a higher Mclk for now */
	if ( SISchipset == SIS86C205 ) {
	    /* 80 MHz MCLK */
	    /*new->Port_3C4[0x28] = 0xCF ;
	    new->Port_3C4[0x29] = 0x9C ;*/
	    /* 70 MHz MCLK */
	    new->Port_3C4[0x28] = 0xC1 ;
	    new->Port_3C4[0x29] = 0x1A ;
	    /* 60 MHz MCLK */
	    /*new->Port_3C4[0x28] = 0x57 ;
	    new->Port_3C4[0x29] = 0x14 ;*/
	    /* 45 MHz MCLK */
	    /*new->Port_3C4[0x28] = 0x15 ;
	    new->Port_3C4[0x29] = 0x06 ;*/
	}
#endif
	
	new->Port_3C4[0x27] |= 0x30 ; /* invalid logical screen width */

	if ( sisUseMMIO ) {
	    new->Port_3C4[0x27] |= 0x40 ; /* enable Graphic Engine Prog */
	    if ( !sisMMIOBase ) 
		sisMMIOBase = (unsigned char *)vgaBase ;	    
	    switch ( PCIMMIOBase ) {
	      case 0xA0000:
		new->Port_3C4[0x0B] |= 0x20 ; /* enable MMIO at 0xAxxxx */
		break;
	      case 0xB0000:
		new->Port_3C4[0x0B] |= 0x40 ; /* enable MMIO  at 0xBxxxx*/
		break;
	      default:
		new->Port_3C4[0x0B] |= 0x60 ; /* enable MMIO at PCI reg */
	    }
	    new->Port_3C4[0x0C] |= 0x80 ; /* 64-bit mode */
	    /*
	     * Setup the address to write monochrome source data to, for
	     * system to the screen colour expansion.
	     */
	    sisBltDataWindow = vgaLinearBase ;
	}
#else /* VGA16 */
	if (new->std.NoClock >= 0) {
	    new->ClockReg = new->std.NoClock;   
	    temp = inb(0x3CC) & ~0x0C ; 
	    new->ClockReg2 = temp | ( (new->ClockReg<<2) & 0x0C) ;
	}

#endif
        return(TRUE);
}

/*
 * SISAdjust --
 *      adjust the current video frame to display the mousecursor
 */

static void 
SISAdjust(x, y)
	int x, y;
{
	unsigned char temp;
	int base;

#ifdef MONOVGA
	base = (y * vga256InfoRec.displayWidth + x + 3) >> 3;
#else
	base = y * vga256InfoRec.displayWidth + x ;
	/* calculate base bpp dep. */
	switch (vgaBitsPerPixel) {
	  case 16:
	    base >>= 1;
	    break;
	  case 24:
	    base = ((base * 3)) >> 2;
	    base -= base % 6;
	    break;
	  default:       /* 8bpp */
	    base >>= 2;
	    break;
    }
#endif

  	outw(vgaIOBase + 4, (base & 0x00FF00) | 0x0C);
	outw(vgaIOBase + 4, ((base & 0x00FF) << 8) | 0x0D);

	outb(0x3C4, 0x27); temp = inb(0x3C5) & 0xF0;
	temp |= (base & 0x0F0000) >> 16;
	outb(0x3C5, temp);

#ifdef XFreeXDGA
	if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
		/* Wait until vertical retrace is in progress. */
		while (inb(vgaIOBase + 0xA) & 0x08);
		while (!(inb(vgaIOBase + 0xA) & 0x08));
	}
#endif
}

/*
 * SISValidMode --
 *
 */
static int
SISValidMode(mode, verbose,flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
	return MODE_OK;
}

/*
 * MGADisplayPowerManagementSet --
 *
 * Sets VESA Display Power Management Signaling (DPMS) Mode.
 */
#ifdef DPMSExtension
static void SISDisplayPowerManagementSet(PowerManagementMode)
int PowerManagementMode;
{
	unsigned char extDDC_PCR;
	unsigned char crtc17;
	unsigned char seq1;

#ifdef DEBUG
	ErrorF("SISDisplayPowerManagementSet(%d)\n",PowerManagementMode);
#endif
	if (!xf86VTSema) return;
	outb(vgaIOBase + 4, 0x17);
	crtc17 = inb(vgaIOBase + 5);
	outb(0x3C4, 0x11);
	extDDC_PCR = inb(0x3C5) & ~0xC0;
	switch (PowerManagementMode)
	{
	case DPMSModeOn:
	    /* HSync: On, VSync: On */
	    seq1 = 0x00 ;
	    crtc17 |= 0x80;
	    break;
	case DPMSModeStandby:
	    /* HSync: Off, VSync: On */
	    seq1 = 0x20 ;
	    extDDC_PCR |= 0x40;
	    break;
	case DPMSModeSuspend:
	    /* HSync: On, VSync: Off */
	    seq1 = 0x20 ;
	    extDDC_PCR |= 0x80;
	    break;
	case DPMSModeOff:
	    /* HSync: Off, VSync: Off */
	    seq1 = 0x20 ;
	    extDDC_PCR |= 0xC0;
	    /* DPMSModeOff is not supported with ModeStandby | ModeSuspend  */
            /* need same as the generic VGA function */
	    crtc17 &= ~0x80;
	    break;
	}
	outw(0x3C4, 0x0100);	/* Synchronous Reset */
	outb(0x3C4, 0x01);	/* Select SEQ1 */
	seq1 |= inb(0x3C5) & ~0x20;
	outb(0x3C5, seq1);
	usleep(10000);
	outb(vgaIOBase + 4, 0x17);
	outb(vgaIOBase + 5, crtc17);
	outb(0x3C4, 0x11);
	outb(0x3C5, extDDC_PCR);
	outw(0x3C4, 0x0300);	/* End Reset */
}
#endif










