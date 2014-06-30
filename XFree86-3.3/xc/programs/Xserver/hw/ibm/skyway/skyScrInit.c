/*
 * $XConsortium: skyScrInit.c /main/7 1995/12/05 15:45:03 matt $ 
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991 
 *
 * All Rights Reserved 
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of IBM not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS, IN NO EVENT SHALL IBM BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 */


/*
 * skyScrInit.c - initialize display and screen structure
 */


#include <sys/types.h>
#ifndef _IBM_LFT
#include <sys/hft.h>
#else
#include <sys/rcmioctl.h>
#endif  /* _IBM_LFT */
#include <sys/entdisp.h>
#include <sys/rcm_win.h>
#include <sys/aixgsc.h>

#define NEED_REPLIES
#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "misc.h"
#include "os.h"
#include "windowstr.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "extnsionst.h"
#include "dixstruct.h"
#include "resource.h"
#include "opaque.h"
#include "inputstr.h"
#include "cursorstr.h"
#include "servermd.h"
#include "ibmScreen.h"
#include "ibmTrace.h"

#include "aixCursor.h"
#include "mipointer.h"
#include "misprite.h"
#include "OSio.h"

#include "skyProcs.h"
#include "skyHdwr.h"
#include "skyReg.h"
#include "skyPriv.h"

#ifdef SKY_MBUF_HACK
#define _MULTIBUF_SERVER_
#include "multibufst.h"
#endif

static unsigned long ddpGeneration=0;
unsigned long ddpGCPrivateIndex;


#ifndef _IBM_LFT
uint		skyHandle[MAXSCREENS];
#else
gsc_handle	skyHandle[MAXSCREENS];
#endif

ulong           SKY_SEGMENT[MAXSCREENS];
ulong           SKY_VRAM_BASE[MAXSCREENS];
ulong           SKY_VRAM_START[MAXSCREENS];
ulong           SKY_DMA0[MAXSCREENS];
ulong           SKY_DMA1[MAXSCREENS];
ulong           SKY_DMA2[MAXSCREENS];
ulong           SKY_DMA3[MAXSCREENS];

skyIORegPtr     IOREG[MAXSCREENS];		/* Skyway IO Registers   */
skyCopRegPtr    COPREG[MAXSCREENS];		/* Coprocessor registers */
skyCopRegRec    COPREG_SHADOW[MAXSCREENS];	/* Shadow of hdwr values */

ulong          *SKY_TILESTIP[MAXSCREENS];
ulong           SKY_TILESTIPID_CTR[MAXSCREENS];


extern void     miRecolorCursor();
extern void     miPaintWindow();



Bool
skyScreenInit(scrnNum, pScreen, argc, argv)
    int             scrnNum;
    ScreenPtr       pScreen;
    int             argc;	/* these two may NOT be changed */
    char          **argv;
{

    ColormapPtr     pColormap;
    Bool            retval;
    int             i;

    struct sky_map  skyRegMap;
    make_gp         skyHdwrInfo;
    ulong           segAddr;
#ifdef _IBM_LFT
    extern get_rcm_fd();
    extern get_disp_name_by_number();
#endif


    TRACE(("skyIO(%d,0x%x,%d,0x%x)\n", scrnNum, pScreen, argc, argv));


    /*************************************************************************
    *                                                                        *
    *                        Allocate Private Areas                          *
    *                                                                        *
    *************************************************************************/

    if (ddpGeneration != serverGeneration)
    {
	ddpGCPrivateIndex  = AllocateGCPrivateIndex();
	ddpGeneration      = serverGeneration;
    }

    if (!AllocateGCPrivate(pScreen, ddpGCPrivateIndex, sizeof(skyPrivGCRec)))
	return FALSE;




#ifndef _IBM_LFT
    /*************************************************************************
    *                                                                        *
    *          Go into HFT monitor mode to gain access to skyway             *
    *                                                                        *
    *************************************************************************/
    if (ioctl(ibmScreenFD(scrnNum), HFHANDLE, &skyHandle[scrnNum]) < 0)
#else
    /*************************************************************************
    *                                                                        *
    *          Query LFT to get the gsc handle for skyway     		     *
    *                                                                        *
    *************************************************************************/
    /* The aixScreenFD is the the adapter number, used to get the appropriate
     * display name
     */
    strcpy(skyHandle[scrnNum].devname,  ibmScreenDevice(scrnNum));
    if ( ioctl(ibmScreenFD(scrnNum), GSC_HANDLE, &skyHandle[scrnNum]) < 0)
#endif
    {
	perror("ioctl on display");
	return FALSE;
    }

    skyHdwrInfo.pData  = (genericPtr) &skyRegMap;
    skyHdwrInfo.length = sizeof(skyRegMap);

#ifndef _IBM_LFT
    if (aixgsc(skyHandle[scrnNum], MAKE_GP, &skyHdwrInfo))
    {
	perror("cannot enter HFT monitor mode");
	return FALSE;
    }
#else
    if (aixgsc(skyHandle[scrnNum].handle, MAKE_GP, &skyHdwrInfo))
    {
	perror("aixgsc MAKE_GP call failed");
    }

#endif

/*---------------------------------------------------------------------------*/

    /*************************************************************************
    *                                                                        *
    *                     Initialize skyway hardware                         *
    *                                                                        *
    *************************************************************************/

    segAddr = (ulong) skyHdwrInfo.segment;

    SKY_SEGMENT[scrnNum]    = segAddr;
    SKY_VRAM_BASE[scrnNum]  = skyRegMap.vr_addr;
    SKY_VRAM_START[scrnNum] = segAddr | skyRegMap.vr_addr;
    IOREG[scrnNum]          = (skyIORegPtr) (segAddr | skyRegMap.io_addr);
    COPREG[scrnNum]         = (skyCopRegPtr) (segAddr | skyRegMap.cp_addr);

    /* Initialize installed TileStip list */

    SKY_TILESTIPID_CTR[scrnNum] = 0;

    SKY_TILESTIP[scrnNum] = xalloc(sizeof(ulong)*SKY_TILESTIP_AREAS);
    if (SKY_TILESTIP[scrnNum] == NULL)
	return FALSE;

    for (i=0; i<SKY_TILESTIP_AREAS; i++)
	SKY_TILESTIP[scrnNum][i] = 0;


    TRACE(("SEGADDR[%d]      = %x\n", scrnNum, segAddr));
    TRACE(("VRAM_SEGMENT[%d] = %x\n", scrnNum, SKY_SEGMENT[scrnNum]));
    TRACE(("VRAM_BASE[%d]    = %x\n", scrnNum, SKY_VRAM_BASE[scrnNum]));
    TRACE(("VRAM_START[%d]   = %x\n", scrnNum, SKY_VRAM_START[scrnNum]));
    TRACE(("IOREG[%d]        = %x\n", scrnNum, IOREG[scrnNum]));
    TRACE(("COPREG[%d]       = %x\n", scrnNum, COPREG[scrnNum]));

    skyHdwrInit(scrnNum);




    /*************************************************************************
    *                                                                        *
    *               Setup skyway dependent data and vectors                  *
    *                                                                        *
    *************************************************************************/

#ifdef SKY_MBUF_HACK
    if (!cfbSetupScreen(pScreen, SKY_VRAM_START[scrnNum],
			SKY_WIDTH, SKY_HEIGHT, 92, 92, SKY_WIDTH*2))
	return FALSE;
#else
    if (!cfbSetupScreen(pScreen, SKY_VRAM_START[scrnNum],
			SKY_WIDTH, SKY_HEIGHT, 92, 92, SKY_WIDTH))
	return FALSE;

    pScreen->CreateGC = skyCreateGC;
#endif

    pScreen->RealizeCursor = skyRealizeCursor;
    pScreen->UnrealizeCursor = skyUnrealizeCursor;
    pScreen->DisplayCursor = skyDisplayCursor;
    pScreen->RecolorCursor = miRecolorCursor;

#ifndef SKY_MBUF_HACK
    pScreen->PaintWindowBackground = miPaintWindow;
    pScreen->PaintWindowBorder = miPaintWindow;
    pScreen->CopyWindow = skyCopyWindow;
#endif

    pScreen->InstallColormap = skyInstallColormap;
    pScreen->UninstallColormap = skyUninstallColormap;
    pScreen->ListInstalledColormaps = skyListInstalledColormaps;
    pScreen->StoreColors = skyStoreColors;

    pScreen->QueryBestSize = skyQueryBestSize;

    printf("w,h=%d,%d\n", SKY_WIDTH, SKY_HEIGHT);
#ifdef SKY_MBUF_HACK
    if (!cfbFinishScreenInit(pScreen, SKY_VRAM_START[scrnNum],
	       SKY_WIDTH, SKY_HEIGHT, 92, 92, SKY_WIDTH*2))
	return FALSE;
#else
    if (!cfbFinishScreenInit(pScreen, SKY_VRAM_START[scrnNum],
	       SKY_WIDTH, SKY_HEIGHT, 92, 92, SKY_WIDTH))
	return FALSE;
#endif


    if (!cfbCreateDefColormap(pScreen))
	return FALSE;


    /* XXX - Should wrap or else BackingStore will leak memory */
    pScreen->CloseScreen = skyScreenClose;


    /*************************************************************************
    *                                                                        *
    *                Setup system-dependent data and vectors                 *
    *                                                                        *
    *************************************************************************/

    ibmScreen(scrnNum) = pScreen;
    pScreen->SaveScreen = ibmSaveScreen;


    /* Warning: miScreenInit sets BlockHandler and WakeHandler to NoopDDA,
     * so do not move this before cfbFinishScreenInit().
     */

    pScreen->BlockHandler = AIXBlockHandler;
    pScreen->WakeupHandler = AIXWakeupHandler;
    pScreen->blockData = (pointer) 0;
    pScreen->wakeupData = (pointer) 0;

    pScreen->SetCursorPosition = AIXSetCursorPosition;
    pScreen->CursorLimits = AIXCursorLimits;
    pScreen->PointerNonInterestBox = AIXPointerNonInterestBox;
    pScreen->ConstrainCursor = AIXConstrainCursor;

#ifdef SKY_MBUF_HACK
    {
	PixmapPtr pPixmap, selectPlane;
	DevUnion *frameBuffer;
	xMbufBufferInfo	*pInfo;
	int		nInfo;
	DepthPtr	pDepth;
	int		i,j,k;


	/* Create second buffer */


	pPixmap = (PixmapPtr ) xalloc(sizeof(PixmapRec));
	if (!pPixmap)
	    return FALSE;
	pPixmap->drawable.type = DRAWABLE_PIXMAP;
	pPixmap->drawable.depth = pScreen->rootDepth;
	pPixmap->drawable.pScreen = pScreen;
	pPixmap->drawable.serialNumber = 0;
	pPixmap->drawable.x = 0;
	pPixmap->drawable.y = 0;
	pPixmap->drawable.width = SKY_WIDTH;
	pPixmap->drawable.height = SKY_HEIGHT;
	pPixmap->refcnt = 1;
	pPixmap->devPrivate.ptr = (pointer)(SKY_VRAM_START[scrnNum]
	    + SKY_HEIGHT*SKY_WIDTH*2);
	pPixmap->devKind = PixmapBytePad(SKY_WIDTH*2,
					 pScreen->rootDepth);

	frameBuffer = (DevUnion *) xalloc(2*sizeof(DevUnion));
	if (!frameBuffer)
	    return FALSE;

	frameBuffer[0].ptr = pScreen->devPrivate;
	frameBuffer[1].ptr = (pointer) pPixmap;


	/* Create selectPlane */

	selectPlane = (PixmapPtr ) xalloc(sizeof(PixmapRec));
	if (!selectPlane)
	    return FALSE;
	selectPlane->drawable.type = DRAWABLE_PIXMAP;
	selectPlane->drawable.depth = pScreen->rootDepth;
	selectPlane->drawable.pScreen = pScreen;
	selectPlane->drawable.serialNumber = 0;
	selectPlane->drawable.x = 0;
	selectPlane->drawable.y = 0;
	selectPlane->drawable.width = SKY_WIDTH;
	selectPlane->drawable.height = SKY_HEIGHT;
	selectPlane->refcnt = 1;
	selectPlane->devPrivate.ptr = (pointer)
	    (SKY_VRAM_START[scrnNum] + SKY_WIDTH);
	selectPlane->devKind = PixmapBytePad(SKY_WIDTH*2,
					 pScreen->rootDepth);

	/* Mbuf supports every depth and visual combination
	 * that the screen does.
	 */
	
	nInfo = 0;
	for (i = 0; i < pScreen->numDepths; i++)
	{
	    pDepth = &pScreen->allowedDepths[i];
	    nInfo += pDepth->numVids;
	}

	pInfo = (xMbufBufferInfo *)
	    xalloc (pScreen->numVisuals * sizeof (xMbufBufferInfo));
	if (!pInfo)
	    return FALSE;

	k = 0;
	for (i = 0; i < pScreen->numDepths; i++)
	{
	    pDepth = &pScreen->allowedDepths[i];
	    for (j = 0; j < pDepth->numVids; j++)
	    {
		pInfo[k].visualID = pDepth->vids[j];
		pInfo[k].maxBuffers = 2;
		pInfo[k].depth = pDepth->depth;
		k++;
	    }
	}

	RegisterDoubleBufferHardware(pScreen, nInfo, pInfo,
				     frameBuffer, selectPlane, NULL, NULL);
    }
#endif
    return TRUE;
}



/*
 * skyScreenClose Screen is being destroyed. Release its resources. 
 */

Bool
skyScreenClose(scrnNum, pScreen)
    int             scrnNum;
    ScreenPtr       pScreen;
{
    unmake_gp       skyInfo;

    TRACE(("skyScreenClose(scrnNum=%d, pScreen=0x%x)\n", scrnNum, pScreen));

    if (pScreen->devPrivate)
	Xfree(pScreen->devPrivate);

#ifndef _IBM_LFT
    if (aixgsc(skyHandle[scrnNum], UNMAKE_GP, &skyInfo))
#else
    if (aixgsc(skyHandle[scrnNum].handle, UNMAKE_GP, &skyInfo))
#endif
    {
	TRACE(("aixgsc UNMAKE_GP failed\n"));
	return -1;
    }

    return TRUE;
}
