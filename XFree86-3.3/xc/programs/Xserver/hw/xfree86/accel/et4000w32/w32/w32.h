/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/w32/w32.h,v 3.13.2.1 1997/05/16 11:35:13 hohndel Exp $ */ 
/*******************************************************************************
                        Copyright 1994 by Glenn G. Lai

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyr notice appear in all copies and that
both that copyr notice and this permission notice appear in
supporting documentation, and that the name of Glenn G. Lai not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

Glenn G. Lai DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

Glenn G. Lai
P.O. Box 4314
Austin, Tx 78765
glenn@cs.utexas.edu)
4/6/94
6/30/94
*******************************************************************************/
/* $XConsortium: w32.h /main/12 1996/10/25 10:21:49 kaleb $ */

#ifndef W32_H
#define W32_H

#include "X.h"
#include "misc.h"
#include "vgaBank.h"
#include <stdio.h>

typedef unsigned char *ByteP; 
typedef volatile unsigned char *VByteP;
typedef unsigned short *WordP;
typedef unsigned *LongP;

extern
int W32RamdacType;

#define UNKNOWN_DAC    -1
#define NORMAL_DAC      0
#define ATT20C47xA_DAC  1
#define Sierra1502X_DAC 2
#define ATT20C497_DAC   3
#define ATT20C490_DAC   4
#define ATT20C493_DAC   5
#define ATT20C491_DAC   6
#define ATT20C492_DAC   7
#define ICS5341_DAC    10
#define GENDAC_DAC     11
#define STG1700_DAC    12
#define STG1702_DAC    13
#define STG1703_DAC    14
#define ET6000_DAC     15
#define CH8398_DAC     17


extern
ByteP W32Buffer;

extern
VByteP ACL;


extern
LongP MBP0, MBP1, MBP2; 


extern
ByteP MMU_CONTROL;


extern
ByteP ACL_SUSPEND_TERMINATE,
      ACL_OPERATION_STATE,
      ACL_SYNC_ENABLE,
      ACL_INTERRUPT_MASK,
      ACL_INTERRUPT_STATUS,
      ACL_ACCELERATOR_STATUS;

/* for ET6000: */
#define ACL_6K_CONFIG ACL_SYNC_ENABLE

extern
WordP ACL_X_POSITION,
      ACL_Y_POSITION;

extern
WordP ACL_NQ_X_POSITION,
      ACL_NQ_Y_POSITION;


extern
LongP ACL_PATTERN_ADDRESS,
      ACL_SOURCE_ADDRESS;


extern
WordP ACL_PATTERN_Y_OFFSET,
      ACL_SOURCE_Y_OFFSET,
      ACL_DESTINATION_Y_OFFSET;


extern
ByteP ACL_VIRTUAL_BUS_SIZE,     /* only for w32 and w32i */
      ACL_XY_DIRECTION,     	/* only for w32 and w32i */
      ACL_PIXEL_DEPTH;          /* only for w32p_rev_A and w32p_rev_B */


extern
ByteP ACL_PATTERN_WRAP,
      ACL_SOURCE_WRAP;


extern
WordP ACL_X_COUNT,
      ACL_Y_COUNT;


extern
ByteP ACL_ROUTING_CONTROL,
      ACL_RELOAD_CONTROL,
      ACL_BACKGROUND_RASTER_OPERATION,
      ACL_FOREGROUND_RASTER_OPERATION;

/* for ET6000: */
#define ACL_MIX_CONTROL ACL_ROUTING_CONTROL
#define ACL_STEPPING_INHIBIT ACL_RELOAD_CONTROL

extern
LongP ACL_DESTINATION_ADDRESS,

      /* only for w32p_rev_A and w32p_rev_B */
      ACL_MIX_ADDRESS;


extern
WordP ACL_MIX_Y_OFFSET,
      ACL_ERROR_TERM,
      ACL_DELTA_MINOR,
      ACL_DELTA_MAJOR;

/* for ET6000 only */
extern
ByteP ACL_POWER_CONTROL;


extern ByteP W32Buffer;


extern Bool W32;
extern Bool W32i;
extern Bool W32OrW32i;
extern Bool W32p;
extern Bool W32pa;
extern Bool W32pCAndLater;
extern Bool W32et6000;
extern Bool FrameBuffer;

extern unsigned long ET6Kbase;

extern long W32Foreground;
extern long W32Background;
extern long W32Pattern;
extern long W32Mix;
extern long W32MixPong;
extern long MixDstPing;
extern long MixDstPong;

extern long W32BltCount;
extern long W32BltHop;
extern long W32BoxCount;
extern long W32BoxHop;
extern long W32PointCount;
extern long W32PointHop;

extern ByteP W32BytePtr;
extern WordP W32WordPtr;
extern LongP W32LongPtr;


extern int W32OpTable[16];
extern int W32PatternOpTable[16];

#if 0
extern FILE *GlennDebug;
#define GLENN(E) {fprintf(GlennDebug, "[%s]", E);}
#endif

/******************************************************************************/
void W32WarpCursor(unsigned x, unsigned y, unsigned x_size, unsigned y_size);
void W32CursorAddress(unsigned a);
void W32FillCursor(unsigned char *cursor, unsigned width, unsigned height);
void W32CursorOn(void);
void W32CursorOff(void);

void W32Init(void);
void W32Exit(void);

void figure(char*);


/******************************************************************************/
/* Doesn't seem to work for w32 */
#define FourByOneWrap 0x20


#define WAIT_QUEUE \
{while (*(volatile unsigned char *)ACL_ACCELERATOR_STATUS & 0x1);}


/* some bug can cause a hang forever for some reason on Linux 1.3.X kernels. 
 * Wait max 30 msec, then go on anyway 
 */
  #define WAIT_ACL \
{ int cnt=30; \
  while ((*(volatile unsigned char *)ACL_ACCELERATOR_STATUS & 0x2) && (--cnt)) \
  { \
    usleep(1000); \
  } \
  if (!(cnt)) ErrorF("W32 Accelerator: WAIT_ACL timeout. Skipped.\n"); \
}
  
#define WAIT_XY \
{while (*(volatile unsigned char *)ACL_ACCELERATOR_STATUS & 0x4);}


#define SET_XY(X, Y) \
    {*((LongP) ACL_X_COUNT) = (((Y) - 1) << 16) + ((X) - 1) * (PSZ >> 3);}


/* Must do 0x09 (in one operation) for the W32 */
#define START_ACL(dst) \
{ \
    *(ACL_DESTINATION_ADDRESS) = dst; \
    if (W32OrW32i) *ACL_OPERATION_STATE = 0x09; \
}


/* X, Y positions set to zero's for w32 and w32i */
#define RESET_ACL \
{ \
    *ACL_SUSPEND_TERMINATE = 0x0; \
    *ACL_SUSPEND_TERMINATE = 0x10; \
    WAIT_ACL \
    *ACL_SUSPEND_TERMINATE = 0x0; \
    /* use apertures 1 and 2 and set up aperture 3 for the accelerator */ \
    /* clear interrupt indicators */ \
    *ACL_INTERRUPT_STATUS = 0xe; \
    *ACL_INTERRUPT_MASK = 0x0; \
    *ACL_INTERRUPT_STATUS = 0x0; \
    *ACL_ACCELERATOR_STATUS = 0x0; \
    if (W32et6000) \
    { \
      *ACL_STEPPING_INHIBIT = 0x0; \
      *ACL_6K_CONFIG = 0x0; /* maximum performance */ \
      *ACL_POWER_CONTROL = 0x01;  /* conserve power when ACL is idle */ \
    } \
    else \
    { \
      *ACL_RELOAD_CONTROL = 0x0; \
      *ACL_SYNC_ENABLE = 0x1; \
    } \
    if (W32OrW32i) \
    { \
        *ACL_X_POSITION = 0; \
        *ACL_Y_POSITION = 0; \
        *ACL_OPERATION_STATE = 0x0; \
    } \
    else /* w32p and ET6000 */ \
    { /* Enable the W32p startup bit and set use an eight-bit pixel depth */ \
        *ACL_NQ_X_POSITION = 0; \
        *ACL_NQ_Y_POSITION = 0; \
	*ACL_PIXEL_DEPTH = (PSZ - 8) << 1; \
        *ACL_OPERATION_STATE = 0x10; \
    } \
    *MMU_CONTROL = 0x74; \
      /* bit 2 in MMU_CONTROL is set. This routes MBP2 data to the accel \
       * instead of to video memory. This is not possible in the ET6000 \
       */ \
}


#define START_ACL_CPU(dst) \
{ \
    if (W32OrW32i) \
	*(MBP2) = dst; \
    else \
	*(ACL_DESTINATION_ADDRESS) = dst; \
}


/* Remove the following, as PFILL already does what we want */  
#define dup(i) (((i)<<24)|((i)<<16)|((i)<<8)|(i))


#define UNROLL8(Y)  \
for (i = 0; i < ((Y) >> 3); i++) \
{ \
    GGL GGL GGL GGL GGL GGL GGL GGL \
} \
switch ((Y) & 0x7) \
{ \
    case 7: GGL \
    case 6: GGL \
    case 5: GGL \
    case 4: GGL \
    case 3: GGL \
    case 2: GGL \
    case 1: GGL \
    default: break; \
}


#define UNROLL4(Y)  \
for (i = 0; i < ((Y) >> 2); i++) \
{ \
    GGL GGL GGL GGL\
} \
switch ((Y) & 0x3) \
{ \
    case 3: GGL \
    case 2: GGL \
    case 1: GGL \
    default: break; \
}


/*
 *  Wait for the accelerator to stop
 */
#define WAIT WAIT_XY


#define W32Ptr W32BytePtr


#define W32_SET(DST) W32_SET_BYTE(DST)
#define W32_SET_SELF(DST) W32_BYTE(DST)

#define W32_SET2(DST) W32_SET_BYTE2(DST)
#define W32_SET_SELF2(DST) W32_BYTE2(DST)

/* Always work on long words */
#define W32_SPAN(DST) \
if (FrameBuffer)

#define W32_SET_BYTE(DST) \
{ \
    *MBP0 = (CARD32)(DST) & 0xfffffffc; \
    W32Ptr = (void*)(W32Buffer + ((CARD32)(DST) & 0x3)); \
}


#define W32_SET_SHORT(DST) \
{ \
    *MBP0 = (CARD32)(DST) & 0xfffffffc; \
    W32Ptr = (void*)(W32Buffer + ((CARD32)(DST) & 0x3)); \
}


#define W32_SET_LONG(DST) \
{ \
    *MBP0 = (CARD32)DST; \
    W32Ptr = (void*)W32Buffer; \
}


#define W32_SET_BYTE2(DST) \
{ \
    *MBP0 = (CARD32)(DST) & 0xfffffffc; \
    *MBP1 = ((CARD32)(DST) & 0xfffffffc) + 8192; \
    W32Ptr = (void*)(W32Buffer + ((CARD32)(DST) & 0x3)); \
}


#define W32_SET_SHORT2(DST) W32_SET_BYTE2(DST)


#define W32_SET_LONG2(DST) \
{ \
    *MBP0 = (CARD32)DST; \
    *MBP1 = (CARD32)(DST) + 8192; \
    W32Ptr = (void*)W32Buffer; \
}


#define W32_BYTE(DST) \
{ \
    *MBP0 = (CARD32)(DST) & 0xfffffffc; \
    DST = (void*)(W32Buffer + ((CARD32)(DST) & 0x3)); \
}
#define W32_SHORT(DST) W32_BYTE(DST)

#define W32_BYTE1(DST) \
{ \
    *MBP1 = (CARD32)(DST) & 0xfffffffc; \
    DST = (void*)(W32Buffer + 8192 + ((CARD32)(DST) & 0x3)); \
}


#define W32_BYTE2(DST) \
{ \
    *MBP0 = (CARD32)(DST) & 0xfffffffc; \
    *MBP1 = ((CARD32)(DST) & 0xfffffffc) + 8192; \
    DST = (void*)(W32Buffer + ((CARD32)(DST) & 0x3)); \
}
#define W32_SHORT2(DST) W32_BYTE2(DST)


#define W32_LONG(DST) \
{ \
    *MBP0 = (CARD32)(DST); \
    DST = (void*)W32Buffer; \
}


#define W32_LONG2(DST) \
{ \
    *MBP0 = (CARD32)(DST); \
    *MBP1 = (CARD32)(DST) + 8192; \
    DST = (void*)W32Buffer; \
}



#define W32_PIXEL(DST, COLOR) W32_PIXEL8(DST, COLOR)


#define W32_PIXEL8(DST, COLOR) \
{ \
    *MBP0 = (CARD32)(DST) & 0xfffffffc; \
    *(W32Buffer + ((CARD32)(DST) & 0x3)) = COLOR; \
} 


#define W32_PIXEL16(DST, COLOR) \
{ \
    *MBP0 = (CARD32)(DST) & 0xfffffffc; \
    *(WordP)(W32Buffer + ((CARD32)(DST) & 0x3)) = COLOR; \
} 


/*
 *  The following is NOT for packed 32-bit pixels 
 */
#define W32_PIXEL32(DST, COLOR) \
{ \
    *MBP0 = (CARD32)(DST); \
    *(LongP)W32Buffer = COLOR; \
} 


/*
 *  The following is NOT for packed 32-bit pixels 
 */
#define W32_PIXEL32(DST, COLOR) \
{ \
    *MBP0 = (CARD32)(DST); \
    *(LongP)W32Buffer = COLOR; \
} 


/* if (cfbGetGCPrivate(pGC)->rop == GXnoop) */

/*GGLGGL*/
#define CHECK_NOOP if (pGC->alu == GXnoop) return;

#define TEST_FB(BASE) FrameBuffer = (CARD32)(BASE) == VGABASE;

#define TEST_SET_FB(BASE) \
{ \
    FrameBuffer = FALSE; \
    if ((CARD32)(BASE) == VGABASE) \
    { \
	FrameBuffer = TRUE; \
	BASE = 0; \
    } \
}

#define FB_LONG(BASE) if (FrameBuffer) W32_LONG(BASE); 
#define FB_LONG2(BASE) if (FrameBuffer) W32_LONG2(BASE); 

#define FB_SET_LONG(BASE) if (FrameBuffer) W32_SET_LONG(BASE); 
#define FB_SET_LONG2(BASE) if (FrameBuffer) W32_SET_LONG2(BASE); 


#define W32_APERTURE 8192

#endif /* W32_H */
