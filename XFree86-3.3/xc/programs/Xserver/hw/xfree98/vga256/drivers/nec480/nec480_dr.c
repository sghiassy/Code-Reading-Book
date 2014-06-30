/* $XConsortium: nec480_dr.c /main/6 1996/10/23 18:51:20 kaleb $ */
/*
 * Id: nec480_driver.c,v 1.6 1995/06/25 00:28:39 ueno Exp
 *
 *    It was my dream.                        -- UENO Shuichi.
 *
 */

/* $XFree86: xc/programs/Xserver/hw/xfree98/vga256/drivers/nec480/nec480_dr.c,v 3.6.2.3 1997/05/09 09:31:45 hohndel Exp $ */

/*
 * These are X and server generic header files.
 */
#include "X.h"
#include "input.h"
#include "screenint.h"

/*
 * These are XFree86-specific header files
 */
#include "compiler.h"
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "vga.h"

#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>

/*
 * Driver data structures.
 */
typedef struct {
	vgaHWRec std;
} vgaNEC480Rec, *vgaNEC480Ptr;

/*
 * Forward definitions for the functions that make up the driver.  See
 * the definitions of these functions for the real scoop.
 */
static Bool     NEC480Probe();
static char *   NEC480Ident();
static void     NEC480EnterLeave();
static Bool     NEC480Init();
static int	NEC480ValidMode();
static void *   NEC480Save();
static void     NEC480Restore();

/*
 * These are VRAM bank select functions.
 */
extern	void     NEC480SetRead();
extern	void     NEC480SetWrite();
extern	void     NEC480SetReadWrite();

/*
 * This data structure defines the driver itself.  The data structure is
 * initialized with the functions that make up the driver and some data 
 * that defines how the driver operates.
 */
vgaVideoChipRec NEC480 = {
	/* 
	 * Function pointers
	 */
	NEC480Probe,
	NEC480Ident,
	NEC480EnterLeave,
	NEC480Init,
	NEC480ValidMode,
	NEC480Save,
	NEC480Restore,
	(void (*)())NoopDDA,
	(void (*)())NoopDDA,
	(void (*)())NoopDDA,
	(void (*)())NoopDDA,
	NEC480SetRead,
	NEC480SetWrite,
	NEC480SetReadWrite,
	/*
	 * This is the size of the mapped memory window, usually 64k.
	 */
	0x10000,		
	/*
	 * This is the size of a video memory bank for this chipset.
	 */
	0x08000,
	/*
	 * This is the number of bits by which an address is shifted
	 * right to determine the bank number for that address.
	 */
	15,
	/*
	 * This is the bitmask used to determine the address within a
	 * specific bank.
	 */
	0x7FFF,
	/*
	 * These are the bottom and top addresses for reads inside a
	 * given bank.
	 */
	0x00000, 0x08000,
	/*
	 * And corresponding limits for writes.
	 */
	0x08000, 0x10000,
	/*
	 * Whether this chipset supports a single bank register or
	 * seperate read and write bank registers.
	 */
	TRUE,
	/*
	 * If the chipset requires vertical timing numbers to be divided
	 * by two for interlaced modes, set this to VGA_DIVIDE_VERT.
	 */
	VGA_NO_DIVIDE_VERT, /* ? */
	/*
	 * This is a dummy initialization for the set of option flags
	 * that this driver supports.  It gets filled in properly in the
	 * probe function, if the probe succeeds (assuming the driver
	 * supports any such flags).
	 */
	{0,},
	/*
	 * This determines the multiple to which the virtual width of
	 * the display must be rounded for the 256-color server.
	 */
	8,
	FALSE,
	0,
	0,
	FALSE,
	FALSE,
	FALSE,
	NULL,
	1,                         /* ChipClockMulFactor */
	1                          /* ChipClockDivFactor */
};

unsigned short *vramwindow_r;
unsigned short *vramwindow_w;
unsigned short *vramwindow;
static   char  *ppmodep, ppmode_sv;

/*
 * NEC480Ident --
 *
 * Returns the string name for supported chipset 'n'.
 */
static char *
NEC480Ident(int n)
{
	static char *chipsets[] = {"pegc"};

	if (n + 1 > sizeof(chipsets) / sizeof(char *))
		return(NULL);
	else
		return(chipsets[n]);
}

#define	NEC480_MAX_CLOCK_IN_KHZ	31500

/*
 * NEC480Probe --
 */
static Bool
NEC480Probe()
{
	xf86ClearIOPortList(vga256InfoRec.scrnIndex);
	xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);
	/*
	 * First we attempt to figure out if one of the supported chipsets
	 * is present.
	 */
	if (vga256InfoRec.chipset)
	{
		/*
		 * This is the easy case.  The user has specified the
		 * chipset in the XF86Config file.  All we need to do here
		 * is a string comparison against each of the supported
		 * names available from the Ident() function.
		 */
		if (StrCaseCmp(vga256InfoRec.chipset, NEC480Ident(0)))
			return (FALSE);
	}

	/*
	 * If the user has specified the amount of memory in the XF86Config
	 * file, we respect that setting.
	 */
  	if (!vga256InfoRec.videoRam)
    	{
		/*
		 * Otherwise, do whatever chipset-specific things are 
		 * necessary to figure out how much memory (in kBytes) is 
		 * available.
		 */
		vga256InfoRec.videoRam = 512;
    	}

	/*
	 * Again, if the user has specified the clock values in the XF86Config
	 * file, we respect those choices.
	 */
  	if (!vga256InfoRec.clocks)
    	{
		vga256InfoRec.clocks = 1;
		vga256InfoRec.clock[0] = NEC480_MAX_CLOCK_IN_KHZ;
    	}

	vga256InfoRec.maxClock = NEC480_MAX_CLOCK_IN_KHZ;
  	vga256InfoRec.chipset = NEC480Ident(0);
  	vga256InfoRec.bankedMono = TRUE;

	NEC480EnterLeave(ENTER);

	/*
	 * Map the VRAM window's address
	 */
	vramwindow = xf86MapVidMem(0,VGA_REGION,(pointer)(0xe0000),4096);
	vramwindow_r = vramwindow + 2;
	vramwindow_w = vramwindow + 3;
	*(vramwindow_r) = 0;
	*(vramwindow_w) = 1;

	/* Packed Pixel Mode */
	ppmodep = (char *)((char *)vramwindow + 0x100);
	ppmode_sv = *ppmodep;
	*ppmodep = 0;

	return(TRUE);
}

/*
 * NEC480EnterLeave --
 */

static void gdc_init();

static void 
NEC480EnterLeave(Bool enter)
{
	static int flag = 0;
	static int graph_mode, hsync31, cont_page;

	if(flag == 0)
	{
		if (enter == ENTER)
			xf86EnableIOPorts(vga256InfoRec.scrnIndex);
		/* Save current graphic mode, 1: Extended graphic mode */
		outb(0x9a0, 0x0a);
		graph_mode = inb(0x9a0) & 0x01;
		/* Save current G-VRAM page status, 1: continuation */
		outb(0x9a0, 0x0d);
		cont_page = inb(0x9a0) & 0x01;
		/* Save current horizontal sync, 1: 31.5KHz */
		if( vga256InfoRec.clock[0] == NEC480_MAX_CLOCK_IN_KHZ ){
			hsync31 = inb(0x9a8) & 0x01;
		}
		flag = 1;
	}

	if(enter == ENTER)
	{
		if (flag != 0)
			xf86EnableIOPorts(vga256InfoRec.scrnIndex);
		while (!(inb(0x60) & 0x20)) ;	/* V-SYNC wait */
		outb(0x62, 0xc);		/* text off */
		outb(0xA2, 0xc);		/* graphics off */
		/* GDC : 5MHz mode */
		outb(0x6a, 0x83);
		outb(0x6a, 0x85);
		if( vga256InfoRec.clock[0] == NEC480_MAX_CLOCK_IN_KHZ ){
			gdc_init();
		}
		outb(0x6a, 0x07);
		outb(0x6a, 0x21);		/* Extended mode */
		outb(0x6a, 0x69);		/* Continuous G-VRAM page */
		if( vga256InfoRec.clock[0] == NEC480_MAX_CLOCK_IN_KHZ ){
			outb(0x9a8, 0x01);	/* 24.8KHz -> 31.5KHz */
		}
		while (!(inb(0x60) & 0x20)) ;	/* V-SYNC wait */
		outb(0x62, 0xc);		/* text off */
		outb(0xA2, 0xd);		/* graphics on */
	}
	if(enter == LEAVE)
	{
		if(graph_mode == 0)
		{
			outb(0x6a, 0x07);
			outb(0x6a, 0x20);	/* Standard mode */
		}
		if(cont_page == 0)
		{
			outb(0x6a, 0x07);
			outb(0x6a, 0x68);	/* separate VRAM page */
		}
		if( vga256InfoRec.clock[0] == NEC480_MAX_CLOCK_IN_KHZ
		   && hsync31==0 ){
			outb(0x9a8, 0x00);	/* 31.5KHz-> 24.8KHz */
		}
		outb(0xA2, 0xc);		/* graphics off */
		outb(0x62, 0xd);		/* text on */
		*ppmodep = ppmode_sv;
		xf86DisableIOPorts(vga256InfoRec.scrnIndex);
	}
}

/*
 * NEC480Restore --
 */
static void 
NEC480Restore(vgaNEC480Ptr restore)
{
	vgaHWRestore((vgaHWPtr)restore);
}

/*
 * NEC480Save --
 */
static void *
NEC480Save(vgaNEC480Ptr save)
{
	save = (vgaNEC480Ptr)vgaHWSave((vgaHWPtr)save,
			sizeof(vgaNEC480Rec));
  	return ((void *) save);
}

/*
 * NEC480Init --
 */
static Bool
NEC480Init(DisplayModePtr mode)
{
	/*
	 * The character clock is set to 4 dots for 256 color modes
	 * we adapt them here so that the VGA module will initialise
	 * the CRTC fields correctly
	 */
	mode->HTotal <<= 1;
	mode->HDisplay <<= 1;
	mode->HSyncStart <<= 1;
	mode->HSyncEnd <<= 1;

	/*
	 * This will allocate the datastructure and initialize all of the
	 * generic VGA registers.
	 */
	if (!vgaHWInit(mode,sizeof(vgaNEC480Rec)))
		return(FALSE);

	/*
	 * Restore changed horizontal display parameters
	 */
	mode->HTotal >>= 1;
	mode->HDisplay >>= 1;
	mode->HSyncStart >>= 1;
	mode->HSyncEnd >>= 1;

	return (TRUE);
}

/*
 * NEC480ValidMode --
 *
 */
static int
NEC480ValidMode(DisplayModePtr mode, Bool verbose, int flag)
{
	return(MODE_OK);
}

union gdc_sync_parm
{
	unsigned char param[8];
	struct {
		unsigned int	_S_S	:1,
				_S_G	:1,
				_S_D	:1,
				_S_I	:1,
				_S_F	:1,
				_S_CHR	:1,
				zero1	:2,
				_S_CR	:8,
				_S_HS	:5,
				_S_VS	:5,
				_S_HFP	:6;
		unsigned int	_S_HBP	:6,
				zero2	:2,
				_S_VFP	:6,
				zero3	:2,
				_S_LF	:10,
				_S_VBP	:6;
	} _s_t;
#define _S_G	_s_t._S_G
#define _S_D	_s_t._S_D
#define _S_CR	_s_t._S_CR
#define _S_HS	_s_t._S_HS
#define _S_VS	_s_t._S_VS
#define _S_HFP	_s_t._S_HFP
#define _S_HBP	_s_t._S_HBP
#define _S_VFP	_s_t._S_VFP
#define _S_LF	_s_t._S_LF
#define _S_VBP	_s_t._S_VBP
};

#define _GDC_CR		(80 - 2)
#define _GDC_HS		(12 - 1)
#define _GDC_HFP	(4  - 1)
#define _GDC_HBP	(4  - 1)
#define _GDC_VS		(2)
#define _GDC_VFP	(6)
#define _GDC_VBP	(37)
#define _GDC_LF		(480)

static void gdc_init()
{
	union gdc_sync_parm s;

	while((inb(0x60) & 0x04) == 0) ; /* FIFO empty	*/
	outb(0x62, 0x00);	/* RESET			*/

	bzero(&s, sizeof(s));
	s._S_CR  = _GDC_CR;
	s._S_HS  = _GDC_HS;
	s._S_HFP = _GDC_HFP;
	s._S_HBP = _GDC_HBP;
	s._S_VS  = _GDC_VS;
	s._S_VFP = _GDC_VFP;
	s._S_VBP = _GDC_VBP;
	s._S_LF  = _GDC_LF;
	while((inb(0x60) & 0x04) == 0) ; /* FIFO empty	*/
	outb(0x62, 0x0E);	/* SYNC			*/
	outb(0x60, s.param[0]);	/*   P1 : 0 0 CHR F I D G S	*/
	outb(0x60, s.param[1]);	/*   P2 : C/R			*/
	outb(0x60, s.param[2]);	/*   P3 : VSl(3) + HS(5)	*/
	outb(0x60, s.param[3]);	/*   P4 : HFP(6) + VSh(2)	*/
	outb(0x60, s.param[4]);	/*   P5 : 0 0 HBP(6)		*/
	outb(0x60, s.param[5]);	/*   P6 : 0 0 VFP(6)		*/
	outb(0x60, s.param[6]);	/*   P7 : L/Fl			*/
	outb(0x60, s.param[7]);	/*   P8 : VBP(6) + L/Fh(2)	*/

	while((inb(0x60) & 0x04) == 0) ; /* FIFO empty	*/
	outb(0x62, 0x47);	/* TEXT GDC PITCH		*/
	outb(0x60, 0x50);	/*				*/

	while((inb(0x60) & 0x04) == 0) ; /* FIFO empty	*/
	while ( (inb(0x60) & 0x20) != 0); /* VSYNC wait */
	while ( (inb(0x60) & 0x20) == 0);
	outb(0x62, 0x70);	/* TEXT GDC SCROLL		*/
	outb(0x60, 0x00);	/*   :			*/
	outb(0x60, 0x00);	/*   :			*/
	outb(0x60, 0x00);	/*   :			*/
	outb(0x60, 0x1E);	/*   :			*/

	while((inb(0x60) & 0x04) == 0) ; /* FIFO empty	*/
	outb(0x62, 0x00);	/* RESET			*/

	bzero(&s, sizeof(s));
	s._S_D   = 1;
	s._S_G   = 1;
	s._S_CR  = _GDC_CR;
	s._S_HS  = _GDC_HS;
	s._S_HFP = _GDC_HFP;
	s._S_HBP = _GDC_HBP;
	s._S_VS  = _GDC_VS;
	s._S_VFP = _GDC_VFP;
	s._S_VBP = _GDC_VBP;
	s._S_LF  = _GDC_LF;
	while((inb(0xA0) & 0x4) == 0) ; /* FIFO empty	*/
	outb(0xA2, 0x0E);	/* SYNC */
	outb(0xA0, s.param[0]); /*   P1 : 0 0 CHR F I D G S	*/
	outb(0xA0, s.param[1]); /*   P2 : C/R			*/
	outb(0xA0, s.param[2]); /*   P3 : VSl(3) + HS(5)	*/
	outb(0xA0, s.param[3]); /*   P4 : HFP(6) + VSh(2)	*/
	outb(0xA0, s.param[4]); /*   P5 : 0 0 HBP(6)		*/
	outb(0xA0, s.param[5]); /*   P6 : 0 0 VFP(6)		*/
	outb(0xA0, s.param[6]); /*   P7 : L/Fl			*/
	outb(0xA0, s.param[7]); /*   P8 : VBP(6) + L/Fh(2)	*/

	while((inb(0x60) & 0x04) == 0) ; /* FIFO empty	*/
	while ( (inb(0xA0) & 0x20) != 0); /* VSYNC wait */
	while ( (inb(0xA0) & 0x20) == 0);
	outb(0xA2, 0x70);	/* Graph GDC SCROLL		*/
	outb(0xA0, 0x00);	/*   :			*/
	outb(0xA0, 0x00);	/*   :			*/
	outb(0xA0, 0x00);	/*   :			*/
	outb(0xA0, 0x5E);	/*   :			*/

	while((inb(0x60) & 0x04) == 0) ; /* FIFO empty	*/
	outb(0xA2, 0x47);	/*   PITCH			*/
	outb(0xA0, 0x50);	/*				*/
}
