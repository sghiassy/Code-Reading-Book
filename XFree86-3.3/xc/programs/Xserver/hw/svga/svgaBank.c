/* $XConsortium: svgaBank.c,v 1.8 94/01/07 09:53:07 dpw Exp $ */
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
#include "cfb.h"
#include "cfbmskbits.h"
#include "mi.h"
#include "mibstore.h"
#include "regionstr.h"
#include "dixfontstr.h"
#include "fontstruct.h"
#include "windowstr.h"
#include "gcstruct.h"
#include "servermd.h"

#include <fcntl.h>
#include <sys/mman.h>

/*
 * This thing originated from an idea of Edwin Goei and his bank switching
 * code for the DEC TX board.
 */

extern void QueryGlyphExtents();
extern WindowPtr *WindowTable;

typedef struct _svgaBankScreen *svgaBankScreenPtr;
typedef struct _svgaBankGC     *svgaBankGCPtr;

#define MAX_BANKS 64

#define BANK_SINGLE  0
#define BANK_SHARED  1
#define BANK_DOUBLE  2

#define WINDOW_SUPPORTED        0x01
#define WINDOW_READABLE         0x02
#define WINDOW_WRITEABLE        0x04

typedef struct _svgaBankScreen {
  int           type;
  int		winSize;
  int           winShift;
  int           srcIndex;
  int           dstIndex;
  unchar        *srcBase;
  unchar        *dstBase;

  int		numBanks;
  RegionPtr     pBanks[MAX_BANKS];

  void          (*SetWindow)(
		  ScreenPtr pScreen,
                  int       position,
                  int       windowNo
                );

  /*
   * Screen Wrappers
   */
  Bool		(*CloseScreen)();
  void		(*GetImage)();
  void		(*GetSpans)();
  Bool		(*CreateGC)();
  void		(*PaintWindowBackground)();
  void		(*PaintWindowBorder)();
  void		(*CopyWindow)();
} svgaBankScreenRec;

typedef struct _svgaBankGC {
  GCOps         *wrapOps;
  GCFuncs	*wrapFuncs;
  RegionPtr     pBankedClips[MAX_BANKS];
} svgaBankGCRec;

typedef struct {
  ushort srcBankNo;
  ushort dstBankNo;
  short  x1;
  short  y1;
  short  x2;
  short  y2;
} svgaBlitQueue;


#define SET_SINGLE_BANK(pDrawable, no) {				\
  (*pScreenPriv->SetWindow)(((DrawablePtr)pDrawable)->pScreen,          \
                            (no) << pScreenPriv->winShift,		\
			    pScreenPriv->srcIndex);			\
  if (pScreenPriv->type == BANK_SHARED) 				\
    (*pScreenPriv->SetWindow)(((DrawablePtr)pDrawable)->pScreen,        \
                              (no) << pScreenPriv->winShift,		\
			      pScreenPriv->dstIndex);			\
  cfbGetWindowPixmap((DrawablePtr)pDrawable)->devPrivate.ptr 	=	\
    pScreenPriv->srcBase - (pScreenPriv->winSize * (no));		\
}

#define SET_SOURCE_BANK(pDrawable, no) {				\
  (*pScreenPriv->SetWindow)(((DrawablePtr)pDrawable)->pScreen,          \
                            (no) << pScreenPriv->winShift,		\
			    pScreenPriv->srcIndex);			\
  ((PixmapPtr)pDrawable)->devPrivate.ptr =				\
    pScreenPriv->srcBase - (pScreenPriv->winSize * (no));		\
}

#define SET_DESTINATION_BANK(pDrawable, no) {				\
  (*pScreenPriv->SetWindow)(((DrawablePtr)pDrawable)->pScreen,          \
                            (no) << pScreenPriv->winShift,		\
			    pScreenPriv->dstIndex);			\
  ((PixmapPtr)pDrawable)->devPrivate.ptr =				\
    pScreenPriv->dstBase - (pScreenPriv->winSize * (no));		\
}

static int     svgaBankScreenIndex;
static int     svgaBankGCIndex;
static ulong   svgaBankGeneration = 0;
static GCFuncs svgaBankGCFuncs;
static GCOps   svgaBankGCOps;

#define BANK_SCRPRIVLVAL(pScreen) \
  (pScreen)->devPrivates[svgaBankScreenIndex].ptr

#define BANK_SCRPRIVATE(pScreen) \
  ((svgaBankScreenPtr)(BANK_SCRPRIVLVAL(pScreen)))

#define BANK_GCPRIVATE(pGC) \
  ((svgaBankGCPtr)((pGC)->devPrivates[svgaBankGCIndex].ptr))

#define SCREEN_PROLOGUE(pScreen, field)\
  ((pScreen)->field = BANK_SCRPRIVATE(pScreen)->field)

#define SCREEN_EPILOGUE(pScreen, field, wrapper)\
    ((pScreen)->field = wrapper)

#define GC_PROLOGUE(pGC, pGCPriv) \
    (pGC)->ops = pGCPriv->wrapOps;					\
    (pGC)->funcs = pGCPriv->wrapFuncs;

#define GC_EPILOGUE(pGC, pGCPriv) \
    pGCPriv->wrapOps   = (pGC)->ops;					\
    pGCPriv->wrapFuncs = (pGC)->funcs;					\
    (pGC)->ops = &svgaBankGCOps;					\
    (pGC)->funcs = &svgaBankGCFuncs;					\

#define ON_HARDWARE(pDrawable)						\
  (svgaVTActive &&							\
   ((((DrawablePtr)(pDrawable))->type == DRAWABLE_WINDOW) ||		\
    (((DrawablePtr)(pDrawable))->class != 0)))

#define OP_INIT(pGC)							\
    svgaBankScreenPtr pScreenPriv = BANK_SCRPRIVATE((pGC)->pScreen);	\
    svgaBankGCPtr     pGCPriv = BANK_GCPRIVATE(pGC);			\
    cfbPrivGCPtr      pOrigGCPriv = cfbGetGCPrivate(pGC);		\
    RegionPtr         pOrigCompositeClip = pOrigGCPriv->pCompositeClip;	\
    int i;

#define OP_PROLOGUE(pGC)						\
    (pGC)->ops = pGCPriv->wrapOps;

#define OP_EPILOGUE(pGC)						\
    (pGC)->ops = &svgaBankGCOps;

#define OP_TOP_PART(pGC)						\
    for (i = 0; i < pScreenPriv->numBanks; i++)				\
      if ((pOrigGCPriv->pCompositeClip = pGCPriv->pBankedClips[i]) != NULL) \
        {								\
	  SET_SINGLE_BANK(pDrawable, i);

#define OP_BOTTOM_PART(pGC)						\
	}								\
    pOrigGCPriv->pCompositeClip = pOrigCompositeClip;


#define OP_SIMPLE(pDrawable, pGC, statement)				\
  {									\
    OP_INIT(pGC);							\
    if (ON_HARDWARE(pDrawable)) {					\
      OP_TOP_PART(pGC);							\
      OP_PROLOGUE(pGC);							\
      statement;							\
      OP_BOTTOM_PART(pGC);						\
    }									\
    else {								\
      OP_PROLOGUE(pGC);							\
      statement;							\
    }									\
    OP_EPILOGUE(pGC);							\
  }



/*
 * Screen Wrappers
 */

static Bool
svgaBankCloseScreen(
    int         index,
    ScreenPtr	pScreen
)
{
  int               i;
  svgaBankScreenPtr pScreenPriv = BANK_SCRPRIVATE(pScreen);

  pScreen->CloseScreen = pScreenPriv->CloseScreen;

  if (pScreenPriv->GetImage) {
    pScreen->GetImage              = pScreenPriv->GetImage;
    pScreen->GetSpans              = pScreenPriv->GetSpans;
    pScreen->CreateGC              = pScreenPriv->CreateGC;
    pScreen->PaintWindowBackground = pScreenPriv->PaintWindowBackground;
    pScreen->PaintWindowBorder     = pScreenPriv->PaintWindowBorder;
    pScreen->CopyWindow            = pScreenPriv->CopyWindow;
  }

  for (i = 0; i < pScreenPriv->numBanks; i++)
    if (pScreenPriv->pBanks[i])
      REGION_DESTROY(pScreen, pScreenPriv->pBanks[i]);

  if (pScreenPriv->dstBase)
    munmap((caddr_t)pScreenPriv->dstBase, pScreenPriv->winSize);

  if (pScreenPriv->srcBase)
    munmap((caddr_t)pScreenPriv->srcBase, pScreenPriv->winSize);

  Xfree(pScreenPriv);
  return (*pScreen->CloseScreen)(index, pScreen);
}

static void
svgaBankGetImage(
    DrawablePtr pDrawable,
    int		sx,
    int         sy,
    int         w,
    int         h,
    uint        format,
    ulong       planemask,
    char       *pImage
)
{
  ScreenPtr pScreen = pDrawable->pScreen;

  SCREEN_PROLOGUE(pScreen, GetImage);

  if (!ON_HARDWARE(pDrawable))
    (*pScreen->GetImage)(pDrawable, sx, sy, w, h, format, planemask, pImage);

  else {
    svgaBankScreenPtr pScreenPriv = BANK_SCRPRIVATE(pDrawable->pScreen);
    BoxPtr            pBox;
    int               i, nBox, csx1, csy1, csx2, csy2, rowOffset;
    int               xorg, yorg;

    if (format == ZPixmap)
      rowOffset = PixmapBytePad(w, pDrawable->depth);
    else
      rowOffset = BitmapBytePad(w);
    
    xorg = pDrawable->x;
    yorg = pDrawable->y;
    sx   += xorg;
    sy   += yorg;

    for (i = 0; i < pScreenPriv->numBanks; i++) {

      nBox = REGION_NUM_RECTS(pScreenPriv->pBanks[i]);
      pBox = REGION_RECTS(pScreenPriv->pBanks[i]);

      for (;nBox--; pBox++) {
	csx1 = sx;
	csy1 = sy;
	csx2 = csx1 + w;
	csy2 = csy1 + h;

	if (pBox->x1 > csx1) csx1 = pBox->x1;
	if (pBox->y1 > csy1) csy1 = pBox->y1;
	if (pBox->x2 < csx2) csx2 = pBox->x2;
	if (pBox->y2 < csy2) csy2 = pBox->y2;

	if ((csx1 < csx2) && (csy1 < csy2)) {

	  SET_SINGLE_BANK(pDrawable, i);

	  (*pScreen->GetImage)(pDrawable,
			       csx1 - xorg,
			       csy1 - yorg,
			       csx2 - csx1,
			       csy2 - csy1,
			       format,
			       planemask,
			       pImage +
				  ((csy1 - sy) * rowOffset) + (csx1 - sx));
	}
      }
    }
  }

  SCREEN_EPILOGUE(pScreen, GetImage, svgaBankGetImage);
}

static void
svgaBankGetSpans(
    DrawablePtr	pDrawable,
    int		wMax,
    DDXPointPtr	ppt,
    int		*pwidth,
    int		nspans,
    char       *pImage
)
{
  ScreenPtr pScreen = pDrawable->pScreen;

  SCREEN_PROLOGUE(pScreen, GetSpans);

  if (!ON_HARDWARE(pDrawable))
    (*pScreen->GetSpans)(pDrawable, wMax, ppt, pwidth, nspans, pImage);

  else {
    svgaBankScreenPtr pScreenPriv = BANK_SCRPRIVATE(pDrawable->pScreen);
    BoxPtr            pBox;
    DDXPointRec       pt;
    int               i, cx1, cy1, cx2, cy2, width, nBox;

    for (;nspans--; ppt++, pwidth++) 

      if (*pwidth > 0) {
    
	for (i = 0; i < pScreenPriv->numBanks; i++) {
	  nBox = REGION_NUM_RECTS(pScreenPriv->pBanks[i]);
	  pBox = REGION_RECTS(pScreenPriv->pBanks[i]);

	  for (;nBox--; pBox++) {
	    cx1 = ppt->x;
	    cy1 = ppt->y;
	    cx2 = cx1 + *pwidth;
	    cy2 = cy1 + 1;

	    if (pBox->x1 > cx1) cx1 = pBox->x1;
	    if (pBox->y1 > cy1) cy1 = pBox->y1;
	    if (pBox->x2 < cx2) cx2 = pBox->x2;
	    if (pBox->y2 < cy2) cy2 = pBox->y2;

	    if ((cx1 < cx2) && (cy1 < cy2)) {

	      SET_SINGLE_BANK(pDrawable, i);

	      pt.x = (ushort)cx1;
	      pt.y = (ushort)cy1;
	      width = cx2 - cx1;

	      (*pScreen->GetSpans)(pDrawable,
				   wMax, &pt, &width, 1,
				   pImage + (cx1 - ppt->x));
	    }
	  }
	}

	pImage = pImage + PixmapBytePad(*pwidth, pDrawable->depth);
      }
  }

  SCREEN_EPILOGUE(pScreen, GetSpans, svgaBankGetSpans);
}

static Bool
svgaBankCreateGC(
    GCPtr pGC
)
{
  ScreenPtr     pScreen = pGC->pScreen;
  svgaBankGCPtr pGCPriv = BANK_GCPRIVATE(pGC);
  Bool	        ret;

  SCREEN_PROLOGUE(pScreen, CreateGC);

  if ((ret = (*pScreen->CreateGC)(pGC))) {

    pGCPriv->wrapOps   = pGC->ops;
    pGCPriv->wrapFuncs = pGC->funcs;
    pGC->ops           = &svgaBankGCOps;
    pGC->funcs         = &svgaBankGCFuncs;

    memset(pGCPriv->pBankedClips, 0, sizeof(pGCPriv->pBankedClips));
  }

  SCREEN_EPILOGUE(pScreen, CreateGC, svgaBankCreateGC);

  return ret;
}


static void
svgaBankPaintWindow(
    WindowPtr pWin,
    RegionPtr pRegion,
    int	      what
)
{
  ScreenPtr         pScreen     = pWin->drawable.pScreen;
  svgaBankScreenPtr pScreenPriv = BANK_SCRPRIVATE(pScreen);
  RegionRec         tmpReg;
  int               i;
  void              (*PaintWindow)();

  if (what == PW_BORDER)
    {
      SCREEN_PROLOGUE(pScreen, PaintWindowBorder);
      PaintWindow = pScreen->PaintWindowBorder;
    }
  else
    {
      SCREEN_PROLOGUE(pScreen, PaintWindowBackground);
      PaintWindow = pScreen->PaintWindowBackground;
    }

  REGION_INIT(pScreen, &tmpReg, NullBox, 0);
  
  for (i = 0; i < pScreenPriv->numBanks; i++)
    {
      REGION_INTERSECT(pScreen, &tmpReg, pRegion, pScreenPriv->pBanks[i]);

      if (REGION_NOTEMPTY(pScreen, &tmpReg))
	{
	  SET_SINGLE_BANK(((DrawablePtr)pWin), i);

	  (*PaintWindow)(pWin, &tmpReg, what);
	}
    }

  REGION_UNINIT(pScreen, &tmpReg);

  if (what == PW_BORDER)
    {
      SCREEN_EPILOGUE(pScreen, PaintWindowBorder, svgaBankPaintWindow);
    }
  else
    {
      SCREEN_EPILOGUE(pScreen, PaintWindowBackground, svgaBankPaintWindow);
    }
}


static void
svgaBankCopyWindow(
    WindowPtr   pWindow,
    DDXPointRec ptOldOrg,
    RegionPtr   pRgnSrc
)
{
  ScreenPtr     pScreen    = pWindow->drawable.pScreen;
  GCPtr         pGC;
  int           dx, dy, nBox;
  DrawablePtr   pDrawable = (DrawablePtr)WindowTable[pScreen->myNum];
  RegionPtr     pRgnDst;
  BoxPtr        pBox, pBoxTmp, pBoxNext, pBoxBase, pBoxNew1, pBoxNew2;
  XID	        subWindowMode = IncludeInferiors;

  pGC = GetScratchGC(pDrawable->depth, pScreen);

  ChangeGC(pGC, GCSubwindowMode, &subWindowMode);
  ValidateGC(pDrawable, pGC);

  pRgnDst = REGION_CREATE(pScreen, NULL, 1);
  
  dx = ptOldOrg.x - pWindow->drawable.x;
  dy = ptOldOrg.y - pWindow->drawable.y;
  REGION_TRANSLATE(pScreen, pRgnSrc, -dx, -dy);
  REGION_INTERSECT(pScreen, pRgnDst, &pWindow->borderClip, pRgnSrc);

  pBox = REGION_RECTS(pRgnDst);
  nBox = REGION_NUM_RECTS(pRgnDst);

  pBoxNew1 = NULL;
  pBoxNew2 = NULL;

  if (nBox > 1 && dy < 0)
    {
      /* walk source botttom to top */

      pBoxNew1 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nBox);

      pBoxBase = pBoxNext = pBox + nBox -1;
      while (pBoxBase >= pBox)
	{
	  while ((pBoxNext >= pBox) &&
		 (pBoxBase->y1 == pBoxNext->y1))
	    pBoxNext--;

	  pBoxTmp = pBoxNext +1;

	  while (pBoxTmp <= pBoxBase)
	    *pBoxNew1++ = *pBoxTmp++;

	  pBoxBase = pBoxNext;
	}
         
      pBoxNew1 -= nBox;
      pBox = pBoxNew1;
    }

  if (nBox > 1 && dx < 0)
    {
      /* walk source right to left */

      pBoxNew2 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nBox);

      pBoxBase = pBoxNext = pBox;
      while (pBoxBase < pBox + nBox)
	{
	  while ((pBoxNext < pBox + nBox) &&
		 (pBoxNext->y1 == pBoxBase->y1))
	    pBoxNext++;

	  pBoxTmp = pBoxNext;

	  while (pBoxTmp != pBoxBase)
	    *pBoxNew2++ = *--pBoxTmp;

	  pBoxBase = pBoxNext;
	}

      pBoxNew2 -= nBox;
      pBox = pBoxNew2;
    }

  while (nBox--) {

    (*pGC->ops->CopyArea)(pDrawable, pDrawable, pGC,
			  pBox->x1 + dx,
			  pBox->y1 + dy,
			  pBox->x2 - pBox->x1,
			  pBox->y2 - pBox->y1,
			  pBox->x1,
			  pBox->y1);

    pBox++;
  }

  FreeScratchGC(pGC);

  REGION_DESTROY(pScreen, pRgnDst);

  if (pBoxNew2) DEALLOCATE_LOCAL(pBoxNew2);
  if (pBoxNew1) DEALLOCATE_LOCAL(pBoxNew1);
}



/*
 * GCFuncs wrappers
 */

static void
svgaBankValidateGC(
    GCPtr   	pGC,
    ulong       changes,
    DrawablePtr pDrawable
)
{
  svgaBankGCPtr pGCPriv = BANK_GCPRIVATE(pGC);

  GC_PROLOGUE(pGC, pGCPriv);

  (*pGC->funcs->ValidateGC)(pGC, changes, pDrawable);

  if ((changes & (GCClipXOrigin|GCClipYOrigin|GCClipMask|GCSubwindowMode)) ||
      (pDrawable->serialNumber != (pGC->serialNumber & DRAWABLE_SERIAL_BITS))
      )
    {
      ScreenPtr         pScreen        = pGC->pScreen;
      svgaBankScreenPtr pScreenPriv    = BANK_SCRPRIVATE(pScreen);
      RegionPtr         pCompositeClip = cfbGetGCPrivate(pGC)->pCompositeClip;
      RegionPtr         prgnClip;
      int               i;

      if (ON_HARDWARE(pDrawable)) {
	for (i = 0; i < pScreenPriv->numBanks; i++) {

	  if (!(prgnClip = pGCPriv->pBankedClips[i]))
	    prgnClip = REGION_CREATE(pScreen, NULL, 1);

	  REGION_INTERSECT(pScreen, prgnClip,
				pScreenPriv->pBanks[i],
				pCompositeClip);

	  if ((REGION_NUM_RECTS(prgnClip) <= 1) &&
	      (prgnClip->extents.x1 == prgnClip->extents.x2 ||
	       prgnClip->extents.y1 == prgnClip->extents.y2))
	    {
	      REGION_DESTROY(pScreen, prgnClip);
	      pGCPriv->pBankedClips[i] = NULL;
	    }
	  else
	    pGCPriv->pBankedClips[i] = prgnClip;
	}
      }
      else {
	/*
	 * Here we are on a pixmap and don't need all that special clipping
	 * stuff, hence free it.
	 */
	for (i = 0; i < pScreenPriv->numBanks; i++) {

	  if (pGCPriv->pBankedClips[i])
	    {
	      REGION_DESTROY(pScreen, pGCPriv->pBankedClips[i]);
	      pGCPriv->pBankedClips[i] = NULL;
	    }
	}
      }
    }

  GC_EPILOGUE(pGC, pGCPriv);
}

static void
svgaBankChangeGC(
    GCPtr pGC,
    ulong mask
)
{
  svgaBankGCPtr pGCPriv = BANK_GCPRIVATE(pGC);

  GC_PROLOGUE(pGC, pGCPriv);

  (*pGC->funcs->ChangeGC)(pGC, mask);

  GC_EPILOGUE(pGC, pGCPriv);
}

static void
svgaBankCopyGC(
    GCPtr pGCSrc,
    ulong mask,
    GCPtr pGCDst
)
{
  svgaBankGCPtr pGCPriv = BANK_GCPRIVATE(pGCDst);

  GC_PROLOGUE(pGCDst, pGCPriv);

  (*pGCDst->funcs->CopyGC)(pGCSrc, mask, pGCDst);

  GC_EPILOGUE(pGCDst, pGCPriv);
}

static void
svgaBankDestroyGC(
    GCPtr pGC
)
{
  svgaBankGCPtr     pGCPriv     = BANK_GCPRIVATE(pGC);
  ScreenPtr         pScreen     = pGC->pScreen;
  svgaBankScreenPtr pScreenPriv = BANK_SCRPRIVATE(pScreen);
  int               i;

  GC_PROLOGUE(pGC, pGCPriv);

  (*pGC->funcs->DestroyGC)(pGC);

  for (i = 0; i < pScreenPriv->numBanks; i++) {

    if (pGCPriv->pBankedClips[i])
      REGION_DESTROY(pScreen, pGCPriv->pBankedClips[i]);
  }

  GC_EPILOGUE(pGC, pGCPriv);
}

static void
svgaBankChangeClip(
    GCPtr   pGC,
    int	    type,
    pointer pvalue,
    int	    nrects
)
{
  svgaBankGCPtr pGCPriv = BANK_GCPRIVATE(pGC);

  GC_PROLOGUE(pGC, pGCPriv);

  (*pGC->funcs->ChangeClip)(pGC, type, pvalue, nrects);

  GC_EPILOGUE(pGC, pGCPriv);
}

static void
svgaBankDestroyClip(
    GCPtr pGC
)
{
  svgaBankGCPtr pGCPriv = BANK_GCPRIVATE(pGC);

  GC_PROLOGUE(pGC, pGCPriv);

  (*pGC->funcs->DestroyClip)(pGC);

  GC_EPILOGUE(pGC, pGCPriv);
}

static void
svgaBankCopyClip(
    GCPtr pGCDst,
    GCPtr pGCSrc
)
{
  svgaBankGCPtr pGCPriv = BANK_GCPRIVATE(pGCDst);

  GC_PROLOGUE(pGCDst, pGCPriv);

  (*pGCDst->funcs->CopyClip)(pGCDst, pGCSrc);

  GC_EPILOGUE(pGCDst, pGCPriv);
}

static GCFuncs svgaBankGCFuncs = {
    svgaBankValidateGC,
    svgaBankChangeGC,
    svgaBankCopyGC,
    svgaBankDestroyGC,
    svgaBankChangeClip,
    svgaBankDestroyClip,
    svgaBankCopyClip,
};


/*
 * GCOps wrappers
 */

static void
svgaBankFillSpans(
    DrawablePtr pDrawable,
    GCPtr	pGC,
    int		nInit,
    DDXPointPtr pptInit,
    int		*pwidthInit,
    int 	fSorted
)
{
  /* Inputs are not destroyed */
   
  OP_SIMPLE(pDrawable,
	    pGC,
	    (*pGC->ops->FillSpans)(pDrawable, pGC, nInit, pptInit,
				   pwidthInit, fSorted)
	    );
}

static void
svgaBankSetSpans(
    DrawablePtr	pDrawable,
    GCPtr	pGC,
    char	*psrc,
    DDXPointPtr ppt,
    int		*pwidth,
    int		nspans,
    int		fSorted
)
{
  /* Inputs are not destroyed */

  OP_SIMPLE(pDrawable,
	    pGC,
	    (*pGC->ops->SetSpans)(pDrawable, pGC, psrc, ppt, pwidth,
				  nspans, fSorted)
	    );
}

static void
svgaBankPutImage(
    DrawablePtr pDrawable,
    GCPtr   	pGC,
    int		depth,
    int	    	x,
    int	    	y,
    int	    	w,
    int	    	h,
    int         leftPad,		
    int     	format,
    char       *pImage
)
{
  OP_SIMPLE(pDrawable,
	    pGC,
	    (*pGC->ops->PutImage)(pDrawable, pGC,
				  depth, x, y, w, h, leftPad, format, pImage)
	    );
}

/*
 * Wow, here the realy tricky routines, the CopyArea/CopyPlane stuff.
 * First off, we have to clip correctly against the source in order to
 * make the minimal copies in case of slow systems. Also the exposure handling
 * is quite tricky, and hence much of the routine looks like svgaBitblt.
 * Special intrest should go to the way the copies are sequenced. The
 * list of boxes after the source clip is used to build a workqueue, that
 * contains the atomic copies (that means only from one bank to one bank).
 * Doing so gives us a list of things to do, which is minimal.
 */
static RegionPtr
svgaBankCopyArea(
    DrawablePtr	pSrc,
    DrawablePtr pDst,
    GCPtr   	pGC,
    int	    	srcx,
    int	    	srcy,
    int	    	w,
    int	    	h,
    int	    	dstx,
    int	    	dsty
)
{
  int           cx1, cy1, cx2, cy2, ccx1, ccy1, ccx2, ccy2;
  int           ns, nd, dx, dy, xorg, yorg;
  int           nBox, nBoxClipSrc, nBoxClipDst, nQueue;
  BoxPtr        pBox, pBoxClipSrc, pBoxClipDst;
  BoxRec        fastBox;
  PixmapPtr     pSrcShadow, pScreenPixmap;
  RegionPtr     ret, prgnSrcClip;
  RegionRec     rgnDst;
  char          *pImage;
  ScreenPtr     pScreen = pGC->pScreen;
  svgaBlitQueue *pQueue, *pQueueNew1, *pQueueNew2, *Queue;
  svgaBlitQueue *pQueueTmp, *pQueueNext, *pQueueBase;
  Bool          fastBlit = FALSE;
  Bool          fExpose, freeSrcClip, fastClip, fastExpose;

  OP_INIT(pGC);

  if (ON_HARDWARE(pSrc) && ON_HARDWARE(pDst)) {

    fExpose = pOrigGCPriv->fExpose;

    fastBox.x1 = srcx + pSrc->x;
    fastBox.y1 = srcy + pSrc->y;
    fastBox.x2 = fastBox.x1 + w;
    fastBox.y2 = fastBox.y1 + h;
    dx = dstx + pDst->x - fastBox.x1;
    dy = dsty + pDst->y - fastBox.y1;

    /*
     * Clip here correctly against the source. Otherwise we will blit too
     * much for SINGLE and SHARED banked systems, which looks pretty bad ...
     */
    freeSrcClip = FALSE;
    fastClip    = FALSE;
    fastExpose  = FALSE;

    if (pGC->subWindowMode == IncludeInferiors)
      {
	if (!((WindowPtr)pSrc)->parent)
	  {
	    fastExpose = TRUE;
	    /*
	     * clip the source; if regions extend beyond the source size,
	     * make sure exposure events get sent
	     */
	    if (fastBox.x1 < pSrc->x)
	      {
		fastBox.x1 = pSrc->x;
		fastExpose = FALSE;
	      }
	    if (fastBox.y1 < pSrc->y)
	      {
		fastBox.y1 = pSrc->y;
		fastExpose = FALSE;
	      }
	    if (fastBox.x2 > pSrc->x + (int) pSrc->width)
	      {
		fastBox.x2 = pSrc->x + (int) pSrc->width;
		fastExpose = FALSE;
	      }
	    if (fastBox.y2 > pSrc->y + (int) pSrc->height)
	      {
		fastBox.y2 = pSrc->y + (int) pSrc->height;
		fastExpose = FALSE;
	      }
	    
	    nBox = 1;
	    pBox = &fastBox;
	    fastClip = TRUE;
	  }
	else if ((pSrc == pDst) && (pGC->clientClipType == CT_NONE))
	  {
	    prgnSrcClip = pOrigGCPriv->pCompositeClip;
	  }
	else
	  {
	    prgnSrcClip = NotClippedByChildren((WindowPtr)pSrc);
	    freeSrcClip = TRUE;
	  }
      }
    else
      {
	prgnSrcClip = &((WindowPtr)pSrc)->clipList;
      }

    if (!fastClip)
      {
	REGION_INIT(pGC->pScreen, &rgnDst, &fastBox, 1);
	REGION_INTERSECT(pGC->pScreen, &rgnDst, &rgnDst, prgnSrcClip);
	pBox = REGION_RECTS(&rgnDst);
	nBox = REGION_NUM_RECTS(&rgnDst);
      }

    nQueue = nBox * pScreenPriv->numBanks * 9;
    pQueue = Queue = (svgaBlitQueue*)ALLOCATE_LOCAL(nQueue *
						    sizeof(svgaBlitQueue));

    for(;nBox--; pBox++) {

      for (ns = 0; ns < pScreenPriv->numBanks; ns++) {

	for (nBoxClipSrc = REGION_NUM_RECTS(pScreenPriv->pBanks[ns]),
	     pBoxClipSrc = REGION_RECTS(pScreenPriv->pBanks[ns]);
	     nBoxClipSrc--;
	     pBoxClipSrc++) {

	  cx1 = max(pBox->x1, pBoxClipSrc->x1);
	  cy1 = max(pBox->y1, pBoxClipSrc->y1);
	  cx2 = min(pBox->x2, pBoxClipSrc->x2);
	  cy2 = min(pBox->y2, pBoxClipSrc->y2);

	  /* Check to see if the region is empty */
	  if ((cx1 >= cx2) || (cy1 >= cy2)) continue;

	  /* Translate c[xy]* so they are now destination coordinates */
	  cx1 += dx;
	  cy1 += dy;
	  cx2 += dx;
	  cy2 += dy;

	  for (nd = 0; nd < pScreenPriv->numBanks; nd++) {

	    for (nBoxClipDst = REGION_NUM_RECTS(pScreenPriv->pBanks[nd]),
		 pBoxClipDst = REGION_RECTS(pScreenPriv->pBanks[nd]);
		 nBoxClipDst--;
		 pBoxClipDst++) {
		
	      ccx1 = max(cx1, pBoxClipDst->x1);
	      ccy1 = max(cy1, pBoxClipDst->y1);
	      ccx2 = min(cx2, pBoxClipDst->x2);
	      ccy2 = min(cy2, pBoxClipDst->y2);

	      /* Check to see if the region is empty */
	      if ((ccx1 >= ccx2) || (ccy1 >= ccy2)) continue;
	      
	      pQueue->srcBankNo = ns;
	      pQueue->dstBankNo = nd;
	      pQueue->x1        = ccx1;
	      pQueue->y1        = ccy1;
	      pQueue->x2        = ccx2;
	      pQueue->y2        = ccy2;
	      pQueue++;
	    }
	  }
	}
      }
    }

    if (!fastClip)
      {
	REGION_UNINIT(pGC->pScreen, &rgnDst);
	if (freeSrcClip)
	  REGION_DESTROY(pGC->pScreen, prgnSrcClip);
      }

    /* sort */

    nQueue     = pQueue - Queue ;
    pQueue     = Queue;

    pQueueNew1 = NULL;
    pQueueNew2 = NULL;

    if (pSrc == pDst && nQueue > 1) {

      if (srcy < dsty) {

	/* walk source botttom to top */
	pQueueNew1 = (svgaBlitQueue*)ALLOCATE_LOCAL(sizeof(svgaBlitQueue) *
						    nQueue);

	pQueueBase = pQueueNext = pQueue + nQueue -1;
	while (pQueueBase >= pQueue)
	  {
	    while ((pQueueNext >= pQueue) && 
		   (pQueueBase->y1 == pQueueNext->y1))
	      pQueueNext--;

	    pQueueTmp = pQueueNext +1;
	    while (pQueueTmp <= pQueueBase)
	      *pQueueNew1++ = *pQueueTmp++;

	    pQueueBase = pQueueNext;
	  }
	pQueueNew1 -= nQueue;
	pQueue = pQueueNew1;
      }

      if (srcx < dstx) {
	
	/* walk source right to left */
	pQueueNew2 = (svgaBlitQueue*)ALLOCATE_LOCAL(sizeof(svgaBlitQueue) *
						    nQueue);

	pQueueBase = pQueueNext = pQueue;
	while (pQueueBase < pQueue + nQueue)
	  {
	    while ((pQueueNext < pQueue + nQueue) &&
		   (pQueueNext->y1 == pQueueBase->y1))
	      pQueueNext++;

	    pQueueTmp = pQueueNext;
	    while (pQueueTmp != pQueueBase)
	      *pQueueNew2++ = *--pQueueTmp;

	    pQueueBase = pQueueNext;
	  }

	pQueueNew2 -= nQueue;
	pQueue = pQueueNew2;
      }
    }

    pScreenPixmap = cfbGetScreenPixmap(pScreen);

    pSrcShadow = GetScratchPixmapHeader(pScreen,
					pScreenPixmap->drawable.width,
					pScreenPixmap->drawable.height,
					pScreenPixmap->drawable.depth,
					pScreenPixmap->drawable.bitsPerPixel,
					pScreenPixmap->devKind,
					pScreenPixmap->devPrivate.ptr);
    pSrcShadow->drawable.class = 1; /* ### */

    switch (pScreenPriv->type) {

    case BANK_SHARED:
      fastBlit = (pGC->alu == GXcopy) && ((pGC->planemask & 0xff) == 0xff);
      /* FALLTHRU */

    case BANK_SINGLE:
      pImage = (char*)ALLOCATE_LOCAL(PixmapBytePad(w, pSrc->depth)*h);

      while(nQueue--) {
	
	if (pQueue->srcBankNo == pQueue->dstBankNo) {

	  pOrigGCPriv->fExpose = FALSE;

	  OP_PROLOGUE(pGC);

	  SET_SINGLE_BANK(pScreenPixmap, pQueue->srcBankNo);

	  (pGC->ops->CopyArea)((DrawablePtr)pScreenPixmap,
			       (DrawablePtr)pScreenPixmap, pGC,
			       pQueue->x1 - dx,
			       pQueue->y1 - dy,
			       pQueue->x2 - pQueue->x1,
			       pQueue->y2 - pQueue->y1,
			       pQueue->x1,
			       pQueue->y1);

	  OP_EPILOGUE(pGC);
	}
	else if (fastBlit) {

	  pOrigGCPriv->fExpose = FALSE;

	  OP_PROLOGUE(pGC);

	  SET_SOURCE_BANK     (pSrcShadow,    pQueue->srcBankNo);
	  SET_DESTINATION_BANK(pScreenPixmap, pQueue->dstBankNo);

	  (pGC->ops->CopyArea)((DrawablePtr)pSrcShadow,
			       (DrawablePtr)pScreenPixmap,
			       pGC,
			       pQueue->x1 - dx,
			       pQueue->y1 - dy,
			       pQueue->x2 - pQueue->x1,
			       pQueue->y2 - pQueue->y1,
			       pQueue->x1,
			       pQueue->y1);

	  OP_EPILOGUE(pGC);
	}
	else {

	  (*pScreen->GetImage)((DrawablePtr)pScreenPixmap, 
			       pQueue->x1 - dx,
			       pQueue->y1 - dy,
			       pQueue->x2 - pQueue->x1,
			       pQueue->y2 - pQueue->y1,
			       ZPixmap, ~0L, pImage);

	  (*pGC->ops->PutImage)((DrawablePtr)pScreenPixmap, pGC, pDst->depth,
				pQueue->x1,
				pQueue->y1,
				pQueue->x2 - pQueue->x1,
				pQueue->y2 - pQueue->y1,
				0, ZPixmap, pImage);
	}

	pQueue++;
      }

      DEALLOCATE_LOCAL(pImage);
      break;


    case BANK_DOUBLE:

      pOrigGCPriv->fExpose = FALSE;
      
      while(nQueue--) {

	OP_PROLOGUE(pGC);

	if (pQueue->srcBankNo == pQueue->dstBankNo) {

	  SET_SINGLE_BANK(pScreenPixmap, pQueue->srcBankNo);

	  (pGC->ops->CopyArea)((DrawablePtr)pScreenPixmap,
			       (DrawablePtr)pScreenPixmap, pGC,
			       pQueue->x1 - dx,
			       pQueue->y1 - dy,
			       pQueue->x2 - pQueue->x1,
			       pQueue->y2 - pQueue->y1,
			       pQueue->x1,
			       pQueue->y1);
	}
	else {
	  SET_SOURCE_BANK     (pSrcShadow,    pQueue->srcBankNo);
	  SET_DESTINATION_BANK(pScreenPixmap, pQueue->dstBankNo);

	  (pGC->ops->CopyArea)((DrawablePtr)pSrcShadow,
			       (DrawablePtr)pScreenPixmap,
			       pGC,
			       pQueue->x1 - dx,
			       pQueue->y1 - dy,
			       pQueue->x2 - pQueue->x1,
			       pQueue->y2 - pQueue->y1,
			       pQueue->x1,
			       pQueue->y1);
	}

	pQueue++;
      }

      OP_EPILOGUE(pGC);

      break;
    }

    pOrigGCPriv->fExpose = fExpose;

    pSrcShadow->drawable.class = 0; /* ### */

    FreeScratchPixmapHeader(pSrcShadow);

    if (pQueueNew2) DEALLOCATE_LOCAL(pQueueNew2);
    if (pQueueNew1) DEALLOCATE_LOCAL(pQueueNew1);

    DEALLOCATE_LOCAL(Queue);

    if (fExpose && !fastExpose)
      ret = miHandleExposures(pSrc, pDst, pGC,
			      srcx, srcy, w, h, dstx, dsty, 0);
    else
      ret = NULL;
  }

  else if (ON_HARDWARE(pSrc)) {

    fExpose = pOrigGCPriv->fExpose;
    pOrigGCPriv->fExpose = FALSE;

    xorg = pSrc->x;
    yorg = pSrc->y;
    dx   = dstx - srcx;
    dy   = dsty - srcy;
    srcx += xorg;
    srcy += yorg;
    
    for (i = 0; i < pScreenPriv->numBanks; i++) {

      nBox = REGION_NUM_RECTS(pScreenPriv->pBanks[i]);
      pBox = REGION_RECTS(pScreenPriv->pBanks[i]);

      for (;nBox--; pBox++) {
	cx1 = srcx;
	cy1 = srcy;
	cx2 = cx1 + w;
	cy2 = cy1 + h;

	if (pBox->x1 > cx1) cx1 = pBox->x1;
	if (pBox->y1 > cy1) cy1 = pBox->y1;
	if (pBox->x2 < cx2) cx2 = pBox->x2;
	if (pBox->y2 < cy2) cy2 = pBox->y2;

	if ((cx1 < cx2) && (cy1 < cy2)) {

	  OP_PROLOGUE(pGC);

	  SET_SINGLE_BANK(pSrc, i);

	  (pGC->ops->CopyArea)(pSrc, pDst, pGC,
			       cx1 - xorg,
			       cy1 - yorg,
			       cx2 - cx1,
			       cy2 - cy1,
			       cx1 + dx - xorg,
			       cy1 + dy - yorg);
	}
      }
    }

    pOrigGCPriv->fExpose = fExpose;

    OP_EPILOGUE(pGC);

    if (pOrigGCPriv->fExpose)
      ret = miHandleExposures (pSrc, pDst, pGC,
			       srcx - xorg,
			       srcy - yorg,
			       w, h, dstx, dsty, 0);
    else
      ret = NULL;
  }

  else if (ON_HARDWARE(pDst)) {

    xorg = pDst->x;
    yorg = pDst->y;
    dx = srcx - dstx;
    dy = srcy - dsty;
    dstx += xorg;
    dsty += yorg;

    for (i = 0; i < pScreenPriv->numBanks; i++)

      if (pGCPriv->pBankedClips[i]) {

	nBox = REGION_NUM_RECTS(pGCPriv->pBankedClips[i]);
	pBox = REGION_RECTS(pGCPriv->pBankedClips[i]);

	for (;nBox--; pBox++) {
	  cx1 = dstx;
	  cy1 = dsty;
	  cx2 = cx1 + w;
	  cy2 = cy1 + h;

	  if (pBox->x1 > cx1) cx1 = pBox->x1;
	  if (pBox->y1 > cy1) cy1 = pBox->y1;
	  if (pBox->x2 < cx2) cx2 = pBox->x2;
	  if (pBox->y2 < cy2) cy2 = pBox->y2;

	  if ((cx1 < cx2) && (cy1 < cy2)) {
	    
	    OP_PROLOGUE(pGC);

	    SET_SINGLE_BANK(pDst, i);

	    (pGC->ops->CopyArea)(pSrc, pDst, pGC,
				 cx1 + dx - xorg,
				 cy1 + dy - yorg,
				 cx2 - cx1,
				 cy2 - cy1,
				 cx1 - xorg,
				 cy1 - yorg);
	  }
	}
      }

    OP_EPILOGUE(pGC);

    ret = NULL;
  }

  else {
    OP_PROLOGUE(pGC);

    ret = (pGC->ops->CopyArea)(pSrc, pDst, pGC, srcx, srcy, w, h, dstx, dsty);

    OP_EPILOGUE(pGC);
  }

  return ret;
}

static void
svgaBankPolyPoint(
    DrawablePtr pDrawable,
    GCPtr	pGC,
    int		mode,
    int		npt,
    xPoint 	*pptInit
)
{
  xPoint *ppt;

  OP_INIT(pGC);

  if (ON_HARDWARE(pDrawable)) {

    ppt = (xPoint*)ALLOCATE_LOCAL(npt * sizeof(xPoint));
  
    for (i = 0; i < pScreenPriv->numBanks; i++)
      if ((pOrigGCPriv->pCompositeClip = pGCPriv->pBankedClips[i]) != NULL)
        {
	  OP_PROLOGUE(pGC);

	  SET_SINGLE_BANK(pDrawable, i);

	  memcpy(ppt, pptInit, npt * sizeof(xPoint));

	  (*pGC->ops->PolyPoint)(pDrawable, pGC, mode, npt, ppt);
	}

    pOrigGCPriv->pCompositeClip = pOrigCompositeClip;

    DEALLOCATE_LOCAL(ppt);
  }
  else {
    OP_PROLOGUE(pGC);

    (*pGC->ops->PolyPoint)(pDrawable, pGC, mode,
			   npt, pptInit);
  }
  
  OP_EPILOGUE(pGC);
}

static void
svgaBankPolylines(
    DrawablePtr	pDrawable,
    GCPtr   	pGC,
    int	    	mode,
    int	    	npt,
    DDXPointPtr	pptInit
)
{
  DDXPointPtr ppt;

  OP_INIT(pGC);

  if (npt <= 0) return;

  if (ON_HARDWARE(pDrawable)) {

    if (pGC->lineWidth != 0)
      {
	if (pGC->lineStyle == LineSolid)
	  miWideLine(pDrawable, pGC, mode, npt, pptInit);
	else
	  miWideDash(pDrawable, pGC, mode, npt, pptInit);
      }
    else 
      {
	ppt = (DDXPointPtr)ALLOCATE_LOCAL(npt * sizeof(DDXPointRec));

	for (i = 0; i < pScreenPriv->numBanks; i++)
	  if ((pOrigGCPriv->pCompositeClip = pGCPriv->pBankedClips[i]) != NULL)
	    {
	      OP_PROLOGUE(pGC);

	      SET_SINGLE_BANK(pDrawable, i);
    
	      memcpy(ppt, pptInit, npt * sizeof(DDXPointRec));

	      (*pGC->ops->Polylines)(pDrawable, pGC, mode, npt, ppt);
	    }

	pOrigGCPriv->pCompositeClip = pOrigCompositeClip;

	DEALLOCATE_LOCAL(ppt);
      }
  }
  else {
    OP_PROLOGUE(pGC);

    (*pGC->ops->Polylines)(pDrawable, pGC, mode, npt, pptInit);
  }

  OP_EPILOGUE(pGC);
}

static void
svgaBankPolyFillRect(
    DrawablePtr pDrawable,
    GCPtr	pGC,
    int		nrectFill,
    xRectangle	*prectInit
)
{
  xRectangle *prect;

  OP_INIT(pGC);

  if (nrectFill <= 0) return;

  if (ON_HARDWARE(pDrawable)) {

    prect = (xRectangle*)ALLOCATE_LOCAL(nrectFill * sizeof(xRectangle));

    for (i = 0; i < pScreenPriv->numBanks; i++)
      if ((pOrigGCPriv->pCompositeClip = pGCPriv->pBankedClips[i]) != NULL)
        {
	  OP_PROLOGUE(pGC);

	  SET_SINGLE_BANK(pDrawable, i);
    
	  memcpy(prect, prectInit, nrectFill * sizeof(xRectangle));

	  (*pGC->ops->PolyFillRect)(pDrawable, pGC, nrectFill, prect);
	}

    pOrigGCPriv->pCompositeClip = pOrigCompositeClip;

    DEALLOCATE_LOCAL(prect);
  }
  else {
    OP_PROLOGUE(pGC);

    (*pGC->ops->PolyFillRect)(pDrawable, pGC, nrectFill, prectInit);
  }

  OP_EPILOGUE(pGC);
}

static void
svgaBankImageGlyphBlt(
    DrawablePtr  pDrawable,
    GC 		 *pGC,
    int 	 x,
    int          y,
    uint         nglyph,
    CharInfoPtr  *ppci,
    pointer	 pglyphBase
)
{
  OP_SIMPLE(pDrawable,
	    pGC,
	    (*pGC->ops->ImageGlyphBlt)(pDrawable, pGC,
				       x, y, nglyph, ppci, pglyphBase)
	    );
}

static void
svgaBankPolyGlyphBlt(
    DrawablePtr  pDrawable,
    GC 		 *pGC,
    int 	 x,
    int          y,
    uint         nglyph,
    CharInfoPtr  *ppci,
    pointer	 pglyphBase
)
{
  OP_SIMPLE(pDrawable,
	    pGC,
	    (*pGC->ops->PolyGlyphBlt)(pDrawable, pGC,
				      x, y, nglyph, ppci, pglyphBase)
	    );
}

static void
svgaBankPushPixels(
    GCPtr	pGC,
    PixmapPtr	pBitmap,
    DrawablePtr pDrawable,
    int		w,
    int         h,
    int         x,
    int         y
)
{
  OP_SIMPLE(pDrawable,
	    pGC,
	    (*pGC->ops->PushPixels)(pGC, pBitmap, pDrawable, w, h, x, y)
	    );
}

static GCOps svgaBankGCOps = {
  svgaBankFillSpans,	svgaBankSetSpans,	svgaBankPutImage,	
  svgaBankCopyArea,	miCopyPlane,		svgaBankPolyPoint,
  svgaBankPolylines,	miPolySegment,		miPolyRectangle,
  miPolyArc,		miFillPolygon,  	svgaBankPolyFillRect,
  miPolyFillArc,	miPolyText8,		miPolyText16,
  miImageText8,		miImageText16,		svgaBankImageGlyphBlt,
  svgaBankPolyGlyphBlt,	svgaBankPushPixels,	NULL,
};


static void
svgaBankSaveAreas(
    PixmapPtr pPixmap,
    RegionPtr prgnSave,
    int       xorg,
    int       yorg,
    WindowPtr pWin
)
{
  ScreenPtr         pScreen = pPixmap->drawable.pScreen;
  DrawablePtr       pDrawable = (DrawablePtr)cfbGetScreenPixmap(pScreen);
  svgaBankScreenPtr pScreenPriv = BANK_SCRPRIVATE(pScreen);
  RegionRec        rgnClipped;
  int              i;

  REGION_INIT(pScreen, &rgnClipped, NullBox, 0);
  REGION_TRANSLATE(pScreen, prgnSave, xorg, yorg);

  for (i = 0; i < pScreenPriv->numBanks; i++) {

    REGION_INTERSECT(pScreen, &rgnClipped, prgnSave, pScreenPriv->pBanks[i]);

    if (!REGION_NIL(&rgnClipped)) {

      SET_SINGLE_BANK(pDrawable, i);

      REGION_TRANSLATE(pScreen, &rgnClipped, -xorg, -yorg);

      cfbSaveAreas(pPixmap, &rgnClipped, xorg, yorg, pWin);
    }
  }

  REGION_TRANSLATE(pScreen, prgnSave, -xorg, -yorg);
  REGION_UNINIT(pScreen, &rgnClipped);
}

static void
svgaBankRestoreAreas(
    PixmapPtr pPixmap,
    RegionPtr prgnRestore,
    int       xorg,
    int       yorg,
    WindowPtr pWin
)
{
  ScreenPtr         pScreen = pPixmap->drawable.pScreen;
  DrawablePtr       pDrawable = (DrawablePtr)cfbGetScreenPixmap(pScreen);
  svgaBankScreenPtr pScreenPriv = BANK_SCRPRIVATE(pScreen);
  RegionRec         rgnClipped;
  int               i;

  REGION_INIT(pScreen, &rgnClipped, NullBox, 0);

  for (i = 0; i < pScreenPriv->numBanks; i++) {

    REGION_INTERSECT(pScreen, &rgnClipped, prgnRestore, pScreenPriv->pBanks[i]);

    if (!REGION_NIL(&rgnClipped)) {

      SET_SINGLE_BANK(pDrawable, i);
      
      cfbRestoreAreas(pPixmap, &rgnClipped, xorg, yorg, pWin);
    }

  }
  REGION_UNINIT(pScreen, &rgnClipped);
}

miBSFuncRec svgaBankBSFuncRec = {
    svgaBankSaveAreas,
    svgaBankRestoreAreas,
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};


Bool
svgaBankScreenInit(
    ScreenPtr  pScreen,
    ulong      winAAttributes,
    ulong      winBAttributes,
    int        winGranularity,
    int        winSize,
    ulong      winABase,
    ulong      winBBase,
    ulong      bytesPerScanLine,
    short      width,
    short      height,
    void       (*SetWindow)()
)
{
  short             xsb, ysb, xse, yse;
  int               numBanks, i; 
  ulong             bankBase;
  unchar            *winAImage, *winBImage;
  svgaBankScreenPtr pScreenPriv;
  xRectangle       *pRects, pRectsBase[3];
  static int        fd = -1;

  if ((fd == -1) && ((fd = open("/dev/mem", O_RDWR)) == -1)) {
    return FALSE;
  }

  if (svgaBankGeneration != serverGeneration) {
    svgaBankScreenIndex = AllocateScreenPrivateIndex();
    if (svgaBankScreenIndex < 0)
      return FALSE;

    svgaBankGCIndex = AllocateGCPrivateIndex();
    svgaBankGeneration = serverGeneration;
  }

  if (!AllocateGCPrivate(pScreen, svgaBankGCIndex, sizeof(svgaBankGCRec)))
    return FALSE;

  if (!(pScreenPriv = (svgaBankScreenPtr) Xcalloc(sizeof(svgaBankScreenRec))))
    return FALSE;

  switch(winSize / winGranularity) {
  case   1: pScreenPriv->winShift = 0; break;
  case   2: pScreenPriv->winShift = 1; break;
  case   4: pScreenPriv->winShift = 2; break;
  case   8: pScreenPriv->winShift = 3; break;
  case  16: pScreenPriv->winShift = 4; break;
  case  32: pScreenPriv->winShift = 5; break;
  case  64: pScreenPriv->winShift = 6; break;
  case 128: pScreenPriv->winShift = 7; break;
  case 256: pScreenPriv->winShift = 8; break;
  }

  winSize *= 1024;
  numBanks  = (bytesPerScanLine * height -1) / winSize  +1;

  pScreenPriv->numBanks  = numBanks;
  pScreenPriv->winSize   = winSize;
  pScreenPriv->SetWindow = SetWindow;

  if (winAAttributes & WINDOW_SUPPORTED)
    if ((int)(winAImage = (unchar*)mmap((caddr_t)0, winSize,
					PROT_READ | PROT_WRITE, MAP_SHARED,
					fd, winABase))
	== -1)
      {
	Error("couldn't map videomemory");
	return FALSE;
      }

  if (winBAttributes & WINDOW_SUPPORTED)
    if ((int)(winBImage = (unchar*)mmap((caddr_t)0, winSize,
					PROT_READ | PROT_WRITE, MAP_SHARED,
					fd, winBBase))
	== -1)
      {
	Error("couldn't map videomemory");
	return FALSE;
      }

  /*
   * Find out which banking type we have
   */
  if ((winAAttributes ==
       (WINDOW_SUPPORTED | WINDOW_READABLE | WINDOW_WRITEABLE)) &&

      (winBAttributes ==
       (WINDOW_SUPPORTED | WINDOW_READABLE | WINDOW_WRITEABLE)))
    {
      pScreenPriv->type     = BANK_DOUBLE;
      pScreenPriv->srcIndex = 0;
      pScreenPriv->dstIndex = 1;
      pScreenPriv->srcBase  = winAImage;
      pScreenPriv->dstBase  = winBImage;
    }
  else if ((winAAttributes == (WINDOW_SUPPORTED | WINDOW_READABLE)) &&
	   (winBAttributes == (WINDOW_SUPPORTED | WINDOW_WRITEABLE)) &&
	   (winABase == winBBase))
    {
      pScreenPriv->type     = BANK_SHARED;
      pScreenPriv->srcIndex = 0;
      pScreenPriv->dstIndex = 1;
      pScreenPriv->srcBase  = winAImage;
      pScreenPriv->dstBase  = winAImage;
    }
  else if ((winAAttributes == (WINDOW_SUPPORTED | WINDOW_WRITEABLE)) &&
	   (winBAttributes == (WINDOW_SUPPORTED | WINDOW_READABLE)) &&
	   (winABase == winBBase))
    {
      pScreenPriv->type     = BANK_SHARED;
      pScreenPriv->srcIndex = 1;
      pScreenPriv->dstIndex = 0;
      pScreenPriv->srcBase  = winAImage;
      pScreenPriv->dstBase  = winBImage;
    }
  else if (winAAttributes ==
	   (WINDOW_SUPPORTED | WINDOW_READABLE | WINDOW_WRITEABLE))
    {
      pScreenPriv->type     = BANK_SINGLE;
      pScreenPriv->srcIndex = 0;
      pScreenPriv->srcBase  = winAImage;
  }
  else {
    Xfree(pScreenPriv);
    return FALSE;
  }

  /*
   * Build the list of regions for the banking ...
   */
  for (bankBase = winSize, xsb = 0, ysb = 0, i = 0;
       i < numBanks;
       i++, bankBase += winSize, xsb = xse, ysb = yse)
    {
      pRects = pRectsBase;

      xse = ((bankBase % bytesPerScanLine) * 8) / PSZ;
      yse = bankBase / bytesPerScanLine;

      if (xsb == 0) {
	  pRects->x      = 0;
	  pRects->y      = ysb;
	  pRects->width  = width;
	  pRects->height = yse - ysb;
	  pRects++;

	  if (xse != 0) {
	    pRects->x      = 0;
	    pRects->y      = yse;
	    pRects->width  = xse;
	    pRects->height = 1;
	    pRects++;
	  }
	}
      else {
	pRects->x      = xsb;
	pRects->y      = ysb;
	pRects->width  = width - xsb;
	pRects->height = 1;
	pRects++;

	pRects->x      = 0;
	pRects->y      = ysb +1;
	pRects->width  = width;
	pRects->height = yse - ysb -1;
	pRects++;

	if (xse != 0) {
	  pRects->x      = 0;
	  pRects->y      = yse;
	  pRects->width  = xse;
	  pRects->height = 1;
	  pRects++;
	}
      }

      pScreenPriv->pBanks[i] = RECTS_TO_REGION(pScreen, pRects - pRectsBase,
							 pRectsBase,
							 0);
    }

  pScreenPriv->CloseScreen = pScreen->CloseScreen;
  pScreen->CloseScreen     = svgaBankCloseScreen;

  BANK_SCRPRIVLVAL(pScreen) = (pointer)pScreenPriv;

  pScreenPriv->GetImage              = pScreen->GetImage;
  pScreenPriv->GetSpans              = pScreen->GetSpans;
  pScreenPriv->CreateGC              = pScreen->CreateGC;
  pScreenPriv->PaintWindowBackground = pScreen->PaintWindowBackground;
  pScreenPriv->PaintWindowBorder     = pScreen->PaintWindowBorder;
  pScreenPriv->CopyWindow            = pScreen->CopyWindow;

  pScreen->GetImage              = svgaBankGetImage;
  pScreen->GetSpans              = svgaBankGetSpans;
  pScreen->CreateGC              = svgaBankCreateGC;
  pScreen->PaintWindowBackground = svgaBankPaintWindow;
  pScreen->PaintWindowBorder     = svgaBankPaintWindow;
  pScreen->CopyWindow            = svgaBankCopyWindow;
  
  return TRUE;
}
