XCOMM $XFree86: xc/programs/Xserver/hw/xfree98/XF98Conf.cpp,v 3.13 1997/01/19 12:51:41 dawes Exp $
XCOMM
XCOMM Copyright (c) 1994 by The XFree86 Project, Inc.
XCOMM
XCOMM Permission is hereby granted, free of charge, to any person obtaining a
XCOMM copy of this software and associated documentation files (the "Software"),
XCOMM to deal in the Software without restriction, including without limitation
XCOMM the rights to use, copy, modify, merge, publish, distribute, sublicense,
XCOMM and/or sell copies of the Software, and to permit persons to whom the
XCOMM Software is furnished to do so, subject to the following conditions:
XCOMM 
XCOMM The above copyright notice and this permission notice shall be included in
XCOMM all copies or substantial portions of the Software.
XCOMM 
XCOMM THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
XCOMM IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
XCOMM FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
XCOMM THE XFREE86 PROJECT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
XCOMM WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
XCOMM OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
XCOMM SOFTWARE.
XCOMM 
XCOMM Except as contained in this notice, the name of the XFree86 Project shall
XCOMM not be used in advertising or otherwise to promote the sale, use or other
XCOMM dealings in this Software without prior written authorization from the
XCOMM XFree86 Project.
XCOMM
XCOMM $XConsortium: XF98Conf.cpp /main/9 1996/10/25 10:34:37 kaleb $

XCOMM **********************************************************************
XCOMM Refer to the XF86Config(4/5) man page for details about the format of 
XCOMM this file. This man page is installed as MANPAGE 
XCOMM **********************************************************************

XCOMM **********************************************************************
XCOMM Files section.  This allows default font and rgb paths to be set
XCOMM **********************************************************************

Section "Files"

XCOMM The location of the RGB database.  Note, this is the name of the
XCOMM file minus the extension (like ".txt" or ".db").  There is normally
XCOMM no need to change the default.

    RgbPath	RGBPATH

XCOMM Multiple FontPath entries are allowed (which are concatenated together),
XCOMM as well as specifying multiple comma-separated entries in one FontPath
XCOMM command (or a combination of both methods)

    FontPath	MISCFONTPATH
USE_75FONTS	DPI75USFONTPATH
USE_100FONTS	DPI100USFONTPATH
USE_T1FONTS	T1FONTPATH
USE_SPFONTS	SPFONTPATH
USE_75FONTS	DPI75FONTPATH
USE_100FONTS	DPI100FONTPATH

XCOMM For OSs that support Dynamically loaded modules, ModulePath can be
XCOMM used to set a search path for the modules.  This is currently supported
XCOMM for Linux ELF, FreeBSD 2.x and NetBSD 1.x.  The default path is shown
XCOMM here.

XCOMM    ModulePath	MODULEPATH

EndSection

XCOMM **********************************************************************
XCOMM Module section -- this is an optional section which is used to specify
XCOMM which dynamically loadable modules to load.  Dynamically loadable
XCOMM modules are currently supported only for Linux ELF, FreeBSD 2.x
XCOMM and NetBSD 1.x.  Currently, dynamically loadable modules are used
XCOMM only for some extended input (XInput) device drivers.
XCOMM **********************************************************************
XCOMM
XCOMM Section "Module"
XCOMM
XCOMM This loads the module for the Joystick driver
XCOMM
XCOMM Load "xf86Jstk.so"
XCOMM
XCOMM EndSection

XCOMM **********************************************************************
XCOMM Server flags section.
XCOMM **********************************************************************

Section "ServerFlags"

XCOMM Uncomment this to cause a core dump at the spot where a signal is 
XCOMM received.  This may leave the console in an unusable state, but may
XCOMM provide a better stack trace in the core dump to aid in debugging

XCOMM    NoTrapSignals

XCOMM Uncomment this to disable the <Crtl><Alt><BS> server abort sequence
XCOMM This allows clients to receive this key event.

XCOMM    DontZap

XCOMM Uncomment this to disable the <Crtl><Alt><KP_+>/<KP_-> mode switching
XCOMM sequences.  This allows clients to receive these key events.

XCOMM    DontZoom

XCOMM Uncomment this to disable tuning with the xvidtune client. With
XCOMM it the client can still run and fetch card and monitor attributes,
XCOMM but it will not be allowed to change them. If it tries it will
XCOMM receive a protocol error.

XCOMM	DisableVidModeExtension

XCOMM Uncomment this to enable the use of a non-local xvidtune client.

XCOMM	AllowNonLocalXvidtune

XCOMM Uncomment this to disable dynamically modifying the input device
XCOMM (mouse and keyboard) settings.

XCOMM	DisableModInDev

XCOMM Uncomment this to enable the use of a non-local client to
XCOMM change the keyboard or mouse settings (currently only xset).

XCOMM	AllowNonLocalModInDev

EndSection

XCOMM **********************************************************************
XCOMM Input devices
XCOMM **********************************************************************

XCOMM **********************************************************************
XCOMM Keyboard section
XCOMM **********************************************************************

Section "Keyboard"

    Protocol	"Standard"

XCOMM when using XQUEUE, comment out the above line, and uncomment the
XCOMM following line

XCOMM    Protocol	"Xqueue"

    AutoRepeat	500 5

XCOMM Let the server do the NumLock processing.  This should only be required
XCOMM when using pre-R6 clients
XCOMM    ServerNumLock

XCOMM Specifiy which keyboard LEDs can be user-controlled (eg, with xset(1))
XCOMM    Xleds      1 2 3

XCOMM To set the LeftAlt to Meta, RightAlt key to ModeShift, 
XCOMM RightCtl key to Compose, and ScrollLock key to ModeLock:

XCOMM    LeftAlt     Meta
XCOMM    RightAlt    ModeShift
XCOMM    RightCtl    Compose
XCOMM    ScrollLock  ModeLock

XCOMM To disable the XKEYBOARD extension, uncomment XkbDisable.

XCOMM XkbDisable

XCOMM To customise the XKB settings to suit your keyboard, modify the
XCOMM lines below (which are the defaults).

XCOMM These are the default XKB settings for XFree98
XCOMM Xkbkeycodes "xfree98"
XCOMM XkbTypes    "default"
XCOMM XkbCompat   "pc98"
XCOMM XkbSymbols  "nec/jp(pc98)"
XCOMM XkbGeometry "nec(pc98)"

XCOMM To specify a keymap file entry to use, use XkbKeymap.  This will
XCOMM override the other Xkb parameters described above.
XCOMM An example is:
XCOMM    XkbKeymap   "xfree98"

EndSection


XCOMM **********************************************************************
XCOMM Pointer section
XCOMM **********************************************************************

Section "Pointer"

    Protocol	"BusMouse"
XCOMM For FreeBSD(98)-2.X
FREEBSDMOUSEDEV
XCOMM For NetBSD/pc98 (based on NetBSD 1.1 or later)
NETBSDNEWMOUSEDEV
XCOMM For NetBSD/pc98 (based on NetBSD 1.0)
NETBSDOLDMOUSEDEV

XCOMM When using XQUEUE, comment out the above two lines, and uncomment
XCOMM the following line.

XCOMM    Protocol	"Xqueue"

XCOMM Baudrate and SampleRate are only for some Logitech mice

XCOMM    BaudRate	9600
XCOMM    SampleRate	150

XCOMM Emulate3Buttons is an option for 2-button Microsoft mice
XCOMM Emulate3Timeout is the timeout in milliseconds (default is 50ms)

XCOMM    Emulate3Buttons
XCOMM    Emulate3Timeout	50

XCOMM ChordMiddle is an option for some 3-button Logitech mice

XCOMM    ChordMiddle

EndSection


XCOMM **********************************************************************
XCOMM Xinput section -- this is optional and is required only if you
XCOMM are using extended input devices.  This is for example only.  Refer
XCOMM to the XF86Config man page for a description of the options.
XCOMM **********************************************************************
XCOMM
XCOMM Section "Xinput"
XCOMM    SubSection "WacomStylus"
XCOMM        Port "/dev/ttyS1"
XCOMM        DeviceName "Wacom"
XCOMM    EndSubSection
XCOMM    SubSection "WacomCursor"
XCOMM        Port "/dev/ttyS1"
XCOMM    EndSubSection
XCOMM    SubSection "WacomEraser"
XCOMM        Port "/dev/ttyS1"
XCOMM    EndSubSection
XCOMM
XCOMM    SubSection "Elographics"
XCOMM        Port "/dev/ttyS1"
XCOMM        DeviceName "Elo"
XCOMM        MinimumXPosition 300
XCOMM        MaximumXPosition 3500
XCOMM        MinimumYPosition 300
XCOMM        MaximumYPosition 3500
XCOMM        Screen 0
XCOMM        UntouchDelay 10
XCOMM        ReportDelay 10
XCOMM    EndSubSection
XCOMM   
XCOMM    SubSection "Joystick"
XCOMM        Port "/dev/joy0"
XCOMM        DeviceName "Joystick"
XCOMM        TimeOut 10
XCOMM        MinimumXPosition 100
XCOMM        MaximumXPosition 1300
XCOMM        MinimumYPosition 100
XCOMM        MaximumYPosition 1100
XCOMM        # CenterX 700
XCOMM        # CenterY 600
XCOMM        Delta 20
XCOMM    EndSubSection
XCOMM
XCOMM The Mouse Subsection contains the same type of entries as the
XCOMM standard Pointer Section (see above), with the addition of the
XCOMM DeviceName entry.
XCOMM
XCOMM    SubSection "Mouse"
XCOMM        Port "/dev/mouse2"
XCOMM        DeviceName "Second Mouse"
XCOMM        Protocol "Logitech"
XCOMM    EndSubSection
XCOMM EndSection


XCOMM **********************************************************************
XCOMM Monitor section
XCOMM **********************************************************************

XCOMM Any number of monitor sections may be present

Section "Monitor"

    Identifier	"Generic Monitor"
    VendorName	"Unknown"
    ModelName	"Unknown"

XCOMM HorizSync is in kHz unless units are specified.
XCOMM HorizSync may be a comma separated list of discrete values, or a
XCOMM comma separated list of ranges of values.
XCOMM NOTE: THE VALUES HERE ARE EXAMPLES ONLY.  REFER TO YOUR MONITOR'S
XCOMM USER MANUAL FOR THE CORRECT NUMBERS.

    HorizSync   31.5  # typical for a single frequency fixed-sync monitor

XCOMM    HorizSync	30-64         # multisync
XCOMM    HorizSync	31.5, 35.2    # multiple fixed sync frequencies
XCOMM    HorizSync	15-25, 30-50  # multiple ranges of sync frequencies

XCOMM VertRefresh is in Hz unless units are specified.
XCOMM VertRefresh may be a comma separated list of discrete values, or a
XCOMM comma separated list of ranges of values.
XCOMM NOTE: THE VALUES HERE ARE EXAMPLES ONLY.  REFER TO YOUR MONITOR'S
XCOMM USER MANUAL FOR THE CORRECT NUMBERS.

    VertRefresh 60  # typical for a single frequency fixed-sync monitor

XCOMM    VertRefresh	50-100        # multisync
XCOMM    VertRefresh	60, 65        # multiple fixed sync frequencies
XCOMM    VertRefresh	40-50, 80-100 # multiple ranges of sync frequencies

XCOMM Modes can be specified in two formats.  A compact one-line format, or
XCOMM a multi-line format.

XCOMM A generic VGA 640x480 mode (hsync = 31.5kHz, refresh = 60Hz)
XCOMM These two are equivalent

XCOMM    ModeLine "640x480" 25.175 640 664 760 800 480 491 493 525

    Mode "640x480"
        DotClock	25.175
        HTimings	640 664 760 800
        VTimings	480 491 493 525
    EndMode

XCOMM These two are equivalent

XCOMM    ModeLine "1024x768i" 45 1024 1048 1208 1264 768 776 784 817 Interlace

XCOMM    Mode "1024x768i"
XCOMM        DotClock	45
XCOMM        HTimings	1024 1048 1208 1264
XCOMM        VTimings	768 776 784 817
XCOMM        Flags		"Interlace"
XCOMM    EndMode

EndSection

Section "Monitor"
XCOMM NOTE: THE VALUES HERE ARE EXAMPLES ONLY.  REFER TO YOUR MONITOR'S
XCOMM USER MANUAL FOR THE CORRECT NUMBERS.
    Identifier	"Multi sync"
    VendorName	"IDEK"
    ModelName	"MF8615"
    Bandwidth	40
    HorizSync	24-70
    VertRefresh	50-100

    Mode "640x400"
        DotClock	28.322
        HTimings	640 664 712 808
        VTimings	400 400 402 417
    EndMode

    Mode "640x480"
        DotClock	28.0
        HTimings	640 690 752 800
        VTimings	480 496 544 560
    EndMode

    Mode "NEC480"
        DotClock	31.5
        HTimings	640 664 760 800
        VTimings	480 491 493 525
    EndMode


    Mode "800x600"
        DotClock	36.00
        HTimings	800 840 900 1000
        VTimings	600 602 610 664
    EndMode

    Mode "1024x768"
        DotClock	65.00
        HTimings	1024 1188 1210 1370
        VTimings	768   768  770  790
    EndMode

    Mode "1024x768i"
        DotClock	45.00
        HTimings	1024 1030 1230 1260
        VTimings	768   768  790  830
	Flags		"Interlace"
    EndMode

    Mode "1024x768H"
        DotClock	75.00
        HTimings	1024 1068 1184 1328
        VTimings	768   771  777  806
    EndMode

    Mode "1280x1024"
        DotClock	109.00
        HTimings	1280 1290 1680 1696
        VTimings	1024 1027 1030 1064
    EndMode


    Mode "1280x1024H"
        DotClock	110.00
        HTimings	1280 1300 1315 1700
        VTimings	1024 1027 1030 1064
    EndMode

EndSection

Section "Monitor"
XCOMM This is dummy definition. Don't edit this section.
XCOMM EGC can't take variable value.
    Identifier "PC98normal"
    VendorName "Unknown"
    ModelName "Unknown"
    BandWidth 25.2
    HorizSync 43.75
    VertRefresh 109.38
    Mode "640x400"
        DotClock        28
        HTimings        640 640 640 640
        VTimings        400 400 400 400
    EndMode

EndSection

XCOMM **********************************************************************
XCOMM Graphics device section
XCOMM **********************************************************************

XCOMM Any number of graphics device sections may be present

Section "Device"
    Identifier	"EGC16"
    VendorName	"Unknown"
    BoardName	"Unknown"
    Chipset	"generic"
XCOMM    VideoRam	256
XCOMM    Clocks	25.2 28.3
EndSection

Section "Device"
    Identifier	"NEC480"
    VendorName	"Unknown"
    BoardName	"Unknown"
    Chipset	"pegc"
    VideoRam	512
    Clocks	31.5
XCOMM Virtual resolution for 640x400
XCOMM    Clocks	28.322
EndSection

Section "Device"
    Identifier	"WAB"
    VendorName	"MELCO"
    BoardName	"WAB-S/1000/2000"
    Chipset	"clgd5426"
XCOMM    Chipset	"clgd5428"
    VideoRam	1024
XCOMM    VideoRam	2048
EndSection

Section "Device"
    Identifier	"WABEP"
    VendorName	"MELCO"
    BoardName	"WAB-EP"
    Chipset	"clgd5428"
    Option	"med_dram"
EndSection

Section "Device"
    Identifier	"WSNA2F"
    VendorName	"MELCO"
    BoardName	"WSN-A2F"
    Chipset	"clgd5434"
    Option	"med_dram"
EndSection

Section "Device"
    Identifier  "GA98NB1"
    VendorName  "IO DATA"
    BoardName   "GA-98NBI"
    ChipSet     "clgd5434"
    ClockChip   "cirrus"
    Option      "ga98nb1"
XCOMM    Option     "mmio"
XCOMM    Option     "sw_cursor"
EndSection

Section "Device"
    Identifier  "GA98NB2"
    VendorName  "IO DATA"
    BoardName   "GA-98NBII"
    ChipSet     "clgd5434"
    ClockChip   "cirrus"
    Option      "ga98nb2"
XCOMM    Option     "mmio"
XCOMM    Option     "sw_cursor"
EndSection

Section "Device"
    Identifier  "GA98NB4"
    VendorName  "IO DATA"
    BoardName   "GA-98NBIV"
    ChipSet     "clgd5434"
    ClockChip   "cirrus"
    Option      "ga98nb4"
XCOMM    Option     "mmio"
XCOMM    Option     "sw_cursor"
EndSection

Section "Device"
    Identifier	"WAP"
    VendorName	"MELCO"
    BoardName	"WAP-2000/4000"
    Chipset	"clgd5434"
XCOMM    Option	"epsonmemwin"
    Option	"wap"
EndSection

Section "Device"
    Identifier	"NKVNEC"
    VendorName	"EPSON_NEC"
    BoardName	"PCNKV/PCNKV2/NEC_CIRRUS"
    Chipset	"clgd5428"
XCOMM    Chipset	"clgd5429"
XCOMM    Option	"nec_cirrus"
XCOMM    Option	"fast_dram"
    VideoRam    1024
EndSection

Section "Device"
    Identifier	"NECTrident"
    VendorName	"NEC"
    BoardName	"NEC Trident"
XCOMM    Option	"Linear"
XCOMM    Option	"med_dram"
XCOMM    Option	"hw_cursor"
Endsection

Section "Device"
    Identifier	"GA-DRV98"
    VendorName	"IO DATA"
    BoardName	"GA-DRV/98"
XCOMM    Option	"med_dram"
XCOMM    Option	"hw_cursor"
Endsection

Section "Device"
    Identifier	"PW"
    VendorName	"canopus"
    BoardName	"PowerWndow928/801"
    Chipset	"s3_generic"
XCOMM    Chipset	"mmio_928"
    Ramdac	"sc15025"
    Dacspeed	110
    Option	"dac_8_bit"
XCOMM    Option	"epsonmemwin"
XCOMM    Option	"nomemaccess"
    ClockChip	"icd2061a"
EndSection

Section "Device"
    Identifier	"PW928II"
    VendorName	"canopus"
    BoardName	"PowerWndow928II"
XCOMM    Chipset	"mmio_928"
    Chipset	"s3_generic"
    Ramdac	"att20c505"
    Dacspeed	110
    Option	"dac_8_bit"
XCOMM    Option	"pw_mux"
    Option	"bt485_curs"
XCOMM    Option	"epsonmemwin"
XCOMM    Option	"nomemaccess"
    ClockChip	"icd2061a"
EndSection

Section "Device"
    Identifier	"PW805i"
    VendorName	"canopus"
    BoardName	"PowerWndow805i"
    Chipset	"s3_generic"
    Ramdac	"s3gendac"
    Dacspeed	110
    Option	"dac_8_bit"
XCOMM   Option	"pw805i"
XCOMM    Option	"epsonmemwin"
XCOMM    Option	"nomemaccess"
    ClockChip	"s3_sdac"
EndSection

Section "Device"
    Identifier  "PW928GLB"
    VendorName  "canopus"
    BoardName   "PowerWindow928GLB"
    Chipset     "s3_generic"
XCOMM    Chipset    "mmio_928"
    Ramdac      "sc15025"
    Dacspeed    110
    Option      "dac_8_bit"
XCOMM    Option    "pw_localbus"
XCOMM    Option    "nomemaccess"
XCOMM    Option    "nolinear"
    ClockChip   "icd2061a"
EndSection

Section "Device"
    Identifier  "PW928IILB"
    VendorName  "canopus"
    BoardName   "PowerWindow928IILB"
    Chipset     "s3_generic"
XCOMM    Chipset    "mmio_928"
    Ramdac      "att20c505"
    Dacspeed    110
    Option      "dac_8_bit"
    Option      "bt485_curs"
    Option      "pw_localbus"
XCOMM    Option     "pw_mux"
XCOMM    Option     "nomemaccess"
XCOMM    Option     "nolinear"
    ClockChip   "icd2061a"
EndSection

Section "Device"
    Identifier	"PW964LB"
    VendorName	"canopus"
    BoardName	"PowerWindow964LB"
    Chipset	"s3_generic"
    Option	"pw_localbus"
    Option     "number_nine"
    Ramdac	"ti3025"
    ClockChip	"ti3025"
    VideoRam	4096
EndSection

Section "Device"
    Identifier	"PCSKB"
    VendorName	"EPSON"
    BoardName	"PCSKB"
    Chipset	"s3_generic"
    Ramdac	"sc15025"
    Dacspeed	110
    Option	"dac_8_bit"
XCOMM    Option	"nomemaccess"
    Option	"pcskb"
    Clocks	 25.0  28.0  40.0   0.0  50.0  77.0  36.0  45.0
    Clocks	130.0 120.0  80.0  31.0 110.0  65.0  75.0  94.0
EndSection

Section "Device"
    Identifier	"PCSKB4"
    VendorName	"EPSON"
    BoardName	"PCSKB4"
    Chipset	"s3_generic"
    Ramdac	"sc15025"
XCOMM    Ramdac	"20c498"
    Dacspeed	110
    Option	"dac_8_bit"
    Option	"nomemaccess"
    Option	"pcskb4"
    Clocks	 25.0  28.0  40.0   0.0  50.0  77.0  36.0  45.0
    Clocks	130.0 120.0  80.0  31.0 110.0  65.0  75.0  94.0
EndSection

Section "Device"
    Identifier	"NECWAB"
    VendorName	"NEC"
    BoardName	"NEC WAB-A/B/928"
    Chipset	"s3_generic"
    Dacspeed	110
    Ramdac	"sc15025"
    Option	"dac_8_bit"
XCOMM    Option	"necwab"
XCOMM    Option	"nomemaccess"
    Clocks	 25.0  28.0  40.0   0.0  50.0  77.0  36.0  45.0
    Clocks	130.0 120.0  80.0  31.0 110.0  65.0  75.0  94.0
EndSection

Section "Device"
    Identifier	"NEC864"
    VendorName	"NEC"
    BoardName	"NEC 864"
    Chipset	"s3_generic"
    Option	"necwab"
    Ramdac	"s3_sdac"
    ClockChip	"s3_sdac"
EndSection

Section "Device"
    Identifier	"GA-968"
    VendorName	"IO DATA"
    BoardName	"GA-968V4/PCI"
    Chipset	"s3_generic"
XCOMM Chipset	"mmio_928"
XCOMM Option	"ga968"
Endsection

XCOMM **********************************************************************
XCOMM Screen sections
XCOMM **********************************************************************

XCOMM The colour SVGA-based PC98 server

Section "Screen"
    Driver	"svga"
    Device	"WAB"
XCOMM    Device	"WABEP"
XCOMM    Device	"NEC480"
XCOMM    Device	"WAP"
XCOMM    Device	"WSNA2F"
XCOMM    Device	"NKVNEC"
XCOMM    Device	"GA98NB1"
XCOMM    Device	"GA98NB2"
XCOMM    Device	"GA98NB4"
XCOMM    Device	"NECTrident"
XCOMM    Device	"GA-DRV98"
    Monitor	"Multi sync"
    Subsection "Display"
        Depth	    8
        Modes	    "800x600" "640x480" "1024x768"
XCOMM        Modes	    "NEC480" "640x400"
        ViewPort    0 0
XCOMM        Virtual     1024 1024
XCOMM        Virtual     1024 1022
XCOMM        Virtual      640 480
XCOMM        Virtual      640 400
        Virtual     1280 816
    EndSubsection
EndSection

XCOMM The 16-colour VGA-based PC98-EGC server

Section "Screen"
    Driver	"vga16"
    Device	"EGC16"
    Monitor	"Multi sync"
    Subsection "Display"
        Modes	    "640x400"
        ViewPort    0 0
    Visual	"PseudoColor"
    EndSubsection
EndSection

XCOMM The accelerated servers (S3-NEC,PW,PW-LB,PCSKB)

Section "Screen"
     Driver	"accel"
XCOMM     Device	"PW"
XCOMM     Device	"PW928II"
XCOMM     Device	"PW805i"
XCOMM     Device	"PW928GLB"
XCOMM     Device	"PW928IILB"
XCOMM     Device	"PW964LB"
XCOMM     Device	"GA-968"
XCOMM     Device	"PCSKB"
XCOMM     Device	"PCSKB4"
XCOMM     Device	"NECWAB"
XCOMM     Device	"NEC864"    
     Monitor	"Multi sync"
     Subsection  "Display"
         Depth	    8
XCOMM         Modes	    "1024x768i" "640x480" "800x600"
XCOMM         Modes	    "1024x768H" "640x480" "800x600"
         Modes	    "1024x768" "640x480" "800x600"
XCOMM         Modes	    "1280x1024" "1024x768" "640x480" "800x600"
XCOMM         Modes	    "1280x1024H"
         ViewPort    0 0
XCOMM         Virtual	    1280 1024
         Virtual	    1024 768
     EndSubsection
     SubSection "Display"
         Depth	    16
         Weight	    565
XCOMM         Modes	    "640x480" "800x600" "1024x768"
         Modes	    "800x600" "1024x768"
         ViewPort    0 0
         Virtual	    1024 768
     EndSubsection
EndSection
