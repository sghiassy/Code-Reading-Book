/* $XConsortium: nvreg.h /main/2 1996/10/28 05:13:41 kaleb $ */
 /***************************************************************************\
|*                                                                           *|
|*          Copyright (c) 1996 NVIDIA, Corp.  All rights reserved.           *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1996  NVIDIA, Corp.  NVIDIA design patents  pending     *|
|*     in the U.S. and foreign countries.                                    *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/nv/nvreg.h,v 3.2 1996/12/27 07:05:43 dawes Exp $ */

#ifndef __NVREG_H_
#define __NVREG_H_

#define NV_FRAME_BUFFER 0x01000000 /* Frame buffer is at this BYTE address*/

/* Frame buffer registers */
#define NV_PFB_BOOT_0             (0x600000/4)
#define NV_PFB_CONFIG_0           (0x600200/4)
#define NV_PFB_START              (0x600400/4)
#define NV_PFB_HOR_FRONT_PORCH    (0x600500/4)
#define NV_PFB_HOR_SYNC_WIDTH     (0x600510/4)
#define NV_PFB_HOR_BACK_PORCH     (0x600520/4)
#define NV_PFB_HOR_DISP_WIDTH     (0x600530/4)
#define NV_PFB_VER_FRONT_PORCH    (0x600540/4)
#define NV_PFB_VER_SYNC_WIDTH     (0x600550/4)
#define NV_PFB_VER_BACK_PORCH     (0x600560/4)
#define NV_PFB_VER_DISP_WIDTH     (0x600570/4)

#define NV_PFB_BOOT_0_RAM_AMOUNT         1,0
#define NV_PFB_CONFIG_0_VERTICAL         0,0     /* 1 during blank */
#define NV_PFB_CONFIG_0_PIXEL_DEPTH      9,8
#define NV_PFB_CONFIG_0_SCANLINE         20,20
#define NV_PFB_CONFIG_0_PCLK_VCLK_RATIO  26,24
#define NV_PFB_CONFIG_0_RESOLUTION       6,4

/* Text mode config registers */
#define NV_PRM_CONFIG_0              (0x6c0200/4)
#define NV_MEMORY_TRACE              (0x6c1f00/4)

#define NV_PBUS_ACCESS               (0x1200/4)


/* Addreses for the DAC micro port */
#define NV_DAC_WRITE_PALETTE   (0x609000/4)
#define NV_DAC_COLOUR_DATA     (0x609004/4)
#define NV_DAC_PIXEL_MASK      (0x609008/4)
#define NV_DAC_READ_PALETTE    (0x60900c/4)
#define NV_DAC_INDEX_LO        (0x609010/4)
#define NV_DAC_INDEX_HI        (0x609014/4)
#define NV_DAC_INDEX_DATA      (0x609018/4)
#define NV_DAC_GAME_PORT       (0x60901c/4)


/* Extended register values */
#define NV_DAC_COMPANY_ID             0x00
#define NV_DAC_DEVICE_ID              0x01
#define NV_DAC_REVISION_ID            0x02
#define NV_DAC_CONF_0                 0x04
#define NV_DAC_CONF_1                 0x05
#define NV_DAC_RGB_PAL_CTRL           0x09 
#define NV_DAC_VPLL_M_PARAM           0x18
#define NV_DAC_VPLL_N_PARAM           0x19
#define NV_DAC_VPLL_O_PARAM           0x1a
#define NV_DAC_VPLL_P_PARAM           0x1b

#define NV_DAC_CONF_0_IDC_MODE        5,5
#define NV_DAC_CONF_0_VGA_STATE       4,4
#define NV_DAC_CONF_0_PORT_WIDTH      3,2
#define NV_DAC_CONF_0_VISUAL_DEPTH    1,0

#define NV_DAC_CONF_1_VCLK_IMPEDANCE  3,3
#define NV_DAC_CONF_1_PCLK_VCLK_RATIO 2,0

#define NV_DAC_SGS_ID                 0x44
#define NV_DAC_1764_ID                0x64
#define NV_DAC_1732_ID                0x32



/* This points at the base of the memory mapped for the NV1
 * Note that it is an int pointer, all writes must be 32 bit
 */
extern volatile int *nvPort;

#define ReadExtReg(reg) \
  ((nvPort[NV_DAC_INDEX_LO] = (reg) & 0xff),\
  (nvPort[NV_DAC_INDEX_HI] = ((reg) >> 8) & 0xff),\
  (nvPort[NV_DAC_INDEX_DATA] & 0xff))
#define WriteExtReg(reg,value)\
  nvPort[NV_DAC_INDEX_LO] = (reg) & 0xff,\
  nvPort[NV_DAC_INDEX_HI] = ((reg) >> 8) & 0xff,\
  nvPort[NV_DAC_INDEX_DATA] = ((value) & 0xff)

/* Little macro to construct bitmask for contiguous ranges of bits */
#define BITFIELDMASK(t,b) (((unsigned)(1U << (((t)-(b)+1)))-1)  << (b))
#define LOW_VALUE_OF_BITFIELDMASK(t,b) (b)
/* Macro to set specific bitfields (mask has to be a macro x,y) ! */
#define SETBITFIELD(var,mask,value) var=((var) & (~BITFIELDMASK(mask))) | \
                                ((value) << LOW_VALUE_OF_BITFIELDMASK(mask))
#define GETBITFIELD(var,mask) (((unsigned)((var) & BITFIELDMASK(mask))) \
                                   >> LOW_VALUE_OF_BITFIELDMASK(mask))

#endif


