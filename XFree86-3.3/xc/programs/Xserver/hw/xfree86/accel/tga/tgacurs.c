/* $XConsortium: tgacurs.c /main/4 1996/10/27 11:47:36 kaleb $ */
/*
 * Copyright 1991 MIPS Computer Systems, Inc.
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of MIPS not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  MIPS makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 * 
 * MIPS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL MIPS
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/tga/tgacurs.c,v 3.3 1996/12/27 07:03:48 dawes Exp $ */

/*
 * Modified by Amancio Hasty and Jon Tombs and Erik Nygren
 * 
 * Modified by Alan Hourihane for TGA <alanh@fairlite.demon.co.uk>
 */

/*
 * Device independent (?) part of HW cursor support
 */

#include <signal.h>

#define NEED_EVENTS
#include <X.h>
#include "Xproto.h"
#include <misc.h>
#include <input.h>
#include <cursorstr.h>
#include <regionstr.h>
#include <scrnintstr.h>
#include <servermd.h>
#include <windowstr.h>
#include "xf86.h"
#include "inputstr.h"
#include "mipointer.h"
#include "mfb.h"
#include "xf86Priv.h"
#include "xf86_Option.h"
#include "xf86_OSlib.h"
#include "tga.h"
#include "tgacurs.h"

static void tgaVerticalRetraceWait(
#if NeedFunctionPrototypes
   void
#endif
);

static Bool tgaUnrealizeCursor(
#if NeedFunctionPrototypes
   ScreenPtr, CursorPtr
#endif
);

static void tgaSetCursor(
#if NeedFunctionPrototypes
   ScreenPtr, CursorPtr, int, int
#endif
);

extern Bool tgaBtRealizeCursor(
#if NeedFunctionPrototypes
    ScreenPtr, CursorPtr
#endif
);

#if 0
extern Bool tgaDECRealizeCursor(
#if NeedFunctionPrototypes
    ScreenPtr, CursorPtr
#endif
);
#endif

extern void tgaBtCursorOn(
#if NeedFunctionPrototypes
   void
#endif
);

extern void tgaBtCursorOff(
#if NeedFunctionPrototypes
   void
#endif
);

extern void tgaBtLoadCursor(
#if NeedFunctionPrototypes
   ScreenPtr, CursorPtr, int, int
#endif
);

extern void tgaBtMoveCursor(
#if NeedFunctionPrototypes
   ScreenPtr, int, int
#endif
);

#if 0
extern void tgaDECMoveCursor(
#if NeedFunctionPrototypes
   ScreenPtr, int, int
#endif
);
#endif

#if 0
static miPointerSpriteFuncRec tgaDECPointerSpriteFuncs =
{
   tgaDECRealizeCursor,
   tgaUnrealizeCursor,
   tgaSetCursor,
   tgaDECMoveCursor,
};
#endif

static miPointerSpriteFuncRec tgaBtPointerSpriteFuncs =
{
   tgaBtRealizeCursor,
   tgaUnrealizeCursor,
   tgaSetCursor,
   tgaBtMoveCursor,
};

extern miPointerScreenFuncRec xf86PointerScreenFuncs;
extern xf86InfoRec xf86Info;
extern unsigned char tgaSwapBits[256];

static int tgaCursGeneration = -1;
static CursorPtr tgaSaveCursors[MAXSCREENS];

extern int tgahotX, tgahotY;

/*
 * tgaVerticalRetraceWait --
 *     Polls until the vertical retrace is entered
 */
void
tgaVerticalRetraceWait()
{
#if 0
  unsigned long vctr, last_vctr; /* Current line position in vertical sweep */
  for (vctr = last_vctr = tgaFetch(VRTC,CtlBase);  /* Prime the loop */
       vctr >= last_vctr;
       last_vctr = vctr, vctr = tgaFetch(VRTC,CtlBase));
#endif
}

Bool
tgaCursorInit(pm, pScr)
     char *pm;
     ScreenPtr pScr;
{
   tgaBlockCursor = FALSE;
   tgaReloadCursor = FALSE;

   if (tgaCursGeneration != serverGeneration) {
     tgahotX = 0;
     tgahotY = 0;
     if (OFLG_ISSET(OPTION_BT485_CURS, &tgaInfoRec.options))
     {
	if (!(miPointerInitialize(pScr, &tgaBtPointerSpriteFuncs,
			       &xf86PointerScreenFuncs, FALSE)))
	return FALSE;
	pScr->RecolorCursor = tgaBtRecolorCursor;
     }
#if 0
     else
     if (OFLG_ISSET(OPTION_HW_CURSOR, &tgaInfoRec.options))
     {
	if (!(miPointerInitialize(pScr, &tgaDECPointerSpriteFuncs,
				&xf86PointerScreenFuncs, FALSE)))
	return FALSE;
     }
#endif
     tgaCursGeneration = serverGeneration;
   }
   
   return TRUE;
}

void
tgaShowCursor()
{
  if (OFLG_ISSET(OPTION_BT485_CURS, &tgaInfoRec.options))
  	tgaBtCursorOn();
#if 0
  else
  if (OFLG_ISSET(OPTION_HW_CURSOR, &tgaInfoRec.options))
	tgaDECCursorOn();
#endif
}

void
tgaHideCursor()
{
  if (OFLG_ISSET(OPTION_BT485_CURS, &tgaInfoRec.options))
  	tgaBtCursorOff();
#if 0
  else
  if (OFLG_ISSET(OPTION_HW_CURSOR, &tgaInfoRec.options))
	tgaDECCursorOff();
#endif
}

static Bool
tgaUnrealizeCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   pointer priv;

   if (pCurs->bits->refcnt <= 1 &&
       (priv = pCurs->bits->devPriv[pScr->myNum]))
      xfree(priv);
   return TRUE;
}

static void
tgaSetCursor(pScr, pCurs, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int   x, y;
{
   int index = pScr->myNum;

   if (!pCurs)
      return;

   tgahotX = pCurs->bits->xhot;
   tgahotY = pCurs->bits->yhot;
   tgaSaveCursors[index] = pCurs;

   if (!tgaBlockCursor) {
     if (OFLG_ISSET(OPTION_BT485_CURS, &tgaInfoRec.options))
     	tgaBtLoadCursor(pScr, pCurs, x, y);
#if 0
     else
     if (OFLG_ISSET(OPTION_HW_CURSOR, &tgaInfoRec.options))
	tgaDECLoadCursor(pScr, pCurs, x , y);
#endif
   } else
      tgaReloadCursor = TRUE;
}

void
tgaRestoreCursor(pScr)
     ScreenPtr pScr;
{
  int index = pScr->myNum;
  int x, y;
  
  tgaReloadCursor = FALSE;
  miPointerPosition(&x, &y);
  if (OFLG_ISSET(OPTION_BT485_CURS, &tgaInfoRec.options))
  	tgaBtLoadCursor(pScr, tgaSaveCursors[index], x, y);
#if 0
  else
  if (OFLG_ISSET(OPTION_HW_CURSOR, &tgaInfoRec.options))
	tgaDECLoadCursor(pScr, tgaSaveCursors[index], x, y);
#endif
}

void
tgaRepositionCursor(pScr)
     ScreenPtr pScr;
{
   int x, y;
  
   miPointerPosition(&x, &y);
   if (OFLG_ISSET(OPTION_BT485_CURS, &tgaInfoRec.options))
   	tgaBtMoveCursor(pScr, x, y);
#if 0
   else
   if (OFLG_ISSET(OPTION_HW_CURSOR, &tgaInfoRec.options))
	tgaDECMoveCursor(pScr, x, y);
#endif
}

void
tgaWarpCursor(pScr, x, y)
     ScreenPtr pScr;
     int   x, y;
{
   if (xf86VTSema) {
#if 0
      /* Wait for vertical retrace */
      tgaVerticalRetraceWait();
#endif
   }
   miPointerWarpCursor(pScr, x, y);
   xf86Info.currentScreen = pScr;
}

void 
tgaQueryBestSize(class, pwidth, pheight, pScreen)
     int class;
     unsigned short *pwidth;
     unsigned short *pheight;
     ScreenPtr pScreen;
{
   if (*pwidth > 0) {
      switch (class) {
         case CursorShape:
	    *pwidth = 64;
	    *pheight = 64;
	    break;
         default:
	    (void) mfbQueryBestSize(class, pwidth, pheight, pScreen);
	    break;
      }
   }
}
