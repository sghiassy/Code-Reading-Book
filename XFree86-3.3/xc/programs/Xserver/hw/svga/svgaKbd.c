/* $XConsortium: svgaKbd.c,v 1.2 94/02/24 10:44:35 dpw Exp $ */
/*
 * Copyright 1990,91,92,93 by Thomas Roell, Germany.
 * Copyright 1991,92,93    by SGCS (Snitily Graphics Consulting Services), USA.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this  permission notice appear
 * in supporting documentation, and that the name of Thomas Roell nor
 * SGCS be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * Thomas Roell nor SGCS makes no representations about the suitability
 * of this software for any purpose. It is provided "as is" without
 * express or implied warranty.
 *
 * THOMAS ROELL AND SGCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THOMAS ROELL OR SGCS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "svga.h"
#include "atKeynames.h"

/*
 * For Scancodes see notes in atKeynames.h  !!!!
 */
static KeySym map[NUM_KEYCODES * GLYPHS_PER_KEY] = { 
    /* 0x00 */  NoSymbol,       NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x01 */  XK_Escape,      NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x02 */  XK_1,           XK_exclam,	NoSymbol,	NoSymbol,
    /* 0x03 */  XK_2,           XK_at,		NoSymbol,	NoSymbol,
    /* 0x04 */  XK_3,           XK_numbersign,	NoSymbol,	NoSymbol,
    /* 0x05 */  XK_4,           XK_dollar,	NoSymbol,	NoSymbol,
    /* 0x06 */  XK_5,           XK_percent,	NoSymbol,	NoSymbol,
    /* 0x07 */  XK_6,           XK_asciicircum,	NoSymbol,	NoSymbol,
    /* 0x08 */  XK_7,           XK_ampersand,	NoSymbol,	NoSymbol,
    /* 0x09 */  XK_8,           XK_asterisk,	NoSymbol,	NoSymbol,
    /* 0x0a */  XK_9,           XK_parenleft,	NoSymbol,	NoSymbol,
    /* 0x0b */  XK_0,           XK_parenright,	NoSymbol,	NoSymbol,
    /* 0x0c */  XK_minus,       XK_underscore,	NoSymbol,	NoSymbol,
    /* 0x0d */  XK_equal,       XK_plus,	NoSymbol,	NoSymbol,
    /* 0x0e */  XK_BackSpace,   NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x0f */  XK_Tab,         NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x10 */  XK_Q,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x11 */  XK_W,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x12 */  XK_E,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x13 */  XK_R,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x14 */  XK_T,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x15 */  XK_Y,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x16 */  XK_U,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x17 */  XK_I,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x18 */  XK_O,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x19 */  XK_P,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x1a */  XK_bracketleft, XK_braceleft,	NoSymbol,	NoSymbol,
    /* 0x1b */  XK_bracketright,XK_braceright,	NoSymbol,	NoSymbol,
    /* 0x1c */  XK_Return,      NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x1d */  XK_Control_L,   NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x1e */  XK_A,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x1f */  XK_S,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x20 */  XK_D,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x21 */  XK_F,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x22 */  XK_G,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x23 */  XK_H,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x24 */  XK_J,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x25 */  XK_K,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x26 */  XK_L,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x27 */  XK_semicolon,   XK_colon,	NoSymbol,	NoSymbol,
    /* 0x28 */  XK_quoteright,  XK_quotedbl,	NoSymbol,	NoSymbol,
    /* 0x29 */  XK_quoteleft,	XK_asciitilde,	NoSymbol,	NoSymbol,
    /* 0x2a */  XK_Shift_L,     NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x2b */  XK_backslash,   XK_bar,		NoSymbol,	NoSymbol,
    /* 0x2c */  XK_Z,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x2d */  XK_X,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x2e */  XK_C,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x2f */  XK_V,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x30 */  XK_B,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x31 */  XK_N,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x32 */  XK_M,           NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x33 */  XK_comma,       XK_less,	NoSymbol,	NoSymbol,
    /* 0x34 */  XK_period,      XK_greater,	NoSymbol,	NoSymbol,
    /* 0x35 */  XK_slash,       XK_question,	NoSymbol,	NoSymbol,
    /* 0x36 */  XK_Shift_R,     NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x37 */  XK_KP_Multiply, NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x38 */  XK_Alt_L,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x39 */  XK_space,       NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x3a */  XK_Caps_Lock,   NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x3b */  XK_F1,          NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x3c */  XK_F2,          NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x3d */  XK_F3,          NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x3e */  XK_F4,          NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x3f */  XK_F5,          NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x40 */  XK_F6,          NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x41 */  XK_F7,          NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x42 */  XK_F8,          NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x43 */  XK_F9,          NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x44 */  XK_F10,         NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x45 */  XK_Num_Lock,    NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x46 */  XK_Scroll_Lock,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x47 */  XK_KP_7,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x48 */  XK_KP_8,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x49 */  XK_KP_9,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x4a */  XK_KP_Subtract, NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x4b */  XK_KP_4,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x4c */  XK_KP_5,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x4d */  XK_KP_6,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x4e */  XK_KP_Add,      NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x4f */  XK_KP_1,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x50 */  XK_KP_2,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x51 */  XK_KP_3,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x52 */  XK_KP_0,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x53 */  XK_KP_Decimal,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x54 */  NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x55 */  NoSymbol,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x56 */  XK_less,	XK_greater,	NoSymbol,	NoSymbol,
    /* 0x57 */  XK_F11,		NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x58 */  XK_F12,		NoSymbol,	NoSymbol,	NoSymbol,

    /* 0x59 */  XK_Home,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x5a */  XK_Up,		NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x5b */  XK_Prior,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x5c */  XK_Left,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x5d */  XK_Begin,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x5e */  XK_Right,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x5f */  XK_End,		NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x60 */  XK_Down,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x61 */  XK_Next,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x62 */  XK_Insert,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x63 */  XK_Delete,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x64 */  XK_KP_Enter,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x65 */  XK_Control_R,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x66 */  XK_Pause,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x67 */  XK_Print,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x68 */  XK_KP_Divide,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x69 */  XK_Mode_switch,	NoSymbol,	NoSymbol,	NoSymbol,
    /* 0x6a */  XK_Break,	NoSymbol,	NoSymbol,	NoSymbol,
};

/*
 * LegalModifier --
 *      determine whether a key is a legal modifier key, i.e send a
 *      press/release sequence.
 */

/* ARGSUSED */
Bool
LegalModifier(
    unsigned int key,
    DevicePtr pDev
)
{
  return (TRUE);
}

      
/*
 * svgaKbdGetMapping --
 *	Get the national keyboard mapping. The keyboard type is set, a new map
 *      and the modifiermap is computed.
 */

void
svgaKbdGetMapping(
    KeySymsPtr pKeySyms,
    CARD8      *pModMap
)
{
  KeySym *k;
  int    i;

  /*
   * compute the modifier map
   */
  for (i = 0; i < MAP_LENGTH; i++)
    pModMap[i] = NoSymbol;  /* make sure it is restored */
  
  for (k = map, i = MIN_KEYCODE;
       i < (NUM_KEYCODES + MIN_KEYCODE);
       i++, k += 4)
    
    switch(*k) {
      
    case XK_Shift_L:
    case XK_Shift_R:
      pModMap[i] = ShiftMask;
      break;
      
    case XK_Control_L:
    case XK_Control_R:
      pModMap[i] = ControlMask;
      break;
      
    case XK_Caps_Lock:
      pModMap[i] = LockMask;
      break;
      
    case XK_Alt_L:
      pModMap[i] = AltMask;
      break;
      
    case XK_Num_Lock:
#ifdef notdef
      pModMap[i] = NumLockMask;
#endif
      break;

    case XK_Scroll_Lock:
      pModMap[i] = ScrollLockMask;
      break;

      /* kana support */
    case XK_Kana_Lock:
    case XK_Kana_Shift:
      pModMap[i] = KanaMask;
      break;

      /* alternate troggle for multinational support */
    case XK_Alt_R:
    case XK_Mode_switch:
      pModMap[i] = AltLangMask;
      break;

    }
  
  pKeySyms->map        = map;
  pKeySyms->mapWidth   = GLYPHS_PER_KEY;
  pKeySyms->minKeyCode = MIN_KEYCODE;
  pKeySyms->maxKeyCode = MAX_KEYCODE;
}
