/*
 * $XConsortium: skyHdwr.c /main/7 1995/12/05 15:44:46 matt $ 
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991 
 *
 * All Rights Reserved 
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of IBM not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS, IN NO EVENT SHALL IBM BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 */

/*
 * skyHdwr.c - initialize hardware registers 
 */

#include <sys/types.h>
#ifndef _IBM_LFT
#include <sys/hft.h>
#endif
#include <sys/entdisp.h>

#include "X.h"
#include "screenint.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "cursorstr.h"
#include "ibmScreen.h"
#include "ibmTrace.h"

#include "skyHdwr.h"
#include "skyReg.h"


int 
skyHdwrInit(index)
    int             index;
{
    TRACE(("skyHdwrInit\n"));

    SKY_MODE_REG(index)    = 0x0c;	/* native motorola mode */
    SKY_MEM_REG(index)     = 0x0b;	/* Motorola order, 8 bits/pixel */
    SKY_WINCTRL_REG(index) = 0;		/* No pc window */

    /* Setup coprocessor to known state */

    SKYBusyWait(index);
    SKYSyncCoprocessorShadow(index);
    SKYSetupScreenPix(index, PixSize8);
    SKYTurnOffColorComp(index);


    /* Clear Screen */

    SKYSetALU(index, GXcopy);
    SKYSetPlaneMask(index, 0xff);
    SKYSetFgColor(index, 0);
    SKYSetOpDim21(index, 1280, 1024);

    SKYSetDstXY_PixelOp(index,
	0, 0,						/* Dst X,Y    */
	POForeReg | POStepBlt | PODestC | POPatFore |	/* Pixel Op   */
	POMaskDis | POModeAll | POOct0);

    SKYBusyWait(index);


    /* Set the io register pointers					 */

    SKY_SINDEX_REG(index) = 0x5600;	/* disable the cursor in the DAC   */
    SKY_SINDEX_REG(index) = 0x6c04;
    SKY_SINDEX_REG(index) = 0x5061;	/* set CRTC to prepare for reset  */
    SKY_SINDEX_REG(index) = 0x5060;	/* set CRTC to reset              */

    SKY_INT_REG(index) = 0x00;
    SKY_INS_REG(index) = 0xff;	/* clear all interrupt status bits */
    SKY_VMC_REG(index) = 0x00;	/* disable virtual memory interrupts */
    SKY_VMS_REG(index) = 0xff;	/* clear virtual memory interrupts */



    /*
     * set the memory configuration register to 64 bit serializer width, 256K
     * x 4 module type, and 64 bit physical width for 64 bit wide serializer
     */

    SKY_SINDEX_REG(index) = 0x009e;

    /* initialize the DAC                                                */

    SKY_SINDEX_REG(index) = 0x6006;	/* Put '01000011'b into Brooktree     */
    SKY_SINDEX_REG(index) = 0x6443;	/* command register to initialize it. */
    SKY_SINDEX_REG(index) = 0x6004;	/* Turn on all bits in the read mask. */
    SKY_SINDEX_REG(index) = 0x64FF;
    SKY_SINDEX_REG(index) = 0x6005;	/* 60  INDX REG = 05 */
    SKY_SINDEX_REG(index) = 0x6400;	/* 64  BLNK MSK = 00 */
    SKY_SINDEX_REG(index) = 0x6007;	/* 60  INDX REG = 07 */
    SKY_SINDEX_REG(index) = 0x6400;	/* 64  TEST REG = 00 */

    /*
     * The following values are taken from the Addendum to the SKYWAY 1 Video
     * Subsystem Hardware Workbook dated October 3, 1988.    However 
     */
    /* some values are taken from swstew.c, and don't match the addendum */

    SKY_SINDEX_REG(index) = 0x5402;	/* Clock Frequency must be set to
					 * 0x02 */
    SKY_SINDEX_REG(index) = 0x3600;	/* turn off the sprite control
					 * register */
    SKY_SINDEX_REG(index) = 0x6400;	/* use Brooktree Palette DAC control  */
				        /* register to turn off palette mask  */
    SKY_SINDEX_REG(index) = 0x5103;	/* 8 bits/pixel, x1 scaling factors   */
    /* pass two start */
    SKY_SINDEX_REG(index) = 0x0102;	/* Set 0.25 clock increment on        */
    SKY_SINDEX_REG(index) = 0x16db;
    SKY_SINDEX_REG(index) = 0x18ac;
    SKY_SINDEX_REG(index) = 0x1a93;
    SKY_SINDEX_REG(index) = 0x1cc5;
    SKY_SINDEX_REG(index) = 0x1e06;
    SKY_SINDEX_REG(index) = 0x2a04;
    /* pass two end */

    SKY_SINDEX_REG(index) = 0x10db;	/* 1760 pixels per scan line          */
    SKY_SINDEX_REG(index) = 0x129f;	/* 1280 pixels in active picture area */
    SKY_SINDEX_REG(index) = 0x149f;	/* 1280 is end of picture border area */
    SKY_SINDEX_REG(index) = 0x201f;	/* vertical total regs set to 0x41f,  */
    SKY_SINDEX_REG(index) = 0x2104;	/* which is 1056 lines                */
    SKY_SINDEX_REG(index) = 0x22ff;	/* vertical display end registers are */
    SKY_SINDEX_REG(index) = 0x2303;	/* set to 0x3ff, which is 1024 lines  */
    SKY_SINDEX_REG(index) = 0x241f;	/* vertical blanking start registers  */
    SKY_SINDEX_REG(index) = 0x2504;	/* are set to 0x041f                  */
    SKY_SINDEX_REG(index) = 0x26ff;	/* vertical blanking end registers
					 * are */
    SKY_SINDEX_REG(index) = 0x2703;	/* set to 0x03ff                      */
    SKY_SINDEX_REG(index) = 0x2801;	/* vertical sync pulse start
					 * registers */
    SKY_SINDEX_REG(index) = 0x2904;	/* are set to 0x0401                  */
    SKY_SINDEX_REG(index) = 0x4000;	/* set the Start Address registers to */
    SKY_SINDEX_REG(index) = 0x4100;	/* define the start address of the    */
    SKY_SINDEX_REG(index) = 0x4200;	/* active picture to address 0        */
    SKY_SINDEX_REG(index) = 0x43a0;	/* buffer pitch registers set to 1280 */
    SKY_SINDEX_REG(index) = 0x4400;	/* pixels per scan line               */
    SKY_SINDEX_REG(index) = 0x64ff;	/* turn on palette mask               */
    SKY_SINDEX_REG(index) = 0x2cff;	/* vertical line compare lo           */
    SKY_SINDEX_REG(index) = 0x2d07;	/* vertical line compare hi           */
    SKY_SINDEX_REG(index) = 0x5063;	/* set Display Mode 1 register to:    */
					/* Normal operation                */
					/* No clock error                  */
					/* Non interlaced                  */
					/* Video Feature Interface disabled */
					/* Composite syncs enabled         */
					/* + Vertical, - Horizontal        */
					/* sync polarity                */
    return (0);
}

