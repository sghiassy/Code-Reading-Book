/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach32/mach32mem.c,v 3.5 1996/12/23 06:38:43 dawes Exp $ */
/*
 * Copyright 1993 ATI
 *
 * Used by XFree86 with permission
 *
 * Modified heavily by Mark_Weaver@brown.edu for XFree86
 */
/* $XConsortium: mach32mem.c /main/4 1996/02/21 17:27:15 kaleb $ */



#include "compiler.h"
#include "mach32.h"

#ifdef NEW_MEM_DETECT

static void SetupRestoreEngine_m();
static unsigned short ReadPixel_m();
static void WritePixel_m();

/*
 * Private definitions
 */
#define SETUP_ENGINE    0
#define RESTORE_ENGINE  1

#define MAX_TEST_PIXELS 4

typedef unsigned short MyPixel;

static struct {
    int memSize;		/* Mem in kb if this test succeeds */
    struct {
	short x, y;		/* One entry for each pixel */
    } pt[MAX_TEST_PIXELS + 1];	/* Terminated by x < 0 */
} testCases[] = {
    /*
     * Given the engine settings used, only a 4M card will have
     * enough memory to back up the 1025th line of the display.
     * Since the pixel coordinates are zero-based, line 1024 will
     * be the first one which is only backed on 4M cards.
     *
     * <Mark_Weaver@brown.edu>:
     * In case memory is being wrapped, (0,0) and (0,1024)
     * to make sure they can each hold a unique value.
     */
    {4096, {{0,0}, {0,1024}, {-1,-1}}},

    /*
     * We know this card has 2M or less. On a 1M card, the first 2M
     * of the card's memory will have even doublewords backed by
     * physical memory and odd doublewords unbacked.
     *
     * Pixels 0 and 1 of a row will be in the zeroth doubleword, while
     * pixels 2 and 3 will be in the first. Check both pixels 2 and 3
     * in case this is a pseudo-1M card (one chip pulled to turn a 2M
     * card into a 1M card).
     *
     * <Mark_Weaver@brown.edu>:
     * I don't have a 1M card, so I'm taking a stab in the dark.
     * Maybe memory wraps every 512 lines, or maybe odd doublewords
     * are aliases of their even doubleword counterparts.  I try
     * everything here.
     */
    {2048, {{0,0}, {0,512}, {2,0}, {3,0}, {-1,-1}}},

    /*
     * This is a either a 1M card or a 512k card. Test pixel 1, since
     * it is an odd word in an even doubleword.
     *
     * NOTE: We have not received 512k cards for testing - this is an
     *       extrapolation of the 1M/2M determination code.
     *
     * <Mark_Weaver@brown.edu>:
     * This is the same idea as the test above.
     */
    {1024, {{0,0}, {0,256}, {1,0}, {-1,-1}}},

    /*
     * We assume it is a 512k card by default, since that is the
     * minimum configuration.
     */
    {512, {{-1,-1}}}
};

#define NUM_TEST_CASES (sizeof(testCases) / sizeof(testCases[0]))

/*
 * Bit patterns which are extremely unlikely to show up when reading
 * from nonexistant memory (which normally shows up as either all
 * bits set or all bits clear).
 */

static MyPixel testColor[MAX_TEST_PIXELS]={0x5aa5, 0x55aa, 0xa55a, 0xca53};


/***************************************************************************
 *
 * int mach32GetMemSize(void);
 *
 * DESCRIPTION:
 *  Determine the amount of video memory installed on the graphics card.
 *  This is done because the 68800-6 contains a bug which causes MISC_OPTIONS
 *  to report 1M rather than the true amount of memory.
 *
 * RETURN VALUE:
 *  Amount of memory in kb
 *
 * GLOBALS CHANGED:
 *  none
 *
 * BASED ON CODE BY:
 *  Robert Wolff
 *
 * CHANGE HISTORY:
 *  Modified for use with XFree86 (David Dawes)
 *  Rewritten for XFree86 (Mark_Weaver@brown.edu)
 *
 * TEST HISTORY:
 *
 ***************************************************************************/

/*
 * I believe that redundancy is the root of all evil in the world.
 * These are few macros to make things below little less redundant.
 * <Mark_Weaver@brown.edu>
 */
#define TestPixel testCases[caseNum].pt[pixelNum]
#define ForEachTestPixel \
    for (pixelNum = 0; TestPixel.x >= 0; ++pixelNum)

int mach32GetMemSize()
{
    MyPixel savedPixel[MAX_TEST_PIXELS];
    int caseNum, pixelNum;
    int success;

    /*
     * Switch into accelerator mode, and initialize the engine to
     * a pitch of 1024 pixels in 16BPP.
     */
    SetupRestoreEngine_m(SETUP_ENGINE);

    for (caseNum = 0; caseNum < NUM_TEST_CASES; ++caseNum) {
	/*
	 * If you're confused, see the #defines above this routine
	 */

	/* Save all the pixel values */
	ForEachTestPixel
	    savedPixel[pixelNum] = ReadPixel_m(TestPixel.x, TestPixel.y);

	/* Write the test patterns */
	ForEachTestPixel
	    WritePixel_m(TestPixel.x, TestPixel.y, testColor[pixelNum]);
	
	/* See if any of the pixels forgot what we wrote */
	success = 1;
	ForEachTestPixel {
	    if (ReadPixel_m(TestPixel.x, TestPixel.y) != testColor[pixelNum])
		success = 0;
	}

	/* Regardless of the outcome, restore all the values */
	ForEachTestPixel
	    WritePixel_m(TestPixel.x, TestPixel.y, savedPixel[pixelNum]);

	if (success) {
	    /*
	     * If the test was successful, we're done.
	     * Restore the engine, and return the appropriate value
	     */
	    SetupRestoreEngine_m(RESTORE_ENGINE);
	    return testCases[caseNum].memSize;
	}
    }

    /*
     * We shouldn't ever get here.  But just in case we do.
     */
    SetupRestoreEngine_m(RESTORE_ENGINE);
    return 512;

}

#undef TestPixel
#undef ForEachTestPixel

/***************************************************************************
 *
 * void SetupRestoreEngine_m(DesiredStatus);
 *
 * int DesiredStatus;   Whether the user wants to set up or restore
 *
 * DESCRIPTION:
 *  Set engine to 1024 pitch 16BPP with 512k of VGA memory,
 *  or restore the engine and boundary status, as selected by the user.
 *
 * GLOBALS CHANGED:
 *  none
 *
 * CALLED BY:
 *  GetTrueMemSize_m()
 *
 * AUTHOR:
 *  Robert Wolff
 *
 * CHANGE HISTORY:
 *
 * TEST HISTORY:
 *
 ***************************************************************************/
static void SetupRestoreEngine_m(DesiredStatus)
int DesiredStatus;
{
    static unsigned short MiscOptions;  /* Contents of MISC_OPTIONS register */
    static unsigned short ExtGeConfig;  /* Contents of EXT_GE_CONFIG register */
    static unsigned short MemBndry;     /* Contents of MEM_BNDRY register */
    unsigned short tmp;


    if (DesiredStatus == SETUP_ENGINE) {
	
	tmp = inw(CLOCK_SEL);
	outw(CLOCK_SEL, tmp | 0x0001);
        /* Passth8514_m(SHOW_ACCEL); */

        /*
         * Set up a 512k VGA boundary so "blue screen" writes that happen
         * when we are in accelerator mode won't show up in the wrong place.
         */
        MemBndry = inw(MEM_BNDRY);     /* Set up shared memory */
        outw(MEM_BNDRY, 0);

        /*
         * Save the contents of the MISC_OPTIONS register, then
         * tell it that we have 4M of video memory. Otherwise,
         * video memory will wrap when it hits the boundary
         * in the MEM_SIZE_ALIAS field.
         */
        MiscOptions = inw(MISC_OPTIONS);
        outw(MISC_OPTIONS, MiscOptions | MEM_SIZE_4M);

        /*
         * Set 16BPP with pitch of 1024. Only set up the drawing
         * engine, and not the CRT, since the results of this test
         * are not intended to be seen.
         */
        ExtGeConfig = inw(R_EXT_GE_CONFIG);
        outw(EXT_GE_CONFIG, PIX_WIDTH_16BPP | ORDER_16BPP_565 | 0x000A);
        outw(GE_PITCH, (1024 >> 3));
        outw(GE_OFFSET_HI, 0);
        outw(GE_OFFSET_LO, 0);
    } else {   /* DesiredStatus == RESTORE_ENGINE */
        /*
         * Restore the memory boundary, MISC_OPTIONS register,
         * and EXT_GE_CONFIG. It is not necessary to reset the
         * drawing engine pitch and offset, because they don't
         * affect what is displayed and they will be set to
         * whatever values are needed when the desired video
         * mode is set.
         */
        outw(EXT_GE_CONFIG, ExtGeConfig);
        outw(MISC_OPTIONS, MiscOptions);
        outw(MEM_BNDRY, MemBndry);

        /*
         * Give the VGA control of the screen.
         */
	tmp = inw(CLOCK_SEL);
	outw(CLOCK_SEL, tmp & ~0x0001);
        /* Passth8514_m(SHOW_VGA); */
    }
    return;

}   /* SetupRestoreEngine_m() */



/***************************************************************************
 *
 * MyPixel ReadPixel_m(XPos, YPos);
 *
 * short XPos;      X coordinate of pixel to read
 * short YPos;      Y coordinate of pixel to read
 *
 * DESCRIPTION:
 *  Read a single pixel from the screen.
 *
 * RETURN VALUE:
 *  Colour of pixel at the desired location.
 *
 * GLOBALS CHANGED:
 *  none
 *
 * CALLED BY:
 *  GetTrueMemSize_m()
 *
 * AUTHOR:
 *  Robert Wolff
 *
 * CHANGE HISTORY:
 *
 * TEST HISTORY:
 *
 ***************************************************************************/

static MyPixel ReadPixel_m(XPos, YPos)
short XPos, YPos;
{
    MyPixel RetVal;

    /*
     * Don't read if the engine is busy.
     */
    WaitIdleEmpty();

    /*
     * Set up the engine to read colour data from the screen.
     */
    WaitQueue(7);
    outw(RD_MASK, 0x0FFFF);
    outw(DP_CONFIG, FG_COLOR_SRC_BLIT | DATA_WIDTH | DRAW | DATA_ORDER);
    outw(CUR_X, XPos);
    outw(CUR_Y, YPos);
    outw(DEST_X_START, XPos);
    outw(DEST_X_END, XPos+1);
    outw(DEST_Y_END, YPos+1);

    /*
     * Wait for the engine to process the orders we just gave it and
     * start asking for data.
     */
    WaitQueue(16);
    while (!(inw(GE_STAT) & DATA_READY));

    RetVal = inw(PIX_TRANS);
    WaitIdleEmpty();
    return RetVal;

}   /* ReadPixel_m() */



/***************************************************************************
 *
 * void WritePixel_m(XPos, YPos, Colour);
 *
 * short XPos;      X coordinate of pixel to read
 * short YPos;      Y coordinate of pixel to read
 * MyPixel Colour;  Colour to paint the pixel
 *
 * DESCRIPTION:
 *  Write a single pixel to the screen.
 *
 * GLOBALS CHANGED:
 *  none
 *
 * CALLED BY:
 *  GetTrueMemSize_m()
 *
 * AUTHOR:
 *  Robert Wolff
 *
 * CHANGE HISTORY:
 *
 * TEST HISTORY:
 *
 ***************************************************************************/

static void WritePixel_m(XPos, YPos, Colour)
short XPos, YPos;
MyPixel Colour;
{
    /*
     * Set up the engine to paint to the screen.
     */
    WaitQueue(9);  
    outw(WRT_MASK, 0x0FFFF);
    outw(DP_CONFIG, FG_COLOR_SRC_FG | DRAW | READ_WRITE);
    outw(ALU_FG_FN, MIX_FN_PAINT);
    outw(FRGD_COLOR, Colour);
    outw(CUR_X, XPos);
    outw(CUR_Y, YPos);
    outw(DEST_X_START, XPos);
    outw(DEST_X_END, XPos+1);
    outw(DEST_Y_END, YPos+1);

    return;

}   /* WritePixel_m() */
#endif /* NEW_MEM_DETECT */
