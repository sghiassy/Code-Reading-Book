/* $XFree86: xc/programs/Xserver/hw/xfree86/vga2/mfb.banked/mfblinebank.h,v 3.9 1996/12/23 06:55:24 dawes Exp $ */
/* mfblinebank.h */
/* included from mfb.h if MFB_LINE_BANK is defined */


/* $XConsortium: mfblinebank.h /main/7 1996/02/21 18:01:05 kaleb $ */

/* This should be       */
/* #include "vga.h"     */
/* #include "vgaBank.h" */
/* But this would mean to include a dozen other header files ... */

extern void (* vgaSetReadFunc)();
extern void (* vgaSetWriteFunc)();
extern void (* vgaSetReadWriteFunc)();
extern int vgaSegmentSize;
extern int vgaSegmentShift;
extern int vgaSegmentMask;

extern void *vgaReadBottom;
extern void *vgaReadTop;
extern void *vgaWriteBottom;
extern void *vgaWriteTop;

extern int vgaReadseg;
extern int vgaWriteseg;
extern int vgaReadWriteseg;
extern int vgaSaveReadseg;

#ifdef CSRG_BASED
#define VGABASE 0xFF000000
#else
#if defined(__alpha__)
#define VGABASE 0xFFFFFFFFF0000000UL
#else
#define VGABASE 0xF0000000
#endif
#endif

#if __GNUC__ > 1
#define USE_GCC_INLINE
#endif

#if __GNUC__ > 1 && !defined(__alpha__) && !defined(__arm32__)
#define USE_ASM_BANK_MACROS
#endif

#ifdef USE_GCC_INLINE
#define gcc_inline __inline__
#else
#define gcc_inline /**/
#endif

#ifdef BANK_DEBUG
extern
#else
static gcc_inline
#endif
PixelType *vga2ScanlineOffsetFuncSrc(
#if NeedFunctionPrototypes
PixelType * /* _pointer */,
int         /* offset */
#endif
);

#ifdef BANK_DEBUG
extern
#else
static gcc_inline
#endif
PixelType *vga2ScanlineOffsetFuncDst(
#if NeedFunctionPrototypes
PixelType * /* _pointer */,
int         /* offset */
#endif
);

#ifdef BANK_DEBUG
extern
#else
static gcc_inline
#endif
PixelType *vga2ScanlineOffsetFuncBoth(
#if NeedFunctionPrototypes
PixelType * /* _pointer */,
int         /* offset */
#endif
);

#ifdef BANK_DEBUG
extern void READ_WRITE_DST(
#if NeedFunctionPrototypes
void
#endif
);
extern void READ_SRC_WRITE_DST(
#if NeedFunctionPrototypes
void
#endif
);
#endif

#ifndef BANK_DEBUG

#ifdef USE_ASM_BANK_MACROS
#define DO_BANK_READ(_bank)						\
__asm__ __volatile__							\
("call *%0"								\
 : /* OUT     */							\
 : /* IN      */ "c" /*ecx*/ ((unsigned long)vgaSetReadFunc),		\
		 "a" /*eax*/ (_bank)					\
 : /* CLOBBER */ "ax","dx","memory" /*eax,edx*/				\
);
#define DO_BANK_WRITE(_bank)						\
__asm__ __volatile__							\
("call *%0"								\
 : /* OUT     */							\
 : /* IN      */ "c" /*ecx*/ ((unsigned long)vgaSetWriteFunc),		\
		 "a" /*eax*/ (_bank)					\
 : /* CLOBBER */ "ax","dx","memory" /*eax,edx*/				\
);
#define DO_BANK_READ_WRITE(_bank)						\
__asm__ __volatile__							\
("call *%0"								\
 : /* OUT     */							\
 : /* IN      */ "c" /*ecx*/ ((unsigned long)vgaSetReadWriteFunc),	\
		 "a" /*eax*/ (_bank)					\
 : /* CLOBBER */ "ax","dx","memory" /*eax,edx*/				\
);
#else
extern void vgaBankRead(
#if NeedFunctionPrototypes
int n
#endif
);
extern void vgaBankWrite(
#if NeedFunctionPrototypes
int n
#endif
);
extern void vgaBankReadWrite(
#if NeedFunctionPrototypes
int n
#endif
);
#define DO_BANK_READ(_bank)       vgaBankRead(_bank)
#define DO_BANK_WRITE(_bank)      vgaBankWrite(_bank)
#define DO_BANK_READ_WRITE(_bank) vgaBankReadWrite(_bank)
#endif /* USE_ASM_BANK_MACROS */


static gcc_inline PixelType *vga2ScanlineOffsetFuncSrc(p,offset)
    PixelType *p;
    int offset;
{
register /*signed*/ long delta;
register /*signed*/ long deltabank;
  if ((unsigned long)(p + offset) >= VGABASE) {
     /* virtual framebuffer address */
     p += offset;
     p = (PixelType *)((unsigned long)p - (unsigned long)VGABASE);
     deltabank = (/*signed*/ long)p >> vgaSegmentShift;
     vgaReadseg = deltabank;
     DO_BANK_READ(vgaReadseg);
     p = (PixelType *)( (unsigned long)p + (unsigned long)vgaReadBottom
                        - (deltabank << vgaSegmentShift) );
     return(p);
  }
  /* At least on Linux, the mapped Area is at 0x40000000, so test >= first */
  if ((unsigned long)p >= (unsigned long)vgaReadBottom) {
     if ((unsigned long)p <= (unsigned long)vgaReadTop) {
        /* within the framebuffer */
        p += offset;
        /* outside of the bank now ? */
        delta = (/*signed*/ long)p - (/*signed*/ long)vgaReadBottom;
        deltabank = delta >> vgaSegmentShift;
        if (0!=deltabank) {
                vgaReadseg += deltabank;
                DO_BANK_READ(vgaReadseg);
                p = (PixelType *)( (unsigned long)p
                                   - (deltabank << vgaSegmentShift) );
        }
        return(p);
     }
  }
  /* no framebuffer address */
  return(p+offset);
}

static gcc_inline PixelType *vga2ScanlineOffsetFuncDst(p,offset)
    PixelType *p;
    int offset;
{
register /*signed*/ long delta;
register /*signed*/ long deltabank;
  if ((unsigned long)(p + offset) >= VGABASE) {
     /* virtual framebuffer address */
     p += offset;
     p = (PixelType *)((unsigned long)p - (unsigned long)VGABASE);
     deltabank = (/*signed*/ long)p >> vgaSegmentShift;
     vgaWriteseg = deltabank;
     DO_BANK_WRITE(vgaWriteseg);
     p = (PixelType *)( (unsigned long)p + (unsigned long)vgaWriteBottom
                        - (deltabank << vgaSegmentShift) );
     return(p);
  }
  /* At least on Linux, the mapped Area is at 0x40000000, so test >= first */
  if ((unsigned long)p >= (unsigned long)vgaWriteBottom) {
     if ((unsigned long)p <= (unsigned long)vgaWriteTop) {
        /* within the framebuffer */
        p += offset;
        /* outside of the bank now ? */
        delta = (/*signed*/ long)p - (/*signed*/ long)vgaWriteBottom;
        deltabank = delta >> vgaSegmentShift;
        if (0!=deltabank) {
                vgaWriteseg += deltabank;
                DO_BANK_WRITE(vgaWriteseg);
                p = (PixelType *)( (unsigned long)p
                                   - (deltabank << vgaSegmentShift) );
        }
        return(p);
     }
  }
  /* no framebuffer address */
  return(p+offset);
}

static gcc_inline PixelType *vga2ScanlineOffsetFuncBoth(p,offset)
    PixelType *p;
    int offset;
{
register /*signed*/ long delta;
register /*signed*/ long deltabank;
  if ((unsigned long)(p + offset) >= VGABASE) {
     /* virtual framebuffer address */
     p += offset;
     p = (PixelType *)((unsigned long)p - (unsigned long)VGABASE);
     deltabank = (/*signed*/ long)p >> vgaSegmentShift;
     vgaWriteseg = deltabank;
     DO_BANK_READ_WRITE(vgaWriteseg);
     p = (PixelType *)( (unsigned long)p + (unsigned long)vgaWriteBottom
                        - (deltabank << vgaSegmentShift) );
     return(p);
  }
  /* At least on Linux, the mapped Area is at 0x40000000, so test >= first */
  if ((unsigned long)p >= (unsigned long)vgaWriteBottom) {
     if ((unsigned long)p <= (unsigned long)vgaWriteTop) {
        /* within the framebuffer */
        p += offset;
        /* outside of the bank now ? */
        delta = (/*signed*/ long)p - (/*signed*/ long)vgaWriteBottom;
        deltabank = delta >> vgaSegmentShift;
        if (0!=deltabank) {
                vgaWriteseg += deltabank;
                DO_BANK_READ_WRITE(vgaWriteseg);
                p = (PixelType *)( (unsigned long)p
                                   - (deltabank << vgaSegmentShift) );
        }
        return(p);
     }
  }
  /* no framebuffer address */
  return(p+offset);
}

#define READ_WRITE_DST()     DO_BANK_READ(vgaWriteseg)

#define READ_SRC_WRITE_DST() DO_BANK_READ(vgaReadseg)


#endif /* !BANK_DEBUG */

#define mfbScanlineOffset(_pointer, offset) \
	vga2ScanlineOffsetFuncBoth((_pointer), (offset))

#define mfbScanlineInc(_pointer, offset) \
	(_pointer) = mfbScanlineOffset((_pointer), (offset))


#define mfbScanlineOffsetSrc(_pointer, offset) \
	vga2ScanlineOffsetFuncSrc((_pointer), (offset))

#define mfbScanlineIncSrc(_pointer, offset) \
	(_pointer) = mfbScanlineOffsetSrc((_pointer), (offset))

#define mfbScanlineDeltaSrc(pointer, y, width) \
	mfbScanlineOffsetSrc(pointer, (y) * (width))
    
#define mfbScanlineSrc(pointer, x, y, width) \
	mfbScanlineOffsetSrc(pointer, (y) * (width) + ((x) >> MFB_PWSH))


#define mfbScanlineOffsetDst(_pointer, offset) \
	vga2ScanlineOffsetFuncDst((_pointer), (offset))

#define mfbScanlineIncDst(_pointer, offset) \
	(_pointer) = mfbScanlineOffsetDst((_pointer), (offset))

#define mfbScanlineDeltaDst(pointer, y, width) \
	mfbScanlineOffsetDst(pointer, (y) * (width))
    
#define mfbScanlineDst(pointer, x, y, width) \
	mfbScanlineOffsetDst(pointer, (y) * (width) + ((x) >> MFB_PWSH))
