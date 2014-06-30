/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/tvga8900/bankc.c,v 3.2.2.1 1997/05/03 09:49:55 dawes Exp $ */
/*
 * Copyright 1997 by Alan Hourihane, Wigan, England.
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
 */

#define PSZ 8
#include "vga256.h"
void TVGA8900SetRead(int bank)
{
  outw(0x3c4, (((bank & 0xff) ^ 0x02)<<8)|0x0E);
}
void TGUISetRead(int bank)
{
  outb(0x3d9, bank & 0xff);
}
void TVGA8900SetWrite(int bank)
{
  outw(0x3c4, (((bank & 0xff) ^ 0x02)<<8)|0x0E);
}
void TGUISetWrite(int bank)
{
  outb(0x3d8, bank & 0xff);
}
void TVGA8900SetReadWrite(int bank)
{
  outw(0x3c4, (((bank & 0xff) ^ 0x02)<<8)|0x0E);
}
void TGUISetReadWrite(int bank)
{
  outw(0x3d8, (bank & 0xff)<< 8 | bank & 0xff);
}
