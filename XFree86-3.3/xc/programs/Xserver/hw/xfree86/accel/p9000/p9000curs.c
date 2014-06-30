/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000curs.c,v 3.5.2.1 1997/05/31 13:34:40 dawes Exp $ */
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

/*
 * Modified by Amancio Hasty and Jon Tombs and Erik Nygren
 * 
 */
/* $XConsortium: p9000curs.c /main/5 1996/02/21 17:32:14 kaleb $ */

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
#include "p9000.h"
#include "p9000reg.h"
#include "p9000curs.h"


static void p9000VerticalRetraceWait(
#if NeedFunctionPrototypes
   void
#endif
);

static Bool p9000UnrealizeCursor(
#if NeedFunctionPrototypes
   ScreenPtr, CursorPtr
#endif
);

static void p9000SetCursor(
#if NeedFunctionPrototypes
   ScreenPtr, CursorPtr, int, int
#endif
);

extern Bool p9000BtRealizeCursor(
#if NeedFunctionPrototypes
    ScreenPtr, CursorPtr
#endif
);

extern void p9000BtCursorOn(
#if NeedFunctionPrototypes
   void
#endif
);

extern void p9000BtCursorOff(
#if NeedFunctionPrototypes
   void
#endif
);

extern void p9000BtLoadCursor(
#if NeedFunctionPrototypes
   ScreenPtr, CursorPtr, int, int
#endif
);

extern void p9000BtMoveCursor(
#if NeedFunctionPrototypes
   ScreenPtr, int, int
#endif
);



static miPointerSpriteFuncRec p9000BtPointerSpriteFuncs =
{
   p9000BtRealizeCursor,
   p9000UnrealizeCursor,
   p9000SetCursor,
   p9000BtMoveCursor,
};

extern miPointerScreenFuncRec xf86PointerScreenFuncs;
extern xf86InfoRec xf86Info;
extern unsigned char p9000SwapBits[256];

static int p9000CursGeneration = -1;
static CursorPtr p9000SaveCursors[MAXSCREENS];

extern int p9000hotX, p9000hotY;

/*
 * p9000VerticalRetraceWait --
 *     Polls until the vertical retrace is entered
 */
void
p9000VerticalRetraceWait()
{
  unsigned long vctr, last_vctr; /* Current line position in vertical sweep */
  for (vctr = last_vctr = p9000Fetch(VRTC,CtlBase);  /* Prime the loop */
       vctr >= last_vctr;
       last_vctr = vctr, vctr = p9000Fetch(VRTC,CtlBase));
}

Bool
p9000CursorInit(pm, pScr)
     char *pm;
     ScreenPtr pScr;
{
   p9000BlockCursor = FALSE;
   p9000ReloadCursor = FALSE;

   if (p9000CursGeneration != serverGeneration) {
     if (!(miPointerInitialize(pScr, &p9000BtPointerSpriteFuncs,
			       &xf86PointerScreenFuncs, FALSE)))
       return FALSE;

     p9000hotX = 0;
     p9000hotY = 0;
     pScr->RecolorCursor = p9000BtRecolorCursor;
     p9000CursGeneration = serverGeneration;
   }
   
   return TRUE;
}

void
p9000ShowCursor()
{
  p9000BtCursorOn();
}

void
p9000HideCursor()
{
  p9000BtCursorOff();
}

static Bool
p9000UnrealizeCursor(pScr, pCurs)
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
p9000SetCursor(pScr, pCurs, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int   x, y;
{
   int index = pScr->myNum;

   if (!pCurs)
      return;

   p9000hotX = pCurs->bits->xhot;
   p9000hotY = pCurs->bits->yhot;
   p9000SaveCursors[index] = pCurs;

   if (!p9000BlockCursor) {
     p9000BtLoadCursor(pScr, pCurs, x, y);
   } else
      p9000ReloadCursor = TRUE;
}

void
p9000RestoreCursor(pScr)
     ScreenPtr pScr;
{
  int index = pScr->myNum;
  int x, y;
  
  p9000ReloadCursor = FALSE;
  miPointerPosition(&x, &y);
  p9000BtLoadCursor(pScr, p9000SaveCursors[index], x, y);
}

void
p9000RepositionCursor(pScr)
     ScreenPtr pScr;
{
   int x, y;
  
   miPointerPosition(&x, &y);
   p9000BtMoveCursor(pScr, x, y);
}

void
p9000WarpCursor(pScr, x, y)
     ScreenPtr pScr;
     int   x, y;
{
   if (xf86VTSema) {
      /* Wait for vertical retrace */
      p9000VerticalRetraceWait();
   }
   miPointerWarpCursor(pScr, x, y);
   xf86Info.currentScreen = pScr;
}

void 
p9000QueryBestSize(class, pwidth, pheight, pScreen)
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
