/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/ibm8514/init.c,v 3.5 1996/12/23 06:37:59 dawes Exp $ */
/*
 * Written by Jake Richter
 * Copyright (c) 1989, 1990 Panacea Inc., Londonderry, NH - All Rights Reserved
 *
 * This code may be freely incorporated in any program without royalty, as
 * long as the copyright notice stays intact.
 *
 * Additions by Kevin E. Martin (martin@cs.unc.edu)
 *
 * KEVIN E. MARTIN AND TIAGO GONS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL KEVIN E. MARTIN OR TIAGO GONS BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified by Tiago Gons (tiago@comosjn.hobby.nl)
 */
/* $XConsortium: init.c /main/5 1996/10/19 17:52:31 kaleb $ */


#include "X.h"
#include "input.h"
#include "reg8514.h"
#include "ibm8514.h"
#include "xf86.h"
#include "xf86_OSlib.h"
#include "xf86Procs.h"

static short numPlanes = -1;
static short resolution = -1;
static short old_DAC_MASK = -1;
static LUTENTRY oldlut[256];
static short LUTInited = -1;


void ibm8514CleanUp(void)
{
    int i;

    if (LUTInited != -1) {
	outb(DAC_W_INDEX, 0);
	for (i = 0; i < 256; i++) {
	    outb(DAC_DATA, oldlut[i].r);
	    outb(DAC_DATA, oldlut[i].g);
	    outb(DAC_DATA, oldlut[i].b);
	}
    }

    if (old_DAC_MASK != -1) {
	outb(DAC_MASK, old_DAC_MASK);
    }
    outw(ADVFUNC_CNTL, 6);

    xf86DisableIOPorts(ibm8514InfoRec.scrnIndex);
}

/*	ibm8514Init(res)

	Initializes the 8514/A into the requested resolution.  Valid input
        values are 0 for 640x480 and 1 for 1024x768.
*/
void ibm8514Init(short res)
{
    static short initRegs[] = {
	DISP_CNTL, ADVFUNC_CNTL, V_TOTAL, V_SYNC_STRT,
	V_DISP, H_TOTAL, H_SYNC_STRT, H_DISP,
	H_SYNC_WID, V_SYNC_WID, DISP_CNTL, 0
    };

    static short mode640x4[] = {
        0x0041, 0x0003, 0x0830, 0x07a8,
        0x0779, 0x0063, 0x0052, 0x004f,
        0x002c, 0x0022, 0x0021
    };

    static short mode640x8[] = {
        0x0043, 0x0003, 0x0418, 0x03d2,
        0x033b, 0x0063, 0x0052, 0x004f,
        0x002c, 0x0022, 0x0023
    };

    static short mode1024x4[] = {
        0x0053, 0x0007, 0x0660, 0x0600,
        0x05fb, 0x009d, 0x0081, 0x007f,
        0x0016, 0x0008, 0x0033
    };

    static short mode1024x8[] = {
        0x0053, 0x0007, 0x0660, 0x0600,
        0x05fb, 0x009d, 0x0081, 0x007f,
        0x0016, 0x0008, 0x0033
    };

    static short *modeList[] = {mode640x4, mode1024x4, mode640x8, mode1024x8};

    short i;

    /* Get board status information */
    i = inw(SUBSYS_STAT);

#if 0
    /* Check if monitor capable of 1024x768 is attached.
     * If not, print error message and abort.
     * In this case, we want an IBM 8514 compatible monitor.
     */
    if ((i & MONITORID_MASK) != MONITORID_8514) {
	ibm8514CleanUp();
	FatalError("Attached monitor not capable of displaying 8514!\n");
    }
#endif

    old_DAC_MASK = inb(DAC_MASK);
    outb(DAC_MASK, 0x00);

    /* Determine the pixel depth of the board.  Then, based on depth,
     * set the memory control register use the appropriate CAS (VRAM
     * Column Address Select) configuration.
     */
    if (i & _8PLANE) {
	numPlanes = 8;
	outw(MULTIFUNC_CNTL, MEM_CNTL | VRTCFG_4 | HORCFG_8);
    } else {
	numPlanes = 4;
	ibm8514CleanUp();
	FatalError("Video card only has 512Kb (1Mb required)!\n");
    }

    /* Now initialize the display controller part of the 8514/A.
     * We have set up the initialization values in two sets of
     * tables:  4 bpp and 8 bpp.  The "numPlanes" comparison below
     * determines which set of values to use (dual indices).
     */
    for (i = 0; initRegs[i] != 0; i++)
	outw(initRegs[i], modeList[res + (numPlanes == 4 ? 0:2)][i]);

    /* Now reenable the screen, but only the planes that actually exist.
     * Otherwise, you end up with bus noise on the display.
     */
    if (numPlanes == 8)
	outb(DAC_MASK, 0xff);
    else
	outb(DAC_MASK, 0x0f);

    resolution = res;
}

/*	InitLUT()

	Loads the Look-Up Table with all black.
	Assumes 8-bit board is in use.  If 4 bit board, the only the first
	16 entries in LUT will be used.
*/
void InitLUT(void)
{
    short i, j;

    outb(DAC_R_INDEX, 0);
    for (i = 0; i < 256; i++) {
	oldlut[i].r = inb(DAC_DATA);
	oldlut[i].g = inb(DAC_DATA);
	oldlut[i].b = inb(DAC_DATA);
    }
    LUTInited = 1;

    outb(DAC_W_INDEX, 0);

    /* Load the first 16 LUT entries */
    for (i = 0; i < 16; i++) {
	outb(DAC_DATA, 0);
	outb(DAC_DATA, 0);
	outb(DAC_DATA, 0);
    }

    if (numPlanes == 8) {
	/* Load the remaining 240 LUT entries */
	for (i = 1; i < 16; i++) {
	    for (j = 0; j < 16; j++) {
		outb(DAC_DATA, 0);
		outb(DAC_DATA, 0);
		outb(DAC_DATA, 0);
	    }
	}
    }
}

/*	InitEnvironment()

	Initializes the 8514/A's drawing environment and clears the display.
*/
void InitEnvironment(void)
{

    InitLUT();

    /* Current mixes, src, foreground active */
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);

    /* Clipping rectangle to full drawable space */
    outw(MULTIFUNC_CNTL, SCISSORS_T | 0x000);
    outw(MULTIFUNC_CNTL, SCISSORS_L | 0x000);
    if (resolution == MODE_640) {
	outw(MULTIFUNC_CNTL, SCISSORS_R | 0x3ff); /* Should these be */
	outw(MULTIFUNC_CNTL, SCISSORS_B | 0x3ff); /* different???    */
    } else {
	outw(MULTIFUNC_CNTL, SCISSORS_R | 0x3ff);
	outw(MULTIFUNC_CNTL, SCISSORS_B | 0x3ff);
    }

    /* Enable writes to all planes and reset color compare */
    outw(WRT_MASK, 0xffff);
    outw(MULTIFUNC_CNTL, PIX_CNTL | 0x0000);

#ifndef DIRTY_STARTUP
    if (serverGeneration == 1) /* 5-5-93 TCG : servergen.. is always 1 ? */
    {
	/* Clear the display.  Need to set the color, origin, and size.
	 * Then draw.
	 */
	WaitQueue(6);
	if( xf86FlipPixels )
	  outw(FRGD_COLOR, 1);
	else
	  outw(FRGD_COLOR, 0);
	outw(CUR_X, 0);
	outw(CUR_Y, 0);
	outw(MAJ_AXIS_PCNT, 1023);
	outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | 1023);
	outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW | PLANAR | WRTDATA);
    }
#endif /* ! DIRTY_STARTUP */

    WaitQueue(4);

    /* Reset current draw position */
    outw(CUR_X, 0);
    outw(CUR_Y, 0);

    /* Reset current colors, foreground is all on, background is 0. */
    outw(FRGD_COLOR, 0xffff);
    outw(BKGD_COLOR, 0x0000);
}
