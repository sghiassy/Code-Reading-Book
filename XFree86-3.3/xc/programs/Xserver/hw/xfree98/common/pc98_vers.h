/* $XConsortium: pc98_vers.h /main/7 1996/10/19 18:21:30 kaleb $ */





/* $XFree86: xc/programs/Xserver/hw/xfree98/common/pc98_vers.h,v 3.8 1996/12/23 07:06:37 dawes Exp $ */

#ifndef _PC98_VERSION_H
#define _PC98_VERSION_H

#define PC98_VGA16_BOARDS "EGC(normal mode)"

#define PC98_VGA256_BOARDS "PEGC(packed pixel mode)"

#define PC98_CIRRUS_BOARDS "WAB-S/1000/2000/EP WSR-G/E WSN-A2F WAP-2000/4000 GA-98NB2/4\n\tPCNKV/NKV2 NEC(542x,543x,544x)"

#define PC98_TGUI_BOARDS "(Under testing: NEC(9320/9660) IO-DATA(9680))"

#define PC98_S3_BOARDS	 "PW928/928G/928II/801/801+/801G PCSKB/SKB2 \n\tWAB-A/B NEC(928)\n\t(Under testing: NEC(864) PCSKB4 PW805i/964/928LB/964LB GA-968V4/PCI)"

#endif

/**************
 * Change LOG *
 **************
XFree86-3.1.2  PC98 servers

PC98 servers were included in XFree86 main tree.

PL.9	Xfree98 directory was created.
PL.10	PC98 initialize code was separated from cir_driver.c. (made cir_pc98.c)
PL.11   Imakefiles were updated. XF98Conf.cpp was added.
PL.12	WAB-EP Server was added.
	Fix problems WAB-S and NKVNEC.
PL.13	WSN-A2F Server was added.
	Fix PCI-bus probe.
	Debug code was deleted. (s3_pc98.c)
PL.14	Trident 9320/9660 make rule was added.
	PW805i initialize code was modified.	
	PW964 initialize code was added.
	XF98_NEC480 for PANIX(98) was available.
PL.15	XF98_GA968(GA-968V4/PCI) was added.

=======================================================================

XFree86(98)-3.1.2 TEST Version

PL.0    Initial version
PL.1	Changed the structure of directories.
PL.2	Test code for NEC Internel 864 was added.
PL.3	Debug code for NEC Internel 864 was added.
        Test code for generic SVGA-98 server was added.
        Some bugs was fixed.
PL.4	Initialize code for s3 sdac and vga registers was added.
PL.5	Initialize bug was fixed. (for s3 sdac)
	Power Window LB can use variable address. (for test)
PL.6	PWLB window address was modified.
PL.7	Initialize code of all registers was moved.
PL.8	NEC_480 Server corresponded to gcc-2.4.5 bugs. (for NetBSD/pc98)
	PW wrap arround bug was fixed. (in pw_mux mode)
=======================================================================

XFree86(98)-3.1.2 Beta Version

PL.0    Same as XFree86(98)-3.1.2 TEST Version PL.8

=======================================================================

XFree86(98)-3.1.2 Alpha Version

PL.0    Same as XFree86(98)-3.1.2 TEST Version PL.0

======================================================================= 

XFree86(98)-3.1.1 Release0 Version

PL.0    Same as XFree86(98)-3.1.1 TEST Version PL.15

======================================================================= 

XFree86(98)-3.1.1 TEST Version

PL.0    Initial version
PL.1    PW805i TEST code was added.(By Masato Yoshida at NIFTY-SERVE)
PL.2	s3pc98.c/s3pc98.h are added.
	TEST code for NEC WAB was added.
PL.3	PCSKB4(no memaccess) was available.
PL.4	Some bugs are corrected.
PL.5	Small small chages....
PL.6	PCNKV/2 was available.
PL.7    PW928LB was available.
        NEC-WAB & SKB4 were modified.(Drawing noise was removed.) 
        Switching VT trouble was corrected.(PCNKV) 
PL.8    NEC-WAB was modified.Memaccess on WAB-B was available.
PL.9    VGA16 was available on NetBSD/PC98.
        NEC-CIRRUS was available.
        NEC-WAB&PW928LB were modified.
PL.10   All Imakefiles are modified.
	PC98 Servers are selectable on xf86site.def.
	NEC480 server is available.
PL.11   Original sources were updated.(README.late(1995-4-20))
PL.12	broken diff
PL.13	NEC_480 server was modified.(PPM and GDC freq setting.)
PL.14	common98/Imakefile was modified.
	NEC_480 server was modified.(restoring GDC parameters)
PL.15	Optin "epsonmemwin" was modified.
	NEC_480 server was modified.(GDC parameters were changed)

======================================================================= 

XFree86(98)-3.1.1 Beta Version

PL.0    Same as XFree86(98)-3.1.1 TEST Version PL.6

=======================================================================

XFree86(98)-3.1.1 Alpha Version

PL.0    Same as XFree86(98)-3.1.1 TEST Version PL.1

======================================================================= 

XFree86(98)-3.1 TEST Version

PL.0	WAB+S3
PL.1	WAB+EGC+S3
PL.2	Lacked patches(EGC) were added.
PL.3	Identifier was added.
PL.4	CirrusColorExpandSolidFill() and 
        CirrusColorExpandWriteText() are available.
PL.5	GA-98NB2/4 support code was added.
PL.6	Some mistake corrected and suported EPSON memwin on GA-98NB.
PL.7	15/16bpp on PW928 was available.(except PW928II)	
PL.8	README.late was applied(s3.h PL 3(original's PL) & vgaFasm.h)	
        Option "pw_mux" for PW928II MUX(may be dangerous!!)
	15/16bpp and 1280x1024resolutin was available on PW928II
        Waittime for initializing LUT(s3) was increased.(100->300)
	Keybord remap was changed.(Number Pads & Func Keys use X's keymap)
PL.9    Internal version for sync. 
PL.10   WAP4000 support code was added.(WAP-2000 was not tested.)
        15/16bpp on PW801 was available.
	Option "ga98nb2" "ga98nb4" "wap2000" "wap4000" were added.
	New file compiler_pc98.h was added.
	Compile define was changed.(#define PC98_GANB -> PC98_GANB_WAP)
PL.11   NetBSD/pc98-alpha (based on NetBSD 1.0) was supported unofficially.
        Option "wap2000" & "wap4000" were changed to "wap".Autodetect is 
        available.
        GA-98NB1/C suport code was added.But it is not tested.Option "ga98nb1"
        was added.
PL.12   PW805i TEST code was added.(By Masato Yoshida at NIFTY-SERVE) 
PL.13   PW805i TEST code was updated.

======================================================================= 
XFree86(98)-3.1 RELEASE Version

PL.0    same as TEST version PL.11

======================================================================== */
