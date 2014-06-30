/* $XConsortium: nglehdw.h,v 1.3 95/01/24 01:58:28 dpw Exp $ */

/*************************************************************************
 * 
 * (c)Copyright 1992 Hewlett-Packard Co.,  All Rights Reserved.
 * 
Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name of Hewlett Packard not be used in
advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.
 *
 *************************************************************************/


#ifndef NGLEHDW_H /*[ NGLEHDW_H */
#define NGLEHDW_H

/* Define pointer to NGLE registers */
#ifndef DREGS_PTR
#define DREGS_PTR pDregs
#endif

#define NGLE_LOCK(pScreenPriv)
#define NGLE_UNLOCK(pScreenPriv)

#define NGLE_READ32(source) ((long)(source))
#define NGLE_WRITE32(dest, data) ((dest) = (long)(data))

typedef ngle_dregs_t	NgleHdwRec;
typedef ngle_dregs_t	*NgleHdwPtr;

#define SETUP_HW(pDregs)						\
{									\
    char stat;								\
    volatile char * pstat = &((pDregs)->reg15.b.b0);			\
    do {								\
	stat = *pstat;							\
	if (!stat)							\
	    stat = *pstat;						\
    } while(stat);							\
}

#define SETUP_FB(pDregs, ID, depth)					\
{									\
    SETUP_HW(pDregs);							\
    switch (ID)								\
    {									\
	case S9000_ID_ARTIST:						\
	case S9000_ID_A1659A:						\
	    (pDregs)->reg10 = 0x13601000;				\
	    break;							\
	case S9000_ID_A1439A:						\
	    if (depth == 24)						\
		(pDregs)->reg10 = 0xBBA0A000;				\
	    else  /* depth = 8 */					\
		(pDregs)->reg10 = 0x13601000;				\
	    break;							\
	case S9000_ID_HCRX:						\
	    if (depth == 24)						\
		(pDregs)->reg10 = 0xBBA0A000;				\
	    else  /* depth = 8 */					\
		(pDregs)->reg10 = 0x13602000;				\
	    break; 							\
	case S9000_ID_TIMBER:						\
	case CRX24_OVERLAY_PLANES:					\
	    (pDregs)->reg10 = 0x13602000;				\
	    break;							\
    }									\
    (pDregs)->reg14.all = 0x83000300;					\
    SETUP_HW(pDregs);							\
    (pDregs)->reg16.b.b1 = 1;						\
}

#ifdef HPUX_ANSIC_ACKNOWLEDGES_VOLATILE_STRUCTURE /* [ */
#define GET_FIFO_SLOTS( cnt, numslots)					\
{									\
    while (cnt < numslots)						\
    {									\
	cnt = DREGS_PTR->reg34.all;					\
    }									\
    cnt -= numslots;							\
}
#else /* Compiler does not treat pDregs as volatile ][ use local volatile */
#define GET_FIFO_SLOTS( cnt, numslots)					\
{									\
    volatile unsigned long * pHwFifoFreeCnt = &(DREGS_PTR->reg34.all);	\
    while (cnt < numslots)						\
    {									\
	cnt = *pHwFifoFreeCnt;						\
    }									\
    cnt -= numslots;							\
}
#endif  /* Use local volatile ] */

#define START_CURSOR_COLORMAP_ACCESS(pDregs) {				\
	    SETUP_HW(pDregs);						\
	    (pDregs)->reg10 = 0xBBE0F000;				\
	    (pDregs)->reg14.all = 0x03000300;				\
	    (pDregs)->reg13 = ~0;					\
	    (pDregs)->reg3 = 0;						\
	    (pDregs)->reg4 = 0;						\
	    (pDregs)->reg4 = 0;						\
	}

#define WRITE_CURSOR_COLOR(pDregs,color) {				\
	    (pDregs)->reg4 = (color);					\
	}

#define FINISH_CURSOR_COLORMAP_ACCESS(pDregs,ID,depth) {		\
	    (pDregs)->reg2 = 0;						\
	    if (ID == S9000_ID_ARTIST)					\
		(pDregs)->reg26 = 0x80008004;				\
	    else							\
		(pDregs)->reg1 = 0x80008004;				\
	    SETUP_FB(pDregs,ID,depth);					\
	}

#define START_IMAGE_COLORMAP_ACCESS(pDregs) {				\
	    SETUP_HW(pDregs);						\
	    (pDregs)->reg10 = 0xBBE0F000;				\
	    (pDregs)->reg14.all = 0x03000300;				\
	    (pDregs)->reg13 = ~0;					\
	}

#define WRITE_IMAGE_COLOR(pDregs, index, color) {			\
	    SETUP_HW(pDregs);						\
	    (pDregs)->reg3 = ((0x100+index) << 2);			\
	    (pDregs)->reg4 = (color);					\
	}

#define FINISH_IMAGE_COLORMAP_ACCESS(pDregs, ID, depth) {		\
	    (pDregs)->reg2 = 0x400;					\
	    if (depth == 24)						\
    		(pDregs)->reg1 = 0x83000100;				\
    	    else  /* depth = 8 */					\
	    {								\
		if (ID == S9000_ID_ARTIST)				\
		    (pDregs)->reg26 = 0x80000100;			\
		else							\
		    (pDregs)->reg1 = 0x80000100;			\
	    }								\
	    SETUP_FB(pDregs, ID, depth);				\
	}

#define GET_CURSOR_SPECS(pDregs,pScreenPriv) {				      \
	    Card32 activeLinesHi, activeLinesLo;			      \
									      \
	    if ((pScreenPriv)->deviceID != S9000_ID_ARTIST)		      \
		(pScreenPriv)->sprite.horizBackPorch = (pDregs)->reg19.b.b1;  \
	    else							      \
		(pScreenPriv)->sprite.horizBackPorch = (pDregs)->reg19.b.b1 + \
						       (pDregs)->reg19.b.b2 + \
						       2;		      \
	    activeLinesLo   = (pDregs)->reg20.b.b0;		      	      \
	    activeLinesHi   = ((pDregs)->reg21.b.b2) & 0xf;		      \
	    (pScreenPriv)->sprite.maxYLine = ((activeLinesHi << 8)	      \
		|(activeLinesLo & 0xff));				      \
	}

#define START_CURSOR_MASK_ACCESS(pDregs) {				\
	    SETUP_HW(pDregs);						\
	    (pDregs)->reg14.all = 0x00000300;				\
	    (pDregs)->reg13 = ~0;					\
	    (pDregs)->reg11 = 0x28A07000;				\
	    (pDregs)->reg3 = 0;						\
	}

#define START_CURSOR_DATA_ACCESS(pDregs) {				\
	    SETUP_HW(pDregs);						\
	    (pDregs)->reg14.all = 0x00000300;				\
	    (pDregs)->reg11 = 0x28A06000;				\
	    (pDregs)->reg3 = 0;						\
	}

#define WRITE_CURSOR_BITS(pDregs,word1,word2) {				\
	    SETUP_HW(pDregs);						\
	    (pDregs)->reg4 = (word1);					\
	    (pDregs)->reg5 = (word2);					\
	}

#define ENABLE_CURSOR(pScreenPriv, cursorXYValue, nOffscreenScanlines) { \
	    (pScreenPriv)->sprite.enabledCursorXYValue = (cursorXYValue);\
	    if ((pScreenPriv)->sprite.moveOnVBlank) {			 \
		while (((pScreenPriv)->pDregs->reg21.all) & 0x00040000); \
		while (!(((pScreenPriv)->pDregs->reg21.all)		 \
			& 0x00040000));					 \
	    }								 \
	    SETUP_HW(pScreenPriv->pDregs);				 \
	    (pScreenPriv)->pDregs->reg17.all = (cursorXYValue);		 \
	    (pScreenPriv)->pDregs->reg18.all =				 \
		    (0x80 | nOffscreenScanlines);			 \
	}

#define DISABLE_CURSOR(pScreenPriv) {					\
	    volatile unsigned long *pDregsCursorXY = 			\
		    &((pScreenPriv)->pDregs->reg17.all);		\
	    long enabledCursorValue = 					\
		    (pScreenPriv)->sprite.enabledCursorXYValue;		\
									\
	    SETUP_HW((pScreenPriv)->pDregs);				\
	    if ((pScreenPriv)->deviceID != S9000_ID_ARTIST)		\
		*pDregsCursorXY = (enabledCursorValue & 0xe007ffff);	\
	    else							\
		(pScreenPriv)->pDregs->reg18.all =			\
			(enabledCursorValue & 0x0000003f);		\
	}

#define GET_ROMTABLE_INDEX(romTableIdx) {				\
	    (romTableIdx) = pDregs->reg16.b.b3 - 1;			\
	}

#define SETUP_RAMDAC(pDregs) {						\
	    volatile Card32         *pAuxControlSpace;			\
									\
	    pAuxControlSpace = (Card32 *) (((Card32) pDregs) + 0x1000); \
	    SETUP_HW(pDregs);						\
	    *(pAuxControlSpace + (0x20>>2)) = 0x04000000;		\
	    *(pAuxControlSpace + (0x28>>2)) = 0xff000000;		\
	}

#define CRX24_SETUP_RAMDAC(pDregs) {					\
	    volatile Card32         *pAuxControlSpace;			\
									\
	    pAuxControlSpace = (Card32 *) (((Card32) pDregs) + 0x1000); \
	    SETUP_HW(pDregs);						\
	    *(pAuxControlSpace)   = 0x04000000;				\
	    *(pAuxControlSpace+1) = 0x02000000;				\
	    *(pAuxControlSpace+2) = 0xff000000;				\
	    *(pAuxControlSpace)   = 0x05000000;				\
	    *(pAuxControlSpace+1) = 0x02000000;				\
	    *(pAuxControlSpace+2) = 0x03000000;				\
	}

#define HCRX_SETUP_RAMDAC(pDregs) {					\
	    NGLE_WRITE32(pDregs->reg32,0xffffffff);			\
	}

#define CRX24_SET_OVLY_MASK(pDregs) {					\
	    SETUP_HW(pDregs);						\
	    (pDregs)->reg11 = 0x13a02000;				\
	    (pDregs)->reg14.all = 0x03000300;				\
    	    (pDregs)->reg3 = 0x000017f0;				\
            (pDregs)->reg13 = 0xffffffff;				\
            (pDregs)->reg22 = (long) (~0UL);				\
            (pDregs)->reg23 = 0x0;					\
	}


#define ENABLE_DISPLAY(pDregs) {					\
	    volatile Card32         *pAuxControlSpace;			\
									\
	    pAuxControlSpace = (Card32 *)(((Card32)(pDregs)) + 0x1000); \
	    SETUP_HW(pDregs);						\
	    *(pAuxControlSpace + (0x30>>2)) = 0x06000000;		\
	    *(pAuxControlSpace + (0x38>>2)) = 0x43000000;		\
	}

#define DISABLE_DISPLAY(pDregs) {					\
	    volatile Card32         *pAuxControlSpace;			\
									\
	    pAuxControlSpace = (Card32 *)(((Card32)(pDregs)) + 0x1000); \
	    SETUP_HW(pDregs);						\
	    *(pAuxControlSpace + (0x30>>2)) = 0x06000000;		\
	    *(pAuxControlSpace + (0x38>>2)) = 0x03000000;		\
	}

#define CRX24_ENABLE_DISPLAY(pDregs) {					\
	    volatile Card32         *pAuxControlSpace;			\
									\
	    pAuxControlSpace = (Card32 *)(((Card32)(pDregs)) + 0x1000); \
	    SETUP_HW(pDregs);						\
	    *pAuxControlSpace     = 0x01000000;				\
	    *(pAuxControlSpace+1) = 0x02000000;				\
	    *(pAuxControlSpace+2) = 0x10000000;				\
	}

#define CRX24_DISABLE_DISPLAY(pDregs) {					\
	    volatile Card32         *pAuxControlSpace;			\
									\
	    pAuxControlSpace = (Card32 *)(((Card32)(pDregs)) + 0x1000); \
	    SETUP_HW(pDregs);						\
	    *pAuxControlSpace     = 0x01000000;				\
	    *(pAuxControlSpace+1) = 0x02000000;				\
	    *(pAuxControlSpace+2) = 0x30000000;				\
	}

#define ARTIST_ENABLE_DISPLAY(pDregs) {					\
	    volatile unsigned long *pDregsMiscVideo = 			\
		    &((pDregs)->reg21.all);				\
	    volatile unsigned long *pDregsMiscCtl = 			\
		    &((pDregs)->reg27.all);				\
									\
	    SETUP_HW(pDregs);						\
	    (pDregs)->reg21.all = *pDregsMiscVideo | 0x0A000000;	\
	    (pDregs)->reg27.all = *pDregsMiscCtl   | 0x00800000;	\
	}

#define ARTIST_DISABLE_DISPLAY(pDregs) {				\
	    volatile unsigned long *pDregsMiscVideo = 			\
		    &((pDregs)->reg21.all);				\
	    volatile unsigned long *pDregsMiscCtl = 			\
		    &((pDregs)->reg27.all);				\
									\
	    SETUP_HW(pDregs);						\
	    (pDregs)->reg21.all = *pDregsMiscVideo & ~0x0A000000;	\
	    (pDregs)->reg27.all = *pDregsMiscCtl   & ~0x00800000;	\
	}

#define HYPER_CONFIG_PLANES_24         0x00000100
#define IS_24_DEVICE(pScreenPriv) \
    (pScreenPriv->deviceSpecificConfig & HYPER_CONFIG_PLANES_24)

#define IS_888_DEVICE(pScreenPriv) (!(IS_24_DEVICE(pScreenPriv)))

#define HYPER_CURSOR_DATA_BIT 0x80
#define HYPER_CURSOR_ENABLE_BIT 0x80000000

#define HYPER_SET_CURSOR_ADDRESS(value)					\
    NGLE_WRITE32(DREGS_PTR->reg30, (value))

#define HYPER_WRITE_CURSOR_DATA(value)					\
    NGLE_WRITE32(DREGS_PTR->reg31, (value))

#define HYPER_CURSOR_XY(x,y)						\
    (((x<0)?(((-x & 0xfff) | 0x1000)<<16):((x & 0xfff)<<16)) |		\
    ((y<0)?((-y & 0xfff) | 0x1000):(y & 0x1fff)));

#define HYPER_ENABLE_CURSOR(pScreenPriv, hypCursorXYValue)		\
{									\
    /* Only called if cursor is visible */				\
    assert(pScreenPriv->sprite.visible == TRUE);			\
									\
    /* Save value written for use by disable cursor */\
    pScreenPriv->sprite.enabledCursorXYValue = (hypCursorXYValue);\
    NGLE_WRITE32(pScreenPriv->pDregs->reg29.all, (hypCursorXYValue) |	\
		 HYPER_CURSOR_ENABLE_BIT);				\
									\
}

#define HYPER_DISABLE_CURSOR(pScreenPriv)				\
{									\
    long enabledCursorValue = pScreenPriv->sprite.enabledCursorXYValue;	\
									\
    NGLE_WRITE32(pScreenPriv->pDregs->reg29.all, (enabledCursorValue &	\
		~(HYPER_CURSOR_ENABLE_BIT)));				\
}

/* Macros for controlling cursor position, used in ngleMoveSprite() */
#define HYPER_ACCEL_BUSY_DODGER_IDLE					\
    ((NGLE_READ32(DREGS_PTR->reg15.all) &  0xc001000) == 0x00010000)

        /* not FIFO paced */
#define HYPER_CPC(pScreenPriv,bifCursorXYValue)				\
    pScreenPriv->sprite.enabledCursorXYValue = (bifCursorXYValue);	\
    NGLE_WRITE32(DREGS_PTR->reg17.all,					\
	(bifCursorXYValue) | HYPER_CURSOR_ENABLE_BIT);

        /* FIFO paced */
#define HYPER_FIFO_CP(pScreenPriv,hypCursorXYValue)			\
    GET_FIFO_SLOTS(nFreeFifoSlots,2);					\
    HYPER_GET_VID_BUS_ACCESS;						\
    HYPER_ENABLE_CURSOR(pScreenPriv, hypCursorXYValue);

#define HYPER_GET_VID_BUS_ACCESS					\
    NGLE_WRITE32(DREGS_PTR->reg28,0);

#define HYPER_ENABLE_DISPLAY(pDregs)					\
{									\
    volatile unsigned long *pDregsHypMiscVideo = 			\
		&((pDregs)->reg33);					\
									\
    SETUP_HW(pDregs);							\
    (pDregs)->reg33 = *pDregsHypMiscVideo | 0x0A000000;			\
}

#define HYPER_DISABLE_DISPLAY(pDregs)					\
{									\
    volatile unsigned long *pDregsHypMiscVideo = 			\
		&((pDregs)->reg33);					\
									\
    SETUP_HW(pDregs);							\
    (pDregs)->reg33 = *pDregsHypMiscVideo & ~0x0A000000;		\
}

/* define the various BufferNumber used by SETUP_ATTR_ACCESS */
#define BUFF0_CMAP0 0x00001e02
#define BUFF1_CMAP0 0x02001e02
#define BUFF1_CMAP3 0x0c001e02
#define ARTIST_CMAP0 0x00000102
#define HYPER_CMAP8  0x00000100
#define HYPER_CMAP24 0x00000800

#define SETUP_ATTR_ACCESS(pDregs,BufferNumber) {			\
	    SETUP_HW(pDregs);						\
	    (pDregs)->reg11 = 0x2EA0D000;				\
	    (pDregs)->reg14.all = 0x23000302;				\
	    (pDregs)->reg12 = (BufferNumber);				\
	    (pDregs)->reg8 = 0xffffffff;				\
	}

#define SET_ATTR_SIZE(pDregs,Width,Height) {				\
	    (pDregs)->reg6.all = 0;					\
	    (pDregs)->reg9.all = (Card32)(((Width)<<16)|(Height));	\
	    (pDregs)->reg6.all = 0x05000000;				\
	    (pDregs)->reg9.all = 0x00040001;				\
	}

#define FINISH_ATTR_ACCESS(pDregs) {					\
	    SETUP_HW(pDregs);						\
	    (pDregs)->reg12 = 0;					\
	}

#define SETUP_COPYAREA(pDregs)						\
	    (pDregs)->reg16.b.b1 = 0;

#define	    IndexedDcd	0	/* Pixel data is indexed (pseudo) color */
#define	    Otc04	2	/* Pixels in each longword transfer (4) */
#define	    Otc32	5	/* Pixels in each longword transfer (32) */
#define	    Ots08	3	/* Each pixel is size (8)d transfer (1) */
#define	    OtsIndirect	6	/* Each bit goes through FG/BG color(8) */
#define	    AddrLong	5	/* FB address is Long aligned (pixel) */
#define	    BINovly	0x2	/* 8 bit overlay */
#define	    BINapp0I	0x0	/* Application Buffer 0, Indexed */
#define	    BINapp1I	0x1	/* Application Buffer 1, Indexed */
#define	    BINapp0F8	0xa	/* Application Buffer 0, Fractional 8-8-8 */
#define	    BINattr	0xd	/* Attribute Bitmap */
#define	    RopSrc 	0x3
#define	    BitmapExtent08  3	/* Each write hits ( 8) bits in depth */
#define	    BitmapExtent32  5	/* Each write hits (32) bits in depth */
#define	    DataDynamic	    0	/* Data register reloaded by direct access */
#define	    MaskDynamic	    1	/* Mask register reloaded by direct access */
#define	    MaskOtc	    0	/* Mask contains Object Count valid bits */

#define MaskAddrOffset(offset) (offset)
#define StaticReg(en) (en)
#define BGx(en) (en)
#define FGx(en) (en)

#define BAJustPoint(offset) (offset)
#define BAIndexBase(base) (base)
#define BA(F,C,S,A,J,B,I)						\
	(((F)<<31)|((C)<<27)|((S)<<24)|((A)<<21)|((J)<<16)|((B)<<12)|(I))

#define IBOvals(R,M,X,S,D,L,B,F)					\
	(((R)<<8)|((M)<<16)|((X)<<24)|((S)<<29)|((D)<<28)|((L)<<31)|((B)<<1)|(F))

#define NGLE_QUICK_SET_IMAGE_BITMAP_OP(val)				\
	NGLE_WRITE32(DREGS_PTR->reg14.all, val)

#define NGLE_QUICK_SET_DST_BM_ACCESS(val)				\
	NGLE_WRITE32(DREGS_PTR->reg11, val)

#define NGLE_QUICK_SET_CTL_PLN_REG(val)					\
	NGLE_WRITE32(DREGS_PTR->reg12, val)

#define NGLE_REALLY_SET_IMAGE_PLANEMASK(plnmsk32)			\
	NGLE_WRITE32(DREGS_PTR->reg13, plnmsk32)

#define NGLE_REALLY_SET_IMAGE_FG_COLOR(fg32)				\
	NGLE_WRITE32(DREGS_PTR->reg35,fg32)

#define NGLE_LONG_FB_ADDRESS(fbaddrbase, x, y) (void *) (		\
	(unsigned long) (fbaddrbase) +					\
	    (								\
		(unsigned int)  ( (y) << 13      ) |			\
		(unsigned int)  ( (x) << 2       )			\
	    )								\
	)

#define NGLE_BINC_SET_DSTADDR(addr)					\
	NGLE_WRITE32(DREGS_PTR->reg3, (addr))

#define NGLE_BINC_SET_SRCADDR(addr)					\
	NGLE_WRITE32(DREGS_PTR->reg2, (addr))

#define NGLE_BINC_SET_DSTMASK(mask)					\
	NGLE_WRITE32(DREGS_PTR->reg22, (mask))

#define NGLE_BINC_WRITE32(data32)					\
	NGLE_WRITE32(DREGS_PTR->reg23, (data32))

#define NGLE_MFGP_REGISTER_TYPE  dreg_cplx_t
#define NGLE_MFGP_REGISTER_TYPE_ASLONG(ngleMfgpReg)     ngleMfgpReg.all

#define NGLE_SET_TRANSFERDATA(trandata32)				\
	NGLE_WRITE32(DREGS_PTR->reg8, (trandata32))

#define HYPER_SET_LENXY_START_FAST_RECFILL(value)			\
	NGLE_WRITE32(DREGS_PTR->reg37.all, (value))

#define NGLE_SET_LENXY_START_RECFILL(lenxy32)				\
	NGLE_WRITE32(DREGS_PTR->reg9.all, (lenxy32))

#define SET_LENXY_START_RECFILL(lenxy32)				\
	NGLE_SET_LENXY_START_RECFILL(NGLE_MFGP_REGISTER_TYPE_ASLONG(lenxy32))

#define NGLE_SET_SCOREBOARD_OVERRIDE(data32)
/*
	NGLE_WRITE32(DREGS_PTR->bifSBO, data32)
*/
#define NGLE_SET_DSTXY(xy32)						\
	NGLE_WRITE32(DREGS_PTR->reg6.all, (xy32))

#define PACK_2CARD16(dest32, highcard16, lowcard16)			\
	dest32.w.high   = (highcard16);					\
	dest32.w.low    = (lowcard16);

#define START_COLORMAPLOAD(cmapBltCtlData32)				\
	NGLE_WRITE32(pDregs->reg38, (cmapBltCtlData32))

#endif /* NGLEHDW_H ]*/
