/* $XConsortium: session.c,v 1.6 94/04/17 20:33:58 rws Exp $ */
/* $XFree86: xc/programs/Xserver/XIE/dixie/request/session.c,v 3.2.4.1 1997/05/22 14:00:28 dawes Exp $ */
/**** session.c ****/
/****************************************************************************

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

	session.c: Initialization code for the XIE server. 

	Dean Verheiden -- AGE Logic, Inc  March, 1993

*****************************************************************************/

#define _XIEC_SESSION

#define NEED_EVENTS
#define NEED_REPLIES
#include "X.h"			/* Needed for just about anything	*/
#include "Xproto.h"		/* defines protocol-related stuff	*/
#include "misc.h"		/* includes os.h, which type FatalError	*/
#include "dixstruct.h" 		/* this picks up ClientPtr definition	*/
#include "extnsionst.h"		/* defines things like ExtensionEntry	*/
#include <stdio.h>		/* needed if we do any printf's		*/

#include "XIE.h"		
#include "XIEproto.h"		/* Xie protocol specification		*/
#include <corex.h>		/* core X interface routine definitions */ 
#include <tables.h>
#include <macro.h>		/* server internal macros		*/

#include <memory.h>
#include <technq.h>		/* extern def for technique_init	*/

#ifdef DYNAMIC_MODULE
#include "xf86Version.h"
#endif

/* function declarations */
static	int	XieDispatch(),	/* dispatcher for XIE opcodes */
		SXieDispatch(),	/* dispatcher for swapped code */
		DeleteXieClient(); /* Clean up routine */
static void	XieReset();	/* reset the XIE code, eg, on reboot */
static int	DdxInit();

#define REPORT_MEMORY_LEAKS
#ifdef  REPORT_MEMORY_LEAKS
extern int ALLOCS;
extern int STRIPS;
extern int BYTES;
#endif

ExtensionEntry	*extEntry;
RESTYPE		RC_XIE;			/* XIE Resource Class		*/
#if XIE_FULL
RESTYPE		RT_COLORLIST;		/* ColorList resource type	*/
#endif
RESTYPE		RT_LUT;			/* Lookup table resource type	*/
RESTYPE		RT_PHOTOFLO;		/* Photoflo   resource type	*/
RESTYPE		RT_PHOTOMAP;		/* Photomap   resource type	*/
RESTYPE		RT_PHOTOSPACE;		/* Photospace resource type	*/
#if XIE_FULL
RESTYPE		RT_ROI;			/* Region Of Interest type	*/
#endif
RESTYPE		RT_XIE_CLIENT;		/* XIE Type for Shutdown Notice */

struct _client_table {
  XID	   Shutdown_id;
  int 	(**proc_table)();	/* Table of version specific procedures */
  int 	(**sproc_table)();	
  CARD16   minorVersion;
  CARD16   pad;		
} client_table[MAXCLIENTS];

void XieInit()
{
  ExtensionEntry *AddExtension();
  
  /* Initialize XIE Resources */
  RC_XIE	= CreateNewResourceClass();
#if XIE_FULL
  RT_COLORLIST	= RC_XIE | CreateNewResourceType(DeleteColorList);
#endif
  RT_LUT	= RC_XIE | CreateNewResourceType(DeleteLUT);
  RT_PHOTOFLO	= RC_XIE | CreateNewResourceType(DeletePhotoflo);
  RT_PHOTOMAP	= RC_XIE | CreateNewResourceType(DeletePhotomap);
  RT_PHOTOSPACE	= RC_XIE | CreateNewResourceType(DeletePhotospace);
#if XIE_FULL
  RT_ROI	= RC_XIE | CreateNewResourceType(DeleteROI);
#endif
  RT_XIE_CLIENT	= RC_XIE | CreateNewResourceType(DeleteXieClient);
  
  
  extEntry = AddExtension(xieExtName, 		/* extension name   */
			  xieNumEvents, 	/* number of events */
			  xieNumErrors,		/* number of errors */
			  XieDispatch,		/* Xie's dispatcher */
			  SXieDispatch,		/* Swapped dispatch */
			  XieReset, 		/* Reset XIE stuff  */
			  StandardMinorOpcode	/* choose opcode dynamically */
			  );
  
  if (extEntry == NULL)
    FatalError(" could not add Xie as an extension\n");
  
  /* Initialize client table */
  bzero((char *)client_table, sizeof(client_table));

  if (!technique_init() || DdxInit() != Success)
    FatalError(" could not add Xie as an extension\n");

}

/**********************************************************************/
/* Register client with core X under a FakeClientID */
static int RegisterXieClient(client, minor)
ClientPtr client;
CARD16 minor;
{
    client_table[client->index].Shutdown_id  = FakeClientID(client->index);
    client_table[client->index].minorVersion = minor;
    
    init_proc_tables(minor,
		     &(client_table[client->index].proc_table),
		     &(client_table[client->index].sproc_table));
    
    /* Register the client with Core X for shutdown */
    return (AddResource(client_table[client->index].Shutdown_id,
		    RT_XIE_CLIENT,
		    &(client_table[client->index])));
}

/**********************************************************************/
static int XieDispatch (client)
     register ClientPtr	client;
{
  REQUEST(xieReq); 	/* make "stuff" point to client's request buffer */
  
  /* QueryImageExtension establishes a communication version */
  if (stuff->opcode == X_ieQueryImageExtension)
    return (ProcQueryImageExtension(client));		

  /* First make sure that a communication version has been established  */
  if (client_table[client->index].Shutdown_id == 0 && /* Pick a favorite */
      !RegisterXieClient(client, xieMinorVersion))    /* minor version   */
	return ( BadAlloc );
  if (stuff->opcode > 0 && stuff->opcode <= xieNumProtoReq) 
    /* Index into version specific routines */
    return (CallProc(client));
  else 
    return ( BadRequest);
}


/**********************************************************************/
static int SXieDispatch (client)
     register ClientPtr	client;
{
  REQUEST(xieReq);	/* make "stuff" point to client's request buffer */
  
  /* QueryImageExtension establishes a communication version */
  if (stuff->opcode == X_ieQueryImageExtension)
    return (SProcQueryImageExtension(client));		

  /* First make sure that a communication version has been established  */
  if (client_table[client->index].Shutdown_id == 0 && /* Pick a favorite */
      !RegisterXieClient(client, xieMinorVersion))    /* minor version   */
	return ( BadAlloc );
  if (stuff->opcode > 0 && stuff->opcode <= xieNumProtoReq) 
    /* Index into version specific routines */
    return (CallSProc(client));

  else 
    return ( BadRequest);
}

/**********************************************************************/
ProcQueryImageExtension(client)
     register ClientPtr client;
{
  xieQueryImageExtensionReply reply;
  XID FakeClientID();
  REQUEST(xieQueryImageExtensionReq);
  REQUEST_SIZE_MATCH( xieQueryImageExtensionReq );
  
  reply.type = X_Reply;
  reply.sequenceNum = client->sequence;
  
  reply.majorVersion = xieMajorVersion;
  
  if (stuff->majorVersion != xieMajorVersion || 
      stuff->minorVersion < xieEarliestMinorVersion ||
      stuff->minorVersion > xieLatestMinorVersion) 
    reply.minorVersion = xieMinorVersion;
  else 
    reply.minorVersion = stuff->minorVersion;

  reply.length = sizeof(Preferred_levels)>>2;
  
#if XIE_FULL
  reply.serviceClass          = xieValFull;
#else
  reply.serviceClass          = xieValDIS;
#endif
  reply.alignment             = ALIGNMENT;
  reply.unconstrainedMantissa = UNCONSTRAINED_MANTISSA;
  reply.unconstrainedMaxExp   = UNCONSTRAINED_MAX_EXPONENT;
  reply.unconstrainedMinExp   = UNCONSTRAINED_MIN_EXPONENT;
  
  /* 
    If this is the first QueryImageExtension for this client, register fake_id 
    with Core X to get a closedown notification later 
  */
  if (client_table[client->index].Shutdown_id == 0) 
  	if (!RegisterXieClient(client, reply.minorVersion)) 
        	return ( BadAlloc );
  
  /***	Take care of swapping bytes if necessary	***/
  if (client->swapped) {
    register int n;
    
    swaps(&reply.sequenceNum,n);
    swapl(&reply.length,n);
    swaps(&reply.majorVersion,n);
    swaps(&reply.minorVersion,n);
    swaps(&reply.unconstrainedMantissa,n);
    swapl(&reply.unconstrainedMaxExp,n);
    swapl(&reply.unconstrainedMinExp,n);
  }
  WriteToClient(client,sz_xieQueryImageExtensionReply,(char*)&reply);

  /*
   * Send the list of preferred levels (swapped as necessary)
   */
  if(reply.length)
    if(client->swapped)
      CopySwap32Write(client,sizeof(Preferred_levels),Preferred_levels);
    else
      WriteToClient(client,sizeof(Preferred_levels),(char*)Preferred_levels);
  
  return(Success);
}


/**********************************************************************/
SProcQueryImageExtension(client)
     register ClientPtr client;
{
  REQUEST(xieQueryImageExtensionReq);
  register int n;
  swaps(&stuff->length,n);
  swaps(&stuff->majorVersion, n);
  swaps(&stuff->minorVersion, n);
  return( ProcQueryImageExtension(client) );
}

/************************************************************************/

static int DdxInit() 
{
  return Success;
}

/**********************************************************************/
static int DeleteXieClient(data, id)
pointer data;
XID id;
{
  bzero((char *)&(client_table[CLIENT_ID(id)]), sizeof(struct _client_table));
}

/**********************************************************************/
static void XieReset (extEntry)
     ExtensionEntry	*extEntry;
{
#ifdef REPORT_MEMORY_LEAKS

  /* memory leak debug code
   */
  /* Initialize client table */
  bzero((char *)client_table, sizeof(client_table));
  if(ALLOCS)		/* check on outstanding mallocs and callocs */
    ErrorF("XieReset: %d allocs still outstanding.\n", ALLOCS);

  if(STRIPS || BYTES)	/* check on outstanding data manager allocs */
    ErrorF("XieReset: %d strips with %d data bytes still outstanding.\n",
	   STRIPS, BYTES);
#endif  
}

#ifdef DYNAMIC_MODULE
/*
 * Entry point of dynamic loading
 */
extern void (*XieInitPtr)(void);

int
#ifndef DLSYM_BUG
init_module(server_version)
#else
init_xie(server_version)
#endif
unsigned long server_version;
{

  XieInitPtr = XieInit;
  ErrorF("\tXIE extension module for XFree86%s\n", XF86_VERSION);
  return 1;
}
#endif /* DYNAMIC_MODULE */

/**** End of session.c ****/
