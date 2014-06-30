/* $XFree86: xc/programs/Xserver/hw/xfree86/common/coKeynames.h,v 3.3 1996/12/23 06:43:16 dawes Exp $ */
/*
 * This file is derived from "atKeynames.h"
 * Written by Holger Veit (veit@du9ds3.uni-duisburg.de)
 * Modified to provide a translation table between codrv's scancodes, and
 * the keycodes defined in "atKeynames.h" (David Dawes)
 */
/* $XConsortium: coKeynames.h /main/5 1996/02/21 17:37:43 kaleb $ */

#ifndef _COKEYNAMES_H
#define _COKEYNAMES_H

/*
 * See also note in atKeynames.h
 * This file is an experiment to standardize the three different sets of
 * keycodes. Set1 is known to be nice, but only for AT/84 keyboards, because
 * it delivers single byte sequences, with bit 7 set for release and reset
 * for press. Unfortunately, the 101/102 keyboard requires an E0/E1 prefix
 * for the additional keys. Set2 is a hack in a similiar way, but the 
 * release code can be recognized by the byte F0 (in most cases ;-) ). 
 * The length of a press or release sequence differs between 1 and 4 bytes,
 * which is bad. Set3 comes close to the best solution, but is not available 
 * with AT/84 keyboards.
 * The optimum would be a single byte code for press and release, which is 
 * possible for keyboards with <= 127 keys (leave one idle code).
 * So I try out this idea: Leave the whole keyboard processing to the
 * *NIX kernel. The kernel delivers an IBM keynumber, which has nothing to do
 * with any scan code, but is a unique ID of a key. The IBM numbering scheme
 * has not changed over the different keyboards, however, some keyboards
 * have additional national keys with a unique ID.
 *
 * For the 386bsd CO driver the following does only hold: There is a key
 * overloading feature, which allows mapping of national characters to
 * the keyboard. We want to have this feature propagated into the Xserver.
 * So the following mapping is the default mapping only. 
 * The XF86Keyboard code uses a special keycap database to get the real 
 * mapping (This could have been done by patching xmodmap as well).
 */

/*
 * -hv- Warning: This table has a totally different coding from
 * atKeynames.h, but uses the same names
 * I didn't want to rewrite the whole xf86 for this :=)
 *
 * definition of the AT84/MF101/MF102 Keyboard:
 * ============================================================
 * Defined             Key Cap Glyphs       Pressed value
 * Key Name            Main       Also       (hex)    (dec)
 * ----------------   ---------- -------    ------    ------
 */

unsigned char xf86CodrvMap[128] = {
#ifndef PC98
 0,
 KEY_Tilde,       /* ` (Accent)  ~ (Tilde) 0x01      1 */
 KEY_1,           /* 1           !         0x02      2 */
 KEY_2,           /* 2           @         0x03      3 */
 KEY_3,           /* 3           #         0x04      4 */
 KEY_4,           /* 4           $         0x05      5 */
 KEY_5,           /* 5           %         0x06      6 */
 KEY_6,           /* 6           ^         0x07      7 */
 KEY_7,           /* 7           &         0x08      8 */
 KEY_8,           /* 8           *         0x09      9 */
 KEY_9,           /* 9           (         0x0a     10 */
 KEY_0,           /* 0           )         0x0b     11 */
 KEY_Minus,       /* - (Minus)   _ (Under) 0x0c     12 */
 KEY_Equal,       /* = (Equal)   +         0x0d     13 */
 0,
 KEY_BackSpace,   /* Back Space            0x0f     15 */
 KEY_Tab,         /* Tab                   0x10     16 */
 KEY_Q,           /* Q                     0x11     17 */
 KEY_W,           /* W                     0x12     18 */
 KEY_E,           /* E                     0x13     19 */
 KEY_R,           /* R                     0x14     20 */
 KEY_T,           /* T                     0x15     21 */
 KEY_Y,           /* Y                     0x16     22 */
 KEY_U,           /* U                     0x17     23 */
 KEY_I,           /* I                     0x18     24 */
 KEY_O,           /* O                     0x19     25 */
 KEY_P,           /* P                     0x1a     26 */
 KEY_LBrace,      /* [           {         0x1b     27 */
 KEY_RBrace,      /* ]           }         0x1c     28 */
 KEY_BSlash,      /* \(BckSlash) |(VertBar)0x1d     29 */
 KEY_CapsLock,    /* CapsLock              0x1e     30 */
 KEY_A,           /* A                     0x1f     31 */
 KEY_S,           /* S                     0x20     32 */
 KEY_D,           /* D                     0x21     33 */
 KEY_F,           /* F                     0x22     34 */
 KEY_G,           /* G                     0x23     35 */
 KEY_H,           /* H                     0x24     36 */
 KEY_J,           /* J                     0x25     37 */
 KEY_K,           /* K                     0x26     38 */
 KEY_L,           /* L                     0x27     39 */
 KEY_SemiColon,   /* ;(SemiColon) :(Colon) 0x28     40 */
 KEY_Quote,       /* ' (Apostr)  " (Quote) 0x29     41 */
 0,
 KEY_Enter,       /* Enter                 0x2b     43 */
 KEY_ShiftL,      /* Shift(left)           0x2c     44 */
 0,
 KEY_Z,           /* Z                     0x2e     46 */
 KEY_X,           /* X                     0x2f     47 */
 KEY_C,           /* C                     0x30     48 */
 KEY_V,           /* V                     0x31     49 */
 KEY_B,           /* B                     0x32     50 */
 KEY_N,           /* N                     0x33     51 */
 KEY_M,           /* M                     0x34     52 */
 KEY_Comma,       /* , (Comma)   < (Less)  0x35     53 */
 KEY_Period,      /* . (Period)  >(Greater)0x36     54 */
 KEY_Slash,       /* / (Slash)   ?         0x37     55 */
 0,
 KEY_ShiftR,      /* Shift(right)          0x39     57 */
 KEY_LCtrl,       /* Ctrl(left)            0x3a     58 */
 0,
 KEY_Alt,         /* Alt(left)             0x3c     60 */
 KEY_Space,       /*   (SpaceBar)          0x3d     61 */
 KEY_AltLang,     /* AltLang(right)        0x3e     62 */
 0,
 KEY_RCtrl,       /* Ctrl(right)           0x40     64 */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 KEY_Insert,      /* Insert                0x4b     75 */
 KEY_Delete,      /* Delete                0x4c     76 */
 0, 0,
 KEY_Left,        /* Left                  0x4f     79 */
 KEY_Home,        /* Home                  0x50     80 */
 KEY_End,         /* End                   0x51     81 */
 0,
 KEY_Up,          /* Up                    0x53     83 */
 KEY_Down,        /* Down                  0x54     84 */
 KEY_PgUp,        /* PgUp                  0x55     85 */
 KEY_PgDown,      /* PgDown                0x56     86 */
 0, 0,
 KEY_Right,       /* Right                 0x59     89 */
 KEY_NumLock,     /* NumLock               0x5a     90 */
 KEY_KP_7,        /* 7           Home      0x5b     91 */
 KEY_KP_4,        /* 4           Left      0x5c     92 */
 KEY_KP_1,        /* 1           End       0x5d     93 */
 0,
 KEY_KP_Divide,   /* Divide                0x5f     95 */
 KEY_KP_8,        /* 8           Up        0x60     96 */
 KEY_KP_5,        /* 5                     0x61     97 */
 KEY_KP_2,        /* 2           Down      0x62     98 */
 KEY_KP_0,        /* 0           Insert    0x63     99 */
 KEY_KP_Multiply, /* *                     0x64    100 */
 KEY_KP_9,        /* 9           PgUp      0x65    101 */
 KEY_KP_6,        /* 6           Right     0x66    102 */
 KEY_KP_3,        /* 3           PgDown    0x67    103 */
 KEY_KP_Decimal,  /* . (Decimal) Delete    0x68    104 */
 KEY_KP_Minus,    /* - (Minus)             0x69    105 */
 KEY_KP_Plus,     /* + (Plus)              0x6a    106 */
 0,
 KEY_KP_Enter,    /* Enter                 0x6b    108 */
 0,
 KEY_Escape,      /* Escape                0x6d    110 */
 0,
 KEY_F1,          /* F1                    0x6e    112 */
 KEY_F2,          /* F2                    0x6f    113 */
 KEY_F3,          /* F3                    0x70    114 */
 KEY_F4,          /* F4                    0x71    115 */
 KEY_F5,          /* F5                    0x72    116 */
 KEY_F6,          /* F6                    0x73    117 */
 KEY_F7,          /* F7                    0x74    118 */
 KEY_F8,          /* F8                    0x75    119 */
 KEY_F9,          /* F9                    0x76    120 */
 KEY_F10,         /* F10                   0x77    121 */
 KEY_F11,         /* F11                   0x78    122 */
 KEY_F12,         /* F12                   0x79    123 */
 KEY_Print,       /* Print                 0x7a    124 */
 KEY_ScrollLock,  /* ScrollLock            0x7b    125 */
 KEY_SysReqest,   /* SysReqest             0x7c    126 */
#endif
 0
};

#endif /* _COKEYNAMES_H */
