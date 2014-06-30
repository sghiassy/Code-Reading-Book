/* $XConsortium: Probe.h /main/27 1996/10/25 21:19:15 kaleb $ */
/*
 * (c) Copyright 1993,1994 by David Wexelblat <dwex@xfree86.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * DAVID WEXELBLAT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Except as contained in this notice, the name of David Wexelblat shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from David Wexelblat.
 *
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/Probe.h,v 3.45.2.4 1997/05/22 14:00:34 dawes Exp $ */

/*
 * Includes
 */

#if defined(__STDC__) && defined(__GNUC__)
#define inline __inline__
#endif
#ifdef _POSIX_SOURCE
#include <sys/types.h>
#endif
#include <stdio.h>
#ifndef	MACH386
#include <unistd.h>
#include <stdlib.h>
#endif	/* MACH386 */
#include <ctype.h>
#if defined(SYSV) || defined(SVR4) || defined(linux)
# include <string.h>
# include <memory.h>
#else
#ifdef _MINIX
# include <string.h>
#else
# include <strings.h>
# define strchr(a,b) 	index((a),(b))
# define strrchr(a,b) 	rindex((a),(b))
#endif /* _MINIX */
#endif

#include "PCI.h"

#ifdef __STDC__
# define CONST const
# define __STDCARGS(a)	a
#else
# define CONST
# define __STDCARGS(a)	()
#endif

/*
 * Types
 */
typedef unsigned char Byte;
typedef unsigned short Word;
typedef unsigned long Long;
typedef int Bool;
#define FALSE 0
#define TRUE 1
#ifndef NULL
# define NULL	(void *)0
#endif


typedef struct {
	Word	lo, hi;
} Range;

typedef Bool (*ProbeFunc) __STDCARGS((int *));
typedef int (*MemCheckFunc) __STDCARGS((int));
typedef void (*SetBank) __STDCARGS((int, int, int));
typedef void (*GetBank) __STDCARGS((int, int *, int *));
typedef struct {
	int		chipset;	/* Chipset for bank operations	*/
	SetBank		set_bank;	/* Set a read/write bank	*/
	GetBank		get_bank;	/* Get the current bank number	*/
	int		bank_size;	/* Size of a memory bank	*/
	int		max_bank;	/* Maximum bank number 		*/
} MemCheck_Descriptor;
typedef struct {
	char		*name;		/* Chipset vendor/class name 	*/
	ProbeFunc	f;		/* Probe function		*/
	Word		*ports;		/* List of ports used by probe	*/
	int		num_ports;	/* Number of ports in list	*/
	Bool		bit16;		/* Does probe use 16-bit ports?	*/
	Bool		uses_bios;	/* Does probe read the BIOS?	*/
	Bool		check_coproc;	/* Check for attached coprocessor */
	MemCheckFunc	memcheck;	/* For doing memory check	*/
} Chip_Descriptor;

/*
 * Prototypes
 */

/* OS_xxxx.c */
int OpenVideo __STDCARGS((void));
void CloseVideo __STDCARGS((void));
Byte *MapVGA __STDCARGS((void));
void UnMapVGA __STDCARGS((Byte *));
Byte *MapMem __STDCARGS((unsigned long, unsigned long));
void UnMapMem __STDCARGS((Byte *, unsigned long));
int ReadBIOS __STDCARGS((const unsigned, Byte *, const int));
int EnableIOPorts __STDCARGS((const int, const Word *));
int DisableIOPorts __STDCARGS((const int, const Word *));
void ShortSleep __STDCARGS((const int Delay));

/* Utils.c */
Byte inp __STDCARGS((Word));
Word inpw __STDCARGS((Word));
Long inpl __STDCARGS((Word));
void outp __STDCARGS((Word, Byte));
void outpw __STDCARGS((Word, Word));
void outpl __STDCARGS((Word, Long));
Byte rdinx __STDCARGS((Word, Byte));
void wrinx __STDCARGS((Word, Byte, Byte));
void wrinx2 __STDCARGS((Word, Byte, Word));
void wrinx3 __STDCARGS((Word, Byte, Long));
void wrinx2m __STDCARGS((Word, Byte, Word));
void wrinx3m __STDCARGS((Word, Byte, Long));
void modinx __STDCARGS((Word, Byte, Byte, Byte));
void modreg __STDCARGS((Word, Byte, Byte));
void setinx __STDCARGS((Word, Byte, Byte));
void clrinx __STDCARGS((Word, Byte, Byte));
Bool tstrg __STDCARGS((Word, Byte));
Bool testinx2 __STDCARGS((Word, Byte, Byte));
Bool testinx __STDCARGS((Word, Byte));
void dactopel __STDCARGS((void));
Byte trigdac __STDCARGS((void));
Byte dactocomm __STDCARGS((void));
Byte getdaccomm __STDCARGS((void));
void waitforretrace __STDCARGS((void));
Bool Excluded __STDCARGS((Range *, Chip_Descriptor *, Bool));
int StrCaseCmp __STDCARGS((char *, char *));
unsigned int StrToUL __STDCARGS((const char *));

/*
 * Ident functions
 */
int Probe_Mem __STDCARGS((MemCheck_Descriptor));
void Probe_RamDac __STDCARGS((int, int *));
Bool Probe_MDA __STDCARGS((int *));
Bool Probe_Herc __STDCARGS((int *));
Bool Probe_CGA __STDCARGS((int *));
Bool Probe_VGA __STDCARGS((int *));
/* SVGA */
Bool Probe_Tseng __STDCARGS((int *));
Bool Probe_WD __STDCARGS((int *));
Bool Probe_CT __STDCARGS((int *));
Bool Probe_Video7 __STDCARGS((int *));
Bool Probe_Genoa __STDCARGS((int *));
Bool Probe_Trident __STDCARGS((int *));
Bool Probe_SiS __STDCARGS((int *));
Bool Probe_Matrox __STDCARGS((int *));
Bool Probe_Oak __STDCARGS((int *));
Bool Probe_Cirrus __STDCARGS((int *));
Bool Probe_Cirrus54 __STDCARGS((int *));
Bool Probe_Cirrus64 __STDCARGS((int *));
Bool Probe_Ahead __STDCARGS((int *));
Bool Probe_ATI __STDCARGS((int *));
Bool Probe_S3 __STDCARGS((int *));
Bool Probe_AL __STDCARGS((int *));
Bool Probe_Alliance __STDCARGS((int *));
Bool Probe_Yamaha __STDCARGS((int *));
Bool Probe_NCR __STDCARGS((int *));
Bool Probe_MX __STDCARGS((int *));
Bool Probe_Primus __STDCARGS((int *));
Bool Probe_RealTek __STDCARGS((int *));
Bool Probe_Compaq __STDCARGS((int *));
Bool Probe_HMC __STDCARGS((int *));
Bool Probe_UMC __STDCARGS((int *));
Bool Probe_Weitek __STDCARGS((int *));
Bool Probe_ARK __STDCARGS((int *));
Bool Probe_SigmaDesigns __STDCARGS((int *));
/* CoProc */
Bool Probe_8514 __STDCARGS((int *));
Bool Probe_ATIMach __STDCARGS((int *));
Bool Probe_I128 __STDCARGS((int *));
Bool Probe_GLINT __STDCARGS((int *));

/*
 * Print functions
 */
void Print_SVGA_Name __STDCARGS((int));
void Print_Herc_Name __STDCARGS((int));
void Print_RamDac_Name __STDCARGS((int));
void Print_CoProc_Name __STDCARGS((int));

/*
 * Globals
 */
extern struct pci_config_reg *pcrp;
extern Bool NoPCI;
extern Bool PCIProbed;
extern char MyName[];
extern Word vgaIOBase;
extern Bool Verbose;
extern Long Chip_data;
extern Byte *Bios_Base;
extern Bool AssumeEGA;

extern Chip_Descriptor VGA_Descriptor;

extern Chip_Descriptor AL_Descriptor;
extern Chip_Descriptor Alliance_Descriptor;
extern Chip_Descriptor ATI_Descriptor;
extern Chip_Descriptor Ahead_Descriptor;
extern Chip_Descriptor CT_Descriptor;
extern Chip_Descriptor Cirrus_Descriptor;
extern Chip_Descriptor Cirrus54_Descriptor;
extern Chip_Descriptor Cirrus64_Descriptor;
extern Chip_Descriptor Compaq_Descriptor;
extern Chip_Descriptor Genoa_Descriptor;
extern Chip_Descriptor HMC_Descriptor;
extern Chip_Descriptor MX_Descriptor;
extern Chip_Descriptor NCR_Descriptor;
extern Chip_Descriptor Oak_Descriptor;
extern Chip_Descriptor Primus_Descriptor;
extern Chip_Descriptor RealTek_Descriptor;
extern Chip_Descriptor S3_Descriptor;
extern Chip_Descriptor Trident_Descriptor;
extern Chip_Descriptor SiS_Descriptor;
extern Chip_Descriptor Matrox_Descriptor;
extern Chip_Descriptor Tseng_Descriptor;
extern Chip_Descriptor UMC_Descriptor;
extern Chip_Descriptor Video7_Descriptor;
extern Chip_Descriptor WD_Descriptor;
extern Chip_Descriptor Weitek_Descriptor;
extern Chip_Descriptor Yamaha_Descriptor;
extern Chip_Descriptor ARK_Descriptor;
extern Chip_Descriptor SigmaDesigns_Descriptor;

extern Chip_Descriptor IBM8514_Descriptor;
extern Chip_Descriptor ATIMach_Descriptor;
extern Chip_Descriptor I128_Descriptor;
extern Chip_Descriptor GLINT_Descriptor;

/*
 * Useful macros
 */
/* VGA */
#define COPYRIGHT_BASE	(0x1E)
#define CRTC_IDX	(vgaIOBase+0x04)
#define CRTC_REG	(vgaIOBase+0x05)
#define GRC_IDX		(0x3CE)
#define GRC_REG		(0x3CF)
#define SEQ_IDX		(0x3C4)
#define SEQ_REG		(0x3C5)
#define ATR_IDX		(0x3C0)
#define ATR_REG_W	(0x3C0)
#define ATR_REG_R	(0x3C1)
#define MISC_OUT_W	(0x3C2)
#define MISC_OUT_R	(0x3CC)
/* 8514 */
#define CONFIG_STATUS_1	0x12EE
#define MISC_OPTIONS	0x36EE
#define CHIP_ID		0xFAEE
#define SUBSYS_CNTL	0x42E8
#define EXT_CONF_3	0x5EE8
#define ERR_TERM	0x92E8
#define GPCTRL_ENAB	0x4000
#define GPCTRL_RESET	0x8000
#define CHPTEST_NORMAL	0x1000
#define ROM_ADDR_1	0x52EE
#define DESTX_DIASTP	0x8EE8
#define READ_SRC_X	0xDAEE
#define GP_STAT		0x9AE8
#define GPBUSY		0x0200

/*
 * RAMDAC Types
 */
#define DAC_UNKNOWN	0	/* Non-standard, but unknown */
#define DAC_STANDARD	1	/* Standard 8-bit pseudo-color DAC */
#define DAC_ALG1101	2	/* Avance Logic ALG1101 */
#define DAC_SS24	3	/* Diamond SS2410 */
#define DAC_SIERRA15	4	/* Sierra 15-bit HiColor */
#define DAC_SIERRA15_16	5	/* Sierra 15/16-bit HiColor */
#define DAC_SIERRA24	6	/* Sierra 15/16/24-bit DAC */
#define DAC_MUSIC4870	7	/* MUSIC 15/16-bit HiColor DAC */
#define DAC_MUSIC4910	8	/* MUSIC 15/16/24-bit DAC */
#define DAC_ACUMOS	9	/* AcuMos ADAC1 15/16/24-bit DAC */
#define DAC_ATI68830	10	/* ATI 68830 15/16-bit DAC */
#define DAC_ATI68860	11	/* ATI 68860 15/16/24-bit DAC, mux */
#define DAC_ATI68875	12	/* ATI 68875 15/16/24-bit DAC, mux */
#define DAC_ATIMISC24	13	/* ATI Miscellaneous 3'rd party DACs */
#define DAC_CIRRUSA	14	/* Cirrus 5420 8-bit pseudo-color DAC */
#define DAC_CIRRUSB	15	/* Cirrus 542{2,4,6,8} 15/16/24-bit DAC */
#define DAC_CIRRUS24	16	/* Cirrus 15/16/24-bit separate DAC */
#define DAC_ATT490	17	/* AT&T 20C490 15/16/24-bit DAC */
#define DAC_ATT491	18	/* AT&T 20C491 15/16/24-bit DAC w/gamma corr */
#define DAC_ATT492	19	/* AT&T 20C492 15/16/18-bit DAC w/gamma corr */
#define DAC_ATT493	20	/* AT&T 20C493 15/16/18-bit DAC */
#define DAC_ATT497	21	/* AT&T 20C497 8-bit PseudoColor,24-bit wide */
#define DAC_BT485	22	/* BrookTree 485 RAMDAC */
#define DAC_ATT504	23	/* AT&T 20C504 (Bt484 clone, I think) */
#define DAC_ATT505	24	/* AT&T 20C505 (Bt485 clone) */
#define DAC_TVP3020	25	/* TI ViewPoint TVP3020 RAMDAC */
#define DAC_TVP3025	26	/* TI ViewPoint TVP3025 RAMDAC */
#define DAC_EDSUN	27	/* EDSUN CEG DAC */
#define DAC_ATT498	28	/* AT&T 20C498/21C498 15/16/24-bit DAC w/pixel-mux */
#define DAC_ATT22C498	29	/* AT&T 22C498 15/16/24-bit DAC w/pixel-mux */
#define DAC_STG1700	30	/* STG 1700 15/16/24-bit DAC w/pixel-mux */
#define DAC_S3_GENDAC	31	/* S3 86C708 GENDAC 15/16/24-bit DAC w/PLL  */
#define DAC_S3_SDAC	32	/* S3 86C716 SDAC 15/16/24 w/pixel-mux w/PLL */
#define DAC_TVP3026	33	/* TI ViewPoint TVP3026 RAMDAC */
#define DAC_IBMRGB524	34	/* IBM RGB524 RAMDAC */
#define DAC_IBMRGB525	35	/* IBM RGB525 RAMDAC */
#define DAC_IBMRGB528	36	/* IBM RGB528 RAMDAC */
#define DAC_STG1703	37	/* STG 1703 15/16/24-bit DAC w/pixel-mux w/dual PLL */
#define DAC_ATT409	38	/* AT&T 20C409 15/16/24-bit DAC */
#define DAC_ATT499	39	/* AT&T 20C499 15/16/24-bit DAC */
#define DAC_TKD8001	40	/* Trident TKD8001 15/16/24-bit DAC */
#define DAC_TGUIDAC	41	/* Trident TGUIDAC 15/16/24-bit DAC */
#define DAC_ATI_INTERNAL 42     /* ATI Mach64 integrated DAC */
#define DAC_MU9C1880	43	/* Music 9C1880 */
#define DAC_IMSG174	44	/* Inmos G-174 */
#define DAC_STG1702	45	/* STG 1702 15/16/24-bit DAC w/pixel-mux */
#define DAC_CH8398	46	/* Chrontel 8398 */
#define DAC_ATT408	47	/* AT&T 20C408 */
#define DAC_TVP3030	48	/* TI ViewPoint TVP3030 RAMDAC */
#define DAC_ET6K	49	/* Built-in 15/16/24-bit ET6000 RAMDAC */
#define DAC_ZOOMDAC	50	/* IC Works w30C516 ZOOMDAC or ATT20C498 */
#define DAC_ALSC_642x	51	/* Alliance ProMotion 642[24] integrated DAC */
#define DAC_ICS5341	52	/* ICS5341 16-bit SDAC-like DAC used on ET4000W32p */
#define DAC_ICS5301	53	/* ICS5301 8-bit GENDAC-like DAC used on ET4000W32 */
#define DAC_MGA1064SG	54	/* Matrox Mystique integrated DAC */

#define DAC_MAX		DAC_MGA1064SG	/* UPDATE THIS! */

#define DAC_6_8_PROGRAM	0x40	/* RAMDAC programmable for 6/8-bit tables */
#define DAC_8BIT	0x80	/* RAMDAC with 8-bit wide lookup tables */

#define DAC_CHIP(x) ((x) & ~(DAC_8BIT|DAC_6_8_PROGRAM))


/*
 * RAMDAC names
 */

struct RamDac_Name {
	CONST char *Short;
	CONST char *Long;
};
extern struct RamDac_Name RamDac_Names[];

/*
 * Base chipset classes
 */
#define CHIP_MDA	0	/* Monochrome Display Adapter */
#define CHIP_CGA	1	/* Color Display Adapter */
#define CHIP_EGA	2	/* Enhanced Graphics Adapter */
#define CHIP_PGC	3	/* Professional Graphics Controller */
#define CHIP_VGA	4	/* Video Graphics Array */
#define CHIP_MCGA	5	/* MultiColor Graphics Array */
#define CHIP_COPROC	6	/* Graphics Coprocessor */

/*
 * Hercules cards
 */
#define HERC_TYPE(n)	(CHIP_MDA | ((n) << 8))
#define CHIP_HERC_STD	HERC_TYPE(1)
#define CHIP_HERC_PLUS	HERC_TYPE(2)
#define CHIP_HERC_COL	HERC_TYPE(3)

/*
 * SVGA cards
 */
#define SVGA_TYPE(v,n)	(((v) << 16) | ((n) << 8) | CHIP_VGA)
#define V_AHEAD		1
#define V_ATI		2
#define V_AL		3
#define V_CT		4
#define V_CIRRUS	5
#define V_COMPAQ	6
#define V_GENOA		7
#define V_HMC		8
#define V_MX		9
#define V_NCR		10
#define V_OAK		11
#define V_PRIMUS	12
#define V_REALTEK	13
#define V_S3		14
#define V_TRIDENT	15
#define V_TSENG		16
#define V_UMC		17
#define V_VIDEO7	18
#define V_WD		19
#define V_WEITEK	20
#define V_YAMAHA	21
#define V_SIS		22
#define V_ARK		23
#define V_ALLIANCE	24
#define V_MATROX	25
#define V_SD		26

#define NUM_VENDORS	26
#define CHPS_PER_VENDOR	34

#define CHIP_AHEAD_UNK	SVGA_TYPE(V_AHEAD,0)	/* Ahead unknown	*/
#define CHIP_AHEAD_A	SVGA_TYPE(V_AHEAD,1)	/* Ahead V5000 Version A*/
#define CHIP_AHEAD_B	SVGA_TYPE(V_AHEAD,2)	/* Ahead V5000 Version B*/
#define CHIP_ARK_UNK	SVGA_TYPE(V_ARK,0)	/* ARK Logic unknown	*/
#define CHIP_ARK1000VL	SVGA_TYPE(V_ARK,1)	/* ARK Logic ARK1000VL	*/
#define CHIP_ARK1000PV	SVGA_TYPE(V_ARK,2)	/* ARK Logic ARK1000PV	*/
#define CHIP_ARK2000PV	SVGA_TYPE(V_ARK,3)	/* ARK Logic ARK2000PV	*/
#define CHIP_ARK2000MT	SVGA_TYPE(V_ARK,4)	/* ARK Logic ARK2000MT	*/
#define CHIP_ARK2000MI	SVGA_TYPE(V_ARK,5)	/* ARK Logic ARK2000MI	*/
#define CHIP_ATI_UNK	SVGA_TYPE(V_ATI,0)	/* ATI unknown		*/
#define CHIP_ATI18800	SVGA_TYPE(V_ATI,1)	/* ATI 18800 		*/
#define CHIP_ATI18800_1	SVGA_TYPE(V_ATI,2)	/* ATI 18800-1 		*/
#define CHIP_ATI28800_2	SVGA_TYPE(V_ATI,3)	/* ATI 28800-2 		*/
#define CHIP_ATI28800_4	SVGA_TYPE(V_ATI,4)	/* ATI 28800-4		*/
#define CHIP_ATI28800_5	SVGA_TYPE(V_ATI,5)	/* ATI 28800-5		*/
#define CHIP_ATI28800_6	SVGA_TYPE(V_ATI,6)	/* ATI 28800-6		*/
#define CHIP_ATI68800_3	SVGA_TYPE(V_ATI,7)	/* ATI 68800-3		*/
#define CHIP_ATI68800_6	SVGA_TYPE(V_ATI,8)	/* ATI 68800-6 		*/
#define CHIP_ATI68800LX	SVGA_TYPE(V_ATI,9)	/* ATI 68800LX		*/
#define CHIP_ATI68800AX	SVGA_TYPE(V_ATI,10)	/* ATI 68800AX		*/
#define CHIP_ATI88800GXC SVGA_TYPE(V_ATI,11)	/* ATI 88800GX-C	*/
#define CHIP_ATI88800GXD SVGA_TYPE(V_ATI,12)	/* ATI 88800GX-D	*/
#define CHIP_ATI88800GXE SVGA_TYPE(V_ATI,13)	/* ATI 88800GX-E	*/
#define CHIP_ATI88800GXF SVGA_TYPE(V_ATI,14)	/* ATI 88800GX-F	*/
#define CHIP_ATI88800CX	SVGA_TYPE(V_ATI,15) 	/* ATI 88800CX		*/
#define CHIP_ATI264CT	SVGA_TYPE(V_ATI,16) 	/* ATI 264CT		*/
#define CHIP_ATI264ET	SVGA_TYPE(V_ATI,17) 	/* ATI 264ET		*/
#define CHIP_ATI264LT	SVGA_TYPE(V_ATI,18)	/* ATI 264LT		*/
#define CHIP_ATI264VT	SVGA_TYPE(V_ATI,19) 	/* ATI 264VT		*/
#define CHIP_ATI264VTB	SVGA_TYPE(V_ATI,20)	/* ATI 264VT-B		*/
#define CHIP_ATI264GT	SVGA_TYPE(V_ATI,21) 	/* ATI 264GT		*/
#define CHIP_ATI264GTB	SVGA_TYPE(V_ATI,22)	/* ATI 264GT-B		*/
#define CHIP_AL_UNKNOWN	SVGA_TYPE(V_AL,0)	/* Avance Logic unknown	*/
#define CHIP_AL2101	SVGA_TYPE(V_AL,1)	/* Avance Logic 2101	*/
#define CHIP_AL2228	SVGA_TYPE(V_AL,2)	/* Avance Logic 2228	*/
#define CHIP_ALSC_UNK	SVGA_TYPE(V_ALLIANCE,0)	/* Alliance unknown	*/
#define CHIP_ALSC6410	SVGA_TYPE(V_ALLIANCE,1)	/* Alliance PM6410	*/
#define CHIP_ALSC6422	SVGA_TYPE(V_ALLIANCE,2)	/* Alliance PM6422	*/
#define CHIP_ALSCAT24	SVGA_TYPE(V_ALLIANCE,3)	/* Alliance AT24	*/
#define CHIP_CT_UNKNOWN	SVGA_TYPE(V_CT,0)	/* C&T unknown		*/
#define CHIP_CT450	SVGA_TYPE(V_CT,1)	/* C&T 82c450		*/
#define CHIP_CT451	SVGA_TYPE(V_CT,2)	/* C&T 82c451		*/
#define CHIP_CT452	SVGA_TYPE(V_CT,3)	/* C&T 82c452		*/
#define CHIP_CT453	SVGA_TYPE(V_CT,4)	/* C&T 82c453		*/
#define CHIP_CT455	SVGA_TYPE(V_CT,5)	/* C&T 82c455		*/
#define CHIP_CT456	SVGA_TYPE(V_CT,6)	/* C&T 82c456		*/
#define CHIP_CT457	SVGA_TYPE(V_CT,7)	/* C&T 82c457		*/
#define CHIP_CTF65510	SVGA_TYPE(V_CT,8)	/* C&T F65510		*/
#define CHIP_CTF65520	SVGA_TYPE(V_CT,9)	/* C&T F65520		*/
#define CHIP_CTF65530	SVGA_TYPE(V_CT,10)	/* C&T F65530		*/
#define CHIP_CTF65540	SVGA_TYPE(V_CT,11)	/* C&T F65540		*/
#define CHIP_CTF65545	SVGA_TYPE(V_CT,12)	/* C&T F65545		*/
#define CHIP_CTF65548	SVGA_TYPE(V_CT,13)	/* C&T F65548		*/
#define CHIP_CTF65546	SVGA_TYPE(V_CT,14)	/* C&T F65546		*/
#define CHIP_CTF65550	SVGA_TYPE(V_CT,15)	/* C&T F65550		*/
#define CHIP_CTF65554	SVGA_TYPE(V_CT,16)	/* C&T F65554		*/
#define CHIP_CL_UNKNOWN	SVGA_TYPE(V_CIRRUS,0)	/* Cirrus unknown	*/
#define CHIP_CL510	SVGA_TYPE(V_CIRRUS,1)	/* Cirrus CL-GD 510/520	*/
#define CHIP_CL610	SVGA_TYPE(V_CIRRUS,2)	/* Cirrus CL-GD 610/620	*/
#define CHIP_CLV7	SVGA_TYPE(V_CIRRUS,3)	/* Cirrus Video 7 OEM	*/
#define CHIP_CLAVGA2	SVGA_TYPE(V_CIRRUS,4)	/* Cirrus/AcuMos AVGA2	*/
#define CHIP_CL5402	SVGA_TYPE(V_CIRRUS,5)	/* Cirrus 5402		*/
#define CHIP_CL5402R1	SVGA_TYPE(V_CIRRUS,6)	/* Cirrus 5402 rev 1	*/
#define CHIP_CL5420	SVGA_TYPE(V_CIRRUS,7)	/* Cirrus 5420		*/
#define CHIP_CL5420R1	SVGA_TYPE(V_CIRRUS,8)	/* Cirrus 5420 rev 1	*/
#define CHIP_CL5422	SVGA_TYPE(V_CIRRUS,9)	/* Cirrus 5422		*/
#define CHIP_CL5424	SVGA_TYPE(V_CIRRUS,10)	/* Cirrus 5424		*/
#define CHIP_CL5426	SVGA_TYPE(V_CIRRUS,11)	/* Cirrus 5426		*/
#define CHIP_CL5428	SVGA_TYPE(V_CIRRUS,12)	/* Cirrus 5428		*/
#define CHIP_CL5429	SVGA_TYPE(V_CIRRUS,13)	/* Cirrus 5429		*/
#define CHIP_CL5430	SVGA_TYPE(V_CIRRUS,14)	/* Cirrus 5430		*/
#define CHIP_CL5434	SVGA_TYPE(V_CIRRUS,15)	/* Cirrus 5434		*/
#define CHIP_CL6205	SVGA_TYPE(V_CIRRUS,16)	/* Cirrus 6205		*/
#define CHIP_CL6215	SVGA_TYPE(V_CIRRUS,17)	/* Cirrus 6215		*/
#define CHIP_CL6225	SVGA_TYPE(V_CIRRUS,18)	/* Cirrus 6225		*/
#define CHIP_CL6235	SVGA_TYPE(V_CIRRUS,19)	/* Cirrus 6235		*/
#define CHIP_CL5410	SVGA_TYPE(V_CIRRUS,20)	/* Cirrus 6510		*/
#define CHIP_CL6410	SVGA_TYPE(V_CIRRUS,21)	/* Cirrus 6410		*/
#define CHIP_CL6412	SVGA_TYPE(V_CIRRUS,22)	/* Cirrus 6412		*/
#define CHIP_CL6420A	SVGA_TYPE(V_CIRRUS,23)	/* Cirrus 6420A		*/
#define CHIP_CL6420B	SVGA_TYPE(V_CIRRUS,24)	/* Cirrus 6420B		*/
#define CHIP_CL6440	SVGA_TYPE(V_CIRRUS,25)	/* Cirrus 6440		*/
#define CHIP_CL5436	SVGA_TYPE(V_CIRRUS,26)	/* Cirrus 5436		*/
#define CHIP_CL7541	SVGA_TYPE(V_CIRRUS,27)	/* Cirrus 7541		*/
#define CHIP_CL7542	SVGA_TYPE(V_CIRRUS,28)	/* Cirrus 7542		*/
#define CHIP_CL7543	SVGA_TYPE(V_CIRRUS,29)	/* Cirrus 7543		*/
#define CHIP_CL5446	SVGA_TYPE(V_CIRRUS,30)	/* Cirrus 5446		*/
#define CHIP_CL5462	SVGA_TYPE(V_CIRRUS,31)	/* Cirrus 5462		*/
#define CHIP_CL7548	SVGA_TYPE(V_CIRRUS,32)	/* Cirrus 7548		*/
#define CHIP_CL5464	SVGA_TYPE(V_CIRRUS,33)	/* Cirrus 5464		*/
#define CHIP_CPQ_UNK	SVGA_TYPE(V_COMPAQ,0)	/* Compaq unknown	*/
#define CHIP_CPQ_IVGS	SVGA_TYPE(V_COMPAQ,1)	/* Compaq Int Vid Gr Sys*/
#define CHIP_CPQ_AVGA	SVGA_TYPE(V_COMPAQ,2)	/* Compaq Advanced VGA	*/
#define CHIP_CPQ_Q1024	SVGA_TYPE(V_COMPAQ,3)	/* Compaq QVision/1024	*/
#define CHIP_CPQ_Q1280	SVGA_TYPE(V_COMPAQ,4)	/* Compaq QVision/1280	*/
#define CHIP_CPQ_AVGA_P	SVGA_TYPE(V_COMPAQ,5)	/* Compaq AVGA Portable	*/
#define CHIP_G_6100	SVGA_TYPE(V_GENOA,0)	/* Genoa GVGA 6100	*/
#define CHIP_G_6200	SVGA_TYPE(V_GENOA,1)	/* Genoa GVGA 6200	*/
#define CHIP_G_6400	SVGA_TYPE(V_GENOA,2)	/* Genoa GVGA 6400	*/
#define CHIP_HM86304	SVGA_TYPE(V_HMC,0)	/* HMC HM86304		*/
#define CHIP_MX68000	SVGA_TYPE(V_MX,0)	/* MX 68000		*/
#define CHIP_MX68010	SVGA_TYPE(V_MX,1)	/* MX 68010		*/
#define CHIP_NCR_UNK	SVGA_TYPE(V_NCR,0)	/* NCR unknown		*/
#define CHIP_NCR77C21	SVGA_TYPE(V_NCR,1)	/* NCR 77C21		*/
#define CHIP_NCR77C22	SVGA_TYPE(V_NCR,2)	/* NCR 77C22		*/
#define CHIP_NCR77C22E	SVGA_TYPE(V_NCR,3)	/* NCR 77C22E		*/
#define CHIP_NCR77C22EP	SVGA_TYPE(V_NCR,4)	/* NCR 77C22E+		*/
#define CHIP_NCR77C32B	SVGA_TYPE(V_NCR,5)	/* NCR 77C32BLT		*/
#define CHIP_OAK_UNK	SVGA_TYPE(V_OAK,0)	/* OAK unknown		*/
#define CHIP_OAK037C	SVGA_TYPE(V_OAK,1)	/* OAK OTI037C		*/
#define CHIP_OAK057	SVGA_TYPE(V_OAK,2)	/* OAK OTI-057		*/
#define CHIP_OAK067	SVGA_TYPE(V_OAK,3)	/* OAK OTI-067		*/
#define CHIP_OAK077	SVGA_TYPE(V_OAK,4)	/* OAK OTI-077		*/
#define CHIP_OAK083	SVGA_TYPE(V_OAK,5)	/* OAK OTI-083		*/
#define CHIP_OAK087	SVGA_TYPE(V_OAK,6)	/* OAK OTI-087		*/
#define CHIP_P2000	SVGA_TYPE(V_PRIMUS,0)	/* Primus P2000		*/
#define CHIP_RT_UNK	SVGA_TYPE(V_REALTEK,0)	/* Realtek unknown	*/
#define CHIP_RT_3103	SVGA_TYPE(V_REALTEK,1)	/* Realtek RT3103	*/
#define CHIP_RT_3105	SVGA_TYPE(V_REALTEK,2)	/* Realtek RT3105	*/
#define CHIP_RT_3106	SVGA_TYPE(V_REALTEK,3)	/* Realtek RT3106	*/
#define CHIP_S3_UNKNOWN	SVGA_TYPE(V_S3,0)	/* S3 unknown		*/
#define CHIP_S3_911	SVGA_TYPE(V_S3,1)	/* S3 86c911		*/
#define CHIP_S3_924	SVGA_TYPE(V_S3,2)	/* S3 86c924 or 911A	*/
#define CHIP_S3_801B	SVGA_TYPE(V_S3,3)	/* S3 86c801 A-B step	*/
#define CHIP_S3_801C	SVGA_TYPE(V_S3,4)	/* S3 86c801 C step	*/
#define CHIP_S3_801D	SVGA_TYPE(V_S3,5)	/* S3 86c801 D step	*/
#define CHIP_S3_801I	SVGA_TYPE(V_S3,6)	/* S3 86c801i (?)	*/
#define CHIP_S3_805B	SVGA_TYPE(V_S3,7)	/* S3 86c805 A-B step	*/
#define CHIP_S3_805C	SVGA_TYPE(V_S3,8)	/* S3 86c805 C step	*/
#define CHIP_S3_805D	SVGA_TYPE(V_S3,9)	/* S3 86c805 D step	*/
#define CHIP_S3_805I	SVGA_TYPE(V_S3,10)	/* S3 86c805i		*/
#define CHIP_S3_928D	SVGA_TYPE(V_S3,11)	/* S3 86c928 A-D step	*/
#define CHIP_S3_928E	SVGA_TYPE(V_S3,12)	/* S3 86c928 E-step	*/
#define CHIP_S3_928P	SVGA_TYPE(V_S3,13)	/* S3 86c928PCI		*/
#define CHIP_S3_864	SVGA_TYPE(V_S3,14)	/* S3 Vision864		*/
#define CHIP_S3_964	SVGA_TYPE(V_S3,15)	/* S3 Vision964		*/
#define CHIP_S3_866	SVGA_TYPE(V_S3,16)	/* S3 Vision866		*/
#define CHIP_S3_868	SVGA_TYPE(V_S3,17)	/* S3 Vision868		*/
#define CHIP_S3_968	SVGA_TYPE(V_S3,18)	/* S3 Vision968		*/
#define CHIP_S3_Trio32	SVGA_TYPE(V_S3,19)	/* S3 Trio32		*/
#define CHIP_S3_Trio64	SVGA_TYPE(V_S3,20)	/* S3 Trio64		*/
#define CHIP_S3_Trio64V SVGA_TYPE(V_S3,21)	/* S3 Trio64V+		*/
#define CHIP_S3_968_3DLABS_300SX SVGA_TYPE(V_S3,22)	/* S3 968 & 3Dlabs 300SX	*/
#define CHIP_S3_968_3DLABS_UNK   SVGA_TYPE(V_S3,23)	/* S3 968 & 3Dlabs 300SX	*/
#define CHIP_S3_ViRGE   SVGA_TYPE(V_S3,24)	/* S3 ViRGE		*/
#define CHIP_S3_ViRGE_VX SVGA_TYPE(V_S3,25)	/* S3 ViRGE/VX		*/
#define CHIP_S3_Aurora64VP	SVGA_TYPE(V_S3,26)	/* S3 Aurora64V+	*/
#define CHIP_S3_Trio64UVP	SVGA_TYPE(V_S3,27)	/* S3 Trio64UV+		*/
#define CHIP_S3_Trio64V2_DX	SVGA_TYPE(V_S3,28)	/* S3 Trio64V2/DX	*/
#define CHIP_S3_Trio64V2_GX	SVGA_TYPE(V_S3,29)	/* S3 Trio64V2/GX	*/
#define CHIP_S3_ViRGE_DX SVGA_TYPE(V_S3,30)	/* S3 ViRGE/DX		*/
#define CHIP_S3_ViRGE_GX SVGA_TYPE(V_S3,31)	/* S3 ViRGE/GX		*/
#define CHIP_S3_PLATO_PX SVGA_TYPE(V_S3,32)	/* S3 PLATO/PX		*/
#define CHIP_TVGA_UNK	SVGA_TYPE(V_TRIDENT,0)	/* Trident unknown	*/
#define CHIP_TVGA8200	SVGA_TYPE(V_TRIDENT,1)	/* Trident LX8200	*/
#define CHIP_TVGA8800BR	SVGA_TYPE(V_TRIDENT,2)	/* Trident 8800BR	*/
#define CHIP_TVGA8800CS	SVGA_TYPE(V_TRIDENT,3)	/* Trident 8800CS	*/
#define CHIP_TVGA8900B	SVGA_TYPE(V_TRIDENT,4)	/* Trident 8900B	*/
#define CHIP_TVGA8900C	SVGA_TYPE(V_TRIDENT,5)	/* Trident 8900C	*/
#define CHIP_TVGA8900CL	SVGA_TYPE(V_TRIDENT,6)	/* Trident 8900CL	*/
#define CHIP_TVGA9000	SVGA_TYPE(V_TRIDENT,7)	/* Trident 9000		*/
#define CHIP_TVGA9000I	SVGA_TYPE(V_TRIDENT,8)	/* Trident 9000i	*/
#define CHIP_TVGA9100B	SVGA_TYPE(V_TRIDENT,9)	/* Trident LCD9100B	*/
#define CHIP_TVGA9200CX	SVGA_TYPE(V_TRIDENT,10)	/* Trident 9200CXr	*/
#define CHIP_TVGA9320	SVGA_TYPE(V_TRIDENT,11)	/* Trident LCD9320	*/
#define CHIP_TVGA9400CX	SVGA_TYPE(V_TRIDENT,12)	/* Trident 9400CXi	*/
#define CHIP_TVGA9420	SVGA_TYPE(V_TRIDENT,13)	/* Trident GUI9420	*/
#define CHIP_TVGA9420D	SVGA_TYPE(V_TRIDENT,14) /* Trident GUI9420DGi	*/
#define CHIP_TVGA9440	SVGA_TYPE(V_TRIDENT,15)	/* Trident GUI9440AGi	*/
#define CHIP_TVGA9660	SVGA_TYPE(V_TRIDENT,16)	/* Trident GUI9660	*/
#define CHIP_TVGA9680	SVGA_TYPE(V_TRIDENT,17) /* Trident GUI9680	*/
#define CHIP_TVGA9682	SVGA_TYPE(V_TRIDENT,18) /* Trident GUI9682	*/
#define CHIP_TVGA9685	SVGA_TYPE(V_TRIDENT,19) /* Trident GUI9685	*/
#define CHIP_TVGA9692	SVGA_TYPE(V_TRIDENT,20) /* Trident GUI9692	*/
#define CHIP_TVGA9382	SVGA_TYPE(V_TRIDENT,21) /* Trident Cyber9382	*/
#define CHIP_TVGA9385	SVGA_TYPE(V_TRIDENT,22) /* Trident Cyber9385	*/
#define CHIP_TVGA9385_1	SVGA_TYPE(V_TRIDENT,23) /* Trident Cyber9385-1  */
#define CHIP_SIS_UNK	SVGA_TYPE(V_SIS,0)	/* SiS unknown		*/
#define CHIP_SIS86C201	SVGA_TYPE(V_SIS,1)	/* SiS SG86C201		*/
#define CHIP_SIS86C202	SVGA_TYPE(V_SIS,2)	/* SiS SG86C202		*/
#define CHIP_SIS86C205	SVGA_TYPE(V_SIS,3)	/* SiS SG86C205		*/
#define CHIP_MATROX_UNK	SVGA_TYPE(V_MATROX,0)	/* Matrox unknown	*/
#define CHIP_MGA2085PX	SVGA_TYPE(V_MATROX,1)	/* Matrox Atlas		*/
#define CHIP_MGA2064W	SVGA_TYPE(V_MATROX,2)	/* Matrox Millennium	*/
#define CHIP_MGA1064SG	SVGA_TYPE(V_MATROX,3)	/* Matrox Mystique	*/
#define CHIP_TSENG_UNK	SVGA_TYPE(V_TSENG,0)	/* Tseng unknown	*/
#define CHIP_ET3000	SVGA_TYPE(V_TSENG,1)	/* Tseng ET3000		*/
#define CHIP_ET4000	SVGA_TYPE(V_TSENG,2)	/* Tseng ET4000		*/
#define CHIP_ET4000W32	SVGA_TYPE(V_TSENG,3)	/* Tseng ET4000/W32	*/
#define CHIP_ET4000W32I	SVGA_TYPE(V_TSENG,4)	/* Tseng ET4000/W32i	*/
#define CHIP_ET4KW32P_A	SVGA_TYPE(V_TSENG,5)	/* Tseng ET4000/W32p rA	*/
#define CHIP_ET4KW32I_B	SVGA_TYPE(V_TSENG,6)	/* Tseng ET4000/W32i rB	*/
#define CHIP_ET4KW32I_C	SVGA_TYPE(V_TSENG,7)	/* Tseng ET4000/W32i rC	*/
#define CHIP_ET4KW32P_B	SVGA_TYPE(V_TSENG,8)	/* Tseng ET4000/W32p rB	*/
#define CHIP_ET4KW32P_C	SVGA_TYPE(V_TSENG,9)	/* Tseng ET4000/W32p rC	*/
#define CHIP_ET4KW32P_D	SVGA_TYPE(V_TSENG,10)	/* Tseng ET4000/W32p rD	*/
#define CHIP_ET6K	SVGA_TYPE(V_TSENG,11)	/* Tseng ET6000		*/
#define CHIP_UMC_408	SVGA_TYPE(V_UMC,0)	/* UMC 85c408		*/
#define CHIP_V7_UNKNOWN	SVGA_TYPE(V_VIDEO7,0)	/* Video7 unknown	*/
#define CHIP_V7_FWRITE	SVGA_TYPE(V_VIDEO7,1)	/* Video7 Fastwrite/VRAM*/
#define CHIP_V7_1024i	SVGA_TYPE(V_VIDEO7,2)	/* Video7 1024i		*/
#define CHIP_V7_VRAM2_B	SVGA_TYPE(V_VIDEO7,3)	/* Video7 VRAM II (B)	*/
#define CHIP_V7_VRAM2_C	SVGA_TYPE(V_VIDEO7,4)	/* Video7 VRAM II (C,D)	*/
#define CHIP_HT216BC	SVGA_TYPE(V_VIDEO7,5)	/* Video7 HT216B,C	*/
#define CHIP_HT216D	SVGA_TYPE(V_VIDEO7,6)	/* Video7 HT216D	*/
#define CHIP_HT216E	SVGA_TYPE(V_VIDEO7,7)	/* Video7 HT216E	*/
#define CHIP_HT216F	SVGA_TYPE(V_VIDEO7,8)	/* Video7 HT216F	*/
#define CHIP_V7_VEGA	SVGA_TYPE(V_VIDEO7,9)	/* Video7 VEGA		*/
#define CHIP_WD_UNK	SVGA_TYPE(V_WD,0)	/* WD unknown		*/
#define CHIP_WD_PVGA1	SVGA_TYPE(V_WD,1)	/* WD PVGA1		*/
#define CHIP_WD_90C00	SVGA_TYPE(V_WD,2)	/* WD 90C00		*/
#define CHIP_WD_90C10	SVGA_TYPE(V_WD,3)	/* WD 90C10		*/
#define CHIP_WD_90C11	SVGA_TYPE(V_WD,4)	/* WD 90C11		*/
#define CHIP_WD_90C20	SVGA_TYPE(V_WD,5)	/* WD 90C20		*/
#define CHIP_WD_90C20A	SVGA_TYPE(V_WD,6)	/* WD 90C20A		*/
#define CHIP_WD_90C22	SVGA_TYPE(V_WD,7)	/* WD 90C22		*/
#define CHIP_WD_90C24	SVGA_TYPE(V_WD,8)	/* WD 90C24		*/
#define CHIP_WD_90C26	SVGA_TYPE(V_WD,9)	/* WD 90C26		*/
#define CHIP_WD_90C27	SVGA_TYPE(V_WD,10)	/* WD 90C27 (guess)	*/
#define CHIP_WD_90C30	SVGA_TYPE(V_WD,11)	/* WD 90C30		*/
#define CHIP_WD_90C31	SVGA_TYPE(V_WD,12)	/* WD 90C31		*/
#define CHIP_WD_90C33	SVGA_TYPE(V_WD,13)	/* WD 90C33		*/
#define CHIP_WEIT_UNK	SVGA_TYPE(V_WEITEK,0)	/* Weitek unknown	*/
#define CHIP_WEIT_5086	SVGA_TYPE(V_WEITEK,1)	/* Weitek 5086		*/
#define CHIP_WEIT_5186	SVGA_TYPE(V_WEITEK,2)	/* Weitek 5186		*/
#define CHIP_WEIT_5286	SVGA_TYPE(V_WEITEK,3)	/* Weitek 5286		*/
#define CHIP_YAMAHA6388	SVGA_TYPE(V_YAMAHA,0)	/* Yamaha 6388 VPDC	*/
#define CHIP_SD_RM_UNK	SVGA_TYPE(V_SD,0)	/* Sigma Desigs unknown	*/
#define CHIP_SD_RM64GX	SVGA_TYPE(V_SD,1)	/* Sigma Desigs SD6425	*/

/*
 * Graphics Coprocessors
 */
#define COPROC_TYPE(c,n)	(((c) << 16) | ((n) << 8) | CHIP_COPROC)
#define C_8514		0
#define C_XGA		1
#define C_MACH64	2
#define C_I128		3
#define C_GLINT		4

#define NUM_CP_TYPES	5
#define CHPS_PER_CPTYPE	8

#define CHIP_8514	COPROC_TYPE(C_8514,0)	/* 8514/A or true clone */
#define CHIP_MACH8	COPROC_TYPE(C_8514,1)	/* ATI Mach8		*/
#define CHIP_MACH32	COPROC_TYPE(C_8514,2)	/* ATI Mach32		*/
#define CHIP_CT480	COPROC_TYPE(C_8514,3)	/* C&T 82c480		*/

#define CHIP_MACH64	COPROC_TYPE(C_MACH64,0)	/* ATI Mach64		*/

#define CHIP_I128	COPROC_TYPE(C_I128,0)	/* Number9 Imagine I128 */

#define CHIP_300SX	COPROC_TYPE(C_GLINT,1)	/* 3DLabs GLINT 300SX	*/
#define CHIP_500TX	COPROC_TYPE(C_GLINT,2)	/* 3DLabs GLINT 500TX	*/
#define CHIP_DELTA	COPROC_TYPE(C_GLINT,4)	/* 3DLabs GLINT DELTA	*/

/*
 * Useful macros
 */
#define IS_MDA(c)	((c) == CHIP_MDA)
#define IS_HERC(c)	((((c) & 0xFF) == CHIP_MDA) && ((c) != CHIP_MDA))
#define HERC_CHIP(c)	(((c) >> 8) & 0xFF)
#define IS_CGA(c)	((c) == CHIP_CGA)
#define IS_EGA(c)	((c) == CHIP_EGA)
#define IS_PGC(c)	((c) == CHIP_PGC)
#define IS_VGA(c)	((c) == CHIP_VGA)
#define IS_SVGA(c)	((((c) & 0xFF) == CHIP_VGA) && ((c) != CHIP_VGA))
#define SVGA_VENDOR(c)	(((c) >> 16) & 0xFF)
#define SVGA_CHIP(c)	(((c) >> 8) & 0xFF)
#define IS_MCGA(c) 	((c) == CHIP_MCGA)
#define IS_COPROC(c)	(((c) & 0xFF) == CHIP_COPROC)
#define COPROC_CLASS(c)	(((c) >> 16) & 0xFF)
#define COPROC_CHIP(c)	(((c) >> 8) & 0xFF)
