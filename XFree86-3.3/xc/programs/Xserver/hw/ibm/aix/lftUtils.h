/*
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
#ifdef _IBM_LFT

#include <sys/inputdd.h>

#define SKYWAY_ID 	0x04210000
extern char *get_rcm_path();
extern int get_rcm_fd();
extern int init_lft_disp_info();
extern char *get_disp_name_by_number();
extern unsigned long get_lft_disp_id();
extern int get_num_lft_displays();

/* For Input handling */

#define INPUT_RING_SIZE 	4096

typedef struct {
	int fd;
	
	void (*eventHandler)();
} InputDevPrivate;

#define MOUSE_ID 	0
#define KEYBOARD_ID	1

extern InputDevPrivate lftInputInfo[];
extern struct inputring *lft_input_ring; /* Need to initialize */

extern int ms2_qhead, ms2_qtail;        /* 2 buttons mouse check */

#define BUTTONMAPSIZE 4
#define NONE    0x00
#define LEFT    Button1
#define MIDDLE  Button2
#define RIGHT   Button3

#define UP      ButtonRelease
#define DOWN    ButtonPress

#define MOUSEBUTTONS (MOUSEBUTTON1|MOUSEBUTTON3) /* left & right */


#endif  /* _IBM_LFT */

	
