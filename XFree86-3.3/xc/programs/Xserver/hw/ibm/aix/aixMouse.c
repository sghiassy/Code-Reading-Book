/*
 * $XConsortium: aixMouse.c /main/7 1995/12/12 16:56:03 matt $
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND 
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

#include <stdio.h>
#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "input.h"
#include "cursorstr.h"

#include "ibmIO.h"
#include "ibmMouse.h"
#include "ibmScreen.h"

#include "ibmTrace.h"

#include "AIX.h"
#include "AIXext.h"

extern  char    *getenv();



/***================================================================***/

static int
rtGetMotionEvents(pDevice, buff, start, stop, pScr)
    DeviceIntPtr pDevice;
    xTimecoord *buff;
    CARD32 start, stop;
    ScreenPtr pScr;
{
    TRACE(("rtGetMotionEvents( buff= 0x%x, start= %d, stop= %d )\n",
	                                                buff,start,stop));
    return 0;
}

#ifndef _IBM_LFT

/*---------------------------------- HFT Case ------------------------------*/
#include <sys/hft.h>
#include "hftQueue.h"

/***================================================================***/

static unsigned always0= 0;
#define BUTTONMAPSIZE   5


#ifdef AIXV3
extern int hftQFD;
#endif

int
AIXMouseProc(pDev, onoff)
    DevicePtr   pDev;
    int onoff;
{
    BYTE map[BUTTONMAPSIZE];

#ifdef AIXV3
    struct hfchgloc hf_info;
#endif

    TRACE(("AIXMouseProc( pDev= 0x%x, onoff= 0x%x )\n",pDev, onoff ));

    switch (onoff)
    {
    case DEVICE_INIT:
	    ibmPtr = pDev;
	    map[1] = Button1;
	    map[2] = Button2;
	    map[3] = Button3;
	    map[4] = Button4;
	    InitPointerDeviceStruct(ibmPtr, map, BUTTONMAPSIZE,
			rtGetMotionEvents, ibmChangePointerControl, 0);
	    SetInputCheck( &hftPending, &always0 );

#ifdef AIXV3
	    /* Set mouse sample rate, resolution, threshhold, and scale.
	       Ideally, the default values should be okay, but they're not,
	       so this is neccessary to make the mouse smooth. -- EWu 8/17/89 */

	    hf_info.hf_cmd = HFMRATE;
	    hf_info.loc_value1 = 60;
	    if (ioctl(hftQFD,HFCHGLOC,&hf_info)<0)
		perror("setting mouse ioctl rate");

	    hf_info.hf_cmd = HFMRES;
	    hf_info.loc_value1 = 8;
	    if (ioctl(hftQFD,HFCHGLOC,&hf_info)<0)
		perror("setting mouse ioctl resolution");

	    hf_info.hf_cmd = HFMTHRESH;
	    hf_info.loc_value1 = 2;
	    hf_info.loc_value2 = 2;
	    if (ioctl(hftQFD,HFCHGLOC,&hf_info)<0)
		perror("setting mouse ioctl threshhold");

	    hf_info.hf_cmd = HFMSCALE;
	    hf_info.loc_value1 = 1;
	    if (ioctl(hftQFD,HFCHGLOC,&hf_info)<0)
		perror("mouse ioctl scale");
#endif
	    break;
    case DEVICE_ON:
	    pDev->on = TRUE;
	    break;
    case DEVICE_OFF:
	    pDev->on = FALSE;
	    break;
    case DEVICE_CLOSE:
	    break;
    }
    return Success;
}
/*---------------------------------- HFT End ------------------------------*/

#else
/*-------------------------------- LFT Begin ------------------------------*/
#include <fcntl.h>
#include <stdio.h>
#include <sys/time.h>
#include "lftUtils.h"

extern int TimeoutCount;

int ms2_qhead, ms2_qtail;        /* 2 buttons mouse check */

static void
Mouse2BlockHandler(blockData, wt, LastSelectMask)
     pointer blockData;         /* unused */
     struct timeval **wt;
     long *LastSelectMask;
{
#define SelectWaitTime 10000		/* value compatible with 3.2.x */

     if (wt == NULL)
        return;

     /* if delayed to determine if middle button is intended */
     /* for 2 buttons mouse, then reset the select time. */

     if (TimeoutCount > 0) {
        static struct timeval time_out = {0, SelectWaitTime};

        if (*wt == NULL) {
            *wt = &time_out;
        } else {
            (*wt)->tv_sec = time_out.tv_sec;
            (*wt)->tv_usec = time_out.tv_usec;
        }
     }
}



static void
Mouse2WakeupHandler(blockData, i, pReadMask)
     pointer blockData;		/* unused */
     int i;			/* returned code from select */
     long *pReadMask;		/* selected fd masks */
{
     /* check for 2 buttons mouse's delayed event */
     if (TimeoutCount > 0) {
	extern int AIXMouseChordDelay;
	static long begin_time;
	long delta_time;

#define MAX_DELAY_TIME 1000	/* one second */
#define ABS(x) (((x)>0)?(x):-(x))

	delta_time = ABS(GetTimeInMillis() - begin_time);

	/* in heavy loaded situation, it would take minutes to go */
	/* through 20 dispatch loops, so enforce 1 second as cap */
	/* for delayed time. In some test, this triples the mouse  */
	/* reponse time. */

	if (TimeoutCount == AIXMouseChordDelay)
	    begin_time = GetTimeInMillis();
	else if (delta_time > MAX_DELAY_TIME)
	    TimeoutCount = 1;


	if ((--TimeoutCount) <= 0) {
	     TRACE(("AIXWakeupHandler found delayed event\n"));
	     ms2_qhead = !ms2_qtail;
	}

        TRACE(("delta_time=%d,TimeCount=%d\n", delta_time, TimeoutCount));
     }
}


int
AIXMouseProc(pDev, onoff)
    DevicePtr   pDev;
    int onoff;
{
    BYTE map[BUTTONMAPSIZE];
    InputDevPrivate *pPriv = &lftInputInfo[MOUSE_ID];
    int mouse_type;
    extern void aixMouse2Event();
    extern void aixMouse3Event();

    TRACE(("aixMouseProc(pDev=0x%x, onoff=%d)\n", pDev, onoff));
    
    switch(onoff) {
	case DEVICE_INIT:
		map[1] = Button1;
		map[2] = Button2;
		map[3] = Button3;

		ibmPtr = pDev;
	    	InitPointerDeviceStruct(ibmPtr, map, BUTTONMAPSIZE,
			rtGetMotionEvents, ibmChangePointerControl, 0);
		/* Open mouse device */

		break;
	case DEVICE_ON :
	   {
	   struct uregring reg_ring; 	/* to register mouse input ring */
		if(pDev->on == TRUE) {
			break;
		}
		pPriv->fd = open("/dev/mouse0", O_RDWR);
		if(pPriv->fd < 0) {
			FatalError("Cannot open /dev/mouse0 device\n");
		}
		/* Register input ring, we use the same
		 * ring for both mouse and keyboard
		 */
		reg_ring.ring = (caddr_t)lft_input_ring;
		reg_ring.report_id = MOUSE_ID;
		reg_ring.size = INPUT_RING_SIZE;
		if(ioctl(pPriv->fd, MREGRING, (caddr_t)&(reg_ring)) == -1) {
			FatalError("Mouse Input ring register failed\n");
		}
		/* Set some mouse controls */

		ioctl(pPriv->fd, MSAMPLERATE, MSR60);
		ioctl(pPriv->fd, MRESOLUTION, MRES3);
		ioctl(pPriv->fd, MTHRESHOLD, ibmPointerThreshold);
		ioctl(pPriv->fd, MSCALE, MSCALE11); /* 1:1 scale */
		/* Find out the type of mouse */
		ioctl(pPriv->fd,  MQUERYID, &mouse_type);
		if(mouse_type == MOUSE3B) {
		    pPriv->eventHandler = aixMouse3Event;
		}
		else {
		    pPriv->eventHandler = aixMouse2Event;
		    if (!RegisterBlockAndWakeupHandlers(
			(BlockHandlerProcPtr)Mouse2BlockHandler,
                        (WakeupHandlerProcPtr)Mouse2WakeupHandler, NULL)) {
			FatalError("Error in Register Block/Wakeup handler\n");
		    }
		}
		pDev->on = TRUE;
	   }
	   break;
	case DEVICE_OFF:
	   {
		struct uregring null_ring;
		/* Unregister mouse ring */
		null_ring.ring = (caddr_t)NULL;
		ioctl(pPriv->fd, MREGRING, &null_ring);
		pPriv->eventHandler = (void(*)())NoopDDA;
		pDev->on = FALSE;
	   }
	   break;
    }
    return Success;
}

/*---------------------------------- LFT End ------------------------------*/
#endif
