/*
 * $XConsortium: aixEvents.c /main/10 1996/01/18 07:08:22 dpw $
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


#define NEED_EVENTS
#define NEED_REPLIES
#include "X.h"
#include "Xproto.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"
#include "input.h"
#include "inputstr.h"

#include "AIX.h"
#include "aixInput.h"
#include "compiler.h"
#include "ibmIO.h"
#include "ibmMouse.h"
#include "ibmKeybd.h"
#include "ibmScreen.h"
#include "ibmTrace.h"

#include "OSio.h"

#ifdef _IBM_LFT
#include "lftUtils.h"
#else
#include <sys/hft.h>
#include "hftQueue.h"
#endif

#ifdef XKB
#include "XKBsrv.h"
extern Bool noXkbExtension;
#endif

/*-------------------------------------------------------------------------*/
#ifdef AIXEXTENSIONS
#include "AIXproto.h"
#include "AIXext.h"

static int aixTabletSaveX = 1 ;
static int aixTabletSaveY = 1;
int aixTabletScaleX ;
int aixTabletScaleY ;


#ifdef CFBSERVER
extern DevicePtr rtDial ;
extern DevicePtr rtLpfk ;
#endif

#ifndef CFBSERVER /* come frome aixExtHook */
AIXInfoRec      aixInfo  = {0,                          /* kbdid */
	                    0,                          /* display vrmid */
	                    0,                          /* display model */
	                    0,                          /* autoloadmode */
	                   -1,                          /* fd  */
	                    0,                          /* kbd iodn */
	                    0,                          /* loc iodn */
#ifdef AIXTABLET
	                    0,                          /* mouse iodn */
	                    0,                          /* tablet iodn */
#endif
	                    0,                          /* dial iodn */
	                    0,                          /* lpfk iodn */
	                    0,                          /* loctype */
	                   };
#endif

#ifdef AIXTABLET
extern DevicePtr rtTablet ;
#endif

#endif
/*-------------------------------------------------------------------------*/

/***====================================================================***/
/***			Common Routines for both HFT and LFT		***/
/***====================================================================***/
extern  int      screenIsSaved;
extern  int      kbdType ;

#ifdef _IBM_LFT
struct ir_mouse delayedEvent;
#else
static  hftEvent delayedEvent;
#endif

static  int      delayed_left;
static  int      delayed_right;
static  int      delayed_middle;
static  unsigned char lastButtons = 0x00 ;
static  short    lastModKeys;
static  int      pendingX;
static  int      pendingY;

static int  kanjiCapsLockOn = 0;

#define GET_OS_TIME() (GetTimeInMillis())

extern void aixFlushMouse();
extern int aixPtrEvent() ;
extern int aixTabletEvent();
extern int aixDialEvent();
extern int aixLpfkEvent();

void
aixFlushMouse()
{
register        int     x,y;
register        ibmPerScreenInfo        *screenInfo;
	        int     oldScr, newScr, v;
	        int     setCursor;
	        xEvent  e;

    TRACE(("aixFlushMouse\n"));

    if ((pendingX)||(pendingY)) {
	ibmAccelerate(pendingX,pendingY);
	x= AIXCurrentX+pendingX;
	y= AIXCurrentY-pendingY;

	pendingX= 0;
	pendingY= 0;

	oldScr = ibmCurrentScreen;
	newScr = ibmCurrentScreen;
	screenInfo = ibmScreens[ oldScr ];
	setCursor = FALSE;

	while( y<0 || x<0 || y>screenInfo->ibm_ScreenBounds.y2 || x>screenInfo->ibm_ScreenBounds.x2 ) {
	    if( y < 0 ){
	        if( (v=aixWrapUp(newScr)) < 0 ){
	                /* if no wrap this direction */
	            y= 0;
	            setCursor= TRUE;
	        }
	        else{
	            newScr = v;
	            screenInfo = ibmScreens[newScr];
	            y+= screenInfo->ibm_ScreenBounds.y2;
	            setCursor= TRUE;
	        }
	    }
	    else if( y>screenInfo->ibm_ScreenBounds.y2 ) {
	        if( (v=aixWrapDown(newScr)) < 0 ){
	                /* if no wrap this direction */
	            y= screenInfo->ibm_ScreenBounds.y2;
	            setCursor= TRUE;
	        }
	        else{
	            newScr = v;
	            screenInfo = ibmScreens[newScr];
	            y-= screenInfo->ibm_ScreenBounds.y2;
	            setCursor= TRUE;
	        }
	    }

	    if( x < 0 ){
	        if( (v=aixWrapLeft(newScr)) < 0 ){
	                /* if no wrap this direction */
	            x= 0;
	            setCursor= TRUE;
	        }
	        else{
	            newScr = v;
	            screenInfo = ibmScreens[newScr];
	            x+= screenInfo->ibm_ScreenBounds.x2;
	            setCursor= TRUE;
	        }
	    }
	    else if( x>screenInfo->ibm_ScreenBounds.x2 ) {
	        if( (v=aixWrapRight(newScr)) < 0 ){
	                /* if no wrap this direction */
	            x= screenInfo->ibm_ScreenBounds.x2;
	            setCursor= TRUE;
	        }
	        else{
	            newScr = v;
	            screenInfo = ibmScreens[newScr];
	            x-= screenInfo->ibm_ScreenBounds.x2;
	            setCursor= TRUE;
	        }
	    }
	}

	if (setCursor)
	    setCursorPosition(x,y);

	if (oldScr != newScr ) {
	TRACE(("switching screens in ProcessInputEvents\n"));
	NewCurrentScreen( screenInfo->ibm_Screen, x, y ) ;
	}
	XTEST1_STEAL_MOTION_DATA(x, y)

#ifdef SOFTWARE_CURSOR
	miPointerMoveCursor(screenInfo->ibm_Screen, x, y, TRUE) ;
#else
	(* screenInfo->ibm_CursorShow )( x, y ) ;
#endif

	e.u.u.type=                     MotionNotify;
	e.u.keyButtonPointer.rootX=     AIXCurrentX= x;
	e.u.keyButtonPointer.rootY=     AIXCurrentY= y;
	e.u.keyButtonPointer.time=      lastEventTime = GET_OS_TIME();
	pendingX= pendingY= 0;
	(*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
    }
}

#ifdef _IBM_LFT
#define LSHIFT  (KBDUXLSH)
#define RSHIFT  (KBDUXRSH)
#define RALT    (KBDUXRALT)
#define LALT    (KBDUXLALT)
#define CTRL    0
#define MODKEYMASK      (LSHIFT|RSHIFT|RALT|LALT|CTRL)
#define setModKeyState(ms,pe)   \
        ((ms)=((((pe)->kbd_status[0]<<8)|(pe)->kbd_status[1])&MODKEYMASK))
#else
#define LSHIFT  (HFUXLSH)
#define RSHIFT  (HFUXRSH)
#define RALT    (HFUXRALT)
#define LALT    (HFUXLALT)
#define CTRL    0
#define MODKEYMASK      (LSHIFT|RSHIFT|RALT|LALT|CTRL)
#define setModKeyState(ms,pe)   \
	        ((ms)=((((pe)->keStatus[0]<<8)|(pe)->keStatus[1])&MODKEYMASK))
#endif /* !_IBM_LFT */



#ifdef XTESTEXT1
#define FAKEEVENT(ev,key,up)    {\
	(ev)->u.u.detail= (key);\
	(ev)->u.u.type= ((up)?KeyRelease:KeyPress);\
	 XTEST1_STEAL_KEY_EVENT(e) \
	(*ibmKeybd->processInputProc)((ev),(DeviceIntPtr)ibmKeybd,1);\
	}
#elif defined(SOFTWARE_CURSOR)
#define FAKEEVENT(ev,key,up)    {\
	(ev)->u.u.detail= (key);\
	(ev)->u.u.type= ((up)?KeyRelease:KeyPress);\
	miPointerPosition(ibmScreens[ibmCurrentScreen]->ibm_Screen,\
	                  &((ev)->u.keyButtonPointer.rootX),       \
	                  &((ev)->u.keyButtonPointer.rootY)         \
	                  ) ;                                       \
	(*ibmKeybd->processInputProc)((ev),(DeviceIntPtr)ibmKeybd,1);\
	}
#else /* HARDWARE_CURSOR */
#define FAKEEVENT(ev,key,up)    {\
	(ev)->u.u.detail= (key);\
	(ev)->u.u.type= ((up)?KeyRelease:KeyPress);\
	(*ibmKeybd->processInputProc)((ev),(DeviceIntPtr)ibmKeybd,1);\
	}
#endif /* XTESTEXT1 */

void
aixFakeModKeyEvent(changedModKeys)
unsigned short  changedModKeys;
{
xEvent          e;

    TRACE(("aixFakeModKeyEvent(%d)\n"));

    e.u.keyButtonPointer.rootX= AIXCurrentX;
    e.u.keyButtonPointer.rootY= AIXCurrentY;
    e.u.keyButtonPointer.time=  lastEventTime= GET_OS_TIME();

    if (changedModKeys&LSHIFT) {
	FAKEEVENT(&e,Aix_Shift_L,(lastModKeys&LSHIFT));
    }
    if (changedModKeys&RSHIFT) {
	FAKEEVENT(&e,Aix_Shift_R,(lastModKeys&RSHIFT));
    }
    if (changedModKeys&LALT) {
	FAKEEVENT(&e,Aix_Alt_L,(lastModKeys&LALT));
    }
    if (changedModKeys&RALT) {
	FAKEEVENT(&e,Aix_Alt_R,(lastModKeys&RALT));
    }

    return;

}

/***====================================================================***/
/***		Common Routines for both HFT and LFT End		***/
/***====================================================================***/

/***====================================================================***/
/***				HFT Routines				***/
/***====================================================================***/
#ifndef _IBM_LFT
extern void     ibmReactivateScreens(), ibmDeactivateScreens();

void
AIXInitEventHandlers()
{
    extern int aixKbdEvent() ;

    TRACE(("InitEventHandlers()\n"));
#ifdef XTESTEXT1
    xtest_command_key = 0x78;   /* F1 key */
#endif /* XTESTEXT1 */

    if (hftInstallHandler(HFT_LOCATOR,aixPtrEvent)==HFT_ERROR) {
	ErrorF("Couldn't install mouse handler\n");
    }
    if (hftInstallHandler(HFT_KEYBOARD,aixKbdEvent)==HFT_ERROR) {
	ErrorF("Couldn't install keyboard handler\n");
    }
    hftInstallHandler(HFT_EVENT_ARRIVED,HFT_IGNORE);
    if (hftInstallHandler(HFT_GRANTED,ibmReactivateScreens)==HFT_ERROR) {
	ErrorF("Couldn't install grant routine\n");
    }
    if (hftInstallHandler(HFT_RETRACTED,ibmDeactivateScreens)==HFT_ERROR) {
	ErrorF("Couldn't install retract handler\n");
    }
#ifdef AIXEXTENSIONS
    if (hftInstallHandler(HFT_TABLET,aixTabletEvent)==HFT_ERROR) {
	ErrorF("Couldn't install tablet handler\n");
    }
    if (hftInstallHandler(HFT_DIAL,aixDialEvent)==HFT_ERROR) {
	ErrorF("Couldn't install dial handler\n");
    }
    if (hftInstallHandler(HFT_LPFK,aixLpfkEvent)==HFT_ERROR) {
	ErrorF("Couldn't install lpfk handler\n");
    }
#endif
}

void
ProcessInputEvents()
{

#ifdef IBM_SPECIAL_MALLOC
extern int ibmShouldDumpArena;

    if (ibmShouldDumpArena) {
	ibmDumpArena();
    }
#endif /* IBM_SPECIAL_MALLOC */

    TRACE(("ProcessInputEvents (pending=%d)\n",hftPending));

    if ( screenIsSaved == SCREEN_SAVER_ON ) {
	SaveScreens( SCREEN_SAVER_OFF, ScreenSaverReset );
    }
    hftDispatchEvents();
    if (pendingX||pendingY)
	aixFlushMouse();
}


	/*
	 * All of this keyboard stuff needs to
	 * be reorganized anyway (into osKeymap.h and a single keyboard.c)
	 */



/***====================================================================***/

static int
aixKbdEvent(pEv)
hftEvent        *pEv;
{
hftKeyEv        *pKey= &pEv->hftEv.hftKey;
unsigned short  theseModKeys;
DeviceIntPtr    KeyboardPtr;
xEvent          e;
int             key;

    TRACE(("aixKbdEvent (%d)\n",pKey->kePos));

    key = pKey->kePos + AIX_MIN_KEY ;

    if ((pendingX)||(pendingY))
	aixFlushMouse();

    setModKeyState(theseModKeys,pKey);
    if (theseModKeys!=lastModKeys) {
	aixFakeModKeyEvent(theseModKeys^lastModKeys);
    }

    lastModKeys= theseModKeys;

    switch (key) {
	case Aix_Alt_L:
	        {
	        if (kbdType == HF106KBD)        /* special case */
	                break ;
	        else
	                return(1);
	        }
	case Aix_Shift_L:
	case Aix_Shift_R:
	case Aix_Alt_R:
	                return(1);
	case Aix_Backspace:
	        if ((!ibmDontZap)&&(pKey->keStatus[0]&HFUXCTRL)&&
	                                (pKey->keStatus[0]&HFUXALT)) {
	            GiveUp(0);
	        }
	        break;
	default:
	        break;
    }

    if ((!ibmKeyRepeat)&&pKey->keStatus[1]&HFUXRPT)
	return(1);

    e.u.keyButtonPointer.rootX= AIXCurrentX;
    e.u.keyButtonPointer.rootY= AIXCurrentY;
    e.u.keyButtonPointer.time=  lastEventTime= GET_OS_TIME();
    e.u.u.detail= key ;
    if (pKey->keStatus[1]&HFUXRPT)      {
	e.u.u.type= KeyRelease;
	 XTEST1_STEAL_KEY_EVENT(e)
#ifdef SOFTWARE_CURSOR
	miPointerPosition(ibmScreens[ibmCurrentScreen]->ibm_Screen,
	                  &e.u.keyButtonPointer.rootX,
	                  &e.u.keyButtonPointer.rootY
	                  ) ;
#endif
	(*ibmKeybd->processInputProc)(&e,(DeviceIntPtr)ibmKeybd,1);
	e.u.u.type= KeyPress;
    }
    else if (pKey->keStatus[0]&HFUXMAKE)        e.u.u.type= KeyPress;
    else                                        e.u.u.type= KeyRelease;

#ifdef XKB
    if (noXkbExtension) {
#endif
    if (kbdType == HF106KBD) {
	/*
	        On 106 keyboard the Caps Lock key is in the same
	        place as the Left Alt key and is only active when
	        the Right Alt key is down.  To toggle Caps Lock type
	        the sequence Right_Alt down, Left_Alt down, then let
	        the keys up in any order.  The Left_Alt acts like a
	        normal key when Right_Alt is up.
	*/

    KeyboardPtr = (DeviceIntPtr)ibmKeybd;

    if( (key == Aix_Alt_L) && (e.u.u.type == KeyPress) )
    {
	/* if left alt key was just pressed */
	/* if right alt key is down */
	if( theseModKeys & RALT ){
	        if( kanjiCapsLockOn ){
	                /* turn OFF caps lock */
#ifdef CFBSERVER
/* where is is defined ??? */
	            kanjiClearCapsLock();
	            kanjiCapsLockOn = 0;
#endif
	            SetCapsLockLED(0);
	        }
	        else{
	                /* turn ON caps lock */
#ifdef CFBSERVER
	            kanjiSetCapsLock();
	            kanjiCapsLockOn = 1;
#endif
	            SetCapsLockLED(1);
	        }
	}
    }

    /*
     *  toggle num lock key:
     *  ignore releases, toggle on & off on presses
     */

    if( (KeyboardPtr)->key->modifierMap[key] & NumLockMask )
    {
       if( e.u.u.type == KeyRelease )
	        return (1);
       if( KeyboardPtr->key->down[key >> 3] & (1 << (key & 7)) )
       {
	    e.u.u.type = KeyRelease;
	    SetNumLockLED(0);
       } else
	    SetNumLockLED(1);
    }

    } /* end if (kbdType == HF106KBD) */

    if (kbdType == HF101KBD || kbdType == HF102KBD) {

    /*
     *  toggle lock shift keys:
     *  ignore releases, toggle on & off on presses
     */

    KeyboardPtr = (DeviceIntPtr)ibmKeybd;

    if( KeyboardPtr->key->modifierMap[key] & LockMask )
    {
	if( e.u.u.type == KeyRelease )
	     return (1);
	if( KeyboardPtr->key->down[key >> 3] & (1 << (key & 7)) )
	{
	     e.u.u.type = KeyRelease;
	     SetCapsLockLED(0);
	} else
	     SetCapsLockLED(1);
    }

    /*
     *  toggle num lock key:
     *  ignore releases, toggle on & off on presses
     */

    if( KeyboardPtr->key->modifierMap[key] & NumLockMask )
    {
       if( e.u.u.type == KeyRelease )
	        return (1);
       if( KeyboardPtr->key->down[key >> 3] & (1 << (key & 7)) )
       {
	    e.u.u.type = KeyRelease;
	    SetNumLockLED(0);
       } else
	    SetNumLockLED(1);
    }

    }
#ifdef XKB
    }
#endif

	XTEST1_STEAL_KEY_EVENT(e)
#ifdef SOFTWARE_CURSOR
	miPointerPosition(ibmScreens[ibmCurrentScreen]->ibm_Screen,
	                  &(e.u.keyButtonPointer.rootX),
	                  &(e.u.keyButtonPointer.rootY)
	                  ) ;
#endif
    (*ibmKeybd->processInputProc)(&e,(DeviceIntPtr)ibmKeybd,1);

    return(1);
}

/***====================================================================***/

#define NONE    0x00
#define LEFT    Button1
#define MIDDLE  Button2
#define RIGHT   Button3

#define UP      ButtonRelease
#define DOWN    ButtonPress

static int aix3ButtonPtrEvent();


static void
locatorTimeout()
{
aixPtrEvent(&delayedEvent);
return;
}


static int
aixPtrEvent(pIn)
hftEvent        *pIn;
{
extern  int     AIXMouseChordDelay;
hftLocEv        *pEv= &pIn->hftEv.hftLoc;
unsigned char   buttons= pEv->leButtons&(HFT_BUTTONS|HFT_MBUTTON);
xEvent          e;


    TRACE(("aixPtrEvent (%d,%d)\n",pEv->leDeltaX,pEv->leDeltaY));
    pendingX+= pEv->leDeltaX;
    pendingY+= pEv->leDeltaY;

    if (pendingX||pendingY)
	aixFlushMouse();
    e.u.keyButtonPointer.rootX=     AIXCurrentX;
    e.u.keyButtonPointer.rootY=     AIXCurrentY;
    e.u.keyButtonPointer.time=      GET_OS_TIME();

    if (buttons&HFT_MBUTTON) {
    /* We don't care about all the 2 button stuff anymore because
       we know there is a three button mouse.  Therefore, LEFT&RIGHT
       buttons down don't mean middle button( like they had to for a two
       button mouse), they mean left and right button down consecutively.*/
	if (delayed_right || (buttons&HFT_RBUTTON) ) {
	    hftAddTimeout(NULL,0);
	    e.u.u.detail= RIGHT;
	    e.u.u.type=   DOWN;
	    XTEST1_STEAL_MOUSE_EVENT(e)
	    (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	}
	if (delayed_left || (buttons&HFT_LBUTTON) ) {
	    hftAddTimeout(NULL,0);
	    e.u.u.detail= LEFT;
	    e.u.u.type=   DOWN;
	    XTEST1_STEAL_MOUSE_EVENT(e)
	    (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	}
	/* if we had previously generated a middle event from a LEFT&RIGHT
	   and we are waiting for it to go away */
	if (delayed_middle) {
	    hftAddTimeout(NULL,0);
	    e.u.u.detail= MIDDLE;
	    e.u.u.type=   UP;
	    XTEST1_STEAL_MOUSE_EVENT(e)
	    (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	}
	e.u.u.detail= MIDDLE;
	e.u.u.type=   DOWN;
	XTEST1_STEAL_MOUSE_EVENT(e)
	(*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	lastButtons = buttons;
	if (hftInstallHandler(HFT_LOCATOR,aix3ButtonPtrEvent)==HFT_ERROR)
	    ErrorF("Couldn't install three button mouse handler\n");
	return(1);
    }


    switch(lastButtons)
    {
	case NONE  :
	     switch(buttons)
	     {
	         case NONE  :
	              break;
	         case HFT_LBUTTON  :
	              delayed_left = TRUE;
	              break;
	         case HFT_RBUTTON  :
	              delayed_right = TRUE;
	              break;
	         case HFT_BUTTONS  :
	              e.u.u.detail= MIDDLE;
	              e.u.u.type=   DOWN;
		      XTEST1_STEAL_MOUSE_EVENT(e)
	              (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	              break;
	      }
	     break;
	case HFT_LBUTTON  :
	     switch(buttons)
	     {
	         case NONE  :
	              if (delayed_left) {
	                  hftAddTimeout(NULL,0);
	                  e.u.u.detail= LEFT;
	                  e.u.u.type=   DOWN;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	                  delayed_left = FALSE;
	              }
	              if (delayed_middle) {
	                  e.u.u.detail= MIDDLE;
	                  e.u.u.type=   UP;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	                  delayed_middle = FALSE;
	              } else {
	                  e.u.u.detail= LEFT;
	                  e.u.u.type=   UP;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	              }
	              break;
	         case HFT_LBUTTON  :
	              if (delayed_left) {
	                  hftAddTimeout(NULL,0);
	                  e.u.u.detail= LEFT;
	                  e.u.u.type=   DOWN;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	                  delayed_left = FALSE;
	              }
	              break;
	         case HFT_RBUTTON  :
	              if (delayed_left) {
	                  hftAddTimeout(NULL,0);
	                  e.u.u.detail= LEFT;
	                  e.u.u.type=   DOWN;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	                  delayed_left = FALSE;
	              }
	                  e.u.u.detail= LEFT;
	                  e.u.u.type=   UP;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	              delayed_right = TRUE;
	              break;
	         case HFT_BUTTONS  :
	              if (delayed_left) {
	                  hftAddTimeout(NULL,0);
	                  e.u.u.detail= MIDDLE;
	                  e.u.u.type=   DOWN;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	                  delayed_left = FALSE;
	              } else if (!delayed_middle) {
	                  e.u.u.detail= LEFT;
	                  e.u.u.type=   UP;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	                  e.u.u.detail= MIDDLE;
	                  e.u.u.type=   DOWN;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	              }
	              break;
	      }
	     break;
	case HFT_RBUTTON  :
	     switch(buttons)
	     {
	         case NONE  :
	              if (delayed_right) {
	                  hftAddTimeout(NULL,0);
	                  e.u.u.detail= RIGHT;
	                  e.u.u.type=   DOWN;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	                  delayed_right = FALSE;
	              }
	              if (delayed_middle) {
	                  e.u.u.detail= MIDDLE;
	                  e.u.u.type=   UP;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	                  delayed_middle = FALSE;
	              } else {
	                  e.u.u.detail= RIGHT;
	                  e.u.u.type=   UP;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	              }
	              break;
	         case HFT_LBUTTON  :
	              if (delayed_right) {
	                  hftAddTimeout(NULL,0);
	                  e.u.u.detail= RIGHT;
	                  e.u.u.type=   DOWN;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	                  delayed_right = FALSE;
	              }
	              e.u.u.detail= RIGHT;
	              e.u.u.type=   UP;
		      XTEST1_STEAL_MOUSE_EVENT(e)
	              (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	              delayed_left = TRUE;
	              break;
	         case HFT_RBUTTON  :
	              if (delayed_right) {
	                  hftAddTimeout(NULL,0);
	                  e.u.u.detail= RIGHT;
	                  e.u.u.type=   DOWN;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	                  delayed_right = FALSE;
	              }
	              break;
	         case HFT_BUTTONS  :
	              if (delayed_right) {
	                  hftAddTimeout(NULL,0);
	                  e.u.u.detail= MIDDLE;
	                  e.u.u.type=   DOWN;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	                  delayed_right = FALSE;
	              } else if (!delayed_middle) {
	                  e.u.u.detail= RIGHT;
	                  e.u.u.type=   UP;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	                  e.u.u.detail= MIDDLE;
	                  e.u.u.type=   DOWN;
		          XTEST1_STEAL_MOUSE_EVENT(e)
	                  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	              }
	              break;
	      }
	     break;
	case HFT_BUTTONS  :
	     switch(buttons)
	     {
	         case NONE  :
	              e.u.u.detail= MIDDLE;
	              e.u.u.type=   UP;
		      XTEST1_STEAL_MOUSE_EVENT(e)
	              (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	              break;
	         case HFT_LBUTTON  :
	         case HFT_RBUTTON  :
	              delayed_middle = TRUE;
	              break;
	         case HFT_BUTTONS  :
	              break;
	      }
	      break;
    }

    lastButtons = buttons;

    if (delayed_left || delayed_right) {
	delayedEvent = *pIn;
	delayedEvent.hftEv.hftLoc.leDeltaX = 0;
	delayedEvent.hftEv.hftLoc.leDeltaY = 0;
	hftAddTimeout(locatorTimeout,AIXMouseChordDelay);
    }
    return(1);
}



/***====================================================================***/

static int
aix3ButtonPtrEvent(pIn)
hftEvent        *pIn;
{
extern  int     AIXMouseChordDelay;
hftLocEv        *pEv= &pIn->hftEv.hftLoc;
unsigned char   buttons= pEv->leButtons&(HFT_BUTTONS|HFT_MBUTTON);
unsigned char   changed;
xEvent          e;

    TRACE(("aix3ButtonPtrEvent (%d,%d)\n",pEv->leDeltaX,pEv->leDeltaY));
    pendingX+= pEv->leDeltaX;
    pendingY+= pEv->leDeltaY;

    if (pendingX||pendingY)
	aixFlushMouse();

    e.u.keyButtonPointer.rootX=     AIXCurrentX;
    e.u.keyButtonPointer.rootY=     AIXCurrentY;
    e.u.keyButtonPointer.time=      GET_OS_TIME();

    changed = buttons ^ lastButtons;

    if (changed & HFT_BUTTON1) {
	if (buttons & HFT_BUTTON1)      e.u.u.type = DOWN;
	    else                        e.u.u.type = UP;
	e.u.u.detail= LEFT;
#ifdef  XTESTEXT1
	if (!on_steal_input ||
	    XTestStealKeyData(e.u.u.detail, e.u.u.type, XE_MOUSE,
	                      AIXCurrentX, AIXCurrentY))
#endif /* XTESTEXT1 */
	(*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
    }
    if (changed & HFT_BUTTON3) {
	if (buttons & HFT_BUTTON3)      e.u.u.type = DOWN;
	    else                        e.u.u.type = UP;
	e.u.u.detail= RIGHT;
#ifdef  XTESTEXT1
	if (!on_steal_input ||
	    XTestStealKeyData(e.u.u.detail, e.u.u.type, XE_MOUSE,
	                      AIXCurrentX, AIXCurrentY))
#endif /* XTESTEXT1 */
	(*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
    }
    if (changed & HFT_BUTTON2) {
	if (buttons & HFT_BUTTON2)      e.u.u.type = DOWN;
	    else                        e.u.u.type = UP;
	e.u.u.detail= MIDDLE;
#ifdef  XTESTEXT1
	if (!on_steal_input ||
	    XTestStealKeyData(e.u.u.detail, e.u.u.type, XE_MOUSE,
	                      AIXCurrentX, AIXCurrentY))
#endif /* XTESTEXT1 */
	(*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
    }

    lastButtons = buttons;
    return(1);
}


#ifdef AIXEXTENSIONS
static int
aixTabletEvent(pIn)
hftEvent        *pIn;
{
hftLocEv        *pEv= &pIn->hftEv.hftLoc;
unsigned char   tmpbuttons,buttons ;
xEvent          e;

#ifdef AIXTABLET

xExtEvent       xe;

	TRACE(("aixTabletEvent()\n"));
	TRACE(("aixTabletEvent: Event type = %d\n", pIn->hftEvType));
	TRACE(("aixTabletEvent: Buttons    = %d\n", pEv->leButtons));
	TRACE(("aixTabletEvent: DeltaX     = %d\n", pEv->leDeltaX));
	TRACE(("aixTabletEvent: DeltaY     = %d\n", pEv->leDeltaY));

	xe.u.Tablet.rootX = AIXCurrentX;
	xe.u.Tablet.rootY = AIXCurrentY;
	xe.u.Tablet.deviceid = DEVTABLET;
	xe.u.Tablet.axes_count = 2;             /* always 2 for tablet */
	xe.u.Tablet.first_axes = 0;             /* 0 - x  1 - y */
	xe.u.Tablet.axes_data[0] = pEv->leDeltaX;
	xe.u.Tablet.axes_data[1] = pEv->leDeltaY;
	lastEventTime = xe.u.Tablet.time = GET_OS_TIME();

	xe.u.u.type = DeviceMotion;
	xe.u.u.detail = pEv->leButtons & HFT_TABLET_BUTTONS;

	if (rtTablet->on)
	    (*(rtTablet->processInputProc))((xEvent *)&xe, rtTablet,1);

#else /* AIXTABLET */

#ifdef AIXV3
	/* check if cursor has moved or different button is pressed */
	if ((aixTabletSaveX == pEv->leDeltaX) &&
	    (aixTabletSaveY == pEv->leDeltaY) &&
	    ((pEv->leButtons & HFT_TABLET_BUTTONS) == lastButtons))
	        return (1) ;

	buttons = pEv->leButtons & HFT_TABLET_BUTTONS ;
#else

	/* bug compatible */

	if (pEv->leButtons == 3 )
	        return ;

	buttons = pEv->leButtons >> 3 ;

	switch (buttons)
	{
	case  1 :
	        buttons = 0x80 ;
	        break ;
	case  2 :
	        buttons = 0x40 ;
	        break ;
	case  3 :
	        buttons = 0x20 ;
	        break ;
	case  4 :
	        buttons = 0x10 ;
	        break ;
	}
#endif

	if (lastButtons != (buttons & HFT_TABLET_BUTTONS)) {
	    if (lastButtons) {
	        e.u.u.type = ButtonRelease;
	        buttons = 0x00 ;
	        tmpbuttons = lastButtons;
	    } else {
	        e.u.u.type = ButtonPress;
	        tmpbuttons = buttons;
	    }

	    lastButtons = buttons & HFT_TABLET_BUTTONS ;

	    switch (tmpbuttons) {
	        case  HFT_BUTTON1 : e.u.u.detail = Button1;  break;
	        case  HFT_BUTTON2 : e.u.u.detail = Button2;  break;
	        case  HFT_BUTTON3 : e.u.u.detail = Button3;  break;
	        case  HFT_BUTTON4 : e.u.u.detail = Button4;  break;
	    }
	}
	else
	{
	    e.u.u.detail= 0 ;
	    e.u.u.type=  MotionNotify;
	}

	aixFlushTablet(pEv->leDeltaX,pEv->leDeltaY);

	e.u.keyButtonPointer.rootX=     AIXCurrentX;
	e.u.keyButtonPointer.rootY=     AIXCurrentY;
	e.u.keyButtonPointer.time=      GET_OS_TIME();

	(*(ibmPtr->processInputProc))(&e,ibmPtr,1);
#endif /* AIXTABLET */

	return (1) ;
}


static int
aixDialEvent (pIn)
hftEvent        *pIn;
{
    xExtEvent   xe;
    hftDialEv   *pEv = &pIn->hftEv.hftDial;

    xe.u.DialLpfk.rootX =  AIXCurrentX;
    xe.u.DialLpfk.rootY =  AIXCurrentY;
    lastEventTime = xe.u.DialLpfk.time = GET_OS_TIME();

    xe.u.u.type = DialRotate;
    xe.u.u.detail = (char) pEv->deDialNo;
    xe.u.DialLpfk.value = (char) pEv->deDelta;

#ifdef CFBSERVER
    if (rtDial->on)
	(*(rtDial->processInputProc))((xEvent *)&xe, rtDial,1);
#endif

    return (1);
}


static int
aixLpfkEvent (pIn)
hftEvent        *pIn;
{
    xExtEvent   xe;
    hftLPFKEv   *pEv = &pIn->hftEv.hftLpfk;

    xe.u.DialLpfk.rootX =  AIXCurrentX;
    xe.u.DialLpfk.rootY =  AIXCurrentY;
    lastEventTime = xe.u.DialLpfk.time = GET_OS_TIME();

    xe.u.u.type = LPFKeyPress ;
    xe.u.u.detail = pEv->lpfkeKeyNo ;

#ifdef CFBSERVER
    if (rtLpfk->on)
	(*(rtLpfk->processInputProc))((xEvent *)&xe, rtLpfk,1);
#endif

    return (1);
}
#endif

/***====================================================================***/
/***			HFT Routines End				***/
/***====================================================================***/

#else 

/***====================================================================***/
/***			LFT Routines Begin				***/
/***====================================================================***/

#include <signal.h>

/* Globals for Input Handling */

InputDevPrivate lftInputInfo[2]; /* This gets initialized during the
				  * mouse and keyboard proc init 
				  */
struct inputring *lft_input_ring = NULL; 

static void PollHandler();
static void QuitHandler();
static void MsgHandler();

static void
SetupSignals()
{
    struct  sigaction   svquit;
    struct  sigaction   svmessage;
    struct  sigaction   kskap;
    struct  sigaction   oldsig;


    /* handler for SIGMSG */

    svmessage.sa_handler = MsgHandler;
    SIGINITSET(svmessage.sa_mask);
    SIGADDSET(svmessage.sa_mask,SIGMSG);
    svmessage.sa_flags = SA_RESTART ;
    sigaction(SIGMSG,&svmessage, &oldsig);

    /* handler for SIGQUIT */

    svquit.sa_handler=  QuitHandler;
    SIGINITSET(svquit.sa_mask);
    SIGADDSET(svquit.sa_mask,SIGQUIT);
    svquit.sa_flags = SA_RESTART ;
    sigaction(SIGQUIT,&svquit,&oldsig);


    /* Handle SIGKAP for kill-sequence from keyboard dd */

    kskap.sa_handler=  PollHandler;
    SIGINITSET(kskap.sa_mask);
    SIGADDSET(kskap.sa_mask, SIGKAP);
    kskap.sa_flags = SA_RESTART ;
    sigaction(SIGKAP,&kskap,&oldsig);

    return ;
}


static void
QuitHandler()
{
    /* using sigaction, no re-arm handler for SIGQUIT. */
    return;
}


static void
MsgHandler()
{
    /* using sigaction, no need to re-arm the SIGMSG handler. */
    return;
}



static void
PollHandler()
{
    extern int aixPollPending;
    TRACE(("Enter PollHandler. Contrl-Alt-Backspace received\n"));

    /* SIGKAP handler for X Server. Occurred when keyboard dd found
     * a kill sequence from keyboard. (<ctrl><alt><backspace> is
     * default kill sequence.)
     */
    aixPollPending = TRUE;
}


void
AIXInitEventHandlers()
{
#ifdef XTESTEXT1
    extern KeyCode xtest_command_key;
    xtest_command_key = 0x78;   /* F1 key */
#endif /* XTESTEXT1 */
    if (lft_input_ring == NULL) {
        if (!(lft_input_ring = (struct inputring *) malloc(INPUT_RING_SIZE))) {
            FatalError("Input ring allocation failed!");
            exit(1);
        }
    }

    lft_input_ring->ir_notifyreq = IRSIGALWAYS; /* notify new event */
    lft_input_ring->ir_overflow = IROFCLEAR;
    lft_input_ring->ir_size = INPUT_RING_SIZE - sizeof(struct inputring);
    lft_input_ring->ir_head = (caddr_t)lft_input_ring + 
					sizeof(struct inputring);
    lft_input_ring->ir_tail = lft_input_ring->ir_head;

    /* register inputcheck for all devices.
     * Server maintains 1 input ring for all devices.
     */
    SetInputCheck((long*)&lft_input_ring->ir_head, (long*)&lft_input_ring->ir_tail);
    SetupSignals();
	

}

#define COPY_RAW_EVENT(src, size_in_bytes)                      \
{                                                               \
    int i;                                                      \
    sink = &raw_ev[0];                                   	\
    /* Copy report structure into a contiguous buffer */        \
    for (i = 0; i < size_in_bytes; i++)              		\
    {                                                           \
        *sink = *src;                                           \
        sink++;                                                 \
        src++;                                                  \
                                                                \
        /* Does this wrap around for a complete event? */   	\
        if (src >= end_of_input_ring)			        \
            src = ((u_char *) ir) + sizeof(struct inputring);   \
     }                                                          \
}

void
ProcessInputEvents()
{
    u_char raw_ev[50];		/* to copy out event from ring */
    u_char *pRawEvent;
    struct inputring *ir = lft_input_ring;
    u_char *end_of_input_ring = ((u_char *) ir) + INPUT_RING_SIZE;
    u_char *sink, *head;
    int ev_size, ev_type;
    InputDevPrivate *pPriv = &lftInputInfo[KEYBOARD_ID];
    extern int aixPollPending;
    

    TRACE(("Enter ProcessInputEvents head=0x%x, tail=0x%x\n",ir->ir_head,ir->ir_tail))

    if ( screenIsSaved == SCREEN_SAVER_ON ) {
	SaveScreens( SCREEN_SAVER_OFF, ScreenSaverReset );
    }
    /**********************************************************
    *  If input ring is overflow, reset pointers and ir_flag.
    **********************************************************/

    if (ir->ir_overflow == IROVERFLOW) 
    {
	 TRACE(("InputRing overflow\n"));
	 ioctl(pPriv->fd, KSRFLUSH, NULL);
	 return;
    }

    head = (u_char *) ir->ir_head;

    /******************************************************************
    *  Check lft input ring for new events.
    *  It is assumed that there are no partial events on the ring.
    *  We don't use a variable to hold ir_tail, because the lft
    *  could be adding stuff to the queue while we are processing it.
    ******************************************************************/

    while (head != (u_char *)ir->ir_tail)
    {
	/**********************************************************
      	*  Get size and type of next event in buffer.
        *  The event can be keybd, tablet, mouse, lpfk, or dial.
	*
	*  Then get the event itself.
	*
	*  Don't forget to check to see whether or not these
	*  structures wrap around to the beginning of the input 
	*  ring!
        **********************************************************/

	if (head < (u_char *)ir->ir_tail)
	{
	    /************************************************************
	    *  Everything in the queue is contiguous!  Grab the type 
	    *  and size, and then point to the event in the ring buffer 
	    ************************************************************/
      	    ev_size = ((struct ir_report *) head)->report_size;
      	    ev_type = ((struct ir_report *) head)->report_id;
	    pRawEvent = head;
	    head += ev_size;
	}
	else
	{
	    /***************************************************************
	    *  The event list wraps to the beginning of the circular queue.
	    *  Check to see if this particular event wraps around.
	    ***************************************************************/
	    if ((head + sizeof(struct ir_report)) < end_of_input_ring)
	    {
		/**********************************
		*  Contiguous -- grab the info
		**********************************/
      	    	ev_size = ((struct ir_report *) head)->report_size;
      	    	ev_type = ((struct ir_report *) head)->report_id;
	    }
	    else
	    {
		/**************************************************
		*  Wraps -- copy to contiguous temporary location
		**************************************************/
      	        u_char *tmp_head = head;
	        COPY_RAW_EVENT (tmp_head, sizeof(struct ir_report));
      	    	ev_size = ((struct ir_report *) raw_ev)->report_size;
      	    	ev_type = ((struct ir_report *) raw_ev)->report_id;
	    }

	    if ((head + ev_size) < end_of_input_ring)
      	    {
		/************************************************
	        *  Contiguous -- point to it in the ring buffer 
		************************************************/
	    	pRawEvent = head;
	    	head += ev_size;
      	    }
      	    else
      	    {
		/*************************************************
		*  Wraps -- copy to contiguous temporary location
		*************************************************/
	        COPY_RAW_EVENT (head, ev_size);
	        pRawEvent = &raw_ev[0];
      	    }
	}
	/**************************************************************
      	*  Get the next LFT event, and process it to X format.
       	*  Call the specific event handler function.
	**************************************************************/

      	(*lftInputInfo[ev_type].eventHandler) (pRawEvent);

	/**************************************************************
      	*  Increase the head pointer to the next event location
	**************************************************************/

      	ir->ir_head = (caddr_t) head;

	/**************************************************************
      	*  Detect <Ctrl><alt><Backspace> key sequence by checking
       	*  pollPending, (pollPending get set in PollHandler) 
       	*  and respond to keyboard.
	**************************************************************/

      	if (aixPollPending) 
	{
	    ioctl(pPriv->fd, KSKAPACK);
	    aixPollPending = FALSE;
      	}
    } /* while more events */

}

/*
 *  The aixMouse3Event is modified from 3.x aix3ButtonsPtrEvents.
 *  This is to process events for 3 buttons mouse which is available
 *  on most RISC6000 platforms.
 */

void
aixMouse3Event(xev)
    char *xev;		/* raw mouse event from input ring */
{
    uchar   buttons;		/* status of mouse buttons */
    uchar   changed;
    int     delta_x;
    int     delta_y;
    xEvent  e;
    struct  ir_mouse *pEv = (struct ir_mouse *) xev;

    TRACE(("Enter aixMouse3Event (dx=%d, dx=%d)\n", pEv->mouse_deltax,pEv->mouse_deltay));

    buttons  = pEv->mouse_status&(MOUSEBUTTONS|MOUSEBUTTON2);
    delta_x = pEv->mouse_deltax;
    delta_y = pEv->mouse_deltay;
    ibmAccelerate (delta_x, delta_y);

    pendingX += delta_x;
    pendingY += delta_y;

    if (buttons == lastButtons)
    {
	    aixFlushMouse ();
    }
    else
    {
	/*********************************************************
	*  The buttons have changed since last time.  Flush the 
	*  Motion event, and then send the button event.
	*********************************************************/
	aixFlushMouse ();

    	e.u.keyButtonPointer.rootX = AIXCurrentX;
    	e.u.keyButtonPointer.rootY = AIXCurrentY;
    	e.u.keyButtonPointer.time  = pEv->mouse_header.report_time;

    	changed = buttons ^ lastButtons;

    	if (changed & MOUSEBUTTON1) 
	{
	    if (buttons & MOUSEBUTTON1) e.u.u.type = DOWN;
	    else                        e.u.u.type = UP;
	    e.u.u.detail= LEFT;
	    XTEST1_STEAL_MOUSE_EVENT(e);
            (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
    	}
    	if (changed & MOUSEBUTTON3) 
	{
	    if (buttons & MOUSEBUTTON3) e.u.u.type = DOWN;
	    else                        e.u.u.type = UP;
	    e.u.u.detail= RIGHT;
	    XTEST1_STEAL_MOUSE_EVENT(e)
            (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
    	}
    	if (changed & MOUSEBUTTON2) 
	{
	    if (buttons & MOUSEBUTTON2) e.u.u.type = DOWN;
	    else                        e.u.u.type = UP;
	    e.u.u.detail= MIDDLE;
	    XTEST1_STEAL_MOUSE_EVENT(e)
            (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
    	}
    	lastButtons = buttons;
    }
}


/**
 **  The aixMouse2Event is modified from 3.x aix2ButtonsPtrEvents.
 **  This is to process events for 2 buttons mouse which is required
 **  for Power PC.
 **/

int    TimeoutCount = 0;
#define  SET_TIMEOUT(tout) {TimeoutCount=(tout);}


void
aixMouse2Event(xev)
    char *xev;			/* raw mouse event from input ring */
{
    xEvent  e;
    uchar   buttons;		/* status of mouse buttons */
    struct  ir_mouse *pEv = (struct ir_mouse *)xev;
    int     delta_x, delta_y;
    extern  int     AIXMouseChordDelay;



    TRACE(("Enter aixMouse2Event raw=0x%x, (dx=%d, dx=%d, pendingX=%d, pendingY=%d)\n",xev, pEv->mouse_deltax,pEv->mouse_deltay, pendingX, pendingY));



    if (pEv->mouse_status & MOUSEBUTTON2)
	return;	 /* sanity check for middle button! */

    buttons   = (pEv->mouse_status & MOUSEBUTTONS);

    delta_x = pEv->mouse_deltax;
    delta_y = pEv->mouse_deltay;
    ibmAccelerate (delta_x, delta_y);

    pendingX += delta_x;
    pendingY += delta_y;

    if (pendingX||pendingY)
	aixFlushMouse();


    /* if no button event recorded previous or currently,      */
    /* then this is just a mouse motion event which has been   */
    /* processed by aixFlushMouse. No need to proceed further. */

    if ((lastButtons == 0) && (buttons == 0))
	return;


    e.u.keyButtonPointer.rootX =  AIXCurrentX;
    e.u.keyButtonPointer.rootY =  AIXCurrentY;
    e.u.keyButtonPointer.time  =  pEv->mouse_header.report_time;


    /* process current button according to previous and */
    /* current button events. */

    if (lastButtons == NONE)
    {
	/* this is brand new event. If it's either left/right, */
	/* then set delayed event. Or if both left/right occurred */
	/* at once, then it's an intended middle button. */

	switch (buttons)
	{
	   case NONE :
		/* MotionNotify event with NO button down */
		break;
	   case MOUSEBUTTON1 :			/* left */
		delayed_left = TRUE;
		break;
	   case MOUSEBUTTON3 :			/* right */
		delayed_right = TRUE;
		break;
	   case MOUSEBUTTONS :			/* middle */
		e.u.u.detail = MIDDLE;
		e.u.u.type =   DOWN;
		XTEST1_STEAL_MOUSE_EVENT(e);
            	(*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		break;
	}
    }
    else if (lastButtons == MOUSEBUTTON1)
    {

	switch (buttons)
	{
	   case NONE :
		if (delayed_left) {
		   /* left button released before time out */
		   SET_TIMEOUT(0);
		   e.u.u.detail= LEFT;
		   e.u.u.type=   DOWN;
		   XTEST1_STEAL_MOUSE_EVENT(e);
       		   (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		   delayed_left = FALSE;
		}

		if (delayed_middle) {
		   e.u.u.detail = MIDDLE;
		   e.u.u.type =   UP;
		   XTEST1_STEAL_MOUSE_EVENT(e);
       		   (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		   delayed_middle = FALSE;
		}
		else {
		   /* for button release, status mask == NONE */
		   e.u.u.detail = LEFT;
		   e.u.u.type =   UP;
		   XTEST1_STEAL_MOUSE_EVENT(e);
       		   (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		}
		break;

	   case MOUSEBUTTON1 :
		if (delayed_left) {
		   SET_TIMEOUT(0);
		   e.u.u.detail = LEFT;
		   e.u.u.type =   DOWN;
		   XTEST1_STEAL_MOUSE_EVENT(e);
       		   (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		   delayed_left = FALSE;
		}
		break;

	   case MOUSEBUTTON3 :

		/* Receive right button while left button is down. */
		/* Process left button up/down event at once because */
		/* lastButtons value will be lost afterward */

		if (delayed_left) {
		   SET_TIMEOUT(0);
		   e.u.u.detail = LEFT;
		   e.u.u.type =   DOWN;
		   XTEST1_STEAL_MOUSE_EVENT(e);
       		   (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		   delayed_left = FALSE;
		}

		e.u.u.detail = LEFT;
		e.u.u.type =   UP;
		XTEST1_STEAL_MOUSE_EVENT(e);
       		(*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		delayed_right = TRUE;
		break;

	   case MOUSEBUTTONS :
		if (delayed_left) {
		   SET_TIMEOUT(0);
		   e.u.u.detail = MIDDLE;
		   e.u.u.type =   DOWN;
		   XTEST1_STEAL_MOUSE_EVENT(e);
       		   (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		   delayed_left = FALSE;
		}
		else if (!delayed_middle) {
		   e.u.u.detail= LEFT;
		   e.u.u.type=   UP;
		   XTEST1_STEAL_MOUSE_EVENT(e);
       		   (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);

		   e.u.u.detail = MIDDLE;
		   e.u.u.type =   DOWN;
		   XTEST1_STEAL_MOUSE_EVENT(e);
       		   (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		}
		break;

	  } /* switch */
     }
     else if (lastButtons == MOUSEBUTTON3)
     {
	  switch(buttons)
	  {
	     case NONE :
		  if (delayed_right) {
		     /* right button released before time out */
		     SET_TIMEOUT(0);
		     e.u.u.detail= RIGHT;
		     e.u.u.type=   DOWN;
		     XTEST1_STEAL_MOUSE_EVENT(e);
       		     (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		     delayed_right = FALSE;
		  }

		  if (delayed_middle) {
		     e.u.u.detail= MIDDLE;
		     e.u.u.type=   UP;
		     XTEST1_STEAL_MOUSE_EVENT(e);
       		     (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		     delayed_middle = FALSE;
		  }
		  else {
		     /* right button released after time out */
		     e.u.u.detail= RIGHT;
		     e.u.u.type=   UP;
		     XTEST1_STEAL_MOUSE_EVENT(e);
       		     (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		  }
		  break;

	     case MOUSEBUTTON1 :

		/* Receive right button while left button is down. */
		/* Process left button up/down event at once because */
		/* lastButtons value will be lost afterward */

		  if (delayed_right) {
		     SET_TIMEOUT(0);
		     e.u.u.detail= RIGHT;
		     e.u.u.type=   DOWN;
		     XTEST1_STEAL_MOUSE_EVENT(e);
       		     (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		     delayed_right = FALSE;
		  }

		  e.u.u.detail= RIGHT;
		  e.u.u.type=   UP;
		  XTEST1_STEAL_MOUSE_EVENT(e);
       		  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		  delayed_left = TRUE;
		  break;

	     case MOUSEBUTTON3 :
		  if (delayed_right) {
		      SET_TIMEOUT(0);
		      e.u.u.detail= RIGHT;
		      e.u.u.type=   DOWN;
		      XTEST1_STEAL_MOUSE_EVENT(e);
       		      (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		      delayed_right = FALSE;
		  }
		  break;

	     case MOUSEBUTTONS :
		  if (delayed_right) {
			SET_TIMEOUT(0);
			e.u.u.detail= MIDDLE;
			e.u.u.type=   DOWN;
			XTEST1_STEAL_MOUSE_EVENT(e);
       		        (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
			delayed_right = FALSE;
		  }
		  else if (!delayed_middle) {
			e.u.u.detail= RIGHT;
			e.u.u.type=   UP;
			XTEST1_STEAL_MOUSE_EVENT(e);
       		        (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);

			e.u.u.detail= MIDDLE;
			e.u.u.type=   DOWN;
			XTEST1_STEAL_MOUSE_EVENT(e);
       		        (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		  }
		  break;

	   } /* switch */
     }
     else if (lastButtons == MOUSEBUTTONS)
     {
	  switch(buttons)
	  {
	     case NONE :
		  e.u.u.detail= MIDDLE;
		  e.u.u.type=   UP;
		  XTEST1_STEAL_MOUSE_EVENT(e);
       		  (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
		  break;
	     case MOUSEBUTTON1 :
	     case MOUSEBUTTON3 :
		  delayed_middle = TRUE;
		  break;
	     case MOUSEBUTTONS :
		  break;

	  } /* switch */
    } /* if */

    lastButtons = buttons;


    /**  Receiving a left/right button, set the mouse chord. If
     **     1) timeout out, then this is a left/right button event,
     **  else
     **     2) if opposite button received within the time out
     **  period, then this is a intended middle button from user.
     **/


    if (delayed_left || delayed_right) {
	delayedEvent = *pEv;
	delayedEvent.mouse_deltax = 0;
	delayedEvent.mouse_deltay = 0;
	SET_TIMEOUT(AIXMouseChordDelay);
    }

    return;
}

static void
SetCapsLockState(keyc)
KeyClassPtr keyc ;
{
    keyc->state |= LockMask;
}

static void
ClearCapsLockState(keyc)
KeyClassPtr keyc ;
{
    keyc->state &= ~(LockMask);
}


void
aixKbdEvent(xev)
char *xev;		/* raw keyboard event from ring */
{
ushort		theseModKeys;
DeviceIntPtr    KeybdPtr = (DeviceIntPtr)ibmKeybd;
KeyClassPtr     keyc  = KeybdPtr->key;
xEvent		e;
int             key;
CARD8* 		pModMap = KeybdPtr->key->modifierMap;
KeySymsPtr      curKeySyms = &KeybdPtr->key->curKeySyms;
int 		map_width  = curKeySyms->mapWidth;
int             min_key_code = curKeySyms->minKeyCode;
Bool 		capslock_toggle = FALSE;
struct ir_kbd   *pKey = (struct ir_kbd *)xev;
extern 		CARD8 aixkillMap[];
extern 		uchar aixkillMasks[];
extern Bool    ibmDontZap;

InputDevPrivate *pPriv = &lftInputInfo[KEYBOARD_ID];

    TRACE(("Enter aixKeybdEvent raw_ev=0x%x, key_pos=%d\n", xev, pKey->kbd_keypos));


    key = pKey->kbd_keypos + AIX_MIN_KEY ;

#ifdef XKB
    if (noXkbExtension) {
#endif
    setModKeyState(theseModKeys, pKey);
    if (theseModKeys!=lastModKeys)
	 aixFakeModKeyEvent(theseModKeys^lastModKeys);

    lastModKeys= theseModKeys;

    switch (key) {
	case Aix_Caps_Lock:	    /* key 0x38 */
		 if(pModMap[key] & LockMask) 
			capslock_toggle = TRUE;
		break;
	case Aix_Alt_L:     /* key 0x68 */
		 if(pModMap[key] & LockMask) 
			capslock_toggle = TRUE;
		break;

	case Aix_Shift_L:  
        case Aix_Shift_R:
        case Aix_Alt_R:
		return;

	default:
		break;
    }
#ifdef XKB
    }
#endif

    /*  Checking if key is the *Kill* keyboard sequence (The default
     *  is Ctrl-Alt-BackSpace). In 4.x, it might be necessary for
     *  application (cozy) to configure this.
     */

    if ((!ibmDontZap) && (key==aixkillMap[2]))
    {
	/*
	 * The checking order could be changed in the future ?
	 */

	if ((pKey->kbd_status[0] & aixkillMasks[0]) &&	/* Ctrl */
	    (pKey->kbd_status[1] & aixkillMasks[1]))	/* Alt */
	    GiveUp(0);
    }


    if ((!ibmKeyRepeat)&&pKey->kbd_status[1]&KBDUXRPT)
	return;


    e.u.keyButtonPointer.rootX=	AIXCurrentX;
    e.u.keyButtonPointer.rootY=	AIXCurrentY;
    e.u.keyButtonPointer.time=	pKey->kbd_header.report_time;
    e.u.u.detail= key ;

    if (pKey->kbd_status[1]&KBDUXRPT)      {
	e.u.u.type= KeyRelease;
	XTEST1_STEAL_KEY_EVENT(e)
        (*ibmKeybd->processInputProc)(&e,(DeviceIntPtr)ibmKeybd,1);
	e.u.u.type= KeyPress;
    }
    else if (pKey->kbd_status[0]&KBDUXMAKE)
	e.u.u.type= KeyPress;
    else
	e.u.u.type= KeyRelease;

#ifdef XKB
    if (noXkbExtension) {
#endif
    if(capslock_toggle || (pModMap[key] & LockMask)) /* for the default case */
    {
	if(e.u.u.type == KeyRelease)
		return;

	if(KeybdPtr->key->down[key >> 3] & (1 << (key & 7))) 
	{
		e.u.u.type = KeyRelease;
		SetCapsLockLED(0);
		SetCapsLockState(keyc); 
	}
	else {
		SetCapsLockLED(1);
		ClearCapsLockState(keyc); 
	}

    }

    if(pModMap[key] & NumLockMask)
    {
	if(e.u.u.type == KeyRelease) return;
	if(KeybdPtr->key->down[key >>3] & (1 << (key & 7)) )
	{
		e.u.u.type = KeyRelease;
		SetNumLockLED(0);
	}
	else
		SetNumLockLED(1);
    }
#ifdef XKB
    }
#endif
	

    XTEST1_STEAL_KEY_EVENT(e)
    (*ibmKeybd->processInputProc)(&e,(DeviceIntPtr)ibmKeybd,1);

    TRACE(("Exit aixKbdEvent\n"));
    return;

} /* aixKbdEvent */

/***====================================================================***/
/***			LFT Routines End				***/
/***====================================================================***/
#endif /* _IBM_LFT */


/***====================================================================***/
/*
 * Additional test extension functions.
 */
#ifdef XTESTEXT1
void
XTestGenerateEvent(dev_type, keycode, keystate, mousex, mousey)
	int     dev_type;
	int     keycode;
	int     keystate;
	int     mousex;
	int     mousey;
{
	xEvent  e;
	TRACE(("XTestGenerateEvent#1\n"));

	if (pendingX||pendingY)
	    aixFlushMouse();
	/*
	 * the server expects to have the x and y position of the locator
	 * when the action happened placed in other_p[XPOINTER]
	 */
	if (dev_type == XE_MOUSE)
	{
	        if (keystate == XTestKEY_UP)
	                keystate = ButtonRelease;
	        else
	                keystate = ButtonPress;
	        e.u.keyButtonPointer.rootX= AIXCurrentX = mousex;
	        e.u.keyButtonPointer.rootY= AIXCurrentY = mousey;
	        e.u.keyButtonPointer.time= lastEventTime = GET_OS_TIME();
	        e.u.u.detail=                   keycode;
	        e.u.u.type=                     keystate;
	        (*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	}
	else  /* keyboard events */
	{
	        if (keystate == XTestKEY_UP)
	                keystate = KeyRelease;
	        else
	                keystate = KeyPress;
	        e.u.keyButtonPointer.rootX= AIXCurrentX = mousex;
	        e.u.keyButtonPointer.rootY= AIXCurrentY = mousey;
	        e.u.keyButtonPointer.time= lastEventTime = GET_OS_TIME();
	        e.u.u.detail=                   keycode;
	        e.u.u.type=                     keystate;
	        (*ibmKeybd->processInputProc)(&e,(DeviceIntPtr)ibmKeybd,1);
	}

#ifdef notdef
	/*
	   ##### Here is the original HP code, the generated event is put in
	   ##### event queue.  Similar implementation might be needed
	   ##### in our AIX hft layer, but until we see XTM code, we'll
	   ##### have better idea.   -glee
	 */
	/*
	 * set the last event time so that the screen saver code will
	 * think that a key has been pressed
	 */
	lastEventTime = GetTimeInMillis();

	put_keyevent(keycode,
	             keystate,
	             0,
	             dev_type,
	             tmp_ptr,
	             &hil_info);

	ProcessInputEvents();
#endif

}


void
XTestGetPointerPos(fmousex, fmousey)
	short *fmousex, *fmousey;
{
	*fmousex = AIXCurrentX;
	*fmousey = AIXCurrentY;
}

/******************************************************************************
 *
 *      Tell the server to move the mouse.
 *
 */
void
XTestJumpPointer(jx, jy, dev_type)
/*
 * the x and y position to move the mouse to
 */
int     jx;
int     jy;
/*
 * which device is supposed to move (ignored)
 */
int     dev_type;
{
	xEvent e;
	TRACE(("XTestJumpPointer#1\n"));

	ProcessInputEvents();
	/*
	 * set the last event time so that the screen saver code will
	 * think that the mouse has been moved
	 */
	e.u.keyButtonPointer.time = lastEventTime = GET_OS_TIME();
	/*
	 * tell the server where the mouse is being moved to
	 */
	e.u.keyButtonPointer.rootX = AIXCurrentX = jx;
	e.u.keyButtonPointer.rootY = AIXCurrentY = jy;
	e.u.u.type = MotionNotify;

	(*ibmPtr->processInputProc)(&e,(DeviceIntPtr)ibmPtr,1);
	(*(ibmCursorShow(ibmCurrentScreen)))(jx, jy);
}
#endif /* XTESTEXT1 */

#ifdef AIXEXTENSIONS


static void
aixFlushTablet(deltax,deltay)
int deltax,deltay;
{
register        int     x,y;
register        ibmPerScreenInfo        *screenInfo;
	        int     setCursor;
	        xEvent  e;

    TRACE(("aixFlushTablet(%d, %d)\n", deltax, deltay));

    screenInfo = ibmScreens[ ibmCurrentScreen ];
    setCursor = TRUE ;

#ifdef AIXV3
    /* save current values */
    aixTabletSaveX = deltax;
    aixTabletSaveY = deltay;

    /* deltax and deltay are really absolute tablet coordinates */
    x = deltax * screenInfo->ibm_ScreenBounds.x2 / aixTabletScaleX;
    y = deltay * screenInfo->ibm_ScreenBounds.y2 / aixTabletScaleY;
#else
    x = AIXCurrentX ;
    y = screenInfo->ibm_ScreenBounds.y2 - AIXCurrentY ;

    /*
     * Analize horizontal movement for releative coordinates
     */

    if ( deltax <= aixTabletSaveX )
	x =  deltax * x / aixTabletSaveX ;
    else if ((screenInfo->ibm_ScreenBounds.x2 * aixTabletScaleX) != aixTabletSaveX)
	x += (deltax - aixTabletSaveX) * (screenInfo->ibm_ScreenBounds.x2 - x) /
	     (screenInfo->ibm_ScreenBounds.x2 * aixTabletScaleX - aixTabletSaveX);

    if ( !deltax )
	aixTabletSaveX = 1 ;
    else
	aixTabletSaveX = deltax;

    /*
     * Analize vertical movement for releative coordinates
     */

    if ( deltay <= aixTabletSaveY )
       y = deltay * y / aixTabletSaveY ;
    else if ((screenInfo->ibm_ScreenBounds.y2 * aixTabletScaleY) != aixTabletSaveY)
       y += (deltay - aixTabletSaveY) * (screenInfo->ibm_ScreenBounds.y2 - y) /
	    (screenInfo->ibm_ScreenBounds.y2 * aixTabletScaleY - aixTabletSaveY);

    if ( !deltay)
	aixTabletSaveY = 1 ;
    else
	aixTabletSaveY = deltay ;
#endif

    y = screenInfo->ibm_ScreenBounds.y2 - y ;

    if ( y > screenInfo->ibm_ScreenBounds.y2)
	y = screenInfo->ibm_ScreenBounds.y2;

    if ( x > screenInfo->ibm_ScreenBounds.x2)
	x = screenInfo->ibm_ScreenBounds.x2;

    if ( y <= 0)
	y = 0;

    if ( x <= 0)
	x = 0;

    if (setCursor) {
	/* OS-DEPENDENT MACRO GOES HERE!!
	 * MACRO DEFINED IN FILE ibmos.h
	 * TELL OS THAT CURSOR HAS MOVED
	 * TO A NEW POSITION
	 */
	setCursorPosition(x,y);
    }

    (* screenInfo->ibm_CursorShow )( x, y ) ;

    AIXCurrentX= x ;
    AIXCurrentY= y ;
    pendingX= pendingY= 0;

}

#endif /* AIXEXTENSIONS */

/***====================================================================***/
