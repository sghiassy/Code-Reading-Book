/* $XConsortium: pexAccBuf.c,v 1.1 94/09/08 19:48:15 hersh Exp $ */
/*

Copyright (c) 1994  X Consortium

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


#include "X.h"
#include "Xproto.h"
#include "pexError.h"
#include "PEXproto.h"
#include "dipex.h"
#include "pexLookup.h"

#ifdef min
#undef min
#endif
 
#ifdef max
#undef max
#endif




/*++	PEXAccumulateBuffer
 --*/
ErrorCode
PEXAccumulateBuffer (cntxtPtr, strmPtr)
pexContext              *cntxtPtr;
pexAccumulateBufferReq       *strmPtr;
{
    ErrorCode err = Success;

    CHECK_FP_FORMAT (strmPtr->fpFormat);
    LU_DRAWABLE(strmPtr->drawable, prend->pDrawable);

    err = AccumulateBuffer(prend->pDrawable, strmPtr->src_weight,
	  strmPtr->dst_weight);
    if (err) PEX_ERR_EXIT(err,0,cntxtPtr);
    return( err );

} /* end-PEXAccumulateBuffer() */

/*++	PEXAllocAccumBuffer
 --*/
ErrorCode
PEXAllocAccumBuffer (cntxtPtr, strmPtr)
pexContext              *cntxtPtr;
pexAllocAccumBufferReq       *strmPtr;
{
    ErrorCode err = Success;

    LU_DRAWABLE(strmPtr->drawable, prend->pDrawable);

    err = AllocAccumBuffer(prend->pDrawable );
    if (err) PEX_ERR_EXIT(err,0,cntxtPtr);
    return( err );

} /* end-PEXAllocAccumBuffer() */

/*++	PEXFreeAccumBuffer
 --*/
ErrorCode
PEXFreeAccumBuffer (cntxtPtr, strmPtr)
pexContext              *cntxtPtr;
pexFreeAccumBufferReq       *strmPtr;
{
    ErrorCode err = Success;

    LU_DRAWABLE(strmPtr->drawable, prend->pDrawable);

    err = FreeAccumBuffer(prend->pDrawable );
    if (err) PEX_ERR_EXIT(err,0,cntxtPtr);
    return( err );

} /* end-PEXFreeAccumBuffer() */

/*++	PEXLoadAccumBuffer
 --*/
ErrorCode
PEXLoadAccumBuffer (cntxtPtr, strmPtr)
pexContext              *cntxtPtr;
pexLoadAccumBufferReq       *strmPtr;
{
    ErrorCode err = Success;

    LU_DRAWABLE(strmPtr->drawable, prend->pDrawable);

    err = LoadAccumBuffer(prend->pDrawable );
    if (err) PEX_ERR_EXIT(err,0,cntxtPtr);
    return( err );

} /* end-PEXLoadAccumBuffer() */

/*++	PEXReturnAccumBuffer
 --*/
ErrorCode
PEXReturnAccumBuffer (cntxtPtr, strmPtr)
pexContext              *cntxtPtr;
pexReturnAccumBufferReq       *strmPtr;
{
    ErrorCode err = Success;

    CHECK_FP_FORMAT (strmPtr->fpFormat);
    LU_DRAWABLE(strmPtr->drawable, prend->pDrawable);

    err = ReturnAccumBuffer(prend->pDrawable, strmPtr->scale);
    if (err) PEX_ERR_EXIT(err,0,cntxtPtr);
    return( err );

} /* end-PEXReturnAccumBuffer() */

/*++
 *
 * 	End of File
 --*/
