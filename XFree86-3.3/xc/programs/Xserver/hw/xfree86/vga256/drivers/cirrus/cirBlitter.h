/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cirBlitter.h,v 3.12 1996/12/23 06:56:27 dawes Exp $ */


/* Definitions for BitBLT engine communication. */


/* $XConsortium: cirBlitter.h /main/5 1996/10/25 10:30:37 kaleb $ */

/* BitBLT modes. */

#define FORWARDS		0x00
#define BACKWARDS		0x01
#define SYSTEMDEST		0x02
#define SYSTEMSRC		0x04
#define TRANSPARENCYCOMPARE	0x08
#define PIXELWIDTH16		0x10
#define PIXELWIDTH24		0x20	/* 5436/46 only. */
#define PIXELWIDTH32		0x30	/* 5434/36/46 only. */
#define PATTERNCOPY		0x40
#define COLOREXPAND		0x80

/* Extended BitBLT modes (36/46 only). */

#define SOURCEDWORDGRANULARITY	0x01
#define INVERTSOURCESENSE	0x02
#define SOLIDCOLORFILL		0x04

/* Address: the 5426 adresses 2MBytes, the 5434 can address 4MB. */

/* Set BLT source address; source address is modified by blit.
 * Shadow variable is invalidated. */
#define SETSRCADDR(srcAddr) \
    outw (0x3CE, (((srcAddr) & 0x000000FF) << 8) | 0x2C); \
    outw (0x3CE, (((srcAddr) & 0x0000FF00)) | 0x2D); \
    outw (0x3CE, (((srcAddr) & 0x003F0000) >> 8) | 0x2E); \
    cirrusBltSrcAddrShadow = -1;

/* Set BLT source address; source address is preserved by blit.
 * Usually dummy source address that needs to set to 0. */
#define SETSRCADDRUNMODIFIED(srcAddr) \
  if (srcAddr != cirrusBltSrcAddrShadow) { \
    cirrusBltSrcAddrShadow = srcAddr; \
    outw (0x3CE, (((srcAddr) & 0x000000FF) << 8) | 0x2C); \
    outw (0x3CE, (((srcAddr) & 0x0000FF00)) | 0x2D); \
    outw (0x3CE, (((srcAddr) & 0x003F0000) >> 8) | 0x2E); \
  }

#define SETDESTADDR(dstAddr) \
    outw (0x3CE, (((dstAddr) & 0x000000FF) << 8) | 0x28); \
    outw (0x3CE, (((dstAddr) & 0x0000FF00)) | 0x29); \
    outw (0x3CE, (((dstAddr) & 0x003F0000) >> 8) | 0x2A);

/* Pitch: the 5426 goes up to 4095, the 5434 can do 8191. */

#define SETDESTPITCH(dstPitch) \
  if ((dstPitch) != cirrusBltDestPitchShadow) { \
    cirrusBltDestPitchShadow = dstPitch; \
    outw (0x3CE, (((dstPitch) & 0x000000FF) << 8) | 0x24); \
    outw (0x3CE, (((dstPitch) & 0x00001F00)) | 0x25); \
  }

#define SETSRCPITCH(srcPitch) \
  if ((srcPitch) != cirrusBltSrcPitchShadow) { \
    cirrusBltSrcPitchShadow = srcPitch; \
    outw (0x3CE, (((srcPitch) & 0x000000FF) << 8) | 0x26); \
    outw (0x3CE, (((srcPitch) & 0x00001F00)) | 0x27); \
  }

/* Width: the 5426 goes up to 2048, the 5434 can do 8192. */

#define SETWIDTH(fillWidth) \
  outw (0x3CE, ((((fillWidth) - 1) & 0x000000FF) << 8) | 0x20); \
  outw (0x3CE, ((((fillWidth) - 1) & 0x00001F00)) | 0x21);

/* Height: the 5426 goes up to 1024, the 5434 can do 2048. */
/* It appears many 5434's only go up to 2048. */

/* The height is preserved by a blit only on the 543x. */
#define SETHEIGHT543X(fillHeight) \
  if ((fillHeight) != cirrusBltHeightShadow) { \
    cirrusBltHeightShadow = fillHeight; \
    outw (0x3CE, ((((fillHeight) - 1) & 0x000000FF) << 8) | 0x22); \
    outw (0x3CE, (((fillHeight) - 1) & 0x00000700) | 0x23); \
  }

#define SETHEIGHT(fillHeight) \
  outw (0x3CE, ((((fillHeight) - 1) & 0x000000FF) << 8) | 0x22); \
  outw (0x3CE, (((fillHeight) - 1) & 0x00000700) | 0x23);

#define SETBLTMODE(m) \
  if ((m) != cirrusBltModeShadow) { \
    cirrusBltModeShadow = m; \
    outw(0x3CE, ((m) << 8) | 0x30); \
  }

#define SETBLTWRITEMASK(m) outw(0x3ce, ((m) << 8) | 0x2f);

#define SETTRANSPARENCYCOLOR(c) outw(0x3ce, ((c) << 8) | 0x34);

#define SETTRANSPARENCYCOLOR16(c) \
  outw(0x3ce, ((c) << 8) | 0x34); \
  outw(0x3ce, (c & 0xff00) | 0x35);

#define SETTRANSPARENCYCOLORMASK16(m) \
  outw(0x3ce, ((m) << 8) | 0x38); \
  outw(0x3ce, ((m) & 0xff00) | 0x39);

#define SETROP(rop) \
  if ((rop) != cirrusBltRopShadow) { \
    cirrusBltRopShadow = rop; \
    outw (0x3CE, ((rop) << 8) | 0x32); \
  }

/* 32-bit colors are specific to the BitBLT engine, so define macros here. */

#define SETFOREGROUNDCOLOR32(c) \
    outw(0x3ce, 0x01 + ((c) << 8)); \
    outw(0x3ce, 0x11 + ((c) & 0xff00)); \
    outw(0x3ce, 0x13 + (((c) & 0xff0000) >> 8)); \
    outw(0x3ce, 0x15 + (((unsigned int)(c) & 0xff000000) >> 16));

#define SETBACKGROUNDCOLOR32(c) \
    outw(0x3ce, 0x00 + ((c) << 8)); \
    outw(0x3ce, 0x10 + ((c) & 0xff00)); \
    outw(0x3ce, 0x12 + (((c) & 0xff0000) >> 8)); \
    outw(0x3ce, 0x14 + (((unsigned int)(c) & 0xff000000) >> 16));

#define STARTBLT() { \
  unsigned char tmp; \
  outb(0x3ce, 0x31); \
  tmp = inb(0x3cf); \
  outb(0x3cf, tmp | 0x02); \
  }

#define SETBLTSTATUS(v) { \
  unsigned char tmp; \
  outw(0x3ce, 0x31 | (v << 8)); \
  }
  
#define BLTBUSY(s) { \
  outb(0x3ce, 0x31); \
  s = inb(0x3cf) & 1; \
  }

#define BLTEMPTY(s) { \
  outb(0x3ce, 0x31); \
  s = (inb(0x3cf) & 0x10) == 0; \
  }

#define BLTRESET() { \
  unsigned char tmp; \
  outb(0x3ce, 0x31); \
  tmp = inb(0x3cf); \
  outb(0x3cf, tmp | 0x04); \
  outb(0x3cf, tmp); \
  }

#define WAITUNTILFINISHED() CirrusBLTWaitUntilFinished()

/*
 * Extra definitions added for XAA driver. These are most useful for MMIO,
 * but we must still provide them to allow for a regular I/O XAA driver.
 */

#define SETBLTMODEEXT(m) outw(0x3ce, ((m) << 8) | 0x33);

/*
 * Don't set the BLT mode extensions. They are only used on recent chips
 * when MMIO is enabled.
 */
#define SETBLTMODEANDROP(m, mext, rop) \
    SETBLTMODE(m) \
    SETROP(rop)

#define SETWIDTHANDHEIGHT(w, h) \
    SETWIDTH(w) \
    SETHEIGHT(h)
