/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgaPCI.h,v 3.22.2.5 1997/05/22 14:00:45 dawes Exp $ */
/*
 * PCI Probe
 *
 * Copyright 1995  The XFree86 Project, Inc.
 *
 * A lot of this comes from Robin Cutshaw's scanpci
 *
 */
/* $XConsortium: vgaPCI.h /main/16 1996/10/25 21:22:32 kaleb $ */

#ifndef _VGA_PCI_H
#define _VGA_PCI_H

#include "xf86_PCI.h"

#define PCI_VENDOR_NCR_1	0x1000
#define PCI_VENDOR_ATI		0x1002
#define PCI_VENDOR_AVANCE	0x1005
#define PCI_VENDOR_TSENG	0x100C
#define PCI_VENDOR_WEITEK	0x100E
#define PCI_VENDOR_DIGITAL	0x1011
#define PCI_VENDOR_CIRRUS	0x1013
#define PCI_VENDOR_NCR_2	0x101A
#define PCI_VENDOR_TRIDENT	0x1023
#define PCI_VENDOR_MATROX	0x102B
#define PCI_VENDOR_CHIPSTECH	0x102C
#define PCI_VENDOR_SIS		0x1039
#define PCI_VENDOR_SGS		0x104A
#define PCI_VENDOR_NUMNINE	0x105D
#define PCI_VENDOR_UMC		0x1060
#define PCI_VENDOR_NVIDIA	0x10DE
#define PCI_VENDOR_ALLIANCE	0x1142
#define PCI_VENDOR_3DLABS	0x3D3D
#define PCI_VENDOR_S3		0x5333
#define PCI_VENDOR_ARK		0xEDD8


/* ATI */
#define PCI_CHIP_MACH32		0x4158
#define PCI_CHIP_MACH64GX	0x4758
#define PCI_CHIP_MACH64CX	0x4358
#define PCI_CHIP_MACH64CT	0x4354
#define PCI_CHIP_MACH64ET	0x4554
#define PCI_CHIP_MACH64VT	0x5654
#define PCI_CHIP_MACH64GT	0x4754

/* Avance Logic */
#define PCI_CHIP_ALG2301	0x2301

/* Tseng */
#define PCI_CHIP_ET4000_W32P_A	0x3202
#define PCI_CHIP_ET4000_W32P_B	0x3205
#define PCI_CHIP_ET4000_W32P_D	0x3206
#define PCI_CHIP_ET4000_W32P_C	0x3207
#define PCI_CHIP_ET6000		0x3208

/* Weitek */
#define PCI_CHIP_P9000		0x9001
#define PCI_CHIP_P9100		0x9100

/* Cirrus Logic */
#define PCI_CHIP_GD7548		0x0038
#define PCI_CHIP_GD5430		0x00A0
#define PCI_CHIP_GD5434_4	0x00A4
#define PCI_CHIP_GD5434_8	0x00A8
#define PCI_CHIP_GD5436		0x00AC
#define PCI_CHIP_GD5446         0x00B8
#define PCI_CHIP_GD5480         0x00BC
#define PCI_CHIP_GD5462		0x00D0
#define PCI_CHIP_GD5464		0x00D4
#define PCI_CHIP_GD5464BD	0x00D5
#define PCI_CHIP_GD5465		0x00D6
#define PCI_CHIP_GD7541		0x1204
#define PCI_CHIP_GD7542		0x1200
#define PCI_CHIP_GD7543		0x1202

/* Trident */
#define PCI_CHIP_9320		0x9320
#define PCI_CHIP_9420		0x9420
#define PCI_CHIP_9440		0x9440
#define PCI_CHIP_9660		0x9660
#if 0
#define PCI_CHIP_9680		0x9680
#define PCI_CHIP_9682		0x9682
#endif

/* Matrox */
#define PCI_CHIP_MGA2085	0x0518
#define PCI_CHIP_MGA2064	0x0519
#define PCI_CHIP_MGA1064	0x051a

/* Chips & Tech */
#define PCI_CHIP_65545		0x00D8
#define PCI_CHIP_65548		0x00DC
#define PCI_CHIP_65550		0x00E0
#define PCI_CHIP_65554		0x00E4

/* SiS */
#define PCI_CHIP_SG86C201	0x0001
#define PCI_CHIP_SG86C202	0x0002
#define PCI_CHIP_SG86C205	0x0205

/* SGS */
#define PCI_CHIP_STG2000	0x0008
#define PCI_CHIP_STG1764	0x0009

/* Number Nine */
#define PCI_CHIP_I128		0x2309
#define PCI_CHIP_I128_2		0x2339

/* NVIDIA */
#define PCI_CHIP_NV1		0x0008
#define PCI_CHIP_DAC64		0x0009

/* Alliance Semiconductor */
#define PCI_CHIP_AP6410		0x3210
#define PCI_CHIP_AP6422		0x6422
#define PCI_CHIP_AT24		0x6424

/* 3Dlabs */
#define PCI_CHIP_300SX		0x0001
#define PCI_CHIP_500TX		0x0002
#define PCI_CHIP_DELTA		0x0003
#define PCI_CHIP_PERMEDIA	0x0004

/* S3 */
#define PCI_CHIP_VIRGE		0x5631
#define PCI_CHIP_TRIO		0x8811
#define PCI_CHIP_AURORA64VP	0x8812
#define PCI_CHIP_TRIO64UVP	0x8814
#define PCI_CHIP_TRIO64V2_DXGX	0x8901
#define PCI_CHIP_PLATO_PX	0x8902
#define PCI_CHIP_VIRGE_VX	0x883D
#define PCI_CHIP_VIRGE_DXGX	0x8A01
#define PCI_CHIP_868		0x8880
#define PCI_CHIP_928		0x88B0
#define PCI_CHIP_864_0		0x88C0
#define PCI_CHIP_864_1		0x88C1
#define PCI_CHIP_964_0		0x88D0
#define PCI_CHIP_964_1		0x88D1
#define PCI_CHIP_968		0x88F0

/* ARK Logic */
#define PCI_CHIP_1000PV		0xA091
#define PCI_CHIP_2000PV		0xA099
#define PCI_CHIP_2000MT		0xA0A1
#define PCI_CHIP_2000MI		0xA0A9

/* Increase this as required */
#define MAX_DEV_PER_VENDOR 16

typedef struct vgaPCIInformation {
    int Vendor;
    int ChipType;
    int ChipRev;
    CARD32 MemBase;
    CARD32 MMIOBase;
    CARD32 IOBase;
    int Bus;
    int Card;
    int Func;
    pciConfigPtr ThisCard; /* This isn't valid after calling xf86cleanpci() */
    pciConfigPtr *AllCards; /* This isn't valid after calling xf86cleanpci() */
} vgaPCIInformation;

extern vgaPCIInformation *vgaPCIInfo;

typedef struct pciVendorDeviceInfo {
    unsigned short VendorID;
    char *VendorName;
    struct pciDevice {
	unsigned short DeviceID;
	char *DeviceName;
    } Device[MAX_DEV_PER_VENDOR];
} pciVendorDeviceInfo;

extern pciVendorDeviceInfo xf86PCIVendorInfo[];

extern vgaPCIInformation *vgaGetPCIInfo(
#if NeedFunctionPrototypes
    void
#endif
);
   
#ifdef INIT_PCI_VENDOR_INFO
pciVendorDeviceInfo xf86PCIVendorInfo[] = {
    {PCI_VENDOR_NCR_1,	"NCR",	{
				{0x0000,		NULL}}},
    {PCI_VENDOR_ATI,	"ATI",	{
				{PCI_CHIP_MACH32,	"Mach32"},
				{PCI_CHIP_MACH64GX,	"Mach64 GX"},
				{PCI_CHIP_MACH64CX,	"Mach64 CX"},
				{PCI_CHIP_MACH64CT,	"Mach64 CT"},
				{PCI_CHIP_MACH64ET,	"Mach64 ET"},
				{PCI_CHIP_MACH64VT,	"Mach64 VT"},
				{PCI_CHIP_MACH64GT,	"Mach64 GT"},
				{0x0000,		NULL}}},
    {PCI_VENDOR_AVANCE,	"Avance Logic",	{
				{PCI_CHIP_ALG2301,	"ALG2301"},
				{0x0000,		NULL}}},
    {PCI_VENDOR_TSENG,	"Tseng Labs", {
				{PCI_CHIP_ET4000_W32P_A, "ET4000W32P revA"},
				{PCI_CHIP_ET4000_W32P_B, "ET4000W32P revB"},
				{PCI_CHIP_ET4000_W32P_C, "ET4000W32P revC"},
				{PCI_CHIP_ET4000_W32P_D, "ET4000W32P revD"},
				{PCI_CHIP_ET6000,	 "ET6000"},
				{0x0000,		NULL}}},
    {PCI_VENDOR_WEITEK,	"Weitek", {
				{PCI_CHIP_P9000,	"P9000"},
				{PCI_CHIP_P9100,	"P9100"},
				{0x0000,		NULL}}},
    {PCI_VENDOR_DIGITAL, "Digital", {
				{0x0000,		NULL}}},
    {PCI_VENDOR_CIRRUS,	"Cirrus Logic", {
				{PCI_CHIP_GD5430,	"GD5430"},
				{PCI_CHIP_GD5434_4,	"GD5434"},
				{PCI_CHIP_GD5434_8,	"GD5434"},
				{PCI_CHIP_GD5436,	"GD5436"},
				{PCI_CHIP_GD5446,       "GD5446"},
				{PCI_CHIP_GD5480,       "GD5480"},
				{PCI_CHIP_GD5462,       "GD5462"},
				{PCI_CHIP_GD5464,       "GD5464"},
				{PCI_CHIP_GD5464BD,     "GD5464BD"},
				{PCI_CHIP_GD5465,       "GD5465"},
				{PCI_CHIP_GD7541,	"GD7541"},
				{PCI_CHIP_GD7542,	"GD7542"},
				{PCI_CHIP_GD7543,	"GD7543"},
				{PCI_CHIP_GD7548,	"GD7548"},
				{0x0000,		NULL}}},
    {PCI_VENDOR_NCR_2,	"NCR",	{
				{0x0000,		NULL}}},
    {PCI_VENDOR_TRIDENT, "Trident", {
				{PCI_CHIP_9320,		"TGUI 9320"},
				{PCI_CHIP_9420,		"TGUI 9420"},
				{PCI_CHIP_9440,		"TGUI 9440"},
				{PCI_CHIP_9660,		"TGUI 9660/9680/9682"},
#if 0
				{PCI_CHIP_9680,		"TGUI 9680"},
				{PCI_CHIP_9682,		"TGUI 9682"},
#endif
				{0x0000,		NULL}}},
    {PCI_VENDOR_MATROX,	"Matrox", {
				{PCI_CHIP_MGA2085,	"MGA 2085PX"},
				{PCI_CHIP_MGA2064,	"MGA 2064W"},
				{PCI_CHIP_MGA1064,	"MGA 1064SG"},
				{0x0000,		NULL}}},
    {PCI_VENDOR_CHIPSTECH, "C&T", {
				{PCI_CHIP_65545,	"65545"},
				{PCI_CHIP_65548,	"65548"},
				{PCI_CHIP_65550,	"65550"},
				{PCI_CHIP_65554,	"65554"},
				{0x0000,		NULL}}},
    {PCI_VENDOR_SIS,	"SiS",	{
				{PCI_CHIP_SG86C201,	"SG86C201"},
				{PCI_CHIP_SG86C202,	"SG86C202"},
				{PCI_CHIP_SG86C205,	"SG86C205"},
				{0x0000,		NULL}}},
    {PCI_VENDOR_SGS,	"SGS-Thomson",	{
				{PCI_CHIP_STG2000,	"STG2000"},
				{0x0000,		NULL}}},
    {PCI_VENDOR_NUMNINE, "Number Nine", {
				{PCI_CHIP_I128,		"Imagine 128"},
				{PCI_CHIP_I128_2,	"Imagine 128 II"},
				{0x0000,		NULL}}},
    {PCI_VENDOR_UMC,	"UMC",	{
				{0x0000,		NULL}}},
    {PCI_VENDOR_NVIDIA,	"NVidia",	{
				{PCI_CHIP_NV1,		"NV1"},
				{0x0000,		NULL}}},
    {PCI_VENDOR_ALLIANCE, "Alliance Semiconductor", {
				{PCI_CHIP_AP6410,	"ProMotion 6410"},
				{PCI_CHIP_AP6422,	"ProMotion 6422"},
				{PCI_CHIP_AT24,		"ProMotion AT24"},
				{0x0000,		NULL}}},
    {PCI_VENDOR_3DLABS, "3Dlabs", {
				{PCI_CHIP_300SX,	"GLINT 300SX"},
				{PCI_CHIP_500TX,	"GLINT 500TX"},
				{PCI_CHIP_DELTA,	"GLINT Delta"},
				{PCI_CHIP_PERMEDIA,	"GLINT Permedia"},
				{0x0000,		NULL}}},
    {PCI_VENDOR_S3,	"S3",	{
				{PCI_CHIP_VIRGE,	"ViRGE"},
				{PCI_CHIP_TRIO,		"Trio32/64"},
				{PCI_CHIP_AURORA64VP,	"Aurora64V+"},
				{PCI_CHIP_TRIO64UVP,	"Trio64UV+"},
				{PCI_CHIP_TRIO64V2_DXGX,"Trio64V2/DX or /GX"},
				{PCI_CHIP_PLATO_PX,	"PLATO/PX"},
				{PCI_CHIP_VIRGE_VX,	"ViRGE/VX"},
				{PCI_CHIP_VIRGE_DXGX,	"ViRGE/DX or /GX"},
				{PCI_CHIP_868,		"868"},
				{PCI_CHIP_928,		"928"},
				{PCI_CHIP_864_0,	"864"},
				{PCI_CHIP_864_1,	"864"},
				{PCI_CHIP_964_0,	"964"},
				{PCI_CHIP_964_1,	"964"},
				{PCI_CHIP_968,		"968"},
				{0x0000,		NULL}}},
    {PCI_VENDOR_ARK,	"ARK Logic", {
				{PCI_CHIP_1000PV,	"1000PV"},
				{PCI_CHIP_2000PV,	"2000PV"},
				{PCI_CHIP_2000MT,	"2000MT"},
				{PCI_CHIP_2000MI,	"2000MI"},
				{0x0000,		NULL}}},
    {0x0000,		NULL,	{
				{0x0000,		NULL}}},
};
#endif

#endif /* VGA_PCI_H */
