/* $XFree86: xc/programs/Xserver/hw/xfree86/common/Mach64.c,v 3.4 1996/12/23 06:43:02 dawes Exp $ */





/* $XConsortium: Mach64.c /main/5 1996/02/21 17:36:50 kaleb $ */
#include "X.h"
#include "os.h"

#define _NO_XF86_PROTOTYPES

#include "xf86.h"
#include "xf86_Config.h"

extern ScrnInfoRec mach64InfoRec;

/*
 * This limit is currently set to 80MHz because this is the limit of many
 * ramdacs when running in 1:1 mode.  It will be increased when support
 * is added for using the ramdacs in 2:1 mode.  Increasing this limit
 * could result in damage to your hardware.
 */
#define MAX_MACH64_CLOCK		80000

int mach64MaxClock = MAX_MACH64_CLOCK;

ScrnInfoPtr xf86Screens[] = 
{
  &mach64InfoRec,
};

int  xf86MaxScreens = sizeof(xf86Screens) / sizeof(ScrnInfoPtr);

int xf86ScreenNames[] =
{
  ACCEL,
  -1
};

int mach64ValidTokens[] =
{
  STATICGRAY,
  GRAYSCALE,
  STATICCOLOR,
  PSEUDOCOLOR,
  TRUECOLOR,
  DIRECTCOLOR,
  CHIPSET,
  CLOCKS,
  MODES,
  OPTION,
  VIDEORAM,
  VIEWPORT,
  VIRTUAL,
  CLOCKPROG,
  BIOSBASE,
  MEMBASE,
  -1
};

#include "xf86ExtInit.h"
