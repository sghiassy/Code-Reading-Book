/* $XConsortium: t89_driver.h /main/3 1996/02/21 18:08:07 kaleb $ */
/*
 * Copyright 1995 by Alan Hourihane, Wigan, England.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Alan Hourihane not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Alan Hourihane makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ALAN HOURIHANE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ALAN HOURIHANE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Alan Hourihane, alanh@fairlite.demon.co.uk
 */
/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/tvga8900/t89_driver.h,v 3.9.2.3 1997/05/18 12:00:20 dawes Exp $ */

/*
 * Trident Chipset Definitions
 */

#define TVGA8200LX	0
#define TVGA8800CS	1
#define TVGA8900B	2
#define TVGA8900C	3
#define TVGA8900CL	4
#define TVGA8900D	5
#define TVGA9000	6
#define TVGA9000i	7
#define TVGA9100B	8
#define TVGA9200CXr	9
#define TGUI9320LCD	10
#define TGUI9400CXi	11
#define TGUI9420	12
#define TGUI9420DGi	13
#define TGUI9430DGi	14
#define TGUI9440AGi	15
#define TGUI96xx	16
#define CYBER938x	17

#ifdef INITIALIZE_LIMITS
/* Clock Limits */
int tridentClockLimit[] = {
	80000,
	80000,
	80000,
	80000,
	80000,
	80000,
	80000,
	80000,
	80000,
	80000,
	80000,
	80000,
	80000,
	80000,
	80000,
	90000,
	135000,
};

int tridentClockLimit16bpp[] = {
	40000,
	40000,
	40000,
	40000,
	40000,
	40000,
	40000,
	40000,
	40000,
	40000,
	40000,
	40000,
	40000,
	40000,
	40000,
	45000,
	135000,
}; 

int tridentClockLimit32bpp[] = {
	25180,
	25180,
	25180,
	25180,
	25180,
	25180,
	25180,
	25180,
	25180,
	25180,
	25180,
	25180,
	25180,
	25180,
	25180,
	25180,
	70000,
};

#else

extern int tridentClockLimit[];
extern int tridentClockLimit16bpp[];
extern int tridentClockLimit32bpp[];

#endif


/*
 * Trident DAC's
 */

#define TKD8001		0
#define TGUIDAC		1

extern int TVGAchipset;
extern Bool IsCyber;
