/*
 * $XConsortium: ibmInit.c /main/9 1995/12/05 15:44:26 matt $
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

#include "X.h"
#include "servermd.h"
#include "site.h"
#include "misc.h"
#include "miscstruct.h"
#include "input.h"
#include "opaque.h"
#include "scrnintstr.h"
#include "cursorstr.h"

#include "ibmScreen.h"

#include "ibmKeybd.h"
#include "ibmMouse.h"

#include "OSio.h"

#include "hftUtils.h"

#include "ibmTrace.h"

#define MOTION_BUFFER_SIZE 0
#define NUMDEVICES 2

static  int     ibmOpenAllScreens=      FALSE;
static  int     ibmScreensWanted=       0;
extern  char    *ibmBlackPixelText ;
extern  char    *ibmWhitePixelText ;
Bool    ibmDontZap = FALSE;
Bool    ibmDisableLock= FALSE;

static  Bool
ibmFormatExists(screenInfo,newFmt)
    ScreenInfo          *screenInfo;
    PixmapFormatPtr     newFmt;
{
    PixmapFormatPtr     oldFmt;
    int                 ndx;

    TRACE(("ibmFormatExisits(screenInfo= 0x%x,newFmt=0x%x(%d,%d,%d))\n",
	                             screenInfo,newFmt,newFmt->depth,
	                             newFmt->bitsPerPixel,newFmt->scanlinePad));
    for (ndx=0;ndx<screenInfo->numPixmapFormats;ndx++) {
	oldFmt= &screenInfo->formats[ndx];
	if ((newFmt->depth==oldFmt->depth)&&
	    (newFmt->bitsPerPixel==oldFmt->bitsPerPixel)&&
	    (newFmt->scanlinePad==oldFmt->scanlinePad)) {
	        return TRUE;
	}
    }
    return FALSE;
}

/***==================================================================***/
/*
	This function will probe for the type of adapter specified by
	"scr" (which is one of ibmPossibleScreens) and adapter number
	"dev_number".  If it exists it will be made active and added to
	the list of screens.  If x and y are negative, a location for
	wrapping will be found automatically.
*/
/* returns true if successful */
static  int
ibmProbeAndAdd(scr, dev_number, x, y)
	/* describes one adapter (as in "skyway") */
    ibmPerScreenInfo  *scr;
	/* tells wich of the type (as in "skyway1", "skyway2" */
    int  dev_number;
	/* if the x, y are -1 then use (ibmNumScreens,0) */
    int  x, y;
{
    unsigned  tmpDeviceID;
    int  tmpScreenFD;
#ifdef _IBM_LFT
    char *disp_name;
    extern char* get_disp_name_by_number();
#endif
    TRACE(("ibmProbeAndAdd(scr, dev_number=%d)\n", dev_number));

    if( ibmNumScreens >= MAXSCREENS ){
	TRACE(("MAXSCREENS is %d and there are already %d screens open\n", MAXSCREENS, ibmNumScreens));
	return FALSE;
    }

    if( x < 0 )
	x = ibmNumScreens;
    if( y < 0 )
	y = 0;

    if ( ( tmpScreenFD = (* scr->ibm_ProbeFunc)(scr->ibm_DeviceID,dev_number) ) >= 0 ) {

#ifndef _IBM_LFT
	/*------ Only for hft enabled system ------*/
	tmpDeviceID = hftQueryDeviceID(tmpScreenFD);
	if( (tmpDeviceID & HFT_DEVID_MASK ) != scr->ibm_DeviceID ){
	    TRACE(("ibmProbeAndAdd()  we didn't get what we asked for\n"));
	    TRACE(("wanted=0x%x  got=0x%x\n", scr->ibm_DeviceID, tmpDeviceID));
	    close(tmpScreenFD);
	    return FALSE ;
	}
#endif 

	aixPutScreenAt(ibmNumScreens,x,y);
	ibmScreens[ibmNumScreens] = (ibmPerScreenInfo *) xalloc( sizeof(ibmPerScreenInfo) );
	*(ibmScreens[ibmNumScreens]) = *scr;

#ifndef _IBM_LFT
	ibmScreens[ibmNumScreens]->ibm_DeviceID = tmpDeviceID;
#else
	ibmScreens[ibmNumScreens]->ibm_DeviceID = scr->ibm_DeviceID;
	/* get the device name */
	disp_name = get_disp_name_by_number(dev_number);
	ibmScreens[ibmNumScreens]->ibm_ScreenDevice = (char*)Xalloc(strlen(disp_name)+1);
	strcpy(ibmScreens[ibmNumScreens]->ibm_ScreenDevice, disp_name);
#endif
	ibmScreens[ibmNumScreens]->ibm_ScreenFD = tmpScreenFD;
	ibmNumScreens++;
	return TRUE ;
    }
    return FALSE ;
}


/*
	This function will probe for the specific adapter specified by
	"dev_id".  If it exists it will be made active and added to the
	list of screens.  If x and y are negative, a location for
	wrapping will be found automatically.
*/
/* returns true if successful */
int
ibmFindProbeAndAdd(dev_id, anumber, x, y)
    unsigned  dev_id;
    int anumber;
	/* if the x, y are -1 then use (ibmNumScreens,0) */
    int  x, y;
{
    ibmPerScreenInfo **scrPtr = ibmPossibleScreens ;
    ibmPerScreenInfo *scr ;

    TRACE(("ibmFindProbeAndAdd(dev_id, x, y)\n"));
#ifndef _IBM_LFT
    while ( ( scr = *scrPtr++ ) && scr->ibm_ScreenFlag ) {
	if( scr->ibm_DeviceID == (dev_id & HFT_DEVID_MASK) ){
	    return( ibmProbeAndAdd(scr, (dev_id & HFT_DEVNUM_MASK), x, y) );
	}
    }
#else
    while ( ( scr = *scrPtr++ ) && scr->ibm_ScreenFlag ) {
	if( scr->ibm_DeviceID == dev_id ){
	    return( ibmProbeAndAdd(scr, anumber, x, y) );
	}
    }
#endif
    return FALSE;
}


/*
	This function will probe for the specific adapter specified
	by "number".  "number" is the value you get from running the
	"lsdisp" command.  If it exists it will be made active and
	added to the list of screens.  If x and y are negative, a
	location for wrapping will be found automatically.
*/
/* returns true if successful */
static  int
ibmNumberProbeAndAdd(number, x, y)
    unsigned  number;
	/* if the x, y are -1 then use (ibmNumScreens,0) */
    int  x, y;
{
#ifndef _IBM_LFT
    hftDeviceID  *pDevices;
#endif
    int  nDevices;
    unsigned  dev_id;

    TRACE(("ibmNumberProbeAndAdd(number, x, y)\n"));

	/* ouside we use 1 to n */
	/* inside we use 0 to n-1 */
    number--;
    if( number < 0 )
	return(FALSE);

#ifndef _IBM_LFT
	/* if can't find any devices */
    if( (nDevices = hftQueryDeviceIDs(&pDevices)) <= 0 )
	return(FALSE);

	/* if bad device number */
    if( number >= nDevices )
	return(FALSE);

    dev_id = pDevices[number].hftDevID;
    return( ibmFindProbeAndAdd(dev_id, 0, x, y) );
#else
     if((nDevices = get_num_lft_displays() <=0) || 
	 (number >= nDevices))  
	return FALSE;
     /* this is a pseudo id see AIX/hftUtils.c */
     dev_id = get_lft_disp_id(number);
    TRACE(("device number %d is 0x%x\n", number, dev_id));
    return( ibmFindProbeAndAdd(dev_id, number, x, y) );
#endif

}

/***==================================================================***/

static  int
ibmFindSomeScreens()
{
register ibmPerScreenInfo **scrPtr = ibmPossibleScreens ;
register ibmPerScreenInfo *scr ;

    TRACE(("ibmFindSomeScreens()\n"));
    osGetDefaultScreens();
    if ( !ibmNumScreens ) {
	while ( ( scr = *scrPtr++ ) && scr->ibm_ScreenFlag ) {
	        /* look for the first adapter of this */
	        /* type, don't care what wrap location */
	    if( ibmProbeAndAdd(scr, 0, -1,-1) ){
	        if (!ibmOpenAllScreens)
	            return TRUE ;
	    }
	}
    }
    else
	return TRUE ;

    if ((ibmOpenAllScreens)&&(ibmNumScreens>0))
	return TRUE ;
    return FALSE ;
}

/***==================================================================***/

static  void
ibmAddScreens(screenInfo, argc, argv)
    ScreenInfo          *screenInfo;
    int                  argc;
    char                *argv[];
{
    PixmapFormatPtr     newFmt,oldFmt;
    int                  ndx,fmtNdx;
    static int           been_here;

    TRACE(("ibmAddScreens(screenInfo= 0x%x, argc= %d, argv]=x%x)\n",
	                                                screenInfo,argc,argv));

    for (ndx=0;ndx<ibmNumScreens;ndx++) {
	if (!been_here) {
	    if (ibmScreens[ndx]->ibm_Wanted) {
	        ErrorF("Multiple requests for screen '%s'  -- ignored\n",
	                                                ibmScreenFlag(ndx));
	        continue;
	    }
	    ibmScreens[ndx]->ibm_Wanted= TRUE;
	}

	for (fmtNdx=0;fmtNdx<ibmNumFormats(ndx);fmtNdx++) {
	    if (!ibmFormatExists(screenInfo,&ibmScreenFormats(ndx)[fmtNdx])) {
	        newFmt= &ibmScreenFormats(ndx)[fmtNdx];
	        oldFmt= &screenInfo->formats[screenInfo->numPixmapFormats++];
	        oldFmt->depth=          newFmt->depth;
	        oldFmt->bitsPerPixel=   newFmt->bitsPerPixel;
	        oldFmt->scanlinePad=    newFmt->scanlinePad;
	        if (screenInfo->numPixmapFormats>MAXFORMATS) {
	            ErrorF("Too many formats! Exiting\n");
	            exit(1);
	        }
	    }
	}
	AddScreen(ibmScreenInit(ndx),argc,argv);
	ibmSetScreenState(ndx,SCREEN_ACTIVE);
    }
    been_here= TRUE;
}

/***==================================================================***/

static DevicePtr keyboard;
static DevicePtr mouse;

void
InitInput(argc, argv)
    int  argc;
    char ** argv;
{
extern  int OS_MouseProc(),OS_KeybdProc();

    TRACE(("InitInput()\n"));

    OS_InitInput();
    mouse=      AddInputDevice(OS_MouseProc,    TRUE);
    keyboard=   AddInputDevice(OS_KeybdProc,    TRUE);

    RegisterPointerDevice( mouse );
    RegisterKeyboardDevice( keyboard );
#ifdef SOFTWARE_CURSOR
    miRegisterPointerDevice(ibmScreens[ibmCurrentScreen]->ibm_Screen, mouse);
#endif
#ifdef AIXEXTENSIONS
    AddandRegisterAIXInputStuff();
#endif
    return ;
}

/***==================================================================***/

ibmPerScreenInfo        *(*ibmAdditionalScreenArg)();

int
ddxProcessArgument(argc,argv,i)
int     argc;
char    *argv[];
int     i;
{
int                     skip= 1;

extern  char *ibmArenaFile;
extern  int ibmQuietFlag ;

    TRACE(("ddxProcessArgument( argc= %d, argv= 0x%x, i=%d )\n",argc,argv,i));

#ifdef OS_ProcessArgument
    if (skip=OS_ProcessArgument(argc,argv,i))           return(skip);
    else                                                skip= 1;
#endif
    if ( strcmp( argv[i], "-pckeys" ) == 0 )       ibmUsePCKeys= 1;
    else if ( strcmp( argv[i], "-quiet" ) == 0 )        ibmQuietFlag = 1;
    else if ( strcmp( argv[i], "-rtkeys" ) == 0 )       ibmUsePCKeys= 0;
    else if ( strcmp( argv[i], "-verbose" ) == 0 )      ibmQuietFlag = 0;
    else if ( strcmp( argv[i], "-wrapx"  ) == 0 )       ibmXWrapScreen= TRUE;
    else if ( strcmp( argv[i], "-wrapy"  ) == 0 )       ibmYWrapScreen= TRUE;
    else if ( strcmp( argv[i], "-wrap"  ) == 0 )
	                                ibmXWrapScreen= ibmYWrapScreen= TRUE;
#ifdef TRACE_X
    else if ( strcmp( argv[i], "-trace"  ) == 0 )       ibmTrace= TRUE;
    else if ( strcmp( argv[i], "-tracesync"  ) == 0 )   ibmTraceSync= TRUE;
#endif
    else if ( strcmp( argv[i], "-nohdwr" ) == 0 )       ibmUseHardware= FALSE;
#ifdef IBM_SPECIAL_MALLOC
    else if ( strcmp( argv[i], "-malloc" ) == 0 )       {
	int lvl= atoi(argv[++i]);
	SetMallocCheckLevel(lvl);
	ErrorF("allocator check level set to %d...\n",lvl);
	skip= 2;
    }
    else if ( strcmp( argv[i], "-plumber" ) == 0 ) {
	ibmSetupPlumber(argv[++i]);
	skip= 2;
    }
#endif /* IBM_SPECIAL_MALLOC */
    else if ( strcmp( argv[i], "-T") == 0)
	ibmDontZap = TRUE;
    else if ( strcmp( argv[i], "-wp") == 0)
	{
	ibmWhitePixelText = argv[++i];
	skip= 2;
	}
    else if ( strcmp( argv[i], "-bp") == 0)
	{
	ibmBlackPixelText = argv[++i];
	skip= 2;
	}
    else if ( strcmp( argv[i], "-all" ) == 0 )
	ibmOpenAllScreens= TRUE;
    else if ( strncmp( argv[i], "-P", 2) == 0)
	{
	    char  *sarg;
	    int  x, y;

	    skip = 0;
	    y = argv[i][2] - '0';       /* row */
	    x = argv[i][3] - '0';       /* column */
	    sarg = argv[++i];
	    if( sarg && (x>=0) && x<=9 && y>=0 && y<=9 ){
	        register ibmPerScreenInfo **ppScr = ibmPossibleScreens;
	        register ibmPerScreenInfo  *pScr;
	        int found= FALSE;
	        int  argl, flagl, anumber;

	        anumber = 0;
	        argl = strlen(sarg);
	        while ( ( pScr = *ppScr++ ) && pScr->ibm_ScreenFlag && !found ) {
	                /* offset past the '-' in ibm_ScreenFlag */
	            flagl = strlen(pScr->ibm_ScreenFlag)-1;
	            if (!strncmp(sarg,&(pScr->ibm_ScreenFlag[1]),flagl)) {
	                if( flagl < argl ){
	                    anumber = atoi( &(sarg[flagl]) );

	                    /* outside the adapters are numbered starting */
	                    /* from 1, inside they are numbered starting */
	                    /* from 0 */
	                    anumber--;
	                    /* outside the adapters are numbered starting */
	                    /* from 1, inside they are numbered starting */
	                    /* from 0 */

	                    if( anumber < 0 )
	                        anumber = 0;
	                }
	                else{
	                    anumber = 0;
	                }
	                skip= 2;
	                ibmScreensWanted++;
	                if( ibmProbeAndAdd(pScr, anumber, x, y) ){
	                    ;
	                }
	                else  {
	                    ErrorF("%s not available\n",sarg);
	                }
	                found= TRUE;
	            }
	        }

	        if ((!found)&&(ibmAdditionalScreenArg)) {
	            pScr= (*ibmAdditionalScreenArg)(sarg);
	            if (pScr) {

	                /* have to find some way to set this for */
	                /* additional screens */
	                anumber = 0;
	                /* have to find some way to set this for */
	                /* additional screens */

	                skip= 2;
	                ibmScreensWanted++;
	                if( ibmProbeAndAdd(pScr, anumber, x, y) ){
	                    found= TRUE;
	                }
	                else {
	                    ErrorF("%s not available\n",sarg);
	                }
	            }
	        }

	        if (!found) {
	            int  dnumber;

	            skip= 2;
	            ibmScreensWanted++;
	            dnumber = atoi( sarg );
	            if( ibmNumberProbeAndAdd(dnumber, x, y) ){
	                found = TRUE;
	            }
	            else{
	                ErrorF("%s not available\n",sarg);
	            }
	        }
	    }
	}
    else {
	register ibmPerScreenInfo **ppScr = ibmPossibleScreens;
	register ibmPerScreenInfo  *pScr;
	int found= FALSE;
	/* argument length, ibm_ScreenFlag length, */
	/* adapter number */
	int  argl, flagl, anumber;

	anumber = 0;
	argl = strlen(argv[i]);
	skip= 0;
	while ( ( pScr = *ppScr++ ) && pScr->ibm_ScreenFlag && !found ) {
	    flagl = strlen(pScr->ibm_ScreenFlag);
	    if (!strncmp(argv[i],pScr->ibm_ScreenFlag,flagl)) {
	        if( flagl < argl ){
	            anumber = atoi( &(argv[i][flagl]) );

	            /* outside the adapters are numbered starting */
	            /* from 1, inside they are numbered starting */
	            /* from 0 */
	            anumber--;
	            /* outside the adapters are numbered starting */
	            /* from 1, inside they are numbered starting */
	            /* from 0 */

	            if( anumber < 0 )
	                anumber = 0;
	        }
	        else{
	            anumber = 0;
	        }
	        skip= 1;
	        ibmScreensWanted++;
	        if( ibmProbeAndAdd(pScr, anumber, -1, -1) ){
	            ;
	        }
	        else  {
	            ErrorF("%s not available\n",&argv[i][1]);
	        }
	        found= TRUE;
	    }
	}

	if ((!found)&&(ibmAdditionalScreenArg)) {
	    pScr= (*ibmAdditionalScreenArg)(argv[i]);
	    if (pScr) {

	        /* have to find some way to set this for */
	        /* additional screens */
	        anumber = 0;
	        /* have to find some way to set this for */
	        /* additional screens */

	        skip= 1;
	        ibmScreensWanted++;
	        if( ibmProbeAndAdd(pScr, anumber, -1, -1) ){
	            found= TRUE;
	        }
	        else {
	            ErrorF("%s not available\n",&argv[i][1]);
	        }
	    }
	}

#if defined(DYNAMIC_LINK_SCREENS)
	/* No More pre-linked screens! Try dynamic linking. */
	if (!found) {
	    if ( pScr = ibmDynamicScreenAttach( argv[i] ) ) {
	        ibmScreens[ ibmNumScreens++ ] = pScr;
	        skip= 1;
	    }
	}
#endif

    }
    return(skip);
}

/***==================================================================***/

extern void ibmPrintBuildDate() ;
extern void ibmInfoMsg() ;

void
InitOutput(screenInfo, argc, argv)
    ScreenInfo  *screenInfo;
    int          argc;
    char        *argv[];
{
    static      int     been_here= 0;

    TRACE(("InitOutput( screenInfo= 0x%x)\n",screenInfo));

    screenInfo->imageByteOrder = IMAGE_BYTE_ORDER;
    screenInfo->bitmapScanlineUnit = BITMAP_SCANLINE_UNIT;
    screenInfo->bitmapScanlinePad = BITMAP_SCANLINE_PAD;
    screenInfo->bitmapBitOrder = BITMAP_BIT_ORDER;

    screenInfo->numPixmapFormats = 1;
    screenInfo->formats[0].depth= 1;
    screenInfo->formats[0].bitsPerPixel= 1;
    screenInfo->formats[0].scanlinePad= BITMAP_SCANLINE_PAD;

    if (!been_here) {
	been_here= TRUE;

	if (ibmNumScreens!=ibmScreensWanted) {
	    TRACE(("ibmNumScreens=%d, ibmScreensWanted=%d\n", ibmNumScreens, ibmScreensWanted));
	    ErrorF("Couldn't open all requested screens.");
	    exit(1);
	}
	else if ((ibmNumScreens==0)&&(!ibmFindSomeScreens())) {
	    ErrorF("Couldn't open any screens.");
	    exit(1);
	}

	/* Informational Messages */
	ibmPrintBuildDate();
	ibmInfoMsg(
  "X Window System protocol version %d, revision %d (vendor release %d)\n",
	        X_PROTOCOL, X_PROTOCOL_REVISION, VENDOR_RELEASE ) ;

	ibmMachineDependentInit(); /* usually opens /dev/bus */
	if (ibmUsePCKeys)       ibmInfoMsg( "Using PC keyboard layout...\n" );
	else                    ibmInfoMsg( "Using RT keyboard layout...\n" );
    }
    ibmAddScreens(screenInfo,argc,argv);
#ifdef AIXEXTENSIONS
    AddandRegisterAIXOutputStuff(screenInfo);
#endif
    return;
}
