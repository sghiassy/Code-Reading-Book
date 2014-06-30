/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/newmmio.h,v 3.6 1996/11/24 09:54:06 dawes Exp $ */
/***************************************************************************
 * 
 * typedefs and macros for old and new MMIO mode, Trio64V+ and 868/968
 *
 *	who	when	vers
 *	BL	0300296	0.1
 ***************************************************************************/

/* base for S3_OUTW macro  */
#define S3_NEWMMIO_REGBASE	0x1000000  /* 16MB */
#define S3_NEWMMIO_REGSIZE	  0x10000  /* 64KB */

#ifdef S3_NEWMMIO
#include <Xmd.h>


#define int16	CARD16
#define int32	CARD32

#define S3_NEWMMIO_VGABASE	(S3_NEWMMIO_REGBASE + 0x8000)

typedef struct { int16 vendor_ID; int16 device_ID; } pci_id;
typedef struct { int16 cmd; int16 devsel; } cmd_devsel;

typedef struct  {
        pci_id  pci_ident;
        cmd_devsel cmd_device_sel; 
        int32 class_code;
        int32 dummy5;
        int32 base0;
        char dummy1[0x20-sizeof(int32)];
        int32 bios_base;
        char dummy2[0x3c - 0x32];
} pci_conf_regs;


typedef struct { int16 filler; int16 adv_f_cntl; } adv_f_cntl;

typedef struct {
        int32	cur_point;
        char    dummy1[4];
        int32	dest_stp;
        char    dummy2[4];
        int32	err_term;
        char    dummy3[4];
        int32	command;
        int32	short_stroke;
        int32   bkgd_color;
        int32   frgd_color;
        int32   wrt_mask;
        int32   rd_mask;
        int32   color_cmp;
        int32	col_mix;
        int32	sciss_topleft;
        int32	sciss_botright;
        int32	pix_mult;
        int32	mult_misc; 
        int32	axis_pcnt;
} packed_enhanced_regs;

typedef struct  {
        int32  prim_stream_cntl;
        int32  col_chroma_key_cntl;
        char    dummy1[0x8190 - 0x8184-sizeof(int32)];
        int32  second_stream_cntl;
        int32  chroma_key_upper_bound;
        int32  second_stream_stretch;
        char dummy2[0x81a0 - 0x8198-sizeof(int32)];
        int32  blend_cntl;
        char    dummy3[0x81c0 - 0x81a0-sizeof(int32)];
        int32  prim_fbaddr0;
        int32  prim_fbaddr1;
        int32  prim_stream_stride;
        int32  double_buffer;
        int32  second_fbaddr0;
        int32  second_fbaddr1;
        int32  second_stream_stride;
        int32  opaq_overlay_cntl;
        int32  k1;
        int32  k2;
        int32  dda_vert;
        int32  streams_fifo;
        int32  prim_start_coord;
        int32  prim_window_size;
        int32  second_start_coord;
        int32  second_window_size;
} streams_proc_regs;

typedef struct { char atr_cntl_ind; char attr_cntl_dat; char misc_out; 
                 char viseo_enable; } v3c0; 
typedef struct { char seq_index; char seq_data; char dac_mask;
                      char dac_rd_index; } v3c4;
typedef struct { char dac_wr_index; char dac_data; char feature_cntl;
                      char filler; } v3c8;
typedef struct v3cc { char misc_out; char filler; char graph_cntl_index;
                      char graph_cntl_data; } v3cc;
typedef struct {
        v3c0    v3c0_regs;
        v3c4    v3c4_regs;
        v3c8    v3c8_regs;
        v3cc    v3cc_regs;
} vga_3c_regs;

typedef struct { char crt_index; char crt_data; int16 filler; } v3d4;
typedef struct { int16 filler1; char feature_cntl; char filler2;} v3d8;

typedef struct  {
        int32   filler;
        v3d4    v3d4_regs;
        v3d8    v3d8_regs;
} vga_3bd_regs ; 

typedef struct {
                int32	subsystem_csr;
		int32	dummy;
                adv_f_cntl adv_func_cntl;
} subsys_regs;


typedef struct  {
        int32	cur_x;
        char    filler1[0x8ae8 - 0x86e8 - sizeof(int32)];
        int32	dy_axstep;
        char    filler2[0x8ee8 - 0x8ae8 - sizeof(int32)];
        int32	dx_diastep;
        char    filler3[0x92e8 - 0x8ee8 - sizeof(int32)];
        int32	line_err;
        char    filler33[0x96e8 - 0x92e8 - sizeof(int32)];
        int32	mj_ax_pcnt;
        char    filler4[0x9ae8 - 0x96e8 - sizeof(int32)];
        int32	gp_stat;
        char    filler5[0x9ee8 - 0x9ae8 - sizeof(int32)];
        int32	stroke_vectrans; 
        char    filler6[0xa2e8 - 0x9ee8 - sizeof(int32)];
        int32  back_color;
        char    filler7[0xa6e8 - 0xa2e8 - sizeof(int32)];
        int32  fore_col;
        char    filler8[0xaae8 - 0xa6e8 - sizeof(int32)];
        int32  bitplane_wmask;
        char    filler88[0xaee8 - 0xaae8 - sizeof(int32)];
        int32  bitplane_rmask;
        char    filler9[0xb2e8 - 0xaee8 - sizeof(int32)];
        int32  color_compare;
        char    filler10[0xb6e8 - 0xb2e8 - sizeof(int32)];
        int32	back_mix;
        char    filler101[0xbae8 - 0xb6e8 - sizeof(int32)];
        int32	fore_mix;
        char    filler11[0xbee8 - 0xbae8 - sizeof(int32)];
        int32	r_reg_data;
        char    filler12[0xe2e8 - 0xbee8 - sizeof(int32)];
        int32	pixel_data_transfer;
} enhanced_regs;

typedef struct {
	int32	lpb_mode;
	int32	lpb_fifostat;
	int32	lpb_intflags;
	int32	lpb_fb0addr;
	int32	lpb_fb1addr;
	int32	lpb_direct_addr;
	int32	lpb_direct_data;
	int32	lpb_gpio;
	int32	lpb_serial_port;
	int32	lpb_input_winsize;
	int32	lpb_data_offsets;
	int32	lpb_hor_decimctl;
	int32	lpb_vert_decimctl;
	int32	lpb_line_stride;
	int32	lpb_output_fifo;
} lpbus_regs; 

typedef struct {
        unsigned char img[0x8000];
        union { pci_conf_regs regs; 
                char dummy[0x100]; 
        } pci_regs;
        union { packed_enhanced_regs regs; 
                char dummy[0x80]; 
        } pk_enh_regs;
        union { streams_proc_regs regs; 
                char dummy[0x82e8-0x8180]; 
        } streams_regs; 
        union { int32	cur_y;
                char dummy[0x83b0 - 0x82e8];
        } cur_y;
        union { vga_3bd_regs    regs;
                char dummy[0x83c0 - 0x83b0];
        } v3b_regs;
        union { vga_3c_regs     regs;
                char dummy[0x83d0 - 0x83c0];
        } v3c_regs;
        union { vga_3bd_regs    regs;
                char dummy[0x8504 - 0x83d0];
        } v3d_regs;
        union { subsys_regs     regs;
                char dummy[0x86e8 - 0x8504];
        } subs_regs;
        union { enhanced_regs   regs;
                char dummy[0xff00 - 0x86e8];
        } enh_regs;
        union { lpbus_regs   regs;
                char dummy[0xff5c - 0xff00];
        } lbp_regs;
} mm_trio_regs ; 

#define mmtr	volatile mm_trio_regs *


#define SET_WRT_MASK(msk)	((mmtr)s3MmioMem)->pk_enh_regs.regs.wrt_mask = (msk)
#define SET_RD_MASK(msk)	((mmtr)s3MmioMem)->pk_enh_regs.regs.rd_mask  = (msk)
#define SET_FRGD_COLOR(col)	((mmtr)s3MmioMem)->pk_enh_regs.regs.frgd_color = (col)
#define SET_BKGD_COLOR(col)	((mmtr)s3MmioMem)->pk_enh_regs.regs.bkgd_color = (col)
#define SET_FRGD_MIX(fmix)	((mmtr)s3MmioMem)->enh_regs.regs.fore_mix = (fmix)
#define SET_BKGD_MIX(bmix)	((mmtr)s3MmioMem)->enh_regs.regs.back_mix = (bmix)
#define SET_PIX_CNTL(val)	((mmtr)s3MmioMem)->pk_enh_regs.regs.pix_mult = (val) | (MULT_MISC2 << 16)
#define SET_MIN_AXIS_PCNT(min)	((mmtr)s3MmioMem)->enh_regs.regs.r_reg_data = (min) & 0xffff
#define SET_MAJ_AXIS_PCNT(maj)	((mmtr)s3MmioMem)->enh_regs.regs.mj_ax_pcnt = (maj)
#define SET_CURPT(c_x, c_y)	((mmtr)s3MmioMem)->pk_enh_regs.regs.cur_point = ((c_y)&0xffff) | ((c_x) << 16)
#define SET_CUR_X(c_x)		((mmtr)s3MmioMem)->enh_regs.regs.cur_x = (c_x)
#define SET_CUR_Y(c_y)		((mmtr)s3MmioMem)->cur_y.cur_y = (c_y)
#define SET_DESTSTP(x,y)	((mmtr)s3MmioMem)->pk_enh_regs.regs.dest_stp = ((y)&0xffff) | ((x) << 16)
#define SET_AXIS_PCNT(maj, min)	((mmtr)s3MmioMem)->pk_enh_regs.regs.axis_pcnt = ((min)&0xffff) | ((maj) << 16)
#define SET_CMD(c_d) 		{ mem_barrier(); ((mmtr)s3MmioMem)->pk_enh_regs.regs.command = (c_d); }
#define SET_ERR_TERM(e)		((mmtr)s3MmioMem)->pk_enh_regs.regs.err_term = (e)
#define SET_SCISSORS(x1,y1,x2,y2) {\
				((mmtr)s3MmioMem)->pk_enh_regs.regs.sciss_topleft  = ((y1)&0xffff) | ((x1) << 16);\
				((mmtr)s3MmioMem)->pk_enh_regs.regs.sciss_botright = ((y2)&0xffff) | ((x2) << 16);\
				}
#define SET_SCISSORS_RB(x,y)	((mmtr)s3MmioMem)->pk_enh_regs.regs.sciss_botright = ((y)&0xffff) | ((x) << 16)
#define SET_MULT_MISC(val)	((mmtr)s3MmioMem)->pk_enh_regs.regs.mult_misc = (val)

/*
 * reads from GP_STAT
 */
#if !defined(__alpha__)
#define INB_GP_STAT() 	((((mmtr)s3MmioMem)->enh_regs.regs.gp_stat) & 0xff)
#define INW_GP_STAT() 	((((mmtr)s3MmioMem)->enh_regs.regs.gp_stat))
#else
#define INB_GP_STAT() 	inb(GP_STAT)
#define INW_GP_STAT() 	inw(GP_STAT)
#endif
	
#else /* S3_NEWMMIO */

#define SET_FRGD_MIX(fmix)	S3_OUTW(FRGD_MIX, (fmix))
#define SET_BKGD_MIX(bmix)	S3_OUTW(BKGD_MIX, (bmix))
#define SET_PIX_CNTL(val) 	S3_OUTW(MULTIFUNC_CNTL, PIX_CNTL| (val))

#define SET_WRT_MASK(msk) 	S3_OUTW32(WRT_MASK,msk) 
#define SET_RD_MASK(msk) 	S3_OUTW32(RD_MASK,msk) 
#define SET_FRGD_COLOR(col)  	S3_OUTW32(FRGD_COLOR, col)
#define SET_BKGD_COLOR(col)  	S3_OUTW32(BKGD_COLOR, col)

#define SET_CUR_X(cur_x) 	S3_OUTW(CUR_X, cur_x)
#define SET_CUR_Y(cur_y) 	S3_OUTW(CUR_Y, cur_y)
#define SET_CURPT(cur_x, cur_y) { SET_CUR_X(cur_x); SET_CUR_Y(cur_y); }

#define SET_DESTSTP(x,y) { \
	S3_OUTW(DESTX_DIASTP, x);\
	S3_OUTW(DESTY_AXSTP, y);\
}
#define SET_AXIS_PCNT(maj, min)	{\
	S3_OUTW(MAJ_AXIS_PCNT, maj);\
	S3_OUTW(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (min));\
}
#define SET_MAJ_AXIS_PCNT(maj)	S3_OUTW(MAJ_AXIS_PCNT, maj)
#define SET_MIN_AXIS_PCNT(min) 	S3_OUTW(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (min))
#define SET_CMD(cmd)		S3_OUTW(CMD, cmd)

#define SET_SCISSORS_RB(x,y) {\
	S3_OUTW(MULTIFUNC_CNTL, SCISSORS_R | (x)); \
	S3_OUTW(MULTIFUNC_CNTL, SCISSORS_B | (y)); \
}
#define SET_MULT_MISC(val) 	S3_OUTW(MULTIFUNC_CNTL, MULT_MISC | (val))

#define SET_SCISSORS(x1,y1,x2,y2) {\
   S3_OUTW(MULTIFUNC_CNTL, SCISSORS_T | (y1));\
   S3_OUTW(MULTIFUNC_CNTL, SCISSORS_L | (x1));\
   S3_OUTW(MULTIFUNC_CNTL, SCISSORS_R | (x2));\
   S3_OUTW(MULTIFUNC_CNTL, SCISSORS_B | (y2));\
}
#define SET_ERR_TERM(err)	S3_OUTW(ERR_TERM, err)

#define INB_GP_STAT() 		inb(GP_STAT)
#define INW_GP_STAT() 		inw(GP_STAT)

#endif	/* S3_NEWMMIO */

#define SET_PIX_TRANS_W(val)	outw(PIX_TRANS, val)
#define SET_PIX_TRANS_L(val)	outl(PIX_TRANS, val)
#define SET_MIX(fmix,bmix) { SET_FRGD_MIX(fmix); SET_BKGD_MIX(bmix); }
#define SET_DAC_W_INDEX(index) outb(DAC_W_INDEX, index)
#define SET_DAC_DATA(val) 	outb(DAC_DATA,val)
