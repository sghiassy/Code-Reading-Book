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
/* BitBlt option codes */
#define  igsMaskMapDisabled	0x00000000
#define  igsMaskMapBoundEnabled 0x00000040    
#define  igsMaskMapEnabled	0x00000080    
#define  igsAllPixel		0x00000000
#define  igsMaskAreaBound	0x00000030    
#define  igsLRDown		0x00000000 
#define  igsLRUp                0x00000002
#define  igsRLDown              0x00000004   
#define  igsRLUp                0x00000006
#define  igsSrcPxMapA		0x00100000
#define  igsDestPxMapA		0x00010000
#define  igsPatternPxMapA	0x00001000
#define  igsPatternPxMapB	0x00002000
#define  igsPatternPxMapC	0x00003000
#define  igsPatternForeground	0x00008000   
#define  igsPatternFromSrc	0x00009000   
#define  igsSrcPxMapB		0x00200000
#define  igsDestPxMapB		0x00020000
#define  igsSrcPxMapC		0x00300000
#define  igsDestPxMapC		0x00030000
#define  igsForeSrcColor 	0x00000000 /* Src is a colour (solid fill) */
#define  igsBackSrcColor 	0x00000000 
#define  igsForeSrcPxMap 	0x20000000 /* Src is a pixmap (copy) */
#define  igsBackSrcPxMap 	0x80000000
#define  igsPxBlt		0x08000000
#define  igsPxBltInvert		0x09000000
#define  igsPxBltAreaFill	0x0A000000
#define  igsPxBltTern           0x0B000000
#define  igsStepDrawRead 	0x02000000
#define  igsLineDrawRead 	0x03000000
#define  igsStepDrawWrite	0x04000000
#define  igsLineDrawWrite	0x05000000

#define  igsTextOutOpq		0x00001000
#define  igsPattTilOpq		0x00002000
#define  igsStyleLineOpq	0x00003000
#define  igsTextOutTra		0x00005000
#define  igsPattTilTra		0x00006000
#define  igsStyleLineTra	0x00007000
#define  igsFixFgPat            0x00008000
#define  igsSrcTilOpq		0x00009000
#define  igsSrcTilTra		0x0000D000
#define  igsHostBltWrite	0x00020000
#define  igsHostBltRead		0x00010000
#define  igsHostBltWrSrc2       0x00030000
#define  igsColexpOpq           0x00100000
#define  igsColexpTra           0x00500000

#define  igsPosDX		0x00000000
#define  igsNegDX	        0x00000004
#define  igsPosDY		0x00000000
#define  igsNegDY		0x00000002
#define  igsPosDZ		0x00000000
#define  igsNegDZ		0x00000001

#define igsPatCopy              0xF0

/* Colour Compare Values */
#define  igsCompareEq           0x00000002
#define  igsCompareNeq          0x00000006
#define  igsDontCompare         0x00000004

/* Text codes */
#define IGS_TXT_CHAR            0x04000000
#define IGS_TXT_EOL             0x08000000
#define IGS_TXT_EOD             0x0f000000
#define IGS_TXT_WIDTH           0x000f0000

/* IGS graphics engine registers */

extern int igsXGABase;
#define  igsPixMapIndex	 (unsigned char  *) (igsXGABase+0x0012)
#define  igsPixMapBase	 (unsigned long  *) (igsXGABase+0x0014)
#define  igsPixMapWidth   (unsigned short   *) (igsXGABase+0x0018)
#define  igsPixMapHeight  (unsigned short   *) (igsXGABase+0x001A)
#define  igsPixMapFormat  (unsigned char  *) (igsXGABase+0x001C)
#define  igsBresErrTerm   (unsigned short   *) (igsXGABase+0x0020)
#define  igsBresK1     	 (unsigned short   *) (igsXGABase+0x0024)
#define  igsBresK2      	 (unsigned short   *) (igsXGABase+0x0028)
#define  igsForeMix     	 (unsigned char  *) (igsXGABase+0x0048)
#define  igsBackMix     	 (unsigned char  *) (igsXGABase+0x0049)
#define  igsDestColorComp  (unsigned char  *) (igsXGABase+0x004A)
#define  igsColorCompVal  (unsigned long  *) (igsXGABase+0x004C)
#define  igsPxBitMask	 (unsigned long  *) (igsXGABase+0x0050)
#define  igsForeColor	 (unsigned long  *) (igsXGABase+0x0058)
#define  igsBackColor	 (unsigned long  *) (igsXGABase+0x005C)
#define  igsOpDimension1  (unsigned short   *) (igsXGABase+0x0060)
#define  igsOpDimension2  (unsigned short   *) (igsXGABase+0x0062)
#define  igsSrcBasePtr    (unsigned long  *) (igsXGABase+0x0070)
#define  igsDstXPhase     (unsigned char  *) (igsXGABase+0x0078)
#define  igsDstYPhase     (unsigned char  *) (igsXGABase+0x007A)
#define  igsPatXPhase	  (unsigned char  *) (igsXGABase+0x0150)
#define  igsPatYPhase	  (unsigned char  *) (igsXGABase+0x007A)
#define  igsMonoMapByteCount (unsigned short *) (igsXGABase+0x013C)
#define  igsSrc2MapWidth  (unsigned long  *) (igsXGABase+0x0118) 
#define  igsSrc2StartPtr  (unsigned long  *) (igsXGABase+0x0174)
#define  igsSrcStartPtr   (unsigned long  *) (igsXGABase+0x0170)
#define  igsDstStartPtr   (unsigned long  *) (igsXGABase+0x0178)
#define  igsDstMapWidth   (unsigned short   *) (igsXGABase+0x0218)

#define  igsPxOp       (unsigned long  *) (igsXGABase+0x007C)
#define  igsPxOpL      (unsigned short   *) (igsXGABase+0x007C)
#define  igsPxOpH      (unsigned short   *) (igsXGABase+0x007C+0x2)



