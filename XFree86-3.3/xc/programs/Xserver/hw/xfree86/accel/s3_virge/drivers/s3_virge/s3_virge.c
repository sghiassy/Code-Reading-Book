/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/drivers/s3_virge/s3_virge.c,v 3.2 1996/12/27 07:03:37 dawes Exp $ */
/*
 * Copyright 1993 by David Dawes <dawes@physics.su.oz.au>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of David Dawes 
 * not be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.
 * David Dawes makes no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * DAVID DAWES DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL DAVID DAWES BE LIABLE FOR 
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER 
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: s3_virge.c /main/3 1996/10/25 11:35:48 kaleb $ */

#include "s3v.h"

static Bool S3_VIRGEProbe();
static char *S3_VIRGEIdent();
extern void s3EnterLeaveVT();
extern Bool s3Initialize();
extern void s3AdjustFrame();
extern Bool s3SwitchMode();

s3VideoChipRec S3_VIRGE = {
  S3_VIRGEProbe,
  S3_VIRGEIdent,
  s3EnterLeaveVT,
  s3Initialize,
  s3AdjustFrame,
  s3SwitchMode,
};

static char *
S3_VIRGEIdent(n)
     int n;
{
   static char *chipsets[] = {"s3_ViRGE"};

   if (n + 1 > sizeof(chipsets) / sizeof(char *))
      return(NULL);
   else
      return(chipsets[n]);
}


static Bool
S3_VIRGEProbe()
{
   /*
    * We don't even get called unless the card is identified as S3, so just
    * return TRUE, unless the chipset is specified as something other than
    * "s3_ViRGE" in XF86Config.
    */

   if (s3InfoRec.chipset) {
      if (StrCaseCmp(s3InfoRec.chipset, S3_VIRGEIdent(0)))
	 return(FALSE);
      else
	 return(TRUE);
   }

   s3InfoRec.chipset = S3_VIRGEIdent(0);
   return(TRUE);
}
