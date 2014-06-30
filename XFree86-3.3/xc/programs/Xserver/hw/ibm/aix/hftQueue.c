/*
 * $XConsortium: hftQueue.c /main/7 1995/12/05 15:44:04 matt $
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

#ifndef _IBM_LFT

#include <signal.h>
#define HFRDATA 8912         /* override HFRDATA in hft.h */
#include <sys/hft.h>
#include <fcntl.h>           /* cals */

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "input.h"
#include "scrnintstr.h"
#include "cursorstr.h"
#include "miscstruct.h"
#include "aixCursor.h"
#include "ibmScreen.h"

#include "ibmTrace.h"
#include "aixError.h"
#include "hftQueue.h"

	/*
	 * Defines to simplify dealing with AIX.
	 */

#define HFWDCHARS(n)    (n)>>8&0xff, (n)&0xff
#define FLWDCHARS(n)    (n)>>24&0xff,(n)>>16&0xff,(n)>>8&0xff,(n)&0xff
#define VTD(n)  HFINTROESC, HFINTROLBR, HFINTROEX, FLWDCHARS(n)

#ifdef AIXV3  /* fix version 2 bug */
#define ringlen sizeof(hftRingbuf)
#define RINGLEN HFWDCHARS(ringlen)
#define RINGVTD VTD(sizeof(hftRingreq) - 3)
#define RINGOFFSET      FLWDCHARS(sizeof(hftRing) - sizeof(hftRingbuf) -\
	                          sizeof(hftRingreq.hf_intro) - \
	                          sizeof(hftRingreq.hf_sublen) -\
	                          sizeof(hftRingreq.hf_subtype))
#define RINGBASE        ((unsigned char *)&hftRingbuf.hf_rsvd[0])
#define RINGFIRST       32
#define RINGLAST        sizeof(hftRingbuf)
#else

#define ringlen sizeof(hftRingbuf)
#define RINGLEN HFWDCHARS(ringlen)
#define RINGVTD VTD(sizeof(hftRing) - 3)
#define RINGOFFSET      FLWDCHARS(sizeof(hftRing) - sizeof(hftRingbuf) -\
	                          sizeof(hftRingreq.hf_intro) - \
	                          sizeof(hftRingreq.hf_sublen) -\
	                          sizeof(hftRingreq.hf_subtype))
#define RINGBASE        ((unsigned char *)&hftRingbuf.hf_rsvd[0])
#define RINGFIRST       32
#define RINGLAST        sizeof(hftRingbuf)
#endif

#define HFT_ESC_KEYBOARD        0x77
#define HFT_ESC_LOCATOR         'y'
#define HFT_ESC_ADAPT           'r'
#define HFT_ESC_FOCUSIN         'z'

	/*
	 * Package global variables.
	 */

volatile unsigned        hftPending;
volatile unsigned        hftGrantPending;
volatile unsigned        hftRetractPending;

static  int             hftSetInterrupts= 0;
static  int             hftHaveScreen=  FALSE;
	int             hftQFD= -1;
	int             hftNeedsReset=  FALSE;

static  hftEvHandler    hftHandlers[HFT_NEVENTS]= {
	                        HFT_IGNORE,
	                        HFT_IGNORE,
	                        HFT_IGNORE,
	                        HFT_IGNORE,
	                        HFT_IGNORE,
	                        HFT_IGNORE,
	                        HFT_IGNORE,
	                        HFT_IGNORE,
	                        HFT_IGNORE,
	                        HFT_IGNORE
	                };

static  int             hftEvSize[HFT_NEVENTS]= {
	                        sizeof(hftKeyEv),
	                        sizeof(hftLocEv),
	                        sizeof(hftAdaptEv),
	                        sizeof(hftLocEv),
	                        sizeof(hftLPFKEv),
	                        sizeof(hftDialEv)
	                };

#define HFT_N_LOC_STYPES        4
static  int             hftLocatorSubtype[HFT_N_LOC_STYPES] = {
	                HFT_LOCATOR,    /* mouse report */
	                HFT_TABLET,     /* locator report */
	                HFT_LPFK,       /* LPFK report */
	                HFT_DIAL        /* Dial report */
};

static  struct  hfprotocol      hftProtoReq= {
	        VTD(sizeof(hftProtoReq)-3),     /* escape sequence introducer */
	        HFMOMPROH, HFMOMPROL, 2, 0      /* random garbage */
	};

struct hfmomscreq       hftRingreq;
struct {
     char hf_rsvd[2];
     char hf_intreq;
     char hf_ovflow;
     unsigned hf_source;
     unsigned hf_sink;
     int hf_unused[5];
     char hf_rdata[HFRDATA];
} hftRingbuf ;
/*
      = {
	RINGVTD,HFMOMREQH,HFMOMREQL,2,0,RINGLEN,RINGOFFSET,
	0,0,0xff,0,RINGFIRST,RINGFIRST,0,0,0,0,0,
	};
*/
#ifdef AIXV3
#define INT_TO_ARRAY(a,i)       *((char *)      (a))       = (i) >> 24 ; \
	                        *((char *)      (a) + 1)   = (i) >> 16 ; \
	                        *((char *)      (a) + 2)   = (i) >> 8 ; \
	                        *((char *)      (a) + 3)   = (i) ;
#define SHORT_TO_ARRAY(a,i)     *((char *)      (a))       = (i) >> 8 ; \
	                        *((char *)      (a) + 1 )  = (i) ;
#endif

static  hftEvent        hftProtoArrived= { HFT_EVENT_ARRIVED };
static  hftEvent        hftProtoGranted= { HFT_GRANTED };
static  hftEvent        hftProtoRetracted= { HFT_RETRACTED };
static  hftEvent        hftProtoBefore= { HFT_BEFORE_EVENTS };
static  hftEvent        hftProtoAfter= { HFT_AFTER_EVENTS };

static  hftEvHandler    hftTimeoutHndlr;
static  int             hftTimeoutCount;
extern  int             AIXDefaultDisplay ;    /* cals */

/***====================================================================***/

void
hftInterruptAlways()
{
    TRACE(("hftInterruptAlways()\n"));
    hftRingbuf.hf_intreq= hftSetInterrupts= 0xff;
    return;
}

/***====================================================================***/

void
hftInterruptOnFirst()
{
    TRACE(("hftInterruptOnFirst()\n"));
    hftRingbuf.hf_intreq= hftSetInterrupts= 0;
    return;
}


static void
hftQuitMsg()
{
    TRACE(("hftQuit()\n"));
    return ;
}
static void
hftIgnoreMsg()
{
    TRACE(("hftIgnoreMsg()\n"));
    hftRingbuf.hf_intreq= hftSetInterrupts;
    hftPending++;
    if (hftHandlers[HFT_EVENT_ARRIVED])
	(*hftHandlers[HFT_EVENT_ARRIVED])(&hftProtoArrived);
    return ;
}

static void
hftGrant()
{
    TRACE(("hftGrant() hftGrantPending=%d, hftRetractPending=%d, hftPending=%d\n", hftGrantPending, hftRetractPending, hftPending));
    hftGrantPending++;
    hftPending++;
    if (hftHandlers[HFT_EVENT_ARRIVED])
	(*hftHandlers[HFT_EVENT_ARRIVED])(&hftProtoArrived);
    return ;
}

static void
hftRetract()
{
    TRACE(("hftRetract() hftGrantPending=%d, hftRetractPending=%d, hftPending=%d\n", hftGrantPending, hftRetractPending, hftPending));
    hftRetractPending++;
    hftPending++;
    if (hftHandlers[HFT_EVENT_ARRIVED])
	(*hftHandlers[HFT_EVENT_ARRIVED])(&hftProtoArrived);
    return ;
}

static void
hftSetUpSignals(save)
int     save;
{
struct  sigaction       svquit;
struct  sigaction       svgrant;
struct  sigaction       svretract;
struct  sigaction       svmessage;
struct  sigaction       oldsig;

    TRACE(("hftSetUpSignals()\n"));

    svgrant.sa_handler=         hftGrant;
    SIGINITSET(svgrant.sa_mask);
    SIGADDSET(svgrant.sa_mask,SIGGRANT);
    SIGADDSET(svgrant.sa_mask,SIGRETRACT);
    svgrant.sa_flags=   SA_RESTART ;

    svretract.sa_handler=       hftRetract;

    SIGINITSET(svretract.sa_mask);
    SIGADDSET(svretract.sa_mask,SIGGRANT);
    SIGADDSET(svretract.sa_mask,SIGRETRACT);
    svretract.sa_flags= SA_RESTART;

    svmessage.sa_handler=       hftIgnoreMsg;
    SIGINITSET(svmessage.sa_mask);
    SIGADDSET(svmessage.sa_mask,SIGMSG);
    svmessage.sa_flags= SA_RESTART ;

    svquit.sa_handler=  hftQuitMsg;
    SIGINITSET(svquit.sa_mask);
    SIGADDSET(svquit.sa_mask,SIGQUIT);
    svquit.sa_flags=    SA_RESTART ;

    sigaction(SIGQUIT,&svquit,&oldsig);
    sigaction(SIGGRANT,&svgrant,&oldsig);
    if (save) {
	if ((oldsig.sa_handler!=SIG_DFL)&&(oldsig.sa_handler!=SIG_IGN))
	    hftHandlers[HFT_GRANTED]= oldsig.sa_handler;
    }
    sigaction(SIGRETRACT,&svretract,&oldsig);
    if (save) {
	if ((oldsig.sa_handler!=SIG_DFL)&&(oldsig.sa_handler!=SIG_IGN))
	    hftHandlers[HFT_RETRACTED]= oldsig.sa_handler;
    }
    sigaction(SIGMSG,&svmessage,&oldsig);
    if (save) {
	if ((oldsig.sa_handler!=SIG_DFL)&&(oldsig.sa_handler!=SIG_IGN))
	    hftHandlers[HFT_EVENT_ARRIVED]= oldsig.sa_handler;
    }

    return ;
}

	/*
	 * defined in hftQueue.h
	 */

int
hftInitQueue(deviceId,saveOldHandlers)
int     deviceId;
int     saveOldHandlers;
{
    unsigned    offset;

    TRACE(("hftInitQueue() hftQFD is %d\n",hftQFD));

    if (hftQFD<0) {
	errFatal(("NO OUTPUT DEVICE"));
      /**
	hftQFD = open("/dev/hft",O_WRONLY);
	if (hftQFD<0)
	    ErrorF("hftInitQueue: Cannot open /dev/hft to query device ids\n");
	return(TRUE);
       **/
    }

    TRACE(("hftInitQueue: After FindAScreen hftQFD is %d\n",hftQFD));

    hftSetUpSignals(saveOldHandlers);

    hftPutAllIntoMonitorMode(&hftRingbuf);
    aixConnectScreens();
    hftActivateVT(hftQFD);
    hftPending= 0;

#ifdef AIXEXTENSIONS
    hftQueryHardwareConfig(deviceId);
#endif

    return(TRUE);
}

/***====================================================================***/

static  struct  hfprotocol      hftKSRProtoReq= {
	        VTD(sizeof(hftProtoReq)-3),     /* escape sequence introducer */
	        HFKSRPROH, HFKSRPROL, 2, 0      /* random garbage */
	};

static  struct  hfmomscrel      hftVTDRelease= {
	                                VTD(sizeof(hftVTDRelease)-3),
	                                HFMOMRELH,HFMOMRELL
	                        };
/* file descriptor to send the Release to after a Retract has been received */
extern int  RetractFD;

#include <stdio.h>
void
hftTermQueue()
{
int     rtrn;

   TRACE(("hftTermQueue()\n"));

   if (hftNeedsReset) {
	rtrn= write(hftQFD,&hftVTDRelease,sizeof(hftVTDRelease));
	if (rtrn!=sizeof(hftVTDRelease)) {
	    ErrorF("hftTermQueue release screen VTD failed (%d)\n",
	                                                                rtrn);
	}
	hftKSRProtoReq.hf_select[0]= HFHOSTS|HFXLATKBD;
	hftKSRProtoReq.hf_value[0]= HFXLATKBD;
	hftKSRProtoReq.hf_select[1]= HFWRAP|HFMOUSE|HFTABLET|HFLPFKS|HFDIALS;
	hftKSRProtoReq.hf_value[1]= HFWRAP;
	rtrn= write(hftQFD,&hftKSRProtoReq,sizeof(hftKSRProtoReq));
	if (rtrn!=sizeof(hftKSRProtoReq)) {
	    ErrorF("hftTermQueue KSR protocol VTD failed (%d)\n",
	                                                                rtrn);
	}
	rtrn= ioctl(hftQFD,HFCMON,0);
	if (rtrn!=0) {
	    ErrorF("hftTermQueue CMON ioctl failed\n",rtrn);
	}
	hftQFD= -1;
	hftNeedsReset= 0;
   }
   return;
}

/***====================================================================***/

	/*
	 * described in hftQueue.h
	 */

hftEvHandler
hftInstallHandler(evType,pHandler)
int     evType;
hftEvHandler    pHandler;
{
hftEvHandler    pOld;

    TRACE(("hftInstallHandler(%d,0x%x)\n",evType,pHandler));
    if ((hftQFD==-1)||(evType<0)||(evType>=HFT_NEVENTS)) {
	return(HFT_ERROR);
    }
    pOld= hftHandlers[evType];
    hftHandlers[evType]= pHandler;
    return(pOld);
}

	/*
	 *  described in hftQueue.h
	 */

void
hftAddTimeout(pHandler,count)
hftEvHandler    pHandler;
int             count;
{
    TRACE(("hftAddTimeout(0x%x,%d)\n",pHandler,count));

    if ((count>0)&&(pHandler!=NULL)) {
	hftTimeoutHndlr=        pHandler;
	hftTimeoutCount=        count;
    }
    else {
	hftTimeoutHndlr=        NULL;
	hftTimeoutCount=        0;
    }
    return;
}

#define HFT_HEADERSIZE  3
#define HFT_BYTESLEFT   (source>sink?source-sink:source-sink+HFRDATA)
#define HFT_INCR(s)     (++s>=RINGLAST?s=RINGFIRST:0)

void
hftDispatchEvents()
{
	 char            *input= hftRingbuf.hf_rdata-RINGFIRST;
	 unsigned        source= hftRingbuf.hf_source;
	 unsigned        sink= hftRingbuf.hf_sink;
	 unsigned char   *buf;
	 hftEvent        thisEvent;
	 int             nPending,tmp;
	 int             device;

    TRACE(("hftDispatchEvents()\n"));

    if ((hftPending)&&(hftHandlers[HFT_BEFORE_EVENTS])) {
	(*hftHandlers[HFT_BEFORE_EVENTS])(&hftProtoBefore);
    }

    if (hftGrantPending) {
	if (hftHandlers[HFT_GRANTED])
	    (*hftHandlers[HFT_GRANTED])(&hftProtoGranted);
	hftGrantPending= 0;

	/* make sure the VT we get input from is active */
	/* (after a grant it will be visible, but it may */
	/* not be active) */
	hftActivateVT(hftQFD);

    }
    while (sink!=source) {
	if (HFT_BYTESLEFT<HFT_HEADERSIZE)  {
	    if (hftRingbuf.hf_ovflow)   goto incomplete;
	    else                        break;
	}
	if (input[sink]!=27)
	        goto illegal;
	HFT_INCR(sink);
	if (input[sink]!='[')
	        goto illegal;
	HFT_INCR(sink);
	if      (input[sink]==HFT_ESC_KEYBOARD)         device= HFT_KEYBOARD;
	else if (input[sink]==HFT_ESC_LOCATOR)          device= HFT_LOCATOR;
	else if (input[sink]==HFT_ESC_ADAPT)            device= HFT_ADAPT;
	else if (input[sink]==HFT_ESC_FOCUSIN) {
	  TRACE(("new HFT_ESC_FOCUSIN\n"));
	      HFT_INCR(sink);
	      hftRingbuf.hf_sink= sink;
	      continue;   /* with while loop */
	  }
	else                                            goto illegal;
	HFT_INCR(sink);
	nPending= hftEvSize[device];

	if (HFT_BYTESLEFT<nPending)
	    goto incomplete;
	buf= (unsigned char *)&thisEvent.hftEv;
	while (nPending--) {
	    *buf++= input[sink];
	    HFT_INCR(sink);
	}
	hftRingbuf.hf_sink= sink;

	if (device==HFT_LOCATOR)
	    device= hftLocatorSubtype[thisEvent.hftEv.hftLoc.leStype];

	if (hftHandlers[device]!=NULL) {
	    (*hftHandlers[device])(&thisEvent);
	}
    }
    if (hftRetractPending) {
	if (hftHandlers[HFT_RETRACTED])
	    (*hftHandlers[HFT_RETRACTED])(&hftProtoRetracted);
	hftRetractPending= 0;
	write(RetractFD,&hftVTDRelease,sizeof(hftVTDRelease));
    }

    if (hftTimeoutCount>0) {
	if (--hftTimeoutCount==0)
	    (*hftTimeoutHndlr)();
    }

    if ((sink==hftRingbuf.hf_source)&&(hftTimeoutCount<=0))
	hftPending= 0;


    if (hftHandlers[HFT_AFTER_EVENTS]) {
	(*hftHandlers[HFT_AFTER_EVENTS])(&hftProtoAfter);
    }

    return;
	/*
	 * Called when a partial event is sitting on the queue.
	 * if there is an overflow, print a warning and reset the
	 * queue.  Could do some fancy attempts to parse event if
	 * we really wanted to.  If there is no overflow, just return.
	 */
incomplete:
	if (hftRingbuf.hf_ovflow) {
	    errWarning(("ring buffer overflow! events lost"));
	    hftRingbuf.hf_sink= hftRingbuf.hf_source;
	    hftRingbuf.hf_ovflow= 0;
	    hftPending= 0;
	}
	return;
	/*
	 * called when an illegal character is found in a ring buffer
	 * structure header (or an illegal device is specified).
	 * prints a warning and finds the next legal header.
	 */
illegal:
	TRACE((" sink is %d  input char is %d \n",sink, input[sink]));
	ErrorF("Illegal character in ring buffer header - ");
	ErrorF("sink is %02x  input char is %02x \n",sink, input[sink]);
	while (sink!=hftRingbuf.hf_source) {
	    if (input[sink]==27) {
	        hftRingbuf.hf_sink= sink;
	        HFT_INCR(sink);
	        if (input[sink]!='[')   continue;
	        HFT_INCR(sink);
	        if ((input[sink]==HFT_ESC_KEYBOARD)||
	                                (input[sink]==HFT_ESC_LOCATOR)||
	                                (input[sink]==HFT_ESC_ADAPT)) {
	            break;
	        }
	    }
	    else HFT_INCR(sink);
	}
	hftRingbuf.hf_sink= sink;
	if (sink==hftRingbuf.hf_source)
	    hftPending= 0;
	return;
}

/*
struct hfmomring {                      * monitor mode ring status, ptrs *
   char  hf_rsvd[2];                    * reserved *
   char  hf_intreq;                     * intr request, send a SIGMSG:
	                                     0x00 = when buffer becomes empty
	                                     0xFF = for each input event *
   char  hf_ovflow;                     * overflow:  0x00=normal,
	                                              0xFF=overflow *
   ulong hf_source;                     * current sys ptr (offset=32+) *
   ulong hf_sink;                       * current user ptr (offset=32+) *
   long  hf_unused[5];                  * reserved *
   char  hf_rdata[HFRDATA];             * data goes here *
};
*/

void
hftEnterMonitorMode(fd, rptr)
    int  fd;
    struct hfmomring  *rptr;
{
    struct hfsmon  flags;
    extern unsigned  hftGrantPending;

    TRACE(("hftEnterMonitorMode(fd=%d) #1\n",fd));
    TRACE(("hftEnterMonitorMode()  hftGrantPending=%d\n",hftGrantPending));
    hftGrantPending = 0;
    flags.hf_momflags= HFSINGLE;
    if (ioctl(fd,HFSMON,&flags)!=0) {
	errFatal(("error entering monitor mode"));
	exit(1);
    }

    hftProtoReq.hf_select[0]=   HFXLATKBD;
    hftProtoReq.hf_select[1]=   HFMOUSE | HFTABLET | HFDIALS | HFLPFKS ;
    hftProtoReq.hf_value[0]=    0;
    hftProtoReq.hf_value[1]=    HFMOUSE | HFTABLET | HFDIALS | HFLPFKS ;

    if (write(fd,&hftProtoReq,sizeof(hftProtoReq))!=sizeof(hftProtoReq)) {
	errFatal(("protocol write failed"));
    }

/*-----------------------------------------------------------*/


    if( fd == hftQFD ){
	rptr->hf_intreq= hftSetInterrupts;

	hftRingreq.hf_intro[0] = 0x1b ;
	hftRingreq.hf_intro[1] = '[' ;
	hftRingreq.hf_intro[2] = 'x' ;
	INT_TO_ARRAY(&(hftRingreq.hf_intro[3]),sizeof(hftRingreq)- 3);
	hftRingreq.hf_intro[7] = HFMOMREQH ;
	hftRingreq.hf_intro[8] = HFMOMREQL ;
	hftRingreq.hf_sublen = 2 ;
	hftRingreq.hf_subtype = 0 ;
	SHORT_TO_ARRAY(&(hftRingreq.hf_ringlen[0]),sizeof(struct hfmomring));
	INT_TO_ARRAY(&(hftRingreq.hf_ringoffset[0]), ((char *)rptr) - (char *)&(hftRingreq.hf_ringlen[0]));
	rptr->hf_intreq = 0 ;
	rptr->hf_ovflow = 0 ;
	rptr->hf_source = 32 ;
	rptr->hf_sink = 32 ;

	if (write(fd,&(hftRingreq),sizeof(hftRingreq))!=sizeof(hftRingreq)) {
	    errFatal(("request write failed"));
	}
	TRACE(("AFTER 1 write Ring ADDR is %x hftRing source is %d dstis %d \n",&(hftRingreq),rptr->hf_source,rptr->hf_sink));
	TRACE(("size = %d\n", sizeof(hftRingreq)));
    }
    else{
	TRACE(("HFINTROSZ=%d\n",HFINTROSZ));
	hftRingreq.hf_intro[0] = 0x1b ;
	hftRingreq.hf_intro[1] = '[' ;
	hftRingreq.hf_intro[2] = 'x' ;
	INT_TO_ARRAY(&(hftRingreq.hf_intro[3]), HFINTROSZ-3);
	hftRingreq.hf_intro[7] = HFMOMREQH ;
	hftRingreq.hf_intro[8] = HFMOMREQL ;

	if (write(fd,&(hftRingreq),HFINTROSZ)!=HFINTROSZ) {
	    errFatal(("request write failed"));
	}
    }

/*-----------------------------------------------------------*/

    TRACE(("hftEnterMonitorMode() waiting for grant #1\n"));
    while( ! hftGrantPending ){
    }
    TRACE(("hftEnterMonitorMode() waiting for grant #2\n"));
    TRACE(("hftEnterMonitorMode() got a grant  hftGrantPending=%d\n",hftGrantPending));
    hftGrantPending = 0;
    TRACE(("hftEnterMonitorMode(fd=%d) #2\n",fd));
}

hftPutAllIntoMonitorMode(rptr)
    struct hfmomring  *rptr;
{
    int  i;

    TRACE(("hftPutAllIntoMonitorMode()\n"));
    for(i=0;i<ibmNumScreens;i++){
	hftActivateVT(ibmScreenFD(i));
	hftEnterMonitorMode(ibmScreenFD(i), rptr);
    }
}

#endif /* !_IBM_LFT */
