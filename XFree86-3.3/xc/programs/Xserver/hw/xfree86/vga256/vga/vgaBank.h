/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgaBank.h,v 3.7 1996/12/23 06:59:28 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 *
 */
/* $XConsortium: vgaBank.h /main/7 1996/02/21 18:10:03 kaleb $ */

#ifndef VGA_BANK_H
#define VGA_BANK_H

extern void *vgaReadBottom;
extern void *vgaReadTop;
extern void *vgaWriteBottom;
extern void *vgaWriteTop;
extern Bool vgaReadFlag, vgaWriteFlag;
#if defined(__alpha__) || defined(__arm32__)
extern unsigned long writeseg;
#else /* __alpha__ || __arm32__ */
extern void *writeseg;
#endif /* __alpha__ || __arm32__ */

/* vgaBank.s */

void *vgaSetReadWrite(
#if NeedFunctionPrototypes
    void *
#endif
);

void *vgaReadWriteNext(
#if NeedFunctionPrototypes
    void *
#endif
);

void *vgaReadWritePrev(
#if NeedFunctionPrototypes
    void *
#endif
);

void *vgaSetRead(
#if NeedFunctionPrototypes
    void *
#endif
);

void *vgaReadNext(
#if NeedFunctionPrototypes
    void *
#endif
);

void *vgaReadPrev(
#if NeedFunctionPrototypes
    void *
#endif
);

void *vgaSetWrite(
#if NeedFunctionPrototypes
    void *
#endif
);

void *vgaWriteNext(
#if NeedFunctionPrototypes
    void *
#endif
);

void *vgaWritePrev(
#if NeedFunctionPrototypes
    void *
#endif
);

void vgaSaveBank(
#if NeedFunctionPrototypes
    void
#endif
);

void vgaRestoreBank(
#if NeedFunctionPrototypes
    void
#endif
);

void vgaPushRead(
#if NeedFunctionPrototypes
    void
#endif
);

void vgaPopRead(
#if NeedFunctionPrototypes
    void
#endif
);

void vgaSetVidPage(
#if NeedFunctionPrototypes
    int
#endif
);

extern int vgaSegmentMask;

extern void SpeedUpComputeNext(
#if NeedFunctionPrototypes
    unsigned,
    unsigned
#endif
);

extern void SpeedUpComputePrev(
#if NeedFunctionPrototypes
    unsigned,
    unsigned
#endif
);

extern unsigned SpeedUpRowsNext[];
extern unsigned SpeedUpRowsPrev[];


#ifdef CSRG_BASED
#define VGABASE 0xFF000000
#else
#if defined(__alpha__)
#define VGABASE 0xFFFFFFFFF0000000UL
#else /* __alpha__ */
#define VGABASE 0xF0000000
#endif /* __alpha__ */
#endif

/* Clear these first -- since they are defined in mfbcustom.h */
#undef CHECKSCREEN
#undef SETRWF
#undef CHECKRWOF
#undef CHECKRWUF
#undef BANK_FLAG
#undef BANK_FLAG_BOTH
#undef SETR
#undef SETW
#undef SETRW
#undef CHECKRO
#undef CHECKWO
#undef CHECKRWO
#undef CHECKRU
#undef CHECKWU
#undef CHECKRWU
#undef NEXTR
#undef NEXTW
#undef PREVR
#undef PREVW
#undef SAVE_BANK
#undef RESTORE_BANK
#undef PUSHR
#undef POPR

#define CHECKSCREEN(x) (((unsigned long)(x) >= VGABASE) ? TRUE : FALSE)

#define SETRWF(f,x) { if(f) x = vgaSetReadWrite(x); }
#define CHECKRWOF(f,x) { if(f && ((void *)x >= vgaWriteTop)) \
			  x = vgaReadWriteNext(x); }
#define CHECKRWUF(f,x) { if(f && ((void *)x < vgaWriteBottom)) \
			  x = vgaReadWritePrev(x); }
#define BANK_FLAG(a) \
  vgaWriteFlag = CHECKSCREEN((a)); \
  vgaReadFlag = FALSE; 

#define BANK_FLAG_BOTH(a,b) \
  vgaReadFlag = CHECKSCREEN((a)); \
  vgaWriteFlag  = CHECKSCREEN((b)); 

#define SETR(x)  { if(vgaReadFlag) x = vgaSetRead(x); }
#define SETW(x)  { if(vgaWriteFlag) x = vgaSetWrite(x); }
#define SETRW(x) { if(vgaWriteFlag) x = vgaSetReadWrite(x); }
#define CHECKRO(x) { if(vgaReadFlag && ((void *)x >= vgaReadTop)) \
			 x = vgaReadNext(x); }
#define CHECKRU(x) { if(vgaReadFlag && ((void *)x < vgaReadBottom)) \
			 x = vgaReadPrev(x); }
#define CHECKWO(x) { if(vgaWriteFlag && ((void *)x >= vgaWriteTop)) \
			 x = vgaWriteNext(x); }
#define CHECKWU(x) { if(vgaWriteFlag && ((void *)x < vgaWriteBottom)) \
			 x = vgaWritePrev(x); }
#define CHECKRWO(x) { if(vgaWriteFlag && ((void *)x >= vgaWriteTop)) \
			  x = vgaReadWriteNext(x); }
#define CHECKRWU(x) { if(vgaWriteFlag && ((void *)x < vgaWriteBottom)) \
			  x = vgaReadWritePrev(x); }

#define NEXTR(x) { x = vgaReadNext(x);}
#define NEXTW(x) { x = vgaWriteNext(x); }
#define PREVR(x) { x = vgaReadPrev(x); }
#define PREVW(x) { x = vgaWritePrev(x); }

#define SAVE_BANK()     { if (vgaWriteFlag) vgaSaveBank(); }
#define RESTORE_BANK()  { if (vgaWriteFlag) vgaRestoreBank(); }

#define PUSHR()         { if(vgaWriteFlag) vgaPushRead(); }
#define POPR()          { if(vgaWriteFlag) vgaPopRead(); }


#endif /* VGA_BANK_H */
