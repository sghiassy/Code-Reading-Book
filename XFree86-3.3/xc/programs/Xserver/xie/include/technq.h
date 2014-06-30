/* $XConsortium: technq.h,v 1.4 94/04/17 20:34:08 rws Exp $ */
/**** module technq.h ****/
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
*******************************************************************************

	technq.h: contains technique definitions

	Dean Verheiden -- AGE Logic, Inc. April 1993

******************************************************************************/

#ifndef _XIEH_TECHNQ
#define _XIEH_TECHNQ

#include <flostr.h>

/*
 * dixie import client photo technique entry points
 */
extern Bool	CopyICPhotoUnSingle();
extern Bool	CopyICPhotoG31D();
extern Bool	CopyICPhotoG32D();
extern Bool	CopyICPhotoG42D();
extern Bool	CopyICPhotoTIFF2();
extern Bool	CopyICPhotoTIFFPackBits();
extern Bool	PrepICPhotoUnSingle();
extern Bool	PrepICPhotoG31D();
extern Bool	PrepICPhotoG32D();
extern Bool	PrepICPhotoG42D();
extern Bool	PrepICPhotoTIFF2();
extern Bool	PrepICPhotoTIFFPackBits();
#if XIE_FULL
extern Bool	CopyICPhotoUnTriple();
extern Bool	PrepICPhotoUnTriple();
extern Bool	CopyICPhotoJPEGBaseline();
extern Bool	PrepICPhotoJPEGBaseline();
#ifdef	BEYOND_SI
extern Bool	CopyICPhotoJPEGLossless();
extern Bool	PrepICPhotoJPEGLossless();
#endif /* BEYOND_SI */
#endif

/*
 * dixie constrain technique entry points
 */
#if XIE_FULL
extern Bool	CopyPConstrainStandard();
extern Bool	CopyPConstrainClipScale();
extern Bool	PrepPConstrainStandard();
extern Bool	PrepPConstrainClipScale();
#endif

/*
 * dixie convolve technique entry points
 */
#if XIE_FULL
extern Bool     CopyConvolveConstant();
extern Bool     PrepConvolveStandard();
#ifdef	BEYOND_SI
extern Bool     CopyConvolveReplicate();
#endif /* BEYOND_SI */
#endif 

/*
 * dixie dither technique entry points
 */
#if XIE_FULL
extern Bool	CopyPDitherErrorDiffusion();
extern Bool	PrepPDitherErrorDiffusion();
extern Bool	CopyPDitherOrdered();
extern Bool	PrepPDitherOrdered();
#endif

/*
 * dixie geometry technique entry points
 */
extern Bool     CopyGeomNearestNeighbor();
extern Bool     PrepGeomNearestNeighbor();
extern Bool     CopyGeomAntiAlias();
extern Bool     PrepGeomAntiAlias();
#if XIE_FULL
extern Bool     CopyGeomBilinearInterp();
extern Bool     PrepGeomBilinearInterp();
extern Bool     CopyGeomGaussian();
extern Bool     PrepGeomGaussian();
#endif

/*
 * dixie match histogram technique entry points
 */
#if XIE_FULL
extern Bool	CopyPHistogramFlat();
extern Bool	CopyPHistogramGaussian();
extern Bool	CopyPHistogramHyperbolic();
extern Bool	PrepPHistogramFlat();
extern Bool	PrepPHistogramGaussian();
extern Bool	PrepPHistogramHyperbolic();
#endif

/*
 * dixie convert to index technique entry points
 */
#if XIE_FULL
extern Bool 	CopyCtoIAllocAll();
extern Bool 	PrepCtoIAllocAll();
#ifdef	BEYOND_SI
extern Bool 	CopyCtoIAllocMatch();
extern Bool 	CopyCtoIAllocRequantize();
#endif /* BEYOND_SI */
#endif

/*
 * dixie export client photo technique entry points
 */
extern Bool	CopyECPhotoUnSingle();
extern Bool	CopyECPhotoG31D();
extern Bool	CopyECPhotoG32D();
extern Bool	CopyECPhotoG42D();
extern Bool	CopyECPhotoTIFF2();
extern Bool	CopyECPhotoTIFFPackBits();

extern Bool	PrepECPhotoUnSingle();
extern Bool	PrepECPhotoG31D();
extern Bool	PrepECPhotoG32D();
extern Bool	PrepECPhotoG42D();
extern Bool	PrepECPhotoTIFF2();
extern Bool	PrepECPhotoTIFFPackBits();
#if XIE_FULL
extern Bool	CopyECPhotoUnTriple();
extern Bool	PrepECPhotoUnTriple();
extern Bool	CopyECPhotoJPEGBaseline();
extern Bool	PrepECPhotoJPEGBaseline();
#ifdef	BEYOND_SI
extern Bool	CopyECPhotoJPEGLossless();
extern Bool	PrepECPhotoJPEGLossless();
#endif /* BEYOND_SI */
#endif

/*
 * dixie convert to and from RBG technique entry points
 */
#if XIE_FULL
extern Bool     CopyPConvertFromRGBCIE();
extern Bool     CopyPConvertFromRGBYCC();
extern Bool     CopyPConvertFromRGBYCbCr();
extern Bool     CopyPConvertToRGBCIE();
extern Bool     CopyPConvertToRGBYCC();
extern Bool     CopyPConvertToRGBYCbCr();
extern Bool     CopyPWhiteAdjustNone();
extern Bool     CopyPWhiteAdjustCIELabShift();
extern Bool     CopyPGamut();

extern Bool     PrepPConvertFromRGBCIE();
extern Bool     PrepPConvertFromRGBYCC();
extern Bool     PrepPConvertFromRGBYCbCr();
extern Bool     PrepPConvertToRGBCIE();
extern Bool     PrepPConvertToRGBYCC();
extern Bool     PrepPConvertToRGBYCbCr();
extern Bool     PrepPWhiteAdjustNone();
extern Bool     PrepPWhiteAdjustCIELabShift();
extern Bool     PrepPGamut();
#endif

/* Global definitions for referencing techniques */

typedef struct _techvec {
  BOOL		NoTech;	      /* If true, this technique has no parameters  */
  BOOL		OptionalTech; /* If true, parameters are optional	    */
  BOOL		FixedTech;    /* If true, parameter size is fixed	    */
  BOOL		pad;
  CARD16	techSize;     /* size of parameters (possibly optional)     */
  CARD16        number;
  xieBoolProc   copyfnc;      /* function to copy parameter from client     */
  xieBoolProc   prepfnc;      /* function to prepare for activation         */
} techVecRec;

/* 
   Standard macro to verify correct technique parameter sizes . . .
   Should work for any technique that has been properly defined 
*/
			 

#define VALIDATE_TECHNIQUE_SIZE(tv, size, isDefault) 			\
	if (isDefault && size || (!isDefault && 			\
	    (tv->FixedTech && 						\
		(!tv->OptionalTech && tv->techSize != size ||		\
	          tv->OptionalTech && size && tv->techSize != size)) ||	\
	    (!tv->FixedTech &&						\
		(!tv->OptionalTech && tv->techSize > size ||		\
		  tv->OptionalTech && size && tv->techSize > size))))	\
	     return(FALSE);


#if defined(_XIEC_TECHNQ)
/*
 *  Technique resource definition
 */

typedef struct _technique {
	CARD8			speed;
	CARD8			nameLength;
	CARD16			techSize;
	CARD8			*name;
	techVecRec		techvec;
} TechRec, *TechPtr;

typedef struct _techgroup {
	xieTypTechniqueGroup	group;
	CARD16			numTechniques;
	CARD16			defaultNumber;
	CARD16			defaultIndex;
	CARD32			groupSize;	/* Size in words */
	TechPtr			tech;
} TechGroupRec, *TechGroupPtr;

typedef struct _techtable {
	CARD16			numGroups;	/* Number of tech groups      */
	CARD16			numDefaults;	/* Number of default groups   */
	CARD16			numTechniques;	/* Total number of techniques */
	CARD16			pad;
	CARD32			tableSize;	/* All tech size (in words)   */
	CARD32			defaultSize;	/* Default size (in words)    */
	TechGroupPtr 		techgroups;
} TechTable;

#define DEFAULT_SPEED		128

#define NO_DEFAULT		0
#define UNINITIALIZED		0

#define TECH_HAS_NO_PARMS	TRUE
#define TECH_HAS_PARMS		FALSE
#define TECH_PARMS_OPTIONAL	TRUE
#define TECH_PARMS_REQUIRED	FALSE
#define TECH_FIXED_SIZE		TRUE
#define TECH_VARIABLE_SIZE	FALSE

/* 
 * Initialize nameLength to 0 and compute at runtime. Some compilers do not
 * support the use of sizeof(static string) at compile time
 */

static Bool NoParamCheck();
static Bool NoTechYet(); 

#if XIE_FULL
/* Array of techniques for coloralloc */
TechRec		Tcoloralloc[] = { 
	{ 	DEFAULT_SPEED,	
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"ALLOC-ALL",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
			sz_xieTecColorAllocAll / 4,
			xieValColorAllocAll,
			CopyCtoIAllocAll,
			PrepCtoIAllocAll
                }
	}
#ifdef	BEYOND_SI
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"MATCH",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
			sz_xieTecColorAllocMatch / 4,
			xieValColorAllocMatch,
			CopyCtoIAllocMatch,
			NoTechYet
		}
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"REQUANTIZE",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecColorAllocRequantize / 4,
                        xieValColorAllocRequantize,
			CopyCtoIAllocRequantize,
			NoTechYet
                }
	}
#endif /* BEYOND_SI */
};
#endif

#if XIE_FULL
/* Array of techniques for constrain */
TechRec		Tconstrain[] = { 
	{ 	DEFAULT_SPEED,	
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"CLIP-SCALE",
		{
			TECH_HAS_NO_PARMS,
			TECH_HAS_NO_PARMS,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecClipScale / 4,
                        xieValConstrainClipScale,
			CopyPConstrainClipScale,
			PrepPConstrainClipScale
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"HARD-CLIP",
		{
			TECH_HAS_NO_PARMS,
			TECH_HAS_NO_PARMS,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecHardClip / 4,
                        xieValConstrainHardClip,
			CopyPConstrainStandard,
			PrepPConstrainStandard
                }
	}
};
#endif

#if XIE_FULL
/* Array of techniques for conversion from RGB to another colorspace */
TechRec		Tconvertfromrgb[] = { 
	{ 	DEFAULT_SPEED,	
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"CIELAB",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_VARIABLE_SIZE,
			UNINITIALIZED,	
                        sz_xieTecRGBToCIELab / 4,
                        xieValRGBToCIELab,
                        CopyPConvertFromRGBCIE, 
                        PrepPConvertFromRGBCIE
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"CIEXYZ",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_VARIABLE_SIZE,
			UNINITIALIZED,	
                        sz_xieTecRGBToCIEXYZ / 4,
                        xieValRGBToCIEXYZ,
                        CopyPConvertFromRGBCIE, 
                        PrepPConvertFromRGBCIE
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"YCbCr",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecRGBToYCbCr / 4,
                        xieValRGBToYCbCr,
                        CopyPConvertFromRGBYCbCr, 
                        PrepPConvertFromRGBYCbCr
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"YCC",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecRGBToYCC / 4,
                        xieValRGBToYCC,
                        CopyPConvertFromRGBYCC, 
                        PrepPConvertFromRGBYCC
                }
	}
};
#endif

#if XIE_FULL
/* Array of techniques for converting to RGB from another colorspace */
TechRec		Tconverttorgb[] = { 
	{ 	DEFAULT_SPEED,	
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"CIELAB",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_VARIABLE_SIZE,
			UNINITIALIZED,	
                        sz_xieTecCIELabToRGB / 4,
                        xieValCIELabToRGB,
                        CopyPConvertToRGBCIE, 
                        PrepPConvertToRGBCIE
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"CIEXYZ",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_VARIABLE_SIZE,
			UNINITIALIZED,	
                        sz_xieTecCIEXYZToRGB / 4,
                        xieValCIEXYZToRGB,
                        CopyPConvertToRGBCIE, 
                        PrepPConvertToRGBCIE
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"YCbCr",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecYCbCrToRGB / 4,
                        xieValYCbCrToRGB,
                        CopyPConvertToRGBYCbCr, 
			PrepPConvertToRGBYCbCr
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"YCC",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecYCCToRGB / 4,
                        xieValYCCToRGB,
                        CopyPConvertToRGBYCC, 
			PrepPConvertToRGBYCC
                }
	}
};
#endif

#if XIE_FULL
/* Array of techniques for convolve */
TechRec		Tconvolve[] = { 
	{ 	DEFAULT_SPEED,	
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"CONSTANT",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_OPTIONAL,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecConvolveConstant / 4,
                        xieValConvolveConstant,
                        CopyConvolveConstant, 
			PrepConvolveStandard
                }
	}
#ifdef	BEYOND_SI
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"REPLICATE",
		{
			TECH_HAS_NO_PARMS,
			TECH_HAS_NO_PARMS,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecConvolveReplicate / 4,
                        xieValConvolveReplicate,
                        CopyConvolveReplicate, 
			PrepConvolveStandard
                }
	}
#endif /* BEYOND_SI */
};
#endif

/* Array of techniques for decode */
TechRec		Tdecode[] = { 
	{ 	DEFAULT_SPEED,	
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"UNCOMPRESSED-SINGLE",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecDecodeUncompressedSingle / 4,
                        xieValDecodeUncompressedSingle,
			CopyICPhotoUnSingle,
			PrepICPhotoUnSingle
		}
	}
#if XIE_FULL
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"UNCOMPRESSED-TRIPLE",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecDecodeUncompressedTriple / 4,
                        xieValDecodeUncompressedTriple,
			CopyICPhotoUnTriple, 
			PrepICPhotoUnTriple
		}
	}
#endif
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"CCITT-G31D",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecDecodeG31D / 4,
                        xieValDecodeG31D,
			CopyICPhotoG31D, 
			PrepICPhotoG31D 
		}
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"CCITT-G32D",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecDecodeG32D / 4,
                        xieValDecodeG32D,
			CopyICPhotoG32D, 
			PrepICPhotoG32D
		}
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"CCITT-G42D",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecDecodeG42D / 4,
                        xieValDecodeG42D,
			CopyICPhotoG42D, 
			PrepICPhotoG42D
		}
	}
#if XIE_FULL
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"JPEG-BASELINE",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_VARIABLE_SIZE,
			UNINITIALIZED,	
                        sz_xieTecDecodeJPEGBaseline / 4,
                        xieValDecodeJPEGBaseline,
			CopyICPhotoJPEGBaseline, 
			PrepICPhotoJPEGBaseline
		}
	}
#ifdef	BEYOND_SI
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"JPEG-LOSSLESS",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecDecodeJPEGLossless / 4,
                        xieValDecodeJPEGLossless,
			CopyICPhotoJPEGLossless, 
			PrepICPhotoJPEGLossless
		}
	}
#endif /* BEYOND_SI */
#endif
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"TIFF-2",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecDecodeTIFF2 / 4,
                        xieValDecodeTIFF2,
			CopyICPhotoTIFF2, 
			PrepICPhotoTIFF2
		}
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"TIFF-PACKBITS",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecDecodeTIFFPackBits / 4,
                        xieValDecodeTIFFPackBits,
			CopyICPhotoTIFFPackBits, 
			PrepICPhotoTIFFPackBits
		}
	}
};

#if XIE_FULL
/* Array of techniques for dither */
TechRec		Tdither[] = { 
	{ 	DEFAULT_SPEED,	
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"ERROR-DIFFUSION",
		{
			TECH_HAS_NO_PARMS,
			TECH_HAS_NO_PARMS,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecDitherErrorDiffusion / 4,
                        xieValDitherErrorDiffusion,
                        CopyPDitherErrorDiffusion, 
			PrepPDitherErrorDiffusion
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"ORDERED",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_OPTIONAL,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecDitherOrdered / 4,
                        xieValDitherOrdered,
                        CopyPDitherOrdered, 
			PrepPDitherOrdered
                }
	}
};
#endif

/* Array of techniques for encode */
TechRec		Tencode[] = { 
	{ 	DEFAULT_SPEED,	
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"UNCOMPRESSED-SINGLE",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecEncodeUncompressedSingle / 4,
                        xieValEncodeUncompressedSingle,
                        CopyECPhotoUnSingle, 
			PrepECPhotoUnSingle
                }
	}
#if XIE_FULL
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"UNCOMPRESSED-TRIPLE",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecEncodeUncompressedTriple / 4,
                        xieValEncodeUncompressedTriple,
                        CopyECPhotoUnTriple, 
			PrepECPhotoUnTriple
                }
	}
#endif
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"CCITT-G31D",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecEncodeG31D / 4,
                        xieValEncodeG31D,
			CopyECPhotoG31D, 
			PrepECPhotoG31D 
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"CCITT-G32D",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecEncodeG32D / 4,
                        xieValEncodeG32D,
			CopyECPhotoG32D, 
			PrepECPhotoG32D
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"CCITT-G42D",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecEncodeG42D / 4,
                        xieValEncodeG42D,
			CopyECPhotoG42D, 
			PrepECPhotoG42D
                }
	}
#if XIE_FULL
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"JPEG-BASELINE",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_VARIABLE_SIZE,
			UNINITIALIZED,	
                        sz_xieTecEncodeJPEGBaseline / 4,
                        xieValEncodeJPEGBaseline,
			CopyECPhotoJPEGBaseline, 
			PrepECPhotoJPEGBaseline
                }
	}
#ifdef	BEYOND_SI
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"JPEG-LOSSLESS",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecEncodeJPEGLossless / 4,
                        xieValEncodeJPEGLossless,
			CopyECPhotoJPEGLossless, 
			PrepECPhotoJPEGLossless
                }
	}
#endif /* BEYOND_SI */
#endif
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"TIFF-2",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecEncodeTIFF2 / 4,
                        xieValEncodeTIFF2,
			CopyECPhotoTIFF2, 
			PrepECPhotoTIFF2
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"TIFF-PACKBITS",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecEncodeTIFFPackBits / 4,
                        xieValEncodeTIFFPackBits,
			CopyECPhotoTIFFPackBits, 
			PrepECPhotoTIFFPackBits
                }
	}
};

#if XIE_FULL
/* Array of techniques for gamut */
TechRec		Tgamut[] = { 
	{ 	DEFAULT_SPEED,	
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"NONE",
		{
			TECH_HAS_NO_PARMS,
			TECH_HAS_NO_PARMS,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecGamutNone / 4,
                        xieValGamutNone,
                        CopyPGamut, 
                        PrepPGamut
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"CLIP-RGB",
		{
			TECH_HAS_NO_PARMS,
			TECH_HAS_NO_PARMS,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecGamutClipRGB / 4,
                        xieValGamutClipRGB,
                        CopyPGamut, 
                        PrepPGamut
                }
	}
};
#endif

/* Array of techniques for geometry */
TechRec		Tgeometry[] = { 
	{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"ANTIALIAS",
		{
			TECH_HAS_NO_PARMS,
			TECH_HAS_NO_PARMS,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecGeomAntialias / 4,
                        xieValGeomAntialias,
                        CopyGeomAntiAlias, 
			PrepGeomAntiAlias
                }
	}
#ifdef BEYOND_SI
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"ANTIALIAS-BY-AREA",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecGeomAntialiasByArea / 4,
                        xieValGeomAntialiasByArea,
                        NoParamCheck, 
			NoTechYet
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"ANTIALIAS-BY-LOWPASS",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecGeomAntialiasByLowpass / 4,
                        xieValGeomAntialiasByLPF,
                        NoParamCheck, 
			NoTechYet
                }
	}
#endif
#if XIE_FULL
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"BILINEAR-INTERPOLATION",
		{
			TECH_HAS_NO_PARMS,
			TECH_HAS_NO_PARMS,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecGeomBilinearInterpolation / 4,
                        xieValGeomBilinearInterp,
                        CopyGeomBilinearInterp, 
                        PrepGeomBilinearInterp 
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"GAUSSIAN",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecGeomGaussian / 4,
                        xieValGeomGaussian,
                        CopyGeomGaussian, 
			PrepGeomGaussian
                }
	}
#endif
	,{ 	DEFAULT_SPEED,	
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"NEAREST-NEIGHBOR",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_OPTIONAL,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecGeomNearestNeighbor / 4,
                        xieValGeomNearestNeighbor,
                        CopyGeomNearestNeighbor, 
			PrepGeomNearestNeighbor
                }
	}
};

#if XIE_FULL
/* Array of techniques for histogram */
TechRec		Thistogram[] = { 
	{ 	DEFAULT_SPEED,	
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"FLAT",
		{
			TECH_HAS_NO_PARMS,
			TECH_HAS_NO_PARMS,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecHistogramFlat / 4,
                        xieValHistogramFlat,
                        CopyPHistogramFlat, 
                        PrepPHistogramFlat
                }
	}
	,{ 	DEFAULT_SPEED,	
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"GAUSSIAN",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecHistogramGaussian / 4,
                        xieValHistogramGaussian,
                        CopyPHistogramGaussian, 
                        PrepPHistogramGaussian
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"HYPERBOLIC",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecHistogramHyperbolic / 4,
                        xieValHistogramHyperbolic,
                        CopyPHistogramHyperbolic, 
                        PrepPHistogramHyperbolic
                }
	}
};
#endif

#if XIE_FULL
/* Array of techniques for white adjust */
TechRec		Twhiteadjust[] = { 
	{ 	DEFAULT_SPEED,	
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"NONE",
		{
			TECH_HAS_NO_PARMS,
			TECH_HAS_NO_PARMS,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecWhiteAdjustNone / 4,
                        xieValWhiteAdjustNone,
                        CopyPWhiteAdjustNone, 
                        PrepPWhiteAdjustNone
                }
	}
	,{ 	DEFAULT_SPEED,
		UNINITIALIZED,
		UNINITIALIZED,
		(CARD8 *)"CIELAB-SHIFT",
		{
			TECH_HAS_PARMS,
			TECH_PARMS_REQUIRED,
			TECH_FIXED_SIZE,
			UNINITIALIZED,	
                        sz_xieTecWhiteAdjustCIELabShift / 4,
                        xieValWhiteAdjustCIELabShift,
                        CopyPWhiteAdjustCIELabShift, 
                        PrepPWhiteAdjustCIELabShift
                }
	}
};
#endif

TechGroupRec techArray[] = {
#if XIE_FULL
	{	
		xieValColorAlloc,
		sizeof(Tcoloralloc)/sizeof(TechRec),
		xieValColorAllocAll,
		UNINITIALIZED,
		UNINITIALIZED,
		Tcoloralloc
	}
	,{	
		xieValConstrain,
		sizeof(Tconstrain)/sizeof(TechRec),
		NO_DEFAULT,
		UNINITIALIZED,
		UNINITIALIZED,
		Tconstrain
	}
	,{	
		xieValConvertFromRGB,
		sizeof(Tconvertfromrgb)/sizeof(TechRec),
		NO_DEFAULT,
		UNINITIALIZED,
		UNINITIALIZED,
		Tconvertfromrgb
	}
	,{	
		xieValConvertToRGB,
		sizeof(Tconverttorgb)/sizeof(TechRec),
		NO_DEFAULT,
		UNINITIALIZED,
		UNINITIALIZED,
		Tconverttorgb
	}
	,{	
		xieValConvolve,
		sizeof(Tconvolve)/sizeof(TechRec),
		xieValConvolveConstant,
		UNINITIALIZED,
		UNINITIALIZED,
		Tconvolve
	},
#endif
	 {	
		xieValDecode,
		sizeof(Tdecode)/sizeof(TechRec),
		NO_DEFAULT,
		UNINITIALIZED,
		UNINITIALIZED,
		Tdecode
	}
#if XIE_FULL
	,{	
		xieValDither,
		sizeof(Tdither)/sizeof(TechRec),
		xieValDitherErrorDiffusion,
		UNINITIALIZED,
		UNINITIALIZED,
		Tdither	
	}
#endif
	,{	
		xieValEncode,
		sizeof(Tencode)/sizeof(TechRec),
		NO_DEFAULT,
		UNINITIALIZED,
		UNINITIALIZED,
		Tencode	
	}
#if XIE_FULL
	,{	
		xieValGamut,
		sizeof(Tgamut)/sizeof(TechRec),
		xieValGamutNone,
		UNINITIALIZED,
		UNINITIALIZED,
		Tgamut	
	}
#endif
	,{	
		xieValGeometry,
		sizeof(Tgeometry)/sizeof(TechRec),
		xieValGeomNearestNeighbor,
		UNINITIALIZED,
		UNINITIALIZED,
		Tgeometry	
	}
#if XIE_FULL
	,{	
		xieValHistogram,
		sizeof(Thistogram)/sizeof(TechRec),
		NO_DEFAULT,
		UNINITIALIZED,
		UNINITIALIZED,
		Thistogram	
	}
	,{	
		xieValWhiteAdjust,
		sizeof(Twhiteadjust)/sizeof(TechRec),
		xieValWhiteAdjustNone,
		UNINITIALIZED,
		UNINITIALIZED,
		Twhiteadjust	
	}
#endif
};

TechTable	techTable = {
	sizeof(techArray)/sizeof(TechGroupRec),
	UNINITIALIZED,
	UNINITIALIZED,
	UNINITIALIZED,
	UNINITIALIZED,
	UNINITIALIZED,
	techArray
};

#else	/* if defined(_XIEC_TECHNQ) */

extern	Bool 		technique_init();
extern	Bool 		TechNeedsParams();
extern	CARD16 		TechDefault();
extern	techVecPtr 	FindTechnique();

#endif	/* if defined(_XIEC_TECHNQ) */

#endif /* end _XIEH_TECHNQ */
