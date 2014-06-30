/* $XFree86: xc/programs/Xserver/hw/xfree86/mono/mfb.banked/mfbmap.h,v 3.2 1996/12/23 06:48:38 dawes Exp $ */
/* mfbmap.h */





/* $XConsortium: mfbmap.h /main/3 1996/02/21 17:50:06 kaleb $ */

#ifndef _MFBMAP_H
#define _MFBMAP_H

#define InverseAlu  mono_InverseAlu
#define endtab  mono_endtab
#define mask  mono_mask
#define mergeRopBits  mono_mergeRopBits
#define mfbAllocatePrivates  mono_mfbAllocatePrivates
#define mfbBSFuncRec  mono_mfbBSFuncRec
#define mfbBlackSolidFS  mono_mfbBlackSolidFS
#define mfbBlackStippleFS  mono_mfbBlackStippleFS
#define mfbBresD  mono_mfbBresD
#define mfbBresS  mono_mfbBresS
#define mfbChangeWindowAttributes  mono_mfbChangeWindowAttributes
#define mfbCloseScreen  mono_mfbCloseScreen
#define mfbCopyArea  mono_mfbCopyArea
#define mfbCopyPixmap  mono_mfbCopyPixmap
#define mfbCopyPlane  mono_mfbCopyPlane
#define mfbCopyRotatePixmap  mono_mfbCopyRotatePixmap
#define mfbCopyWindow  mono_mfbCopyWindow
#define mfbCreateColormap  mono_mfbCreateColormap
#define mfbCreateDefColormap  mono_mfbCreateDefColormap
#define mfbCreateGC  mono_mfbCreateGC
#define mfbCreatePixmap  mono_mfbCreatePixmap
#define mfbCreateWindow  mono_mfbCreateWindow
#define mfbDestroyColormap  mono_mfbDestroyColormap
#define mfbDestroyPixmap  mono_mfbDestroyPixmap
#define mfbDestroyWindow  mono_mfbDestroyWindow
#define mfbDoBitblt  mono_mfbDoBitblt
#define mfbDoBitbltCopy  mono_mfbDoBitbltCopy
#define mfbDoBitbltCopyInverted  mono_mfbDoBitbltCopyInverted
#define mfbDoBitbltGeneral  mono_mfbDoBitbltGeneral
#define mfbDoBitbltOr  mono_mfbDoBitbltOr
#define mfbDoBitbltXor  mono_mfbDoBitbltXor
#define mfbFillPolyBlack  mono_mfbFillPolyBlack
#define mfbFillPolyInvert  mono_mfbFillPolyInvert
#define mfbFillPolyWhite  mono_mfbFillPolyWhite
#define mfbGCPrivateIndex  mono_mfbGCPrivateIndex
#define mfbGetImage  mono_mfbGetImage
#define mfbGetSpans  mono_mfbGetSpans
#define mfbHorzS  mono_mfbHorzS
#define mfbImageGlyphBltBlack  mono_mfbImageGlyphBltBlack
#define mfbImageGlyphBltWhite  mono_mfbImageGlyphBltWhite
#define mfbInstallColormap  mono_mfbInstallColormap
#define mfbInvertSolidFS  mono_mfbInvertSolidFS
#define mfbInvertStippleFS  mono_mfbInvertStippleFS
#define mfbLineSD  mono_mfbLineSD
#define mfbLineSS  mono_mfbLineSS
#define mfbListInstalledColormaps  mono_mfbListInstalledColormaps
#define mfbMapWindow  mono_mfbMapWindow
#define mfbPadPixmap  mono_mfbPadPixmap
#define mfbPaintWindow  mono_mfbPaintWindow
#define mfbPixmapToRegion  mono_mfbPixmapToRegion
#define mfbPolyFillArcSolid  mono_mfbPolyFillArcSolid
#define mfbPolyFillRect  mono_mfbPolyFillRect
#define mfbPolyGlyphBltBlack  mono_mfbPolyGlyphBltBlack
#define mfbPolyGlyphBltInvert  mono_mfbPolyGlyphBltInvert
#define mfbPolyGlyphBltWhite  mono_mfbPolyGlyphBltWhite
#define mfbPolyPoint  mono_mfbPolyPoint
#define mfbPositionWindow  mono_mfbPositionWindow
#define mfbPushPixels  mono_mfbPushPixels
#define mfbPutImage  mono_mfbPutImage
#define mfbQueryBestSize  mono_mfbQueryBestSize
#define mfbRealizeFont  mono_mfbRealizeFont
#define mfbReduceRop  mono_mfbReduceRop
#define mfbRegisterCopyPlaneProc  mono_mfbRegisterCopyPlaneProc
#define mfbResolveColor  mono_mfbResolveColor
#define mfbRestoreAreas  mono_mfbRestoreAreas
#define mfbSaveAreas  mono_mfbSaveAreas
#define mfbScreenInit  mono_mfbScreenInit
#define mfbSegmentSD  mono_mfbSegmentSD
#define mfbSegmentSS  mono_mfbSegmentSS
#define mfbSetScanline  mono_mfbSetScanline
#define mfbSetSpans  mono_mfbSetSpans
#define mfbSolidBlackArea  mono_mfbSolidBlackArea
#define mfbSolidInvertArea  mono_mfbSolidInvertArea
#define mfbSolidPP  mono_mfbSolidPP
#define mfbSolidWhiteArea  mono_mfbSolidWhiteArea
#define mfbStippleBlackArea  mono_mfbStippleBlackArea
#define mfbStippleInvertArea  mono_mfbStippleInvertArea
#define mfbStippleWhiteArea  mono_mfbStippleWhiteArea
#define mfbTEGlyphBltBlack  mono_mfbTEGlyphBltBlack
#define mfbTEGlyphBltWhite  mono_mfbTEGlyphBltWhite
#define mfbTileAreaPPW  mono_mfbTileAreaPPW
#define mfbTileAreaPPWCopy  mono_mfbTileAreaPPWCopy
#define mfbTileAreaPPWGeneral  mono_mfbTileAreaPPWGeneral
#define mfbTileFS  mono_mfbTileFS
#define mfbUninstallColormap  mono_mfbUninstallColormap
#define mfbUnmapWindow  mono_mfbUnmapWindow
#define mfbUnnaturalStippleFS  mono_mfbUnnaturalStippleFS
#define mfbUnnaturalTileFS  mono_mfbUnnaturalTileFS
#define mfbUnrealizeFont  mono_mfbUnrealizeFont
#define mfbValidateGC  mono_mfbValidateGC
#define mfbVertS  mono_mfbVertS
#define mfbWhiteSolidFS  mono_mfbWhiteSolidFS
#define mfbWhiteStippleFS  mono_mfbWhiteStippleFS
#define mfbWindowPrivateIndex  mono_mfbWindowPrivateIndex
#define mfbXRotatePixmap  mono_mfbXRotatePixmap
#define mfbYRotatePixmap  mono_mfbYRotatePixmap
#define mfbZeroPolyArcSS  mono_mfbZeroPolyArcSS
#define partmasks  mono_partmasks
#define rmask  mono_rmask
#define starttab  mono_starttab

#endif
