/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/sis/sis_pntwin.c,v 1.1 1997/01/12 10:43:10 dawes Exp $ */



#include "vga256.h"
#include "cfb16.h"
#include "cfb24.h"
#include "cfb32.h"
#include "xf86.h"

#include "sis_driver.h"

extern int vgaBitsPerPixel;
#if 1
#define sisFillBoxTile32 \
(vgaBitsPerPixel > 16? \
(vgaBitsPerPixel == 32? cfb32FillBoxTile32 : cfb24FillBoxTile32) : \
(vgaBitsPerPixel == 16? cfb16FillBoxTile32 : vga256FillBoxTile32))
#else
#define sisFillBoxTile32 siscfbFillBoxTile32
#endif
#define sisFillBoxTileOdd \
(vgaBitsPerPixel > 16? \
(vgaBitsPerPixel == 32? cfb32FillBoxTileOdd : cfb24FillBoxTileOdd) : \
(vgaBitsPerPixel == 16? cfb16FillBoxTileOdd : vga256FillBoxTileOdd))
	

void
sisPaintWindow(pWin, pRegion, what)
    WindowPtr	pWin;
    RegionPtr	pRegion;
    int		what;
{
	register cfbPrivWin *pPrivWin;
	WindowPtr pBgWin;
	
#ifdef DEBUG
    ErrorF("sisPaintWindow()\n");
#endif
	pPrivWin = cfbGetWindowPrivate(pWin);
	
	switch (what)
	{
	case PW_BACKGROUND:
		switch (pWin->backgroundState)
		{
		case None:
			break;
		case ParentRelative:
			do
			{
				pWin = pWin->parent;
			} while (pWin->backgroundState == ParentRelative);
			(*pWin->drawable.pScreen->PaintWindowBackground)(pWin,
					 		pRegion, what);
			break;
		case BackgroundPixmap:
			if (pPrivWin->fastBackground)
			{
				sisFillBoxTile32 ((DrawablePtr)pWin,
					(int)REGION_NUM_RECTS(pRegion),
					REGION_RECTS(pRegion),
					pPrivWin->pRotatedBackground);
			}
			else
			{
				sisFillBoxTileOdd ((DrawablePtr)pWin,
					(int)REGION_NUM_RECTS(pRegion),
					REGION_RECTS(pRegion),
					pWin->background.pixmap,
					(int) pWin->drawable.x,
					(int) pWin->drawable.y);
			}
			break;
		case BackgroundPixel:
			siscfbFillBoxSolid ((DrawablePtr)pWin,
					(int)REGION_NUM_RECTS(pRegion),
					REGION_RECTS(pRegion),
					pWin->background.pixel,
					pWin->background.pixel,
					GXcopy);
					break;
		}
		break;
	case PW_BORDER:
		if (pWin->borderIsPixel)
		{
			siscfbFillBoxSolid ((DrawablePtr)pWin,
				(int)REGION_NUM_RECTS(pRegion),
				REGION_RECTS(pRegion),
				pWin->border.pixel,
				pWin->border.pixel,
				GXcopy);
		}
		else if (pPrivWin->fastBorder)
		{
			sisFillBoxTile32 ((DrawablePtr)pWin,
				(int)REGION_NUM_RECTS(pRegion),
				REGION_RECTS(pRegion),
				pPrivWin->pRotatedBorder);
		}
		else
		{
			for (pBgWin = pWin;
				pBgWin->backgroundState == ParentRelative;
				pBgWin = pBgWin->parent);

			sisFillBoxTileOdd ((DrawablePtr)pWin,
				(int)REGION_NUM_RECTS(pRegion),
				REGION_RECTS(pRegion),
				pWin->border.pixmap,
				(int) pBgWin->drawable.x,
				(int) pBgWin->drawable.y);
		}
		break;
	}
}




