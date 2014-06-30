/* $XConsortium: xieperf.h,v 1.16 94/04/17 20:39:42 rws Exp $ */

/**** module xieperf.h ****/
/******************************************************************************

Copyright (c) 1993, 1994  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.


				NOTICE
                              
This software is being provided by AGE Logic, Inc. under the
following license.  By obtaining, using and/or copying this software,
you agree that you have read, understood, and will comply with these
terms and conditions:

     Permission to use, copy, modify, distribute and sell this
     software and its documentation for any purpose and without
     fee or royalty and to grant others any or all rights granted
     herein is hereby granted, provided that you agree to comply
     with the following copyright notice and statements, including
     the disclaimer, and that the same appears on all copies and
     derivative works of the software and documentation you make.
     
     "Copyright 1993, 1994 by AGE Logic, Inc."
     
     THIS SOFTWARE IS PROVIDED "AS IS".  AGE LOGIC MAKES NO
     REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.  By way of
     example, but not limitation, AGE LOGIC MAKE NO
     REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS
     FOR ANY PARTICULAR PURPOSE OR THAT THE SOFTWARE DOES NOT
     INFRINGE THIRD-PARTY PROPRIETARY RIGHTS.  AGE LOGIC 
     SHALL BEAR NO LIABILITY FOR ANY USE OF THIS SOFTWARE.  IN NO
     EVENT SHALL EITHER PARTY BE LIABLE FOR ANY INDIRECT,
     INCIDENTAL, SPECIAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOSS
     OF PROFITS, REVENUE, DATA OR USE, INCURRED BY EITHER PARTY OR
     ANY THIRD PARTY, WHETHER IN AN ACTION IN CONTRACT OR TORT OR
     BASED ON A WARRANTY, EVEN IF AGE LOGIC LICENSEES
     HEREUNDER HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
     DAMAGES.
    
     The name of AGE Logic, Inc. may not be used in
     advertising or publicity pertaining to this software without
     specific, written prior permission from AGE Logic.

     Title to this software shall at all times remain with AGE
     Logic, Inc.
*****************************************************************************
  
	xieperf.h -- xieperf header file

	Syd Logan -- AGE Logic, Inc.
  
*****************************************************************************/

#ifndef VMS
#include <X11/Xatom.h>
#include <X11/Xos.h>
#else
#include <decw$include/Xatom.h>
#endif

#ifndef VMS
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#else
#include <decw$include/Xlib.h>
#include <decw$include/Xutil.h>
#endif
#include <X11/Xfuncs.h>
#include <X11/extensions/XIElib.h>

#ifndef NULL
#define NULL 0
#endif

#include <sys/types.h>
#include <sys/stat.h>
#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#else
char *malloc();
char *realloc();
#endif
#include <fcntl.h>


#define WIDTH         600	/* Size of large window to work within  */
#define HEIGHT        600

#define	MONWIDTH      350 
#define	MONHEIGHT     200 

typedef Bool (*InitProc)    (/* XParms xp; Parms p */);
typedef void (*Proc)	    (/* XParms xp; Parms p */);

extern void NullProc	    (/* XParms xp; Parms p */);
extern Bool NullInitProc    (/* XParms xp; Parms p */);

#define VALL	   	 1		/* future use - see x11perf.h */

typedef unsigned char Version;		/* ditto */

#define	ClampInputs	1
#define	ClampOutputs	2

#define	Drawable	1
#define	DrawablePlane 	2

#define NoObscure 	0
#define Obscured	1
#define	Obscuring	2

#define Overlap		1
#define	NoOverlap	2

/* geometry stuff */

#define	GEO_TYPE_CROP		1
#define	GEO_TYPE_SCALE		2
#define	GEO_TYPE_MIRRORX	3
#define	GEO_TYPE_MIRRORY	4
#define	GEO_TYPE_MIRRORXY	5
#define GEO_TYPE_ROTATE		6
#define	GEO_TYPE_DEFAULT	7
#define GEO_TYPE_SCALEDROTATE   8

/* for the tests which can have ROIs and control planes, these are the  
   options. Cannot support ROIs and Control Planes at same time, so these
   are mutually exclusive choices */ 

#define DomainNone      0
#define DomainROI       1
#define DomainCtlPlane  2

/* capabilities masks */

/* the low 8 bits of the short are for test requirements. Bit positions not
   defined below are reserved for future expansion */

#define	CAPA_EVENT	( 1 << 0 )	
#define	CAPA_ERROR	( 1 << 1 )	

#define	CAPA_MASK	0x00ff

#define IsEvent(x)	( x & CAPA_EVENT ? 1 : 0 ) 
#define IsError(x)	( x & CAPA_ERROR ? 1 : 0 ) 

#define IsFaxImage( x ) ( x == xieValDecodeG42D   	||		\
			  x == xieValDecodeG32D   	||		\
			  x == xieValDecodeTIFFPackBits ||		\
			  x == xieValDecodeTIFF2   	||		\
			  x == xieValDecodeG31D ) 

#define IsColorVisual( visclass ) ( visclass == StaticColor || visclass == \
	PseudoColor || visclass == DirectColor || visclass == TrueColor ? 1 : 0 )

#define IsTrueColorOrDirectColor( visclass ) ( visclass == TrueColor || \
	visclass == DirectColor )

#define IsGrayVisual( visclass ) ( !IsColorVisual( visclass ) )

#define IsStaticVisual( visclass ) ( visclass == StaticColor || visclass == TrueColor\
	 || visclass == StaticGray )

#define IsDynamicVisual( visclass ) ( !IsStaticVisual( visclass ) )

/* protocol subset masks */

/* the high 8 bits of the short are for XIE subsets. Again, those bits not
   defined are reserved for future use */

#define SUBSET_MASK	0xff00
#define	SUBSET_FULL	( xieValFull << 8 )
#define SUBSET_DIS	( xieValDIS << 8 )

#ifndef MAX
#define MAX( a, b ) ( a > b ? a : b )
#endif
#ifndef MIN
#define MIN( a, b ) ( a < b ? a : b )
#endif

#define	IsFull( x ) ( x & SUBSET_FULL ? 1 : 0 )
#define IsDIS( x ) ( x & SUBSET_DIS ? 1 : 0 )
#define IsDISAndFull( x ) ( IsFullTest( x ) && IsDISTest( x ) )

/*
 * configuration shared by all tests 
 */
 
/* image configuration - could be nicer, but put info for all
   decode techniques in this one struct */

typedef struct _Image {
    char	*fname;		/* filename */
    int		fsize;	      	/* size in bytes. set in init function */
    int		bandclass;     	/* singleband or tripleband */
    int		width[ 3 ];	/* width of image */
    int         height[ 3 ];    /* height of image */
    int         depth[ 3 ];     /* pixel depth */
    unsigned long levels[ 3 ];	/* 1 << depth */
    int		decode;	     	/* decode method */
    int         fill_order;	
    int         pixel_order;	
    int         pixel_stride[ 3 ];	
    int         scanline_pad[ 3 ];	
    int         left_pad[ 3 ];	
    int		interleave;	
    int		band_order;	
    unsigned int chksum;
    char        *data;       /* image data */
} XIEimage;

/* a file represents an image. 4 files per test are supported */

typedef struct _XIEfile {
    XIEimage	*image1;	
    XIEimage	*image2;	
    XIEimage	*image3;	
    XIEimage	*image4;	
} XIEifile;

/* test parameters */

typedef struct _Parms {
    /* Required fields */
    int  	objects;    /* Number of objects to process in one X call */
    /* Optional fields */
    int		description; /* server requirements flags */
    int         buffer_size; /* size when sending/reading data from xie */
    XIEifile 	finfo;      /* image file info */	
    XPointer	ts;	    /* test specifics */		
} ParmRec, *Parms;

/*
 * test specific configuration. One structure per C source file.
 */

typedef struct _abortParms {
    int 	lutSize; 
    int		lutLevels;
} AbortParms;

typedef struct _encodeParms {
	int	encode;		/* encode technique */
	char	*parms;		/* encode technique structs */
	Bool	exportClient;	/* if True, ECP. Else, EP */
	unsigned char scale;	/* For JPEG q_table scaling */
} EncodeParms;

typedef struct _eventParms {
    int		event;
} EventParms;

typedef struct _rgbParms {
	int			colorspace;
	int			which;
#define RGB_FF 1
#define RGB_IF 2
#define RGB_II 3
	XieMatrix		toMatrix;
	XieMatrix		fromMatrix;
	XieWhiteAdjustTechnique whiteAdjust;
	XieConstant		whitePoint;
	XieGamutTechnique	gamut;
        XieLTriplet		RGBLevels;
	XieConstant 		luma;
	XieConstant		bias;
	XieFloat		scale;
} RGBParms;

typedef struct _errorParms {
    int		error;
} ErrorParms;

typedef struct _awaitParms {
    int		lutSize;
    int		lutLevels;
} AwaitParms;

#define	BandExtract 1
#define BandSelect  2

typedef struct _bandParms {
    XieConstant c1;
    XieConstant c2;
    XieConstant c3;
    int which;		/* BandExtract or BandSelect */
    float bias;
    Atom atom;
    Bool useStdCmap;
} BandParms;

typedef struct _blendParms {
    XieConstant constant;
    XieFloat 	alphaConstant;
    int		bandMask;
    int         useDomain;
    int         numROIs;
} BlendParms;

typedef struct _constrainParms {
    Bool 	photoDest;
    int		constrain;
    int		clamp;
} ConstrainParms;

typedef struct _creatDstryParms {
    int		dummy;
} CreateDestroyParms;

typedef struct _cvtToIndexParms {
    int		dither;
    Bool	useDefaultCmap;
    Bool	flo_notify;
    Bool	addCvtFromIndex;
} CvtToIndexParms;

typedef struct _ditherParms {
    int		dither;
    int		drawable;
    float	threshold;
    int		bandMask;
} DitherParms;

typedef struct _exportClParms {
    int		numROIs;	/* ExportClientROI */
	
    /* following are for ExportClientHistogram with ROIs */

    int			useDomain;
    short		x;
    short		y;
    short		width;
    short		height;
} ExportClParms;

typedef struct _geometryParms {
    int		geoType;
    int		geoHeight;
    int		geoWidth;
    int		geoXOffset;
    int		geoYOffset;
    XieGeometryTechnique geoTech;
    int		geoAngle;
    Bool 	radiometric;
} GeometryParms;

typedef struct _logicalParms {
    XieConstant	logicalConstant;
    unsigned long logicalOp;
    int		logicalBandMask;
    int		useDomain;
    int		numROIs;
} LogicalParms;

typedef struct _importParms {
    int		obscure;
    int		numROIs;
} ImportParms;

typedef struct _importClParms {
    int		numROIs;
} ImportClParms;

typedef struct _pasteUpParms {
    int		overlap;
} PasteUpParms;

typedef struct _redefineParms {
    XieConstant	constant;
    int		bandMask;
    unsigned long op1;
    unsigned long op2;
} RedefineParms;

typedef struct _modifyParms {
    XieConstant	constant;
    int		bandMask;
    unsigned long op;
} ModifyParms;

typedef struct _pointParms {
    Bool	photoDest;
    int		levelsIn;
    int		levelsOut;
    int 	useDomain;
    short	x;
    short	y;
    short	width;
    short	height;
    int		bandMask;
} PointParms;

typedef struct _funnyEncodeParms {
    int			floElements;
    XieOrientation	*fillOrder;
    XieOrientation	*pixelOrder;
    unsigned char 	*pixelStride;	
    unsigned char	*scanlinePad;
    XieOrientation	*bandOrder;
    XieInterleave	*interleave;
    Bool		useMyLevelsPlease;
    XieLTriplet		myBits;
} FunnyEncodeParms;
    
typedef struct _triplePointParms {
    int 	useDomain;
    short	x;
    short	y;
    short	width;
    short	height;
    Atom	atom;
    int		bandMask;
    int 	ditherTech;
    int		threshold;
} TriplePointParms;

typedef struct _unconstrainParms {
    int		constrain;
} UnconstrainParms;

typedef struct _purgeColStParms {
    int		dummy;
} PurgeColStParms;

typedef struct _compareParms {
    XieCompareOp	op;
    XieConstant		constant;
    Bool		combine;
    unsigned int	bandMask;
    int			useDomain;
    short		x;
    short		y;
    short		width;
    short		height;
} CompareParms;

typedef struct _arithmeticParms {
    XieArithmeticOp	op;
    XieConstant		constant;
    unsigned int	bandMask;
    int			useDomain;
    short		x;
    short		y;
    short		width;
    short		height;
    Bool		constrain;
    XieConstrainTechnique constrainTech;	
    float		inLow;			
    float		inHigh;			
} ArithmeticParms;

typedef struct _mathParms {
    XieMathOp		*ops;
    int			nops; 
    unsigned int	bandMask;
    int			useDomain;
    short		x;
    short		y;
    short		width;
    short		height;
    Bool		constrain;
    XieConstrainTechnique constrainTech;
    float		inLow;
    float		inHigh;
} MathParms;

typedef struct _convolveParms {
    Bool		photoDest;
    unsigned int	bandMask;
    int			useDomain;
    short		x;
    short		y;
    short		width;
    short		height;
    XieConvolveTechnique tech;
    XieConstant		constant;
    int			( * kfunc )();
} ConvolveParms;

typedef struct _queryParms {
    int		lutSize;
    int		lutLevels;
    XieTechniqueGroup techGroup;
} QueryParms;

typedef struct _matchHistogramParms {
    XieHistogramShape	shape;
    double		mean;
    double		sigma;
    double		constant;
    Bool		shape_factor;
    int			useDomain;
    short		x;
    short		y;
    short		width;
    short		height;
} MatchHistogramParms;

typedef struct _XParms {
    Display	    *d;
    char	    *displayName;	/* see do_await.c */
    Window	    w;
    Window	    p;
    GC		    fggc;
    GC		    bggc;
    unsigned long   foreground;
    unsigned long   background;
    XVisualInfo     vinfo;
    Bool	    pack;
    Version	    version;
    int		    screenDepth;	/* effective depth of drawables */
} XParmRec, *XParms;

typedef int TestType;

typedef struct _Test {
    char	*option;    /* Name to use in prompt line		    */
    char	*label;     /* Fuller description of test		    */
    InitProc    init;       /* Initialization procedure			    */
    Proc	proc;       /* Timed benchmark procedure		    */
    Proc	passCleanup;/* Cleanup between repetitions of same test     */
    Proc	cleanup;    /* Cleanup after test			    */
    Version     versions;   /* future expansion	    */
    TestType    testType;   /* future expansion     */
    int		clips;      /* Number of obscuring windows to force clipping*/
    ParmRec     parms;      /* Parameters passed to test procedures	    */
} Test;

extern Test test[];

#define ForEachTest(x) for (x = 0; test[x].option != NULL; x++)

XiePhotomap GetXIEPhotomap();
XieRoi GetXIERoi();
XieLut GetXIELut();
Display *GetDisplay();

/*****************************************************************************

For repeatable results, XIEperf should be run using a local connection on a
freshly-started server.  The default configuration runs each test 5 times, in
order to see if each trial takes approximately the same amount of time.
Strange glitches should be examined; if non-repeatable I chalk them up to
daemons and network traffic.  Each trial is run for 5 seconds, in order to
reduce random time differences.  The number of objects processed per second is
displayed to 3 significant digits, but you'll be lucky on most UNIX system if
the numbers are actually consistent to 2 digits.

******************************************************************************/
