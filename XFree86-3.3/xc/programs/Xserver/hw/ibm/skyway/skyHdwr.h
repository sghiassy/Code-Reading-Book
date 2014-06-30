/*
 * $XConsortium: skyHdwr.h,v 1.6 92/11/23 10:24:12 eswu Exp $
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

/*
 * skyHdwr.h - Skyway IO registers, constants, and macros
 */

#ifndef SKYHDWR_H
#define SKYHDWR_H

#include <sys/types.h>

/*****************************************************************************
*                                                                            *
*                            Skyway IO Registers                             *
*                                                                            *
*****************************************************************************/

typedef volatile struct	_skyIOReg
{
    uchar op_mode;	      /* Adapter operating mode	      */
    uchar pc_vram_window;     /* 64K or	1MB VRAM Window	      */
    uchar resv1;	      /* Reserved register	      */
    uchar resv2;	      /* Reserved register	      */

    uchar int_enable;	      /* Interrupts enable flags      */
    uchar int_status;	      /* Interrupt status	      */
    uchar vmem_int_enable;    /* Virtual-mem interrupt enab.  */
    uchar vmem_int_stat;      /* Virtual memory	int status    */

    uchar vram_index;	      /*			      */
    uchar mem_acc_mode;	      /* Pixel-size for	framebuffer:  */
			      /* 0xA for 4-bit,	0xB for	8-bit */
    uchar index;	      /* Index another register set   */
    uchar data_b;	      /* Data register for 8 bit ops  */

    uchar data_c;	      /* Data register for 16/32 bit  */
    uchar data_d;	      /* Data register for 16/32 bit  */
    uchar data_e;	      /* Data register for 32 bit     */
    uchar data_f;	      /* Data register for 32 bit     */
} skyIORegRec, *skyIORegPtr;


/*****************************************************************************
*                                                                            *
*                        Skyway hardware addresses                           *
*                                                                            *
*****************************************************************************/

extern ulong		SKY_SEGMENT[];		/* Skyway segment address    */
extern ulong		SKY_VRAM_BASE[];	/* Offset of VRAM area from  */
						/* segment address           */
extern ulong		SKY_VRAM_START[];	/* Start of VRAM area        */
extern ulong		SKY_DMA0[];		/* DMA address 0             */
extern ulong		SKY_DMA1[];		/* DMA address 1             */
extern ulong		SKY_DMA2[];		/* DMA address 2             */
extern ulong		SKY_DMA3[];		/* DMA address 3             */

extern skyIORegPtr	IOREG[];		/* IO Register address       */

extern ulong		*SKY_TILESTIP[];	/* Table of installed tiles  */
extern ulong		SKY_TILESTIPID_CTR[];	/* TileStip ID counter       */



/*****************************************************************************
*                                                                            *
*                      Macros to Access IO Registers                         *
*                                                                            *
*****************************************************************************/

#define SKY_MODE_REG(i)			IOREG[i]->op_mode
#define SKY_WINCTRL_REG(i) 		IOREG[i]->pc_vram_window
#define SKY_INT_REG(i)			IOREG[i]->int_enable
#define SKY_INS_REG(i)			IOREG[i]->int_status
#define SKY_VMC_REG(i)			IOREG[i]->vmem_int_enable
#define SKY_VMS_REG(i)			IOREG[i]->vmem_int_stat
#define SKY_VMI_REG(i)			IOREG[i]->vram_index
#define SKY_MEM_REG(i)			IOREG[i]->mem_acc_mode

#define SKY_BINDEX_REG(i)		IOREG[i]->index
#define SKY_DATAB_REG(i)		IOREG[i]->data_b
#define SKY_DATAC_REG(i)		IOREG[i]->data_c
#define SKY_DATAD_REG(i)		IOREG[i]->data_d
#define SKY_DATAE_REG(i)		IOREG[i]->data_e

#define SKY_SINDEX_REG(i) \
	(*((volatile unsigned short *)(&IOREG[i]->index)))


#define SKYSetRGBColor(index,r,g,b)					\
{									\
    SKY_SINDEX_REG(index)  =  (PALETTESEQ  << 8);			\
    SKY_SINDEX_REG(index)  =  (PALETTEDATA << 8) | ((r) >> 8);		\
    SKY_SINDEX_REG(index)  =  (PALETTEDATA << 8) | ((g) >> 8);		\
    SKY_SINDEX_REG(index)  =  (PALETTEDATA << 8) | ((b) >> 8);		\
}

#define SKYSetColorIndex(index,n)            				\
{									\
    SKY_SINDEX_REG(index) = (PALETTEMASK << 8) | 0xff;			\
    SKY_SINDEX_REG(index) = (SPINDEXLO << 8)   | (0xff & (n));		\
    SKY_SINDEX_REG(index) = (SPINDEXHI << 8)   | 0x00;			\
}

#define SKYSetColor(index,color_index,red,green,blue)			\
{									\
    /* Prevents blinking. */						\
    SKY_SINDEX_REG(index) = (SPINDEXLO << 8) | 0x07;			\
    SKY_SINDEX_REG(index) = (PALETTEMASK << 8) | 0x00;			\
									\
    SKYSetColorIndex(index, color_index);				\
    SKYSetRGBColor(index, red, green, blue);				\
}


/*****************************************************************************
*                                                                            *
*                         Skyway hardware constants                          *
*                                                                            *
*****************************************************************************/

#ifdef SKY_MBUF_HACK
#define SKY_WIDTH            (1280/2)
#define SKY_HEIGHT           (1024/2)
#else
#define SKY_WIDTH            1280
#define SKY_HEIGHT           1024
#endif

#define SKY_INVISO_SIZE		(SC_VRAM_END_OFF - SC_INVIS_VRAM_OFF + 1)
#define SKY_TILESTIP_AREAS	0x8
#define SKY_MAX_TILESTIP_SIZE	(SKY_INVISO_SIZE/SKY_TILESTIP_AREAS)

#define SKY_INVIS_VRAM_BASE(scrnNum) \
    (SKY_VRAM_BASE[scrnNum] + SC_INVIS_VRAM_OFF)

/* skycolor-- offsets from SKY_VRAM_BASE[scrnNum] or SKY_VRAM_START[scrnNum] */

#define SC_INVIS_VRAM_OFF  0x140000	/* Start of off-screen VRAM */
#define SC_VRAM_END_OFF    0x1fffff	/* Last byte of VRAM        */


/*   CRTC REGISTERS     */

#define Display_Mode1   0x5000      /* Display Mode 1 Register        */
#define Display_Mode2   0x5100      /* Display Mode 2 Register        */

/*   hardware cursor                                                  */

#define CursLo_Plane0   0x5600      /* Cursor address low plane 0     */
#define CursLo_Plane1   0x5700      /* Cursor address low plane 1     */
#define CursHi_Plane0   0x5800      /* Cursor address high plane 0    */
#define CursHi_Plane1   0x5900      /* Cursor address high plane 0    */
#define CursImg_Plane0  0x5A00      /* Cursor image plane 0           */
#define CursImg_Plane1  0x5B00      /* Cursor image plane 1           */
#define CursIndex       0x6000      /* Cursor index                   */
#define CursData        0x6A00      /* Cursor data                    */
#define CursCntl_Plane0 0x6C00      /* Cursor control plane 0         */
#define CursCntl_Plane1 0x6D00      /* Cursor control plane 1         */

/*   colormap                                                         */

#define PaletIndex      0x6000      /* Palette Index                  */
#define PaletCntl       0x6400      /* Palette DAC control            */
#define PaletData       0x6500      /* Palette Data                   */

/*   misc stuff                                                       */

#define x_correct       0x01A8      /* x correction factor for pass 1 */
#define y_correct       0x001A      /* y correction factor for pass 1 */

#define xc_correct2     0x01AD      /* x cursor offset, color, pass 2 */
#define yc_correct2     0x001B      /* y cursor offset, color, pass 2 */
#define xm_correct2     0x0194      /* x cursor offset, mono, pass 2  */
#define ym_correct2     0x001F      /* y cursor offset, mono, pass 2  */

#define DAC_disable     0x0004
#define DAC_enable      0x0044
#define Video_disable   0x0061
#define Video_enable    0x0063
#define ColorCmd        0x38
#define CursPlaneSize   512
#define MaxCursorSize   64
#define BestCursorSize  32


/* Interrupt Enable Register */

#define FRAMEFLYBACK           (1 << 0)
#define COPREJECTED            (1 << 2)
#define COPCOMPLETED           (1 << 3)


/*  Mode    */

#define VGAINHIBITED            0x0
#define VGAENABLED              0x1
#define EXTVGAINHIBITED         0x2
#define EXTVGAENABLED           0x3
#define INTELINHIBITED          0x4
#define INTELENABLED            0x5
#define MOTOROLAINHIBITED       0x6
#define MOTOROLAENABLED         0x7


/*****************************************************************************
*                                                                            *
*          Index Registers (access through ``index'' IO register)            *
*                                                                            *
*****************************************************************************/

#define MEMORYCONF              0x00    /* Memory Configuration Register */
#define COPSAVERESTOREDATA1     0x0c    /* Save/Restore Data 1 */
#define COPSAVERESTOREDATA2     0x0d    /* Save/Restore Data 2 */

#define HORIZONTALTOTAL         0x10    /* Horizontal Total Register */
#define HORIZONTALDPYEND        0x12    /* Horizontal Dpy End Register  */
#define HORIZONTALBLANKSTART    0x14    /* Horizontal Blank Start       */
#define HORIZONTALBLANKEND      0x16    /* Horizontal Blank End         */
#define HORIZONTALPULSESTART    0x18    /* Horizontal Pulse Start       */
#define HORIZONTALPULSEEND1     0x1a    /* Horizontal Pulse End 1       */
#define HORIZONTALPULSEEND2     0x1c    /* Horizontal Pulse End 2       */
#define HORIZONTSPRITELO        0x30    /* Horizontal Sprite Lo Register */
#define HORIZONTSPRITEMI        0x31    /* Horizontal Sprite MI Register */
#define HORIZONTSPRITEHI        0x32    /* Horizontal Sprite HI Register */

#define VERTICALTOTALLO         0x20    /* Vertical Total Lo Register */
#define VERTICALTOTALHI         0x21    /* Vertical Total Hi Register */

#define VERTICALDPYENDLO        0x22    /* Vertical Dpy End Lo Register */
#define VERTICALDPYENDLHI       0x23    /* Vertical Dpy End Hi Register */
#define VERTICALBLANKSTARTLO    0x24    /* Vertical Blank Start Lo Register */
#define VERTICALBLANKSTARTHI    0x25    /* Vertical Blank Start Hi Register */
#define VERTICALBLANKENDLO      0x26    /* Vertical Blank End Lo Register */
#define VERTICALBLANKENDHI      0x27    /* Vertical Blank End Hi Register */

#define VERTICALSYNCPULSESTLO   0x28    /* Vertical Sync Pulse Start Lo */
#define VERTICALSYNCPULSESTHI   0x29    /* Vertical Sync Pulse Start Hi */
#define VERTICALSYNCPULSEPEND   0x2a    /* Vertical Sync Pulse End */
#define VERTICALLINECMPLO       0x2c    /* Vertical Line Compare Lo */
#define VERTICALLINECMPHI       0x2d    /* Vertical Line Compare Hi */

#define VERTICALSPRITESTARTLO   0x33    /* Vertical Sprite Start Lo Register */
#define VERTICALSPRITESTARTHI   0x34    /* Vertical Sprite Start Hi  Register */
#define VERTICALSPRITEPRESET    0x35    /* Vertical Sprite Start Preset */

#define SPRITECTRL              0x36    /* Sprite Control Register */
#define SPRITE0RED              0x38    /* Sprite Color 0 Red Register */
#define SPRITE0GREEN            0x39    /* Sprite Color 0 Green Register */
#define SPRITE0BLUE             0x3a    /* Sprite Color 0 Blue Register */
#define SPRITE1RED              0x3b    /* Sprite Color 1 Red Register */
#define SPRITE1GREEN            0x3c    /* Sprite Color 1 Green Register */
#define SPRITE1BLUE             0x3d    /* Sprite Color 1 Blue Register */

#define STARTADDRLO             0x40    /* Start Address Lo Register */
#define STARTADDRMI             0x41    /* Start Address Mi Register */
#define STARTADDRHI             0x42    /* Start Address Hi Register */
#define BUFFERPITCHLO           0x43    /* Buffer Pitch Lo Register */
#define BUFFERPITCHHI           0x44    /* Buffer Pitch Hi Register */

#define DPYMODE1                0x50    /* Display Mode 1 Register */
#define DPYMODE2                0x51    /* Display Mode 2 Register */
#define MONITORID               0x52    /* Monitor ID Register */
#define SYSTEMID                0x53    /* System  ID Register */
#define CLOCKFREQUENCE          0x54    /* Clock Frequency Select Register */
#define BORDERCOLOR             0x55    /* Border Color Register */
#define SPINDEXLO               0x60    /* Sprite/Palette Index Lo Register */
#define SPINDEXHI               0x61    /* Sprite/Palette Index Hi Register */
#define SPINDEXLOPF             0x62    /* S/P Index Lo Prefetch Register */
#define SPINDEXHIPF             0x63    /* S/P Index Hi Prefetch Register */
#define PALETTEMASK             0x64    /* Palette Mask Register */
#define PALETTEDATA             0x65    /* Palette Data Register */

#define PALETTESEQ              0x66    /* Palette Sequence Register */
#define PALETTERED              0x67    /* Palette Red Prefetch Register */
#define PALETTEGREEN            0x68    /* Palette Green Prefetch Register */
#define PALETTEBLUE             0x69    /* Palette Blue Prefetch Register */
#define SPRITEDATA              0x6a    /* Sprite Data Register */
#define SPRITEPF                0x6b    /* Sprite Prefetch Register */

#endif /* SKYHDWR_H */
