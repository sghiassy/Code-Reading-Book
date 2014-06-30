/*
 * $XConsortium: swap.h /main/8 1996/11/17 22:26:32 rws $
 *
 * Copyright 1994 Network Computing Devices, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name Network Computing Devices, Inc. not be
 * used in advertising or publicity pertaining to distribution of this
 * software without specific, written prior permission.
 *
 * THIS SOFTWARE IS PROVIDED `AS-IS'.  NETWORK COMPUTING DEVICES, INC.,
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING WITHOUT
 * LIMITATION ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NONINFRINGEMENT.  IN NO EVENT SHALL NETWORK
 * COMPUTING DEVICES, INC., BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING
 * SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES, INCLUDING LOSS OF USE, DATA,
 * OR PROFITS, EVEN IF ADVISED OF THE POSSIBILITY THEREOF, AND REGARDLESS OF
 * WHETHER IN AN ACTION IN CONTRACT, TORT OR NEGLIGENCE, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ifndef _SWAP_H_
#define _SWAP_H_

extern void SwapConnSetup(
#if NeedFunctionPrototypes
    xConnSetup * /*pConnSetup*/,
    xConnSetup * /*pConnSetupT*/
#endif
);

extern void SwapWinRoot(
#if NeedFunctionPrototypes
    xWindowRoot * /*pRoot*/,
    xWindowRoot * /*pRootT*/
#endif
);

extern void SwapVisual(
#if NeedFunctionPrototypes
    xVisualType * /*pVis*/,
    xVisualType * /*pVisT*/
#endif
);

extern void SwapConnectionInfo(
#if NeedFunctionPrototypes
    xConnSetup * /*pConnSetup*/
#endif
);

extern void WriteSConnectionInfo(
#if NeedFunctionPrototypes
    ClientPtr /*pClient*/,
    unsigned long /*size*/,
    char * /*pInfo*/
#endif
);

extern void SwapGetPropertyReply(
#if NeedFunctionPrototypes
    xGetPropertyReply * /*rep*/
#endif
);

extern void SwapInternAtomReply(
#if NeedFunctionPrototypes
    xInternAtomReply * /*rep*/
#endif
);

extern void SwapGetAtomNameReply(
#if NeedFunctionPrototypes
    xGetAtomNameReply * /*rep*/
#endif
);

extern void SwapLookupColorReply(
#if NeedFunctionPrototypes
    xLookupColorReply * /*rep*/
#endif
);

extern void SwapAllocColorReply(
#if NeedFunctionPrototypes
    xAllocColorReply * /*rep*/
#endif
);

extern void SwapAllocNamedColorReply(
#if NeedFunctionPrototypes
    xAllocNamedColorReply * /*rep*/
#endif
);

extern void SwapModmapReply(
#if NeedFunctionPrototypes
    xGetModifierMappingReply * /*rep*/
#endif
);

extern void SwapKeymapReply(
#if NeedFunctionPrototypes
    xGetKeyboardMappingReply * /*rep*/
#endif
);

extern void SwapGetImageReply(
#if NeedFunctionPrototypes
    xGetImageReply * /*rep*/
#endif
);

extern void SwapQueryExtensionReply(
#if NeedFunctionPrototypes
    xQueryExtensionReply * /*rep*/
#endif
);

extern void SwapFont(
#if NeedFunctionPrototypes
    xQueryFontReply * /*pr*/,
    Bool /*native*/
#endif
);

extern void LbxSwapFontInfo(
#if NeedFunctionPrototypes
    xLbxFontInfo * /*pr*/,
    Bool /*compressed*/
#endif
);

extern void SwapLongs(
#if NeedFunctionPrototypes
    CARD32 * /*list*/,
    unsigned long /*count*/
#endif
);

extern void SwapShorts(
#if NeedFunctionPrototypes
    short * /*list*/,
    unsigned long /*count*/
#endif
);

extern void SwapConnClientPrefix(
#if NeedFunctionPrototypes
    xConnClientPrefix * /*pCCP*/
#endif
);

extern void SwapNewClient(
#if NeedFunctionPrototypes
    xLbxNewClientReq * /*r*/
#endif
);

extern void SwapCloseClient(
#if NeedFunctionPrototypes
    xLbxCloseClientReq * /*r*/
#endif
);

extern void SwapModifySequence(
#if NeedFunctionPrototypes
    xLbxModifySequenceReq * /*r*/
#endif
);

extern void SwapIncrementPixel(
#if NeedFunctionPrototypes
    xLbxIncrementPixelReq * /*r*/
#endif
);

extern void SwapGetModifierMapping(
#if NeedFunctionPrototypes
    xLbxGetModifierMappingReq * /*r*/
#endif
);

extern void SwapGetKeyboardMapping(
#if NeedFunctionPrototypes
    xLbxGetKeyboardMappingReq * /*r*/
#endif
);

extern void SwapQueryFont(
#if NeedFunctionPrototypes
    xLbxQueryFontReq * /*r*/
#endif
);

extern void SwapChangeProperty(
#if NeedFunctionPrototypes
    xLbxChangePropertyReq * /*r*/
#endif
);

extern void SwapGetProperty(
#if NeedFunctionPrototypes
    xLbxGetPropertyReq * /*r*/
#endif
);

extern void SwapPutImage(
#if NeedFunctionPrototypes
    xLbxPutImageReq * /*r*/
#endif
);

extern void SwapGetImage(
#if NeedFunctionPrototypes
    xLbxGetImageReq * /*r*/
#endif
);

extern void SwapInvalidateTag(
#if NeedFunctionPrototypes
    xLbxInvalidateTagReq * /*r*/
#endif
);

extern void SwapTagData(
#if NeedFunctionPrototypes
    xLbxTagDataReq * /*r*/
#endif
);

extern void SwapQueryExtension(
#if NeedFunctionPrototypes
    xLbxQueryExtensionReq * /*r*/
#endif
);

extern void SwapLbxConnSetupPrefix(
#if NeedFunctionPrototypes
    xLbxConnSetupPrefix * /*csp*/
#endif
);

extern void SwapConnSetupPrefix(
#if NeedFunctionPrototypes
    xConnSetupPrefix * /*csp*/
#endif
);

#endif				/* _SWAP_H_ */
