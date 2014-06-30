/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3name.h,v 3.14 1997/01/18 06:55:04 dawes Exp $ */
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
/* $XConsortium: s3name.h /main/7 1996/10/19 17:57:11 kaleb $ */

#ifndef S3_NAME_H
#define S3_NAME_H

#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP) || (defined (sun) && defined (i386) && defined (SVR4) && defined (__STDC__) && !defined (__GNUC__))
#define CATNAME(prefix,subname) prefix##subname
#else
#define CATNAME(prefix,subname) prefix/**/subname
#endif

#ifdef S3_MMIO
#define CUSTOMNAME(name) CATNAME(mmio928,name)
#endif

/* this generates names for new driver BL 0816150096 */
#ifdef S3_NEWMMIO
#define CUSTOMNAME(name) CATNAME(newmmio,name)
#endif

#ifdef CUSTOMNAME
#define s3Initialize CUSTOMNAME(_s3Initialize)
#define s3RenewCursorColor  CUSTOMNAME(_s3RenewCursorColor)
#define s3EnterLeaveVT CUSTOMNAME(_s3EnterLeaveVT)
#define s3AdjustFrame CUSTOMNAME(_s3AdjustFrame)
#define s3SwitchMode CUSTOMNAME(_s3SwitchMode)
#define s3DPMSSet CUSTOMNAME(_s3DPMSSet)
#define s3CloseScreen CUSTOMNAME(_s3CloseScreen)
#define s3SaveScreen CUSTOMNAME(_s3SaveScreen)
#define s3ListInstalledColormaps CUSTOMNAME(_s3ListInstalledColormaps)
#define s3RestoreDACvalues CUSTOMNAME(_s3RestoreDACvalues)
#define s3GetInstalledColormaps CUSTOMNAME(_s3GetInstalledColormaps)
#define s3StoreColors CUSTOMNAME(_s3StoreColors)
#define s3InstallColormap CUSTOMNAME(_s3InstallColormap)
#define s3UninstallColormap CUSTOMNAME(_s3UninstallColormap)
#define s3RestoreColor0 CUSTOMNAME(_s3RestoreColor0)
#define s3CreateGC CUSTOMNAME(_s3CreateGC)
#define s3CreateGC16 CUSTOMNAME(_s3CreateGC16)
#define s3CreateGC24 CUSTOMNAME(_s3CreateGC24)
#define s3CreateGC32 CUSTOMNAME(_s3CreateGC32)
#define s3SolidFSpans CUSTOMNAME(_s3SolidFSpans)
#define s3TiledFSpans CUSTOMNAME(_s3TiledFSpans)
#define s3StipFSpans CUSTOMNAME(_s3StipFSpans)
#define s3OStipFSpans CUSTOMNAME(_s3OStipFSpans)
#define s3SetSpans CUSTOMNAME(_s3SetSpans)
#define s3GetSpans CUSTOMNAME(_s3GetSpans)
#define s3CopyWindow CUSTOMNAME(_s3CopyWindow)
#define s3ImageInit CUSTOMNAME(_s3ImageInit)
#define s3ImageWriteNoMem CUSTOMNAME(_s3ImageWriteNoMem)
#define s3ImageStipple CUSTOMNAME(_s3ImageStipple)
#define s3ImageOpStipple CUSTOMNAME(_s3ImageOpStipple)
#define s3SaveAreas CUSTOMNAME(_s3SaveAreas)
#define s3RestoreAreas CUSTOMNAME(_s3RestoreAreas)
#define s3BSFuncRec CUSTOMNAME(_s3BSFuncRec)
#define s3ScreenInit CUSTOMNAME(_s3ScreenInit)
#define s3CopyArea CUSTOMNAME(_s3CopyArea)
#define s3FindOrdering CUSTOMNAME(_s3FindOrdering)
#define s3CopyPlane CUSTOMNAME(_s3CopyPlane)
#define s3InitFrect CUSTOMNAME(_s3InitFrect)
#define s3CImageFill CUSTOMNAME(_s3CImageFill)
#define s3CImageStipple CUSTOMNAME(_s3CImageStipple)
#define s3CImageOpStipple CUSTOMNAME(_s3CImageOpStipple)
#define s3PolyPoint CUSTOMNAME(_s3PolyPoint)
#define s3Line CUSTOMNAME(_s3Line)
#define s3Segment CUSTOMNAME(_s3Segment)
#define s3PolyFillRect CUSTOMNAME(_s3PolyFillRect)
#define s3RealizeFont CUSTOMNAME(_s3RealizeFont)
#define s3UnrealizeFont CUSTOMNAME(_s3UnrealizeFont)
#define s3NoCPolyText CUSTOMNAME(_s3NoCPolyText)
#define s3SimpleStipple CUSTOMNAME(_s3SimpleStipple)
#define s3FontStipple CUSTOMNAME(_s3FontStipple)
#define s3NoCImageText CUSTOMNAME(_s3NoCImageText)
#define s3CursorInit CUSTOMNAME(_s3CursorInit)
#define s3ShowCursor CUSTOMNAME(_s3ShowCursor)
#define s3HideCursor CUSTOMNAME(_s3HideCursor)
#define s3RestoreCursor CUSTOMNAME(_s3RestoreCursor)
#define s3RepositionCursor CUSTOMNAME(_s3RepositionCursor)
#define s3WarpCursor CUSTOMNAME(_s3WarpCursor)
#define s3RecolorCursor  CUSTOMNAME(_s3RecolorCursor)
#define s3QueryBestSize CUSTOMNAME(_s3QueryBestSize)
#define s3Dline CUSTOMNAME(_s3Dline)
#define s3Dsegment CUSTOMNAME(_s3Dsegment)
#define s3FontCache8Init CUSTOMNAME(_s3FontCache8Init)
#define s3GlyphWrite CUSTOMNAME(_s3GlyphWrite)
#define s3CacheMoveBlock CUSTOMNAME(_s3CacheMoveBlock)
#define s3SetVidPage CUSTOMNAME(_s3SetVidPage)
#endif /* CUSTOMNAME */

#endif /* S3_NAME_H */
