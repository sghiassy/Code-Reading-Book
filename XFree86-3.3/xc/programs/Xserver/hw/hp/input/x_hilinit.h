#ifndef X_HILINIT_H
#define X_HILINIT_H
/* $XConsortium: x_hilinit.h,v 8.29 94/04/17 20:30:18 rws Exp $ */
/*

Copyright (c) 1986, 1987  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.


Copyright (c) 1986, 1987 by Hewlett-Packard Company

Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name of Hewlett-Packard not be used in
advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE.  Hewlett-Packard shall not be liable for errors 
contained herein or direct, indirect, special, incidental or 
consequential damages in connection with the furnishing, 
performance, or use of this material.

This software is not subject to any license of the American
Telephone and Telegraph Company or of the Regents of the
University of California.

*/

/**************************************************************************
 *
 * file: x_hilinit.h
 *
 * contains key definitions and other static information used by x_hilinit.c
 *
 */
 
#define	STARTUP			0
#define	MAX_STRINGS		(sizeof (strings) / sizeof (struct str_table))
#define	MAX_DEV_TYPES		(sizeof (devices) / sizeof (struct dev_table))
#define	MAX_POINTER_FUNCS	(sizeof (pointerfunc) / sizeof (struct pointerfunc))
#define	QUAD_INDEX		8	/* index of quad entry in dev_table */
#define	NINE_KNOB_ID		0x61
#define	KEY			0
#define	UCHAR_NUMBER		1
#define	STRING			2
#define	MODIFIER		3
#define	USHORT_NUMBER		4
#define	UINT_NUMBER		5

#include "keysym.h"
#include "HPkeysym.h"
#include "ap_keysym.h"

struct	opendevs
    {
    int	type;
    int	pos;
    int	use;
    char path[MAXNAMLEN+1];
    char name[MAXNAMLEN+1];
    char entry[MAXNAMLEN+1];
    };

struct	dev_table
    {
    int			lowid;
    int			highid;
    int			dev_type;
    int			x_type;
    char		*name;
    unsigned char	num_keys;
    unsigned char	min_kcode;
    unsigned char	max_kcode;
    };

struct	dev_table  devices[] =
	{{0x00,0x1f,KEYPAD,KEYBOARD,"KEYPAD",0,0,0},
	 {0x2f,0x2f,APOLLO_LPFK,KEYBOARD,"LPFK-BUTTONBOX",33,64,32},
	 {0x30,0x33,BUTTONBOX,KEYBOARD,"BUTTONBOX",32,10,41},
	 {0x34,0x34,ID_MODULE,XOTHER,"ID_MODULE",0,0,0},
	 {0x35,0x3f,BUTTONBOX,KEYBOARD,"BUTTONBOX",32,10,41},
	 {0x5c,0x5f,BARCODE,KEYBOARD,"BARCODE",109,10,135},
	 {0x60,0x60,ONE_KNOB,MOUSE,"ONE_KNOB",0,0,0},
	 {0x61,0x61,NINE_KNOB,MOUSE,"NINE_KNOB",0,0,0},
	 {0x62,0x67,QUADRATURE,MOUSE,"QUADRATURE",0,0,0},
	 {0x68,0x6b,MOUSE,MOUSE,"MOUSE",0,0,0},
	 {0x6c,0x6f,TRACKBALL,MOUSE,"TRACKBALL",0,0,0},
	 {0x70,0x70,VCD_8_DIALBOX,MOUSE,"KNOB_BOX",0,0,0},
	 {0x71,0x71,SS_SPACEBALL,MOUSE,"SPACEBALL",0,0,0},
	 {0x88,0x8b,TOUCHPAD,MOUSE,"TOUCHPAD",0,0,0},
	 {0x8c,0x8f,TOUCHSCREEN,MOUSE,"TOUCHSCREEN",0,0,0},
	 {0x90,0x97,TABLET,MOUSE,"TABLET",0,0,0},
	 {0x98,0x98,MMII_1812_TABLET,MOUSE,"MMII-TABLET",0,0,0},
	 {0x99,0x99,MMII_1201_TABLET,MOUSE,"MMII-TABLET",0,0,0},
	 {0xA0,0xBF,KEYBOARD,KEYBOARD,"KEYBOARD",93,10,135},
	 {0xC0,0xDF,KEYBOARD,KEYBOARD,"KEYBOARD",109,10,135},
	 {0xE0,0xFF,KEYBOARD,KEYBOARD,"KEYBOARD",87,10,135},
	 {0x00,0x00,NULL_DEVICE,NULL_DEVICE,"NULL",0,0,0}};

char	*position[] = 
    {
    "FIRST",
    "SECOND",
    "THIRD",
    "FOURTH",
    "FIFTH",
    "SIXTH",
    "SEVENTH"
    };

char	*suffix[] =
#if defined(__hp9000s300) || defined(__hp9000s700) || defined(__hp_osf)
    {"1","2","3","4","5","6","7"};
#else
    {"0.1","0.2","0.3","0.4","0.5","0.6","0.7",
     "1.1","1.2","1.3","1.4","1.5","1.6","1.7",
     "2.1","2.2","2.3","2.4","2.5","2.6","2.7",
     "3.1","3.2","3.3","3.4","3.5","3.6","3.7"};
#endif /* __hp9000s300 or __hp9000s700 */

extern	u_char	cursor_down;
extern	u_char	cursor_left;
extern	u_char	cursor_right;
extern	u_char	cursor_up;
extern	u_char	button_1;
extern	u_char	button_2;
extern	u_char	button_3;
extern	u_char	button_4;
extern	u_char	button_5;
extern	u_char	button_6;
extern	u_char	button_7;
extern	u_char	button_8;

extern	u_char	pointer_key_mods[3];
extern	u_char	pointer_amt_mods[3];
extern	u_char	reset_mods[3];
extern	u_char	borrow_mode_mods[3];
extern	u_short	pointer_move;
extern	u_short	pointer_mod1_amt;
extern	u_short	pointer_mod2_amt;
extern	u_short	pointer_mod3_amt;
extern	u_char	borrow_mode;
extern	u_char	reset;
extern	u_char	screen_change_amt;
extern	u_char	button_chording;
extern	u_char	button_latching;
extern	u_char	ptr_button_map[];
extern	u_char	isotropic_scaling;
extern	u_char	screen_orientation;
extern	u_char	screen_row_wrap;
extern	u_char	screen_col_wrap;
extern	u_int	tablet_xorigin;
extern	u_int	tablet_yorigin;
extern	u_int	tablet_width;
extern	u_int	tablet_height;
    
struct	pointerfunc
    {
    char	*name;
    u_char	*code;
    int		type;
    };

struct	pointerfunc pointerfunc [] =
	{{"POINTER_LEFT_KEY", &cursor_left, KEY},
	 {"POINTER_RIGHT_KEY", &cursor_right, KEY},
	 {"POINTER_UP_KEY", &cursor_up, KEY},
	 {"POINTER_DOWN_KEY", &cursor_down, KEY},
	 {"POINTER_KEY_MOD1", &pointer_key_mods[0], MODIFIER},
	 {"POINTER_KEY_MOD2", &pointer_key_mods[1], MODIFIER},
	 {"POINTER_KEY_MOD3", &pointer_key_mods[2], MODIFIER},
	 {"POINTER_BUTTON1_KEY", &button_1, KEY},
	 {"POINTER_BUTTON2_KEY", &button_2, KEY},
	 {"POINTER_BUTTON3_KEY", &button_3, KEY},
	 {"POINTER_BUTTON4_KEY", &button_4, KEY},
	 {"POINTER_BUTTON5_KEY", &button_5, KEY},
	 {"POINTER_BUTTON6_KEY", &button_6, KEY},
	 {"POINTER_BUTTON7_KEY", &button_7, KEY},
	 {"POINTER_BUTTON8_KEY", &button_8, KEY},
	 {"POINTER_MOVE", (u_char *) &pointer_move, USHORT_NUMBER},
	 {"POINTER_MOD1_AMT", (u_char *) &pointer_mod1_amt, USHORT_NUMBER},
	 {"POINTER_MOD2_AMT", (u_char *) &pointer_mod2_amt, USHORT_NUMBER},
	 {"POINTER_MOD3_AMT", (u_char *) &pointer_mod3_amt, USHORT_NUMBER},
#ifdef __apollo
	 {"BORROW_MODE_KEY", &borrow_mode, KEY},
	 {"BORROW_MODE_MOD1_KEY", &borrow_mode_mods[0], MODIFIER},
	 {"BORROW_MODE_MOD2_KEY", &borrow_mode_mods[1], MODIFIER},
#endif /* __apollo */
	 {"RESET", &reset, KEY},
	 {"RESET_MOD1", &reset_mods[0], MODIFIER},
	 {"RESET_MOD2", &reset_mods[1], MODIFIER},
	 {"RESET_MOD3", &reset_mods[2], MODIFIER},
	 {"POINTER_AMT_MOD1", &pointer_amt_mods[0], MODIFIER},
	 {"POINTER_AMT_MOD2", &pointer_amt_mods[1], MODIFIER},
	 {"POINTER_AMT_MOD3", &pointer_amt_mods[2], MODIFIER},
	 {"BUTTON_1_VALUE", &ptr_button_map[1], UCHAR_NUMBER},
	 {"BUTTON_2_VALUE", &ptr_button_map[2], UCHAR_NUMBER},
	 {"BUTTON_3_VALUE", &ptr_button_map[3], UCHAR_NUMBER},
	 {"BUTTON_4_VALUE", &ptr_button_map[4], UCHAR_NUMBER},
	 {"BUTTON_5_VALUE", &ptr_button_map[5], UCHAR_NUMBER},
	 {"BUTTON_6_VALUE", &ptr_button_map[6], UCHAR_NUMBER},
	 {"BUTTON_7_VALUE", &ptr_button_map[7], UCHAR_NUMBER},
	 {"BUTTON_8_VALUE", &ptr_button_map[8], UCHAR_NUMBER},
	 {"SCREEN_CHANGE_AMT", &screen_change_amt, UCHAR_NUMBER},
	 {"BUTTON_CHORDING", &button_chording, STRING},
	 {"BUTTON_LATCHING", &button_latching, STRING},
	 {"TABLET_SUBSET_XORIGIN", (u_char *) &tablet_xorigin, UINT_NUMBER},
	 {"TABLET_SUBSET_YORIGIN", (u_char *) &tablet_yorigin, UINT_NUMBER},
	 {"TABLET_SUBSET_WIDTH", (u_char *) &tablet_width, UINT_NUMBER},
	 {"TABLET_SUBSET_HEIGHT", (u_char *) &tablet_height, UINT_NUMBER},
	 {"ISOTROPIC_SCALING", &isotropic_scaling, STRING},
	 {"SCREEN_ORIENTATION", &screen_orientation, STRING},
	 {"SCREEN_ROW_WRAP", &screen_row_wrap, STRING},
	 {"SCREEN_COL_WRAP", &screen_col_wrap, STRING}};

struct str_table
    {
    char *string;
    u_char value;
    } strings [] = {
    {"OFF",CHORDING_OFF},
    {"ON",CHORDING_ON},
    {"DEFAULT",CHORDING_DEFAULT},
    {"WRAP",WRAP},
    {"NOWRAP",NOWRAP},
    {"SAMESCREEN",SAMESCREEN},
    {"CHANGE_BY_TWO",CHANGE_BY_TWO},
    {"VERTICAL",VERTICAL},
    {"HORIZONTAL",HORIZONTAL},
    {"MATRIX",MATRIX}};

struct _keyset1 {
    char *keystr;
    KeySym sym;
    } keyset1[] = {
    {"ALT_L", XK_Alt_L},
    {"ALT_R", XK_Alt_R},
    {"LEFT_CONTROL", XK_Control_L},
    {"RIGHT_CONTROL", XK_Control_R},
    {"RIGHT_EXTEND", XK_Meta_R},
    {"LEFT_EXTEND", XK_Meta_L},
    {"RIGHT_SHIFT", XK_Shift_R},
    {"LEFT_SHIFT", XK_Shift_L},
    {"CONTROL", XK_Control_L},
    {"KEYPAD_4", XK_KP_4},
    {"KEYPAD_8", XK_KP_8},
    {"KEYPAD_5", XK_KP_5},
    {"KEYPAD_9", XK_KP_9},
    {"KEYPAD_6", XK_KP_6},
    {"KEYPAD_7", XK_KP_7},
    {"KEYPAD_COMMA", XK_KP_Separator},
    {"KEYPAD_ENTER", XK_KP_Enter},
    {"KEYPAD_1", XK_KP_1},
    {"KEYPAD_/", XK_KP_Divide},
    {"KEYPAD_2", XK_KP_2},
    {"KEYPAD_+", XK_KP_Add},
    {"KEYPAD_3", XK_KP_3},
    {"KEYPAD_*", XK_KP_Multiply},
    {"KEYPAD_0", XK_KP_0},
    {"KEYPAD_-", XK_KP_Subtract},
    {"BLANK_F10", XK_F10},
    {"BLANK_F11", XK_F11},
    {"KEYPAD_PERIOD", XK_KP_Decimal},
    {"BLANK_F9", XK_F9},
    {"KEYPAD_TAB", XK_KP_Tab},
    {"BLANK_F12", XK_F12},
    {"`", XK_quoteleft},
    {"STOP", XK_Cancel},
    {"ENTER", XK_Execute},
    {"CLEAR_LINE", XK_ClearLine},
    {"CLEAR_DISPLAY", XK_Clear},
    {"-", XK_minus},
    {"=", XK_equal},
    {"INSERT_LINE", XK_InsertLine},
    {"DELETE_LINE", XK_DeleteLine},
    {"[", XK_bracketleft},
    {"]", XK_bracketright},
    {"\\", XK_backslash},
    {"INSERT_CHAR", XK_InsertChar},
    {"DELETE_CHAR", XK_DeleteChar},
    {";", XK_semicolon},
    {"'", XK_quoteright},
    {"HOME_CURSOR", XK_Home},
    {"PREV", XK_Prior},
    {",", XK_comma},
    {".", XK_period},
    {"/", XK_slash},
    {"SPACE_BAR", XK_space},
    {".", XK_period},
    {"CURSOR_LEFT", XK_Left},
    {"CURSOR_DOWN", XK_Down},
    {"CURSOR_UP", XK_Up},
    {"CURSOR_RIGHT", XK_Right},
    {"CONTROL_R", XK_Control_R},
    {"META_R", XK_Meta_R},
    {"META_L", XK_Meta_L},
    {"SHIFT_R", XK_Shift_R},
    {"SHIFT_L", XK_Shift_L},
    {"CONTROL_L", XK_Control_L},
    {"BREAK", XK_Break},
    {"KP_4", XK_KP_4},
    {"KP_8", XK_KP_8},
    {"KP_5", XK_KP_5},
    {"KP_9", XK_KP_9},
    {"KP_6", XK_KP_6},
    {"KP_7", XK_KP_7},
    {"KP_SEPARATOR", XK_KP_Separator},
    {"KP_ENTER", XK_KP_Enter},
    {"KP_1", XK_KP_1},
    {"KP_DIVIDE", XK_KP_Divide},
    {"KP_2", XK_KP_2},
    {"KP_ADD", XK_KP_Add},
    {"KP_3", XK_KP_3},
    {"KP_MULTIPLY", XK_KP_Multiply},
    {"KP_0", XK_KP_0},
    {"KP_SUBTRACT", XK_KP_Subtract},
    {"B", XK_B},
    {"V", XK_V},
    {"C", XK_C},
    {"X", XK_X},
    {"Z", XK_Z},
    {"ESCAPE", XK_Escape},
    {"F10", XK_F10},
    {"F11", XK_F11},
    {"KP_DECIMAL", XK_KP_Decimal},
    {"F9", XK_F9},
    {"KP_TAB", XK_KP_Tab},
    {"F12", XK_F12},
    {"H", XK_H},
    {"G", XK_G},
    {"F", XK_F},
    {"D", XK_D},
    {"S", XK_S},
    {"A", XK_A},
    {"NOSYMBOL", 0},
    {"CAPS_LOCK", XK_Caps_Lock},
    {"U", XK_U},
    {"Y", XK_Y},
    {"T", XK_T},
    {"R", XK_R},
    {"E", XK_E},
    {"W", XK_W},
    {"Q", XK_Q},
    {"TAB", XK_Tab},
    {"7", XK_7},
    {"6", XK_6},
    {"5", XK_5},
    {"4", XK_4},
    {"3", XK_3},
    {"2", XK_2},
    {"1", XK_1},
    {"QUOTELEFT", XK_quoteleft},
    {"MENU", XK_Menu},
    {"F4", XK_F4},
    {"F3", XK_F3},
    {"F2", XK_F2},
    {"F1", XK_F1},
    {"CANCEL", XK_Cancel},
    {"EXECUTE", XK_Execute},
    {"SYSTEM", XK_System},
    {"F5", XK_F5},
    {"F6", XK_F6},
    {"F7", XK_F7},
    {"F8", XK_F8},
    {"CLEARLINE", XK_ClearLine},
    {"CLEAR", XK_Clear},
    {"8", XK_8},
    {"9", XK_9},
    {"0", XK_0},
    {"MINUS", XK_minus},
    {"EQUAL", XK_equal},
    {"BACKSPACE", XK_BackSpace},
    {"INSERTLINE", XK_InsertLine},
    {"DELETELINE", XK_DeleteLine},
    {"I", XK_I},
    {"O", XK_O},
    {"P", XK_P},
    {"BRACKETLEFT", XK_bracketleft},
    {"BRACKETRIGHT", XK_bracketright},
    {"BACKSLASH", XK_backslash},
    {"INSERTCHAR", XK_InsertChar},
    {"DELETECHAR", XK_DeleteChar},
    {"J", XK_J},
    {"K", XK_K},
    {"L", XK_L},
    {"SEMICOLON", XK_semicolon},
    {"QUOTERIGHT", XK_quoteright},
    {"RETURN", XK_Return},
    {"HOME", XK_Home},
    {"PRIOR", XK_Prior},
    {"M", XK_M},
    {"COMMA", XK_comma},
    {"PERIOD", XK_period},
    {"SLASH", XK_slash},
    {"SELECT", XK_Select},
    {"NEXT", XK_Next},
    {"N", XK_N},
    {"SPACE", XK_space},
    {"LEFT", XK_Left},
    {"DOWN", XK_Down},
    {"UP", XK_Up},
    {"RIGHT", XK_Right},
    {"GRAVE", XK_grave},
    {"NUMBERSIGN", XK_numbersign},
    {"KANJI", XK_Kanji},
    {"GUILLEMOTLEFT", XK_guillemotleft},
    {"EACUTE", XK_eacute},
    {"PRINT", XK_Print},
    {"ASCIICIRCUM", XK_asciicircum},
    {"SCROLL_LOCK", XK_Scroll_Lock},
    {"CEDILLA", XK_cedilla},
    {"NUM_LOCK", XK_Num_Lock},
#ifdef __apollo
    {"APCHARDEL", apXK_CharDel},
    {"REDO", XK_Redo},
    {"APREAD", apXK_Read},
    {"APEDIT", apXK_Edit},
    {"APEXIT", apXK_Exit},
    {"PAUSE", XK_Pause},
    {"APCOPY", apXK_Copy},
    {"APPASTE", apXK_Paste},
    {"APGROW", apXK_Grow},
    {"APLEFTBAR", apXK_LeftBar},
    {"APCMD", apXK_Cmd},
    {"APRIGHTBAR", apXK_RightBar},
    {"DELETE", XK_Delete},
    {"APLEFTBOX", apXK_LeftBox},
    {"APRIGHTBOX", apXK_RightBox},
    {"APOSTROPHE", XK_apostrophe},
    {"APREPEAT", apXK_Repeat},
    {"APPOP", apXK_Pop},
    {"APUPBOX", apXK_UpBox},
    {"APDOWNBOX", apXK_DownBox},
#endif /* __apollo */
    {"", 0}};

#endif /* X_HILINIT_H */
