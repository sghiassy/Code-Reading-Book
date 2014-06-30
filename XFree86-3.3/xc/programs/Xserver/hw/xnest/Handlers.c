/* $XConsortium: Handlers.c,v 1.2 95/07/10 17:42:22 ray Exp $ */
/*

Copyright 1993 by Davor Matic

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation.  Davor Matic makes no representations about
the suitability of this software for any purpose.  It is provided "as
is" without express or implied warranty.

*/
#include "X.h"
#include "Xproto.h"
#include "screenint.h"
#include "input.h"
#include "misc.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "servermd.h"

#include "Xnest.h"

#include "Display.h"
#include "Handlers.h"

void xnestBlockHandler(blockData, pTimeout, pReadMask)
     pointer blockData;
     pointer pTimeout;
     pointer pReadMask;
{
  xnestCollectExposures();
  XFlush(xnestDisplay);
}

void xnestWakeupHandler(result, pReadMask)
     int result;
     pointer pReadMask;
{
  xnestCollectEvents();
}
