XCOMM $XFree86: xc/programs/Xserver/hw/xfree86/XF86Conf.cpp,v 3.29 1996/12/23 06:30:30 dawes Exp $
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
XCOMM $XConsortium: XF86Conf.cpp /main/22 1996/10/23 11:43:51 kaleb $

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
XCOMM lines below (which are the defaults).  For example, for a non-U.S.
XCOMM keyboard, you will probably want to use:
XCOMM    XkbModel    "pc102"
XCOMM If you have a Microsoft Natural keyboard, you can use:
XCOMM    XkbModel    "microsoft"
XCOMM
XCOMM Then to change the language, change the Layout setting.
XCOMM For example, a german layout can be obtained with:
XCOMM    XkbLayout   "de"
XCOMM or:
XCOMM    XkbLayout   "de"
XCOMM    XkbVariant  "nodeadkeys"
XCOMM
XCOMM If you'd like to switch the positions of your capslock and
XCOMM control keys, use:
XCOMM    XkbOptions  "ctrl:swapcaps"


XCOMM These are the default XKB settings for XFree86
XCOMM    XkbRules    "xfree86"
XCOMM    XkbModel    "pc101"
XCOMM    XkbLayout   "us"
XCOMM    XkbVariant  ""
XCOMM    XkbOptions  ""

EndSection


XCOMM **********************************************************************
XCOMM Pointer section
XCOMM **********************************************************************

Section "Pointer"

    Protocol	"Microsoft"
    Device	MOUSEDEV

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

XCOMM **********************************************************************
XCOMM Graphics device section
XCOMM **********************************************************************

XCOMM Any number of graphics device sections may be present

Section "Device"
    Identifier	"Generic VGA"
    VendorName	"Unknown"
    BoardName	"Unknown"
    Chipset	"generic"
XCOMM    VideoRam	256
XCOMM    Clocks	25.2 28.3
EndSection

Section "Device"
    # SVGA server auto-detected chipset
    Identifier	"Generic SVGA"
    VendorName	"Unknown"
    BoardName	"Unknown"
EndSection

XCOMM Section "Device"
XCOMM    Identifier	"Any Trident TVGA 9000"
XCOMM    VendorName	"Trident"
XCOMM    BoardName	"TVGA 9000"
XCOMM    Chipset	"tvga9000"
XCOMM    VideoRam	512
XCOMM    Clocks	25 28 45 36 57 65 50 40 25 28 0 45 72 77 80 75
XCOMM EndSection

XCOMM Section "Device"
XCOMM    Identifier	"Actix GE32+ 2MB"
XCOMM    VendorName	"Actix"
XCOMM    BoardName	"GE32+"
XCOMM    Ramdac	"ATT20C490"
XCOMM    Dacspeed	110
XCOMM    Option	"dac_8_bit"
XCOMM    Clocks	 25.0  28.0  40.0   0.0  50.0  77.0  36.0  45.0
XCOMM    Clocks	130.0 120.0  80.0  31.0 110.0  65.0  75.0  94.0
XCOMM EndSection

XCOMM Section "Device"
XCOMM    Identifier	"Hercules mono"
XCOMM EndSection

XCOMM **********************************************************************
XCOMM Screen sections
XCOMM **********************************************************************

XCOMM The colour SVGA server

Section "Screen"
    Driver	"svga"
    Device	"Generic SVGA"
    Monitor	"Generic Monitor"
    DefaultColorDepth 8
    Subsection "Display"
        Depth	    8
        Modes	    "640x480"
        ViewPort    0 0
        Virtual     800 600
    EndSubsection
EndSection

XCOMM The 16-colour VGA server

Section "Screen"
    Driver	"vga16"
    Device	"Generic VGA"
    Monitor	"Generic Monitor"
    Subsection "Display"
        Modes	    "640x480"
        ViewPort    0 0
        Virtual     800 600
    EndSubsection
EndSection

XCOMM The Mono server

Section "Screen"
    Driver	"vga2"
    Device	"Generic VGA"
    Monitor	"Generic Monitor"
    Subsection "Display"
        Modes	    "640x480"
        ViewPort    0 0
        Virtual     800 600
    EndSubsection
EndSection

XCOMM The hercules driver in the Mono and VGA16 servers
XCOMM
XCOMM Section "Screen"
XCOMM     Driver	"mono"
XCOMM     Device	"Hercules Mono"
XCOMM     Monitor	"Generic Monitor"
XCOMM     Subsection "Display"
XCOMM     EndSubsection
XCOMM EndSection

XCOMM The accelerated servers (S3, Mach32, Mach8, 8514, P9000, AGX, W32)

XCOMM Section "Screen"
XCOMM     Driver	"accel"
XCOMM     Device	"Actix GE32+ 2MB"
XCOMM     Monitor	"Generic Monitor"
XCOMM     DefaultColorDepth 8
XCOMM     Subsection  "Display"
XCOMM         Depth	    8
XCOMM         Modes	    "640x480"
XCOMM         ViewPort    0 0
XCOMM         Virtual	    1280 1024
XCOMM     EndSubsection
XCOMM     SubSection "Display"
XCOMM         Depth	    16
XCOMM         Weight	    565
XCOMM         Modes	    "640x480"
XCOMM         ViewPort    0 0
XCOMM         Virtual	    1024 768
XCOMM     EndSubsection
XCOMM EndSection

