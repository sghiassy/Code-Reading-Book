/*
 * $XConsortium: copyword.s,v 1.2 91/12/11 21:37:24 eswu Exp $
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
 Copy Word Routines (2 to 8 words)

 Use Store-String-Immediate to write multiple words.
 This is faster because you only need one bus access
 to send several words.

 void copyword2(dst_address, w1, w2)
 void copyword3(dst_address, w1, w2, w3)
 ...
 void copyword8(dst_address, w1, ..., w8)
*/


	.file   "copyword2.s"
	.globl  copyword2[ds]
	.csect  copyword2[ds]
	.long   .copyword2[PR]
	.long   TOC[tc0]
	.long   0
	.toc
T.copyword2:     .tc     .copyword2[tc],copyword2[ds]
	.globl  .copyword2[PR]
	.csect  .copyword2[PR]
	stsi    4, 3, 8
	brl



	.file   "copyword3.s"
	.globl  copyword3[ds]
	.csect  copyword3[ds]
	.long   .copyword3[PR]
	.long   TOC[tc0]
	.long   0
	.toc
T.copyword3:     .tc     .copyword3[tc],copyword3[ds]
	.globl  .copyword3[PR]
	.csect  .copyword3[PR]
	stsi    4, 3, 12
	brl



	.file   "copyword4.s"
	.globl  copyword4[ds]
	.csect  copyword4[ds]
	.long   .copyword4[PR]
	.long   TOC[tc0]
	.long   0
	.toc
T.copyword4:     .tc     .copyword4[tc],copyword4[ds]
	.globl  .copyword4[PR]
	.csect  .copyword4[PR]
	stsi    4, 3, 16
	brl



	.file   "copyword5.s"
	.globl  copyword5[ds]
	.csect  copyword5[ds]
	.long   .copyword5[PR]
	.long   TOC[tc0]
	.long   0
	.toc
T.copyword5:     .tc     .copyword5[tc],copyword5[ds]
	.globl  .copyword5[PR]
	.csect  .copyword5[PR]
	stsi    4, 3, 20
	brl



	.file   "copyword6.s"
	.globl  copyword6[ds]
	.csect  copyword6[ds]
	.long   .copyword6[PR]
	.long   TOC[tc0]
	.long   0
	.toc
T.copyword6:     .tc     .copyword6[tc],copyword6[ds]
	.globl  .copyword6[PR]
	.csect  .copyword6[PR]
	stsi    4, 3, 24
	brl



	.file   "copyword7.s"
	.globl  copyword7[ds]
	.csect  copyword7[ds]
	.long   .copyword7[PR]
	.long   TOC[tc0]
	.long   0
	.toc
T.copyword7:     .tc     .copyword7[tc],copyword7[ds]
	.globl  .copyword7[PR]
	.csect  .copyword7[PR]
	stsi    4, 3, 28
	brl



	.file   "copyword8.s"
	.globl  copyword8[ds]
	.csect  copyword8[ds]
	.long   .copyword8[PR]
	.long   TOC[tc0]
	.long   0
	.toc
T.copyword8:     .tc     .copyword8[tc],copyword8[ds]
	.globl  .copyword8[PR]
	.csect  .copyword8[PR]
	stsi    4, 3, 32
	brl
