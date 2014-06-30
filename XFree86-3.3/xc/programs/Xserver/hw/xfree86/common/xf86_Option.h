/* $XFree86: xc/programs/Xserver/hw/xfree86/common/xf86_Option.h,v 3.65.2.6 1997/05/21 15:02:35 dawes Exp $ */
/*
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of David Wexelblat not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  David Wexelblat makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL DAVID WEXELBLAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: xf86_Option.h /main/26 1996/10/27 18:07:36 kaleb $ */

#ifndef _XF86_OPTION_H
#define _XF86_OPTION_H

/*
 * Structures and macros for handling option flags.
 *
 * MAX_OFLAGS should be a multiple of FLAGBITS
 */
#define MAX_OFLAGS	256
#define FLAGBITS	(8 * sizeof(CARD32))
typedef struct {
	CARD32 flag_bits[MAX_OFLAGS/FLAGBITS];
} OFlagSet;

#define OFLG_SET(f,p)	((p)->flag_bits[(f)/FLAGBITS] |= (1 << ((f)%FLAGBITS)))
#define OFLG_CLR(f,p)	((p)->flag_bits[(f)/FLAGBITS] &= ~(1 << ((f)%FLAGBITS)))
#define OFLG_ISSET(f,p)	((p)->flag_bits[(f)/FLAGBITS] & (1 << ((f)%FLAGBITS)))
#define OFLG_ZERO(p)	memset((char *)(p), 0, sizeof(*(p)))

/*
 * Option flags.  Define these in numeric order.
 */
/* SVGA clock-related options */
#define OPTION_LEGEND		 0  /* Legend board with 32 clocks */
#define OPTION_SWAP_HIBIT	 1  /* WD90Cxx-swap high-order clock sel bit */
#define OPTION_8CLKS		 2  /* Probe 8 clocks instead of 4 (PVGA1) */
#define OPTION_16CLKS		 3  /* probe for 16 clocks instead of 8 */
#define OPTION_PROBE_CLKS	 4  /* Force clock probe for cards where a
				       set of preset clocks is used */
#define OPTION_HIBIT_HIGH	 5  /* Initial state of high order clock bit */
#define OPTION_HIBIT_LOW	 6
#define OPTION_CLKDIV2		 7  /* allow using clocks divided by 2 
				       in addition to bare clocks */
#define OPTION_HW_CLKS		 8  /* (ct) Hardware clocks */
#define OPTION_FORCE_VCLK1	 9  /* (ct) Use VClk1 as programmable clock */

/* Laptop display options */
#define OPTION_INTERN_DISP	15  /* Laptops - enable internal display (WD)*/
#define OPTION_EXTERN_DISP	16  /* Laptops - enable external display (WD)*/
#define OPTION_CLGD6225_LCD	17  /* Option to avoid setting the DAC to */
				   /* white on a clgd6225 with the LCD */
				   /* enabled */
#define OPTION_STN              18  /* DSTN option (CT)*/
#define OPTION_EXT_FRAM_BUF     19 /* external frame accelerator (CT) */
#define OPTION_LCD_STRETCH      20 /* disable LCD stretching */
#define OPTION_LCD_CENTER	21 /* enable LCD centering */
#define OPTION_PANEL_SIZE	22 /* (CT) Fix wrong panel size set in registers */

/* Memory options */
#define OPTION_FAST_DRAM	30 /* fast DRAM (for ET4000, S3, AGX) */
#define OPTION_MED_DRAM		31 /* medium speed DRAM (for S3, AGX) */
#define OPTION_SLOW_DRAM	32 /* slow DRAM (for Cirrus, S3, AGX) */
#define OPTION_NO_MEM_ACCESS	33 /* Unable to access video ram directly */
#define OPTION_NOLINEAR_MODE	34 /* chipset has broken linear access mode */
#define OPTION_INTEL_GX		35 /* Linear fb on an Intel GX/Pro (Mach32) */
#define OPTION_NO_2MB_BANKSEL	36 /* For cirrus cards with 512kx8 memory */
#define OPTION_FIFO_CONSERV	37 /* (cirrus) (agx) */
#define OPTION_FIFO_AGGRESSIVE	38 /* (cirrus) (agx) */
#define OPTION_MMIO		39 /* Use MMIO for Cirrus 543x */
#define OPTION_LINEAR		40 /* Use linear fb for Cirrus */
#define OPTION_FIFO_MODERATE  	41 /* (agx) */
#define OPTION_SLOW_VRAM	42 /* (s3) */
#define OPTION_SLOW_DRAM_REFRESH 43 /* (s3) */
#define OPTION_FAST_VRAM	44 /* (s3) */
#define OPTION_PCI_BURST_ON	45 /* W32/SVGA */
#define OPTION_PCI_BURST_OFF	46 /* W32/SVGA */
#define OPTION_W32_INTERLEAVE_ON  47 /* W32/SVGA */
#define OPTION_W32_INTERLEAVE_OFF 48 /* W32/SVGA */
#define OPTION_SLOW_EDODRAM	49 /* slow EDO-DRAM (for S3) */
#define OPTION_EARLY_RAS_PRECHARGE	50 /* shift RAS prechange signal (for S3) */
#define OPTION_LATE_RAS_PRECHARGE	51 /* shift RAS prechange signal (for S3) */

/* Accel/cursor features */
#define OPTION_NOACCEL		60 /* Disable accel support in SVGA server */
#define OPTION_HW_CURSOR	61 /* Turn on HW cursor */
#define OPTION_SW_CURSOR	62 /* Turn off HW cursor (Mach32) */
#define OPTION_NO_BITBLT	63 /* Disable hardware bitblt (cirrus) */
#define OPTION_FAVOUR_BITBLT	64 /* Favour use of BitBLT (cirrus) */
#define OPTION_NO_IMAGEBLT	65 /* Avoid system-to-video BitBLT (cirrus) */
#define OPTION_NO_FONT_CACHE	66 /* Don't enable the font cache */
#define OPTION_NO_PIXMAP_CACHE	67 /* Don't enable the pixmap cache */
#define OPTION_TRIO32_FC_BUG	68 /* Workaround Trio32 font cache bug */
#define OPTION_S3_968_DASH_BUG	69 /* Workaround S3 968 dashed line bug */

/* RAMDAC options */
#define OPTION_BT485_CURS	80 /* Override Bt485 RAMDAC probe */
#define OPTION_TI3020_CURS	81 /* Use 3020 RAMDAC cursor (default) */
#define OPTION_NO_TI3020_CURS	82 /* Override 3020 RAMDAC cursor use */
#define OPTION_DAC_8_BIT	83 /* 8-bit DAC operation */
#define OPTION_SYNC_ON_GREEN	84 /* Set Sync-On-Green in RAMDAC */
#define OPTION_BT482_CURS       85 /* Use Bt482 RAMDAC cursor */
#define OPTION_S3_964_BT485_VCLK	86 /* probe/invert VCLK for 964 + Bt485 */
#define OPTION_TI3026_CURS	87 /* Use 3026 RAMDAC cursor (default) */
#define OPTION_IBMRGB_CURS	88 /* Use IBM RGB52x RAMDAC cursor (default) */
#define OPTION_DAC_6_BIT	89 /* 6-bit DAC operation */

/* Vendor specific options */
#define OPTION_SPEA_MERCURY	100 /* pixmux for SPEA Mercury (S3) */
#define OPTION_NUMBER_NINE	101 /* pixmux for #9 with Bt485 (S3) */
#define OPTION_STB_PEGASUS	102 /* pixmux for STB Pegasus (S3) */
#define OPTION_ELSA_W1000PRO	103 /* pixmux for ELSA Winner 1000PRO (S3) */
#define OPTION_ELSA_W2000PRO	104 /* pixmux for ELSA Winner 2000PRO (S3) */
#define OPTION_DIAMOND		105 /* Diamond boards (S3) */
#define OPTION_GENOA		106 /* Genoa boards (S3) */
#define OPTION_STB		107 /* STB boards (S3) */
#define OPTION_HERCULES		108 /* Hercules boards (S3) */
#define OPTION_MIRO_MAGIC_S4	109 /* miroMagic S4 with (S3) 928 and BT485 */
#define OPTION_ELSA_W2000PRO_X8	110 /* clock/phase_detect for ELSA Winner 2000PRO/X-8 (S3) */
#define OPTION_MIRO_80SV	111 /* clock/phase_detect for MIRO 80SV (S3) */

/* Misc options */
#define OPTION_CSYNC		120 /* Composite sync */
#define OPTION_SECONDARY	121 /* Use secondary address (HGC1280) */
#define OPTION_PCI_HACK		122 /* (S3) */
#define OPTION_POWER_SAVER	123 /* Enable DPMS Power Saving */
#define OPTION_OVERRIDE_BIOS	124 /* Override BIOS for Mach64 */
#define OPTION_NO_BLOCK_WRITE	125 /* No block write mode for Mach64 */
#define OPTION_BLOCK_WRITE	126 /* Block write mode for Mach64 */
#define OPTION_NO_BIOS_CLOCKS	127 /* Override BIOS clocks for Mach64 */
#define OPTION_S3_INVERT_VCLK	128 /* invert VLCK (CR67:0) (S3) */
#define OPTION_NO_PROGRAM_CLOCKS 129 /* Turn off clock programming */
#define OPTION_NO_PCI_PROBE	130 /* Disable PCI probe (VGA) */
#define OPTION_TRIO64VP_BUG1	131 /* Trio64V+ bug hack #1 */
#define OPTION_TRIO64VP_BUG2	132 /* Trio64V+ bug hack #2 */
#define OPTION_TRIO64VP_BUG3	133 /* Trio64V+ bug hack #3 */
#define OPTION_USE_MODELINE	134 /* use modeline for LCD instead of preset (ct)*/
#define OPTION_SUSPEND_HACK	135 /* (CT) Use different suspend/resume scheme */
#define OPTION_18_BIT_BUS	136 /* (CT) Use 18bit TFT bus for 24bpp mode */
#define OPTION_PCI_RETRY	137 /* Use PCI-retry instead of busy-waiting */
#define OPTION_NO_PCI_DISC	138 /* Disable PCI disconnect (S3) */

/* Debugging options */
#define OPTION_SHOWCACHE	150 /* Allow cache to be seen (S3) */
#define OPTION_FB_DEBUG		151 /* Linear fb debug for S3 */

/* Some AGX Tuning/Debugging options -- several are for development testing */
#define OPTION_8_BIT_BUS        160 /* Force 8-bit CPU interface - MR1:0 */
#define OPTION_WAIT_STATE       161 /* Force 1 bus wait state - MR1:1<-1 */
#define OPTION_NO_WAIT_STATE    162 /* Force no bus wait state - MR:1<-0 */
#define OPTION_CRTC_DELAY       163 /* Select XGA Mode Delay - MR1:3 */
#define OPTION_VRAM_128         164 /* VRAM shift every 128 cycles - MR2:0 */
#define OPTION_VRAM_256         165 /* VRAM shift every 256 cycles - MR2:0 */
#define OPTION_REFRESH_20       166 /* # clocks between scr refreshs - MR3:5 */
#define OPTION_REFRESH_25       167 /* # clocks between scr refreshs - MR3:5 */
#define OPTION_VLB_A            168 /* VESA VLB transaction type A   - MR7:2 */
#define OPTION_VLB_B            169 /* VESA VLB transaction type B   - MR7:2 */
#define OPTION_SPRITE_REFRESH   170 /* Sprite refresh every hsync    - MR8:4 */
#define OPTION_SCREEN_REFRESH   171 /* Screen refresh during blank   - MR8:5 */
#define OPTION_VRAM_DELAY_LATCH	172 /* Delay Latch                   - MR7:3 */
#define OPTION_VRAM_DELAY_RAS	173 /* Delay RAS signal              - MR7:4 */
#define OPTION_VRAM_EXTEND_RAS  174 /* Extend the RAS signal         - MR8:6 */
#define OPTION_ENGINE_DELAY     175 /* Wait state for some VLB's     - MR5:3 */

/* Some options for oti087, debugging and fine tunning */
#define OPTION_CLOCK_50         180
#define OPTION_CLOCK_66         181
#define OPTION_NO_WAIT          182
#define OPTION_FIRST_WWAIT      183
#define OPTION_WRITE_WAIT       184
#define OPTION_ONE_WAIT         185
#define OPTION_READ_WAIT        186
#define OPTION_ALL_WAIT         187
#define OPTION_ENABLE_BITBLT    188


/* #ifdef PC98 */
#define OPTION_PCSKB		 190 /* SELECT EPSON PCSKB for S3 Server */
#define OPTION_PCSKB4		 191 /* SELECT EPSON PCSKB for S3 Server */
#define OPTION_PCHKB		 192 /* SELECT EPSON PCHKB for S3 Server */
#define OPTION_NECWAB		 193 /* SELECT NEC WAB-A/B for S3 Server */
#define OPTION_PW805I		 194 /* SELECT Canopus PW805i for S3 Server */
#define OPTION_PWLB		 195 /* SELECT Canopus PW_LB for S3 Server */
#define OPTION_PW968		 196 /* SELECT Canopus PW968 for S3 Server */
#define OPTION_GA98NB1           190 /* SELECT IO DATA GA-98NB1 for SVGA */
#define OPTION_GA98NB2           201 /* SELECT IO DATA GA-98NB2 for SVGA */
#define OPTION_GA98NB4           202 /* SELECT IO DATA GA-98NB4 for SVGA */
#define OPTION_WAP               203 /* SELECT MELCO WAP-2000/4000 for SVGA */
#define OPTION_NEC_CIRRUS        204 /* SELECT NEC Internal Server for SVGA */
#define OPTION_EPSON_MEM_WIN	 205 /* ENABLE mem-window 0xF00000 for EPSON */
#define OPTION_PW_MUX            206 /* ENABLE MUX on PW928II */
#define OPTION_NOINIT		 207 /* Not Initialize SDAC & VGA Registers */
#define OPTION_PC98TGUI		 208 /* SELECT NEC TGUI9660 */
/* #endif */

#define OPTION_TGUI_PCI_READ_ON  211 /* Trident TGUI PCI burst read */
#define OPTION_TGUI_PCI_WRITE_ON 212 /* Trident TGUI PCI burst write */
#define OPTION_TGUI_MCLK_66	 213 /* Run the TGUI at 66MHz MCLK */

/* more Memory options */
#define OPTION_FPM_VRAM		220 /* (s3v) */
#define OPTION_EDO_VRAM		221 /* (s3v) */
#define OPTION_NO_MMIO		222 /* Disable MMIO (Cirrus 543x/4x) */

/* XAA options */
#define OPTION_XAA_BENCHMARK	230 /* Perform start-up benchmarks */
#define OPTION_XAA_NO_COL_EXP	231 /* Disable color expansion. */

/*
 *  MAX flag value is 256.  If larger is needed, remember to update
 *  MAX_OFLAGS at the top of this file.
 */


#define CLOCK_OPTION_PROGRAMABLE 0 /* has a programable clock */
#define CLOCK_OPTION_ICD2061A	 1 /* use ICD 2061A programable clocks      */
#define CLOCK_OPTION_SC11412     3 /* use SC11412 programmable clocks */
#define CLOCK_OPTION_S3GENDAC    4 /* use S3 Gendac programmable clocks */
#define CLOCK_OPTION_TI3025      5 /* use TI3025 programmable clocks */
#define CLOCK_OPTION_ICS2595     6 /* use ICS2595 programmable clocks */
#define CLOCK_OPTION_CIRRUS      7 /* use Cirrus programmable clocks */
#define CLOCK_OPTION_CH8391      8 /* use Chrontel 8391 programmable clocks */
#define CLOCK_OPTION_ICS5342     9 /* use ICS 5342 programmable clocks */
#define CLOCK_OPTION_S3TRIO     10 /* use S3 Trio32/64 programmable clocks */
#define CLOCK_OPTION_TI3026     11 /* use TI3026 programmable clocks */
#define CLOCK_OPTION_IBMRGB     12 /* use IBM RGB52x programmable clocks */
#define CLOCK_OPTION_STG1703    13 /* use STG1703 programmable clocks */
#define CLOCK_OPTION_ICS5341    14 /* use ICS 5341 (ET4000W32p) */
#define CLOCK_OPTION_TRIDENT    15 /* use programmable clock on TGUI */
#define CLOCK_OPTION_ATT409     16 /* use ATT20C409 programmable clock */
#define CLOCK_OPTION_CH8398     17 /* use Chrontel 8398 programmable clock */
#define CLOCK_OPTION_GLORIA8    18 /* use ELSA Gloria-8 TVP3030/ICS9161 clock */
#define CLOCK_OPTION_ET6000     19 /* use ET6000 built-in programmable clock */
#define CLOCK_OPTION_ICS1562    20 /* used for TGA server */
#define CLOCK_OPTION_S3AURORA   21 /* use S3 Aurora64V+ programmable clocks */
#define CLOCK_OPTION_S3TRIO64V2 22 /* use S3 Trio64V2 or ViRGE/DX/GX 170MHz clocks */

/*
 * Table to map option strings to tokens.
 */
typedef struct {
  char *name;
  int  token;
} OptFlagRec, *OptFlagPtr;

#ifdef INIT_OPTIONS
OptFlagRec xf86_OptionTab[] = {
  { "legend",		OPTION_LEGEND },
  { "swap_hibit",	OPTION_SWAP_HIBIT },
  { "8clocks",		OPTION_8CLKS },
  { "16clocks",		OPTION_16CLKS },
  { "probe_clocks",	OPTION_PROBE_CLKS },
  { "hibit_high",	OPTION_HIBIT_HIGH },
  { "hibit_low",	OPTION_HIBIT_LOW },
  { "clkdiv2",		OPTION_CLKDIV2 },
  { "hw_clocks",        OPTION_HW_CLKS },
  { "use_vclk1",        OPTION_FORCE_VCLK1 },

  { "intern_disp",	OPTION_INTERN_DISP },
  { "extern_disp",	OPTION_EXTERN_DISP },
  { "clgd6225_lcd",	OPTION_CLGD6225_LCD },
  { "stn",              OPTION_STN},
  { "ext_fram_buf",	OPTION_EXT_FRAM_BUF },
  { "no_stretch",	OPTION_LCD_STRETCH },
  { "lcd_center",	OPTION_LCD_CENTER },
  { "lcd_centre",	OPTION_LCD_CENTER },
  { "fix_panel_size",	OPTION_PANEL_SIZE },

  { "fast_dram",	OPTION_FAST_DRAM },
  { "med_dram",		OPTION_MED_DRAM },
  { "slow_dram",	OPTION_SLOW_DRAM },
  { "slow_edodram",	OPTION_SLOW_EDODRAM },
  { "early_ras_precharge",	OPTION_EARLY_RAS_PRECHARGE },
  { "late_ras_precharge",	OPTION_LATE_RAS_PRECHARGE },
  { "nomemaccess",	OPTION_NO_MEM_ACCESS },
  { "nolinear",		OPTION_NOLINEAR_MODE },
  { "intel_gx",		OPTION_INTEL_GX },
  { "no_2mb_banksel",	OPTION_NO_2MB_BANKSEL },
  { "fifo_conservative",OPTION_FIFO_CONSERV },
  { "fifo_moderate",    OPTION_FIFO_MODERATE },
  { "fifo_aggressive",	OPTION_FIFO_AGGRESSIVE },
  { "mmio",		OPTION_MMIO },
  { "no_mmio",		OPTION_NO_MMIO },
  { "linear",		OPTION_LINEAR },
  { "slow_vram",	OPTION_SLOW_VRAM },
  { "s3_slow_vram",	OPTION_SLOW_VRAM },
  { "slow_dram_refresh",OPTION_SLOW_DRAM_REFRESH },
  { "s3_slow_dram_refresh",OPTION_SLOW_DRAM_REFRESH },
  { "fast_vram",	OPTION_FAST_VRAM },
  { "s3_fast_vram",	OPTION_FAST_VRAM },
  { "fpm_vram",		OPTION_FPM_VRAM },
  { "edo_vram",		OPTION_EDO_VRAM },
  { "pci_burst_on",	OPTION_PCI_BURST_ON },
  { "pci_burst_off",	OPTION_PCI_BURST_OFF },
  { "w32_interleave_on",OPTION_W32_INTERLEAVE_ON },
  { "w32_interleave_off",OPTION_W32_INTERLEAVE_OFF },
  { "tgui_pci_read_on", OPTION_TGUI_PCI_READ_ON },
  { "tgui_pci_write_on",OPTION_TGUI_PCI_WRITE_ON },
  { "tgui_mclk_66",	OPTION_TGUI_MCLK_66 },

  { "noaccel",		OPTION_NOACCEL },
  { "hw_cursor",	OPTION_HW_CURSOR },
  { "sw_cursor",	OPTION_SW_CURSOR },
  { "no_bitblt",	OPTION_NO_BITBLT },
  { "favour_bitblt",	OPTION_FAVOUR_BITBLT },
  { "favor_bitblt",	OPTION_FAVOUR_BITBLT },
  { "no_imageblt",	OPTION_NO_IMAGEBLT },
  { "no_font_cache",	OPTION_NO_FONT_CACHE },
  { "no_pixmap_cache",	OPTION_NO_PIXMAP_CACHE },
  { "trio32_fc_bug",	OPTION_TRIO32_FC_BUG },
  { "s3_968_dash_bug",	OPTION_S3_968_DASH_BUG },

  { "xaa_benchmark",	OPTION_XAA_BENCHMARK },
  { "xaa_no_color_exp", OPTION_XAA_NO_COL_EXP },

  { "bt485_curs",	OPTION_BT485_CURS },
  { "ti3020_curs",	OPTION_TI3020_CURS },
  { "no_ti3020_curs",	OPTION_NO_TI3020_CURS },
  { "ti3026_curs",	OPTION_TI3026_CURS },
  { "ibmrgb_curs",	OPTION_IBMRGB_CURS },
  { "dac_8_bit",	OPTION_DAC_8_BIT },
  { "sync_on_green",    OPTION_SYNC_ON_GREEN },
  { "bt482_curs",	OPTION_BT482_CURS },
  { "s3_964_bt485_vclk",OPTION_S3_964_BT485_VCLK },
  { "dac_6_bit",	OPTION_DAC_6_BIT },

  { "spea_mercury",	OPTION_SPEA_MERCURY },
  { "number_nine",	OPTION_NUMBER_NINE },
  { "stb_pegasus",	OPTION_STB_PEGASUS },
  { "elsa_w1000pro",	OPTION_ELSA_W1000PRO },
  { "elsa_w1000isa",	OPTION_ELSA_W1000PRO }, /* These are treated the same */
  { "elsa_w2000pro",	OPTION_ELSA_W2000PRO },
  { "elsa_w2000pro/x8",	OPTION_ELSA_W2000PRO_X8 },
  { "miro_80sv",	OPTION_MIRO_80SV },
  { "diamond",		OPTION_DIAMOND },
  { "genoa",		OPTION_GENOA },
  { "stb",		OPTION_STB },
  { "hercules",		OPTION_HERCULES },
  { "miro_magic_s4",	OPTION_MIRO_MAGIC_S4},

  { "composite",	OPTION_CSYNC },
  { "secondary",	OPTION_SECONDARY },
  { "pci_hack",		OPTION_PCI_HACK },
  { "power_saver",	OPTION_POWER_SAVER },
  { "override_bios",	OPTION_OVERRIDE_BIOS },
  { "no_block_write",	OPTION_NO_BLOCK_WRITE },
  { "block_write",	OPTION_BLOCK_WRITE },
  { "no_bios_clocks",	OPTION_NO_BIOS_CLOCKS },
  { "s3_invert_vclk",	OPTION_S3_INVERT_VCLK },
  { "no_program_clocks",OPTION_NO_PROGRAM_CLOCKS },
  { "no_pci_probe",	OPTION_NO_PCI_PROBE },
  { "trio64v+_bug1",	OPTION_TRIO64VP_BUG1 },
  { "trio64v+_bug2",	OPTION_TRIO64VP_BUG2 },
  { "trio64v+_bug3",	OPTION_TRIO64VP_BUG3 },
  { "use_modeline",	OPTION_USE_MODELINE },
  { "suspend_hack",	OPTION_SUSPEND_HACK },
  { "use_18bit_bus",	OPTION_18_BIT_BUS },
  { "pci_retry",	OPTION_PCI_RETRY },
  { "no_pci_disconnect",	OPTION_NO_PCI_DISC },

  { "showcache",	OPTION_SHOWCACHE },
  { "fb_debug",		OPTION_FB_DEBUG },

/* #ifdef PC98 */
  { "pcskb",		OPTION_PCSKB },
  { "pcskb4",		OPTION_PCSKB4 },
  { "pchkb",		OPTION_PCHKB },
  { "necwab",		OPTION_NECWAB },
  { "pw805i",		OPTION_PW805I },
  { "pw_localbus",	OPTION_PWLB },
  { "pw968",		OPTION_PW968 },
  { "ga98nb1",		OPTION_GA98NB1 },
  { "ga98nb2",		OPTION_GA98NB2 },
  { "ga98nb4",		OPTION_GA98NB4 },
  { "wap",		OPTION_WAP },
  { "nec_cirrus",	OPTION_NEC_CIRRUS },
  { "epsonmemwin",	OPTION_EPSON_MEM_WIN },
  { "pw_mux",		OPTION_PW_MUX },
  { "noinit",		OPTION_NOINIT },
  { "pc98_tgui",	OPTION_PC98TGUI },
/* #endif */

  { "8_bit_bus",        OPTION_8_BIT_BUS },
  { "wait_state",       OPTION_WAIT_STATE },
  { "no_wait_state",    OPTION_NO_WAIT_STATE },
  { "crtc_delay",       OPTION_CRTC_DELAY },
  { "vram_128",         OPTION_VRAM_128 },
  { "vram_256",         OPTION_VRAM_256 },
  { "refresh_20",       OPTION_REFRESH_20 },
  { "refresh_25",       OPTION_REFRESH_25 },
  { "vlb_a",            OPTION_VLB_A },
  { "vlb_b",            OPTION_VLB_B },
  { "sprite_refresh",   OPTION_SPRITE_REFRESH },
  { "screen_refresh",   OPTION_SPRITE_REFRESH },
  { "vram_delay_latch", OPTION_VRAM_DELAY_LATCH },
  { "vram_delay_ras",   OPTION_VRAM_DELAY_RAS },
  { "vram_extend_ras",  OPTION_VRAM_EXTEND_RAS },
  { "engine_delay",     OPTION_ENGINE_DELAY },
  { "clock_50",         OPTION_CLOCK_50 },
  { "clock_66",         OPTION_CLOCK_66 },
  { "no_wait",          OPTION_NO_WAIT },
  { "first_wwait",      OPTION_FIRST_WWAIT },
  { "write_wait",       OPTION_WRITE_WAIT },
  { "one_wait",         OPTION_ONE_WAIT },
  { "read_wait",        OPTION_READ_WAIT },
  { "all_wait",         OPTION_ALL_WAIT },
  { "enable_bitblt",    OPTION_ENABLE_BITBLT },

  { "",			-1 },
};

OptFlagRec xf86_ClockOptionTab [] = {
  { "icd2061a",		CLOCK_OPTION_ICD2061A },  /* generic ICD2061A */
  { "ics9161a",		CLOCK_OPTION_ICD2061A },  /* ICD2061A compatible */
  { "dcs2824",		CLOCK_OPTION_ICD2061A },  /* ICD2061A compatible */
  { "sc11412", 		CLOCK_OPTION_SC11412 },   /* Sierra SC11412 */
  { "s3gendac",		CLOCK_OPTION_S3GENDAC },  /* S3 gendac */
  { "s3_sdac",		CLOCK_OPTION_S3GENDAC },  /* S3 SDAC */
  { "ics5300",		CLOCK_OPTION_S3GENDAC },  /* S3 gendac compatible */
  { "ics5342",		CLOCK_OPTION_ICS5342 },   /* not completely S3 SDAC compatible */
  { "s3_trio",		CLOCK_OPTION_S3TRIO },    /* S3 Trio32/64 */
  { "s3_trio32",	CLOCK_OPTION_S3TRIO },    /* S3 Trio32/64 */
  { "s3_trio64",	CLOCK_OPTION_S3TRIO },    /* S3 Trio32/64 */
  { "s3_aurora64",	CLOCK_OPTION_S3AURORA },  /* S3 Aurora64V+ */
  { "s3_trio64v2",	CLOCK_OPTION_S3TRIO64V2 },/* S3 Trio64V2 and ViRGE/DX/GX */
  { "ti3025",		CLOCK_OPTION_TI3025 },    /* TI3025 */
  { "ti3026",		CLOCK_OPTION_TI3026 },    /* TI3026 */
  { "ti3030",		CLOCK_OPTION_TI3026 },    /* TI3030 is TI3026 compatible */
  { "ibm_rgb514",	CLOCK_OPTION_IBMRGB },    /* IBM RGB514 */
  { "ibm_rgb524",	CLOCK_OPTION_IBMRGB },    /* IBM RGB524 */
  { "ibm_rgb525",	CLOCK_OPTION_IBMRGB },    /* IBM RGB525 */
  { "ibm_rgb528",	CLOCK_OPTION_IBMRGB },    /* IBM RGB528 */
  { "ibm_rgb51x",	CLOCK_OPTION_IBMRGB },    /* IBM RGB51x */
  { "ibm_rgb52x",	CLOCK_OPTION_IBMRGB },    /* IBM RGB52x */
  { "ibm_rgb5xx",	CLOCK_OPTION_IBMRGB },    /* IBM RGB52x */
  { "ics2595",		CLOCK_OPTION_ICS2595 },   /* ICS2595 */
  { "cirrus",		CLOCK_OPTION_CIRRUS }, 	  /* Cirrus built-in */
  { "ch8391",		CLOCK_OPTION_CH8391 }, 	  /* Chrontel 8391  */
  { "stg1703",		CLOCK_OPTION_STG1703 },   /* STG1703 */
  { "ics5341",		CLOCK_OPTION_ICS5341 },   /* ET4000 W32p version of S3 SDAC/ICS5341 */
  { "tgui",		CLOCK_OPTION_TRIDENT },   /* Trident TGUI built-in */
  { "att20c409",	CLOCK_OPTION_ATT409 },    /* ATT20C409 */
  { "att20c499",	CLOCK_OPTION_ATT409 },    /* ATT20C499, 409 compatible */
  { "att20c408",	CLOCK_OPTION_ATT409 },    /* ATT20C408, 409 compatible */
  { "ch8398",		CLOCK_OPTION_CH8398 },    /* Chrontel 8398 */
  { "ati18818",		CLOCK_OPTION_ICS2595 },   /* ATI18818, ICS2595 compatible */
  { "et6000",		CLOCK_OPTION_ET6000 },    /* ET6000 */
  { "",			-1 },
};

#else
extern OptFlagRec xf86_OptionTab[];
extern OptFlagRec xf86_ClockOptionTab[];
#endif

#endif /* _XF86_OPTION_H */

