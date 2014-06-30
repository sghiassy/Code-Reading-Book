/*
 * $XConsortium: aixInput.h /main/3 1995/12/05 15:43:45 matt $
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

#ifndef AIXINPUT_H
#define AIXINPUT_H

/* The following are defined in AIX.h */

#ifndef AIX_MIN_KEY
#define AIX_MIN_KEY  0x08
#endif
#ifndef AIX_MAX_KEY
#define AIX_MAX_KEY (0x85 + AIX_MIN_KEY)
#endif
#ifndef AIX_GLYPHS_PER_KEY
#define AIX_GLYPHS_PER_KEY 2
#endif

/*                                        Position
 *                          Base            Code
 */

#define Aix_Control_L    (AIX_MIN_KEY   +    0x3A)
#define Aix_Shift_L      (AIX_MIN_KEY   +    0x2C)
#define Aix_Shift_R      (AIX_MIN_KEY   +    0x39)
#define Aix_Caps_Lock    (AIX_MIN_KEY   +    0x1E)
#define Aix_Alt_L        (AIX_MIN_KEY   +    0x3C)
#define Aix_Alt_R        (AIX_MIN_KEY   +    0x3E)
#define Aix_Backspace    (AIX_MIN_KEY   +    0x0f)

#ifdef AIXps2
#define AIX_Control_R	 (AIX_MIN_KEY	+   0x40)
#endif

#define Aix_Num_Lock	 (AIX_MIN_KEY   +    0x5a)

#define NumLockMask	 Mod5Mask



/*------------------------------XTESTEXT1 stuff ---------------------------*/
#ifdef  XTESTEXT1

#define  XTestSERVER_SIDE
#include <X11/extensions/xtestext1.h>

extern  int     exclusive_steal;
extern  short   xtest_mousex;		/* defined in xtestext1di.c */
extern  short   xtest_mousey;		/* defined in xtestext1di.c */
extern  int     on_steal_input;		/* defined in xtestext1di.c */
					/* steal input mode is on. */
extern  short   xtest_mousex;		/* defined in xtestext1di.c */
extern  short   xtest_mousey;		/* defined in xtestext1di.c */

extern  Bool	XTestStealKeyData();
extern KeyCode xtest_command_key;


/**
 **  XTEST1 device ID.
 **/
#define XE_MOUSE 1			/* mouse */
#define XE_DKB   2			/* main keyboard */


#define XTEST1_STEAL_MOUSE_EVENT(e) { if (!on_steal_input ||	   \
		XTestStealKeyData(e.u.u.detail,			   \
				  e.u.u.type,			   \
				  XE_MOUSE,			   \
				  AIXCurrentX, AIXCurrentY));	   \
			           }


#define XTEST1_STEAL_KEY_EVENT(e) { if (!on_steal_input ||	\
		XTestStealKeyData(e.u.u.detail, e.u.u.type,	\
				  XE_DKB,			\
				  AIXCurrentX, AIXCurrentY));	\
				 }


#define XTEST1_STEAL_MOTION_DATA(x, y) {			\
	if (on_steal_input)					\
        {							\
           /*							\
            * only call if the mouse position			\
	    * has actually moved.				\
            */							\
           if (((x) != xtest_mousex) || ((y) != xtest_mousey))	\
           {							\
                XTestStealMotionData(((x) - xtest_mousex),	\
                                     ((y) - xtest_mousey),	\
                                      XE_MOUSE,			\
                                      xtest_mousex,		\
                                      xtest_mousey);		\
           }							\
        }							\
}


#else

#define XTEST1_STEAL_KEY_EVENT(e) {}
#define XTEST1_STEAL_MOUSE_EVENT(e) {}
#define XTEST1_STEAL_MOTION_DATA(x,y) {}

#endif /* XTESTEXT1 */

/*------------------------------XTESTEXT1 stuff ---------------------------*/


#endif /* AIXINPUT_H */
