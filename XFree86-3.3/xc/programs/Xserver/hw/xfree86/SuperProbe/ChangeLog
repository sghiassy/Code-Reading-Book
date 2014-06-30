Version 0.1 - March 25, 1993
	- Initial delivery to XFree86 beta-team in 1.2A
	- Handles ident of MDA,CGA,MCGA,EGA,VGA,Hercules, and lots of SVGA
	- Handles many RAMDACs
	- Requires ANSI C

Version 0.2 - April 17, 1993
	- Delivery to XFree86 beta-team in 1.2B
	- Modified to search out BIOS base address (don't assume it's at
	  0xC0000).
	- Add Trident 8900CL, Tseng ET4000/W32

Version 0.3 - May 7, 1993
	- Delivery to XFree86 beta-team in 1.2C
	- Allow compilation with K&R C compilers
	- Add support for Minix
	- Add -noprobe option to skip certain chipsets.
	- Cirrus has built-in RAMDAC.  Test for it, since the RAMDAC probe
	  hangs machines with Cirrus boards.
	- #ifdef out C&T probe - it just seems to be TOO evil for words.

Version 0.4 - May 24, 1993
	- Delivery to XFree86 beta team for 1.2D
	- Add support for Mach
	- Add Compaq QVision/1280, fix up Cirrus and NCR

Version 0.5 - June 1, 1993
	- Final delivery for XFree86 1.3
	- Change ET3000 probe
	- Update docs regarding MicroChannel

Version 0.6 - June 28, 1993
	- Redo C&T probe function

Version 0.7 -
	- Update RAMDAC probes to check 6/8-bit wide lookup tables and to
	  identify the different AT&T RAMDACs.
	- Add support for 8514/A, ATI Mach8 and ATI Mach32 detection.

Version 0.8 - 
	- Update the S3 probes with all currently-known information.
	- Bug fixes from Holger Veit

Version 0.9 - 
	- Fix coprocessor ID code.
	- Add Cirrus 5428, correct Cirrus built-in RAMDACs
	- Update OS_Mach.c for Mach 3.0
	- Even MORE S3 rev codes

Version 0.91
	- Add ATI Ultra/XLR (I think...)
	- Update ATIMach.c to not read BIOS so an 8514 Ultra is detected.
	- Add -no_bios flag to disallow BIOS reading and assume EGA/VGA
	- Update Oak probe mechanism and add 057 check
	- Update WD/Paradise probe to distinguish 90C2X chipsets

Version 0.92
	- Fix broken ATIMach.c (infinite loops are a bad thing :->)
	- Add probe for Bt485 RAMDAC on S3 boards
	- Rearrange chipset numbering so that the Unknown values are
	  always index 0.  Add support for printing out a signature for
	  Unknown chipsets.

Version 1.0
	- Add in Cirrus 62x5, 642x chipsets
	- Call it "done" for XFree86 2.0

Version 1.1
	- Add AT&T 20C505/504 checks to the RAMDAC probe
	- Add Solaris x86 support (from David Holland <davidh@dorite.use.com>)
	- Add -no_dac option to skip RAMDAC probe, in case it's nuking things.
	- Long and short RamDac names.
	- If the video BIOS can't be located, don't fail.  Assume that there's
	  no EGA/VGA, and probe for old hardware.
	- Put architecture in place for doing installed memory probes.

Version 1.2 - Jan 2, 1994
	- More work on memory probes.  Add support for probing memory
	  by mapping 0xA0000-0xAFFFF and switching banks.  Support
	  memory probe via this mechanism for Trident and ET4000AX.
	- Add probes for Trident 9200CXr, 9400CXi.
	- Add support for USL-style VTs on bsd-du-jour (from J"org Wunsch).
	- Add -no_mem option to skip memory probe

Version 2.0 - Feb 27, 1994
	- Add memory probes for everything that can be accomplished by
	  reading registers.  Nothing is currently using the physical
	  memory probe.
	- Bazillions of changes from vgadoc3.zip (thanks, Finn).  Added
	  HMC, UMC, Weitek probes.  Added more chipsets for Cirrus,
	  NCR, Oak, RealTek, S3, Trident, Tseng, Video7, WD.  Made some
	  changes to the probes for ATI & Compaq.

Version 2.1 - April 13, 1994
	- Small updates

Version 2.2 - August 28, 1994
	- More small updates

Version 2.3 - November 18, 1994
	- Add support for ATI Mach64.
