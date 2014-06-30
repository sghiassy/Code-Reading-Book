/* $XConsortium: xf86Jstk.c /main/14 1996/10/25 14:11:36 kaleb $ */
/*
 * Copyright 1995 by Frederic Lepied, France. <fred@sugix.frmug.fr.net>       
 *                                                                            
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is  hereby granted without fee, provided that
 * the  above copyright   notice appear  in   all  copies and  that both  that
 * copyright  notice   and   this  permission   notice  appear  in  supporting
 * documentation, and that   the  name of  Frederic   Lepied not  be  used  in
 * advertising or publicity pertaining to distribution of the software without
 * specific,  written      prior  permission.     Frederic  Lepied   makes  no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.                   
 *                                                                            
 * FREDERIC  LEPIED DISCLAIMS ALL   WARRANTIES WITH REGARD  TO  THIS SOFTWARE,
 * INCLUDING ALL IMPLIED   WARRANTIES OF MERCHANTABILITY  AND   FITNESS, IN NO
 * EVENT  SHALL FREDERIC  LEPIED BE   LIABLE   FOR ANY  SPECIAL, INDIRECT   OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA  OR PROFITS, WHETHER  IN  AN ACTION OF  CONTRACT,  NEGLIGENCE OR OTHER
 * TORTIOUS  ACTION, ARISING    OUT OF OR   IN  CONNECTION  WITH THE USE    OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/common/xf86Jstk.c,v 3.18 1996/12/23 06:43:28 dawes Exp $ */

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "misc.h"
#include "inputstr.h"
#include "scrnintstr.h"
#include "XI.h"
#include "XIproto.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86_Config.h"
#include "xf86Procs.h"
#include "xf86Xinput.h"
#include "xf86_OSlib.h"
#include "atKeynames.h"
#include "xf86Version.h"

#include "osdep.h"

/******************************************************************************
 * debugging macro
 *****************************************************************************/
#ifdef DBG
#undef DBG
#endif
#ifdef DEBUG
#undef DEBUG
#endif

static int      debug_level = 0;
#define DEBUG 1
#if DEBUG
#define DBG(lvl, f) {if ((lvl) <= debug_level) f;}
#else
#define DBG(lvl, f)
#endif

/******************************************************************************
 * device records
 *****************************************************************************/

typedef struct 
{
  int           jstkFd;         /* Joystick File Descriptor */
  OsTimerPtr    jstkTimer;      /* timer object */
  int           jstkTimeout;    /* timeout to poll device */
  char          *jstkDevice;    /* device file name */
  int           jstkOldX;       /* previous X position */
  int           jstkOldY;       /* previous Y position */
  int           jstkOldButtons; /* previous buttons state */
  int           jstkMaxX;       /* max X value */
  int           jstkMaxY;       /* max Y value */
  int           jstkMinX;       /* min X value */
  int           jstkMinY;       /* min Y value */
  int           jstkCenterX;    /* X center value */
  int           jstkCenterY;    /* Y center value */
  int           jstkDelta;      /* delta cursor */
} JoystickDevRec, *JoystickDevPtr;

/******************************************************************************
 * configuration stuff
 *****************************************************************************/
#define DEVICENAME 1
#define TIMEOUT 2
#define MAXX 3
#define MAXY 4
#define MINX 5
#define MINY 6
#define CENTERX 7
#define CENTERY 8
#define DELTA 9
#define PORT 10
#define DEBUG_LEVEL 11
#define HISTORY_SIZE 12
#define ALWAYS_CORE 13

static SymTabRec JstkTab[] = {
  { ENDSUBSECTION,	"endsubsection" },
  { DEVICENAME,		"devicename" },
  { TIMEOUT,		"timeout" },
  { MAXX,		"maximumxposition" },
  { MAXY,		"maximumyposition" },
  { MINX,		"minimumxposition" },
  { MINY,		"minimumyposition" },
  { CENTERX,		"centerx" },
  { CENTERY,		"centery" },
  { DELTA,		"delta" },
  { PORT,		"port" },
  { DEBUG_LEVEL,	"debuglevel" },
  { HISTORY_SIZE,	"historysize" },
  { ALWAYS_CORE,        "alwayscore" },
  { -1,			"" },
};

/******************************************************************************
 * external declarations
 *****************************************************************************/

extern void xf86eqEnqueue(
#if NeedFunctionPrototypes
    xEventPtr /*e*/
#endif
);

extern void miPointerDeltaCursor(
#if NeedFunctionPrototypes
    int /*dx*/,
    int /*dy*/,
    unsigned long /*time*/
#endif
);

extern int xf86JoystickGetState(
#ifdef NeedFunctionPrototypes
    int   /*fd*/,
    int * /*x*/,
    int * /*y*/,
    int * /*buttons*/
#endif
    );

extern void xf86JoystickInit(
#ifdef NeedFunctionPrototypes
void
#endif
);

extern int xf86JoystickOff(
#ifdef NeedFunctionPrototypes
int * /*fd*/,
int /*doclose*/
#endif
);

extern int xf86JoystickOn(
#ifdef NeedFunctionPrototypes
char * /*name*/,
int * /*timeout*/,
int * /*centerX*/,
int * /*centerY*/
#endif
);

/*
 * xf86JstkConfig --
 *      Configure the device.
 */
static Bool
xf86JstkConfig(LocalDevicePtr    *array,
               int               index,
               int               max,
               LexPtr            val)
{
  LocalDevicePtr        dev = array[index];
  JoystickDevPtr        priv = (JoystickDevPtr)(dev->private);
  int token;
  
  DBG(1, ErrorF("xf86JstkConfig\n"));
      
  /* Set defaults */
  priv->jstkOldX = -1;
  priv->jstkOldY = -1;
  priv->jstkOldButtons = -1;
  priv->jstkFd = -1;
  priv->jstkTimeout = 0;

  while ((token = xf86GetToken(JstkTab)) != ENDSUBSECTION) {
    switch(token) {
    case DEVICENAME:
      if (xf86GetToken(NULL) != STRING) xf86ConfigError("Option string expected");
      dev->name = strdup(val->str);
      break;
      
    case PORT:
      if (xf86GetToken(NULL) != STRING) xf86ConfigError("Option string expected");
      priv->jstkDevice = strdup(val->str);
      break;

    case TIMEOUT:
      if (xf86GetToken(NULL) != NUMBER) xf86ConfigError("Joystick Timeout expected");
      priv->jstkTimeout = val->num;
     break;

    case MAXX:
      if (xf86GetToken(NULL) != NUMBER)
        xf86ConfigError("Joystick MaximumXPosition expected");
      priv->jstkMaxX = val->num;
     break;
      
    case MAXY:
      if (xf86GetToken(NULL) != NUMBER)
        xf86ConfigError("Joystick MaximumYPosition expected");
      priv->jstkMaxY = val->num;
     break;
      
    case MINX:
      if (xf86GetToken(NULL) != NUMBER)
        xf86ConfigError("Joystick MinimumXPosition expected");
      priv->jstkMinX = val->num;
     break;
      
    case MINY:
      if (xf86GetToken(NULL) != NUMBER)
        xf86ConfigError("Joystick MinimumYPosition expected");
      priv->jstkMinY = val->num;
     break;
      
    case CENTERX:
      if (xf86GetToken(NULL) != NUMBER)
        xf86ConfigError("Joystick CenterX expected");
      priv->jstkCenterX = val->num;
     break;
      
    case CENTERY:
      if (xf86GetToken(NULL) != NUMBER)
        xf86ConfigError("Joystick CenterY expected");
      priv->jstkCenterY = val->num;
     break;
      
    case DELTA:
      if (xf86GetToken(NULL) != NUMBER)
        xf86ConfigError("Joystick Delta expected");
      priv->jstkDelta = val->num;
     break;
      
    case DEBUG_LEVEL:
	if (xf86GetToken(NULL) != NUMBER)
	    xf86ConfigError("Option number expected");
	debug_level = val->num;
	if (xf86Verbose) {
#if DEBUG
	    ErrorF("%s Joystick debug level sets to %d\n", XCONFIG_GIVEN,
		   debug_level);      
#else
	    ErrorF("%s Joystick debug level not sets to %d because debugging is not compiled\n",
		   XCONFIG_GIVEN, debug_level);
#endif
	}
        break;

    case HISTORY_SIZE:
      if (xf86GetToken(NULL) != NUMBER)
	xf86ConfigError("Option number expected");
      dev->history_size = val->num;
      if (xf86Verbose)
	ErrorF("%s Joystick Motion history size is %d\n", XCONFIG_GIVEN,
	       dev->history_size);      
      break;
	    
    case ALWAYS_CORE:
	xf86AlwaysCore(dev, TRUE);
	if (xf86Verbose)
	    ErrorF("%s Joystick device always stays core pointer\n",
		   XCONFIG_GIVEN);
	break;

    case EOF:
      FatalError("Unexpected EOF (missing EndSubSection)");
      break; /* :-) */

    default:
      xf86ConfigError("Joystick subsection keyword expected");
      break;
    }
  }
  
  DBG(1, ErrorF("xf86JstkConfig timeout=%d name=%s maxx=%d maxy=%d minx=%d miny=%d "
		"centerx=%d centery=%d delta=%d\n",
		priv->jstkTimeout, priv->jstkDevice, priv->jstkMaxX, priv->jstkMaxY,
		priv->jstkMinX, priv->jstkMinY, priv->jstkCenterX, priv->jstkCenterY,
		priv->jstkDelta));

  if (xf86Verbose) {
    ErrorF("%s %s: timeout=%d port=%s maxx=%d maxy=%d minx=%d miny=%d\n"
	   "\tcenterx=%d centery=%d delta=%d\n", XCONFIG_GIVEN, dev->name,
	   priv->jstkTimeout, priv->jstkDevice, priv->jstkMaxX, priv->jstkMaxY,
	   priv->jstkMinX, priv->jstkMinY, priv->jstkCenterX, priv->jstkCenterY,
	   priv->jstkDelta);
  }
  return Success;
}

/*
 * xf86JstkEvents --
 *      Read the new events from the device, and enqueue them.
 */
static CARD32
xf86JstkEvents(OsTimerPtr        timer,
               CARD32            atime,
               pointer           arg)
{
  DeviceIntPtr          device = (DeviceIntPtr)arg;
  JoystickDevPtr        priv = (JoystickDevPtr) PRIVATE(device);
  int                   timeout = priv->jstkTimeout;
  int                   x, y, buttons;

  DBG(5, ErrorF("xf86JstkEvents BEGIN device=0x%x priv=0x%x"
                " timeout=%d timer=0x%x\n",
                device, priv, timeout, priv->jstkTimer));
  
  if (xf86JoystickGetState(priv->jstkFd, &x, &y, &buttons)) {
    int loop;
    int length = priv->jstkMaxX - priv->jstkMinX;
    int height = priv->jstkMaxY - priv->jstkMinY;
    int v0 = ((x - priv->jstkMinX) * priv->jstkDelta) / length -
      (priv->jstkDelta / 2);
    int v1 = ((y - priv->jstkMinY) * priv->jstkDelta) / height - 
      (priv->jstkDelta / 2);

    DBG(4, ErrorF("xf86JoystickGetState x=%d y=%d centerX=%d centerY=%d v0=%d "
                  "v1=%d buttons=%d\n",
                  x, y, priv->jstkCenterX, priv->jstkCenterY,
                  v0, v1, buttons));
    
    if ((abs(v0) > (priv->jstkDelta / 20)) ||
        (abs(v1) > (priv->jstkDelta / 20)))
      {
	xf86PostMotionEvent(device, 0, 0, 2, v0, v1);
	
        priv->jstkOldX = x;
        priv->jstkOldY = y;          
      }
    for(loop=1; loop<3; loop++)
      {
        if ((priv->jstkOldButtons & loop) != (buttons & loop))
          {
	    xf86PostButtonEvent(device, 0, loop, ((buttons & loop) == loop),
				0, 2, v0, v1);
          }
      }
    priv->jstkOldButtons = buttons;
  }

  DBG(3, ErrorF("xf86JstkEvents END   device=0x%x priv=0x%x"
                " timeout=%d timer=0x%x\n",
                device, priv, timeout, priv->jstkTimer));

  return timeout;
}

static void
xf86JstkControlProc(DeviceIntPtr	device,
                    PtrCtrl		*ctrl)
{
  DBG(2, ErrorF("xf86JstkControlProc\n"));
}

/*
 * xf86JstkProc --
 *      Handle the initialization, etc. of a joystick
 */
static int
xf86JstkProc(pJstk, what)
     DeviceIntPtr       pJstk;
     int                what;
{
  CARD8                 map[5];
  int                   nbaxes;
  int                   nbbuttons;
  int                   jstkfd;
  int                   loop;
  LocalDevicePtr        local = (LocalDevicePtr)pJstk->public.devicePrivate;
  JoystickDevPtr        priv = (JoystickDevPtr)PRIVATE(pJstk);

  DBG(2, ErrorF("BEGIN xf86JstkProc dev=0x%x priv=0x%x xf86JstkEvents=0x%x\n",
                pJstk, priv, xf86JstkEvents));
  
  switch (what)
    {
    case DEVICE_INIT: 
      DBG(1, ErrorF("xf86JstkProc pJstk=0x%x what=INIT\n", pJstk));
  
      map[1] = 1;
      map[2] = 2;

      nbaxes = 2;
      nbbuttons = 2;

      if (InitButtonClassDeviceStruct(pJstk,
                                      nbbuttons,
                                      map) == FALSE) 
        {
          ErrorF("unable to allocate Button class device\n");
          return !Success;
        }
      
      if (InitFocusClassDeviceStruct(pJstk) == FALSE)
        {
          ErrorF("unable to init Focus class device\n");
          return !Success;
        }
          
      if (InitPtrFeedbackClassDeviceStruct(pJstk,
                                           xf86JstkControlProc) == FALSE)
        {
          ErrorF("unable to init ptr feedback\n");
          return !Success;
        }
          
      if (InitValuatorClassDeviceStruct(pJstk, 
                                    nbaxes,
                                    xf86GetMotionEvents, 
                                    local->history_size,
                                    Relative) /* relatif ou absolute */
          == FALSE) 
        {
          ErrorF("unable to allocate Valuator class device\n"); 
          return !Success;
        }
      else 
        {
          for(loop=0; loop<nbaxes; loop++) {
            InitValuatorAxisStruct(pJstk,
                                   loop,
                                   0, /* min val */
                                   1000, /* max val */
                                   9600); /* resolution */
          }
	  /* allocate the motion history buffer if needed */
	  xf86MotionHistoryAllocate(local);

          xf86JoystickInit();
          AssignTypeAndName(pJstk, local->atom, local->name);
        }

      break; 
      
    case DEVICE_ON:
      priv->jstkFd = jstkfd = xf86JoystickOn(priv->jstkDevice,
                                             &(priv->jstkTimeout),
					     &(priv->jstkCenterX),
					     &(priv->jstkCenterY));

      DBG(1, ErrorF("xf86JstkProc  pJstk=0x%x what=ON name=%s\n", pJstk,
                    priv->jstkDevice));

      if (jstkfd != -1)
      {
        priv->jstkTimer = TimerSet(NULL, 0, /*TimerAbsolute,*/
                                   priv->jstkTimeout,
                                   xf86JstkEvents,
                                   (pointer)pJstk);
        pJstk->public.on = TRUE;
        DBG(2, ErrorF("priv->jstkTimer=0x%x\n", priv->jstkTimer));
      }
      else
        return !Success;
    break;
      
    case DEVICE_OFF:
    case DEVICE_CLOSE:
      DBG(1, ErrorF("xf86JstkProc  pJstk=0x%x what=%s\n", pJstk,
                    (what == DEVICE_CLOSE) ? "CLOSE" : "OFF"));

      jstkfd = xf86JoystickOff(&(priv->jstkFd), (what == DEVICE_CLOSE));
      pJstk->public.on = FALSE;
    break;

    default:
      ErrorF("unsupported mode=%d\n", what);
      return !Success;
      break;
    }
  DBG(2, ErrorF("END   xf86JstkProc dev=0x%x priv=0x%x xf86JstkEvents=0x%x\n",
                pJstk, priv, xf86JstkEvents));
  return Success;
}

/*
 * xf86JstkAllocate --
 *      Allocate Joystick device structures.
 */
static LocalDevicePtr
xf86JstkAllocate()
{
  LocalDevicePtr        local = (LocalDevicePtr) xalloc(sizeof(LocalDeviceRec));
  JoystickDevPtr        priv = (JoystickDevPtr) xalloc(sizeof(JoystickDevRec));
  
  local->name = "JOYSTICK";
  local->flags = XI86_NO_OPEN_ON_INIT;
  local->device_config = xf86JstkConfig;
  local->device_control = xf86JstkProc;
  local->read_input = NULL;
  local->close_proc = NULL;
  local->control_proc = NULL;
  local->switch_mode = NULL;
  local->fd = -1;
  local->atom = 0;
  local->dev = NULL;
  local->private = priv;
  local->type_name = "Joystick";
  local->history_size  = 0;
  
  priv->jstkFd = -1;
  priv->jstkTimer = NULL;
  priv->jstkTimeout = 0;
  priv->jstkDevice = NULL;
  priv->jstkOldX = -1;
  priv->jstkOldY = -1;
  priv->jstkOldButtons = -1;
  priv->jstkMaxX = 1000;
  priv->jstkMaxY = 1000;
  priv->jstkMinX = 0;
  priv->jstkMinY = 0;
  priv->jstkCenterX = -1;
  priv->jstkCenterY = -1;
  priv->jstkDelta = 100;
  
  return local;
}

/*
 * joystick association
 */
DeviceAssocRec joystick_assoc =
{
  "joystick",                   /* config_section_name */
  xf86JstkAllocate              /* device_allocate */
};

#ifdef DYNAMIC_MODULE
/*
 * entry point of dynamic loading
 */
int
#ifndef DLSYM_BUG
init_module(unsigned long	server_version)
#else
init_xf86Jstk(unsigned long     server_version)
#endif
{
    xf86AddDeviceAssoc(&joystick_assoc);

    if (server_version != XF86_VERSION_CURRENT) {
	ErrorF("Warning: Joystick module compiled for version%s\n", XF86_VERSION);
	return 0;
    } else {
	return 1;
    }
}
#endif

/* end of xf86Jstk.c */
