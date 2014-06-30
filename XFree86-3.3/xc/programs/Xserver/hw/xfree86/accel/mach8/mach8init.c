/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach8/mach8init.c,v 3.7 1996/12/23 06:40:06 dawes Exp $ */
/*
 * Written by Jake Richter
 * Copyright (c) 1989, 1990 Panacea Inc., Londonderry, NH - All Rights Reserved
 *
 * This code may be freely incorporated in any program without royalty, as
 * long as the copyright notice stays intact.
 *
 * Additions by Kevin E. Martin (martin@cs.unc.edu)
 *
 * KEVIN E. MARTIN, RICKARD E. FAITH, SCOTT LAIRD, AND TIAGO GONS DISCLAIM
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * KEVIN E. MARTIN, RICKARD E. FAITH, SCOTT LAIRD, OR TIAGO GONS BE LIABLE
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Further modifications by Scott Laird (lair@kimbark.uchicago.edu)
 * and Tiago Gons (tiago@comosjn.hobby.nl)
 */
/* $XConsortium: mach8init.c /main/6 1996/10/19 17:54:13 kaleb $ */

#include "X.h"
#include "input.h"
#include "mach8.h"
#include "regmach8.h"
#include "xf86.h"
#include "xf86_OSlib.h"
#include "xf86Procs.h"

#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

static short numPlanes = -1;
static short resolution = -1;
static short old_DAC_MASK = -1;
static LUTENTRY oldlut[256];
static short LUTInited = -1;


/* mach8calcvmode--
 *      Calculate Mach8 register values from display specifications
 *      from the XF86Config file.
 */

void
mach8calcvmode(vmdef, pMode)
     struct mach8vmodedef *vmdef;
     DisplayModePtr pMode;
{
    vmdef->dc  = DISPEN_ENAB | MEMCFG_4 | ODDBNKENAB;

    vmdef->clk = pMode->Clock;

    vmdef->hd  = (pMode->CrtcHDisplay >> 3) - 1;
    vmdef->ht  = (pMode->CrtcHTotal >> 3) - 1;
    /*vmdef->hss = (pMode->CrtcHSyncStart >> 3) - 1;*/
    vmdef->hss = (pMode->CrtcHSyncStart - 1) >> 3;
    vmdef->hsw = (pMode->CrtcHSyncEnd - pMode->CrtcHSyncStart) >> 3;
    if(vmdef->hsw > 0x1f)
    {
        ErrorF("%s %s: Horizontal Sync width (%d) in mode \"%s\"\n",
               XCONFIG_PROBED, mach8InfoRec.name, vmdef->hsw, pMode->name);
	ErrorF("\tshortened to 248 pixels\n");
    }
    vmdef->hsw &= 0x1F;

    vmdef->vd  = ((((pMode->CrtcVDisplay-1)<<1)&0xFFF8) | 
                 (((pMode->CrtcVDisplay-1))&0x3)) |
                 ((((pMode->CrtcVDisplay-1)&0x80)>>5));

    vmdef->vt  = ((((pMode->CrtcVTotal-1)<<1)&0xFFF8) | 
                 (((pMode->CrtcVTotal-1)&0x3)) |
                 (((pMode->CrtcVTotal-1)&0x80)>>5));

    vmdef->vss = ((((pMode->CrtcVSyncStart-1)<<1)&0xFFF8) | 
                 (((pMode->CrtcVSyncStart-1)&0x3)) |
                 (((pMode->CrtcVSyncStart-1)&0x80)>>5));

    vmdef->vsw = pMode->CrtcVSyncEnd - pMode->CrtcVSyncStart;
    if(vmdef->vsw > 0x1f)
    {
        ErrorF("%s %s: Vertical Sync width (%d) in mode \"%s\"\n",
               XCONFIG_PROBED, mach8InfoRec.name, vmdef->vsw, pMode->name);
	ErrorF("\tshortened to 31 lines\n");
    }
    vmdef->vsw &= 0x1F;

    if(pMode->Flags & V_NHSYNC)
        vmdef->hsw |= HSYNCPOL_NEG;

    if(pMode->Flags & V_NVSYNC)
        vmdef->vsw |= VSYNCPOL_NEG;

    if(pMode->Flags & V_INTERLACE)
        vmdef->dc |= INTERLACE;

    if(pMode->Flags & V_CSYNC)
        vmdef->clk |= COMPOSITE_SYNC;

/*
    ErrorF("Mach8calcvmode: (%x,%x,%x,%x),(%x,%x,%x,%x),(%x,%x)\n",
           vmdef->hd,vmdef->hss,vmdef->hsw,vmdef->ht,      
           vmdef->vd,vmdef->vss,vmdef->vsw,vmdef->vt,
           vmdef->dc,vmdef->clk);
*/
}


/*
 * mach8setcrtregs --
 *      Program the Mach8 for arbitrary resolutions.
 */

void
mach8setcrtregs(vmdef)
     struct mach8vmodedef *vmdef;
{
    /* TCG : disable crt and unlock shadow sets */
    outw(DISP_CNTL, DISPEN_DISAB | MEMCFG_4 | ODDBNKENAB);
    outw(SHADOW_SET, 1);
    outw(SHADOW_CTL, 0);
    outw(SHADOW_SET, 2);
    outw(SHADOW_CTL, 0);
    outw(SHADOW_SET, 0);

    /* set number of pixels per scan line in memory.  This is tied to
     * virtualX.
     */

    if( mach8InfoRec.videoRam > 512 ) {
	outw( CRT_PITCH, 1024 >> 3 );
	outw( GE_PITCH, 1024 >> 3 );
    }
    else {
	outw(CRT_PITCH, mach8InfoRec.virtualX >> 3);
	outw(GE_PITCH, mach8InfoRec.virtualX >> 3);
    }
    outw(H_TOTAL,vmdef->ht);
    outw(H_DISP,vmdef->hd);
    outw(H_SYNC_STRT,vmdef->hss);
    outw(H_SYNC_WID,vmdef->hsw);

    outw(V_TOTAL,vmdef->vt);
    outw(V_DISP,vmdef->vd);
    outw(V_SYNC_STRT,vmdef->vss);
    outw(V_SYNC_WID,vmdef->vsw);

    /* 
     * Make sure that the start of video memory is at the beginning of 
     * memory.  We use these for virtual display panning.
     */
    /* TCG : adjust frame after vt switch */
    if (mach8InfoRec.frameX0 != -1 && mach8InfoRec.frameY0 != -1)
	mach8Adjust(mach8InfoRec.frameX0, mach8InfoRec.frameY0);
    else {
	outw(GE_OFFSET_HI,0);
	outw(GE_OFFSET_LO,0);
	outw(CRT_OFFSET_HI,0);
	outw(CRT_OFFSET_LO,0);
    }

    if (OFLG_ISSET(OPTION_CSYNC, &mach8InfoRec.options))
	outw(CLOCK_SEL, ((vmdef->clk & 0x00ff) << 2) | 0xa01 | COMPOSITE_SYNC);
    else
	outw(CLOCK_SEL,
	 ((vmdef->clk & 0x00ff) << 2) | 0xa01 | (vmdef->clk & COMPOSITE_SYNC));

    outw(DISP_CNTL,vmdef->dc);
}


void
mach8CleanUp(void)
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

    xf86DisableIOPorts(mach8InfoRec.scrnIndex);
}

/*	mach8Init(res)
 *
 *    Initializes the Mach-8 into the requested resolution.  This takes
 *    a mach8vmodedef value generated by mach8calcvmode and sets up
 *    the card.
 */
void
mach8Init(vmdef)
     struct mach8vmodedef *vmdef;
{
    short i;

    /* Get board status information */
    i = inw(SUBSYS_STAT);

    /* Determine the pixel depth of the board.  Then, based on depth,
     * set the memory control register use the appropriate CAS (VRAM
     * Column Address Select) configuration.
     */
    if (i & _8PLANE) {
	numPlanes = 8;
	outw(MULTIFUNC_CNTL, MEM_CNTL | VRTCFG_4 | HORCFG_8);
    } else {
	numPlanes = 4;
	mach8CleanUp();
	FatalError("Video card only has 512Kb (1Mb required)!\n");
    }

    old_DAC_MASK = inb(DAC_MASK);
    outb(DAC_MASK, 0x00);

    mach8setcrtregs(vmdef);

    /* Now reenable the screen, but only the planes that actually exist.
     * Otherwise, you end up with bus noise on the display.
     */
    if (numPlanes == 8)
	outb(DAC_MASK, 0xff);
    else
	outb(DAC_MASK, 0x0f);

    resolution = MODE_CUSTOM;
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

/*	mach8InitEnvironment()

	Initializes the Mach-8's drawing environment and clears the display.
*/
void mach8InitEnvironment()
{
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

/*#define DIRTY_STARTUP*/
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

    /* Load the LUT */
    InitLUT();
}
