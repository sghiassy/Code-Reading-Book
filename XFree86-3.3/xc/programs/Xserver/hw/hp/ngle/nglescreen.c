/* $XConsortium: nglescreen.c,v 1.3 95/01/24 02:15:14 dpw Exp $ */
/*************************************************************************
 * 
 * (c)Copyright 1992 Hewlett-Packard Co.,  All Rights Reserved.
 * 
Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name of Hewlett Packard not be used in
advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.

 *
 *************************************************************************/

/******************************************************************************
 *
 *	This file contains various global variables and routines concerning
 *	the Screen structure.  This includes the ngleScreenInit routine.
 *
 ******************************************************************************/


#include "ngle.h"

#define DEFAULT_CRX_MONITOR_DIAGONAL 19	/* inches */
#define LOWRES_CRX_MONITOR_DIAGONAL  16 /* inches */

#define HYPERBOWL_MODE_FOR_8_OVER_88_LUT0_NO_TRANSPARENCIES	4
#define HYPERBOWL_MODE01_8_24_LUT0_TRANSPARENT_LUT1_OPAQUE	8
#define HYPERBOWL_MODE01_8_24_LUT0_OPAQUE_LUT1_OPAQUE		10
#define HYPERBOWL_MODE2_8_24					15

/* Can this graphics device be an ITE console?
 * Most all NGLE graphics devices can be.
 * The sole exception at this point (May'91) is the right-head of 
 * the Dual CRX card; only the left head can be an ITE console.
 * 
 * The answer to this question is used to determine 
 * whether to issue ioctl(GCON) at startup and ioctl(GCTERM)
 * at server exit (only should be done if ITE may be present).
 *
 * Answer is encoded in bit 2 of device file's minor number.
 *
 */
#define IS_NOT_FIRST_HEAD_ON_THIS_SGC_SLOT(devMinorNumber) \
    (devMinorNumber & 0x00000004)



/******************************************************************************
 *
 * Extern and Forward Declarations:
 *
 ******************************************************************************/

extern double 		sqrt(double);
extern int 		monitorResolution;
extern int 		cfbScreenPrivateIndex;
extern int 		cfb32ScreenPrivateIndex;

#ifdef HP_FAST_SCROLLING
/* The following are the extern declarations for the GCOps tables that
 * CFB sets up and uses.  If CFB changes the GCOps tables it uses, or
 * adds any, this driver will also have to mirror those changes.
 */
extern GCOps cfbTEOps1Rect;
extern GCOps cfbNonTEOps1Rect;
extern GCOps cfbTEOps;
extern GCOps cfbNonTEOps;

extern GCOps cfb32TEOps1Rect;
extern GCOps cfb32NonTEOps1Rect;
extern GCOps cfb32TEOps;
extern GCOps cfb32NonTEOps;
#endif


extern Bool 		(*ddxScreenInitPointer[])();
extern hpPrivPtr	hpPrivates[MAXSCREENS];
extern Bool 		hyperResetPlanes();

Int32			ngleScreenPrivIndex = 0;

/* FORWARD (procedures defined below in this file) */

static void hyperSetupPlanes(
    NgleScreenPrivPtr	    pScreenPriv);

static void rattlerSetupPlanes(
    NgleScreenPrivPtr	    pScreenPriv);

static void ngleSetupAttrPlanes(
    NgleScreenPrivPtr	    pScreenPriv,
    Card32		    BufferNumber);

static void elkSetupPlanes(
    NgleScreenPrivPtr       pScreenPriv);

static Bool ngleClearScreen(
    ScreenPtr	pScreen);

static Bool ngleSaveScreen(
    ScreenPtr	pScreen,
    Bool	on);

static void ngleBlankOrUnblankScreen(
    NgleScreenPrivPtr	pScreenPriv, 
    Bool		doBlank);

static Bool ngleCloseScreen();

static void nglePolyPaintAttr(
    NgleScreenPrivPtr           pScreenPriv,
    Card32			ctlPlaneReg,
    Int32			nBoxes,
    BoxPtr			pBoxes);

/* OWN */

static Card32 	ngleScreenPrivGeneration = 0;
static Card32	cfbDualHeadBug = 0;
static Int32	cfbFirstIndex = 0;



/******************************************************************************
 *
 * NGLE DDX Utility Procedure:          duplicateDeviceFileInScreensFile()
 *
 * Description: check if device file number of this screen has already
 *	been opened by this server.
 *
 *  The least significant bit is masked before checking for duplicates
 *  because:
 *  -	This bit is used to distingish overlay from image plane sets.
 *	(Odd == overlay, by the way).
 *
 ******************************************************************************/

static Bool duplicateDeviceFileInScreensFile(
    int		myIndex,
    int		myMajorAndMinorDeviceNumber,
    int		*duplicateLine)
{
    int		i;
    hpPrivPtr	php;
    struct stat statInfo;

    if (myIndex == 0)
    {
        return(FALSE);
    }
    else
    {
        for (i = 0; i < myIndex; i++)
        {

            if (ddxScreenInitPointer[i] && (i != myIndex))  /* DDX screen */
            {
                php = hpPrivates[i];
                if(stat(php->StandardDevice,&statInfo) < 0) continue;

                if ((statInfo.st_rdev & 0xfffffffe)
		    == (myMajorAndMinorDeviceNumber & 0xfffffffe))
                {
		    *duplicateLine = (hpPrivates[myIndex])->LineNumber;
                    return (TRUE);
                }
            }
        }
        return(FALSE);
    }
}   /* duplicateDeviceFileInScreensFile() */




/******************************************************************************
 *
 * NGLE DDX Utility Procedure:		calculateDPI
 *
 * Description: Returns the value to use for dpi for this monitor.
 *
 ******************************************************************************
 */

static int calculateDPI(
    int xsize, int ysize,
    int diagonal_length)
{
    static int old_diagonal_length = 0;
    int dpi = 0;
    double f_diagonal_length = 1.0;
    double f_num_diagonal_pix;
   
    f_num_diagonal_pix = sqrt((double)((xsize * xsize + ysize * ysize)));
   
    if (diagonal_length < 0)
    {
	FatalError("Incorrect monitor size specified in your /user/lib/X11/Xnscreens file.\n");
    }
    if (diagonal_length == 0)
    /* either first time (use default value) OR subsequent call
       (use old value)  */
    {
	if (!(old_diagonal_length))
	/* first time */
	{
	    diagonal_length = DEFAULT_CRX_MONITOR_DIAGONAL;
	}
	else
	/* second or subsequent use */
	{
	    diagonal_length = old_diagonal_length;
	}
    }
    if(monitorResolution != 0)
	dpi = monitorResolution;
    else 
	if ((diagonal_length > 0) && (diagonal_length <= 100))
        /* assume it's inches -- subtract 1 inch */
        {
            dpi = (int)(f_num_diagonal_pix / (((double)diagonal_length) - 1.0));
            /* along the diagonal */
        }
        else if ((diagonal_length > 100) && (diagonal_length <= 1000))
        {
	    /* assume it's millimeters  -- subtract 25.4 mm */
    
	    f_diagonal_length = (double)(diagonal_length - 25.4);
    
            dpi = (int)((f_num_diagonal_pix / f_diagonal_length) * 25.4);
    
        }
        else
        {
	    FatalError("Incorrect monitor size specified in your /user/lib/X11/Xnscreens file.\n");	
        }
    
    old_diagonal_length = diagonal_length;
    return(dpi);
}  /* calculateDPI */



/******************************************************************************
 *
 * NGLE DDX Utility Procedure:          ngleGetDeviceRomData()
 *
 * Description: Return pointer to in-memory structure holding
 *		ELK device-dependent ROM values.
 *
 ******************************************************************************/

static NgleDevRomDataPtr ngleGetDeviceRomData(
    NgleScreenPrivPtr	pScreenPriv)
{
    crt_frame_buffer_t	gcDescribe;
    Card32		*pBytePerLongDevDepData;/* data byte == LSB */
    Card32		*pRomTable;
    NgleDevRomDataPtr	pPackedDevRomData;
    Int32		sizePackedDevRomData 	= sizeof(NgleDevRomDataRec);
    Card8		*pCard8;
    Int32		i;
    Card8		*mapOrigin		= (Card8 *) NULL;
    Int32		fildes			= pScreenPriv->fildes;
    NgleHdwPtr		pDregs			= pScreenPriv->pDregs;
    Int32		romTableIdx;


    /* Allocate space for packed structure */
    pPackedDevRomData = (NgleDevRomDataPtr) Xcalloc(sizePackedDevRomData);

    /* Get information about the device from the kernel: */
    if (ioctl(fildes, GCDESCRIBE, &gcDescribe) == -1)
    {
	/* 3rd parm is dontcare on s700, ptr to 0 on s400 */
        ioctl(fildes, GCUNMAP, &mapOrigin);
        close(fildes);
        FatalError("File %s: unable to get kernel description of display.\n",
	    __FILE__);
    }

    SETUP_HW(pDregs);

    if (pScreenPriv->deviceID == S9000_ID_ARTIST)
    {
	pPackedDevRomData->cursor_pipeline_delay = 4;
	pPackedDevRomData->video_interleaves     = 4;
    }
    else
    {
	/* Get pointer to unpacked byte/long data in ROM */
	pBytePerLongDevDepData = (Card32 *)
				gcDescribe.crt_region[NGLEDEVDEPROM_CRT_REGION];

	/* Tomcat supports several resolutions: 1280x1024, 1024x768, 640x480.
	 * This code reads the index into a device dependent ROM array
	 * containing the device's currently selected resolution.
	 */
	if (pScreenPriv->deviceID == S9000_ID_TOMCAT)
	{
	    /*  jump to the correct ROM table  */
	    GET_ROMTABLE_INDEX(romTableIdx);
	    while  (romTableIdx > 0)
	    {
		pCard8 = (Card8 *) pPackedDevRomData;
		pRomTable = pBytePerLongDevDepData;
		/* Pack every fourth byte from ROM into structure */
		for (i = 0; i < sizePackedDevRomData; i++)
		{
		    *pCard8++ = (Card8) (*pRomTable++);
		}

		pBytePerLongDevDepData = (Card32 *)
			((Card8 *) pBytePerLongDevDepData +
			       pPackedDevRomData->sizeof_ngle_data);

		romTableIdx--;
	    }
	}

	pCard8 = (Card8 *) pPackedDevRomData;

	/* Pack every fourth byte from ROM into structure */
	for (i = 0; i < sizePackedDevRomData; i++)
	{
	    *pCard8++ = (Card8) (*pBytePerLongDevDepData++);
	}
    }

    SETUP_FB(pDregs,pScreenPriv->deviceID,pScreenPriv->devDepth);

    return(pPackedDevRomData);
}   /* ngleGetDeviceRomData() */



/******************************************************************************
 *
 * NGLE DDX Utility Procedure:          stringContainsString
 *
 * Description: Does string in parm1 contain string in parm2?
 *
 * Limitations:
 *  Case-sensitive
 *
 ******************************************************************************/

static Bool stringContainsString(
    char    *pString,
    char    *pSubString)
{
    char    *pCurPos = pString;
    int     len = strlen(pSubString);

    while (pCurPos = (char *) strchr(pCurPos, pSubString[0]))
    {
        if (bcmp(pCurPos, pSubString, len) == 0)
            return(TRUE);
        else
            pCurPos++;
    }
    return(FALSE);
}   /* stringContainsString */



/******************************************************************************
 *
 * NGLE DDX Utility Procedure:          firstScreenOpenedOnThisSgcSlot()
 *
 * Description: 
 *
 *	Given the possibility of 2 heads on a Dual CRX being controlled
 *	by a single X server, is this screen the first screen on this
 *	SGC slot to be opened?
 *
 * Assumption:
 *	Procedure is actually used to determined whether this screen
 *	will be the LAST to CLOSE.  This is valid assumption because
 *	Server closes devices in reverse order in which they were
 *	opened (first device opened is last device closed).
 *
 *	So to restate: X server first opens device at index 0 and increments
 *	index. It closes highest numbered indexed device first and decrements,
 *	closing device at index 0 last.
 *
 * Limitations:
 *  	Does NOT check for identical minor numbers (i.e, same device is
 *	listed more than once in X*screens file).
 *
 ******************************************************************************/

static Bool firstScreenOpenedOnThisSgcSlot(
    Int32       index,          /* IN Current screen index */
    dev_t       devMinorNumber) /* IN of current device */
{
    int		i;
    hpPrivPtr  php;
    struct stat statInfo;

    if (index == 0)
    {
        return(TRUE);
    }
    else
    {
        for (i = 0; i < index; i++)
        {

            if (ddxScreenInitPointer[i] && (i != index))  /* DDX screen */
            {
                php = hpPrivates[i];
                if(stat(php->StandardDevice,&statInfo) < 0) continue;

		/* Dual CRX convention: use bit 2 to indicate which head.
		 * And don't bother checking bit 0 and bit 1, which
		 * should always be zero for Dual CRX.
		 */
                if ((statInfo.st_rdev&0xfffffff8)==(devMinorNumber&0xfffffff8))
                {
                    return (FALSE);
                }
            }
        }
        return(TRUE);
    }
}   /* firstScreenOpenedOnThisSgcSlot */



/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleScreenInit()
 *
 * Description:
 *
 *	This routine implements pScreen->ScreenInit for the NGLE DDX Driver.
 *
 ******************************************************************************/

Bool ngleScreenInit(
    Int32			index,
    ScreenPtr			pScreen,
    Int32			argc,
    char			**argv)
{

    NgleScreenPrivPtr		pScreenPriv;
    NgleHdwPtr			pDregs;
    struct stat 		statInfo;
    hpPrivPtr                   php;
    int				duplicateLine;
    int				fildes;
    unsigned char 		*mapOrigin;
    Card8			*fbaddr;
    crt_frame_buffer_t		gcDescribe;
    char			*grayscaleEnvar = (char *) 0;
    char			*moveCursorEnvar = (char *) 0;
    int				monitorDiagonal = DEFAULT_CRX_MONITOR_DIAGONAL;
    int                         dpi;
    NgleDevRomDataPtr           pDevRomData;
    VisualPtr			pVisuals;
    int				i;


    /*
     **************************************************************************
     **
     ** 	Allocate and Start Initializing the pScreenPriv Structure:
     **
     **************************************************************************
     */

    /* Allocate the private index for screen. */
    if (ngleScreenPrivGeneration != serverGeneration)
    {
	if ((ngleScreenPrivIndex = AllocateScreenPrivateIndex()) < 0)
	    return(FALSE);
	ngleScreenPrivGeneration = serverGeneration;
    }

    /* Allocate the screen private structure (pScreenPriv), zero it (just
     * in case), point pScreen to it, and point it towards pScreen):
     */
    if (!(pScreenPriv = (NgleScreenPrivPtr) Xalloc(sizeof(NgleScreenPrivRec))))
	return(FALSE);
    bzero((char *) pScreenPriv, sizeof(NgleScreenPrivRec));
    pScreen->devPrivates[ngleScreenPrivIndex].ptr = (pointer) pScreenPriv;

    /* Initialize the HP Private Structure: */
    php = (hpPrivPtr) hpPrivates[pScreen->myNum];
    pScreen->devPrivate = (pointer) php;

    pScreenPriv->pScreen = pScreen;
    pScreenPriv->myNum = index;


    /*
     **************************************************************************
     **
     ** 	Open, Map, Init and Get Information about the Device:
     **		
     **		Obtain values for these pScreenPriv fields via ioctl() calls:
     **		-   fildes	(from open of device file)
     **		-   deviceID	(from ioctl(GCDESCRIBE.crt_id)
     **		-   pDregs	(from ioctl(GCDESCRIBE.crt_control_base)
     **		-   fbaddr	(from ioctl(GCDESCRIBE.crt_frame_base)
     **
     **************************************************************************
     */

    /* Stat the file descriptor to see if the device exists: */
    if(stat(php->StandardDevice, &statInfo) < 0)
    {
	char	errorString[80];

	FatalError("Unable to stat() device file \"%s\"!\n", php->StandardDevice);
    }

    /* Check for duplicate entries for the same device file */
    if (duplicateDeviceFileInScreensFile(index, statInfo.st_rdev,
	    &duplicateLine))
    {
	ErrorLine(php);
	ErrorF("Duplicate device entry in line %d.\n", duplicateLine);
	FatalError("Cannot continue...\n");
    }

    /* Open the device file: */
    if((fildes = open(php->StandardDevice, O_RDWR)) == -1)
    {
	FatalError("Unable to open() device file \"%s\"!\n", php->StandardDevice);
    }
    pScreenPriv->fildes = fildes;

    mapOrigin = (Card8 *) NULL;

    /* Map the framebuffer.
     *
     * Note that a given process can call GCMAP for the same SGC slot
     * more than once without error (can occur with server mapping 
     * both heads of a DualCRX).
     */
    if (ioctl(fildes, GCMAP, &mapOrigin) == -1)
    {
	close(fildes);
	FatalError("Unable to map graphics display into user space!\n");
    }

    /* Get information about the device from the kernel: */
    if (ioctl(fildes, GCDESCRIBE, &gcDescribe) == -1)
    {
	ioctl(fildes, GCUNMAP, &mapOrigin);
	close(fildes);
	FatalError("Unable to get kernel description of display!\n");
    }

    /* Reject any device not in the NGLE family */
    if 	(   (gcDescribe.crt_id != S9000_ID_A1659A)	/* CRX */
	&&  (gcDescribe.crt_id != S9000_ID_ELM)		/* GRX */
	&&  (gcDescribe.crt_id != S9000_ID_TIMBER)	/* 710 Any */
	&&  (gcDescribe.crt_id != S9000_ID_TOMCAT)	/* Dual CRX */
	&&  (gcDescribe.crt_id != S9000_ID_A1439A)	/* CRX24 */
	&&  (gcDescribe.crt_id != S9000_ID_ARTIST)	/* Artist */
	&&  (gcDescribe.crt_id != S9000_ID_HCRX)	/* Hyperdrive */
	)
    {
	ioctl(fildes, GCUNMAP, &mapOrigin);
	close(fildes);
	FatalError(
	    "Graphics device has a unrecognized graphics crt_id of 0x%08x!\n",
	    gcDescribe.crt_id);
    }

    /* Get some values from the gcDescribe structure:
     * gcDescribe.crt_id:
     *	    on s700, will equal 4 most significant bytes of graphics ID
     *		stored in Standard Text Interface ROM
     *	    	(offsets 0x0013, 0x0017, 0x001b, 0x001f of STI ROM).
     * gcDescribe.crt_control_base:
     *	    on all hpux systems, will point to start of control register
     *	    space, skipping over 1MB of ROM space.
     * gcDescribe.crt_frame_base:
     *	    on all hpux systems, will point to start of framebuffer,
     *	    offset 16MB from start of NGLE address space.
     *
     * For 2-headed devices such as Dual CRX, these pDregs and fbaddr
     * settings will be overridden in the device-specific processing
     * below (addresses of 2nd "right" head returned as crt_regions).
     */
    pScreenPriv->deviceID   = gcDescribe.crt_id;
    pScreenPriv->pDregs     = pDregs
			    = (NgleHdwPtr) gcDescribe.crt_control_base;
    pScreenPriv->fbaddr     = (pointer) gcDescribe.crt_frame_base;
    pScreenPriv->devWidth 	= gcDescribe.crt_total_x; 
    pScreenPriv->screenWidth 	= gcDescribe.crt_x; 
    pScreenPriv->screenHeight 	= gcDescribe.crt_y; 

    for (i=0; i < CRT_MAX_REGIONS; i++)
    {
	pScreenPriv->crt_region[i]   = gcDescribe.crt_region[i];
    }
    
    /* Set the device frame buffer depth.  The default depth is set to
     * 8, but can be changed by setting the depth parameter in the
     * Xnscreens file.  Currently, we only support depth 24 on CRX24 & HCRX24,
     * and depth 8 on the remaining displays.
     */
    
    if ((pScreenPriv->deviceID == S9000_ID_A1439A) ||
	(pScreenPriv->deviceID == S9000_ID_HCRX))
        pScreenPriv->devDepth = php->depth;
    else
	pScreenPriv->devDepth = 8;
    
    if (pScreenPriv->deviceID == S9000_ID_TOMCAT)
        {
	    /* If second "right" device, grab addresses of frame buffer
	     * and control space from crt_regions rather than the
	     * typical crt_frame_base and crt_control_base.
	     *
	     * Convention: bit 2 (third least significant bit) of
	     * device file's minor number == 1 indicates right device.
	     */
	    if (IS_NOT_FIRST_HEAD_ON_THIS_SGC_SLOT(statInfo.st_rdev))
	    {
	        pScreenPriv->pDregs
			        = pDregs
                                = (NgleHdwPtr) gcDescribe.crt_region[3];
	        pScreenPriv->fbaddr    
			        = (pointer) gcDescribe.crt_region[2];
            }
	}


    /* Graphics devices will typically call ioctl(CGUNMAP) at screen close.
     *
     * Exception: the second device on a Tomcat (Dual CRX),
     * where "second" means that the other head on the same Dual CRX
     * appears before it in the X*screens file.
     *
     * Set typical value here and allow overriding in device-specific
     * processing below.
     */
    pScreenPriv->lastDeviceOnThisSgcToClose = TRUE;


    /*
     **************************************************************************
     **
     ** 	Device Dependent Initializations
     **
     **************************************************************************
     */

    pScreenPriv->pDevRomData = ngleGetDeviceRomData(pScreenPriv);

    /* Device-specific settings */
    switch (pScreenPriv->deviceID)
    {
    case S9000_ID_ELM:		/* A1924 Gray-Scale GRX */
	
	pScreenPriv->isGrayScale = TRUE;

	/* Since A1924 is a grayscale CRX and the grayscale variable
	 * has just been set, set device ID 
	 * to that of CRX so that the rest of the driver may
	 * appropriately consider this to be a CRX.
	 */
	pScreenPriv->deviceID   = S9000_ID_A1659A;
	break;

    case S9000_ID_HCRX:		/* Hyperdrive */
	/* Hyperdrive doesn't use STI ROM info so don't bother reading it */
	pScreenPriv->pDevRomData = NULL; /* #### strange #### */
	NGLE_LOCK(pScreenPriv);
	{
	    CARD32          *sti_rom_address;
	    CARD32          temp;
	    /*
	     * In order to read the Hyperdrive configuration bits, we need to
	     * read the STI ROM.  But, the STI ROM is not always mapped into
	     * the STI ROM space.  Sometimes the STI ROM is mapped into the
	     * BOOT ROM space.  To find the true STI ROM space, we need to
	     * look at the crt_region array in the gcDescribe structure.
	     * The STI ROM address will either be in entry 0 or entry 1.
	     * If entry 0 is mapped into the same space as the control space,
	     * then this is the true STI ROM space.  If entry 0 is in another
	     * space, then entry 1 has the true STI ROM space.  The space bits
	     * of the address are the upper 6 bits of the address.  For example,
	     * if the address is 0xf4100000, then the space is 0xf4000000.
	     */
	    if (((CARD32) pScreenPriv->pDregs & 0xfc000000) ==
		((CARD32) pScreenPriv->crt_region[0] & 0xfc000000))
	    {
		sti_rom_address = (CARD32 *) pScreenPriv->crt_region[0];
	    }
	    else
	    {
		sti_rom_address = (CARD32 *) pScreenPriv->crt_region[1];
	    }

	    /*
	     * Ok, grab the configuration bits from the hardware.  These bits
	     * come from Dodger and are added to any data read from the STI ROM.
	     * When reading the STI ROM, we need to avoid a potential race
	     * condition by doing a couple of things.  We first need to read
	     * some unbuffered register like BIFF status.  Next we need to wait
	     * for Dodger to go not busy.  Then we can safely read the STI ROM.
	     */
	    temp = NGLE_READ32(pDregs->reg15.all);
	    SETUP_HW(pDregs);
	    pScreenPriv->deviceSpecificConfig = *sti_rom_address;
	}
	NGLE_UNLOCK(pScreenPriv);
	
	pScreenPriv->isGrayScale = FALSE;   /* No grayscale version */
	break;

    case S9000_ID_ARTIST:	/* Artist */
    case S9000_ID_A1439A:	/* CRX24 */
    case S9000_ID_A1659A:	/* CRX */
	
	pScreenPriv->isGrayScale = FALSE;   /* No grayscale version */
	break;

    case S9000_ID_TIMBER:	/* HP9000/710 Graphics */
	/* There are 3 configurations, all with the
	 * same Graphics ID:
	 * - 1280x1024 Color
	 * - 1280x1024 Grayscale
	 * - 1024x768  Color.
	 * The difference in color resolutions is handled simply
	 * by using the width/height values in gcDescribe.
	 * Color vs. Grayscale is handled by placing the
	 * word "GRAYSCALE" in the crt_name returned in gcDescribe
	 * for grayscale Timber graphics devices.
	 */
	/* Be color unless grayscale indicated in crt_name
	 */
	if (	stringContainsString(gcDescribe.crt_name, "GRAYSCALE")
	    ||  stringContainsString(gcDescribe.crt_name, "Grayscale")
	    ||  stringContainsString(gcDescribe.crt_name, "grayscale"))
	{
	    pScreenPriv->isGrayScale = TRUE;
	}
	else
	{
	    pScreenPriv->isGrayScale = FALSE;
	}
	break;

    case S9000_ID_TOMCAT:	/* Dual CRX */
	
	pScreenPriv->isGrayScale = FALSE;

	/* If not the head first appearing in X*screens file:
	 * -	set variable so ioctl(GCUNMAP) is not called at exit.
	 * -	Initialize related screen pointers for this 2nd
	 *	screen AND for related screen opened earlier.
	 */
	if (!firstScreenOpenedOnThisSgcSlot(
		index, statInfo.st_rdev))
	{
	    pScreenPriv->lastDeviceOnThisSgcToClose = FALSE;
	}

	/* Dual CRX supports several resolutions: 1280x1024, 1024x768, 640x480.
	 * This code reads the BIFF scratch register to get the index into a
         * device dependent ROM array containing  the device's currently 
         * selected resolution.
         */
         pScreenPriv->screenWidth = pScreenPriv->pDevRomData->x_size_visible ;
         pScreenPriv->screenHeight = pScreenPriv->pDevRomData->y_size_visible;

	/* Since Tomcat is a 2-headed CRX and all Tomcat-specific
	 * processing has just been completed, set the device ID
	 * to that of CRX so that the rest of the driver may
	 * appropriately consider this to be a CRX.
	 */
	pScreenPriv->deviceID   = S9000_ID_A1659A;
	break;

    default:
	FatalError("Undefined device id!\n");
	break;
    } /* switch (pScreenPriv->deviceID) */


    /*
     **************************************************************************
     **
     ** 	Perform ioctl() initialization of graphics device.
     **
     **************************************************************************
     */

    ioctl(fildes, GCSTATIC_CMAP, &mapOrigin);
     

    /**************************************************************************
     **
     ** 	Set up the Ngle hardware in a byte-per-pixel mode.
     **
     **************************************************************************
     */
     
    SETUP_FB(pDregs,pScreenPriv->deviceID,pScreenPriv->devDepth);

    
    /**************************************************************************
     **
     ** 	Set up screen dimensions.  These values are needed to pass
     **  	to cfbScreenInit().
     **
     **************************************************************************
     */
       
    /* Use the hp procedure to define the screen width and height,
     * using the screen's diagonal, which defaults to 19" for CRX
     * but may be overridden in the X*screens file "monitorsize <n>"
     * parameter (in which case php->MonitorDiagonal is non-zero).
     */
    if (php->MonitorDiagonal != 0)
	monitorDiagonal = php->MonitorDiagonal;
    else
    {	/* A 1024x768 CRX (with same ID) may be offered.
	 * It will use 16" monitor.  The following rule assumes
	 * that higher resolution versions will use 19" monitor
	 * (Currently the case with CRX and CRX24).
	 */
	if (pScreenPriv->screenWidth <= 1024)
	{
	    monitorDiagonal = LOWRES_CRX_MONITOR_DIAGONAL;  /* 16 inches */
	}
	else
	{
	    monitorDiagonal = DEFAULT_CRX_MONITOR_DIAGONAL; /* 19 inches */
	}
    }

    dpi = calculateDPI( pScreenPriv->screenWidth, pScreenPriv->screenHeight,
		       monitorDiagonal);


#ifdef HP_FAST_SCROLLING
    /*
     **************************************************************************
     **
     **		Change the CFB GCOpts tables to use our CopyArea routine
     **		instead of the CFB version.  Do this before calling and
     **		CFB initialization code.
     **
     **	Note:	If in the future, CFB adds / deletes / changes GCOpts
     **		tables, respective changes will have to be made in this
     **		code also.
     **
     **************************************************************************
     */

    cfbTEOps1Rect.CopyArea	= ngleCopyArea8;
    cfbNonTEOps1Rect.CopyArea	= ngleCopyArea8;
    cfbTEOps.CopyArea		= ngleCopyArea8;
    cfbNonTEOps.CopyArea	= ngleCopyArea8;

    cfb32TEOps1Rect.CopyArea	= ngleCopyArea24;
    cfb32NonTEOps1Rect.CopyArea	= ngleCopyArea24;
    cfb32TEOps.CopyArea		= ngleCopyArea24;
    cfb32NonTEOps.CopyArea	= ngleCopyArea24;
#endif


    /*
     **************************************************************************
     **
     ** 11.     Call pNgleScreenInit->InitMiscConfig to perform
     **         device-dependent configuration and initialization
     **         that does not change the hardware state.
     **
     **             FIRST READ-ONLY ACCESS TO HARDWARE BY SERVER
     **     (not counting indirect access via kernel ioctl calls)
     **************************************************************************
     */
    if (pScreenPriv->deviceID == S9000_ID_HCRX)
    {
	Card32          hyperbowl;
        Int32           nFreeFifoSlots = 0;

	/* Initialize Hyperbowl registers */
        GET_FIFO_SLOTS(nFreeFifoSlots, 7);
        if (IS_24_DEVICE(pScreenPriv))
        {
            if (pScreenPriv->devDepth == 24)
                hyperbowl = HYPERBOWL_MODE01_8_24_LUT0_TRANSPARENT_LUT1_OPAQUE;
            else
                hyperbowl = HYPERBOWL_MODE01_8_24_LUT0_OPAQUE_LUT1_OPAQUE;

	    /* First write to Hyperbowl must happen twice (bug) */
	    NGLE_WRITE32(pDregs->reg40, hyperbowl);
	    NGLE_WRITE32(pDregs->reg40, hyperbowl);

	    NGLE_WRITE32(pDregs->reg39, HYPERBOWL_MODE2_8_24);

	    NGLE_WRITE32(pDregs->reg42, 0x014c0148); /* Set lut 0 to be the direct color */
	    NGLE_WRITE32(pDregs->reg43, 0x404c4048);
	    NGLE_WRITE32(pDregs->reg44, 0x034c0348);
	    NGLE_WRITE32(pDregs->reg45, 0x444c4448);
        }
        else
        {
            hyperbowl = HYPERBOWL_MODE_FOR_8_OVER_88_LUT0_NO_TRANSPARENCIES;

	    /* First write to Hyperbowl must happen twice (bug) */
	    NGLE_WRITE32(pDregs->reg40, hyperbowl);
	    NGLE_WRITE32(pDregs->reg40, hyperbowl);

	    NGLE_WRITE32(pDregs->reg42, 0);
	    NGLE_WRITE32(pDregs->reg43, 0);
	    NGLE_WRITE32(pDregs->reg44, 0);
	    NGLE_WRITE32(pDregs->reg45, 0x444c4048);
        }
    }


    /*
     **************************************************************************
     **
     **		Call CFB routines to begin setting up of screen and
     **		visual default values.
     **
     ** 	Call cfbScreenInit() to set up default pScreen values, but
     **		first call cfbSetVisualTypes to only support a subset of the
     **		possible visuals.
     **
     **************************************************************************
     */
    
    if (pScreenPriv->devDepth == 8)
             /* only support PseudoColor */
    {
        cfbSetVisualTypes (8, 1<<PseudoColor, 8);
	
	cfbScreenInit(pScreen, pScreenPriv->fbaddr, pScreenPriv->screenWidth,
		  pScreenPriv->screenHeight, dpi, dpi,
		  pScreenPriv->devWidth);
    }

    else  /* depth = 24 */
	  /* only support Directcolor */
    {
	cfbSetVisualTypes (24, 1<<DirectColor, 8);
	
    	cfb32ScreenInit(pScreen, pScreenPriv->fbaddr, pScreenPriv->screenWidth,
		  pScreenPriv->screenHeight, dpi, dpi,
		  pScreenPriv->devWidth);
    }

    /*
     * The following section of code is to correct a bug in the MIT-provided
     * visual initialization code.  CFB assumes that for a depth 24 display,
     * all hardware writes the bits to the frame buffer in the blue - green -
     * red order.  For HP's Ngle family of depth 24 displays, we write
     * them in RGB order, not GBR order.  To correct this problem, I will
     * change the affected values of the visual records to the correct
     * values.
     */

    pVisuals = pScreen->visuals;
    for (i = 1; i <= pScreen->numVisuals ; i++ )
    {
	if ((pVisuals->class == DirectColor) && (pVisuals->nplanes == 24))
	{
	    /* correct the RGB masks and offsets that were set by */
	    /* cfbScreenInit().                                   */
	    pVisuals->redMask           = 0xff0000;
	    pVisuals->greenMask         = 0xff00;
	    pVisuals->blueMask          = 0xff;
	    pVisuals->offsetRed         = 16;
	    pVisuals->offsetGreen       = 8;
	    pVisuals->offsetBlue        = 0;
	}
	pVisuals++;
    }


    /*
     * This section of code is to correct a bug in the CFB 
     * implementation of dual head support.
     */

    if (cfbDualHeadBug != serverGeneration)
    {
	if (pScreenPriv->devDepth == 8)
	    cfbFirstIndex = cfbScreenPrivateIndex;
	else
	    cfbFirstIndex = cfb32ScreenPrivateIndex;
	
	cfbDualHeadBug = serverGeneration;
    }
    else
    {
	if (pScreenPriv->devDepth == 8)
	    pScreen->devPrivates[cfbFirstIndex].ptr = 
	        pScreen->devPrivates[cfbScreenPrivateIndex].ptr;
	else
	    pScreen->devPrivates[cfbFirstIndex].ptr = 
	        pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr;
	
	cfbScreenPrivateIndex = cfb32ScreenPrivateIndex = cfbFirstIndex;
    }


    /*
     **************************************************************************
     **
     **     Fill in Many of the pScreen Procedure Pointers and Values:
     **
     **************************************************************************
     */

    /**** Random screen procedures: ****/
    pScreen->CloseScreen		= ngleCloseScreen;
    pScreen->SaveScreen			= ngleSaveScreen;

    /**** Colormap Procedures: ****/
    pScreen->CreateColormap		= ngleCreateColormap;
    pScreen->DestroyColormap		= ngleDestroyColormap; 
    pScreen->InstallColormap		= ngleInstallColormap;
    pScreen->UninstallColormap		= ngleUninstallColormap;
    pScreen->ListInstalledColormaps	= ngleListInstalledColormaps;
    pScreen->StoreColors		= ngleStoreColors;
    pScreen->ResolveColor		= ngleResolvePseudoColor;


    /*
     **************************************************************************
     **
     **     Set White and Black Pixel.
     **
     **************************************************************************
     */

    /* Set the pixel index for the default black pixel and white pixel.
     * Used to be always 0 and 1, respectively, but with gray scale
     * default colormap ramping from 1 to 255, it made sense to make
     * 255 the white pixel.
     */
    pScreen->blackPixel = 0;
    pScreen->whitePixel = 1;
    if (pScreenPriv->isGrayScale)
    {
	pScreen->whitePixel = 255;
    }
    else if (pScreenPriv->devDepth == 24)
    {
	pScreen->blackPixel = 0x000000;
	pScreen->whitePixel = 0xffffff;
    }


    /*
     **************************************************************************
     **
     ** 	Fill in Many Parts of the Screen Private Structure that
     **		haven't been filled in yet:
     **
     **************************************************************************
     */

    /* Allow user to override driver control over whether
     * cursors are moved only during vertical blank.
     */
    pScreenPriv->moveCursorOnVBlank = CURSOR_AT_VBLANK_DRIVEROPTION;
    
    moveCursorEnvar = getenv("HPGCRX_MOVE_CURSOR_ON_VBLANK");
    if (    (strcmp(moveCursorEnvar, "FALSE") == 0)
	||  (strcmp(moveCursorEnvar, "False") == 0)
	||  (strcmp(moveCursorEnvar, "false") == 0)
	||  (strcmp(moveCursorEnvar, "0") == 0))
    {
	pScreenPriv->moveCursorOnVBlank =  CURSOR_AT_VBLANK_NEVER;
    }
    else
    {
        if (    (strcmp(moveCursorEnvar,"TRUE") == 0)
            ||  (strcmp(moveCursorEnvar,"True") == 0)
            ||  (strcmp(moveCursorEnvar,"true") == 0)
            ||  (strcmp(moveCursorEnvar,"1") == 0))
	{
	    pScreenPriv->moveCursorOnVBlank =  CURSOR_AT_VBLANK_ALWAYS;
	}
    }


    /*
     **************************************************************************
     **
     ** 	Initialize the Colormap Structures and Code:
     **
     **		Causes default colormap to be loaded into hardware,
     **		so must follow general hardware setup.
     **
     **************************************************************************
     */

    pScreen->defColormap = (Colormap) FakeClientID(0);
    ngleCreateDefColormap(pScreen);


    /*
     **************************************************************************
     **
     ** 	Initialize the Cursor (Sprite and Echo) Structures, Code,
     **		and pScreen procedure pointers for cursors:
     **
     **************************************************************************
     */

    if (pScreenPriv->deviceID == S9000_ID_HCRX)
    {
	hyperInitSprite(pScreen);
    }
    else
    {
	ngleInitSprite(pScreen);
    }

    /*
     **************************************************************************
     **
     ** 20.     Call hyperResetPlanes() to initialize the image,
     **         overlay and attribute planes.  This includes doing what is
     **         necessary to counteract what the ITE has done
     **
     **************************************************************************
     */

    if (pScreenPriv->deviceID == S9000_ID_HCRX)
    {
	hyperResetPlanes(pScreenPriv, SERVER_INIT);
    }

     /* Initialize the non HCRX image planes. */

    /* On CRX24, ITE has set up overlay planes to be 3-plane
     * device.  Set up overlays to be 8 planes and write all
     * pixels to transparent.  Then set up the framebuffer colormap
     * to use all 8 bits.
     *
     * On CRX, set up the framebuffer colormap to use
     * all 8 bits.
     */
    
    else if (pScreenPriv->deviceID == S9000_ID_A1439A)	/* CRX24 */
    {
	rattlerSetupPlanes(pScreenPriv);
    }
    else if ((pScreenPriv->deviceID == S9000_ID_A1659A) || /* CRX or GRX */
	     (pScreenPriv->deviceID == S9000_ID_ARTIST)    /* Artist */
	)
    {
	elkSetupPlanes(pScreenPriv);
    }

    /* Clear attribute planes on non HCRX devices.
     */
    if ((pScreenPriv->deviceID == S9000_ID_A1659A) ||	/* CRX or GRX */
	(pScreenPriv->deviceID == S9000_ID_A1439A) ||	/* CRX24 */
	(pScreenPriv->deviceID == S9000_ID_ARTIST)	/* Artist */
       )
    {
	if (pScreenPriv->devDepth == 24)
	    ngleSetupAttrPlanes(pScreenPriv, BUFF1_CMAP3);
	else  /* depth = 8 */
	    if (pScreenPriv->deviceID == S9000_ID_ARTIST)
		ngleSetupAttrPlanes(pScreenPriv, ARTIST_CMAP0);
	    else
		ngleSetupAttrPlanes(pScreenPriv, BUFF1_CMAP0);
    }

    /*
     **************************************************************************
     **
     ** 	Initialize screen saver:
     **
     **************************************************************************
     */

    /* Make sure video blank enable is turned off.  Could happen in
     * rare case: kill -9 of blanked X server (ngleCloseScreen
     * code not executed).
     */
    ngleBlankOrUnblankScreen(pScreenPriv, SCREEN_SAVER_OFF);


    /*
     **************************************************************************
     **
     ** 	Put the Ngle hardware into a byte-per-pixel state:
     **
     **************************************************************************
     */

    SETUP_FB(pDregs,pScreenPriv->deviceID,pScreenPriv->devDepth);


    /*
     **************************************************************************
     **
     ** 	If we got to here, ngleScreenInit() was successful and we
     **		should return TRUE:
     **
     **************************************************************************
     */
    return(TRUE);

} /* ngleScreenInit() */



/******************************************************************************
 *
 * NGLE DDX Utility Procedure:	elkSetupPlanes
 *
 ******************************************************************************/

static void elkSetupPlanes(
    NgleScreenPrivPtr       pScreenPriv)
{
    NgleHdwPtr              pDregs;

    pDregs 	= (NgleHdwPtr) pScreenPriv->pDregs;

    /**********************************************
     * Write RAMDAC pixel read mask register so all overlay
     * planes are display-enabled.  (CRX uses Bt458 pixel
     * read mask register).
     **********************************************/
    SETUP_RAMDAC(pDregs);
    
    SETUP_FB(pDregs, pScreenPriv->deviceID, pScreenPriv->devDepth);

}   /* elkSetupPlanes() */



/******************************************************************************
 *
 * NGLE DDX Utility Procedure:	rattlerSetupPlanes
 *
 ******************************************************************************/

static void rattlerSetupPlanes(
    NgleScreenPrivPtr       pScreenPriv)
{
    NgleHdwPtr		pDregs;
    Int32		x, y;
    Card32		*bits;

    pDregs 	= (NgleHdwPtr) pScreenPriv->pDregs;

    /**********************************************
     * Write RAMDAC pixel read mask register so all overlay
     * planes are display-enabled.  (CRX24 uses Bt462 pixel
     * read mask register for overlay planes, not image planes).
     **********************************************/
    CRX24_SETUP_RAMDAC(pDregs);
    
    SETUP_FB(pDregs, CRX24_OVERLAY_PLANES, pScreenPriv->devDepth);
	
    for (y=0; y < pScreenPriv->screenHeight; y++)
    {
	bits = (Card32 *) ((char *)pScreenPriv->fbaddr + y * 
					pScreenPriv->devWidth);
	x = pScreenPriv->screenWidth >> 2;
	do 
	{
	    *bits++ = 0xffffffff;
	}  while (x--);
    }
    
    CRX24_SET_OVLY_MASK(pDregs);
    SETUP_FB(pDregs,pScreenPriv->deviceID,pScreenPriv->devDepth);

}   /* rattlerSetupPlanes() */



/******************************************************************************
 *
 * NGLE DDX Utility Procedure:	hyperSetupPlanes
 *
 ******************************************************************************/

static void hyperSetupPlanes(
    NgleScreenPrivPtr       pScreenPriv)
{
    NgleHdwPtr		pDregs;
    Int32		x, y;
    Card32		*bits;

    pDregs 	= (NgleHdwPtr) pScreenPriv->pDregs;

    /**************************************************
     ** Need to clear screen
     **************************************************/
/*
    if (IS_24_DEVICE(pScreenPriv))
	ngleDepth24_ClearImagePlanes(pScreenPriv);
    else
	ngleDepth8_ClearImagePlanes(pScreenPriv);

*/
    /**********************************************
     * Write RAMDAC pixel read mask register so all overlay
     * planes are display-enabled.  (CRX24 uses Bt462 pixel
     * read mask register for overlay planes, not image planes).
     **********************************************/
    HCRX_SETUP_RAMDAC(pDregs);
    
	
    SETUP_FB(pDregs, S9000_ID_HCRX, pScreenPriv->devDepth);
	
    for (y=0; y < pScreenPriv->screenHeight; y++)
    {
	bits = (Card32 *) ((char *)pScreenPriv->fbaddr + y * 
					pScreenPriv->devWidth);
	x = pScreenPriv->screenWidth >> 2;
	do 
	{
	    *bits++ = 0xffffffff;
	}  while (x--);
    }
    
    CRX24_SET_OVLY_MASK(pDregs);
    SETUP_FB(pDregs,pScreenPriv->deviceID,pScreenPriv->devDepth);

}   /* hyperSetupPlanes() */



/******************************************************************************
 *
 * NGLE DDX Utility Procedure:	ngleSetupAttrPlanes
 *
 ******************************************************************************/

static void ngleSetupAttrPlanes(
    NgleScreenPrivPtr	    pScreenPriv,
    Card32		    BufferNumber)
{
    NgleHdwPtr	    pDregs = pScreenPriv->pDregs;

    SETUP_ATTR_ACCESS(pDregs, BufferNumber);

    SET_ATTR_SIZE(pDregs, pScreenPriv->screenWidth, pScreenPriv->screenHeight);

    FINISH_ATTR_ACCESS(pDregs);

    SETUP_FB(pDregs, pScreenPriv->deviceID, pScreenPriv->devDepth);

} /* ngleSetupAttrPlanes() */



/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleClearScreen()
 *
 * Description:
 *
 *	This local routine clears the screen to all zeroes.
 *	(this is not speed-critical routine - used only at server exit)
 *	Slow but thorough technique: clear both application buffers
 *	and for Ratter, overlay planes as well.
 *
 ******************************************************************************/

static Bool ngleClearScreen(
    ScreenPtr	pScreen)
{
    NgleScreenPrivPtr       pScreenPriv;
    Int32		    x, y;
    Card32		    *bits;

    pScreenPriv = NGLE_SCREEN_PRIV(pScreen);

    if (pScreenPriv->devDepth == 24)  /* depth 24 - need different writes */
    {
        for(y=0; y<pScreenPriv->screenHeight; y++)
        {
	    bits = (Card32 *)((char *)pScreenPriv->fbaddr + 
				(y * pScreenPriv->devWidth * 4));
	    x = pScreenPriv->screenWidth;
	    do {
	        *bits++ = 0;
	    } while(--x);
        } 
    }
    else  /* depth 8 */
    {
        for(y=0; y<pScreenPriv->screenHeight; y++)
        {
	    bits = (Card32 *)((char *)pScreenPriv->fbaddr + y * pScreenPriv->devWidth);
	    x = pScreenPriv->screenWidth >> 2;
	    do {
	        *bits++ = 0;
	    } while(--x);
        } 
    }

    /* Clear attribute planes on Hyperdrive, Artist, CRX, CRX24 and GRX devices.
     */
    if ((pScreenPriv->deviceID == S9000_ID_A1659A) ||	/* CRX or GRX */
	(pScreenPriv->deviceID == S9000_ID_A1439A) ||   /* CRX24 */
	(pScreenPriv->deviceID == S9000_ID_ARTIST) ||   /* Artist */
	(pScreenPriv->deviceID == S9000_ID_HCRX)	/* Hyperdrive */
       )
    {
	ngleSetupAttrPlanes(pScreenPriv, BUFF0_CMAP0);
    }
} /* ngleClearScreen() */



/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleBlankOrUnblankScreen
 *
 * Description:
 *
 *	This routine is intended to be called by pScreen->SaveScreen
 *	(a DDX entry point), as well as at server entry and exit.
 *
 *	Video for pScreen is turned on or off, depending on 2nd parameter.
 *
 ******************************************************************************/

static void ngleBlankOrUnblankScreen(
    NgleScreenPrivPtr	pScreenPriv, 
    Bool		blankOrUnblank)
{
    NgleHdwPtr          pDregs		= pScreenPriv->pDregs;


    if (blankOrUnblank == SCREEN_SAVER_ON)
    {
	/* Turn on screen blanking (i.e. blank the screen) */

	/* Disable cursor */
	ngleDisableSprite(pScreenPriv->pScreen);

	/* Disable image display by enabling display of
	 * color 0 of the cursor.  Action accomplished
	 * by writing commands to AUX interface.  Since
	 * CRX24 and CRX/Timber/Tomcat use different
	 * Brooktree DAC parts, the commands written vary.
	 */
	if (pScreenPriv->deviceID == S9000_ID_A1439A)
	{   /* CRX24 */

	    CRX24_DISABLE_DISPLAY(pDregs);
	}
	else if (pScreenPriv->deviceID == S9000_ID_ARTIST)
	{
	    ARTIST_DISABLE_DISPLAY(pDregs);
	}
	else if (pScreenPriv->deviceID == S9000_ID_HCRX)
	{
	    HYPER_DISABLE_DISPLAY(pDregs);
	}
	else
	{   /* CRX and like elk */
	    DISABLE_DISPLAY(pDregs);
	}
    }
    else /* if (blankOrUnblank == SCREEN_SAVER_OFF) */
    {	/* Turn off screen blanking */

	/* If the cursor is on this screen, display it.
	 * An HP input procedure (hp/hp/x_hil.c:process_motion())
	 * maintains a variable indicating which screen has the cursor.
	 */
	extern int hpActiveScreen; /* active screen index in x_hil.c */
    	if (pScreenPriv->myNum == hpActiveScreen)
	{
	    if (pScreenPriv->deviceID == S9000_ID_HCRX)
	    {
		hyperDisplayCursor(pScreenPriv->pScreen,
		    pScreenPriv->sprite.pCursor);
	    }
	    else
	    {
		ngleDisplayCursor(pScreenPriv->pScreen,
		    pScreenPriv->sprite.pCursor);
	    }
	}

	/* Enable image display by disabling display of
	 * color 0 of the cursor.
	 */
	if (pScreenPriv->deviceID == S9000_ID_A1439A)
	{   /* CRX24 */
	    CRX24_ENABLE_DISPLAY(pDregs);
	}
	else if (pScreenPriv->deviceID == S9000_ID_ARTIST)
	{
	    ARTIST_ENABLE_DISPLAY(pDregs);
	}
	else if (pScreenPriv->deviceID == S9000_ID_HCRX)
	{
	    HYPER_ENABLE_DISPLAY(pDregs);
	}
	else
	{   /* CRX and like elk */
	    ENABLE_DISPLAY(pDregs);
	}
    }

    SETUP_FB(pDregs,pScreenPriv->deviceID,pScreenPriv->devDepth);

}   /* ngleBlankOrUnblankScreen() */



/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleSaveScreen()
 *
 * Description:
 *
 *	This routine is intended to implement pScreen->SaveScreen,
 *	a DDX entry point.  Video for pScreen is turned on or off,
 *	depending on parameter "on".
 *
 *	A lower-level procedure is called to actually perform
 *	the blanking or unblanking.
 *
 ******************************************************************************/

static Bool ngleSaveScreen(
    ScreenPtr		pScreen,
    Bool		on)
{
    hpPrivPtr		php;
    NgleScreenPrivPtr	pScreenPriv;


    pScreenPriv = NGLE_SCREEN_PRIV(pScreen);
    php 	= (hpPrivPtr)pScreen->devPrivate;

    if (on == SCREEN_SAVER_ON)
    {	/* Blank screen */

    	if (php->isSaved) return(TRUE);
      	php->isSaved = TRUE;

	ngleBlankOrUnblankScreen(pScreenPriv, SCREEN_SAVER_ON);
    }
    else
    {	/* Uninstall the screen saver: */

    	if (!php->isSaved) return(TRUE);
      	php->isSaved = FALSE;

	ngleBlankOrUnblankScreen(pScreenPriv, SCREEN_SAVER_OFF);
    }

    return(TRUE);

} /* ngleSaveScreen() */



/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleCloseScreen()
 *
 * Description:
 *
 *	This routine implements pScreen->CloseScreen for the NGLE DDX Driver.
 *
 ******************************************************************************/

static Bool ngleCloseScreen(index, pScreen)

    Int32		index;		/* Screen index */
    ScreenPtr		pScreen;	/* Pointer to screen */
{
    NgleScreenPrivPtr	pScreenPriv;
    hpPrivPtr		php;
    Card8		*mapOrigin;
    Int32		retVal;


    pScreenPriv = NGLE_SCREEN_PRIV(pScreen);
    php = (hpPrivPtr)pScreen->devPrivate;

    /* If exiting rather than just resetting, remove cursor and clear */
    if (hpGivingUp)
    {
	(*php->CursorOff)(pScreen);
	if (pScreenPriv->deviceID == S9000_ID_HCRX)
	{
	    hyperResetPlanes(pScreenPriv, SERVER_EXIT);
	}
	else
	{
	    ngleClearScreen(pScreen);
	}
    }

    /* Turn off screen saver (if on) */
    ngleSaveScreen(pScreen, SCREEN_SAVER_OFF);

    /* Allow ITE to change the colormap */
    ioctl(pScreenPriv->fildes, GCVARIABLE_CMAP, &mapOrigin);

    /* Use new and improved ITE soft reset which is almost 
     * finished but doesn't have a header file yet        
     *
     * Only issue at server exit, not server recycle.	  
     *
     * For multi-headed device (Tomcat), only issue if left head
     * (which is only head that can be an ITE console on Tomcat).
     */
#define GCTERM _IOWR('G',20,int)
    if ((hpGivingUp) && 
	(!IS_NOT_FIRST_HEAD_ON_THIS_SGC_SLOT(pScreenPriv->dev_sc)))
    {
	int garbage=0;
	ioctl(pScreenPriv->fildes, GCTERM, &garbage);
    }


    /* Release NGLE control space */
    mapOrigin = (Card8 *) 0;

    /* If a multi-headed device, only unmap framebuffer if
     * last device on the SGC card to close.
     */
    if (pScreenPriv->lastDeviceOnThisSgcToClose)
    {
	if (ioctl(pScreenPriv->fildes, GCUNMAP, &mapOrigin) < 0)
	    return(FALSE);
    }

    /* Free data structures */
    ngleUninitSprite(pScreen);
    Xfree(hpPrivates[index]);

    close(pScreenPriv->fildes);

    /* Free NGLE private structure */
    Xfree(pScreenPriv);

    return(TRUE);

} /* ngleCloseScreen() */


/******************************************************************************
 *
 *  And now for some new routines to handle HCRX8 and HCRX24
 *
 ******************************************************************************/

/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleDepth8_ClearImagePlanes()
 *
 * Description:
 *
 *	This routine clears the image planes for depth 8 devices.
 *
 * Assumptions:
 *	Assumptions fast-locking has been initialized.
 *	Does not assume a lock is in effect.
 *
 ******************************************************************************/

void ngleDepth8_ClearImagePlanes(
    NgleScreenPrivPtr       pScreenPriv)
{
    NgleHdwPtr              pDregs;
    NGLE_MFGP_REGISTER_TYPE packedLenXY;
    NGLE_MFGP_REGISTER_TYPE packedDstXY;
    Int32		    nFreeFifoSlots = 0;

    pDregs 	= (NgleHdwPtr) pScreenPriv->pDregs;

    NGLE_LOCK(pScreenPriv);

    /* Common Hardware setup */
    GET_FIFO_SLOTS(nFreeFifoSlots, 5);

    /* Re-use dstX/Y and transfer data for multiple recfills.  */
    NGLE_SET_SCOREBOARD_OVERRIDE(0x30003);
    NGLE_SET_TRANSFERDATA(0xffffffff);   /* Write foreground color */

    NGLE_REALLY_SET_IMAGE_FG_COLOR(0);	
    NGLE_REALLY_SET_IMAGE_PLANEMASK(0xff);

    PACK_2CARD16(packedDstXY, 0, 0);
    PACK_2CARD16(packedLenXY, pScreenPriv->screenWidth,
		 pScreenPriv->screenHeight);
    NGLE_SET_DSTXY(NGLE_MFGP_REGISTER_TYPE_ASLONG(packedDstXY));

    /* Device-specific image buffer clear */
    switch(pScreenPriv->deviceID)
    {
    case S9000_ID_ARTIST:
	/* Write zeroes to buffer */
	GET_FIFO_SLOTS(nFreeFifoSlots, 3);
	NGLE_QUICK_SET_IMAGE_BITMAP_OP(
	    IBOvals(RopSrc, MaskAddrOffset(0),
		    BitmapExtent08, StaticReg(FALSE),
		    DataDynamic, MaskOtc, BGx(FALSE), FGx(FALSE)));
	NGLE_QUICK_SET_DST_BM_ACCESS(
	    BA(     IndexedDcd, Otc32, OtsIndirect, AddrLong,
		BAJustPoint(0), BINapp0I, BAIndexBase(0)));
	SET_LENXY_START_RECFILL(packedLenXY);
	break;

    case S9000_ID_A1659A:   /* ELK_DEVICE_ID */
	/* Write zeroes to buffer 0 */
	GET_FIFO_SLOTS(nFreeFifoSlots, 3);
	NGLE_QUICK_SET_IMAGE_BITMAP_OP(
	    IBOvals(RopSrc, MaskAddrOffset(0),
		    BitmapExtent08, StaticReg(FALSE),
		    DataDynamic, MaskOtc, BGx(FALSE), FGx(FALSE)));
	NGLE_QUICK_SET_DST_BM_ACCESS(
	    BA(	IndexedDcd, Otc32, OtsIndirect, AddrLong,
		BAJustPoint(0), BINapp0I, BAIndexBase(0)));
	SET_LENXY_START_RECFILL(packedLenXY);

	/* Write zeroes to buffer 1 */
	GET_FIFO_SLOTS(nFreeFifoSlots, 2);
	NGLE_QUICK_SET_DST_BM_ACCESS(
	    BA(	IndexedDcd, Otc32, OtsIndirect, AddrLong,
		BAJustPoint(0), BINapp1I, BAIndexBase(0)));
	SET_LENXY_START_RECFILL(packedLenXY);
	break;

    case S9000_ID_HCRX:
	/* Write zeroes to buffer 0 */
	GET_FIFO_SLOTS(nFreeFifoSlots, 3);
	NGLE_QUICK_SET_IMAGE_BITMAP_OP(IBOvals(RopSrc, MaskAddrOffset(0),
				  BitmapExtent08, StaticReg(FALSE),
				  MaskDynamic, MaskOtc,
				  BGx(TRUE), FGx(FALSE)));
	NGLE_QUICK_SET_DST_BM_ACCESS(
	    BA(	IndexedDcd, Otc32, OtsIndirect, AddrLong,
	    BAJustPoint(0), BINapp0I, BAIndexBase(0)));
	HYPER_SET_LENXY_START_FAST_RECFILL(packedLenXY.all);

	/* Write zeroes to buffer 1 */
	GET_FIFO_SLOTS(nFreeFifoSlots, 2);
	NGLE_QUICK_SET_DST_BM_ACCESS(
	    BA(	IndexedDcd, Otc32, OtsIndirect, AddrLong,
		    BAJustPoint(0), BINapp1I, BAIndexBase(0)));
	HYPER_SET_LENXY_START_FAST_RECFILL(packedLenXY.all);
	break;

    /* There is no default */
    }   /* Device-specific image buffer clear */

    NGLE_UNLOCK(pScreenPriv);

} /* ngleDepth8_ClearImagePlanes() */



/******************************************************************************
 *
 * NGLE DDX Utility Procedure:		ngleDepth24_ClearImagePlanes
 *
 * Description:
 *
 *	This routine clears all 24 image planes to zeroes.
 *
 * Assumptions:
 *	Assumptions fast-locking has been initialized.
 *	Does not assume a lock is in effect.
 *
 ******************************************************************************/

void ngleDepth24_ClearImagePlanes(
    NgleScreenPrivPtr       pScreenPriv)
{
    NgleHdwPtr              pDregs;
    NGLE_MFGP_REGISTER_TYPE packedLenXY;
    NGLE_MFGP_REGISTER_TYPE packedDstXY;
    Int32		    nFreeFifoSlots = 0;

    pDregs 	= (NgleHdwPtr) pScreenPriv->pDregs;

    NGLE_LOCK(pScreenPriv);

    /* Hardware setup */
    GET_FIFO_SLOTS(nFreeFifoSlots, 8);
    NGLE_QUICK_SET_DST_BM_ACCESS(
	BA(IndexedDcd, Otc32, OtsIndirect, AddrLong,
	   BAJustPoint(0), BINapp0F8, BAIndexBase(0)));
    NGLE_SET_SCOREBOARD_OVERRIDE(0);
    NGLE_SET_TRANSFERDATA(0xffffffff);   	/* Write foreground color */

    NGLE_REALLY_SET_IMAGE_FG_COLOR(0);		/* load with zero */
    NGLE_REALLY_SET_IMAGE_PLANEMASK(0xffffff);

    PACK_2CARD16(packedDstXY, 0, 0);
    PACK_2CARD16(packedLenXY, pScreenPriv->screenWidth,
		 pScreenPriv->screenHeight);
    NGLE_SET_DSTXY(NGLE_MFGP_REGISTER_TYPE_ASLONG(packedDstXY));

    /* Write zeroes to all 24 planes of image buffer */
    NGLE_QUICK_SET_IMAGE_BITMAP_OP(IBOvals(RopSrc, MaskAddrOffset(0),
	    BitmapExtent32, StaticReg(FALSE),
	    DataDynamic, MaskOtc, BGx(FALSE), FGx(FALSE)));
    SET_LENXY_START_RECFILL(packedLenXY);

    NGLE_UNLOCK(pScreenPriv);

} /* ngleDepth24_ClearImagePlanes */



/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleClearOverlayPlanes()
 *
 * Description:
 *
 *	This routine "clears" the overlay planes to the pased in value.
 *
 * Assumptions:
 *	Assumptions fast-locking has been initialized.
 *	Does not assume a lock is in effect.
 *
 ******************************************************************************/

void ngleClearOverlayPlanes(
    NgleScreenPrivPtr       pScreenPriv,
    Card32                  planeMask,
    Card32                  planeData)
{
    NgleHdwPtr              pDregs;
    NGLE_MFGP_REGISTER_TYPE packedLenXY;
    NGLE_MFGP_REGISTER_TYPE packedDstXY;
    Int32		    nFreeFifoSlots = 0;

    pDregs 	= (NgleHdwPtr) pScreenPriv->pDregs;

    NGLE_LOCK(pScreenPriv);

    /* Hardware setup */
    GET_FIFO_SLOTS(nFreeFifoSlots, 8);
    NGLE_QUICK_SET_DST_BM_ACCESS(
	BA( IndexedDcd, Otc32, OtsIndirect, AddrLong,
	    BAJustPoint(0), BINovly, BAIndexBase(0)));
    NGLE_SET_SCOREBOARD_OVERRIDE(0);
    NGLE_SET_TRANSFERDATA(0xffffffff);	/* Write foreground color */

    NGLE_REALLY_SET_IMAGE_FG_COLOR(planeData);	/* fill with input data value */
    NGLE_REALLY_SET_IMAGE_PLANEMASK(planeMask);

    PACK_2CARD16(packedDstXY, 0, 0);
    PACK_2CARD16(packedLenXY, pScreenPriv->screenWidth,
		 pScreenPriv->screenHeight);
    NGLE_SET_DSTXY(NGLE_MFGP_REGISTER_TYPE_ASLONG(packedDstXY));

    /* Write zeroes to overlay planes. */
    NGLE_QUICK_SET_IMAGE_BITMAP_OP(
	IBOvals(RopSrc, MaskAddrOffset(0),
		BitmapExtent08, StaticReg(FALSE),
		DataDynamic, MaskOtc, BGx(FALSE), FGx(FALSE)));
    SET_LENXY_START_RECFILL(packedLenXY);

    NGLE_UNLOCK(pScreenPriv);

} /* ngleClearOverlayPlanes() */



/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleResetAttrPlanes()
 *
 * Description:
 *
 *	This routine resets the attribute planes to an initial state.
 *
 * Assumptions:
 *	Assumptions fast-locking has been initialized.
 *	Does not assume a lock is in effect.
 *
 ******************************************************************************/

void ngleResetAttrPlanes(
    NgleScreenPrivPtr       pScreenPriv,
    Card32                  controlPlaneReg)
{
    BoxRec		    box;


    box.x1 = 0;
    box.y1 = 0;
    box.x2 = pScreenPriv->screenWidth;
    box.y2 = pScreenPriv->screenHeight;
    nglePolyPaintAttr(pScreenPriv, controlPlaneReg, 1, &box);

} /* ngleResetAttrPlanes() */



/******************************************************************************
 *
 * NGLE DDX Procedure:		nglePolyPaintAttr()
 *
 * Description:
 *
 *	This routine is called by other NGLE DDX driver routines to perform
 *	a series of solid color, rectangle fills of the attribute plane(s).
 *
 * 	Useful comments about attribute painting:
 *
 *	    The Control Plane Register (CPR) is the equivalent of
 *	    foreground and background pixel registers for the attribute
 *	    planes.
 *
 *	    The most significant byte (CFC:  Control Foreground Color)
 *	    determines what's written if the foreground color is written
 *	    to the attribute planes.
 *
 *	    The next most significant byte (CBC:  Control Background
 *	    Color) determines what's written if a zero is written.
 *
 *	    The third byte (CPM:  Control Plane Mask Byte) indicates
 *	    which group of planes is active.
 *
 *	    Control Bitmap Operation (CBO) determines whether foreground
 *	    or background color is transparent or opaque.  Here, we
 *	    choose opaque for foreground and background (the latter is a
 *	    don't-care).
 *
 *          The ctlPlaneReg parameter which is passed into this routine
 *	    represents the value to be loaded into the CPR.  The CPR
 *	    value is device specific, as each device has its own
 *	    particular mapping of bits in the CPR fields to the devices
 *	    attribute planes.  But, the CPR register appears at the same
 *	    address in each of the devices control register space.
 *	    Therefore, each device can use this common routine to paint
 *	    attribute planes.
 *
 ******************************************************************************/

static void nglePolyPaintAttr(
    NgleScreenPrivPtr           pScreenPriv,
    Card32			ctlPlaneReg,
    Int32			nBoxes,
    BoxPtr			pBoxes)
{
    Int32			nFreeFifoSlots = 0;
/*## For now, treat as 32-bit integers so that we don't have to unpack: ##*/
/*##    Int16			*pBox;##*/
    Int32			*pBox;
    NgleHdwPtr			pDregs;
    NGLE_MFGP_REGISTER_TYPE	packedDstXY;
    NGLE_MFGP_REGISTER_TYPE	packedLenXY;


    /* Return early if there's nothing to do: */
    if (nBoxes <= 0)
	return;

    pDregs = pScreenPriv->pDregs; 
    NGLE_LOCK(pScreenPriv);


    /*
     **************************************************************************
     **
     ** Paint the Boxes in the Attribute Planes:
     **
     **************************************************************************
     */

    GET_FIFO_SLOTS(nFreeFifoSlots, 4);
    NGLE_QUICK_SET_DST_BM_ACCESS(BA(IndexedDcd, Otc32, OtsIndirect,
					AddrLong, BAJustPoint(0),
					BINattr, BAIndexBase(0)));
    NGLE_QUICK_SET_CTL_PLN_REG(ctlPlaneReg);
    NGLE_SET_TRANSFERDATA(0xffffffff);

    /* Loop on boxes: */
    pBox = (Int32 *) pBoxes;
    NGLE_QUICK_SET_IMAGE_BITMAP_OP(IBOvals(RopSrc, MaskAddrOffset(0),
					BitmapExtent08, StaticReg(TRUE),
					DataDynamic, MaskOtc,
					BGx(TRUE), FGx(FALSE)));
    do
    {
	packedDstXY.all = *pBox++;
	packedLenXY.all = (*pBox++) - packedDstXY.all;
	GET_FIFO_SLOTS(nFreeFifoSlots, 2);
	NGLE_SET_DSTXY(NGLE_MFGP_REGISTER_TYPE_ASLONG(packedDstXY));
	SET_LENXY_START_RECFILL(packedLenXY);
    } while (--nBoxes > 0);


    /*
     **************************************************************************
     **
     ** In order to work around an ELK hardware problem (Buffy doesn't
     ** always flush it's buffers when writing to the attribute
     ** planes), at least 4 pixels must be written to the attribute
     ** planes starting at (X == 1280) and (Y != to the last Y written
     ** by BIF):
     **
     **************************************************************************
    */

    if (pScreenPriv->deviceID == S9000_ID_A1659A)   /* ELK_DEVICE_ID */
    {
	/*## NOTE: This may cause problems on a 2K-wide device: ##*/
	if (packedLenXY.xy.y > 0)
	{
	    /* It's safe to use scanline zero: */
	    PACK_2CARD16(packedDstXY, 1280, 0);
	}
	else
	{
	    /* Must generate a safe scanline: */
	    if (packedDstXY.xy.y > 0)
	    {
		PACK_2CARD16(packedDstXY, 1280, 0);
	    }
	    else
	    {
		PACK_2CARD16(packedDstXY, 1280, 1);
	    }
	}

	GET_FIFO_SLOTS(nFreeFifoSlots, 2);
	NGLE_SET_DSTXY(NGLE_MFGP_REGISTER_TYPE_ASLONG(packedDstXY));
	PACK_2CARD16(packedLenXY, 4, 1);
	SET_LENXY_START_RECFILL(packedLenXY);
    }   /* ELK Hardware Kludge */


    /**** Finally, set the Control Plane Register back to zero: ****/
    GET_FIFO_SLOTS(nFreeFifoSlots, 1);
    NGLE_QUICK_SET_CTL_PLN_REG(0);

    NGLE_UNLOCK(pScreenPriv);

} /* nglePolyPaintAttr() */
