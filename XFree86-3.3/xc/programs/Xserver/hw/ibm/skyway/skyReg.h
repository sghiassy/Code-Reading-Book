/*
 * $XConsortium: skyReg.h,v 1.5 94/02/06 17:47:06 rws Exp $
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute	this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both	that copyright notice and this permission notice appear	in
 * supporting documentation, and that the name of IBM not be
 * used	in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES	WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND 
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO	EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION	OF CONTRACT, NEGLIGENCE	OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN	CONNECTION WITH	THE USE	OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

/*
 * skyReg.h - Skyway coprocessor registers, constants, and macros.
 *            You should use the macros to set the registers.
 */


#ifndef	SKYREG_H
#define	SKYREG_H

#include <sys/types.h>

/*****************************************************************************
*                                                                            *
*                        Skyway Coprocessor Registers                        *
*                                                                            *
*****************************************************************************/

/* Use macros below: don't set hardware registers directly */

typedef volatile struct	_skyCopReg
{
    ulong  page_dir_base;     /*			      */
    ulong  cur_virt_addr;     /*			      */
    uchar  resvd1;	      /* Unused	space		      */
    uchar  pollreg;	      /* Polling register for pass 2  */
    ushort resvd1a;	      /* Unused	space		      */
    ushort resvd2;	      /* Unused	space		      */
    uchar  state_a_len;	      /*			      */
    uchar  state_b_len;	      /*			      */
    ushort pix_index;	      /*			      */
    uchar  pi_control;	      /*			      */
    uchar  resvd3;	      /* Unused	space		      */
    uchar *pixmap_base;	      /* VRAM addr of Pixmap start    */
    ulong  pix_hw;	      /* pixmap	height and width      */
    ushort resvd4;	      /* Unused	space		      */
    uchar  resvd4a;	      /* Unused	space		      */
    uchar  pixmap_fmt;	      /*			      */
    ulong  bres_errterm;      /* Bressenham Error Term	      */
    ulong  bres_k1;	      /* Bressenham K1 consant	      */
    ulong  bres_k2;	      /* Bressenham K2 consant	      */
    ulong  dir_steps;	      /* Draw and Step codes	      */
    ulong  resvd5;	      /* Unused	space		      */
    ulong  resvd6;	      /* Unused	space		      */
    ulong  resvd7;	      /* Unused	space		      */
    ulong  resvd8;	      /* Unused	space		      */
    ulong  resvd9;	      /* Unused	space		      */
    ulong  resvdrpw;	      /* Unused	space		      */
    ushort color_comp;	      /* Color compare condition      */
    ushort bgfgmix;	      /* bg and	fg alu		      */
    ulong  color_compval;     /* Color compare value	      */
    ulong  plane_mask;	      /* Mask reg for color masking   */
    ulong  car_chainmsk;      /* Carry chain for arith mixes  */
    ulong  fgd_color;	      /* Foreground color	      */
    ulong  bgd_color;	      /* Background color	      */
    long   opdim21;	      /* Height	& Width	for PixBlits  */
    ulong  resvda;	      /* Unused	space		      */
    ulong  resvdb;	      /* Unused	space		      */
    long   maskyx;	      /* Y,X Coordinate	in Clip	Mask  */
    long   src_yx;	      /* Source	Y,X start in Pixmap   */
    long   pat_yx;	      /* Pattern Y,X start in Pixmap  */
    long   dst_yx;	      /* Destination Y,X start in Pix */
    ulong  pixel_op_reg;      /* Pixel Operation: starts cmds */
} skyCopRegRec;

typedef skyCopRegRec *skyCopRegPtr;

/*****************************************************************************
*                                                                            *
*                      Address of Coprocessor and Shadow                     *
*                                                                            *
*****************************************************************************/

extern skyCopRegPtr	COPREG[];		/* Coprocessor address       */
extern skyCopRegRec	COPREG_SHADOW[];	/* Coprocessor shadow struct */


/*****************************************************************************
*                                                                            *
*                 Utility Macros to Set Coprocessor Registers                *
*                         (Don't use these directly)                         *
*                                                                            *
*****************************************************************************/

#define PackDim(w,h)			(((h) - 1) << 16 | (ushort) ((w) - 1))
#define PackCoord(x,y)			((y) << 16 | (short) (x))

#define SKYSetCopShort(i,reg,v)		COPREG[i]->reg = (v)
#define SKYSetCopByte(i,reg,v)		COPREG[i]->reg = (v)

#define SKYSetCop(i,reg,v)		COPREG[i]->reg = (v)
#define SKYSetCop2(i,reg,v1,v2)		copyword2(&COPREG[i]->reg,v1,v2)
#define SKYSetCop3(i,reg,v1,v2,v3)	copyword3(&COPREG[i]->reg,v1,v2,v3)
#define SKYSetCop4(i,reg,v1,v2,v3,v4)	copyword4(&COPREG[i]->reg,v1,v2,v3,v4)


/* Shadow coprocessor value, don't set coprocessor if value has not changed */

#define SHDSetCop(i,reg,v) 				\
{							\
    register ulong temp = (v);				\
    if (COPREG_SHADOW[i].reg != temp)			\
	COPREG[i]->reg = COPREG_SHADOW[i].reg = temp;	\
}

#define SHDSetCopShort(i,reg,v) 			\
{							\
    register ushort temp = (v);				\
    if (COPREG_SHADOW[i].reg != temp)			\
	COPREG[i]->reg = COPREG_SHADOW[i].reg = temp;	\
}


/*****************************************************************************
*                                                                            *
*      Macros to Set/Poll Coprocessor Registers:  Please Use These!!!        *
*                                                                            *
*****************************************************************************/

/* Set Skyway Coprocessor Registers:  Use these to talk to coprocessor */

#define SKYSetPageDirBase(i,base)	SKYSetCop(i,page_dir_base,(base))
#define SKYSetVirtualAddr(i,addr)	SKYSetCop(i,cur_virt_addr,(addr))
#define SKYGetPollReg(i)		(COPREG[i]->pollreg)

#define SKYSetPixmap(i,index,base,w,h,fmt)			\
{								\
	SKYSetCopShort(i, pix_index, index);			\
	SKYSetCop3(i, pixmap_base, base, PackDim(w,h), fmt);	\
}

#define SKYSetBresenConst(i,e,k1,k2)	SKYSetCop3(i,bres_errterm,(e),(k1),(k2))
#define SKYStepAndDraw(i, code)		SKYSetCop(i,dir_steps, (code))

#define SKYSetColorCompCond(i,cond)	SKYSetCopShort(i,color_comp, (cond))
#define SKYSetMix(i,mix)		SHDSetCopShort(i,bgfgmix, mix);
#define SKYSetFgBgMix(i,FgMix,BgMix)	SHDSetCopShort(i,bgfgmix, \
					(((BgMix) << 8) | (FgMix)))

#define SKYSetColorCompValue(i,val)	SKYSetCop(i,color_compval, (val))
#define SKYSetPlaneMask(i,mask)		SHDSetCop(i,plane_mask,0xff & (mask))
#define SKYSetCarryChain(i,cc)		SKYSetCop(i,car_chainmsk, (cc))
#define SKYSetFgColor(i,fg)		SHDSetCop(i,fgd_color, (fg))
#define SKYSetBgColor(i,bg)		SHDSetCop(i,bgd_color, (bg))
#define SKYSetOpDim21(i,w,h)		SHDSetCop(i,opdim21, PackDim(w,h))

/* Note that the following macros use XY instead of YX: more intuitive */

#define SKYSetMaskXY(i,x,y)		SKYSetCop(i,maskyx, PackCoord(x,y))
#define SKYSetSrcXY(i,x,y)		SKYSetCop(i,src_yx, PackCoord(x,y))
#define SKYSetPatXY(i,x,y)		SKYSetCop(i,pat_yx, PackCoord(x,y))
#define SKYSetDstXY(i,x,y)		SKYSetCop(i,dst_yx, PackCoord(x,y))

#define SKYPixelOp(i,op)		SKYSetCop(i,pixel_op_reg, (op))



/*****************************************************************************
*                                                                            *
*      More Utility Macros to Set/Poll Coprocessor:  Please use them!!!      *
*                                                                            *
*****************************************************************************/

#define SKYSyncCoprocessorShadow(i)	COPREG_SHADOW[i] = *COPREG[index]

#define SKYBusyWait(i)			while (SKYGetPollReg(i) & 0x80)
#define SKYIsBusy(i)			(SKYGetPollReg(i) & 0x80)
#define SKYTurnOffColorComp(i)		SKYSetColorCompCond(i,Color_Cmp_Fal)

#define SKYSetALU(i,alu)		SKYSetFgBgMix(i,alu,alu)

#define SKYSetFgBgColor(i,fg,bg)	\
{					\
    SHDSetCop(i,fgd_color, (fg));	\
    SHDSetCop(i,bgd_color, (bg));	\
}

#define SKYSetPatXY_DstXY_PixelOp(i, px,py, dx,dy, op) \
	SKYSetCop3(i,pat_yx,		\
		PackCoord(px,py),	\
		PackCoord(dx,dy),	\
		(op))

#define SKYSetSrcXY_PatXY_DstXY_PixelOp(i, sx,sy, px,py, dx,dy, op) \
	SKYSetCop4(i,src_yx,		\
		PackCoord(sx,sy),	\
		PackCoord(px,py),	\
		PackCoord(dx,dy),	\
		(op))

#define SKYSetDstXY_PixelOp(i,x,y,op)	SKYSetCop2(i,dst_yx, \
					PackCoord(x,y), (op))

#define SKYSetupScreenPix(i,pixsize)	\
	SKYSetPixmap(i,PixMapC,SKY_VRAM_BASE[i],1280,1024, pixsize)


/* Initiates a pixel operation that uses a tile.
 *    Tiles need the pattern offset written to SrcYX register. The PatYX
 *    register does not need to be set, but it is faster to write across
 *    all four registers with one store-string than individually.
 */

#define SKYTilePixelOp(i, PixelOp, x,y, w,h, pat_x,pat_y, pat_w,pat_h)	\
{									\
    int src_x;								\
    int src_y;								\
									\
    /* modulus from misc.h */						\
    modulus(x - pat_x, pat_w, src_x);					\
    modulus(y - pat_y, pat_h, src_y);					\
									\
    SKYBusyWait(i);							\
    SKYSetOpDim21(i, w,h);						\
    SKYSetSrcXY_PatXY_DstXY_PixelOp(i, src_x,src_y, 0,0, x,y, PixelOp);	\
}


/* Initiates a pixel operation that uses a stipple.
 *    Stipples need the pattern offset written to PatYX register.
 */

#define SKYStipplePixelOp(i, PixelOp, x,y, w,h, pat_x,pat_y, pat_w,pat_h) \
{									\
    int src_x;								\
    int src_y;								\
									\
    /* modulus from misc.h */						\
    modulus(x - pat_x, pat_w, src_x);					\
    modulus(y - pat_y, pat_h, src_y);					\
									\
    SKYBusyWait(i);							\
    SKYSetOpDim21(i, w,h);						\
    SKYSetPatXY_DstXY_PixelOp(i, src_x,src_y, x,y, PixelOp);		\
}


/* Initiates a solid-color pixel operation
 */

#define SKYSolidPixelOp(i, PixelOp, x,y, w,h)				\
{									\
    SKYBusyWait(i);							\
    SKYSetOpDim21(i, w,h);						\
    SKYSetDstXY_PixelOp(i, x,y, PixelOp);				\
}

/*****************************************************************************
*                                                                            *
*                         Coprocessor Constants                              *
*                                                                            *
*****************************************************************************/

/* Pixel Map Index Register */

#define MaskMap   0
#define PixMapA   1
#define PixMapB   2
#define PixMapC   3

/* Pixel Map n Format */
/* M/I Format */

#define MI0  0		/* Little-Endian Format */
#define MI1  0x8	/* Big-Endian Format    */

/* Pixel Size */

#define PixSize1  (0 | MI1)
#define PixSize2  (1 | MI1)
#define PixSize4  (2 | MI1)
#define PixSize8  (3 | MI1)
#define PixSize16 (4 | MI1)


/*  Color Compare Condition         */

#define  Color_Cmp_True  0x0        /* Always True (disable updates)  */
#define  Color_Grt_Col   0x1        /* Dest > Col value               */
#define  Color_Equ_Col   0x2        /* Dest = Col value               */
#define  Color_Les_Col   0x3        /* Dest < Col value               */
#define  Color_Cmp_Fal   0x4        /* Always False (enable updates)  */
#define  Color_GtEq_Col  0x5        /* Dest >= Col value              */
#define  Color_NtEq_Col  0x6        /* Dest <> Col value              */
#define  Color_LsEq_Col  0x7        /* Dest <= Col value              */

#define  Plane_Mask_All  0xFFFF
#define  Carry_Mask      0x3FFF


/******************************************************************************
*                                                                             *
*      The Pixel Operations Register is defined as follows:                   *
*                                                                             *
*      +--------+--------+--------+--------+                                  *
*      | byte 0 | byte 1 | byte 2 | byte 3 |                                  *
*      +--------+--------+--------+--------+                                  *
*                                                                             *
*                                                                             *
*      Byte 0 - XX|XX|X|XXX                                                   *
*               __ __ _ ___                                                   *
*                |  | |  +---------- Direction for PixBlts and Lines          *
*                |  | +------------- Unused                                   *
*                |  +--------------- Draw Mode (end point semantics)          *
*                +------------------ Mask Mode                                *
*                                                                             *
*      Byte 1 - XXXX|XXXX                                                     *
*               ____ ____                                                     *
*                 |    +------------ Unused                                   *
*                 +----------------- Pattern                                  *
*                                                                             *
*      Byte 2 - XXXX|XXXX                                                     *
*               ____ ____                                                     *
*                 |    +------------ Destination Pixel Map                    *
*                 +----------------- Source Pixel Map                         *
*                                                                             *
*      Byte 3 - XX|XX|XXXX                                                    *
*               __ __ ____                                                    *
*                |  |   +----------- Step - specifies drawing operation       *
*                |  +--------------- Foreground color source                  *
*                +------------------ Background color source                  *
*                                                                             *
******************************************************************************/

/* Background color source */

#define POBackReg 0                /* Background color  (register)   */
#define POBackSrc 0x80             /* Source Pixel Map               */

/* Foreground color source */

#define POForeReg 0                /* Foreground  color (register)   */
#define POForeSrc 0x20             /* Source Pixel Map               */

/* Step */

#define POStepDSR 0x2              /* Draw & Step Read               */
#define POStepLDR 0x3              /* Line Draw   Read               */
#define POStepDSW 0x4              /* Draw & Step Write              */
#define POStepLDW 0x5              /* Line Draw   Write              */
#define POStepBlt 0x8              /* Pxblt                          */
#define POStepIBlt 0x9             /* Inverting Pxblt                */
#define POStepAFBlt 0xa            /* Area Fill Pxblt                */

/* Source */

#define POSrcA 0x1000              /* Pixel Map A                    */
#define POSrcB 0x2000              /* Pixel Map B                    */
#define POSrcC 0x3000              /* Pixel Map C                    */
#define POSrcD 0x0000              /* Mask Map  D                    */

/* Destination */

#define PODestA 0x100              /* Pixel Map A                    */
#define PODestB 0x200              /* Pixel Map B                    */
#define PODestC 0x300              /* Pixel Map C                    */
#define PODestD 0x000              /* Mask Map  D                    */

/* Pattern */

#define POPatA 0x100000            /* Pixel Map A                    */
#define POPatB 0x200000            /* Pixel Map B                    */
#define POPatC 0x300000            /* Pixel Map C                    */
#define POPatD 0x000000            /* Mask Map  D                    */

#define POPatFore 0x800000         /* Foreground (Fixed)             */
#define POPatSrc 0x900000          /* Generated from Source          */

/* Mask */

#define POMaskDis 0                /* Mask Map Disabled             */
#define POMaskBEn 0x40000000       /* Mask Map Boundary Enabled     */
#define POMaskEn  0x80000000       /* Mask Map Enabled              */

/* Drawing Mode */

#define POModeAll 0                /* Draw All Pixels                */
#define POModeLast 0x10000000      /* Draw 1s Pixel Null             */
#define POModeFirst 0x20000000     /* Draw Last Pixel Null           */
#define POModeArea 0x30000000      /* Draw Area Boundary             */

/* Direction Octant */

#define POOct0 0
#define POOct1 0x1000000
#define POOct2 0x2000000
#define POOct3 0x3000000
#define POOct4 0x4000000
#define POOct5 0x5000000
#define POOct6 0x6000000
#define POOct7 0x7000000

/*  Octant fields      */

#define  DX              0x4000000
#define  DY              0x2000000
#define  DZ              0x1000000

#endif /* SKYREG_H */
