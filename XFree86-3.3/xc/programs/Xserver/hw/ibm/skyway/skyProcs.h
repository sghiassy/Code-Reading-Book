/*
 * $XConsortium: skyProcs.h,v 1.4 91/12/11 21:25:38 eswu Exp $
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND 
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

/*
 * skyProcs.h - function calls
 */

#ifndef _SKYPROCS_H
#define _SKYPROCS_H

extern Bool skyScreenInit();
extern Bool skyScreenClose();

extern int  skyProbe();

extern void skySaveState();
extern void skyRestoreState();
extern void skyQueryBestSize();

extern Bool skyRealizeCursor();
extern Bool skyUnrealizeCursor();
extern Bool skyDisplayCursor();

extern void skyStoreColors();
extern void skyInstallColormap();
extern void skyUninstallColormap();
extern int  skyListInstalledColormaps();
extern void skyRefreshColormaps();

extern Bool cfbScreenInit();
extern void skyDoBitbltCopy();
extern void skyCopyWindow();

extern Bool skyCreateGC();
extern RegionPtr skyCopyArea();

extern void skyPolyFillRect();
/*
extern void skyPolyFillRectSolid_1Rect();
extern void skyPolyFillRectTile_1Rect();
extern void skyPolyFillRectStipple_1Rect();
*/

/* ----------------- internal helper functions ----------------- */
extern int  skyHdwrInit();

extern unsigned long skySetTileStip();
extern unsigned long skySetSolid();
extern void skyNewTileStipID();

#endif /* _SKYPROC_H */
