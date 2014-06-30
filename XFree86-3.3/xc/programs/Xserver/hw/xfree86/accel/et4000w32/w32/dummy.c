/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/w32/dummy.c,v 3.5 1997/01/12 10:41:36 dawes Exp $ */
/*
 * cfbfuncs.c
 *
 * Initialise low level cfb functions
 * 
 * To coax et4_driver.c into cooperation--GGL 
 */
/* $XConsortium: dummy.c /main/4 1996/02/21 17:21:47 kaleb $ */

#include "vga256.h"

GCOps vga256TEOps1Rect, vga256TEOps, vga256NonTEOps1Rect, vga256NonTEOps;

CfbfuncRec vga256LowlevFuncs;

void speedupvga256FillRectSolidCopy(a, b, c, d)
  DrawablePtr a;
  GCPtr b;
  int c;
  BoxPtr d;
{FatalError("FillRectSolidCopy");}

void speedupvga256DoBitbltCopy(a, b, c, d, e, f, g)
  DrawablePtr a;
  DrawablePtr b;
  int c;
  RegionPtr d;
  DDXPointPtr e;
  unsigned long f;
  unsigned long g;
{FatalError("DoBitbltCopy");}

void speedupvga256SegmentSS(a, b, c, d)
  DrawablePtr a;
  GCPtr b;
  int c;
  xSegment *d;
{FatalError("SegmentSS");}

void speedupvga256LineSS(a, b, c, d, e)
  DrawablePtr a;
  GCPtr b;
  int c, d;
  DDXPointPtr e;
{FatalError("LineSS");}

void speedupvga256TEGlyphBlt8(a, b, c, d, e, f, g)
  DrawablePtr a;
  GC *b;
  int c, d;
  unsigned int e;
  CharInfoPtr *f;
  pointer g;
{FatalError("TEGlyphBlt8");}

void speedupvga2568FillRectTransparentStippled32(a, b, c, d)
  DrawablePtr a;
  GCPtr b;
  int c;
  BoxPtr d;
{FatalError("FillRectTransparentStippled32");}

void speedupvga256FillBoxSolid(a, b, c, d, e, f)
  DrawablePtr a;
  int b;
  BoxPtr c;
  unsigned long d, e;
  int f;
{FatalError("FillBoxSolid");}

void speedupvga2568FillRectOpaqueStippled32(a, b, c, d)
  DrawablePtr a;
  GCPtr b;
  int c;
  BoxPtr d;
{FatalError("FillRectOpqueStippled32");}
