/* $XConsortium: globals.h,v 1.3 94/04/17 20:18:24 mor Exp $ */

/*

Copyright (c) 1993  X Consortium

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

XieExtInfo *_XieExtInfoHeader = NULL;

extern void _XieElemImportClientLUT();
extern void _XieElemImportClientPhoto();
extern void _XieElemImportClientROI();
extern void _XieElemImportDrawable();
extern void _XieElemImportDrawablePlane();
extern void _XieElemImportLUT();
extern void _XieElemImportPhotomap();
extern void _XieElemImportROI();
extern void _XieElemArithmetic();
extern void _XieElemBandCombine();
extern void _XieElemBandExtract();
extern void _XieElemBandSelect();
extern void _XieElemBlend();
extern void _XieElemCompare();
extern void _XieElemConstrain();
extern void _XieElemConvertFromIndex();
extern void _XieElemConvertFromRGB();
extern void _XieElemConvertToIndex();
extern void _XieElemConvertToRGB();
extern void _XieElemConvolve();
extern void _XieElemDither();
extern void _XieElemGeometry();
extern void _XieElemLogical();
extern void _XieElemMatchHistogram();
extern void _XieElemMath();
extern void _XieElemPasteUp();
extern void _XieElemPoint();
extern void _XieElemUnconstrain();
extern void _XieElemExportClientHistogram();
extern void _XieElemExportClientLUT();
extern void _XieElemExportClientPhoto();
extern void _XieElemExportClientROI();
extern void _XieElemExportDrawable();
extern void _XieElemExportDrawablePlane();
extern void _XieElemExportLUT();
extern void _XieElemExportPhotomap();
extern void _XieElemExportROI();

void (*(_XieElemFuncs[]))() =
{
    _XieElemImportClientLUT,
    _XieElemImportClientPhoto,
    _XieElemImportClientROI,
    _XieElemImportDrawable,
    _XieElemImportDrawablePlane,
    _XieElemImportLUT,
    _XieElemImportPhotomap,
    _XieElemImportROI,
    _XieElemArithmetic,
    _XieElemBandCombine,
    _XieElemBandExtract,
    _XieElemBandSelect,
    _XieElemBlend,
    _XieElemCompare,
    _XieElemConstrain,
    _XieElemConvertFromIndex,
    _XieElemConvertFromRGB,
    _XieElemConvertToIndex,
    _XieElemConvertToRGB,
    _XieElemConvolve,
    _XieElemDither,
    _XieElemGeometry,
    _XieElemLogical,
    _XieElemMatchHistogram,
    _XieElemMath,
    _XieElemPasteUp,
    _XieElemPoint,
    _XieElemUnconstrain,
    _XieElemExportClientHistogram,
    _XieElemExportClientLUT,
    _XieElemExportClientPhoto,
    _XieElemExportClientROI,
    _XieElemExportDrawable,
    _XieElemExportDrawablePlane,
    _XieElemExportLUT,
    _XieElemExportPhotomap,
    _XieElemExportROI
};


XieTechFuncRec *_XieTechFuncs[xieValMaxTechGroup];

Bool _XieTechFuncsInitialized = 0;
