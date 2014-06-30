/* $XConsortium: Print.c /main/25 1996/10/28 04:46:33 kaleb $ */
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

/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/Print.c,v 3.46.2.4 1997/05/22 14:00:34 dawes Exp $ */

#include "Probe.h"

static CONST char *SVGA_Names[NUM_VENDORS+1][CHPS_PER_VENDOR] = 
{
/* dummy */	{ "" },	
/* Ahead */	{ "Ahead (chipset unknown)",
		  "Ahead V5000 Version A", "Ahead V5000 Version B" },
/* ATI */	{ "ATI (chipset unknown)", 
		  "ATI 18800", "ATI 18800-1",
		  "ATI 28800-2", "ATI 28800-4", "ATI 28800-5", "ATI 28800-6",
		  "ATI 68800-3", "ATI 68800-6", "ATI 68800LX", "ATI 68800AX",
		  "ATI 88800GX-C", "ATI 88800GX-D", "ATI 88800GX-E",
		  "ATI 88800GX-F",
		  "ATI 88800CX",
		  "ATI 264CT", "ATI 264ET", "ATI 264LT",
		  "ATI 264VT", "ATI 264VT-B",
		  "ATI 264GT (3D Rage I)", "ATI 264GT-B (3D Rage II)" },
/* AL */	{ "Avance Logic (chipset unknown)",
		  "Avance Logic 2101", "Avance Logic 2228" },
/* CT */	{ "Chips & Tech (chipset unknown)",
		  "Chips & Tech 82c450", "Chips & Tech 82c451", 
		  "Chips & Tech 82c452", "Chips & Tech 82c453",
		  "Chips & Tech 82c455", "Chips & Tech 82c456",
		  "Chips & Tech 82c457", "Chips & Tech F65510",
		  "Chips & Tech F65520", "Chips & Tech F65530",
		  "Chips & Tech F65540", "Chips & Tech F65545",
		  "Chips & Tech F65548", "Chips & Tech F65546",
		  "Chips & Tech F65550", "Chips & Tech F65554"},
/* Cirrus */	{ "Cirrus (chipset unknown)",
		  "Cirrus CL-GD 510/520", "Cirrus CL-GD 610/620",
		  "Cirrus Video7 OEM",
		  "Cirrus/AcuMos AVGA2",
		  "Cirrus CL-GD5402", "Cirrus CL-GD5402 Rev 1",
		  "Cirrus CL-GD5420", "Cirrus CL-GD5420 Rev 1",
		  "Cirrus CL-GD5422", "Cirrus CL-GD5424", 
		  "Cirrus CL-GD5426", "Cirrus CL-GD5428",
		  "Cirrus CL-GD5429",
		  "Cirrus CL-GD5430/40", "Cirrus CL-GD5434",
		  "Cirrus CL-GD6205", "Cirrus CL-GD6215",
		  "Cirrus CL-GD6225", "Cirrus CL-GD6235",
		  "Cirrus CL-GD5410",
		  "Cirrus CL-GD6410", "Cirrus CL-GD6412",
		  "Cirrus CL-GD6420A", "Cirrus CL-GD6420B",
		  "Cirrus CL-GD6440",
		  "Cirrus CL-GD5436",
		  "Cirrus CL-GD7541", "Cirrus CL-GD7542",
		  "Cirrus CL-GD7543",
		  "Cirrus CL-GD5446",
		  "Cirrus CL-GD5462",
		  "Cirrus CL-GD7548",
		  "Cirrus CL-GD5464",
		},
/* Compaq */	{ "Compaq (chipset unknown)",
		  "Compaq Int. Vid. Graphics Sys.",
		  "Compaq Advanced VGA", 
		  "Compaq QVision/1024", "Compaq QVision/1280",
		  "Compaq Advanced VGA, Portable" },
/* Genoa */	{ "Genoa GVGA 6100",
		  "Genoa GVGA 6200/6300", "Genoa GVGA 6400/6600" },
/* HMC */	{ "HMC HM86304" },
/* MX */	{ "MX 68000", "MX 68010" },
/* NCR */	{ "NCR (chipset unknown)",
		  "NCR 77C21", "NCR 77C22", "NCR 77C22E", "NCR 77C22E+",
		  "NCR 77C32BLT" },
/* Oak */	{ "Oak (chipset unknown)",
		  "Oak OTI037C", 
		  "Oak OTI-057", "Oak OTI-067", "Oak OTI-077",
		  "Oak OTI-083", "Oak OTI-087" },
/* Primus */	{ "Primus P2000" },
/* Realtek */	{ "Realtek (chipset unknown)",
		  "Realtek RT-3103", "Realtek RT-3105", "Realtek RT-3106" },
/* S3 */	{ "S3 (chipset unknown)",
		  "S3 86C911", "S3 86C924",
		  "S3 86C801, A or B-step", 
		  "S3 86C801, C-step", "S3 86C801, D-step", 
		  "S3 86C801i",
		  "S3 86C805, A or B-step", 
		  "S3 86C805, C-step", "S3 86C805, D-step",
		  "S3 86C805i",
		  "S3 86C928, A,B,C, or D-step", "S3 86C928, E-step",
		  "S3 86C928PCI",
		  "S3 Vision864",
		  "S3 Vision964",
		  "S3 Vision866",
		  "S3 Vision868",
		  "S3 Vision968",
		  "S3 Trio32",
		  "S3 Trio64",
		  "S3 Trio64V+",
		  "S3 Vision968 & 3Dlabs 300SX",
		  "S3 Vision968 & 3Dlabs (unknown)",
		  "S3 ViRGE",
		  "S3 ViRGE/VX",
		  "S3 Aurora64V+",
		  "S3 Trio64UV+",
		  "S3 Trio64V2/DX",
		  "S3 Trio64V2/GX",
		  "S3 ViRGE/DX",
		  "S3 ViRGE/GX",
		  "S3 PLATO/PX",
	       },
/* Trident */	{ "Trident (chipset unknown)",
		  "Trident LX8200",
		  "Trident 8800BR", "Trident 8800CS",
		  "Trident 8900B", "Trident 8900C", "Trident 8900CL/8900D",
		  "Trident 9000", "Trident 9000i",
		  "Trident LCD9100B", "Trident 9200CXr", "Trident LCD9320", 
		  "Trident 9400CXi", "Trident GUI 9420", "Trident GUI 9420DGi",
                  "Trident GUI 9440AGi", "Trident GUI 9660", 
                  "Trident GUI 9680", "Trident ProVidia 9682",
		  "Trident ProVidia 9685", "Trident ProVidia 9692",
		  "Trident Cyber9382", "Trident Cyber9385",
		  "Trident Cyber9385-1", },
/* Tseng */	{ "Tseng (chipset unknown)",
		  "Tseng ET3000", "Tseng ET4000", 
		  "Tseng ET4000/W32", "Tseng ET4000/W32i", 
		  "Tseng ET4000/W32p Rev A",
		  "Tseng ET4000/W32i Rev B",
		  "Tseng ET4000/W32i Rev C",
		  "Tseng ET4000/W32p Rev B",
		  "Tseng ET4000/W32p Rev C",
		  "Tseng ET4000/W32p Rev D",
		  "Tseng ET6000",
		},
/* UMC */	{ "UMC 85c408" },
/* Video7 */	{ "Video7 (chipset unknown)",
		  "Video7 FastWrite/VRAM (HT208)", "Video7 1024i (HT208A)",
		  "Video7 VRAM II (HT208B)", "Video7 VRAM II (HT208C,D)",
		  "Video7 HT216B,C", "Video7 HT216D",
		  "Video7 HT216E", "Video7 HT216E",
		  "Video7 VEGA" },
/* WD */	{ "WD/Paradise (chipset unknown)",
		  "WD/Paradise PVGA1", "WD/Paradise 90C00", 
		  "WD/Paradise 90C10", "WD/Paradise 90C11",
		  "WD/Paradise 90C20", "WD/Paradise 90C20A",
		  "WD/Paradise 90C22", "WD/Paradise 90C24", 
		  "WD/Paradise 90C26", "WD/Paradise 90C27",
		  "WD/Paradise 90C30", "WD/Paradise 90C31",
		  "WD/Paradise 90C33" },
/* Weitek */	{ "Weitek (chipset unknown)",
		  "Weitek 5086", "Weitek 5186", "Weitek 5286" },
/* Yamaha */	{ "Yamaha 6388 VPDC" },
/* SiS */	{ "Silicon Integrated Systems (chipset unknown)",
                  "Silicon Integrated Systems SG86C201",
		  "Silicon Integrated Systems SG86C202",
		  "Silicon Integrated Systems SG86C205" },
/* ARK */	{ "ARK Logic (chipset unknown)",
		  "ARK Logic ARK1000VL",
		  "ARK Logic ARK1000PV",
		  "ARK Logic ARK2000PV",
		  "ARK Logic ARK2000MT",
		  "ARK Logic ARK2000MI (Quadro64)" },
/* Alliance */	{ "Alliance Semiconductor (chipset unknown)",
		  "Alliance Semiconductor ProMotion 6410",
		  "Alliance Semiconductor ProMotion 6422",
		  "Alliance Semiconductor ProMotion AT24" },
/* Matrox */	{ "Matrox (chipset unknown)",
		  "Matrox Atlas",
		  "Matrox Millennium",
		  "Matrox Mystique" },
/* Sigma Designs */	{ "Sigma Designs (chipset unknown)",
		  "Sigma Designs REALmagic64/GX (SD 6425)" },
};

static CONST char *Herc_Names[] = 
{
	"",		/* indices start at 1 */
	"Standard",
	"Plus",
	"InColor",
};

struct RamDac_Name RamDac_Names[] =
{
	{ "Unknown", "Non-standard but unknown DAC" },
	{ "Generic", "Generic 8-bit pseudo-color DAC" },
	{ "ALG1101", "Avance Logc ALG1101" },
	{ "SS2410", "Diamond SS2410 15/16/24-bit DAC" },
	{ "Sierra15", "Sierra SC1148{1,6,8} 15-bit HiColor DAC" },
	{ "Sierra16", 
	  "Sierra SC1148{2,3,4} 15-bit or SC1148{5,7,9} 15/16-bit HiColor" },
	{ "Sierra24", "Sierra 1502{5,6} 15/16/24-bit DAC" },
	{ "MU9C4870", "MUSIC MU9C4870 15/16-bit HiColor DAC" },
	{ "MU9C4910", "MUSIC MU9C4910 15/16/24-bit DAC" },
	{ "ADAC1", "AcuMos ADAC1 15/16/24-bit DAC" },
	{ "68830", "ATI 68830 15/16-bit HiColor DAC" },
	{ "68860", "ATI 68860 15/15/24-bit DAC w/pixel-mux" },
	{ "68875", "ATI-68875/Bt885/TLC34075 15/16/24-bit DAC w/pixel-mux" },
	{ "ATIMisc", 
	  "ATI Misc 3rd-party 15/16/24-bit DAC, probably BT481" },
	{ "Cirrus8", "Cirrus Logic Built-in 8-bit pseudo-color DAC" },
	{ "Cirrus24B", "Cirrus Logic Built-in 15/16/24-bit DAC" },
	{ "Cirrus24", "Cirrus Logic 15/16/24-bit DAC" },
	{ "20C490", "AT&T 20C490 15/16/24-bit DAC" },
	{ "20C491", "AT&T 20C491 15/16/24-bit DAC with gamma correction" },
	{ "20C492", "AT&T 20C492 15/16/18-bit DAC with gamma correction" },
	{ "20C493", "AT&T 20C493 15/16/18-bit DAC" },
	{ "20C497", "AT&T 20C497 24-bit wide, 8-bit pseudo-color DAC" },
	{ "Bt485", "BrookTree Bt485 24-bit TrueColor DAC w/cursor,pixel-mux" },
	{ "20C504", "AT&T 20C504 24-bit TrueColor DAC w/cursor,pixel-mux" },
	{ "20C505", "AT&T 20C505 24-bit TrueColor DAC w/cursor,pixel-mux" },
	{ "TVP3020",
	  "TI ViewPoint3020 24-bit TrueColor DAC w/cursor,pixel-mux" },
	{ "TVP3025",
	  "TI ViewPoint3025 24-bit TrueColor DAC w/cursor,pixel-mux,clock" },
	{ "EDSUN", "EDSUN CEG DAC" },
	{ "20C498", "AT&T 20C498/21C498 15/16/24-bit DAC w/pixel-mux" },
	{ "22C498", "AT&T 22C498 15/16/24-bit DAC w/pixel-mux" },
	{ "STG1700", "STG1700 15/16/24-bit DAC w/pixel-mux" },
	{ "S3_GENDAC", "S3 86C708 GENDAC 15/16/24-bit DAC w/clock-PLL" },
	{ "S3_SDAC", "S3 86C716 SDAC 15/16/24-bit DAC w/pixel-mux w/clock-PLL" },
	{ "TVP3026",
	  "TI ViewPoint3026 24-bit TrueColor DAC w/cursor,pixel-mux,clock" },
	{ "RGB524", 
	  "IBM RGB524 24-bit TrueColor DAC w/cursor,pixel-mux,clock" },
	{ "RGB514/525", 
	  "IBM RGB514/525 24-bit TrueColor DAC w/cursor,pixel-mux,clock" },
	{ "RGB528", 
	  "IBM RGB528 24-bit TrueColor DAC w/cursor,pixel-mux,clock" },
	{ "STG1703", "STG1703 15/16/24-bit DAC w/pixel-mux,clock" },
	{ "20C409", "AT&T 20C409 15/16/24-bit DAC w/clock" },
	{ "20C499", "AT&T 20C499 15/16/24-bit DAC" },
	{ "TKD8001", "Trident Built-In 15/16/24-bit DAC" },
	{ "TGUIDAC", "Trident Built-In 15/16/24-bit DAC" },
	{ "Integrated", "ATI Mach64 integrated 15/16/24/32-bit DAC w/clock" },
	{ "MU9C1880", "Music 9C1880"},
	{ "IMSG174", "Inmos G-174" },
	{ "STG1702", "STG1702 15/16/24-bit DAC w/cursor,pixel-mux" },
	{ "CH8398", "Chrontel 8398 15/16/24-bit DAC w/clock"},
	{ "20C408", "AT&T 20C408 15/16/24-bit DAC w/clock" },
	{ "TVP3030",
	  "TI ViewPoint3030 24-bit TrueColor DAC w/cursor,pixel-mux,clock" },
	{ "ET6000",
	  "Tseng Labs ET6000 built-in 15/16/24-bit DAC w/pixel-mux,clock" },
	{ "w30C516",
	  "IC Works w30C516 ZOOMDAC 15/16/24-bit DAC or AT&T 20C498" },
	{ "PM642x",
	  "Alliance ProMotion built-in 15/16/24-bit DAC w/clock" },
	{ "ICS5341",
	  "ICS5341 SDAC 15/16/24-bit DAC w/pixel-mux w/clock-PLL" },
	{ "ICS5301",
	  "ICS5301 GENDAC 15/16/24-bit DAC w/clock-PLL" },
	{ "MGA1064SG",
	  "Matrox Mystique built-in DAC w/clock" },
};

static CONST char *CoProc_Names[NUM_CP_TYPES][CHPS_PER_CPTYPE] = 
{
/* 8514 */	{ "8514/A (or true clone)",
		  "ATI Mach8", "ATI Mach32",
		  "Chips & Technologies 82C480 (8514/A clone)" },
/* XGA */	{ "" },
/* Mach64 */	{ "ATI Mach64" },
/* Number9 */	{ "Number Nine Imagine I128" },
/* GLINT */	{ "","GLINT 300SX","GLINT 500TX","GLINT 300SX & 500TX",
		  "GLINT Delta","GLINT Delta & 300SX","GLINT Delta & 500TX",
		  "GLINT Delta & 300SX & 500TX" },
};

void Print_SVGA_Name(Chipset)
int Chipset;
{
	int vendor = SVGA_VENDOR(Chipset);
	int chip = SVGA_CHIP(Chipset);
	printf("\tChipset: %s %s\n",
		SVGA_Names[vendor][chip],
		(PCIProbed ? "(PCI Probed)" : "(Port Probed)"));
	if ((!chip) && (~Chip_data))
	{
		printf("\t\tSignature data: %x (please report)\n", 
		       Chip_data);
	}
}

void Print_Herc_Name(Chipset)
int Chipset;
{
	int chip = HERC_CHIP(Chipset);
	printf("\tChipset: %s\n", Herc_Names[chip]);
}

void Print_RamDac_Name(RamDac)
int RamDac;
{
	printf("\tRAMDAC:  %s\n", RamDac_Names[DAC_CHIP(RamDac)].Long);
	if (RamDac & DAC_8BIT)
	{
		printf("\t\t (with 8-bit wide lookup tables)\n");
	}
	else
	{
		printf("\t\t (with 6-bit wide lookup tables ");
		printf("(or in 6-bit mode))\n");
	}
	if (RamDac & DAC_6_8_PROGRAM)
	{
		printf("\t\t (programmable for 6/8-bit wide lookup tables)\n");
	}
}

void Print_CoProc_Name(CoProc)
int CoProc;
{
	int class = COPROC_CLASS(CoProc);
	int chip = COPROC_CHIP(CoProc);
	printf("\t\tChipset: %s\n", CoProc_Names[class][chip]);
}
