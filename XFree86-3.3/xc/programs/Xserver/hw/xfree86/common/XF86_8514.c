/* $XFree86: xc/programs/Xserver/hw/xfree86/common/XF86_8514.c,v 3.6 1996/12/23 06:43:05 dawes Exp $ */





/* $XConsortium: XF86_8514.c /main/4 1996/02/21 17:36:57 kaleb $ */
#include "X.h"
#include "os.h"

#define _NO_XF86_PROTOTYPES

#include "xf86.h"
#include "xf86_Config.h"

extern ScrnInfoRec ibm8514InfoRec;

ScrnInfoPtr xf86Screens[] = 
{
  &ibm8514InfoRec,
};

int  xf86MaxScreens = sizeof(xf86Screens) / sizeof(ScrnInfoPtr);

int xf86ScreenNames[] =
{
  ACCEL,
  -1
};

int ibm8514ValidTokens[] =
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
  -1
};

#include "xf86ExtInit.h"

