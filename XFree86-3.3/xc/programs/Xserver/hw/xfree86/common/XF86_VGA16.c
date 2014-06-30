/* $XFree86: xc/programs/Xserver/hw/xfree86/common/XF86_VGA16.c,v 3.7 1996/12/23 06:43:11 dawes Exp $ */





/* $XConsortium: XF86_VGA16.c /main/4 1996/02/21 17:37:22 kaleb $ */

#include "X.h"
#include "os.h"

#define _NO_XF86_PROTOTYPES

#include "xf86.h"
#include "xf86_Config.h"

extern ScrnInfoRec vga16InfoRec;
extern ScrnInfoRec monoInfoRec;

#ifdef BUILD_VGA16
#define SCREEN0 &vga16InfoRec
#else
#define SCREEN0 NULL
#endif

#ifdef BUILD_VGA16_MONO
#define SCREEN1 &monoInfoRec
#else
#define SCREEN1 NULL
#endif

ScrnInfoPtr xf86Screens[] = 
{
  SCREEN0,
  SCREEN1,
};

int  xf86MaxScreens = sizeof(xf86Screens) / sizeof(ScrnInfoPtr);

int xf86ScreenNames[] =
{
  VGA16,
  MONO,
  -1
};

#ifdef BUILD_VGA16
int vga16ValidTokens[] =
{
  PSEUDOCOLOR,
  STATICCOLOR,
  STATICGRAY,
  GRAYSCALE,
  CHIPSET,
  CLOCKS,
  MODES,
  SCREENNO,
  OPTION,
  VIDEORAM,
  VIEWPORT,
  VIRTUAL,
  CLOCKPROG,
  BIOSBASE,
  -1
};
#endif

#ifdef BUILD_VGA16_MONO
int monoValidTokens[] =
{
  STATICGRAY,
  CHIPSET,
  OPTION,
  MEMBASE,
  SCREENNO,
  VIRTUAL,
  VIEWPORT,
  -1
};
#endif

#define MONO_SERVER
#include "xf86ExtInit.h"

