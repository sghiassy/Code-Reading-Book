/* $XConsortium: PsText.c /main/3 1996/12/30 14:58:35 kaleb $ */
/*
 * (c) Copyright 1996 Hewlett-Packard Company
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc.
 * (c) Copyright 1996 Digital Equipment Corp.
 * (c) Copyright 1996 Fujitsu Limited
 * (c) Copyright 1996 Hitachi, Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the names of the copyright holders
 * shall not be used in advertising or otherwise to promote the sale, use
 * or other dealings in this Software without prior written authorization
 * from said copyright holders.
 */

/*******************************************************************
**
**    *********************************************************
**    *
**    *  File:		PsText.c
**    *
**    *  Contents:	Character-drawing routines for the PS DDX
**    *
**    *  Created By:	Roger Helmendach (Liberty Systems)
**    *
**    *  Copyright:	Copyright 1996 X Consortium, Inc.
**    *
**    *********************************************************
** 
********************************************************************/
/* $XFree86: xc/programs/Xserver/Xprint/ps/PsText.c,v 1.3 1996/12/31 07:06:35 dawes Exp $ */

#include "Ps.h"
#include "gcstruct.h"
#include "windowstr.h"

int
PsPolyText8(
  DrawablePtr pDrawable,
  GCPtr       pGC,
  int         x,
  int         y,
  int         count,
  char       *string)
{
  if( pDrawable->type==DRAWABLE_PIXMAP )
  {
    DisplayElmPtr   elm;
    PixmapPtr       pix  = (PixmapPtr)pDrawable;
    PsPixmapPrivPtr priv = (PsPixmapPrivPtr)pix->devPrivate.ptr;
    DisplayListPtr  disp;
    GCPtr           gc;

    if ((gc = PsCreateAndCopyGC(pDrawable, pGC)) == NULL) return;

    disp = PsGetFreeDisplayBlock(priv);

    elm  = &disp->elms[disp->nelms];
    elm->type = Text8Cmd;
    elm->gc   = gc;
    elm->c.text8.x      = x;
    elm->c.text8.y      = y;
    elm->c.text8.count  = count;
    elm->c.text8.string = (char *)xalloc(count);
    memcpy(elm->c.text8.string, string, count);
    disp->nelms += 1;
  }
  else
  {
    int          iso;
    int          siz;
    float        mtx[4];
    char        *fnam;
    PsOutPtr     psOut;
    ColormapPtr  cMap;

    if( PsUpdateDrawableGC(pGC, pDrawable, &psOut, &cMap)==FALSE ) return x;
    PsOut_Offset(psOut, pDrawable->x, pDrawable->y);
    PsOut_Color(psOut, PsGetPixelColor(cMap, pGC->fgPixel));
    fnam = PsGetPSFontName(pGC->font);
    if( !fnam ) return x;
    siz = PsGetFontSize(pGC->font, mtx);
    iso = PsIsISOLatin1Encoding(pGC->font);
    if( !siz ) PsOut_TextAttrsMtx(psOut, fnam, mtx, iso);
    else       PsOut_TextAttrs(psOut, fnam, siz, iso);
    PsOut_Text(psOut, x, y, string, count, -1);
  }
  return x;
}

int
PsPolyText16(
  DrawablePtr     pDrawable,
  GCPtr           pGC,
  int             x,
  int             y,
  int             count,
  unsigned short *string)
{
  if( pDrawable->type==DRAWABLE_PIXMAP )
  {
    DisplayElmPtr   elm;
    PixmapPtr       pix  = (PixmapPtr)pDrawable;
    PsPixmapPrivPtr priv = (PsPixmapPrivPtr)pix->devPrivate.ptr;
    DisplayListPtr  disp;
    GCPtr           gc;

    if ((gc = PsCreateAndCopyGC(pDrawable, pGC)) == NULL) return;

    disp = PsGetFreeDisplayBlock(priv);

    elm  = &disp->elms[disp->nelms];
    elm->type = Text16Cmd;
    elm->gc   = gc;
    elm->c.text16.x      = x;
    elm->c.text16.y      = y;
    elm->c.text16.count  = count;
    elm->c.text16.string =
      (unsigned short *)xalloc(count*sizeof(unsigned short));
    memcpy(elm->c.text16.string, string, count*sizeof(unsigned short));
    disp->nelms += 1;
  }
  else
  {
    int   i;
    char *str;
    if( !count ) return x;
    str = (char *)xalloc(count);
    for( i=0 ; i<count ; i++ ) str[i] = string[i];
    PsPolyText8(pDrawable, pGC, x, y, count, str);
    xfree(str);
  }
  return x;
}

void
PsImageText8(
  DrawablePtr pDrawable,
  GCPtr       pGC,
  int         x,
  int         y,
  int         count,
  char       *string)
{
  if( pDrawable->type==DRAWABLE_PIXMAP )
  {
    DisplayElmPtr   elm;
    PixmapPtr       pix  = (PixmapPtr)pDrawable;
    PsPixmapPrivPtr priv = (PsPixmapPrivPtr)pix->devPrivate.ptr;
    DisplayListPtr  disp;
    GCPtr           gc;

    if ((gc = PsCreateAndCopyGC(pDrawable, pGC)) == NULL) return;

    disp = PsGetFreeDisplayBlock(priv);

    elm  = &disp->elms[disp->nelms];
    elm->type = TextI8Cmd;
    elm->gc   = gc;
    elm->c.text8.x      = x;
    elm->c.text8.y      = y;
    elm->c.text8.count  = count;
    elm->c.text8.string = (char *)xalloc(count);
    memcpy(elm->c.text8.string, string, count);
    disp->nelms += 1;
  }
  else
  {
    int          iso;
    int          siz;
    float        mtx[4];
    char        *fnam;
    PsOutPtr     psOut;
    ColormapPtr  cMap;

    if( PsUpdateDrawableGC(pGC, pDrawable, &psOut, &cMap)==FALSE ) return;
    PsOut_Offset(psOut, pDrawable->x, pDrawable->y);
    PsOut_Color(psOut, PsGetPixelColor(cMap, pGC->fgPixel));
    fnam = PsGetPSFontName(pGC->font);
    if( !fnam ) return;
    siz = PsGetFontSize(pGC->font, mtx);
    iso = PsIsISOLatin1Encoding(pGC->font);
    if( !siz ) PsOut_TextAttrsMtx(psOut, fnam, mtx, iso);
    else       PsOut_TextAttrs(psOut, fnam, siz, iso);
    PsOut_Text(psOut, x, y, string, count, PsGetPixelColor(cMap, pGC->bgPixel));
  }
}

void
PsImageText16(
  DrawablePtr     pDrawable,
  GCPtr           pGC,
  int             x,
  int             y,
  int             count,
  unsigned short *string)
{
  if( pDrawable->type==DRAWABLE_PIXMAP )
  {
    DisplayElmPtr   elm;
    PixmapPtr       pix  = (PixmapPtr)pDrawable;
    PsPixmapPrivPtr priv = (PsPixmapPrivPtr)pix->devPrivate.ptr;
    DisplayListPtr  disp;
    GCPtr           gc;

    if ((gc = PsCreateAndCopyGC(pDrawable, pGC)) == NULL) return;

    disp = PsGetFreeDisplayBlock(priv);

    elm  = &disp->elms[disp->nelms];
    elm->type = TextI16Cmd;
    elm->gc   = gc;
    elm->c.text16.x      = x;
    elm->c.text16.y      = y;
    elm->c.text16.count  = count;
    elm->c.text16.string =
      (unsigned short *)xalloc(count*sizeof(unsigned short));
    memcpy(elm->c.text16.string, string, count*sizeof(unsigned short));
    disp->nelms += 1;
  }
  else
  {
    int   i;
    char *str;
    if( !count ) return;
    str = (char *)xalloc(count);
    for( i=0 ; i<count ; i++ ) str[i] = string[i];
    PsImageText8(pDrawable, pGC, x, y, count, str);
    free(str);
  }
}

void
PsImageGlyphBlt(
  DrawablePtr   pDrawable,
  GCPtr         pGC,
  int           x,
  int           y,
  unsigned int  nGlyphs,
  CharInfoPtr  *pCharInfo,
  pointer       pGlyphBase)
{
  /* NOT TO BE IMPLEMENTED */
}

void
PsPolyGlyphBlt(
  DrawablePtr   pDrawable,
  GCPtr         pGC,
  int           x,
  int           y,
  unsigned int  nGlyphs,
  CharInfoPtr  *pCharInfo,
  pointer       pGlyphBase)
{
  /* NOT TO BE IMPLEMENTED */
}
