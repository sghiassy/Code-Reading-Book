/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3ELSA.h,v 3.11 1997/01/14 22:16:54 dawes Exp $ */






/* $XConsortium: s3ELSA.h /main/8 1996/04/18 16:45:18 kaleb $ */
#ifndef _S3ELSA_H_
#define _S3ELSA_H_

#define  ELSA_WINNER_1000	0x910   
#define  ELSA_WINNER_1000VL	0x912   
#define  ELSA_WINNER_1000PCI	0x914   
#define  ELSA_WINNER_1000ISA	0x91A   
#define  ELSA_WINNER_2000	0x920   
#define  ELSA_WINNER_2000VL	0x922   
#define  ELSA_WINNER_2000PCI	0x924   
#define  ELSA_WINNER_1000PRO	0x930   
#define  ELSA_WINNER_1000PRO_TRIO32 0x931
#define  ELSA_WINNER_1000PRO_TRIO64 0x932
#define  ELSA_WINNER_1000AVI	0x936
#define  ELSA_WINNER_1000PRO_X	0x937
#define  ELSA_WINNER_2000PRO	0x940   
#define  ELSA_WINNER_2000PRO_X	0x942
#define  ELSA_WINNER_2000AVI	0x943
#define  ELSA_WINNER_2000PRO_X8	0x94a
#define  ELSA_GLORIA_4		0x980
#define  ELSA_GLORIA_8		0x981

typedef struct {
   unsigned short code;
   char *name;
   long *flags;
} elsa_board_types_t;

extern elsa_board_types_t elsa_board_types[];

/* structure to hold timing/mode info read from EEROM
*/

typedef struct {
   unsigned short 
      crc16,
      wnr_type,		/*  0x3353  ("S3")  */
      board_code,
      serno_l,		/*  low word  */
      serno_h,		/*  high word  */
      eeprom_size,	/*  EEROM size in words  */
      hw_conf,		/*  reserved  */
      max_pixclock,	/*  max pixel clock [MHz] / 4000  */
      max_memclock,	/*  max memory clock [MHz] / 4000  */
      vmode0,		/*  reserved  */
      key_word,		/*  reserved  */
      xsize,		/*  monitor size in mm  */
      ysize,   
      sw_vers;		/*  currently 1  */
} elsa_eeprom_data_t;


typedef struct {
   unsigned short 
      bpp_xres4,  /* bpp (6bit) + xres/4 (10bit) */
      fl_yres,    /* flags (4bit) + yres (12bit) */
      pixfrq4,    /* pixel clock frequency in kHz / 4 (16bit) */
      hfph_htot,  /* hor-frontporch-hi4bit + hor-total (12bit) */
      hfpl_hsw,   /* hor-frontporch-lo6bit + hor-syncwidth (10bit) */
      vfph_vtot,  /* ver-frontporch-hi4bit + ver-total (12bit) */
      vfpl_vsw,   /* ver-frontporch-lo6bit + ver-syncwidth (10bit) */
      res1,       /* reserved */
      res2;       /* reserved */
} elsa_eeprom_timing_t;



/* how to make the fields in this struct */

#define ELSA_TIM_bx(_bpp_, _xres_)   (ushort) ((_bpp_) << 10 | (_xres_) / 4)
#define ELSA_TIM_fy(_flags_, _yres_) (ushort) ((_flags_) << 12 | _yres_)
#define ELSA_TIM_fp(_pixfreq_)       (ushort) ((_pixfreq_) / 4)
#define ELSA_TIM_h1(_hfp_, _htot_)   (ushort) (((_hfp_) & 0x3c0) <<  6 | (_htot_))
#define ELSA_TIM_h2(_hfp_, _hsw_)    (ushort) (((_hfp_) & 0x03f) << 10 | (_hsw_))
#define ELSA_TIM_v1(_vfp_, _vtot_)   (ushort) (((_vfp_) & 0x3c0) <<  6 | (_vtot_))
#define ELSA_TIM_v2(_vfp_, _vsw_)    (ushort) (((_vfp_) & 0x03f) << 10 | (_vsw_))


/* how to extract info from this struct */

#define ELSA_TIM_bpp(et)     (((et).bpp_xres4 & 0xfc00) >> 10)
#define ELSA_TIM_xres(et)    (((et).bpp_xres4 & 0x03ff) * 4)
#define ELSA_TIM_flags(et)   (((et).fl_yres & 0xf000) >> 12)
#define ELSA_TIM_yres(et)    ((et).fl_yres & 0x0fff)
#define ELSA_TIM_pixfrq4(et) ((et).pixfrq4)
#define ELSA_TIM_htot(et)    ((et).hfph_htot & 0x0fff)
#define ELSA_TIM_hfp(et)     ((((et).hfph_htot & 0xf000) >> 6) | (((et).hfpl_hsw & 0xfc00) >> 10))
#define ELSA_TIM_hsw(et)     ((et).hfpl_hsw & 0x03ff)
#define ELSA_TIM_vtot(et)    ((et).vfph_vtot & 0x0fff)
#define ELSA_TIM_vfp(et)     ((((et).vfph_vtot & 0xf000) >> 6) | (((et).vfpl_vsw & 0xfc00) >> 10))
#define ELSA_TIM_vsw(et)     ((et).vfpl_vsw & 0x03ff)



/* Macro to check validity of timing data */

#define ELSA_ET_VM_VALID(pet) (                                       \
  (pet)->bpp_xres4 != 0xFFFF && ((pet)->bpp_xres4 & 0x3FF) >= 16/4 && \
  (pet)->fl_yres   != 0xFFFF && ((pet)->fl_yres   & 0xFFF) >= 16   && \
  (pet)->pixfrq4   != 0xFFFF &&  (pet)->pixfrq4 )


_XFUNCPROTOBEGIN

extern int s3DetectELSA(
#if NeedFunctionPrototypes
			int BIOSbase,
			char **pcard, 
			char **pserno,
			int *max_pix_clock, 
			int *max_mem_clock,
			int *hwconf,
			char **modes
#endif
			);

_XFUNCPROTOEND

#endif /* _S3ELSA_H_ */
