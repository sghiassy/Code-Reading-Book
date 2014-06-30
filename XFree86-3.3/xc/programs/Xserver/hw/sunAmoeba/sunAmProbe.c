/* $XConsortium: sunAmProbe.c,v 1.2 94/04/17 20:32:48 dpw Exp $ */
/*-
 * sunAmProbe.c:
 *	Amoeba implementation of functions to probe and map in the
 *	frame buffer of several supported video cards (CG6, CG3, BW2).
 */

/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the names of Sun or X Consortium
not be used in advertising or publicity pertaining to 
distribution  of  the software  without specific prior 
written permission. Sun and X Consortium make no 
representations about the suitability of this software for 
any purpose. It is provided "as is" without any express or 
implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/

#include    "sun.h"

#include    <pixrect/memreg.h>
#include    "colormap.h"
#include    "colormapst.h"
#include    "resource.h"
#include    <cfb.h>
#include    <sundev/bw2reg.h>

#include    <amoeba.h>
#include    <cmdreg.h>
#include    <stderr.h>
#include    <machdep.h>
#include    <proc.h>
#include    <server/iop/iop.h>
#include    <ampolicy.h>

extern bufptr sunMapAmoebaScreen();
extern int sunAmoebaScreen();

/*
 * Code partly derived from the X11R5 version of sunCG6C.c:
 */

#ifdef FBTYPE_SUNFAST_COLOR

#define CG6_VBASE	0x70000000

#define CG6_IMAGE(fb)	    ((caddr_t)(&(fb)->cpixel))
#define CG6_IMAGEOFF	    ((off_t)0x16000)
#define CG6_GXOFF	    ((off_t)0)

/*-
 *-----------------------------------------------------------------------
 * sunCG6Probe --
 *	Attempt to find and initialize a cg6 framebuffer
 *
 * Results:
 *	TRUE if everything went ok. FALSE if not.
 *
 * Side Effects:
 *	Memory is allocated for the frame buffer and the buffer is mapped.
 *
 *-----------------------------------------------------------------------
 */

Bool
sunCG6CProbe (pScreenInfo, index, fbNum, argc, argv)
    ScreenInfo	  *pScreenInfo;	/* The screenInfo struct */
    int	    	  index;    	/* The index of pScreen in the ScreenInfo */
    int	    	  fbNum;    	/* Index into the sunFbData array */
    int	    	  argc;	    	/* The number of the Server's arguments. */
    char    	  **argv;   	/* The arguments themselves. Don't change! */
{
    IOPFrameBufferInfo	screen;
    pointer		fb;
    int			imagelen;
    int			offset;

    if (sunAmoebaScreen(&screen, "cgsix", 8) == 0)
	return FALSE;
    /* We have a little problem with Amoeba.  The CG6_IMAGEOFF = 0x16000
     * means that the start of the hardware segment is not click-aligned.
     * Therefore we round it up to a multiple of the pagesize and add that
     * extra bit to fb to make it point to the right part of the hardware
     */
    offset = (CG6_IMAGEOFF + CLICKSIZE - 1) & ~(CLICKSIZE - 1);
    imagelen = offset + screen.width * screen.height;
    fb = (pointer) sunMapAmoebaScreen(&screen.fb, imagelen);
    if (fb == NULL) {
	Error("Mapping cg6c: sunMapAmoebaScreen failed");
	return FALSE;
    }
    /* Move the pointer up to where the hardware begins */
    fb += offset - CG6_IMAGEOFF;

    sunFbs[index].fd = -1; /* we never use this but we set it from paranoia */
    sunFbs[index].fb = fb;
    sunFbs[index].info.fb_width = screen.width;
    sunFbs[index].info.fb_height = screen.height;
    sunFbs[index].info.fb_depth = screen.depth;
    sunFbs[index].EnterLeave = NULL;
    return TRUE;
}

#endif /* FBTYPE_SUNFAST_COLOR */

/*
 * Code derived from the X11R5 version of sunCG3C.c:
 */

#define CG3A_HEIGHT      900 
#define CG3A_WIDTH       1152
#define CG3B_HEIGHT	 768
#define CG3B_WIDTH	 1024

typedef struct cg3ac {
#ifdef sparc
	u_char mpixel[128*1024];		/* bit-per-pixel memory */
	u_char epixel[128*1024];		/* enable plane */
#endif
        u_char cpixel[CG3A_HEIGHT][CG3A_WIDTH];   /* byte-per-pixel memory */
} CG3AC, CG3ACRec, *CG3ACPtr;

typedef struct cg3bc {
#ifdef sparc
	u_char mpixel[128*1024];		/* bit-per-pixel memory */
	u_char epixel[128*1024];		/* enable plane */
#endif
        u_char cpixel[CG3B_HEIGHT][CG3B_WIDTH];   /* byte-per-pixel memory */
} CG3BC, CG3BCRec, *CG3BCPtr;

#define CG3AC_IMAGE(fb)      ((caddr_t)((fb)->cpixel))
#define CG3AC_IMAGELEN       (((sizeof (CG3AC) + 4095)/4096)*4096)
#define CG3BC_IMAGE(fb)      ((caddr_t)((fb)->cpixel))
#define CG3BC_IMAGELEN       (((sizeof (CG3BC) + 4095)/4096)*4096)

/*-
 *-----------------------------------------------------------------------
 * sunCG3CProbe --
 *	Attempt to find and initialize a cg3 framebuffer
 *
 * Results:
 *	TRUE if everything went ok. FALSE if not.
 *
 * Side Effects:
 *	Memory is allocated for the frame buffer and the buffer is mapped.
 *
 *-----------------------------------------------------------------------
 */

/*ARGSUSED*/
Bool
sunCG3CProbe (pScreenInfo, index, fbNum, argc, argv)
    ScreenInfo	  *pScreenInfo;	/* The screenInfo struct */
    int	    	  index;    	/* The index of pScreen in the ScreenInfo */
    int	    	  fbNum;    	/* Index into the sunFbData array */
    int	    	  argc;	    	/* The number of the Server's arguments. */
    char    	  **argv;   	/* The arguments themselves. Don't change! */
{
    IOPFrameBufferInfo	screen;
    CG3ACPtr		CG3ACfb;
    CG3BCPtr		CG3BCfb;
    pointer		fb;

    if (sunAmoebaScreen(&screen, "cgthree", 8) == 0)
	return FALSE;
    if (screen.width == CG3A_WIDTH) {
	CG3ACfb = (CG3ACPtr) sunMapAmoebaScreen(&screen.fb, CG3AC_IMAGELEN);
	if (CG3ACfb == NULL)
	    return FALSE;
	fb = (pointer) CG3AC_IMAGE(CG3ACfb);
    }
    else if (screen.width == CG3B_WIDTH) {
	CG3BCfb = (CG3BCPtr) sunMapAmoebaScreen(&screen.fb, CG3BC_IMAGELEN);
	if (CG3BCfb == NULL)
	    return FALSE;
	fb = (pointer) CG3BC_IMAGE(CG3BCfb);
    }
    else {
	Error("Mapping cg3c: unexpected width");
	return FALSE;
    }

    sunFbs[index].fd = -1; /* we never use this but we set it from paranoia */
    sunFbs[index].fb = fb;
    sunFbs[index].info.fb_width = screen.width;
    sunFbs[index].info.fb_height = screen.height;
    sunFbs[index].info.fb_depth = screen.depth;
    sunFbs[index].EnterLeave = NULL;
    return TRUE;
}

/* Code derived from sunMfb.c, 1.3 94/02/01 11:02:34 kaleb: */

typedef struct bw2 {
    u_char      image[BW2_FBSIZE];          /* Pixel buffer */
} BW2, BW2Rec, *BW2Ptr;

Bool
sunBW2Probe(pScreenInfo, index, fbNum, argc, argv)
    ScreenInfo    *pScreenInfo; /* The screenInfo struct */
    int           index;        /* The index of pScreen in the ScreenInfo */
    int           fbNum;        /* Index into the sunFbData array */
    int           argc;         /* The number of the Server's arguments. */
    char          **argv;       /* The arguments themselves. Don't change! */
{
    IOPFrameBufferInfo  screen;
    BW2Ptr              BW2fb;  /* Place the bitmap is mapped in addr space */

    if (sunAmoebaScreen(&screen, "bwtwo", 1) == 0)
        return FALSE;
    BW2fb = (BW2Ptr) sunMapAmoebaScreen(&screen.fb,
                                screen.stride * screen.height / 8);
    if (BW2fb == NULL)
        return FALSE;
    sunFbs[index].fb = (pointer)BW2fb;
    sunFbs[index].info.fb_width = screen.width;
    sunFbs[index].info.fb_height = screen.height;
    sunFbs[index].EnterLeave = NULL;
    return TRUE;
}
