/*
 * $XConsortium: pgcCmap.c,v 1.2 91/07/16 13:11:53 jap Exp $
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

#define NEED_EVENTS

#include "X.h"
#include "Xproto.h"
#include "resource.h"
#include "misc.h"
#include "scrnintstr.h"
#include "screenint.h"
#include "colormapst.h"
#include "colormap.h"
#include "pixmapstr.h"

#include "ibmTrace.h"

#include "pgc.h"

extern pgcScreenRec pgcScreenInfo[] ; 

void
pgcStoreColors( pmap, ndef, pdefs )
register ColormapPtr pmap ;
register int ndef ;
register xColorItem *pdefs ;
{
   register void (*fnp)() ;
   register int i ;
   int this_is_installed ;
   int index ;
   pgcScreenPtr pPGCScreen ;

   index = pmap->pScreen->myNum ;
   pPGCScreen = &pgcScreenInfo[index] ;

   this_is_installed = ( pmap == pPGCScreen->InstalledColormap ) ;

   for ( fnp = pPGCScreen->setColor ; ndef-- ; pdefs++ ) {
	/* first, resolve colors into possible colors */
	(*pmap->pScreen->ResolveColor)(	&(pdefs->red),
					&(pdefs->green),
					&(pdefs->blue),
					pmap->pVisual ) ;
	if ( ( i = pdefs->pixel ) > pmap->pVisual->ColormapEntries )
		ErrorF("pgcStoreColors: Bad pixel number\n") ;
	else {
		/* remember that the index to red is really the entry number */
		pmap->red[i].co.local.red   = pdefs->red ;
		pmap->red[i].co.local.green = pdefs->green ;
		pmap->red[i].co.local.blue  = pdefs->blue ;
		if ( this_is_installed )
    		   (*fnp)( i, pdefs->red, pdefs->green, pdefs->blue, index);
	}
    }

    return ;
}

int
pgcListInstalledColormaps( pScreen, pCmapList )
register ScreenPtr pScreen ;
register Colormap *pCmapList ;
{
   int index ;
   pgcScreenPtr pPGCScreen ;

   index = pScreen->myNum ;
   pPGCScreen = &pgcScreenInfo[index] ;

   	if ( pPGCScreen->InstalledColormap ) {
                *pCmapList = pPGCScreen->InstalledColormap->mid ;
		return 1 ;
	}
	else {
		*pCmapList = 0 ;
		return 0 ;
	}
	/*NOTREACHED*/
}

void
pgcInstallColormap( pColormap ) 
register ColormapPtr pColormap ;
{
   register int i ;
   register void (*fnp)() ;
   int index ;
   pgcScreenPtr pPGCScreen ;
   ScreenPtr pScreen = pColormap->pScreen ;

   TRACE( ( "pgcInstallColormap(pColormap=x%x)", pColormap ) ) ;

   index = pScreen->myNum ;
   pPGCScreen = &pgcScreenInfo[index] ;

   if ( !pColormap ) /* Re-Install of NULL map ?? */
	return ;

   fnp = pPGCScreen->setColor ;

   for ( i = pColormap->pVisual->ColormapEntries ; i-- ;  )
	(*fnp)( i,
		pColormap->red[i].co.local.red,
		pColormap->red[i].co.local.green,
		pColormap->red[i].co.local.blue,
		index ) ;

   if ( pPGCScreen->InstalledColormap != pColormap ) {
	if ( pPGCScreen->InstalledColormap ) /* Remember it may not exist */
	    WalkTree( pScreen, TellLostMap,
		      &(pPGCScreen->InstalledColormap->mid) ) ;
	pPGCScreen->InstalledColormap = pColormap ;
	WalkTree( pScreen, TellGainedMap, &(pColormap->mid) ) ;

    }

   return ;
}

void
pgcUninstallColormap( pColormap ) 
register ColormapPtr pColormap ;
{
   register ColormapPtr pCmap ;

   TRACE( ( "pgcUninstallColormap(pColormap=x%x)\n", pColormap ) ) ;

   pCmap = (ColormapPtr) LookupIDByType( pColormap->pScreen->defColormap,
				   RT_COLORMAP ) ;
   if ( pCmap != pColormap ) /* never uninstall the default map */
	(* pColormap->pScreen->InstallColormap)( pCmap ) ;

   return ;
}

pgcRefreshColormaps(pScreen)
ScreenPtr pScreen ;
{
   ColormapPtr pCmap ;
   int index ;
   pgcScreenPtr pPGCScreen ;

   index = pScreen->myNum ;
   pPGCScreen = &pgcScreenInfo[index] ;

   pCmap = pPGCScreen->InstalledColormap ;
   pgcInstallColormap(pCmap);

}
