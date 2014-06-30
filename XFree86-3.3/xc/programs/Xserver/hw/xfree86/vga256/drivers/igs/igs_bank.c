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

#ifdef	__arm32__
/*#include <machine/sysarch.h>*/
#define	arm32_drain_writebuf()	sysarch(1, 0)

static int igsReadBank = -1;
static int igsWriteBank = -1;
#endif


void IGSSetRead(int bank)
{
    outw(0x3CE, (((bank & 0x3F) << 8) | 0x32));

#ifdef	__arm32__
    /* Must drain StrongARM write buffer on bank switch! */
    if (bank != igsReadBank) {
	arm32_drain_writebuf();
	igsReadBank = bank;
    }
#endif
}


void IGSSetWrite(int bank)
{
    outw(0x3CE, (((bank & 0x3F) << 8) | 0x31));

#ifdef	__arm32__
    /* Must drain StrongARM write buffer on bank switch! */
    if (bank != igsWriteBank) {
	arm32_drain_writebuf();
	igsWriteBank = bank;
    }
#endif
}


void IGSSetReadWrite(int bank)
{
    outw(0x3CE, (((bank & 0x3F) << 8) | 0x31));

#ifdef	__arm32__
    /* Must drain StrongARM write buffer on bank switch! */
    if (bank != igsWriteBank) {
	arm32_drain_writebuf();
	igsWriteBank = bank;
    }
#endif
}
