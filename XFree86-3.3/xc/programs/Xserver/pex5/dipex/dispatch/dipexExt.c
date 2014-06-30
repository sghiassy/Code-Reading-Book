/* $XConsortium: dipexExt.c,v 5.11 94/04/17 20:36:04 dpw Exp $ */
/* $XFree86: xc/programs/Xserver/PEX5/dipex/dispatch/dipexExt.c,v 3.3.4.1 1997/05/22 14:00:27 dawes Exp $ */

/***********************************************************

Copyright (c) 1989, 1990, 1991  X Consortium

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

Copyright 1989, 1990, 1991 by Sun Microsystems, Inc. 

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Sun Microsystems,
not be used in advertising or publicity pertaining to distribution of 
the software without specific, written prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "PEXproto.h"
#include "misc.h"
#include "os.h"
#include "pixmapstr.h"
#include "extnsionst.h"
#include "dixstruct.h"
#include "resource.h"
#include "pexUtils.h"

#define LOCAL_FLAG
#define _DIPEXEXT_
#include "dipex.h"
#include "pexSwap.h"
#undef _DIPEXEXT_
#undef LOCAL_FLAG

#ifdef DYNAMIC_MODULE
#include "xf86Version.h"
#endif

unsigned long add_pad_of[] = {0, 3, 2, 1};

unsigned int ColourSpecSizes[] = {
    sizeof(CARD32),			    /* PEXIndexedColour */
    3 * sizeof(PEXFLOAT),			    /* PEXRgbFloatColour */
    3 * sizeof(PEXFLOAT),			    /* PEXCieFloatColour */
    3 * sizeof(PEXFLOAT),			    /* PEXHsvFloatColour */
    3 * sizeof(PEXFLOAT),			    /* PEXHlsFloatColour */
    sizeof(CARD32),			    /* PEXRgb8Colour */
    2 * sizeof(CARD32),			    /* PEXRgb16Colour */
};

extern ddpex43rtn ddpexInit();

extern ddBuffer *pPEXBuffer;
extern ddBuffer *diInitPEXBuffer();
extern RequestFunction PEXRequest[];
extern void SwapCARD32();
extern void SwapCARD16();
static int DeletePexClient();

/*
 ******************************************************************************
 *
 *	Function:	PexExtensionInit
 *
 ******************************************************************************
 */
void
PexExtensionInit()
{
    ExtensionEntry *AddExtension();
    ExtensionEntry *PextEntry = 0;
    int ProcPEXDispatch();
    void PEXResetProc();
    extern int	DeleteStructure(), FreeSearchContext(), FreeRenderer(),
		FreePickMeasure(), dipexFreePhigsWks(), FreePipelineContext(),
		FreeNameSet(), FreeLUT(), FreePEXFont(), FreeWksDrawable();
    extern ddpex43rtn OpenPEXFont();
    dipexFont	*pPEXFont;

    PEXStructType   = CreateNewResourceType (DeleteStructure);
    PEXSearchType   = CreateNewResourceType (FreeSearchContext);
    PEXRendType	    = CreateNewResourceType (FreeRenderer);
    PEXPickType	    = CreateNewResourceType (FreePickMeasure);
    PEXWksType	    = CreateNewResourceType (dipexFreePhigsWks);
    PEXPipeType	    = CreateNewResourceType (FreePipelineContext);
    PEXNameType	    = CreateNewResourceType (FreeNameSet);
    PEXLutType	    = CreateNewResourceType (FreeLUT);
    PEXFontType	    = CreateNewResourceType (FreePEXFont);
    PEXContextType  = CreateNewResourceType (DeletePexClient);

    PEXClass	    = CreateNewResourceClass();
    PEXWksDrawableType
		    = CreateNewResourceType (FreeWksDrawable);

    if (!PEXStructType || !PEXSearchType || !PEXRendType || !PEXWksDrawableType
	|| !PEXWksType || !PEXPipeType   || !PEXNameType || !PEXFontType 
	|| !PEXLutType || !PEXPickType   || !PEXClass    || !PEXContextType ) {
	    ErrorF("PEXExtensionInit: Could not create PEX resource types");
	    return;
	}

    /*
	Init buffer for varying-sized replies from ddpex
     */
    pPEXBuffer = diInitPEXBuffer();

    if (!pPEXBuffer) {
	ErrorF("PEXExtensionInit: failed to alloc buffer\n");
	return; }

    /*
     *	Init any device dependent stuff
     */

    if (ddpexInit()) {
	ErrorF("PEXExtensionInit: Couldn't init ddPEX!");
	/* this isn't needed for the SI since ddpexInit won't fail
	   in the SI but maybe some vendors versions might...
	*/
  	diFreePEXBuffer(pPEXBuffer);
	return; 
    }

    /*
     *  Open up the default font
     */
    pPEXFont=(dipexFont *)xalloc((unsigned long)(sizeof(dipexFont)));
    if (!pPEXFont) {
	ErrorF("PEXExtensionInit: Memory error--could not allocate default PEX font");
  	diFreePEXBuffer(pPEXBuffer);
	return; 
    }
	
    pPEXFont->refcnt = 1;
    pPEXFont->ddFont.id = FakeClientID(0);
    
    defaultPEXFont = &(pPEXFont->ddFont);

    if ( OpenPEXFont(	(ddULONG)(strlen(DEFAULT_PEX_FONT_NAME)), 
			(ddUCHAR *)(DEFAULT_PEX_FONT_NAME),
			pPEXFont) != Success) {
	char *errmsg;
	char *static_message =
		    "PEXExtensionInit: Couldn't open default PEX font file ";

        errmsg = (char *) xalloc(strlen(static_message) +
				 strlen(DEFAULT_PEX_FONT_NAME) +
				 2  /* 1 byte for space between strings,
				     * 1 byte for null */
				 );
        if (errmsg) {
          sprintf(errmsg, "%s %s", static_message, DEFAULT_PEX_FONT_NAME);

	  ErrorF(errmsg);
          xfree(errmsg);
        }

	xfree(pPEXFont);
	defaultPEXFont = 0; 
  	diFreePEXBuffer(pPEXBuffer);
	return; 
    }
    
    /*
     * Note that fonts resources are stored with the type (dipexFont *),
     * and they are referenced sometimes in the DD layer as diFontHandle
     * (which is a pointer to a ddFontResource).  Since the first part
     * of the dipexFont structure consists of a ddFontResource, this
     * works.  Even though it is ugly, it's best not to start changing
     * all of the font code at this time (right before a public release),
     * and hopefully, it will get cleaned up for PEX 6.0.
     */

    if (!AddResource(	pPEXFont->ddFont.id, PEXFontType,
			(pointer)(pPEXFont))) {
	ErrorF("PEXExtensionInit: Couldn't add default PEX font resource.");
	xfree(pPEXFont);
	defaultPEXFont = 0; 
  	diFreePEXBuffer(pPEXBuffer);
	return;
    }

    PextEntry = AddExtension( PEX_NAME_STRING, PEXNumberEvents, (PEXMaxError+1),
				    ProcPEXDispatch, ProcPEXDispatch,
				    PEXResetProc, StandardMinorOpcode);
    if (!PextEntry) {
	ErrorF("PEXExtensionInit: AddExtensions failed\n");
	xfree(pPEXFont);
	defaultPEXFont = 0; 
  	diFreePEXBuffer(pPEXBuffer);
	return; 
    }

    PexReqCode = PextEntry->base;
    PexErrorBase = PextEntry->errorBase;
    PexEventBase = PextEntry->eventBase;
    EventSwapVector[PexEventBase + PEXMaxHitsReached] = 
						    SwapPEXMaxHitsReachedEvent;
    
}


/*
 ******************************************************************************
 *
 *	Function:	InitPexClient
 *
 ******************************************************************************
 */
pexContext *
InitPexClient(client)
ClientPtr client;
{
    pexContext *cntxtPtr;
    XID pexId;

    /* Allocate space for the context table */
    cntxtPtr = (pexContext *) xalloc( (unsigned long)(sizeof( pexContext )
						+ sizeof(pexSwap)) );
    cntxtPtr->swap = (pexSwap *)(cntxtPtr+1);	/* this is silly */

    /* Create the resource id */
    pexId = PEXID( client, PEXCONTEXTTABLE );

    /* Add the connection context as a resource */
    if (!AddResource (pexId, PEXContextType, (pointer)cntxtPtr)) {
	unsigned short temp;
	temp = MinorOpcodeOfRequest(client);
	SendErrorToClient(  client, (unsigned) PexReqCode,
			    (temp), (unsigned long)0, (int) (BadAlloc));
	xfree((pointer)cntxtPtr);
	return(0); }

    /* setup the default proc vectors */
    cntxtPtr->pexRequest	    = PEXRequest;
    cntxtPtr->pexSwapReply	    = 0;
    cntxtPtr->pexSwapRequestOC	    = 0;
    cntxtPtr->pexSwapReplyOC	    = 0;
    cntxtPtr->swap->ConvertCARD16   = SwapCARD16;
    cntxtPtr->swap->ConvertCARD32   = SwapCARD32;
    cntxtPtr->swap->ConvertFLOAT    = 0;

    /* save the client ptr */
    cntxtPtr->client = client;

    return( cntxtPtr );

}

/*
 ******************************************************************************
 *
 *	Function:	static DeletePexClient
 *
 *	On Entry:	Client quitting, Pex resources need to be freed
 *
 *	On Exit:	Pex client resources freed.
 *
 *	Notes:
 *
 ******************************************************************************
 */

static int DeletePexClient(value, pexId)
pointer value;
XID pexId;
{
    pexContext *cntxtPtr = (pexContext *)value;

    /* Free space allocated for the client context */
    if (cntxtPtr) xfree( (pointer)cntxtPtr );

}


void PEXResetProc()
{
    extern ddpex43rtn FreePEXFont();
    extern void ddpexReset();
    diFontHandle    temp = defaultPEXFont;

    defaultPEXFont = 0;	/* force free */
    FreeResource(temp->id, RT_NONE);

    ddpexReset();
    diFreePEXBuffer(pPEXBuffer);
}


ErrorCode
PEXGenericResourceRequest (cntxtPtr, strmPtr)
pexContext *cntxtPtr;
pexReq *strmPtr;
{ }

#ifdef DYNAMIC_MODULE
/*
 * Entry point of dynamic loading
 */
extern void (*PexExtensionInitPtr)(void);

int
#ifndef DLSYM_BUG
init_module(server_version)
#else
init_pex5(server_version)
#endif
unsigned long server_version;
{

  PexExtensionInitPtr = PexExtensionInit;
  ErrorF("\tPEX extension module for XFree86%s\n", XF86_VERSION);
  return 1;
}
#endif /* DYNAMIC_MODULE */
