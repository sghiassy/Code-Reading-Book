/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/i128/i128reg.h,v 3.5.2.1 1997/05/23 12:19:39 dawes Exp $ */
/*
 * Copyright 1994 by Robin Cutshaw <robin@XFree86.Org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Robin Cutshaw not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Robin Cutshaw makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ROBIN CUTSHAW DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ROBIN CUTSHAW BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: i128reg.h /main/4 1996/05/12 20:56:19 kaleb $ */


struct i128pci {
    CARD32 devicevendor;
    CARD32 statuscommand;
    CARD32 classrev;
    CARD32 bhlc;
    CARD32 base0;
    CARD32 base1;
    CARD32 base2;
    CARD32 base3;
    CARD32 base4;
    CARD32 base5;
    CARD32 rsvd0;
    CARD32 rsvd1;
    CARD32 baserom;
    CARD32 rsvd2;
    CARD32 rsvd3;
    CARD32 lgii;
};

struct i128io {
    CARD32 rbase_g;
    CARD32 rbase_w;
    CARD32 rbase_a;
    CARD32 rbase_b;
    CARD32 rbase_i;
    CARD32 rbase_e;
    CARD32 id;
    CARD32 config1;
    CARD32 config2;
    CARD32 rsvd1;
    CARD32 soft_sw;
};

struct i128mem {
    unsigned char *mw0_ad;
    unsigned char *mw1_ad;
    unsigned char *xyw_ada;
    unsigned char *xyw_adb;
    CARD32 *rbase_g;
    CARD32 *rbase_w;
    CARD32 *rbase_a;
    CARD32 *rbase_b;
    CARD32 *rbase_i;
    unsigned char *rbase_g_b;  /* special byte pointer for ramdac registers */
};

#define I128_DEVICE_ID1		0x2309105D
#define I128_DEVICE_ID2		0x2339105D

/* RBASE_I register offsets */

#define GINTP 0x0000
#define GINTM 0x0004

/* RBASE_G register offsets  (divided by four for double word indexing */

#define WR_ADR   0x0000     /* use rbase_g_b for byte indexing */
#define PAL_DAT  0x0004     /* use rbase_g_b for byte indexing */
#define PEL_MASK 0x0008     /* use rbase_g_b for byte indexing */
#define RD_ADR   0x000C     /* use rbase_g_b for byte indexing */
#define INDEX_TI 0x0018     /* TI  ramdac use rbase_g_b for byte indexing */
#define DATA_TI  0x001C     /* TI  ramdac use rbase_g_b for byte indexing */
#define IDXL_I   0x0010     /* IBM ramdac use rbase_g_b for byte indexing */
#define IDXH_I   0x0014     /* IBM ramdac use rbase_g_b for byte indexing */
#define DATA_I   0x0018     /* IBM ramdac use rbase_g_b for byte indexing */
#define IDXCTL_I 0x001C     /* IBM ramdac use rbase_g_b for byte indexing */
#define INT_VCNT 0x0020/4
#define INT_HCNT 0x0024/4
#define DB_ADR   0x0028/4
#define DB_PTCH  0x002C/4
#define CRT_HAC  0x0030/4
#define CRT_HBL  0x0034/4
#define CRT_HFP  0x0038/4
#define CRT_HS   0x003C/4
#define CRT_VAC  0x0040/4
#define CRT_VBL  0x0044/4
#define CRT_VFP  0x0048/4
#define CRT_VS   0x004C/4
#define CRT_BORD 0x0050/4
#define CRT_ZOOM 0x0054/4
#define CRT_1CON 0x0058/4
#define CRT_2CON 0x005C/4


/* RBASE_W register offsets  (divided by four for double word indexing */

#define MW0_CTRL 0x0000/4
#define MW0_AD   0x0004/4
#define MW0_SZ   0x0008/4   /* 2MB = 0x9, 4MB = 0xA, 8MB = 0xB */
#define MW0_PGE  0x000C/4
#define MW0_ORG  0x0010/4
#define MW0_MSRC 0x0018/4
#define MW0_WKEY 0x001C/4
#define MW0_KDAT 0x0020/4
#define MW0_MASK 0x0024/4


/* RBASE_[AB] register offsets  (divided by four for double word indexing */

#define INTP     0x0000/4
#define  INTP_DD_INT 0x01	/* drawing op completed  */
#define  INTP_CL_INT 0x02
#define INTM     0x0004/4
#define  INTM_DD_MSK 0x01
#define  INTM_CL_MSK 0x02
#define FLOW     0x0008/4
#define  FLOW_DEB    0x01	/* drawing engine busy   */
#define  FLOW_MCB    0x02	/* mem controller busy   */
#define  FLOW_CLP    0x04
#define  FLOW_PRV    0x08	/* prev cmd still running or cache ready */
#define BUSY     0x000C/4
#define  BUSY_BUSY   0x01	/* command pipeline busy */
#define XYW_AD   0x0010/4
#define Z_CTRL   0x0018/4
#define BUF_CTRL 0x0020/4
#define  BC_AMV      0x02
#define  BC_MP       0x04
#define  BC_AMD      0x08
#define  BC_SEN_MSK  0x0300
#define  BC_SEN_DB   0x0000
#define  BC_SEN_VB   0x0100
#define  BC_SEN_MB   0x0200
#define  BC_SEN_CB   0x0300
#define  BC_DEN_MSK  0x0C00
#define  BC_DEN_DB   0x0000
#define  BC_DEN_VB   0x0400
#define  BC_DEN_MB   0x0800
#define  BC_DEN_CB   0x0C00
#define  BC_DSE      0x1000
#define  BC_VSE      0x2000
#define  BC_MSE      0x4000
#define  BC_PS_MSK   0x001F0000
#define  BC_MDM_MSK  0x00600000
#define  BC_MDM_KEY  0x00200000
#define  BC_MDM_PLN  0x00400000
#define  BC_PSIZ_MSK 0x03000000
#define  BC_PSIZ_8B  0x00000000
#define  BC_PSIZ_16B 0x01000000
#define  BC_PSIZ_32B 0x02000000
#define  BC_PSIZ_NOB 0x03000000
#define  BC_CO       0x40000000
#define  BC_CR       0x80000000
#define DE_PGE   0x0024/4
#define  DP_DVP_MSK  0x0000001F
#define  DP_MP_MSK   0x000F0000
#define DE_SORG   0x0028/4
#define DE_DORG   0x002C/4
#define DE_MSRC   0x0030/4
#define DE_WKEY   0x0038/4
#define DE_KYDAT  0x003C/4
#define DE_SPTCH  0x0040/4
#define DE_DPTCH  0x0044/4
#define CMD       0x0048/4
#define  CMD_OPC_MSK 0x000000FF
#define  CMD_ROP_MSK 0x0000FF00
#define  CMD_STL_MSK 0x001F0000
#define  CMD_CLP_MSK 0x00E00000
#define  CMD_PAT_MSK 0x0F000000
#define  CMD_HDF_MSK 0x70000000
#define  CMD_BLIT    0x00000C01
#define CMD_OPC   0x0050/4
#define  CO_NOOP     0x00
#define  CO_BITBLT   0x01
#define  CO_LINE     0x02
#define  CO_ELINE    0x03
#define  CO_TRIAN    0x04
#define  CO_RXFER    0x06
#define  CO_WXFER    0x07
#define CMD_ROP   0x0054/4
#define  CR_CLEAR    0x00
#define  CR_NOR      0x01
#define  CR_AND_INV  0x02
#define  CR_COPY_INV 0x03
#define  CR_AND_REV  0x04
#define  CR_INVERT   0x05
#define  CR_XOR      0x06
#define  CR_NAND     0x07
#define  CR_AND      0x08
#define  CR_EQUIV    0x09
#define  CR_NOOP     0x0A
#define  CR_OR_INV   0x0B
#define  CR_COPY     0x0C
#define  CR_OR_REV   0x0D
#define  CR_OR       0x0E
#define  CR_SET      0x0F
#define CMD_STYLE 0x0058/4
#define  CS_SOLID    0x01
#define  CS_TRNSP    0x02
#define  CS_STP_NO   0x00
#define  CS_STP_PL   0x04
#define  CS_STP_PA32 0x08
#define  CS_STP_PA8  0x0C
#define  CS_EDI      0x10
#define CMD_PATRN 0x005C/4
#define  CP_APAT_NO  0x00
#define  CP_APAT_8X  0x01
#define  CP_APAT_32X 0x02
#define  CP_NLST     0x04
#define  CP_PRST     0x08
#define CMD_CLP   0x0060/4
#define  CC_NOCLP    0x00
#define  CC_CLPRECI  0x02
#define  CC_CLPRECO  0x03
#define  CC_CLPSTOP  0x04
#define CMD_HDF   0x0064/4
#define  CH_BIT_SWP  0x01
#define  CH_BYT_SWP  0x02
#define  CH_WRD_SWP  0x04
#define FORE      0x0068/4
#define BACK      0x006C/4
#define MASK      0x0070/4
#define RMSK      0x0074/4
#define LPAT      0x0078/4
#define PCTRL     0x007C/4
#define  PC_PLEN_MSK  0x0000001F
#define  PC_PSCL_MSK  0x000000E0
#define  PC_SPTR_MSK  0x00001F00
#define  PC_SSCL_MSK  0x0000E000
#define  PC_STATE_MSK 0xFFFF0000
#define CLPTL     0x0080/4
#define  CLPTLY_MSK   0x0000FFFF
#define  CLPTLX_MSK   0xFFFF0000
#define CLPBR     0x0084/4
#define  CLPBRY_MSK   0x0000FFFF
#define  CLPBRX_MSK   0xFFFF0000
#define XY0_SRC   0x0088/4
#define XY1_DST   0x008C/4      /* trigger */
#define XY2_WH    0x0090/4
#define XY3_DIR   0x0094/4
#define  DIR_LR_TB    0x00000000
#define  DIR_LR_BT    0x00000001
#define  DIR_RL_TB    0x00000002
#define  DIR_RL_BT    0x00000003
#define XY4_ZM    0x0098/4
#define  ZOOM_NONE    0x00000000
#define  XY_Y_DATA    0x0000FFFF
#define  XY_X_DATA    0xFFFF0000
#define  XY_I_DATA1   0x0000FFFF
#define  XY_I_DATA2   0xFFFF0000

#define I128_WAIT_READY 1
#define I128_WAIT_DONE  2

typedef struct {
	unsigned char r, b, g;
} LUTENTRY;

#define RGB8_PSEUDO      (-1)
#define RGB16_565         0
#define RGB16_555         1
#define RGB32_888         2
