/* $XConsortium: technique.h,v 1.4 94/04/17 20:18:29 mor Exp $ */

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

#ifndef WORD64

#define BEGIN_TECHNIQUE(_name, _bufDest, _dstParam) \
    _dstParam = (_name *) _bufDest;

#define END_TECHNIQUE(_name, _bufDest, _dstParam) \
    _bufDest += SIZEOF (_name);

#else /* WORD64 */

#define BEGIN_TECHNIQUE(_name, _bufDest, _dstParam) \
{ \
    _name tParam; \
    _dstParam = &tParam;

#define END_TECHNIQUE(_name, _bufDest, _dstParam) \
    memcpy (_bufDest, _dstParam, SIZEOF (_name)); \
    _bufDest += SIZEOF (_name); \
}

#endif /* WORD64 */



#ifndef WORD64

#define STORE_CARD32(_val, _pBuf) \
{ \
    *((CARD32 *) _pBuf) = _val; \
    _pBuf += SIZEOF (CARD32); \
}

#else /* WORD64 */

typedef struct {
    int value   :32;
} Long;

#define STORE_CARD32(_val, _pBuf) \
{ \
    Long _d; \
    _d.value = _val; \
    memcpy (_pBuf, &_d, SIZEOF (CARD32)); \
    _pBuf += SIZEOF (CARD32); \
}

#endif /* WORD64 */


#define _XieRGBToCIEXYZParam         _XieRGBToCIELabParam
#define _XieCIEXYZToRGBParam         _XieCIELabToRGBParam
#define _XieDecodeG32DParam          _XieDecodeG31DParam
#define _XieDecodeG42DParam          _XieDecodeG31DParam
#define _XieDecodeTIFF2Param         _XieDecodeG31DParam
#define _XieDecodeJPEGLosslessParam  _XieDecodeJPEGBaselineParam
