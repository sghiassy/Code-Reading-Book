/*
 * $XConsortium: ibmSave.c /main/4 1995/12/05 15:44:41 matt $
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
#define NEED_REPLIES
#include "Xproto.h"
#include "misc.h"
#include "scrnintstr.h"
#include "os.h"
#include "regionstr.h"
#include "windowstr.h"
#include "input.h"
#include "resource.h"
#include "colormapst.h"
#include "cursorstr.h"
#include "dixstruct.h"
#include "gc.h"
#include "gcstruct.h"
#include "servermd.h"
#include "miscstruct.h"
#include "pixmapstr.h"
#include "colormap.h"

#include "compiler.h"

#include "ibmKeybd.h"
#include "ibmMouse.h"

#include "ibmScreen.h"
#include "OSio.h"
#include "ibmTrace.h"

#include <stdio.h>

extern WindowPtr *WindowTable;

/***====================================================================***/

static	WindowPtr obscuringWins[MAXSCREENS];
static	int	realSaverTime;
static	int	realSaverInterval;
	int	ibmRefreshOnActivate= TRUE ;

/***====================================================================***/

void
ibmObscureScreen(pScreen)
ScreenPtr	pScreen;
{
WindowPtr	pWin;
unsigned	mask= CWBackPixel;
XID		attributes[2];
int		result;

    TRACE(("ibmObscureScreen()\n"));
    attributes[0]= pScreen->blackPixel;
    obscuringWins[pScreen->myNum]= CreateWindow(
			FakeClientID(0),		/* window id */
			WindowTable[pScreen->myNum],	/* parent */
			0,0,				/* x,y */
			pScreen->width,pScreen->height,	/* width,height */
			0,				/* border width */
			InputOutput,			/* class */
			mask, attributes,		/* attributes */
			0,				/* depth */
			(ClientPtr)NULL,		/* client */
			WindowTable[pScreen->myNum]->optional->visual,
							/* visual */
			&result);			/* error */

    pWin = obscuringWins[pScreen->myNum] ;
    TRACE(("PWIN is %x\n",pWin));
    TRACE(("PWINID is %x\n",pWin->drawable.id));

    AddResource(pWin->drawable.id,RT_WINDOW,(pointer)pWin);
    pWin->overrideRedirect= TRUE;
    TRACE(("before Map is \n",pWin));
    MapWindow( pWin, (ClientPtr)NULL);

    return;
}

/***====================================================================***/

void
ibmExposeScreen(pScreen)
ScreenPtr	pScreen;
{
WindowPtr	pWin= obscuringWins[pScreen->myNum];

    TRACE(("ibmExposeScreen(0x%x)\n",pScreen));
    FreeResource(pWin->drawable.id,RC_VANILLA);
    obscuringWins[pScreen->myNum]= NULL;
    return;
}

/***====================================================================***/

static	PixmapPtr saveMaps[MAXSCREENS] = {(PixmapPtr) 0, (PixmapPtr) 0};
   /* must be zero initially */

PixmapPtr
ibmSaveScreenData(pScreen,pSaveFunc)
ScreenPtr	pScreen;
void		(*pSaveFunc)();
{
RegionPtr	pRgn;
PixmapPtr	pPix;

    TRACE(("ibmSaveScreenData(0x%x)\n",pScreen));
    pRgn = &(WindowTable[pScreen->myNum]->winSize);

    if (!saveMaps[pScreen->myNum])
     saveMaps[pScreen->myNum] = (*pScreen->CreatePixmap)(pScreen,
						REGION_RECTS(pRgn)->x2,
						REGION_RECTS(pRgn)->y2,
						pScreen->rootDepth);
    pPix = saveMaps[pScreen->myNum] ;
    (*pSaveFunc)(pPix,pRgn,0,0);
    return(pPix);
}

/***====================================================================***/

void
ibmRestoreScreenData(pScreen,pRestoreFunc)
ScreenPtr	pScreen;
void		(*pRestoreFunc)();
{
RegionPtr	pRgn;

    TRACE(("ibmRestoreScreenData(0x%x)\n",pScreen));
    pRgn= &(WindowTable[pScreen->myNum]->winSize);
    (*pRestoreFunc)(saveMaps[pScreen->myNum],pRgn,0,0);
    return;
}

/***====================================================================***/

#ifdef AIXV3
/*

Hotkey Away:
	1) when we get a retract, call "ibmDeactivateScreens()"
	2) if this is the first retract (to check use: "if( IAmListening ){"),
		call a function to turn off direct window access. Also call
		"DontListenToAnybody()" and set "IAmListening" to FALSE.
	3) find out which screen is getting the retract (with the function
		"WhoIsGettingTheRetract()" and call the save function for
		that screen.

Hotkey Back:
	1) when we get a grant, it will be on the virtual terminal that
		we use for input.
	2) go through the list of screens and make sure the virtual terminal
		is active.  If it is not active call
		"hftActivateVT(ibmScreenFD(scrn))"  and wait for a grant.
		(I'm not sure we will get the grants on all the virtual
		terminals)
	3) go through the list of screens again and call the restore function
		for that screen only if the screen is inactive.
	4) call "PayAttentionToClientsAgain()" to wake up the clients.
	5) call a function to turn on direct window access.

*/
#else /* AIXV3 */
#endif /* AIXV3 */

#ifdef AIXV3
/* file descriptor to send the Release to after a Retract has been received */
int  RetractFD;
int  IAmListening = TRUE;

void
TurnOffDirectWindowAccess()
{
}

void
TurnOnDirectWindowAccess()
{
}


void
ibmDeactivateScreens()
{
ScreenPtr 	pScreen;
void 		(*fnp)();
int		scrn;
    unsigned  devid;

    TRACE(("ibmDeactivateScreens()\n"));
#ifdef OS_SaveState
    OS_SaveState();
#endif

    if( IAmListening ){
	TurnOffDirectWindowAccess();
	DontListenToAnybody();
	IAmListening = FALSE;
    }

    devid = WhoIsGettingTheRetract();

    for (scrn = 0; scrn < ibmNumScreens; scrn++) {
      if( devid == ibmDeviceID(scrn) ){
	pScreen = ibmScreen(scrn);
	RetractFD = ibmScreenFD(scrn);
	TRACE(("ibmDeactivateScreens() scrn=%d  pScreen->myNum=%d\n",scrn,pScreen->myNum));
	if ((!pScreen)||(ibmScreenState(pScreen->myNum)!=SCREEN_ACTIVE)){
	    TRACE(("ibmDeactivateScreens() screen %d not active, not saving\n",scrn));
	    break;;
	}
	if (!pScreen) {
	    ErrorF("Trying to deactivate null screen\n");
	    return ;
	}
	
	if (ibmRefreshOnActivate)	ibmObscureScreen(pScreen);

	/* find out what the screen wants to do vis-a-vis saving */
	fnp = ibmScreens[scrn]->ibm_SaveFunc;
	if (fnp) {
	    TRACE(("ibmDeactivateScreens() before calling save func for %d\n",scrn));
	    (*fnp)(pScreen,!ibmRefreshOnActivate);
	    TRACE(("ibmDeactivateScreens() after calling save func for %d\n",scrn));
	}
	else{
	    TRACE(("ibmDeactivateScreens() screen %d doesn't have a save function\n",scrn));
	}
	ibmSetScreenState(pScreen->myNum,SCREEN_INACTIVE);
	break;
      }
    }

    return;
}

#else /* AIXV3 */

void
ibmDeactivateScreens()
{
ScreenPtr 	pScreen;
void 		(*fnp)();
int		scrn;

    TRACE(("ibmDeactivateScreens()\n"));
#ifdef OS_SaveState
    OS_SaveState();
#endif

    for (scrn = 0; scrn < ibmNumScreens; scrn++) {

	pScreen = ibmScreen(scrn);
	TRACE(("ibmDeactivateScreens() scrn=%d  pScreen->myNum=%d\n",scrn,pScreen->myNum));
	if ((!pScreen)||(ibmScreenState(pScreen->myNum)!=SCREEN_ACTIVE)){
	    TRACE(("ibmDeactivateScreens() screen %d not active, not saving\n",scrn));
	    continue;
	}
	if (!pScreen) {
	    ErrorF("Trying to deactivate null screen\n");
	    return ;
	}
	
	if (ibmRefreshOnActivate)	ibmObscureScreen(pScreen);

	/* find out what the screen wants to do vis-a-vis saving */
	fnp = ibmScreens[scrn]->ibm_SaveFunc;
	if (fnp) {
	    TRACE(("ibmDeactivateScreens() before calling save func for %d\n",scrn));
	    (*fnp)(pScreen,!ibmRefreshOnActivate);
	    TRACE(("ibmDeactivateScreens() after calling save func for %d\n",scrn));
	}
	else{
	    TRACE(("ibmDeactivateScreens() screen %d doesn't have a save function\n",scrn));
	}
	ibmSetScreenState(pScreen->myNum,SCREEN_INACTIVE);
    }
    DontListenToAnybody();
    return;
}
#endif /* AIXV3 */
	
void
ibmReactivateScreens()
{
ScreenPtr 	pScreen;
void 		(*fnp)();
int		scrn;
WindowPtr	pRoot;
ColormapPtr	pCmap;


    TRACE(("ibmReactivateScreens()\n"));

#ifdef OS_RestoreState
    OS_RestoreState();
#endif

    PayAttentionToClientsAgain();

#ifdef AIXV3

    IAmListening = TRUE;


#ifndef _IBM_LFT
    for (scrn = 0; scrn < ibmNumScreens; scrn++) {
	hftActivateVT(ibmScreenFD(scrn));
	/* wait for grant here if we need to */
    }
#endif

#endif /* AIXV3 */


    for (scrn = 0; scrn < ibmNumScreens; scrn++) {
	pScreen = ibmScreen(scrn);
	TRACE(("ibmReactivateScreens() scrn=%d  pScreen->myNum=%d\n",scrn,pScreen->myNum));

	if ((!pScreen)||(ibmScreenState(pScreen->myNum)!=SCREEN_INACTIVE)){
	    TRACE(("ibmReactivateScreens() screen %d is not inactive, not restoring\n",scrn));
	    continue;
	}
		
	ibmSetScreenState(pScreen->myNum,SCREEN_ACTIVE);

	fnp = ibmScreens[scrn]->ibm_RestoreFunc;
	if (fnp){
	    TRACE(("ibmReactivateScreens() before calling restore func for screen %d\n",scrn));
	    (*fnp)(pScreen,!ibmRefreshOnActivate);
	    TRACE(("ibmReactivateScreens() after calling restore func for screen %d\n",scrn));
	}
	else{
	    TRACE(("ibmReactivateScreens() screen %d does not have a restore function\n",scrn));
	}
	if (ibmRefreshOnActivate)	ibmExposeScreen(pScreen);
    }

#ifdef AIXV3
    TurnOnDirectWindowAccess();
#endif /* AIXV3 */

    return;
}
