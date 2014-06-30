/* $XConsortium: conven.c,v 1.9 94/05/02 14:16:30 mor Exp $ */

/*

Copyright (c) 1993, 1994  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

#include "XIElibint.h"

/*
 * Element Convenience Functions
 */

void
XieFloImportClientLUT (element, data_class, band_order, length, levels)

XiePhotoElement	*element;
XieDataClass 	data_class;
XieOrientation	band_order;
XieLTriplet	length;
XieLevels	levels;

{
    element->elemType 			     = xieElemImportClientLUT;
    element->data.ImportClientLUT.data_class = data_class;
    element->data.ImportClientLUT.band_order = band_order;
    element->data.ImportClientLUT.length[0]  = length[0];
    element->data.ImportClientLUT.length[1]  = length[1];
    element->data.ImportClientLUT.length[2]  = length[2];
    element->data.ImportClientLUT.levels[0]  = levels[0];
    element->data.ImportClientLUT.levels[1]  = levels[1];
    element->data.ImportClientLUT.levels[2]  = levels[2];
}


void
XieFloImportClientPhoto (element, data_class, width, height,
    levels, notify, decode_tech, decode_param)

XiePhotoElement		*element;
XieDataClass 		data_class;
XieLTriplet		width;
XieLTriplet		height;
XieLevels		levels;
Bool			notify;
XieDecodeTechnique	decode_tech;
XiePointer		decode_param;

{
    element->elemType				 = xieElemImportClientPhoto;
    element->data.ImportClientPhoto.data_class   = data_class;
    element->data.ImportClientPhoto.width[0]     = width[0];
    element->data.ImportClientPhoto.width[1]     = width[1];
    element->data.ImportClientPhoto.width[2]     = width[2];
    element->data.ImportClientPhoto.height[0]    = height[0];
    element->data.ImportClientPhoto.height[1]    = height[1];
    element->data.ImportClientPhoto.height[2]    = height[2];
    element->data.ImportClientPhoto.levels[0]    = levels[0];
    element->data.ImportClientPhoto.levels[1]    = levels[1];
    element->data.ImportClientPhoto.levels[2]    = levels[2];
    element->data.ImportClientPhoto.notify       = notify;
    element->data.ImportClientPhoto.decode_tech  = decode_tech;
    element->data.ImportClientPhoto.decode_param = decode_param;
}


void
XieFloImportClientROI (element, rectangles)
	
XiePhotoElement	*element;
unsigned int	rectangles;

{
    element->elemType			     = xieElemImportClientROI;
    element->data.ImportClientROI.rectangles = rectangles;
}


void
XieFloImportDrawable (element, drawable, src_x, src_y,
    width, height, fill, notify)

XiePhotoElement	*element;
Drawable	drawable;
int		src_x;
int		src_y;
unsigned int	width;
unsigned int	height;
unsigned long	fill;
Bool		notify;

{
    element->elemType			  = xieElemImportDrawable;
    element->data.ImportDrawable.drawable = drawable;
    element->data.ImportDrawable.src_x    = src_x;
    element->data.ImportDrawable.src_y    = src_y;
    element->data.ImportDrawable.width    = width;
    element->data.ImportDrawable.height   = height;
    element->data.ImportDrawable.fill     = fill;
    element->data.ImportDrawable.notify   = notify;
}


void
XieFloImportDrawablePlane (element, drawable, src_x, src_y,
    width, height, fill, bit_plane, notify)

XiePhotoElement	*element;
Drawable	drawable;
int		src_x;
int		src_y;
unsigned int	width;
unsigned int	height;
unsigned long	fill;
unsigned long	bit_plane;
Bool		notify;

{
    element->elemType				= xieElemImportDrawablePlane;
    element->data.ImportDrawablePlane.drawable  = drawable;
    element->data.ImportDrawablePlane.src_x     = src_x;
    element->data.ImportDrawablePlane.src_y     = src_y;
    element->data.ImportDrawablePlane.width     = width;
    element->data.ImportDrawablePlane.height    = height;
    element->data.ImportDrawablePlane.fill      = fill;
    element->data.ImportDrawablePlane.bit_plane = bit_plane;
    element->data.ImportDrawablePlane.notify    = notify;
}


void
XieFloImportLUT (element, lut)

XiePhotoElement	*element;
XieLut		lut;

{
    element->elemType		= xieElemImportLUT;
    element->data.ImportLUT.lut = lut;
}


void
XieFloImportPhotomap (element, photomap, notify)

XiePhotoElement	*element;
XiePhotomap	photomap;
Bool		notify;

{
    element->elemType			  = xieElemImportPhotomap;
    element->data.ImportPhotomap.photomap = photomap;
    element->data.ImportPhotomap.notify   = notify;
}


void
XieFloImportROI (element, roi)

XiePhotoElement	*element;
XieRoi		roi;

{
    element->elemType		= xieElemImportROI;
    element->data.ImportROI.roi = roi;
}


void
XieFloArithmetic (element, src1, src2, domain, constant, operator, band_mask)

XiePhotoElement		*element;
XiePhototag		src1;
XiePhototag		src2;
XieProcessDomain	*domain;
XieConstant		constant;
XieArithmeticOp		operator;
unsigned int		band_mask;

{
    element->elemType			     = xieElemArithmetic;
    element->data.Arithmetic.src1            = src1;
    element->data.Arithmetic.src2            = src2;
    element->data.Arithmetic.domain.offset_x = domain->offset_x;
    element->data.Arithmetic.domain.offset_y = domain->offset_y;
    element->data.Arithmetic.domain.phototag = domain->phototag;
    element->data.Arithmetic.constant[0]     = constant[0];
    element->data.Arithmetic.constant[1]     = constant[1];
    element->data.Arithmetic.constant[2]     = constant[2];
    element->data.Arithmetic.operator        = operator;
    element->data.Arithmetic.band_mask       = band_mask;
}


void
XieFloBandCombine (element, src1, src2, src3)

XiePhotoElement	*element;
XiePhototag	src1;
XiePhototag	src2;
XiePhototag	src3;

{
    element->elemType		   = xieElemBandCombine;
    element->data.BandCombine.src1 = src1;
    element->data.BandCombine.src2 = src2;
    element->data.BandCombine.src3 = src3;
}


void
XieFloBandExtract (element, src, levels, bias, coefficients)

XiePhotoElement	*element;
XiePhototag	src;
unsigned int	levels;
double		bias;
XieConstant	coefficients;

{
    element->elemType			      = xieElemBandExtract;
    element->data.BandExtract.src             = src;
    element->data.BandExtract.levels          = levels;
    element->data.BandExtract.bias            = bias;
    element->data.BandExtract.coefficients[0] = coefficients[0];
    element->data.BandExtract.coefficients[1] = coefficients[1];
    element->data.BandExtract.coefficients[2] = coefficients[2];
}


void
XieFloBandSelect (element, src, band_number)

XiePhotoElement	*element;
XiePhototag	src;
unsigned int 	band_number;

{
    element->elemType			     = xieElemBandSelect;
    element->data.BandSelect.src             = src;
    element->data.BandSelect.band_number     = band_number;
}


void
XieFloBlend (element, src1, src2, src_constant,
    alpha, alpha_constant, domain, band_mask)

XiePhotoElement		*element;
XiePhototag		src1;
XiePhototag		src2;
XieConstant		src_constant;
XiePhototag		alpha;
double                  alpha_constant;
XieProcessDomain	*domain;
unsigned int		band_mask;

{
    element->elemType			= xieElemBlend;
    element->data.Blend.src1            = src1;
    element->data.Blend.src2            = src2;
    element->data.Blend.src_constant[0] = src_constant[0];
    element->data.Blend.src_constant[1] = src_constant[1];
    element->data.Blend.src_constant[2] = src_constant[2];
    element->data.Blend.alpha           = alpha;
    element->data.Blend.alpha_constant  = alpha_constant;
    element->data.Blend.domain.offset_x = domain->offset_x;
    element->data.Blend.domain.offset_y = domain->offset_y;
    element->data.Blend.domain.phototag = domain->phototag;
    element->data.Blend.band_mask       = band_mask;
}


void
XieFloCompare (element, src1, src2, domain, constant,
    operator, combine, band_mask)

XiePhotoElement		*element;
XiePhototag		src1;
XiePhototag		src2;
XieProcessDomain	*domain;
XieConstant		constant;
XieCompareOp		operator;
Bool			combine;
unsigned int		band_mask;

{
    element->elemType			  = xieElemCompare;
    element->data.Compare.src1            = src1;
    element->data.Compare.src2            = src2;
    element->data.Compare.domain.offset_x = domain->offset_x;
    element->data.Compare.domain.offset_y = domain->offset_y;
    element->data.Compare.domain.phototag = domain->phototag;
    element->data.Compare.constant[0]     = constant[0];
    element->data.Compare.constant[1]     = constant[1];
    element->data.Compare.constant[2]     = constant[2];
    element->data.Compare.operator        = operator;
    element->data.Compare.combine         = combine;
    element->data.Compare.band_mask       = band_mask;
}


void
XieFloConstrain (element, src, levels, constrain_tech, constrain_param)

XiePhotoElement		*element;
XiePhototag		src;
XieLevels		levels;
XieConstrainTechnique	constrain_tech;
XiePointer		constrain_param;

{
    element->elemType			    = xieElemConstrain;
    element->data.Constrain.src             = src;
    element->data.Constrain.levels[0]       = levels[0];
    element->data.Constrain.levels[1]       = levels[1];
    element->data.Constrain.levels[2]       = levels[2];
    element->data.Constrain.constrain_tech  = constrain_tech;
    element->data.Constrain.constrain_param = constrain_param;
}


void
XieFloConvertFromIndex (element, src, colormap, data_class, precision)

XiePhotoElement	*element;
XiePhototag	src;
Colormap	colormap;
XieDataClass	data_class;
unsigned int	precision;

{
    element->elemType			      = xieElemConvertFromIndex;
    element->data.ConvertFromIndex.src        = src;
    element->data.ConvertFromIndex.colormap   = colormap;
    element->data.ConvertFromIndex.data_class = data_class;
    element->data.ConvertFromIndex.precision  = precision;
}


void
XieFloConvertFromRGB (element, src, color_space, color_param)

XiePhotoElement	*element;
XiePhototag	src;
XieColorspace	color_space;
XiePointer     	color_param;

{
    element->elemType			     = xieElemConvertFromRGB;
    element->data.ConvertFromRGB.src         = src;
    element->data.ConvertFromRGB.color_space = color_space;
    element->data.ConvertFromRGB.color_param = color_param;
}


void
XieFloConvertToIndex (element, src, colormap, color_list, notify,
    color_alloc_tech, color_alloc_param)

XiePhotoElement		*element;
XiePhototag		src;
Colormap		colormap;
XieColorList		color_list;
Bool	    		notify;
XieColorAllocTechnique	color_alloc_tech;
XiePointer		color_alloc_param;

{
    element->elemType				   = xieElemConvertToIndex;
    element->data.ConvertToIndex.src               = src;
    element->data.ConvertToIndex.colormap          = colormap;
    element->data.ConvertToIndex.color_list        = color_list;
    element->data.ConvertToIndex.notify            = notify;
    element->data.ConvertToIndex.color_alloc_tech  = color_alloc_tech;
    element->data.ConvertToIndex.color_alloc_param = color_alloc_param;
}


void
XieFloConvertToRGB (element, src, color_space, color_param)

XiePhotoElement	*element;
XiePhototag	src;
XieColorspace	color_space;
XiePointer	color_param;

{
    element->elemType			   = xieElemConvertToRGB;
    element->data.ConvertToRGB.src         = src;
    element->data.ConvertToRGB.color_space = color_space;
    element->data.ConvertToRGB.color_param = color_param;
}


void
XieFloConvolve (element, src, domain, kernel, kernel_size, band_mask,
    convolve_tech, convolve_param)

XiePhotoElement		*element;
XiePhototag		src;
XieProcessDomain	*domain;
float			*kernel;
int			kernel_size;
unsigned int		band_mask;
XieConvolveTechnique	convolve_tech;
XiePointer		convolve_param;

{
    unsigned size;

    element->elemType			   = xieElemConvolve;
    element->data.Convolve.src             = src;
    element->data.Convolve.domain.offset_x = domain->offset_x;
    element->data.Convolve.domain.offset_y = domain->offset_y;
    element->data.Convolve.domain.phototag = domain->phototag;
    element->data.Convolve.kernel_size     = kernel_size;
    element->data.Convolve.band_mask       = band_mask;
    element->data.Convolve.convolve_tech   = convolve_tech;
    element->data.Convolve.convolve_param  = convolve_param;

    size = kernel_size * kernel_size * 4;
    element->data.Convolve.kernel = (float *) Xmalloc (size);
    memcpy (element->data.Convolve.kernel, kernel, size);
}


void
XieFloDither (element, src, band_mask, levels, dither_tech, dither_param)

XiePhotoElement		*element;
XiePhototag		src;
unsigned int		band_mask;
XieLevels		levels;
XieDitherTechnique	dither_tech;
XiePointer		dither_param;

{
    element->elemType		      = xieElemDither;
    element->data.Dither.src          = src;
    element->data.Dither.band_mask    = band_mask;
    element->data.Dither.levels[0]    = levels[0];
    element->data.Dither.levels[1]    = levels[1];
    element->data.Dither.levels[2]    = levels[2];
    element->data.Dither.dither_tech  = dither_tech;
    element->data.Dither.dither_param = dither_param;
}


void
XieFloGeometry (element, src, width, height, coefficients, constant,
    band_mask, sample_tech, sample_param)

XiePhotoElement		*element;
XiePhototag 		src;
unsigned int		width;
unsigned int		height;
float			coefficients[6];
XieConstant		constant;
unsigned int		band_mask;
XieGeometryTechnique	sample_tech;
XiePointer		sample_param;

{
    element->elemType			   = xieElemGeometry;
    element->data.Geometry.src             = src;
    element->data.Geometry.width           = width;
    element->data.Geometry.height          = height;
    element->data.Geometry.coefficients[0] = coefficients[0];
    element->data.Geometry.coefficients[1] = coefficients[1];
    element->data.Geometry.coefficients[2] = coefficients[2];
    element->data.Geometry.coefficients[3] = coefficients[3];
    element->data.Geometry.coefficients[4] = coefficients[4];
    element->data.Geometry.coefficients[5] = coefficients[5];
    element->data.Geometry.constant[0]     = constant[0];
    element->data.Geometry.constant[1]     = constant[1];
    element->data.Geometry.constant[2]     = constant[2];
    element->data.Geometry.band_mask       = band_mask;
    element->data.Geometry.sample_tech 	   = sample_tech;
    element->data.Geometry.sample_param    = sample_param;
}


void
XieFloLogical (element, src1, src2, domain, constant, operator, band_mask)

XiePhotoElement		*element;
XiePhototag		src1;
XiePhototag		src2;
XieProcessDomain	*domain;
XieConstant		constant;
unsigned long		operator;
unsigned int		band_mask;

{
    element->elemType			  = xieElemLogical;
    element->data.Logical.src1            = src1;
    element->data.Logical.src2            = src2;
    element->data.Logical.domain.offset_x = domain->offset_x;
    element->data.Logical.domain.offset_y = domain->offset_y;
    element->data.Logical.domain.phototag = domain->phototag;
    element->data.Logical.constant[0]     = constant[0];
    element->data.Logical.constant[1]     = constant[1];
    element->data.Logical.constant[2]     = constant[2];
    element->data.Logical.operator        = operator;
    element->data.Logical.band_mask       = band_mask;
}


void
XieFloMatchHistogram (element, src, domain, shape, shape_param)

XiePhotoElement		*element;
XiePhototag		src;
XieProcessDomain	*domain;
XieHistogramShape	shape;
XiePointer		shape_param;

{
    element->elemType				 = xieElemMatchHistogram;
    element->data.MatchHistogram.src             = src;
    element->data.MatchHistogram.domain.offset_x = domain->offset_x;
    element->data.MatchHistogram.domain.offset_y = domain->offset_y;
    element->data.MatchHistogram.domain.phototag = domain->phototag;
    element->data.MatchHistogram.shape           = shape;
    element->data.MatchHistogram.shape_param     = shape_param;
}


void
XieFloMath (element, src, domain, operator, band_mask)

XiePhotoElement		*element;
XiePhototag		src;
XieProcessDomain	*domain;
XieMathOp		operator;
unsigned int		band_mask;

{
    element->elemType		       = xieElemMath;
    element->data.Math.src             = src;
    element->data.Math.domain.offset_x = domain->offset_x;
    element->data.Math.domain.offset_y = domain->offset_y;
    element->data.Math.domain.phototag = domain->phototag;
    element->data.Math.operator        = operator;
    element->data.Math.band_mask       = band_mask;
}


void
XieFloPasteUp (element, width, height, constant, tiles, tile_count)

XiePhotoElement	*element;
unsigned int	width;
unsigned int	height;
XieConstant	constant;
XieTile		*tiles;
unsigned int	tile_count;

{
    unsigned size;

    element->elemType		      = xieElemPasteUp;
    element->data.PasteUp.width       = width;
    element->data.PasteUp.height      = height;
    element->data.PasteUp.constant[0] = constant[0];
    element->data.PasteUp.constant[1] = constant[1];
    element->data.PasteUp.constant[2] = constant[2];
    element->data.PasteUp.tile_count  = tile_count;

    size = tile_count * sizeof (XieTile);
    element->data.PasteUp.tiles = (XieTile *) Xmalloc (size);
    memcpy (element->data.PasteUp.tiles, tiles, size);
}


void
XieFloPoint (element, src, domain, lut, band_mask)

XiePhotoElement		*element;
XiePhototag		src;
XieProcessDomain	*domain;
XiePhototag		lut;
unsigned int		band_mask;

{
    element->elemType                   = xieElemPoint;
    element->data.Point.src             = src;
    element->data.Point.domain.offset_x = domain->offset_x;
    element->data.Point.domain.offset_y = domain->offset_y;
    element->data.Point.domain.phototag = domain->phototag;
    element->data.Point.lut             = lut;
    element->data.Point.band_mask       = band_mask;
}

void
XieFloUnconstrain (element, src)

XiePhotoElement		*element;
XiePhototag		src;

{
    element->elemType			    = xieElemUnconstrain;
    element->data.Constrain.src             = src;
}

void
XieFloExportClientHistogram (element, src, domain, notify)

XiePhotoElement		*element;
XiePhototag		src;
XieProcessDomain	*domain;
XieExportNotify		notify;

{
    element->elemType = xieElemExportClientHistogram;
    element->data.ExportClientHistogram.src             = src;
    element->data.ExportClientHistogram.domain.offset_x = domain->offset_x;
    element->data.ExportClientHistogram.domain.offset_y = domain->offset_y;
    element->data.ExportClientHistogram.domain.phototag = domain->phototag;
    element->data.ExportClientHistogram.notify          = notify;
}


void
XieFloExportClientLUT (element, src, band_order, notify, start, length)

XiePhotoElement	*element;
XiePhototag	src;
XieOrientation	band_order;
XieExportNotify	notify;
XieLTriplet 	start;
XieLTriplet     length;

{
    int i;

    element->elemType                        = xieElemExportClientLUT;
    element->data.ExportClientLUT.src        = src;
    element->data.ExportClientLUT.band_order = band_order;
    element->data.ExportClientLUT.notify     = notify;

    for (i = 0; i < 3; i++)
    {
	element->data.ExportClientLUT.start[i] = start[i];
	element->data.ExportClientLUT.length[i] = length[i];
    }

}


void
XieFloExportClientPhoto (element, src, notify, encode_tech, encode_param)

XiePhotoElement		*element;
XiePhototag		src;
XieExportNotify		notify;
XieEncodeTechnique	encode_tech;
XiePointer		encode_param;

{
    element->elemType                            = xieElemExportClientPhoto;
    element->data.ExportClientPhoto.src          = src;
    element->data.ExportClientPhoto.notify       = notify;
    element->data.ExportClientPhoto.encode_tech  = encode_tech;
    element->data.ExportClientPhoto.encode_param = encode_param;
}


void
XieFloExportClientROI (element, src, notify)

XiePhotoElement	*element;
XiePhototag	src;
XieExportNotify	notify;

{
    element->elemType			 = xieElemExportClientROI;
    element->data.ExportClientROI.src    = src;
    element->data.ExportClientROI.notify = notify;
}


void
XieFloExportDrawable (element, src, drawable, gc, dst_x, dst_y)

XiePhotoElement	*element;
XiePhototag	src;
Drawable	drawable;
GC		gc;
int		dst_x;
int		dst_y;

{
    element->elemType                     = xieElemExportDrawable;
    element->data.ExportDrawable.src      = src;
    element->data.ExportDrawable.drawable = drawable;
    element->data.ExportDrawable.gc       = gc;
    element->data.ExportDrawable.dst_x    = dst_x;
    element->data.ExportDrawable.dst_y    = dst_y;
}


void
XieFloExportDrawablePlane (element, src, drawable, gc, dst_x, dst_y)

XiePhotoElement	*element;
XiePhototag	src;
Drawable	drawable;
GC		gc;
int		dst_x;
int		dst_y;

{
    element->elemType                          = xieElemExportDrawablePlane;
    element->data.ExportDrawablePlane.src      = src;
    element->data.ExportDrawablePlane.drawable = drawable;
    element->data.ExportDrawablePlane.gc       = gc;
    element->data.ExportDrawablePlane.dst_x    = dst_x;
    element->data.ExportDrawablePlane.dst_y    = dst_y;
}


void
XieFloExportLUT (element, src, lut, merge, start)

XiePhotoElement	*element;
XiePhototag	src;
XieLut		lut;
Bool 		merge;
XieLTriplet 	start;

{
    int i;

    for (i = 0; i < 3; i++)
	element->data.ExportLUT.start[i] = start[i];

    element->elemType              = xieElemExportLUT;
    element->data.ExportLUT.merge  = merge;
    element->data.ExportLUT.src    = src;
    element->data.ExportLUT.lut    = lut;
}


void
XieFloExportPhotomap (element, src, photomap, encode_tech, encode_param)

XiePhotoElement		*element;
XiePhototag		src;
XiePhotomap		photomap;
XieEncodeTechnique	encode_tech;
XiePointer		encode_param;

{
    element->elemType                         = xieElemExportPhotomap;
    element->data.ExportPhotomap.src          = src;
    element->data.ExportPhotomap.photomap     = photomap;
    element->data.ExportPhotomap.encode_tech  = encode_tech;
    element->data.ExportPhotomap.encode_param = encode_param;
}


void
XieFloExportROI (element, src, roi)

XiePhotoElement	*element;
XiePhototag	src;
XieRoi		roi;

{
    element->elemType           = xieElemExportROI;
    element->data.ExportROI.src = src;
    element->data.ExportROI.roi = roi;
}



/*
 * Technique Convenience Functions
 */

XieColorAllocAllParam *
XieTecColorAllocAll (fill)

unsigned long	fill;

{
    XieColorAllocAllParam *param = (XieColorAllocAllParam *)
	Xmalloc (sizeof (XieColorAllocAllParam));

    param->fill = fill;

    return (param);
}


XieColorAllocMatchParam *
XieTecColorAllocMatch (match_limit, gray_limit)

double	match_limit;
double	gray_limit;

{
    XieColorAllocMatchParam *param = (XieColorAllocMatchParam *)
	Xmalloc (sizeof (XieColorAllocMatchParam));

    param->match_limit = match_limit;
    param->gray_limit  = gray_limit;

    return (param);
}


XieColorAllocRequantizeParam *
XieTecColorAllocRequantize (max_cells)

unsigned long	max_cells;

{
    XieColorAllocRequantizeParam *param = (XieColorAllocRequantizeParam *)
	Xmalloc (sizeof (XieColorAllocRequantizeParam));

    param->max_cells = max_cells;

    return (param);
}


XieRGBToCIELabParam *
XieTecRGBToCIELab (matrix, white_adjust_tech, white_adjust_param)

XieMatrix		matrix;
XieWhiteAdjustTechnique	white_adjust_tech;
XiePointer		white_adjust_param;

{
    XieRGBToCIELabParam *param = (XieRGBToCIELabParam *)
	Xmalloc (sizeof (XieRGBToCIELabParam));

    memcpy (param->matrix, matrix, sizeof (XieMatrix));
    param->white_adjust_tech  = white_adjust_tech;
    param->white_adjust_param = white_adjust_param;

    return (param);
}


XieRGBToCIEXYZParam *
XieTecRGBToCIEXYZ (matrix, white_adjust_tech, white_adjust_param)

XieMatrix		matrix;
XieWhiteAdjustTechnique	white_adjust_tech;
XiePointer		white_adjust_param;

{
    XieRGBToCIEXYZParam *param = (XieRGBToCIEXYZParam *)
	Xmalloc (sizeof (XieRGBToCIEXYZParam));

    memcpy (param->matrix, matrix, sizeof (XieMatrix));
    param->white_adjust_tech  = white_adjust_tech;
    param->white_adjust_param = white_adjust_param;

    return (param);
}


XieRGBToYCbCrParam *
XieTecRGBToYCbCr (levels, luma_red, luma_green, luma_blue, bias)

XieLevels	levels;
double		luma_red;
double		luma_green;
double		luma_blue;
XieConstant	bias;

{
    XieRGBToYCbCrParam *param = (XieRGBToYCbCrParam *)
	Xmalloc (sizeof (XieRGBToYCbCrParam));

    param->levels[0]  = levels[0];
    param->levels[1]  = levels[1];
    param->levels[2]  = levels[2];
    param->luma_red   = luma_red;
    param->luma_green = luma_green;
    param->luma_blue  = luma_blue;
    param->bias[0]    = bias[0];
    param->bias[1]    = bias[1];
    param->bias[2]    = bias[2];

    return (param);
}


XieRGBToYCCParam *
XieTecRGBToYCC (levels, luma_red, luma_green, luma_blue, scale)

XieLevels	levels;
double		luma_red;
double		luma_green;
double		luma_blue;
double		scale;

{
    XieRGBToYCCParam *param = (XieRGBToYCCParam *)
	Xmalloc (sizeof (XieRGBToYCCParam));

    param->levels[0]  = levels[0];
    param->levels[1]  = levels[1];
    param->levels[2]  = levels[2];
    param->luma_red   = luma_red;
    param->luma_green = luma_green;
    param->luma_blue  = luma_blue;
    param->scale      = scale;

    return (param);
}


XieCIELabToRGBParam *
XieTecCIELabToRGB (matrix, white_adjust_tech, white_adjust_param,
    gamut_tech, gamut_param)

XieMatrix		matrix;
XieWhiteAdjustTechnique	white_adjust_tech;
XiePointer		white_adjust_param;
XieGamutTechnique	gamut_tech;
XiePointer		gamut_param;

{
    XieCIELabToRGBParam *param = (XieCIELabToRGBParam *)
	Xmalloc (sizeof (XieCIELabToRGBParam));

    memcpy (param->matrix, matrix, sizeof (XieMatrix));
    param->white_adjust_tech  = white_adjust_tech;
    param->white_adjust_param = white_adjust_param;
    param->gamut_tech         = gamut_tech;
    param->gamut_param        = gamut_param;

    return (param);
}


XieCIEXYZToRGBParam *
XieTecCIEXYZToRGB (matrix, white_adjust_tech, white_adjust_param,
    gamut_tech, gamut_param)

XieMatrix		matrix;
XieWhiteAdjustTechnique	white_adjust_tech;
XiePointer		white_adjust_param;
XieGamutTechnique	gamut_tech;
XiePointer		gamut_param;

{
    XieCIEXYZToRGBParam *param = (XieCIEXYZToRGBParam *)
	Xmalloc (sizeof (XieCIEXYZToRGBParam));

    memcpy (param->matrix, matrix, sizeof (XieMatrix));
    param->white_adjust_tech  = white_adjust_tech;
    param->white_adjust_param = white_adjust_param;
    param->gamut_tech         = gamut_tech;
    param->gamut_param        = gamut_param;

    return (param);
}


XieYCbCrToRGBParam *
XieTecYCbCrToRGB (levels, luma_red, luma_green, luma_blue, bias,
    gamut_tech, gamut_param)

XieLevels		levels;
double			luma_red;
double			luma_green;
double			luma_blue;
XieConstant		bias;
XieGamutTechnique	gamut_tech;
XiePointer		gamut_param;

{
    XieYCbCrToRGBParam *param = (XieYCbCrToRGBParam *)
	Xmalloc (sizeof (XieYCbCrToRGBParam));

    param->levels[0]   = levels[0];
    param->levels[1]   = levels[1];
    param->levels[2]   = levels[2];
    param->luma_red    = luma_red;
    param->luma_green  = luma_green;
    param->luma_blue   = luma_blue;
    param->bias[0]     = bias[0];
    param->bias[1]     = bias[1];
    param->bias[2]     = bias[2];
    param->gamut_tech  = gamut_tech;
    param->gamut_param = gamut_param;

    return (param);
}


XieYCCToRGBParam *
XieTecYCCToRGB (levels, luma_red, luma_green, luma_blue, scale,
    gamut_tech, gamut_param)

XieLevels		levels;
double			luma_red;
double			luma_green;
double			luma_blue;
double			scale;
XieGamutTechnique	gamut_tech;
XiePointer		gamut_param;

{
    XieYCCToRGBParam *param = (XieYCCToRGBParam *)
	Xmalloc (sizeof (XieYCCToRGBParam));

    param->levels[0]   = levels[0];
    param->levels[1]   = levels[1];
    param->levels[2]   = levels[2];
    param->luma_red    = luma_red;
    param->luma_green  = luma_green;
    param->luma_blue   = luma_blue;
    param->scale       = scale;
    param->gamut_tech  = gamut_tech;
    param->gamut_param = gamut_param;

    return (param);
}


XieClipScaleParam *
XieTecClipScale (in_low, in_high, out_low, out_high)

XieConstant in_low;
XieConstant in_high;
XieLTriplet out_low;
XieLTriplet out_high;

{
    int i;

    XieClipScaleParam *param = (XieClipScaleParam *)
	Xmalloc (sizeof (XieClipScaleParam));

    for (i = 0; i < 3; i++)
    {
    	param->input_low[i]   = in_low[i];
    	param->input_high[i]  = in_high[i];
    	param->output_low[i]  = out_low[i];
    	param->output_high[i] = out_high[i];
    }

    return (param);
}


XieConvolveConstantParam *
XieTecConvolveConstant (constant)

XieConstant	constant;

{
    XieConvolveConstantParam *param = (XieConvolveConstantParam *)
	Xmalloc (sizeof (XieConvolveConstantParam));

    param->constant[0] = constant[0];
    param->constant[1] = constant[1];
    param->constant[2] = constant[2];

    return (param);
}


XieDecodeUncompressedSingleParam *
XieTecDecodeUncompressedSingle (fill_order, pixel_order, pixel_stride,
    left_pad, scanline_pad)

XieOrientation	fill_order;
XieOrientation	pixel_order;
unsigned int	pixel_stride;
unsigned int	left_pad;
unsigned int	scanline_pad;

{
    XieDecodeUncompressedSingleParam *param =
	(XieDecodeUncompressedSingleParam *) Xmalloc (
	sizeof (XieDecodeUncompressedSingleParam));

    param->fill_order   = fill_order;
    param->pixel_order  = pixel_order;
    param->pixel_stride = pixel_stride;
    param->left_pad     = left_pad;
    param->scanline_pad = scanline_pad;

    return (param);
}


XieDecodeUncompressedTripleParam *
XieTecDecodeUncompressedTriple (fill_order, pixel_order, band_order,
    interleave, pixel_stride, left_pad, scanline_pad)

XieOrientation	fill_order;
XieOrientation	pixel_order;
XieOrientation 	band_order;
XieInterleave	interleave;
unsigned char	pixel_stride[3];
unsigned char	left_pad[3];
unsigned char	scanline_pad[3];

{
    XieDecodeUncompressedTripleParam *param =
	(XieDecodeUncompressedTripleParam *) Xmalloc (
	sizeof (XieDecodeUncompressedTripleParam));

    param->left_pad[0]     = left_pad[0];
    param->left_pad[1]     = left_pad[1];
    param->left_pad[2]     = left_pad[2];
    param->fill_order      = fill_order;
    param->pixel_stride[0] = pixel_stride[0];
    param->pixel_stride[1] = pixel_stride[1];
    param->pixel_stride[2] = pixel_stride[2];
    param->pixel_order     = pixel_order;
    param->scanline_pad[0] = scanline_pad[0];
    param->scanline_pad[1] = scanline_pad[1];
    param->scanline_pad[2] = scanline_pad[2];
    param->band_order      = band_order;
    param->interleave      = interleave;

    return (param);
}


XieDecodeG31DParam *
XieTecDecodeG31D (encoded_order, normal, radiometric)

XieOrientation	encoded_order;
Bool		normal;
Bool		radiometric;

{
    XieDecodeG31DParam *param = (XieDecodeG31DParam *)
	Xmalloc (sizeof (XieDecodeG31DParam));

    param->encoded_order = encoded_order;
    param->normal        = normal;
    param->radiometric   = radiometric;

    return (param);
}


XieDecodeG32DParam *
XieTecDecodeG32D (encoded_order, normal, radiometric)

XieOrientation	encoded_order;
Bool		normal;
Bool		radiometric;

{
    XieDecodeG32DParam *param = (XieDecodeG32DParam *)
	Xmalloc (sizeof (XieDecodeG32DParam));

    param->encoded_order = encoded_order;
    param->normal        = normal;
    param->radiometric   = radiometric;

    return (param);
}


XieDecodeG42DParam *
XieTecDecodeG42D (encoded_order, normal, radiometric)

XieOrientation	encoded_order;
Bool		normal;
Bool		radiometric;

{
    XieDecodeG42DParam *param = (XieDecodeG42DParam *)
	Xmalloc (sizeof (XieDecodeG42DParam));

    param->encoded_order = encoded_order;
    param->normal        = normal;
    param->radiometric   = radiometric;

    return (param);
}


XieDecodeTIFF2Param *
XieTecDecodeTIFF2 (encoded_order, normal, radiometric)

XieOrientation	encoded_order;
Bool		normal;
Bool		radiometric;

{
    XieDecodeTIFF2Param *param = (XieDecodeTIFF2Param *)
	Xmalloc (sizeof (XieDecodeTIFF2Param));

    param->encoded_order = encoded_order;
    param->normal        = normal;
    param->radiometric   = radiometric;

    return (param);
}


XieDecodeTIFFPackBitsParam *
XieTecDecodeTIFFPackBits (encoded_order, normal)

XieOrientation	encoded_order;
Bool		normal;

{
    XieDecodeTIFFPackBitsParam *param = (XieDecodeTIFFPackBitsParam *)
	Xmalloc (sizeof (XieDecodeTIFFPackBitsParam));

    param->encoded_order = encoded_order;
    param->normal        = normal;

    return (param);
}


XieDecodeJPEGBaselineParam *
XieTecDecodeJPEGBaseline (interleave, band_order, up_sample)

XieInterleave	interleave;
XieOrientation  band_order;
Bool		up_sample;

{
    XieDecodeJPEGBaselineParam *param = (XieDecodeJPEGBaselineParam *)
	Xmalloc (sizeof (XieDecodeJPEGBaselineParam));

    param->interleave = interleave;
    param->band_order = band_order;
    param->up_sample  = up_sample;

    return (param);
}


XieDecodeJPEGLosslessParam *
XieTecDecodeJPEGLossless (interleave, band_order)

XieInterleave	interleave;
XieOrientation  band_order;

{
    XieDecodeJPEGLosslessParam *param = (XieDecodeJPEGLosslessParam *)
	Xmalloc (sizeof (XieDecodeJPEGLosslessParam));

    param->interleave = interleave;
    param->band_order = band_order;

    return (param);
}


XieDitherOrderedParam *
XieTecDitherOrderedParam (threshold_order)

unsigned int	threshold_order;

{
    XieDitherOrderedParam *param = (XieDitherOrderedParam *)
	Xmalloc (sizeof (XieDitherOrderedParam));

    param->threshold_order = threshold_order;

    return (param);
}


XieEncodeUncompressedSingleParam *
XieTecEncodeUncompressedSingle (fill_order, pixel_order,
    pixel_stride, scanline_pad)

XieOrientation	fill_order;
XieOrientation	pixel_order;
unsigned int	pixel_stride;
unsigned int	scanline_pad;

{
    XieEncodeUncompressedSingleParam *param =
	(XieEncodeUncompressedSingleParam *) Xmalloc (
	sizeof (XieEncodeUncompressedSingleParam));

    param->fill_order   = fill_order;
    param->pixel_order  = pixel_order;
    param->pixel_stride = pixel_stride;
    param->scanline_pad = scanline_pad;

    return (param);
}


XieEncodeUncompressedTripleParam *
XieTecEncodeUncompressedTriple (fill_order, pixel_order, band_order,
    interleave, pixel_stride, scanline_pad)

XieOrientation	fill_order;
XieOrientation	pixel_order;
XieOrientation 	band_order;
XieInterleave	interleave;
unsigned char	pixel_stride[3];
unsigned char	scanline_pad[3];

{
    XieEncodeUncompressedTripleParam *param =
	(XieEncodeUncompressedTripleParam *) Xmalloc (
	sizeof (XieEncodeUncompressedTripleParam));

    param->pixel_stride[0] = pixel_stride[0];
    param->pixel_stride[1] = pixel_stride[1];
    param->pixel_stride[2] = pixel_stride[2];
    param->pixel_order     = pixel_order;
    param->scanline_pad[0] = scanline_pad[0];
    param->scanline_pad[1] = scanline_pad[1];
    param->scanline_pad[2] = scanline_pad[2];
    param->fill_order      = fill_order;
    param->band_order      = band_order;
    param->interleave      = interleave;

    return (param);
}


XieEncodeG31DParam *
XieTecEncodeG31D (align_eol, radiometric, encoded_order)

Bool		align_eol;
Bool		radiometric;
XieOrientation	encoded_order;

{
    XieEncodeG31DParam *param = (XieEncodeG31DParam *)
	Xmalloc (sizeof (XieEncodeG31DParam));

    param->align_eol     = align_eol;
    param->radiometric   = radiometric;
    param->encoded_order = encoded_order;

    return (param);
}


XieEncodeG32DParam *
XieTecEncodeG32D (uncompressed, align_eol, radiometric,
    encoded_order, k_factor)

Bool		uncompressed;
Bool		align_eol;
Bool		radiometric;
XieOrientation	encoded_order;
unsigned long	k_factor;

{
    XieEncodeG32DParam *param = (XieEncodeG32DParam *)
	Xmalloc (sizeof (XieEncodeG32DParam));

    param->uncompressed  = uncompressed;
    param->align_eol     = align_eol;
    param->radiometric   = radiometric;
    param->encoded_order = encoded_order;
    param->k_factor      = k_factor;

    return (param);
}


XieEncodeG42DParam *
XieTecEncodeG42D (uncompressed, radiometric, encoded_order)

Bool		uncompressed;
Bool		radiometric;
XieOrientation	encoded_order;

{
    XieEncodeG42DParam *param = (XieEncodeG42DParam *)
	Xmalloc (sizeof (XieEncodeG42DParam));

    param->uncompressed  = uncompressed;
    param->radiometric   = radiometric;
    param->encoded_order = encoded_order;

    return (param);
}


XieEncodeJPEGBaselineParam *
XieTecEncodeJPEGBaseline (interleave, band_order,
			  horizontal_samples, vertical_samples,
			  q_table, q_size,
			  ac_table, ac_size,
			  dc_table, dc_size)

XieInterleave	interleave;
XieOrientation  band_order;
unsigned char	horizontal_samples[3];
unsigned char	vertical_samples[3];
char		*q_table;
unsigned int	q_size;
char		*ac_table;
unsigned int	ac_size;
char		*dc_table;
unsigned int	dc_size;

{
    XieEncodeJPEGBaselineParam *param = (XieEncodeJPEGBaselineParam *)
	Xmalloc (sizeof (XieEncodeJPEGBaselineParam));
    
    param->interleave            = interleave;
    param->band_order            = band_order;

    param->horizontal_samples[0] = horizontal_samples[0];
    param->horizontal_samples[1] = horizontal_samples[1];
    param->horizontal_samples[2] = horizontal_samples[2];

    param->vertical_samples[0]   = vertical_samples[0];
    param->vertical_samples[1]   = vertical_samples[1];
    param->vertical_samples[2]   = vertical_samples[2];

    param->q_size                = q_size;
    param->ac_size               = ac_size;
    param->dc_size               = dc_size;

    param->q_table               = (char *) Xmalloc (q_size);
    param->ac_table              = (char *) Xmalloc (ac_size);
    param->dc_table              = (char *) Xmalloc (dc_size);

    memcpy (param->q_table, q_table, q_size);
    memcpy (param->ac_table, ac_table, ac_size);
    memcpy (param->dc_table, dc_table, dc_size);

    return (param);
}


XieEncodeJPEGLosslessParam *
XieTecEncodeJPEGLossless (interleave, band_order, predictor, table, table_size)

XieInterleave	interleave;
XieOrientation  band_order;
unsigned char	predictor[3];
char		*table;
unsigned int	table_size;

{
    XieEncodeJPEGLosslessParam *param = (XieEncodeJPEGLosslessParam *)
	Xmalloc (sizeof (XieEncodeJPEGLosslessParam));

    param->interleave   = interleave;
    param->band_order   = band_order;
    param->predictor[0] = predictor[0];
    param->predictor[1] = predictor[1];
    param->predictor[2] = predictor[2];
    param->table_size   = table_size;

    param->table = (char *) Xmalloc (table_size);
    memcpy (param->table, table, table_size);

    return (param);
}


XieEncodeTIFF2Param *
XieTecEncodeTIFF2 (encoded_order, radiometric)

XieOrientation	encoded_order;
Bool		radiometric;

{
    XieEncodeTIFF2Param *param = (XieEncodeTIFF2Param *)
	Xmalloc (sizeof (XieEncodeTIFF2Param));

    param->encoded_order = encoded_order;
    param->radiometric   = radiometric;

    return (param);
}


XieEncodeTIFFPackBitsParam *
XieTecEncodeTIFFPackBits (encoded_order)

XieOrientation	encoded_order;

{
    XieEncodeTIFFPackBitsParam *param = (XieEncodeTIFFPackBitsParam *)
	Xmalloc (sizeof (XieEncodeTIFFPackBitsParam));

    param->encoded_order = encoded_order;

    return (param);
}

XieEncodeServerChoiceParam *
XieTecEncodeServerChoice (preference)

unsigned int	preference;

{
    XieEncodeServerChoiceParam *param = (XieEncodeServerChoiceParam *)
	Xmalloc (sizeof (XieEncodeServerChoiceParam));

    param->preference = preference;

    return (param);
}


XieGeomAntialiasByAreaParam *
XieTecGeomAntialiasByArea (simple)

int simple;

{
    XieGeomAntialiasByAreaParam *param = (XieGeomAntialiasByAreaParam *)
	Xmalloc (sizeof (XieGeomAntialiasByAreaParam));

    param->simple = simple;

    return (param);
}


XieGeomAntialiasByLowpassParam *
XieTecGeomAntialiasByLowpass (kernel_size)

int	kernel_size;

{
    XieGeomAntialiasByLowpassParam *param = (XieGeomAntialiasByLowpassParam *)
	Xmalloc (sizeof (XieGeomAntialiasByLowpassParam));

    param->kernel_size = kernel_size;

    return (param);
}


XieGeomGaussianParam *
XieTecGeomGaussian (sigma, normalize, radius, simple)

double		sigma;
double		normalize;
unsigned int	radius;
Bool		simple;

{
    XieGeomGaussianParam *param = (XieGeomGaussianParam *)
	Xmalloc (sizeof (XieGeomGaussianParam));

    param->sigma     = sigma;
    param->normalize = normalize;
    param->radius    = radius;
    param->simple    = simple;

    return (param);
}


XieGeomNearestNeighborParam *
XieTecGeomNearestNeighbor (modify)

unsigned int	modify;

{
    XieGeomNearestNeighborParam *param = (XieGeomNearestNeighborParam *)
	Xmalloc (sizeof (XieGeomNearestNeighborParam));

    param->modify = modify;

    return (param);
}

XieHistogramGaussianParam *
XieTecHistogramGaussian (mean, sigma)

double	mean;
double	sigma;

{
    XieHistogramGaussianParam *param = (XieHistogramGaussianParam *)
	Xmalloc (sizeof (XieHistogramGaussianParam));

    param->mean    = mean;
    param->sigma   = sigma;

    return (param);
}


XieHistogramHyperbolicParam *
XieTecHistogramHyperbolic (constant, shape_factor)

double	constant;
Bool	shape_factor;

{
    XieHistogramHyperbolicParam *param = (XieHistogramHyperbolicParam *)
	Xmalloc (sizeof (XieHistogramHyperbolicParam));

    param->constant     = constant;
    param->shape_factor = shape_factor;

    return (param);
}


XieWhiteAdjustCIELabShiftParam *
XieTecWhiteAdjustCIELabShift (white_point)

XieConstant	white_point;

{
    XieWhiteAdjustCIELabShiftParam *param = (XieWhiteAdjustCIELabShiftParam *)
	Xmalloc (sizeof (XieWhiteAdjustCIELabShiftParam));

    param->white_point[0] = white_point[0];
    param->white_point[1] = white_point[1];
    param->white_point[2] = white_point[2];

    return (param);
}
