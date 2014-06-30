/* $XFree86: xc/programs/Xserver/hw/xfree86/vga16/ibm/wm0.h,v 3.2 1996/12/23 06:53:30 dawes Exp $ */





/* $XConsortium: wm0.h /main/4 1996/02/21 17:59:17 kaleb $ */

#include "vgaReg.h"


/* The following awesome macro steps through all four planes calling
 * 'call.' 
 */
#ifdef	PC98_EGC
	/* not yet */
#else
#define DO_WM0(pgc,call) \
{ int _fg, _bg, _rop, _pm; \
  mfbPrivGC *_pPriv = (mfbPrivGC *)((pgc)->devPrivates[mfbGCPrivateIndex].ptr); \
  _fg = (pgc)->fgPixel; _bg = (pgc)->bgPixel; _rop = _pPriv->rop; \
  _pm = (pgc)->planemask; \
  \
  SetVideoGraphics(Enb_Set_ResetIndex, 0); /* All from CPU */ \
  SetVideoGraphics(Bit_MaskIndex, 0xFF); /* All bits */ \
  SetVideoGraphics(Graphics_ModeIndex, 0); /* Write mode 0 */ \
  SetVideoGraphics(Data_RotateIndex, 0); /* Don't rotate, replace */ \
  \
  if ( (pgc)->planemask & 1) { \
    (pgc)->fgPixel = (_fg>>0)&1; (pgc)->bgPixel = (_bg>>0)&1; \
    _pPriv->rop = mfbReduceRop((pgc)->alu, (pgc)->fgPixel); \
    (pgc)->planemask = 1; \
    SetVideoGraphics(Read_Map_SelectIndex, 0); \
    SetVideoSequencer(Mask_MapIndex, 1);	/* Plane 0 */ \
    (call); \
  } \
  if ( (pgc)->planemask & 2) { \
    (pgc)->fgPixel = (_fg>>1)&1; (pgc)->bgPixel = (_bg>>1)&1; \
    _pPriv->rop = mfbReduceRop((pgc)->alu, (pgc)->fgPixel); \
    (pgc)->planemask = 1; \
    SetVideoGraphics(Read_Map_SelectIndex, 1); \
    SetVideoSequencer(Mask_MapIndex, 2);	/* Plane 1 */ \
    (call); \
  } \
  if ( (pgc)->planemask & 4) { \
    (pgc)->fgPixel = (_fg>>2)&1; (pgc)->bgPixel = (_bg>>2)&1; \
    _pPriv->rop = mfbReduceRop((pgc)->alu, (pgc)->fgPixel); \
    (pgc)->planemask = 1; \
    SetVideoGraphics(Read_Map_SelectIndex, 2); \
    SetVideoSequencer(Mask_MapIndex, 4);	/* Plane 2 */ \
    (call); \
  } \
  if ( (pgc)->planemask & 8) { \
    (pgc)->fgPixel = (_fg>>3)&1; (pgc)->bgPixel = (_bg>>3)&1; \
    _pPriv->rop = mfbReduceRop((pgc)->alu, (pgc)->fgPixel); \
    (pgc)->planemask = 1; \
    SetVideoGraphics(Read_Map_SelectIndex, 3); \
    SetVideoSequencer(Mask_MapIndex, 8);	/* Plane 3 */ \
    (call); \
  } \
  (pgc)->fgPixel = _fg; (pgc)->bgPixel = _bg; _pPriv->rop = _rop; \
  (pgc)->planemask = _pm; \
}
#endif

