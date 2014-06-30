/*
 * Copyright 1997
 * Digital Equipment Corporation. All rights reserved.
 * This software is furnished under license and may be used and copied only in 
 * accordance with the following terms and conditions.  Subject to these 
 * conditions, you may download, copy, install, use, modify and distribute 
 * this software in source and/or binary form. No title or ownership is 
 * transferred hereby.
 * 1) Any source code used, modified or distributed must reproduce and retain 
 *    this copyright notice and list of conditions as they appear in the 
 *    source file.
 *
 * 2) No right is granted to use any trade name, trademark, or logo of Digital 
 *    Equipment Corporation. Neither the "Digital Equipment Corporation" name 
 *    nor any trademark or logo of Digital Equipment Corporation may be used 
 *    to endorse or promote products derived from this software without the 
 *    prior written permission of Digital Equipment Corporation.
 *
 * 3) This software is provided "AS-IS" and any express or implied warranties,
 *    including but not limited to, any implied warranties of merchantability,
 *    fitness for a particular purpose, or non-infringement are disclaimed. In
 *    no event shall DIGITAL be liable for any damages whatsoever, and in 
 *    particular, DIGITAL shall not be liable for special, indirect, 
 *    consequential, or incidental damages or damages for lost profits, loss 
 *    of revenue or loss of use, whether such damages arise in contract, 
 *    negligence, tort, under statute, in equity, at law or otherwise, even if
 *    advised of the possibility of such damage. 
 */
/*
 * The CyberPro2010 driver is based on sample code provided by IGS 
 * Technologies, Inc. http://www.integraphics.com.
 * IGS Technologies, Inc makes no representations about the suitability of 
 * this software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */
/*
** Common definitions for igs driver files
*/
#ifndef _IGS_DRIVER_H_
#define _IGS_DRIVER_H_

/*
** Register definitions
*/
#define PORT46E8   0x46E8
#define PORT102    0x102
#define MISCREAD   0x3CC
#define MISCWRITE  0x3C2
#define SEQINDEX   0x3C4
#define SEQDATA    0x3C5
#define ATTRINDEX  0x3C0
#define ATTRDATAW  0x3C0
#define ATTRDATAR  0x3C1 
#define GRAINDEX   0x3CE
#define GRADATA    0x3CF
#define EXTINDEX   0x3CE
#define EXTDATA    0x3CF
#define RAMDACINDEXR 0x3C7
#define RAMDACINDEXW 0x3C8
#define RAMDACDATA   0x3C9
#define RAMDACMASK   0x3C6

/* Memory mapping offsets */
#define IGS_PHYSMEM          0x800000
#define IGS_ASEG_XGABASE      0xAF000
#define IGS_BSEG_XGABASE      0xBF000
#define IGS_XGALEN            0xFFF

/* IGS specific variables */ 
extern unsigned int igsCursorAddress;
extern Bool igsLinearSupport;
extern unsigned int igsCacheEnd;
extern unsigned char *igsBltDataWindow;
extern volatile unsigned char *igsCopControl;
extern unsigned int igsColorExpandScratchAddr;
extern unsigned int igsColorExpandScratchSize;
/* Common functions */
extern void igsWaitVSync();

#define IGS_SYNC_HACK 
/* Sets the CopControl busy bit to 1 before the sync - without this
  some bits written to the frame buffer immediately after a blit 
  didn't appear on the screen - maybe we were checking the bit before
  it was set by the blitter */

/* #define DEBUG */

#endif /*_IGS_DRIVER_H_*/





