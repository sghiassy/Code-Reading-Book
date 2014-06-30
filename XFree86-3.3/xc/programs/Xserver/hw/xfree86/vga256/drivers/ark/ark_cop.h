/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/ark/ark_cop.h,v 3.3 1996/12/26 01:39:17 dawes Exp $ */

/*
 * Definitions for ARK Logic coprocessor (COP).
 * We use linear COP addresses (no coordinates) to allow acceleration
 * with any linewidth.
 */

/* Macros for memory-mapped I/O COP register access. */

/* MMIO addresses (offset from base address, usually 0xB8000). */

#define BACKGROUNDCOLOR	0x00
#define FOREGROUNDCOLOR	0x02
#define BACKGROUNDCOLORHIGH 0x04
#define FOREGROUNDCOLORHIGH 0x06
#define TRANSPARENCYCOLOR 0x08
#define TRANSPARENCYCOLORMASK 0x0A
#define TRANSPARENCYCOLORHIGH 0x0C
#define TRANSPARENCYCOLORMASKHIGH 0x0E
#define COLORMIXSELECT	0x18
#define WRITEPLANEMASK	0x1A
#define ERRORTERM	0x50
#define AXIALERRORINCREMENT 0x54
#define STENCILPITCH	0x60
#define SOURCEPITCH	0x62
#define DESTPITCH	0x64
#define STENCILADDR	0x68
#define STENCILX	0x68
#define STENCILY	0x6A
#define SOURCEADDR	0x6C
#define SOURCEX		0x6C
#define SOURCEY		0x6E
#define DESTADDR	0x70
#define DESTX		0x70
#define DESTY		0x72
#define WIDTH		0x74
#define HEIGHT		0x76
#define BITMAPCONFIG	0x7C
#define COMMAND		0x7E

/* Flags for COMMAND register. */

#define DRAWNSTEP		0x0000
#define LINEDRAW		0x1000
#define BITBLT			0x2000
#define TEXTBITBLT		0x3000
#define USEPLANEMASK		0x0000
#define DISABLEPLANEMASK	0x0800
#define PATTERN8X8		0x0400
#define SELECTBGCOLOR		0x0000
#define BACKGROUNDBITMAP	0x0200
#define SELECTFGCOLOR		0x0000
#define FOREGROUNDBITMAP	0x0100
#define STENCILONES		0x0000
#define STENCILGENERATED	0x0040
#define STENCILBITMAP		0x0080
#define LINEDRAWALL		0x0000
#define LINESKIPFIRST		0x0010
#define LINESKIPLAST		0x0020
#define ENABLECLIPPING		0x0000
#define DISABLECLIPPING		0x0008
#undef DOWN
#define DOWN			0x0000
#undef UP
#define UP			0x0002
#define RIGHT			0x0000
#define LEFT			0x0004
#define DXGREATERTHANDY		0x0000
#define DYGREATERTHANDX		0x0001

/* Flags for Bitmap Configuration register. */

#define SWAPNIBLES		0x2000
#define SWAPBITS		0x1000
#define SYSTEMSTENCIL		0x0200
#define LINEARSTENCILADDR	0x0100
#define SYSTEMSOURCE		0x0020
#define LINEARSOURCEADDR	0x0010
#define	SYSTEMDEST		0x0002
#define LINEARDESTADDR		0x0001

/* Because of the non-optimal layout of the background and foreground */
/* colors, there are lots of macros for them. */

#define SETBACKGROUNDCOLOR(c) \
	*(unsigned short *)(arkMMIOBase + BACKGROUNDCOLOR) = c;

#define SETFOREGROUNDCOLOR(c) \
	*(unsigned short *)(arkMMIOBase + FOREGROUNDCOLOR) = c;

#define SETBACKGROUNDCOLOR32(c) \
	*(unsigned short *)(arkMMIOBase + BACKGROUNDCOLOR) = c; \
	*(unsigned short *)(arkMMIOBase + BACKGROUNDCOLORHIGH) = (c) >> 16;

#define SETFOREGROUNDCOLOR32(c) \
	*(unsigned short *)(arkMMIOBase + FOREGROUNDCOLOR) = c; \
	*(unsigned short *)(arkMMIOBase + FOREGROUNDCOLORHIGH) = (c) >> 16;

#define SETBGANDFGCOLOR(c) \
	*(unsigned int *)(arkMMIOBase + BACKGROUNDCOLOR) = c;

#define SETBACKGROUNDCOLORHIGH(c) \
	*(unsigned short *)(arkMMIOBase + BACKGROUNDCOLORHIGH) = c;

#define SETFOREGROUNDCOLORHIGH(c) \
	*(unsigned short *)(arkMMIOBase + FOREGROUNDCOLORHIGH) = c;

#define SETBGANDFGCOLORHIGH(c) \
	*(unsigned int *)(arkMMIOBase + BACKGROUNDCOLORHIGH) = c;

#define SETTRANSPARENCYCOLOR(c) \
	*(unsigned short *)(arkMMIOBase + TRANSPARENCYCOLOR) = c;

#define SETTRANSPARENCYCOLOR32(c) \
	*(unsigned short *)(arkMMIOBase + TRANSPARENCYCOLOR) = c & 0xFFFF; \
	*(unsigned short *)(arkMMIOBase + TRANSPARENCYCOLORHIGH) = (c) >> 16;

#define SETTRANSPARENCYCOLORMASK(c) \
	*(unsigned short *)(arkMMIOBase + TRANSPARENCYCOLORMASK) = c;

#define SETTRANSPARENCYCOLORMASK32(c) \
	*(unsigned short *)(arkMMIOBase + TRANSPARENCYCOLORMASK) = c & 0xFFFF; \
	*(unsigned short *)(arkMMIOBase + TRANSPARENCYCOLORMASKHIGH) = (c) >> 16;

#define SETCOLORMIXSELECT(m) \
	*(unsigned short *)(arkMMIOBase + COLORMIXSELECT) = m;

#define SETWRITEPLANEMASK(m) \
	*(unsigned short *)(arkMMIOBase + WRITEPLANEMASK) = m;

#define SETWRITEPLANEMASK32(m) \
	*(unsigned int *)(arkMMIOBase + WRITEPLANEMASK) = m;

#define SETERRORTERM(e) \
	*(short *)(arkMMIOBase + ERRORTERM) = e;

#define SETERRORINCREMENT(e1, e2) \
	*(unsigned int *)(arkMMIOBase + AXIALERRORINCREMENT) = (e1) | ((e2) << 16);

#define SETSTENCILPITCH(p) \
	*(unsigned short *)(arkMMIOBase + STENCILPITCH) = p;

#define SETSOURCEPITCH(p) \
	*(unsigned short *)(arkMMIOBase + SOURCEPITCH) = p;

#define SETDESTPITCH(p) \
	*(unsigned short *)(arkMMIOBase + DESTPITCH) = p;

#define SETSTENCILADDR(p) \
	*(unsigned int *)(arkMMIOBase + STENCILADDR) = p;

#define SETSOURCEADDR(p) \
	*(unsigned int *)(arkMMIOBase + SOURCEADDR) = p;

#define SETSOURCEXY(x, y) \
	*(unsigned int *)(arkMMIOBase + SOURCEADDR) = (y << 16) + x;

#define SETSOURCEX(x) \
	*(unsigned short *)(arkMMIOBase + SOURCEX) = x;

#define SETSOURCEY(y) \
	*(unsigned short *)(arkMMIOBase + SOURCEY) = y;

#define SETDESTADDR(p) \
	*(unsigned int *)(arkMMIOBase + DESTADDR) = p;

#define SETDESTXY(x, y) \
	*(unsigned int *)(arkMMIOBase + DESTADDR) = (y << 16) + x;

#define SETDESTX(x) \
	*(unsigned short *)(arkMMIOBase + DESTX) = x;

#define SETDESTY(y) \
	*(unsigned short *)(arkMMIOBase + DESTY) = y;

#define SETWIDTH(p) \
	*(unsigned short *)(arkMMIOBase + WIDTH) = p - 1;

#define SETHEIGHT(p) \
	*(unsigned short *)(arkMMIOBase + HEIGHT) = p - 1;

#define SETWIDTHANDHEIGHT(w, h) \
	*(unsigned int *)(arkMMIOBase + WIDTH) = ((h - 1) << 16) | (w - 1);

#define SETBITMAPCONFIG(p) \
	*(unsigned short *)(arkMMIOBase + BITMAPCONFIG) = p;

#define SETCOMMAND(p) \
	*(unsigned short *)(arkMMIOBase + COMMAND) = p;

#define COPISBUSY() (inb(0x3CB) & 0x40)

#define WAITUNTILFINISHED() \
	for (;;) { \
		if (!COPISBUSY()) \
			break; \
	}
