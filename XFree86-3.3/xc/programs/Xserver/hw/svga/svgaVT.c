/* $XConsortium: svgaVT.c,v 1.3 93/09/20 12:07:43 rws Exp $ */
/*
 * Copyright 1990,91,92,93 by Thomas Roell, Germany.
 * Copyright 1991,92,93    by SGCS (Snitily Graphics Consulting Services), USA.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this  permission notice appear
 * in supporting documentation, and that the name of Thomas Roell nor
 * SGCS be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * Thomas Roell nor SGCS makes no representations about the suitability
 * of this software for any purpose. It is provided "as is" without
 * express or implied warranty.
 *
 * THOMAS ROELL AND SGCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THOMAS ROELL OR SGCS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "svga.h"
#include "gcstruct.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "servermd.h"
#include <sys/kd.h>
#include <sys/vt.h>

extern WindowPtr *WindowTable;

Bool svgaVTRequestsPending = FALSE;
Bool svgaVTActive = TRUE;

/*
 * svgaVTRequest --
 *      Notice switch requests form the vt manager.
 */

/* ARGSUSED */
void
svgaVTRequest(
    int signo
)
{
  svgaVTRequestsPending = TRUE;
}

static Bool
svgaScreenSave(
    ScreenPtr pScreen
)
{
  PixmapPtr pPixmap = (PixmapPtr)pScreen->devPrivate;
  pointer   pImage;

  pImage = (pointer)Xalloc(pPixmap->devKind * pPixmap->drawable.height);

  if (!pImage) return FALSE;

  (*pScreen->GetImage)((DrawablePtr)pPixmap,
                       0, 0, 
                       pScreen->width,
                       pScreen->height,
                       ZPixmap,
                       ~0L,
                       pImage);

  (*pScreen->ModifyPixmapHeader)(pPixmap,
                                 pPixmap->drawable.width,
                                 pPixmap->drawable.height,
                                 pPixmap->drawable.depth,
                                 pPixmap->drawable.bitsPerPixel,
                                 pPixmap->devKind,
                                 pImage);

  pPixmap->drawable.class = 0;
  pPixmap->drawable.serialNumber = NEXT_SERIAL_NUMBER;

  return TRUE;
}

static Bool
svgaScreenRestore(
    ScreenPtr pScreen
)
{
  PixmapPtr   pPixmap = (PixmapPtr)pScreen->devPrivate;
  DrawablePtr pDrawable = (DrawablePtr)WindowTable[pScreen->myNum];
  GCPtr       pGC;
  pointer     pImage;
  XID         subWindowMode = IncludeInferiors;

  pPixmap->drawable.class = 1;
  pPixmap->drawable.serialNumber = NEXT_SERIAL_NUMBER;

  pImage = pPixmap->devPrivate.ptr;

  (*pScreen->ModifyPixmapHeader)(pPixmap,
                                 pPixmap->drawable.width,
                                 pPixmap->drawable.height,
                                 pPixmap->drawable.depth,
                                 pPixmap->drawable.bitsPerPixel,
                                 pPixmap->devKind,
				 NULL);

  pGC = GetScratchGC(pDrawable->depth, pScreen);

  ChangeGC(pGC, GCSubwindowMode, &subWindowMode);

  ValidateGC(pDrawable, pGC);

  (*pGC->ops->PutImage)(pDrawable, pGC,
                        pDrawable->depth,
                        0, 0, 
                        pScreen->width,
                        pScreen->height,
                        0,
                        ZPixmap,
                        pImage);

  Xfree(pImage);

  FreeScratchGC(pGC);

  return TRUE;
}

/* ARGSUSED */
static int
svgaChangeSerialNumber(
    WindowPtr pWindow,
    pointer   data
)
{
  pWindow->drawable.serialNumber = NEXT_SERIAL_NUMBER;
  return WT_WALKCHILDREN;
}


/*
 * svgaVTSwitch --
 *      Handle requests for switching the vt.
 */

void
svgaVTSwitch()
{
  if (svgaVTActive) {

    if (!svgaScreenSave(screenInfo.screens[0])) return;

    DisableDevice((DeviceIntPtr)LookupKeyboardDevice());
    DisableDevice((DeviceIntPtr)LookupPointerDevice());
      
    if (ioctl(svgaConsoleFd, VT_RELDISP, 1) < 0) {

      /*
       * switch failed 
       */
      if (!svgaScreenRestore(screenInfo.screens[0])) {
	ErrorF("Couldn't restore screen\n");
	GiveUp(0);
      }

      EnableDevice((DeviceIntPtr)LookupKeyboardDevice());
      EnableDevice((DeviceIntPtr)LookupPointerDevice());

    } else {

      WalkTree(screenInfo.screens[0], svgaChangeSerialNumber, NULL);

      svgaSVPMISetText(screenInfo.screens[0]);

      svgaVTActive = FALSE;
    }
  } else {

    if (ioctl(svgaConsoleFd, VT_RELDISP, VT_ACKACQ) < 0) return;

    svgaVTActive = TRUE;

    svgaSVPMISetGraphics(screenInfo.screens[0]);
    (void)svgaSVGABlankScreen(screenInfo.screens[0], SCREEN_SAVER_OFF);

    WalkTree(screenInfo.screens[0], svgaChangeSerialNumber, NULL);

    if (!svgaScreenRestore(screenInfo.screens[0])) {
      ErrorF("Couldn't restore screen\n");
      GiveUp(0);
    }

    EnableDevice((DeviceIntPtr)LookupKeyboardDevice());
    EnableDevice((DeviceIntPtr)LookupPointerDevice());
  }

  svgaVTRequestsPending = FALSE;
}
