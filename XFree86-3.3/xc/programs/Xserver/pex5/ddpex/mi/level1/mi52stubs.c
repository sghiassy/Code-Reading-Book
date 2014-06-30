/* $XConsortium: mi52stubs.c,v 1.1 94/09/13 22:19:39 dpw Exp $ */

/***********************************************************

Copyright (c) 1994  X Consortium

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

******************************************************************/

#include "X.h"
#include "PEX.h"
#include "pexError.h"
#include "pex_site.h"
#include "pexLookup.h"
#include "ddpex4.h"

ErrorCode
PutZBuffer(prend, x, y, width, height, normalizedValues, numValues, Zbuffer)
    ddRendererStr prend;
    int x, y;
    unsigned int width, height;
    pexSwitch normalizedValues;
    unsigned int numValues;
    ddPointer *Zbuffer;
{
    return Success;
}


ErrorCode
SetStructurePermission(pstr, permission)
    diStructHandle pstr;
    unsigned int permission;
{
    return Success;
}


ErrorCode
CopyPixmapToAlpha(prend, pDrawable)
    ddRendererStr prend;
    DrawablePtr pDrawable;
{
    return Success;
}


ErrorCode
InitMultipass(prend)
    ddRendererStr prend;
{
    return Success;
}


ErrorCode
GetZBuffer(prend, x, y, width, height, normalizedValues, numValues,
	   undefinedValues, pPEXBuffer)
    ddRendererStr prend;
    int x, y;
    unsigned int width, height;
    pexSwitch normalizedValues;
    ddULONG *numValues;
    pexSwitch *undefinedValues;
    ddBuffer *pPEXBuffer;
{
    return Success;
}


ErrorCode
ClearRenderer(prend, clearControl)
    ddRendererStr prend;
    pexBitmask clearControl;
{
    return Success;
}


ErrorCode
NextPassWoutReply(prend, multipass_control)
    ddRendererStr prend;
    unsigned int multipass_control;
{
    return Success;
}


ErrorCode
CopyZBufferToPixmap(prend, pDrawable)
    ddRendererStr prend;
    DrawablePtr pDrawable;
{
    return Success;
}


ErrorCode
CopyPipelineStateToPC(prend, ppc, itemMask)
    ddRendererStr prend;
    ddPCStr *ppc;
    unsigned int itemMask;
{
    return Success;
}


ErrorCode
CopyAlphaToPixmap(prend, pDrawable)
    ddRendererStr prend;
    DrawablePtr pDrawable;
{
    return Success;
}


ErrorCode
NextPass(prend, multipass_control, count)
    ddRendererStr prend;
    unsigned int multipass_control;
    ddLONG *count;
{
    return Success;
}


ErrorCode
FlushRenderer(prend, flushFlag)
    ddRendererStr prend;
    pexSwitch flushFlag;
{
    return Success;
}


ErrorCode
CopyPixmapToZBuffer(prend, pDrawable)
    ddRendererStr prend;
    DrawablePtr pDrawable;
{
    return Success;
}


ErrorCode
SetElementPointerAtPickID(pstr, pickId, offset)
    diStructHandle pstr;
    int pickId;
    int offset;
{
    return Success;
}


ErrorCode
CopyPCToPipelineState(prend, ppc, itemMask)
    ddRendererStr *prend;
    ddPCStr *ppc;
    pexBitmask itemMask;
{
    return Success;
}
