/*
 * $XConsortium: aixKeymap.h,v 1.3 91/11/22 17:06:56 eswu Exp $
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

#ifndef AIXKEYMAP_H
#define AIXKEYMAP_H

KeySym aixmap[MAP_LENGTH*AIX_GLYPHS_PER_KEY] = {
    /* 0x00 */  NoSymbol,       NoSymbol,
    /* 0x01 */  XK_quoteleft,   XK_asciitilde,
    /* 0x02 */  XK_1,           XK_exclam,
    /* 0x03 */  XK_2,           XK_at,
    /* 0x04 */  XK_3,           XK_numbersign,
    /* 0x05 */  XK_4,           XK_dollar,
    /* 0x06 */  XK_5,           XK_percent,
    /* 0x07 */  XK_6,           XK_asciicircum,
    /* 0x08 */  XK_7,           XK_ampersand,
    /* 0x09 */  XK_8,           XK_asterisk,
    /* 0x0a */  XK_9,           XK_parenleft,
    /* 0x0b */  XK_0,           XK_parenright,
    /* 0x0c */  XK_minus,       XK_underscore,
    /* 0x0d */  XK_equal,       XK_plus,
    /* 0x0e */  XK_yen,         NoSymbol,
    /* 0x0f */  XK_BackSpace,   NoSymbol,
    /* 0x10 */  XK_Tab,         NoSymbol,
    /* 0x11 */  XK_q,           XK_Q,
    /* 0x12 */  XK_w,           XK_W,
    /* 0x13 */  XK_e,           XK_E,
    /* 0x14 */  XK_r,           XK_R,
    /* 0x15 */  XK_t,           XK_T,
    /* 0x16 */  XK_y,           XK_Y,
    /* 0x17 */  XK_u,           XK_U,
    /* 0x18 */  XK_i,           XK_I,
    /* 0x19 */  XK_o,           XK_O,
    /* 0x1a */  XK_p,           XK_P,
    /* 0x1b */  XK_bracketleft, XK_braceleft,
    /* 0x1c */  XK_bracketright,XK_braceright,
    /* 0x1d */  XK_backslash,   XK_bar,
    /* 0x1e */  XK_Caps_Lock,   NoSymbol,
    /* 0x1f */  XK_a,           XK_A,
    /* 0x20 */  XK_s,           XK_S,
    /* 0x21 */  XK_d,           XK_D,
    /* 0x22 */  XK_f,           XK_F,
    /* 0x23 */  XK_g,           XK_G,
    /* 0x24 */  XK_h,           XK_H,
    /* 0x25 */  XK_j,           XK_J,
    /* 0x26 */  XK_k,           XK_K,
    /* 0x27 */  XK_l,           XK_L,
    /* 0x28 */  XK_semicolon,   XK_colon,
    /* 0x29 */  XK_quoteright,  XK_quotedbl,
    /* 0x2a */  XK_asterisk,    XK_mu,
    /* 0x2b */  XK_Return,      XK_Return,
    /* 0x2c */  XK_Shift_L,     NoSymbol,
    /* 0x2d */  XK_less,        XK_greater,
    /* 0x2e */  XK_z,           XK_Z,
    /* 0x2f */  XK_x,           XK_X,
    /* 0x30 */  XK_c,           XK_C,
    /* 0x31 */  XK_v,           XK_V,
    /* 0x32 */  XK_b,           XK_B,
    /* 0x33 */  XK_n,           XK_N,
    /* 0x34 */  XK_m,           XK_M,
    /* 0x35 */  XK_comma,       XK_less,
    /* 0x36 */  XK_period,      XK_greater,
    /* 0x37 */  XK_slash,       XK_question,
    /* 0x38 */  XK_underscore,  XK_underscore,
    /* 0x39 */  XK_Shift_R,     NoSymbol,
    /* 0x3a */  XK_Control_L,   NoSymbol,
    /* 0x3b */  NoSymbol,       NoSymbol,
    /* 0x3c */  XK_Alt_L,       NoSymbol,
    /* 0x3d */  XK_space,       NoSymbol,
    /* 0x3e */  XK_Alt_R,       NoSymbol,
    /* 0x3f */  NoSymbol,       NoSymbol,
    /* 0x40 */  XK_Execute,     NoSymbol,       /* Action - XXX */
    /* 0x41 */  NoSymbol,       NoSymbol,
    /* 0x42 */  NoSymbol,       NoSymbol,
    /* 0x43 */  NoSymbol,       NoSymbol,
    /* 0x44 */  NoSymbol,       NoSymbol,
    /* 0x45 */  NoSymbol,       NoSymbol,
    /* 0x46 */  NoSymbol,       NoSymbol,
    /* 0x47 */  NoSymbol,       NoSymbol,
    /* 0x48 */  NoSymbol,       NoSymbol,
    /* 0x49 */  NoSymbol,       NoSymbol,
    /* 0x4a */  NoSymbol,       NoSymbol,
    /* 0x4b */  XK_Insert,      NoSymbol,
    /* 0x4c */  XK_Delete,      NoSymbol,
    /* 0x4d */  NoSymbol,       NoSymbol,
    /* 0x4e */  NoSymbol,       NoSymbol,
    /* 0x4f */  XK_Left,        NoSymbol,
    /* 0x50 */  XK_Home,        NoSymbol,
    /* 0x51 */  XK_End,         NoSymbol,
    /* 0x52 */  NoSymbol,       NoSymbol,
    /* 0x53 */  XK_Up,          NoSymbol,       /* up arrow */
    /* 0x54 */  XK_Down,        NoSymbol,       /* down arrow */
    /* 0x55 */  XK_Prior,       NoSymbol,       /* page up */
    /* 0x56 */  XK_Next,        NoSymbol,       /* page down */
    /* 0x57 */  NoSymbol,       NoSymbol,
    /* 0x58 */  NoSymbol,       NoSymbol,
    /* 0x59 */  XK_Right,       NoSymbol,       /* right arrow */
    /* 0x5a */  XK_Num_Lock,    NoSymbol,       /* num lock */
    /* 0x5b */  XK_KP_7,        NoSymbol,
    /* 0x5c */  XK_KP_4,        NoSymbol,
    /* 0x5d */  XK_KP_1,        NoSymbol,
    /* 0x5e */  NoSymbol,       NoSymbol,
    /* 0x5f */  XK_KP_Divide,   NoSymbol,
    /* 0x60 */  XK_KP_8,        NoSymbol,
    /* 0x61 */  XK_KP_5,        NoSymbol,
    /* 0x62 */  XK_KP_2,        NoSymbol,
    /* 0x63 */  XK_KP_0,        NoSymbol,
    /* 0064 */  XK_KP_Multiply, NoSymbol,
    /* 0x65 */  XK_KP_9,        NoSymbol,
    /* 0x66 */  XK_KP_6,        NoSymbol,
    /* 0x67 */  XK_KP_3,        NoSymbol,
    /* 0x68 */  XK_KP_Decimal,  NoSymbol,
    /* 0x69 */  XK_KP_Subtract, NoSymbol,
    /* 0x6a */  XK_KP_Add,      NoSymbol,
    /* 0x6b */  NoSymbol,       NoSymbol,
    /* 0x6c */  XK_KP_Enter,    NoSymbol,
    /* 0x6d */  NoSymbol,       NoSymbol,
    /* 0x6e */  XK_Escape,      NoSymbol,
    /* 0x6f */  NoSymbol,       NoSymbol,
    /* 0x70 */  XK_F1,          NoSymbol,
    /* 0x71 */  XK_F2,          NoSymbol,
    /* 0x72 */  XK_F3,          NoSymbol,
    /* 0x73 */  XK_F4,          NoSymbol,
    /* 0x74 */  XK_F5,          NoSymbol,
    /* 0x75 */  XK_F6,          NoSymbol,
    /* 0x76 */  XK_F7,          NoSymbol,
    /* 0x77 */  XK_F8,          NoSymbol,
    /* 0x78 */  XK_F9,          NoSymbol,
    /* 0x79 */  XK_F10,         NoSymbol,
    /* 0x7a */  XK_F11,         NoSymbol,
    /* 0x7b */  XK_F12,         NoSymbol,
    /* 0x7c */  XK_Print,       NoSymbol,       /* print screen */
    /* 0x7d */  XK_Cancel,      NoSymbol,       /* scroll lock - XXX */
    /* 0x7e */  XK_Pause,       NoSymbol,       /* pause */
    /* 0x7f */  NoSymbol,       NoSymbol,
    /* 0x80 */  NoSymbol,       NoSymbol,
    /* 0x81 */  NoSymbol,       NoSymbol,
    /* 0x82 */  NoSymbol,       NoSymbol,
    /* 0x83 */  XK_Multi_key,   NoSymbol,
    /* 0x84 */  XK_Kanji,       NoSymbol,
    /* 0x85 */  XK_Mode_switch, NoSymbol
};

#endif /* AIXKEYMAP_H */





