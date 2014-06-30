/*
 * $XConsortium: hftUtils.c /main/5 1995/12/05 15:44:10 matt $
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
/*-----------------------------------HFT Code----------------------*/
#include <sys/hft.h>
#include <fcntl.h>

#include "hftUtils.h"

#include "aixError.h"
#include "compiler.h"
#include "ibmTrace.h"

#ifdef AIXV3
#include "X_msg.h"
#endif

#ifdef AIXEXTENSIONS
#include "AIX.h"
#include "AIXext.h"
#endif

extern  int     hftQFD;
extern  int     hftNeedsReset;
extern  int     AIXDefaultDisplay ;

static  CurrentKeyClick = 0;    /* track current key click volume */

/***====================================================================***/

	/*
	 * Defines to simplify dealing with AIX.
	 */

#define HFWDCHARS(n)    (n)>>8&0xff, (n)&0xff
#define FLWDCHARS(n)    (n)>>24&0xff,(n)>>16&0xff,(n)>>8&0xff,(n)&0xff
#define VTD(n)  HFINTROESC, HFINTROLBR, HFINTROEX, FLWDCHARS(n)

#define HFTWRITE(s,m)   \
	if (write(hftQFD,&s,sizeof(s))!=sizeof(s)) {\
	        ErrorF(m);\
	}

#ifdef AIXV3
#include <sys/termio.h>   /* posix line discipline stuff */

int
hftPosix(fd)
int fd;
{
	struct termios  getIO, putIO;

	TRACE((" hftPosix enter ok \n"));
	if(tcgetattr(fd,&getIO)<0){
	        return -1 ;
	}

	if(tcgetattr(fd,&putIO)<0){
	        return -1 ;
	}

	putIO.c_oflag &= ~OPOST ;

	if(tcsetattr(fd,TCSANOW,&putIO)<0){
	        return -1 ;
	}

	TRACE((" hftPosix exit ok \n"));
	return 0 ;
}

#endif

/***====================================================================***/


static  struct  hfkled  hftProtoLED =
    { VTD(sizeof(hftProtoLED)-3),  HFKLEDCH, HFKLEDCL, 2, 1 };

/*
 * Turn all lock LED indicators on or off
 */

SetLockLEDs (onoff)
int       onoff;
{
	hftProtoLED.hf_ledselect = HFCAPSLOCK | HFSCROLLOCK | HFNUMLOCK;
	hftProtoLED.hf_ledvalue = onoff;
	HFTWRITE(hftProtoLED,"set all LEDs write failed\n");
}

/*
 * Turn caps lock LED indicator on or off
 */

SetCapsLockLED (onoff)
int       onoff;
{
	hftProtoLED.hf_ledselect = HFCAPSLOCK;
	hftProtoLED.hf_ledvalue = ( onoff ? HFCAPSLOCK : 0 );
	HFTWRITE(hftProtoLED,"set CapsLockLED write failed\n");
}

/*
 * Turn num lock LED indicator on or off
 */

SetNumLockLED (onoff)
int       onoff;
{
	hftProtoLED.hf_ledselect = HFNUMLOCK;
	hftProtoLED.hf_ledvalue = ( onoff ? HFNUMLOCK : 0 );
	HFTWRITE(hftProtoLED,"set NumLockLED write failed\n");
}

/*
 * Turn scroll lock LED indicator on or off
 */

SetScrollLockLED (onoff)
int       onoff;
{
	hftProtoLED.hf_ledselect = HFSCROLLOCK;
	hftProtoLED.hf_ledvalue = ( onoff ? HFSCROLLOCK : 0 );
	HFTWRITE(hftProtoLED,"set ScrollLockLED write failed\n");
}

void
hftSetLEDS(which,num)
int which ;
int num ;
{
	int onoff ;

	/* take a short cut to analyze the mask */

	onoff = 0 ;

	if (num & 1)
	   onoff |= HFCAPSLOCK ;

	if (num & 2 )
	   onoff |= HFNUMLOCK ;

	if (num & 4 )
	   onoff |= HFSCROLLOCK ;

	SetLockLEDs(onoff);

	/* who cares !! */

}

/***====================================================================***/


static struct hfloth    hftProtoLocThresh =
    { VTD(sizeof(hftProtoLocThresh)-3), HFLOTHCH, HFLOTHCL, 2, 1 };

void
hftSetLocatorThresholds(horz,vert)
unsigned        horz,vert;
{
    TRACE(("hftSetLocatorThresholds(0x%x,0x%x)\n",horz,vert));
    hftProtoLocThresh.hf_hthresh[0]= (horz>>8)&0xff;
    hftProtoLocThresh.hf_hthresh[1]= (horz&0xff);
    hftProtoLocThresh.hf_vthresh[0]= (vert>>8)&0xff;
    hftProtoLocThresh.hf_vthresh[1]= (vert&0xff);
    HFTWRITE(hftProtoLocThresh,"set locator threshold write failed\n");
    return;
}

/***====================================================================***/

static struct hftdzone  hftProtoDZone =
    { VTD(sizeof(hftProtoDZone)-3), HFTDZCH, HFTDZCL, 2, 1 };

void
hftSetTableDeadZone(horz,vert)
unsigned        horz,vert;
{
    TRACE(("hftSetTabletDeadZone(0x%x,0x%x)\n",horz,vert));
    hftProtoDZone.hf_horizontal[0]= (horz>>8)&0xff;
    hftProtoDZone.hf_horizontal[1]= (horz&0xff);
    hftProtoDZone.hf_vertical[0]= (vert>>8)&0xff;
    hftProtoDZone.hf_vertical[1]= (vert&0xff);
    HFTWRITE(hftProtoDZone,"set tablet dead zone write failed\n");
    return;
}

/***====================================================================***/

static struct hfdial_lpfk       hftProtoSetLPFK =
    { VTD(sizeof(hftProtoSetLPFK)-3), HFLPFKSCH, HFLPFKSCL, 2, 1 };

void
hftSetLPFK(keys,flags)
unsigned        keys,flags;
{
    TRACE(("hftSetLPFK(0x%x,0x%x)\n",keys,flags));
    hftProtoSetLPFK.hf_mask.keys[0]= (keys>>24)&0xff;
    hftProtoSetLPFK.hf_mask.keys[1]= (keys>>16)&0xff;
    hftProtoSetLPFK.hf_mask.keys[2]= (keys>>8)&0xff;
    hftProtoSetLPFK.hf_mask.keys[3]= (keys)&0xff;
    hftProtoSetLPFK.hf_data2.lpfk.flags[0]=     (flags>>24)&0xff;
    hftProtoSetLPFK.hf_data2.lpfk.flags[1]=     (flags>>16)&0xff;
    hftProtoSetLPFK.hf_data2.lpfk.flags[2]=     (flags>>8)&0xff;
    hftProtoSetLPFK.hf_data2.lpfk.flags[3]=     (flags)&0xff;
    HFTWRITE(hftProtoSetLPFK,"set LPFK write failed\n");
    return;
}

/***====================================================================***/

static struct hfdial_lpfk       hftProtoSetGranularity =
    { VTD(sizeof(hftProtoSetGranularity)-3), HFDIALSCH, HFDIALSCL, 2, 1 };

void
hftSetDialGranularity(dials,settings)
unsigned        dials;
unsigned char   settings[16];
{
int     i;

    TRACE(("hftSetDialGranularity(0x%x,0x%x)\n",dials,settings));
    hftProtoSetGranularity.hf_mask.dials[0]= (dials>>24)&0xff;
    hftProtoSetGranularity.hf_mask.dials[1]= (dials>>16)&0xff;
    hftProtoSetGranularity.hf_mask.dials[2]= (dials>>8)&0xff;
    hftProtoSetGranularity.hf_mask.dials[3]= (dials)&0xff;
    for (i=0;i<16;i++) {
	hftProtoSetGranularity.hf_data2.granularity[i]= settings[i];
    }
    HFTWRITE(hftProtoSetGranularity,"set dial granularity write failed\n");
    return;
}

/***====================================================================***/

static  struct  hfrconf hftProtoReconfigure;

static struct hfsound   hftProtoSound =
    { VTD(sizeof(hftProtoSound)-3), HFSOUNDCH, HFSOUNDCL, 2, 1, HFEXECALWAYS};

void
hftSound(vol, duration,frequency)
int             vol;
unsigned        duration;
unsigned        frequency;
{
int     real_vol;

     TRACE(("hftSound(%d,%d)\n",duration,frequency));

     real_vol = getVolume (vol);
     if (real_vol == 0)
	return;

     /* set bell volume */
     if (real_vol != CurrentKeyClick) {
	hftProtoReconfigure.hf_op= HFCHGVOLUME;
	hftProtoReconfigure.hf_obj= real_vol;
	ioctl(hftQFD,HFRCONF,&hftProtoReconfigure);
     }

     /* set bell duration and frequency, then ring bell */
	hftProtoSound.hf_dur[0]= (duration>>8)&0xff;
	hftProtoSound.hf_dur[1]= (duration&0xff);
	hftProtoSound.hf_freq[0]= (frequency>>8)&0xff;
	hftProtoSound.hf_freq[1]= (frequency&0xff);
	HFTWRITE(hftProtoSound,"generate sound write failed\n");

     /* restore old value of keyclick */
     if (real_vol != CurrentKeyClick) {
	/*
	 * If the click is off, do not set the volume to 0
	 */
	if (CurrentKeyClick != 0) {
	  hftProtoReconfigure.hf_obj= CurrentKeyClick;
	  ioctl(hftQFD,HFRCONF,&hftProtoReconfigure);
	}
     }

     return;
}


/***====================================================================***/


static int
getVolume (volume)
int     volume;
{
	/* volume range 0-100 */

	if ((volume >= 1) && (volume <= 33))
	    volume = 1;                 /* low */
	else if ((volume >= 34) && (volume <= 66))
	    volume = 2;                 /* medium */
	else if (volume >= 67)
	    volume = 3;                 /* high */

	return (volume);
}


/***====================================================================***/

void
hftSetTypematicDelay(delay)
unsigned        delay;
{
    TRACE(("hftSetTypematicDelay(%d)\n",delay));
    hftProtoReconfigure.hf_op= HFCHGKBDDEL;
    hftProtoReconfigure.hf_obj= delay;
    ioctl(hftQFD,HFRCONF,&hftProtoReconfigure);
    return;
}

/***====================================================================***/

void
hftSetTypematicRate(rate)
unsigned        rate;
{
    TRACE(("hftSetTypematicRate(%d)\n",rate));
    hftProtoReconfigure.hf_op= HFCHGKBDRATE;
    hftProtoReconfigure.hf_obj= rate;
    ioctl(hftQFD,HFRCONF,&hftProtoReconfigure);
    return;
}

/***====================================================================***/

void
hftSetKeyClick(volume)
int     volume;
{

    TRACE(("hftSetKeyClick volume is %d hftQFD is %d\n",volume,hftQFD));

    if (volume != 0) {
	hftProtoReconfigure.hf_op = HFCHGCLICK;
	hftProtoReconfigure.hf_obj = 1;         /* turn click on */
	ioctl(hftQFD, HFRCONF, &hftProtoReconfigure);
    }

    hftProtoReconfigure.hf_obj = CurrentKeyClick = getVolume(volume);
    hftProtoReconfigure.hf_op= HFCHGVOLUME;
    ioctl(hftQFD,HFRCONF,&hftProtoReconfigure);

    return;
}

/***====================================================================***/

void
hftSetLocatorSampleRate(rate)
unsigned        rate;
{
    TRACE(("hftSetLocatorSampleRate(%d)\n",rate));

#ifdef AIXV3
{
    struct hfchgloc x;
    x.hf_cmd = HFTRATE;
    x.loc_value1 = rate;
    ioctl(hftQFD,HFCHGLOC,&x);
}
#else
    hftProtoReconfigure.hf_op= HFCHGLOCRATE;
    hftProtoReconfigure.hf_obj= rate;
    ioctl(hftQFD,HFRCONF,&hftProtoReconfigure);
#endif

    return;
}

/***====================================================================***/

static struct hfqdevidc qdevidcmd =
	{ VTD(sizeof(qdevidcmd)-3), HFQDEVIDCH, HFQDEVIDCL };

static struct hfquery   query =    { qdevidcmd.hf_intro, sizeof( qdevidcmd ) };
static struct hfrconf   reconfig = { HFSETDD };

static struct {
	        char    pad;
	        char    hf_esc;
	        char    hf_lbr;
	        char    hf_ex;
	        int     hf_len;
	        char    hf_typehi;
	        char    hf_typelo;
	        char    hf_sublen;
	        char    hf_subtype;
	        short   hf_numdev;
	        struct {
	            short       hf_idhi,hf_idlo;
	            short       hf_classhi,hf_classlo;
	        } hf_devices[HFT_MAXDEVICES];
}  qDevIDResponse;

static  hftDeviceID hftDevices[HFT_MAXDEVICES];
static int  number_of_devices = -1;

int
hftQueryDeviceIDs(ppDevices)
hftDeviceID     **ppDevices;
{
int     tempfd= -1;
int     mustClose= FALSE;


    TRACE(("hftQueryDeviceIDs(0x%x) hftQFD is %d \n",ppDevices,hftQFD));

    if( number_of_devices > 0 ){
	TRACE(("using info from previous call\n"));
	*ppDevices = hftDevices;
	return(number_of_devices);
    }

    if (hftQFD < 0) {
	tempfd = open("/dev/tty",O_WRONLY);
	if (tempfd<0) {
	    tempfd = open("/dev/console",O_WRONLY);
	}
	if (tempfd<0) {
	    tempfd = open("/dev/hft",O_WRONLY);
	}
	if (tempfd<0) {
	    ErrorF("Cannot open /dev/tty, /dev/console, or /dev/hft to query device ids\n");
	    return 0;
	}
	mustClose= TRUE;
    }
    else tempfd= hftQFD;

    query.hf_resp=      &qDevIDResponse.hf_esc;
    query.hf_resplen=   sizeof(qDevIDResponse)-1;
    qDevIDResponse.hf_len= sizeof( qDevIDResponse ) - 4;

/*
	if tempfd is an hft then put into posix Posix
*/
    if ( ioctl( tempfd, HFQUERY, &query ) ) {
	if (mustClose)
	    close(tempfd);
	mustClose = FALSE;
	if( (tempfd = open("/dev/hft",O_WRONLY)) < 0 ){
#ifdef AIXV3
	    aixErrMsg(M_MSG_8);
#else
	    ErrorF( "AIX: unable to query device id\n" );
#endif
	    return 0;
	}
	if ( ioctl( tempfd, HFQUERY, &query ) ) {
	    close(tempfd);
#ifdef AIXV3
	    aixErrMsg(M_MSG_8);
#else
	    ErrorF( "AIX: unable to query device id\n" );
#endif
	    return 0;
	}
	close(tempfd);
    }

    if (mustClose)
	close(tempfd);

    MOVE( qDevIDResponse.hf_devices, hftDevices, sizeof(hftDevices) );
    *ppDevices= hftDevices;

    number_of_devices = qDevIDResponse.hf_numdev;

    {
	int  i;

	for ( i = 0;  i < number_of_devices;  ++i ) {
	    TRACE(("display %d is 0x%x\n", i, hftDevices[i].hftDevID));
	}
    }

    return(qDevIDResponse.hf_numdev);

}

/***====================================================================***/

static struct hfqhftc   qhft =      { VTD(6),
	                              HFQHFTCH, HFQHFTCL } ;

static struct hfquery   ttquery =    { qhft.hf_intro, sizeof( qhft ) };

unsigned
hftQueryCurrentDevice()
{
int             tempfd= -1;
unsigned        *retval;
volatile struct hfqhftr resp;

    TRACE(("hftQueryCurrentDevice() hftQFD is %d\n",hftQFD));

    if (hftQFD<0) {
	tempfd = open("/dev/tty",O_WRONLY);
	if (tempfd<0) {
	    ErrorF("Cannot open /dev/tty to query device id\n");
	    return 0;
	}
    }
    else tempfd= hftQFD;

    ttquery.hf_resp    = resp.hf_intro;
    ttquery.hf_resplen = sizeof( resp ) + 1 ;

/*
	if tempfd is an hft then put into posix Posix
*/

    if ( ioctl( tempfd, HFQUERY, &ttquery ) ) {
	ErrorF( "AIX: unable to query tty device id\n" );
	if (tempfd!=hftQFD)
	    close(tempfd);
	return 0;
    }

    if (tempfd!=hftQFD)
	close(tempfd);

    return resp.hf_phdevid ;

}

unsigned
hftQueryDeviceID(fd)
    int  fd;
{
    volatile struct hfqhftr  resp;

    TRACE(("hftQueryDeviceID(fd=%d)\n",fd));

    ttquery.hf_resp    = resp.hf_intro;
    ttquery.hf_resplen = sizeof( resp ) + 1 ;

    if ( ioctl(fd, HFQUERY, &ttquery ) ) {
	ErrorF( "AIX: unable to query tty device id\n" );
	return 0;
    }
    TRACE(("hftQueryDeviceID(devid=0x%x)\n",resp.hf_phdevid));

    return resp.hf_phdevid ;
}

volatile struct hfqhftr resp1;
static struct hfquery   query1 =    { qhft.hf_intro, sizeof(qhft) } ;

int
hftGetKeyboardID ()
{
int     tempfd;

    TRACE(("hftGetKeyboardID hftQFD is %d\n",hftQFD));

    query1.hf_resp    = resp1.hf_intro;
    query1.hf_resplen = sizeof( resp1 ) + 1 ;

    tempfd= hftQFD; /* fix this */

    if ( ioctl( tempfd, HFQUERY, &query1 ) ) {
#ifdef AIXV3
	aixErrMsg(M_MSG_7);
#else
	ErrorF ("AIX: unable to querry current keyboard id\n");
#endif
	return (HFT_ILLEGAL_KEYBOARD);
    }

    return (resp1.hf_phkbdid);
}

/***====================================================================***/

int
hftSetDefaultDisplay(idWanted)
int     idWanted;
{
int              fd;

    TRACE(("hftSetDefaultDisplay(idWanted=0x%x)\n",idWanted));

    if (idWanted == 0) {
	return TRUE;
    }

    if (reconfig.hf_obj=hftHasAnAttached(idWanted)) {
	if ((hftQueryDefaultDisplay()&HFT_DEVID_MASK)!=idWanted) {
	    fd= open("/dev/console",O_WRONLY);
	    if (fd<0) {
	        ErrorF("Unable to open /dev/console to set default display\n");
	        return FALSE;
	    }
/*
	if tempfd is an hft then put into posix Posix
*/
	    if ( ioctl( fd, HFRCONF, &reconfig ) ) {
	        ErrorF( "AIX: unable to change default display\n" );
	        close(fd);
	        return FALSE;
	    }
	    close (fd);
	}
    }
    else {
	ErrorF( "requested default display not attached\n" );
	return FALSE;
    }

    return TRUE;
}

extern void ibmInfoMsg() ;

/***====================================================================***/

unsigned
hftQueryDefaultDisplay()
{
hftDeviceID     *pDevices;

    TRACE(("hftQueryDefaultDisplay()\n"));

    if (hftQueryDeviceIDs(&pDevices)) {
	return(pDevices[0].hftDevID&HFT_DEVID_MASK);
    }
    return(HFT_ILLEGAL_ID);
}

/***====================================================================***/

int
hftHasAnAttached(devId)
unsigned int devId;
{
int              i;
int              nDevices;
hftDeviceID     *pDevices;

    TRACE(("hftHasAnAttached(0x%x)\n",devId));

    nDevices= hftQueryDeviceIDs(&pDevices);

    if (nDevices==0)
	return(FALSE);

    for ( i = 0;  i < nDevices;  ++i ) {
	if ( (pDevices[i].hftDevID&HFT_DEVID_MASK) == (devId&HFT_DEVID_MASK) ) {
	    return pDevices[i].hftDevID;
	}
    }

    return FALSE;
}

int
hftHasAnAttachedNumber(devId, devNum)
unsigned int devId;
unsigned int devNum;
{
int              i;
int              nDevices;
hftDeviceID     *pDevices;

    TRACE(("hftHasAnAttachedNumber(devId=0x%x, devNum=%d)\n",devId,devNum));

    nDevices= hftQueryDeviceIDs(&pDevices);

    if (nDevices==0)
	return(FALSE);

    for ( i = 0;  i < nDevices;  ++i ) {
	TRACE(("hftHasAnAttachedNumber(hftDevID=0x%08x)\n",pDevices[i].hftDevID));
	TRACE(("hftHasAnAttachedNumber(   devID=0x%08x)\n",devId));
	TRACE(("hftHasAnAttachedNumber(   devNum=0x%08x)\n",devNum));
	if ( (pDevices[i].hftDevID&HFT_DEVID_MASK) == (devId&HFT_DEVID_MASK) /*&&*/
	/*(pDevices[i].hftDevID&HFT_DEVNUM_MASK) == (devNum&HFT_DEVNUM_MASK)*/ )
	{
	    return pDevices[i].hftDevID;
	}
    }

    TRACE(("hftHasAnAttachedNumber(returning FALSE)\n"));
    return FALSE;

}

struct hfchgdsp  hftcd = {
	'\033','[','x',0,0,0,sizeof(struct hfchgdsp)-3,
	HFCHGDSPCH, HFCHGDSPCL, 2,0,
	0,0,  0,0,0,0,0,0,0,0,  0,0,0,0,  0,0,0,0,0,0,0,0,0,0
};


int
hftChangeVTToPhysicalDisplay(fd, device)
    int  fd;
    unsigned  device;
{
    TRACE(("hftChangeVTToPhysicalDisplay(fd=0%d,device=0x%x)\n",fd,device));

    hftcd.hf_mode[0] = HFNONDEF;
    hftcd.hf_devid[0]= (device>>24)&0xff;
    hftcd.hf_devid[1]= (device>>16)&0xff;
    hftcd.hf_devid[2]= (device>>8)&0xff;
    hftcd.hf_devid[3]= (device)&0xff;

    return( write( fd, &hftcd, sizeof(hftcd) ) == sizeof(hftcd) );
}

int
hftOpenOnDevice(device)
    unsigned int  device;
{
    int  fd;

    TRACE(("hftOpenOnDevice(device=0x%x)\n",device));

    if( (fd=open("/dev/hft", O_RDWR)) < 0 ){
	return -1;
    }
    hftPosix(fd);
    if( hftChangeVTToPhysicalDisplay(fd, device) ){
	return fd;
    }
    close(fd);

    return -1;
}

void hftNoHotkeyVT();

void
hftSetQFDOrNoHotkey(fd)
    int  fd;
{
    TRACE(("hftSetQFDOrNoHotkey(fd=%d)\n",fd));

    if( hftQFD < 0 ){
	/* use this fd for Q */
	hftQFD = fd;
    }
    else
    {
	hftNoHotkeyVT(fd);
	/* disable this fd from hotkey */
    }
}

/* need to declare as void before we use it */
void hftActivateVT();

void
hftManagerCommand(fd, hfcmd)
    int  fd;
    long  hfcmd;
{
    struct hftsmgrcmd  cmd;
    struct hftqdresp  qdev;

    TRACE(("hftManagerCommand(fd=%d, hfcmd=0x%x)\n",fd,hfcmd));

    if( ioctl(fd, HFTQDEV, &qdev) == -1 ){
	ErrorF("hftManagerCommand(fd, hfcmd): ioctl HFTQDEV failed");
	return;
    }

    cmd.hf_cmd = hfcmd;
    cmd.hf_vtid = qdev.hf_vtid;
    cmd.hf_vsid = 0L;

    if( ioctl(fd, HFTCSMGR, &cmd) == -1 ){
	ErrorF("hftManagerCommand(fd, hfcmd): ioctl HFTCSMGR failed");
    }
}

void
hftActivateVT(fd)
    int  fd;
{
    TRACE(("hftActivateVT(fd=%d)\n",fd));
    hftManagerCommand(fd, SMACT);
}

void
hftHideVT(fd)
    int  fd;
{
    TRACE(("hftHideVT(fd=%d)\n",fd));
    hftManagerCommand(fd, SMHIDE);
}

void
hftUnHideVT(fd)
    int  fd;
{
    TRACE(("hftUnHideVT(fd=%d)\n",fd));
    hftManagerCommand(fd, SMUNHIDE);
}

void
hftNoHotkeyVT(fd)
    int  fd;
{
    TRACE(("hftNoHotkeyVT(fd=%d)\n",fd));
    hftManagerCommand(fd, SMNOHOTKEY);
}

struct hfqretractc  whichretract_command = {
	'\033','[','x',0,0,0,sizeof(struct hfqretractc)-3,
	HFQRETRACTCH, HFQRETRACTCL
};
struct hfqretractr  whichretract_response = {
	'\033','[','x',0,0,0,sizeof(struct hfqretractr)-3,
	HFQRETRACTRH, HFQRETRACTRL, 2,0,
	0,0, 0,0,0,0
};
struct hfqretract   whichgotretract = {
	&whichretract_command,
	sizeof(struct hfqretractc),
	&whichretract_response,
	sizeof(struct hfqretractr)
};

unsigned
WhoIsGettingTheRetract()
{
    int  vtid;
    unsigned  devid;

    TRACE(("WhoIsGettingTheRetract\n"));

    if ( ioctl( hftQFD, HFQUERY, &whichgotretract) ) {
	ErrorF("AIX: unable to query who get the retract\n");
	return;
    }

    vtid = ((unsigned)whichretract_response.hf_vtid[0]) << 8;
    vtid |= ((unsigned)whichretract_response.hf_vtid[1]);
    devid = ((unsigned)whichretract_response.hf_devid[0]) << (3*8);
    devid |= ((unsigned)whichretract_response.hf_devid[1]) << (2*8);
    devid |= ((unsigned)whichretract_response.hf_devid[2]) << (1*8);
    devid |= ((unsigned)whichretract_response.hf_devid[3]);

    TRACE(("WhoIsGettingTheRetract  vtid=0x%x  devid=0x%x\n", vtid, devid));

    return devid;
}

#ifdef AIXEXTENSIONS

extern AIXInfoRec aixInfo;
extern int aixTabletScaleX,aixTabletScaleY ;

#ifdef AIXV3
  struct hftqdresp hfqdstuff ;
#else
  union
  {
    struct hfqdev   qdev;
    struct hfqdresp qdresp;
  } hfqdstuff;
#endif

#ifdef AIXV3
static struct hfqgraphdev qlocdev = { VTD( sizeof(qlocdev) - 3 ),
	                              HFQTABLETCH, HFQTABLETCL };
#else
static struct hfqgraphdev qlocdev = { VTD( sizeof(qlocdev) - 3 ),
	                              HFQLOCCH, HFQLOCCL };
#endif

union {
   struct hfqlocr loc;
   char junk[sizeof(struct hfqlocr)+1];
}   resp3;

static struct hfquery   query3 =    { qlocdev.hf_intro, sizeof(qlocdev),
	                              resp3.junk, sizeof( resp3 ) };
#ifdef AIXTABLET
static struct hfqphdevc qphdev =    { VTD( sizeof(struct hfqphdevc) - 3 ),
	                              HFQPDEVCH, HFQPDEVCL, 2, 0, FLWDCHARS(0) }
;
static struct hfqphdevr phdevresp;
static struct hfqphdev  phdevquery ={ &qphdev, sizeof(struct hfqphdevc),
	                              &phdevresp, sizeof(struct hfqphdevr) };
#endif AIXTABLET

void
hftQueryHardwareConfig(device)
unsigned        device;
{

    TRACE(("hftQueryHardwareConfig(0x%x)\n",device));

#ifdef AIXV3
    hfqdstuff.hf_vtid = 10 ;  /* check this value */
    hfqdstuff.hf_dev = 20 ;  /* check this value */
	                     /* devname */
    hfqdstuff.hf_numdisp = 1;
#else
    hfqdstuff.qdev.hf_qdrsvd = 0;
    hfqdstuff.qdev.hf_qdopts = 2;
    hfqdstuff.qdev.hf_qdlen =
       sizeof(hfqdstuff) - sizeof(hfqdstuff.qdev);
#endif

#ifdef AIXV3
    if (ioctl(hftQFD, HFTQDEV, &hfqdstuff)) {
	aixErrMsg(M_MSG_10);
    }
    else {
	struct hfdial_lpfk r;      /* response structure of dials & lpfks */
	struct hfqdial_lpfk q;     /* query structure of dials & lpfks */
	struct hfqgraphdev hf_cmd; /* structure for hft command */

	aixInfo.kbdiodn = hfqdstuff.hf_kbddev ;
#ifdef AIXTABLET
	if ( ioctl(hftQFD , HFQUERY, &phdevquery ) ) {
	    long err_ret;
	    ioctl(hftQFD, HFQERROR, &err_ret);

	    /* err_ret 1801 is response buffer overflow, ignore this error */
	    if (err_ret != 1801) {
	        ErrorF("AIX: unable to query physical device\n" );
	        ErrorF("exiting\n");
	    }
	}
	TRACE(("hftQueryHardwareConfig: locattr = %x\n",
	        phdevresp.hf_locattr[0]));

	if ( ! (phdevresp.hf_locattr[0] & HFLOCREL ) )
	    aixInfo.mouseiodn = 0 ;
	else

	aixInfo.mouseiodn = hfqdstuff.hf_mousedev ;
	TRACE(("MOUSEDEV : mouseiodn is %x\n",aixInfo.mouseiodn));

	if ( ! (phdevresp.hf_locattr[0] & HFLOCABS ) )
	    aixInfo.mouseiodn = 0 ;
	else

	aixInfo.tabletiodn = hfqdstuff.hf_tabletdev ;
	TRACE(("TABLETDEV : tabletiodn is %x\n",aixInfo.tabletiodn));
#else
	aixInfo.lociodn = hfqdstuff.hf_mousedev ;
	TRACE(("MOUSEDEV : lociodn is %d\n",aixInfo.lociodn));
	aixInfo.lociodn = hfqdstuff.hf_tabletdev ;
	TRACE(("TABLETDEV : lociodn is %x\n",aixInfo.lociodn));
#endif AIXTABLET

	hf_cmd.hf_intro[0] = HFINTROESC;
	hf_cmd.hf_intro[1] = HFINTROLBR;
	hf_cmd.hf_intro[2] = HFINTROEX;
	hf_cmd.hf_intro[3] = (sizeof(struct hfqgraphdev) - 3) >> 24 & 0xff;
	hf_cmd.hf_intro[4] = (sizeof(struct hfqgraphdev) - 3) >> 16 & 0xff;
	hf_cmd.hf_intro[5] = (sizeof(struct hfqgraphdev) - 3) >> 8 & 0xff;
	hf_cmd.hf_intro[6] = (sizeof(struct hfqgraphdev) - 3) & 0xff;

	/* Query for dials device */
	hf_cmd.hf_intro[7] = HFQDIALSCH;
	hf_cmd.hf_intro[8] = HFQDIALSCL;
	q.hf_cmd = &hf_cmd;
	q.hf_cmdlen = sizeof(struct hfqgraphdev);
	q.hf_resp = &r;
	q.hf_resplen = sizeof(struct hfdial_lpfk);
	if (ioctl(hftQFD, HFQUERY, &q)) /* returns -1 if dials not available */
	{
	    long error;
	    ioctl(hftQFD, HFQERROR, &error);
	    TRACE(("Query Dials: error code = %ld\n",error));
	    aixInfo.dialiodn = 0;
	}
	else

	aixInfo.dialiodn = hfqdstuff.hf_dialsdev ;

	/* Query for lpfks device */
	hf_cmd.hf_intro[7] = HFQLPFKSCH;
	hf_cmd.hf_intro[8] = HFQLPFKSCL;
	q.hf_cmd = &hf_cmd;
	q.hf_cmdlen = sizeof(struct hfqgraphdev);
	q.hf_resp = &r;
	q.hf_resplen = sizeof(struct hfdial_lpfk);
	if (ioctl(hftQFD, HFQUERY, &q)) /* returns -1 if lpfks not available */
	{
	    long error;
	    ioctl(hftQFD, HFQERROR, &error);
	    TRACE(("Query Dials: error code = %ld\n",error));
	    aixInfo.lpfkiodn = 0;
	}
	else

	aixInfo.lpfkiodn = hfqdstuff.hf_lpfkdev ;

	TRACE(("DIALDEV : dialiodn is %x\n",aixInfo.dialiodn));
	TRACE(("LPFKDEV : lpfkiodn is %x\n",aixInfo.lpfkiodn));
    }

#else
    if (ioctl(hftQFD, HFQDEV, &hfqdstuff)) {
	ErrorF("AIX: unable to query hardware config\n" );
	ErrorF("exiting\n");
    }
    else {
	aixInfo.kbdiodn = hfqdstuff.qdresp.hf_keyiodn ;
	aixInfo.lociodn = hfqdstuff.qdresp.hf_lociodn ;
	aixInfo.dialiodn = hfqdstuff.qdresp.hf_dialsiodn ;
	aixInfo.lpfkiodn = hfqdstuff.qdresp.hf_lpfkiodn ;
    }
#endif

    aixInfo.dpsid =  1 ; /* we have dps */

    switch(device & 0xffff000) {

	case HFT_SKYWAY_ID:     aixInfo.displayid = XDEV_IBM_SKYWAY ;
	                        break;
	case HFT_SKYMONO_ID:    aixInfo.displayid = XDEV_IBM_SKYMONO ;
	                        break;
	case HFT_SABINE_ID :    aixInfo.displayid = XDEV_IBM_SABINE ;
	                        break;
	case HFT_GEMINI_ID:     aixInfo.displayid = XDEV_IBM_GEMINI ;
	                        break;
	case HFT_PEDER_ID:      aixInfo.displayid = XDEV_IBM_PEDER ;
	                        break;
	default:                aixInfo.displayid = XDEV_IBM_GAI ;
    }

#ifdef AIXTABLET
    if ( ioctl(hftQFD , HFQUERY, &phdevquery ) ) {
	long err_ret;
	ioctl(hftQFD, HFQERROR, &err_ret);

	/* err_ret 1801 is response buffer overflow, ignore this error */
	if (err_ret != 1801) {
	        ErrorF("AIX: unable to query physical device\n" );
	        ErrorF("exiting\n");
	}
    }
#endif AIXTABLET

#ifdef AIXV3
    if ( ioctl(hftQFD , HFQUERY, &query3 ) ) {
	ErrorF("AIX: unable to query tablet\n" );
	ErrorF("exiting\n");
    }
#else
    if ( ioctl(hftQFD , HFQUERY, &query3 ) ) {
	ErrorF("AIX: unable to query locator\n" );
	ErrorF("exiting\n");
    }
#endif

#ifdef AIXV3
#ifdef AIXTABLET
    TRACE(("hftQueryHardwareConfig: locattr = %x\n",phdevresp.hf_locattr[0]));
    if ( phdevresp.hf_locattr[0] == HFLOCABS ) {
#else
    TRACE(("hftQueryHardwareConfig: devinfo = %x\n",resp3.loc.hf_devinfo[0]));
    if ( resp3.loc.hf_devinfo[0] & HFLOCABS ) {
#endif AIXTABLET
	/* scale factor = max_cnt * resolution / 1000 */
	int resolution;
	resolution = (resp3.loc.hf_resolution[2] << 8) +
	              resp3.loc.hf_resolution[3];
	aixTabletScaleX = ((resp3.loc.hf_horzmax_cnt[0] << 8)
	                  + resp3.loc.hf_horzmax_cnt[1])
	                  * resolution / 1000;
	aixTabletScaleY = ((resp3.loc.hf_vertmax_cnt[0] << 8)
	                  + resp3.loc.hf_vertmax_cnt[1])
	                  * resolution / 1000;
#else
   if ( resp3.loc.hf_devinfo[0] & HFLOCABS ) {
	aixTabletScaleX = (resp3.loc.hf_horzmax_cnt[0] << 8)
	                  + resp3.loc.hf_horzmax_cnt[1];
	aixTabletScaleY = (resp3.loc.hf_vertmax_cnt[0] << 8)
	                  + resp3.loc.hf_vertmax_cnt[1];
#endif AIXV3
#ifdef AIXTABLET
	if ( phdevresp.hf_locattr[0] == HFLOCREL ) {
	    aixInfo.loctype = DEVMOUSE ;
	    aixInfo.lociodn = aixInfo.mouseiodn ;
	}
	else {
	    aixInfo.loctype = DEVTABLET ;
	    aixInfo.lociodn = aixInfo.tabletiodn ;
	}
#else
	aixInfo.loctype = DEVTABLET ;
#endif AIXTABLET
    }
    else {
	aixInfo.loctype = DEVMOUSE ;
    }
    TRACE(("hftQueryHardwareConfig: loctype = %d\n",aixInfo.loctype));
    aixInfo.kbdid = hftGetKeyboardID();
    aixInfo.inputfd = hftQFD ;
    return;
}
/*------------------------------HFT Code End ----------------------*/
#endif

#endif /* !_IBM_LFT */
