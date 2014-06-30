/*
 * $XConsortium: skyProbe.c /main/4 1995/12/05 15:44:57 matt $ 
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991 
 *
 * All Rights Reserved 
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of IBM not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS, IN NO EVENT SHALL IBM BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 */

/*
 * skyProbe.c - probe for display copied from ibm/GAI/gaiOsd.c 
 */

#include "OSio.h"
#include "ibmTrace.h"

#ifndef _IBM_LFT
#include "hftUtils.h"

/*--------------------------HFT Begin -----------------------------*/

#include <sys/hft.h>

extern unsigned hftQueryCurrentDevice();
extern void
                ibmInfoMsg();
extern int      hftQFD;

/* adapter number */

/* put this in         */
/* #include "X_msg.h"  */
/* aixErrMsg(M_MSG_9); */

int
skyProbe(atype, anumber)
    unsigned        atype;	/* adapter type (like HFT_SKYWAY_ID or just SKYWAY_ID (for lfts)) */
    int             anumber;	/* adapter number */
{
    unsigned int    devid;
    int             fd;

    TRACE(("skyProbe(atype=0x%x, anumber=%d)\n", atype, anumber));

    /**** added so that console can be used in debugging ****/
    AIXDefaultDisplay = HFT_SKYWAY_ID;
    TRACE(("AIXDefaultDisplay is %x\n", AIXDefaultDisplay));
    /********************************************************/

    if ((devid = hftHasAnAttachedNumber(atype, anumber)))
    {
	ibmInfoMsg("Found an IBM Graphics Adapter \n");

	TRACE(("found Adp 0x%x number %d\n",
	       devid, (devid & HFT_DEVNUM_MASK)));

	if ((fd = hftOpenOnDevice(devid)) < 0)
	{
	    ibmInfoMsg("Couldn't open a HFTVT on an IBM Graphics Adapter\n");
	    return -1;
	}
	hftSetQFDOrNoHotkey(fd);
	if (hftQFD == fd)
	{
	    AIXDefaultDisplay = devid;
	    TRACE(("AIXDefaultDisplay is %x\n", AIXDefaultDisplay));
	}
	return fd;
    }
    else
    {
	ibmInfoMsg("Didn't find an IBM Graphics Adapter\n");
	return -1;
    }
}
/*-------------------------- HFT End -----------------------------*/

#else

/*------------------------- LFT Begin -------------------------------*/
#include "skyHdwr.h"
#include "lftUtils.h"
int
skyProbe(atype, anumber)
    unsigned        atype;	/* adapter type (like SKYWAY_ID */
    int             anumber;	/* adapter number */
{
    int             fd;
    char	    *display_name;

    TRACE(("skyProbe(atype=0x%x, anumber=%d)\n", atype, anumber));
    /* 
    * We'll make sure the atype is inded SKYWAY_ID
    */
    if(atype != SKYWAY_ID) {
	    ibmInfoMsg("SkywayProbe : Bad atype\n");
	    return -1;
    }
    /* Now Query the lft to see whether we have skyways (gda{0,1..})
    * in the system 
    */
    display_name = get_disp_name_by_number(anumber);
    if(!strncmp(display_name,"gda",3)) {
	/* Found : Return the RCM fd 
	 * that will be used to get the GSC_HANDLE
	 * call.
	 */
	return get_rcm_fd();
    }
    else
    	return -1;
}

/*------------------------- LFT End -------------------------------*/
#endif /* _IBM_LFT */
