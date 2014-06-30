/* $XConsortium: newmmio.c /main/5 1996/10/24 07:11:34 kaleb $ */




/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/drivers/newmmio/newmmio.c,v 3.5.2.1 1997/05/06 13:26:23 dawes Exp $ */
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


#include "s3.h"
#include "regs3.h"

static Bool NEWMMIO_Probe();
static char *NEWMMIO_Ident();
extern void newmmio_s3EnterLeaveVT();
extern Bool newmmio_s3Initialize();
extern void newmmio_s3AdjustFrame();
extern Bool newmmio_s3SwitchMode();
extern Bool s3NewMmio;

/*
 * If 'volatile' isn't available, we disable the MMIO code 
 */

#if defined(__STDC__) || defined(__GNUC__)
#define HAVE_VOLATILE
#endif

s3VideoChipRec NEWMMIO = {
  NEWMMIO_Probe,
  NEWMMIO_Ident,
#ifdef HAVE_VOLATILE
  newmmio_s3EnterLeaveVT,
  newmmio_s3Initialize,
  newmmio_s3AdjustFrame,
  newmmio_s3SwitchMode,
#else
  NULL,
  NULL,
  NULL,
  NULL,
#endif
};

static char *
NEWMMIO_Ident(n)
     int n;
{
#ifdef HAVE_VOLATILE
   static char *chipsets[] = {"newmmio"};

   if (n + 1 > sizeof(chipsets) / sizeof(char *))
      return(NULL);
   else
      return(chipsets[n]);
#else
   return(NULL);
#endif
}


static Bool
NEWMMIO_Probe()
{
#ifdef HAVE_VOLATILE
   if (s3InfoRec.chipset) {
      if (StrCaseCmp(s3InfoRec.chipset, NEWMMIO_Ident(0)))
	 return(FALSE);
      else {
	 /* don't allow "newmmio" for S3 chips which don't support it 
	    even when specified in the config file, so fall through
	    for more sanity checks... */
      }
   }

   if ((S3_x68_SERIES(s3ChipId) ||
       S3_TRIO64V_SERIES(s3ChipId) ||
       S3_TRIO64V2_SERIES(s3ChipId))
       && xf86LinearVidMem()
       && !OFLG_ISSET(OPTION_NOLINEAR_MODE, &s3InfoRec.options)) {
      s3InfoRec.chipset = NEWMMIO_Ident(0);
      s3NewMmio = TRUE;
      return(TRUE);
   } else {
      return(FALSE);
   }
#else
   return(FALSE);
#endif
}
