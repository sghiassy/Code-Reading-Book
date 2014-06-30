/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/os2/os2_mouse.c,v 3.11 1996/12/23 06:50:39 dawes Exp $ */
/*
 * (c) Copyright 1994 by Holger Veit
 *			<Holger.Veit@gmd.de>
 * Modified (c) 1996 Sebastien Marineau <marineau@genie.uottawa.ca>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * HOLGER VEIT  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Except as contained in this notice, the name of Holger Veit shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from Holger Veit.
 *
 */
/* $XConsortium: os2_mouse.c /main/10 1996/10/27 11:48:51 kaleb $ */

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "misc.h"
#include "inputstr.h"
#include "scrnintstr.h"

#include "compiler.h"

#define I_NEED_OS2_H
#define INCL_DOSFILEMGR
#define INCL_DOSQUEUES
#define INCL_MOU
#undef RT_FONT
#include "xf86.h"
#include "xf86Procs.h"
#include "xf86_OSlib.h"

#include "xf86Procs.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"

extern int miPointerGetMotionEvents(DeviceIntPtr pPtr, xTimecoord *coords,
				    unsigned long start, unsigned long stop,
				    ScreenPtr pScreen);

HMOU hMouse=65535;
HEV hMouseSem;
HQUEUE hMouseQueue;
int MouseTid;
BOOL HandleValid=FALSE;
extern BOOL SwitchedToWPS;
extern CARD32 LastSwitchTime;
void os2MouseEventThread();

int xf86MouseOff(mouse, doclose)
MouseDevPtr mouse;
Bool doclose;
{
	return -1;
}

void xf86SetMouseSpeed(mouse, old, new, cflag)
MouseDevPtr mouse;
int old;
int new;
unsigned cflag;
{
	/* not required */
}

void xf86OsMouseOption(token, lex_ptr)
int token;
pointer lex_ptr;
{
	/* no options, anything seen ignored */
}

/* almost everything stolen from sco_mouse.c */
int xf86OsMouseProc(pPointer, what)
DeviceIntPtr pPointer;
int what;
{
	APIRET rc=0;
	USHORT nbutton,state;
	unsigned char *map;
	int i;
	MouseDevPtr priv = (MouseDevPtr)((DeviceIntPtr) pPointer)->public.devicePrivate;

	switch (what) {
	case DEVICE_INIT: 
		pPointer->public.on = FALSE;
		if(hMouse==65535) rc = MouOpen((PSZ)0, &hMouse);
		if (rc != 0)
			FatalError("Cannot open mouse, rc=%d\n",rc);
		xf86Info.mouseDev->mseFd = -1;

		/* flush mouse queue */
		MouFlushQue(hMouse);

		/* check buttons */
		rc = MouGetNumButtons(&nbutton,hMouse);
		if (rc == 0)
			ErrorF("xf86-OS/2: OsMouse has %d button(s).\n",nbutton);
		if(nbutton==2) nbutton++;
		map = (unsigned char *) xalloc(nbutton + 1);
		if (map == (unsigned char *) NULL)
			FatalError("Failed to allocate OsMouse map structure\n");

		for (i = 1; i <= nbutton; i++)
			map[i] = i;

		InitPointerDeviceStruct((DevicePtr)pPointer, map, nbutton,
			miPointerGetMotionEvents, (PtrCtrlProcPtr)xf86MseCtrl,
			miPointerGetMotionBufferSize());

		xfree(map);

         /* OK, we are ready to start up the mouse thread ! */
                if(!HandleValid){
                      rc = DosCreateEventSem(NULL,&hMouseSem,DC_SEM_SHARED,TRUE);
                      if (rc != 0)
                         FatalError("xf86OpenMouse: could not create mouse queue semaphore, rc=%d\n",rc);
                      MouseTid=_beginthread(os2MouseEventThread,NULL,0x4000,(void *)NULL);
                      ErrorF("xf86-OS/2: Started Mouse event thread, Tid=%d\n",MouseTid);
                      DosSetPriority(2,3,0,MouseTid);
                      }

		HandleValid=TRUE;
		break;
      
	case DEVICE_ON:
		/*AddEnabledDevice(xf86Info.mouseDev->mseFd);*/
		if(!HandleValid) return(-1);
		xf86Info.mouseDev->lastButtons = 0;
		xf86Info.mouseDev->emulateState = 0;
		pPointer->public.on = TRUE;
		state = 0x300;
		rc=MouSetDevStatus(&state,hMouse);
		state = 0x7f;
		rc=MouSetEventMask(&state,hMouse);
		MouFlushQue(hMouse);
		break;
      
	case DEVICE_CLOSE:
	case DEVICE_OFF:
		if(!HandleValid) return(-1);
		pPointer->public.on = FALSE;
		state = 0x300;
		MouSetDevStatus(&state,hMouse);
		state = 0;
		MouSetEventMask(&state,hMouse);
		/*RemoveEnabledDevice(xf86Info.mouseDev->mseFd);*/
		if (what == DEVICE_CLOSE) {
			/* MouClose(hMouse);
			hMouse=65535;
			xf86Info.mouseDev.mseFd = -1;
		        HandleValid=FALSE;  */ /* Comment out for now as this seems to break server */
		}
		break;
	}
	return Success;
}

void xf86OsMouseEvents()
{
	APIRET rc;
        ULONG postCount,dataLength;
        PVOID junkPointer;
	int buttons;
	int state;
	int i, col,row;
        BYTE elemPriority;
        REQUESTDATA requestData;

	if(!HandleValid) return;
 	while((rc = DosReadQueue(hMouseQueue,&requestData,&dataLength,&junkPointer, 
                0L,1L,&elemPriority,hMouseSem))==0){
                  col=requestData.ulData;
         rc = DosReadQueue(hMouseQueue,&requestData,&dataLength,&junkPointer,
                0L,1L,&elemPriority,hMouseSem);
                 row=requestData.ulData;
         rc = DosReadQueue(hMouseQueue,&requestData,&dataLength,&junkPointer,
                0L,1L,&elemPriority,hMouseSem);
               state=requestData.ulData;
         rc = DosReadQueue(hMouseQueue,&requestData,&dataLength,&junkPointer,
                0L,1L,&elemPriority,hMouseSem);
               if(requestData.ulData!= 0xFFFFFFFF) ErrorF("Unexpected mouse event tag, %d\n",
                                 requestData.ulData);

		/* Contrary to other systems, OS/2 has mouse buttons *
		 * in the proper order, so we reverse them before    *
		 * sending the event.                                */
		 
			buttons = ((state & 0x06) ? 4 : 0) |
				  ((state & 0x18) ? 1 : 0) |
				  ((state & 0x60) ? 2 : 0);
			xf86PostMseEvent(xf86Info.pMouse, buttons, col, row);
	}
        DosResetEventSem(hMouseSem,&postCount);
	xf86Info.inputPending = TRUE;
}

int os2MouseQueueQuery()
{
	     /* Now we check for activity on mouse handles */
ULONG numElements,postCount;
APIRET rc;
        if(!HandleValid) return(1);
        DosResetEventSem(hMouseSem,&postCount);
        rc=DosQueryQueue(hMouseQueue,&numElements);
 	 if (numElements>0){     /* Something in mouse queue! */
             return(0);  /* Will this work? */
	     }
         return(1);
}


void os2MouseEventThread(arg)
void *arg;
{
     APIRET rc;
     MOUEVENTINFO mev;
     ULONG queueParam;
     USHORT waitflg;
     char queueName[128];
 
 
         sprintf(queueName,"\\QUEUES\\XF86MOU\\%d",getpid());
         rc=DosCreateQueue(&hMouseQueue,0L,queueName);
         ErrorF("xf86-OS/2: Mouse Queue created, rc=%d\n",rc);
         rc=DosPurgeQueue(hMouseQueue);
 
         while(1){
            waitflg = 1;
            rc=MouReadEventQue(&mev,&waitflg,hMouse);
            if(rc){
               ErrorF("Bad return code from mouse driver, rc=%d\n",rc);
               ErrorF("Mouse aborting!\n");
               break;
               }
 /* Format of mouse packet is the following: first queued message is mev.col, second is mev.row,
         third is state and last is FFFFFFFF for "end-of-record". We could pack this better but it
         is a good start... */
 
             queueParam=mev.col;
             rc=DosWriteQueue(hMouseQueue,queueParam,0L,NULL,0L);
             if(rc) break;
             queueParam=mev.row;
             rc=DosWriteQueue(hMouseQueue,queueParam,0L,NULL,0L);
             if(rc) break;
             queueParam=mev.fs;
             rc=DosWriteQueue(hMouseQueue,queueParam,0L,NULL,0L);
             if(rc) break;
             queueParam=0xFFFFFFFF;
             rc=DosWriteQueue(hMouseQueue,queueParam,0L,NULL,0L);
             if(rc) break;
         }
    ErrorF("An unrecoverable error in mouse queue has occured, rc=%d. Mouse is shutting down.\n",rc);
    DosCloseQueue(hMouseQueue);
}

