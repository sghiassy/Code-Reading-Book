/* $XFree86: xc/programs/Xserver/hw/xfree86/common/XF86_I128.c,v 3.2 1996/12/23 06:43:06 dawes Exp $ */
/*
 *
 * Copyright 1995 XFree86 Inc.
 *
 *
 */
/* $XConsortium: XF86_I128.c /main/2 1996/02/21 17:37:04 kaleb $ */

#include "X.h"
#include "os.h"

#define _NO_XF86_PROTOTYPES

#include "xf86.h"
#include "xf86_Config.h"

extern ScrnInfoRec i128InfoRec;

#define MAX_I128_CLOCK		175000

int i128MaxClock = MAX_I128_CLOCK;

ScrnInfoPtr xf86Screens[] = 
{
  &i128InfoRec,
};

int  xf86MaxScreens = sizeof(xf86Screens) / sizeof(ScrnInfoPtr);

int xf86ScreenNames[] =
{
  ACCEL,
  -1
};

int i128ValidTokens[] =
{
  STATICGRAY,
  GRAYSCALE,
  STATICCOLOR,
  PSEUDOCOLOR,
  TRUECOLOR,
  DIRECTCOLOR,
  MODES,
  OPTION,
  VIDEORAM,
  VIEWPORT,
  VIRTUAL,
  CLOCKPROG,
  INSTANCE,
  -1
};

#include "xf86ExtInit.h"

