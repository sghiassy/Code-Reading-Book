/* $XConsortium: technq.c,v 1.6 94/04/17 20:34:00 rws Exp $ */
/* $XFree86: xc/programs/Xserver/XIE/dixie/request/technq.c,v 3.0 1996/03/29 22:11:30 dawes Exp $ */
/**** module technq.c ****/
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
****************************************************************************
 	technq.c: Routines to handle technique protocol requests

	Dean Verheiden  AGE Logic, Inc.  April 1993
****************************************************************************/

#define _XIEC_TECHNQ

/* 
  A Note on Technique Etiquette:

	To change technique defaults, or add new techniques or technique 
  groups, don't edit this file. Make the appropriate changes to the technq.h 
  file. 

  	If the changes result in the creation of a new default technique or 
  allow a technique to use default parameters, it is also necessary that
  the corresponding technique copy function be updated to supply default
  parameters should they be required.
*/
	

/*
 *  Include files
 */
#include <stdio.h>
/*
 *  Core X Includes
 */
#define NEED_EVENTS
#include <X.h>
#include <Xproto.h>
/*
 *  XIE Includes
 */
#include <XIE.h>
#include <XIEproto.h>
/*
 *  more X server includes.
 */
#include <misc.h>
#include <dixstruct.h>

/*
 *  Element Specific Includes
 */
#include <corex.h>
#include <macro.h>
#include <memory.h>
#include <technq.h>

/*
 *  Xie protocol proceedures called from the dispatcher
 */
int  ProcQueryTechniques();
int  SProcQueryTechniques();

/*
 *  Functions called by other Xie procedures
 */
techVecPtr FindTechnique();

/*
 *  Used internally by this module
 */
static Bool send_reply();
static Bool send_technique_replies();

/*------------------------------------------------------------------------
------------------------ QueryTechniques Procedures -----------------------
------------------------------------------------------------------------*/
int ProcQueryTechniques(client)
     ClientPtr client;
{
  xieQueryTechniquesReply rep;
  REQUEST( xieQueryTechniquesReq );
  REQUEST_SIZE_MATCH( xieQueryTechniquesReq );
  
  /*
   * Fill in the reply header
   */
  bzero((char *)&rep, sz_xieQueryTechniquesReply);
  rep.type        = X_Reply;
  rep.sequenceNum = client->sequence;

  /* First, figure out how big the ListofTechniqueRecs is going to be */
  if (stuff->techniqueGroup == xieValDefault) {
      rep.numTechniques = techTable.numDefaults;
      rep.length        = techTable.defaultSize;
  } else if (stuff->techniqueGroup == xieValAll) {
      rep.numTechniques = techTable.numTechniques;
      rep.length        = techTable.tableSize;
  } else {
      TechGroupPtr tg = techTable.techgroups;
      int g;

      for (g = 0; g < techTable.numGroups; g++, tg++)
 	  if (stuff->techniqueGroup == tg->group) {
	      rep.numTechniques = tg->numTechniques;
	      rep.length        = tg->groupSize;
	      break;
	  }

      if (g >= techTable.numGroups)	/* Couldn't find the group */
	  return(BadValue);
  }

  if( client->swapped ) {      
    /*
     * Swap the reply header fields
     */
    register int n;
    
    swaps(&rep.sequenceNum,n);
    swapl(&rep.length,n);
    swaps(&rep.numTechniques,n);
  }

  WriteToClient(client, sz_xieQueryTechniquesReply, (char *)&rep);

  send_technique_replies(stuff->techniqueGroup,client);

  return(Success);
}                               /* end ProcQueryTechniques */

int SProcQueryTechniques(client)
     ClientPtr client;
{
  register int n;
  REQUEST( xieQueryTechniquesReq );
  swaps(&stuff->length, n);
  REQUEST_SIZE_MATCH( xieQueryTechniquesReq );
  return (ProcQueryTechniques(client));
}                               /* end SProcQueryTechniques */

/*************************************************************************
 Initialization Procedure: Fill in various technique fields
*************************************************************************/

Bool technique_init()
{
    CARD32 defaultSize = 0;
    CARD32 tableSize = 0;
    CARD32 numTechniques = 0;
    CARD32 numDefaults = 0;
    TechGroupPtr tg = techTable.techgroups;
    int g;

    for (g = 0; g < techTable.numGroups; g++, tg++) {
	TechPtr	           tp = tg->tech;
	CARD16	defaultNumber = tg->defaultNumber;
	Bool	  needDefault = (defaultNumber != NO_DEFAULT);
	CARD32      groupSize = 0;
	int t;

	for (t = 0; t < tg->numTechniques; t++, tp++) {
	    tp->nameLength =  strlen((char *)(tp->name));	
	    tp->techSize   = (sizeof(xieTypTechniqueRec) + tp->nameLength + 3) 
									>> 2;
	    groupSize += tp->techSize;
	    if (needDefault && defaultNumber == tp->techvec.number) {
		tg->defaultIndex = t;
		defaultSize += tp->techSize;
		numDefaults++;
		needDefault = FALSE;	
	    }
	}
	if (needDefault) return (FALSE);
	tg->groupSize = groupSize;
	tableSize += groupSize;
	numTechniques += tg->numTechniques;
    }
 
    techTable.numTechniques = numTechniques;
    techTable.numDefaults   = numDefaults;
    techTable.tableSize     = tableSize;
    techTable.defaultSize   = defaultSize;

    return (TRUE);
}				/* end technique_init */

/*************************************************************************
 Send Technique Reply: send technique structures to client, one at a time,
			swapping them first if necessary
*************************************************************************/

static Bool send_reply(group, tech, client)
xieTypTechniqueGroup group;
TechPtr tech;
ClientPtr client;
{
	xieTypTechniqueRec rep;

	rep.needsParam = !(tech->techvec.NoTech || tech->techvec.OptionalTech);
	rep.group = group;
	rep.number = (client->swapped) ? lswaps(tech->techvec.number) : 
						tech->techvec.number;
	rep.speed = tech->speed;
	rep.nameLength = tech->nameLength;

	/* Send everything except the name */
  	WriteToClient(client, sz_xieTypTechniqueRec, (char *)&rep);
	/* Send the name */
  	WriteToClient(client, tech->nameLength, (char *)tech->name);

	return(TRUE);
}

static Bool send_technique_replies(group,client)
xieTypTechniqueGroup	group;
ClientPtr client;
{
    if (group == xieValDefault) {
        TechGroupPtr tg = techTable.techgroups;
        int g;

	for (g = 0; g < techTable.numGroups; g++, tg++) 
	    if (tg->defaultNumber != NO_DEFAULT)
		send_reply(tg->group,&tg->tech[tg->defaultIndex],client);
        return (TRUE);
    } else if (group == xieValAll) {
        TechGroupPtr tg = techTable.techgroups;
        int g;

	for (g = 0; g < techTable.numGroups; g++, tg++) {
	    TechPtr tp = tg->tech;
	    int      t = 0;

	    for (t = 0; t < tg->numTechniques; t++, tp++)
		send_reply(tg->group,tp,client);
	}
        return (TRUE);
    } else {
        TechGroupPtr tg = techTable.techgroups;
        int g;

	for (g = 0; g < techTable.numGroups; g++, tg++)
	    if (group == tg->group) {
	       TechPtr tp = tg->tech;
	       int      t = 0;

	       for (t = 0; t < tg->numTechniques; t++, tp++)
	           send_reply(group,tp,client);

	    	   return(TRUE);
	    }
    }

    return(FALSE);	/* Unrecognized group */
}				/* end return_technique_replies */

/*************************************************************************
FindTechnique: return group's technique entry point structure
*************************************************************************/
techVecPtr FindTechnique(group,number)
xieTypTechniqueGroup	group;
CARD16	number;
{
    TechGroupPtr tg = techTable.techgroups;
    int g;

    for (g = 0; g < techTable.numGroups; g++, tg++)
	if (group == tg->group) {
    	    TechPtr tp = tg->tech;
    	    int      t = 0;

	    if (number == xieValDefault) {
		if (tg->defaultNumber)
		    return (&tg->tech[tg->defaultIndex].techvec);
		else
    		    return((techVecPtr)NULL);	/* No default for this group */
	    } else {
    	        for (t = 0; t < tg->numTechniques; t++, tp++)
		    if (tp->techvec.number == number)
			return (&tp->techvec);
	    }
	}

    return((techVecPtr)NULL);	/* Unrecognized group */
}				/* end FindTechnique */

/*************************************************************************
NoParamCheck: Used as copyfnc for techniques that do not have parameters.
	      Checks to make sure that no parameters have been passed in.
*************************************************************************/
static Bool NoParamCheck(flo, rparms, cparms, tsize) 
     floDefPtr flo;
     pointer rparms, cparms;
     CARD16 tsize;
{
  return(!tsize);
}			/* end NoParamCheck */

/*************************************************************************
NoTechYet: error stub for unimplemented technique routines
*************************************************************************/
static Bool NoTechYet(flo, ped, parm, tech) 
     floDefPtr flo;
     peDefPtr  ped;
     pointer   parm;
     pointer   tech;
{
  return(FALSE);
}			/* end NoTechYet */

/* end module technq.c */
