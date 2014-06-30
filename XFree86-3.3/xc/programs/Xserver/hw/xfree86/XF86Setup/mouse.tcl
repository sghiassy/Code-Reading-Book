# $XConsortium: mouse.tcl /main/6 1996/10/28 05:42:22 kaleb $
#
#
#
#
# $XFree86: xc/programs/Xserver/hw/xfree86/XF86Setup/mouse.tcl,v 3.19.2.2 1997/05/21 15:02:28 dawes Exp $
#
# Copyright 1996 by Joseph V. Moss <joe@XFree86.Org>
#
# See the file "LICENSE" for information regarding redistribution terms,
# and for a DISCLAIMER OF ALL WARRANTIES.
#

#
# Mouse configuration routines
#

set mseTypeList { Microsoft MouseSystems MMSeries Logitech BusMouse
		MouseMan PS/2 MMHitTab GlidePoint IntelliMouse Xqueue OSMouse }

set msePatterns [list {tty[0-9A-Za-o]*} cua* *bm *mse* *mouse* \
                      ps*x psm* m320 pms* com* gpmdata ]
set mseDevices ""
foreach pat $msePatterns {
	if ![catch {glob /dev/$pat}] {
		eval lappend mseDevices [glob /dev/$pat]
	}
}
if [info exists Pointer(RealDev)] {
	lappend mseDevices $Pointer(RealDev)
}
set mseDevices [lrmdups $mseDevices]

proc Mouse_proto_select { win } {
	global mseType baudRate chordMiddle clearDTR clearRTS sampleRate
	global mseDeviceSelected

	set w [winpathprefix $win]
	set canv $w.mouse.mid.right.canvas
	$canv itemconfigure mbut  -fill white
	$canv itemconfigure coord -fill black
	if {[lsearch -exact {BusMouse Xqueue OSMouse PS/2} $mseType] == -1} {
		foreach rate {1200 2400 4800 9600} {
			$w.mouse.brate.$rate configure -state normal
		}
		if { ![string compare MouseMan $mseType] } {
			$w.mouse.brate.2400 configure -state disabled
			$w.mouse.brate.4800 configure -state disabled
			if { $baudRate == 2400 || $baudRate == 4800 } {
				set baudRate 1200
			}
		}
	} else {
		foreach rate {1200 2400 4800 9600} {
			$w.mouse.brate.$rate configure -state disabled
		}
	}
	if { ![string compare MMHitTab $mseType] } {
		$w.mouse.srate.title configure -text "Lines/inch"
		$w.mouse.srate.scale configure -to 1200 -tickinterval 200 \
			-resolution 20
		$w.mouse.srate.scale configure -state normal
	} else {
		$w.mouse.srate.title configure -text "Sample Rate"
		$w.mouse.srate.scale configure -to 150 -tickinterval 25 \
			-resolution 1
		if {[lsearch -exact \
				{MouseMan BusMouse Xqueue OSMouse PS/2} \
				$mseType] == -1} {
			$w.mouse.srate.scale configure -state normal
		} else {
			$w.mouse.srate.scale configure -state disabled
			set sampleRate 0
		}
	}
	if { ![string compare MouseSystems $mseType] } {
		$w.mouse.flags.dtr configure -state normal
		$w.mouse.flags.rts configure -state normal
	} else {
		$w.mouse.flags.dtr configure -state disabled
		$w.mouse.flags.rts configure -state disabled
		set clearDTR [set clearRTS 0]
	}
	if {[lsearch -exact {Microsoft MouseMan} $mseType] == -1} {
		$w.mouse.chdmid configure -state disabled
		set chordMiddle 0
	} else {
		$w.mouse.chdmid configure -state normal
	}
	if { !$mseDeviceSelected } {
		$w.mouse.device.entry delete 0 end
		$w.mouse.device.entry insert 0 \
			[Mouse_defaultdevice $mseType]
		Mouse_setlistbox $w $w.mouse.device.list.lb
	}
#	Mouse_setsettings $win
}

proc Mouse_create_widgets { win } {
	global mseType mseDevices baudRate sampleRate mseTypeList clearDTR
	global emulate3Buttons emulate3Timeout chordMiddle clearRTS

	set w [winpathprefix $win]
	frame $w.mouse -width 640 -height 420 \
		-relief ridge -borderwidth 5
	frame $w.mouse.top
	frame $w.mouse.mid -relief sunken -borderwidth 3
	frame $w.mouse.bot
	pack $w.mouse.top -side top
	pack $w.mouse.mid -side top -fill x -expand yes
	pack $w.mouse.bot -side top

	label $w.mouse.top.title -text "Select the mouse protocol"
	frame $w.mouse.type
	pack $w.mouse.top.title $w.mouse.type -in $w.mouse.top -side top
	set i 0
	foreach Type $mseTypeList {
		set type [string tolower $Type]
		radiobutton $w.mouse.type.$type -text $Type \
			-width 12 \
			-indicatoron false \
			-variable mseType -value $Type \
			-highlightthickness 1 \
			-command [list Mouse_proto_select $win]
		grid $w.mouse.type.$type -column [expr $i % 6] \
			-row [expr $i / 6]
		incr i
	}

	frame $w.mouse.mid.left
	pack $w.mouse.mid.left -side left -fill x -fill y
	frame $w.mouse.device
	pack $w.mouse.device -in $w.mouse.mid.left -side top \
		-pady 3m -padx 3m
	label $w.mouse.device.title -text "Mouse Device"
	entry $w.mouse.device.entry -bd 2
	bind $w.mouse.device.entry <Return> \
                "[list Mouse_setlistbox $win $w.mouse.device.list.lb]; \
		focus $w.mouse.em3but"
	frame $w.mouse.device.list
	listbox $w.mouse.device.list.lb -height 3 \
		-yscroll [list $w.mouse.device.list.sb set]
	eval [list $w.mouse.device.list.lb insert end] $mseDevices
        bind  $w.mouse.device.list.lb <Return> \
                "[list Mouse_setentry $win $w.mouse.device.list.lb]; \
		focus $w.mouse.em3but"
        bind  $w.mouse.device.list.lb <ButtonRelease-1> \
                [list Mouse_setentry $win $w.mouse.device.list.lb]
	scrollbar $w.mouse.device.list.sb \
		 -command [list $w.mouse.device.list.lb yview]
 
	pack $w.mouse.device.list.lb -side left -expand yes -fill both
	pack $w.mouse.device.list.sb -side left -expand yes -fill y

	pack $w.mouse.device.title $w.mouse.device.entry \
		$w.mouse.device.list -side top -fill x

	frame $w.mouse.mid.left.buttons
	pack $w.mouse.mid.left.buttons -in $w.mouse.mid.left \
		-side top -fill x -pady 2m
	checkbutton $w.mouse.em3but -text Emulate3Buttons \
		-indicatoron no -variable emulate3Buttons \
		-command [list Mouse_set_em3but $win]
	checkbutton $w.mouse.chdmid -text ChordMiddle \
		-indicatoron no -variable chordMiddle \
		-command [list Mouse_set_chdmid $win]
	pack $w.mouse.em3but $w.mouse.chdmid -in $w.mouse.mid.left.buttons \
		-side top -fill x -padx 3m -anchor w

	frame $w.mouse.brate
	pack  $w.mouse.brate -in $w.mouse.mid.left -side top -pady 2m
	label $w.mouse.brate.title -text "Baud Rate"
	pack  $w.mouse.brate.title -side top
	frame $w.mouse.brate.left
	frame $w.mouse.brate.right
	pack  $w.mouse.brate.left $w.mouse.brate.right -side left \
		-expand yes -fill x
	foreach rate { 1200 2400 4800 9600 } {
		radiobutton $w.mouse.brate.$rate -text $rate \
			-variable baudRate -value $rate
		pack $w.mouse.brate.$rate -side top -anchor w \
			-in $w.mouse.brate.[expr $rate<4800?"left":"right"]
	}

	frame $w.mouse.flags
	pack $w.mouse.flags -in $w.mouse.mid.left -side top \
		-fill x -pady 3m
	label $w.mouse.flags.title -text Flags
	checkbutton $w.mouse.flags.dtr -text ClearDTR \
		-indicatoron no -variable clearDTR
	checkbutton $w.mouse.flags.rts -text ClearRTS \
		-indicatoron no -variable clearRTS
	pack $w.mouse.flags.title $w.mouse.flags.dtr $w.mouse.flags.rts \
		-side top -fill x -padx 3m -anchor w

	frame $w.mouse.srate
	pack $w.mouse.srate -in $w.mouse.mid -side left -fill y -expand yes
	label $w.mouse.srate.title -text "Sample Rate"
	scale $w.mouse.srate.scale -orient vertical -from 0 -to 150 \
		-tickinterval 25 -variable sampleRate -state disabled
	pack $w.mouse.srate.title -side top
	pack $w.mouse.srate.scale -side top -fill y -expand yes

	frame $w.mouse.em3tm
	pack $w.mouse.em3tm -in $w.mouse.mid -side left -fill y -expand yes
	label $w.mouse.em3tm.title -text "Emulate3Timeout"
	scale $w.mouse.em3tm.scale -orient vertical -from 0 -to 1000 \
		-tickinterval 250 -variable emulate3Timeout -resolution 5
	pack $w.mouse.em3tm.title -side top
	pack $w.mouse.em3tm.scale -side top -fill y -expand yes

	frame $w.mouse.mid.right
	pack $w.mouse.mid.right -side left
	set canv $w.mouse.mid.right.canvas
	canvas $canv -width 2.75i -height 4i -highlightthickness 0 \
			-takefocus 0
	$canv create rectangle 0.25i 1.25i 2.50i 3.75i -fill white \
			-tag {mbut mbut4}
	$canv create rectangle 0.25i 0.25i 1.00i 1.25i -fill white \
			-tag {mbut mbut1}
	$canv create rectangle 1.00i 0.25i 1.75i 1.25i -fill white \
			-tag {mbut mbut2}
	$canv create rectangle 1.75i 0.25i 2.50i 1.25i -fill white \
			-tag {mbut mbut3}
	$canv create text 1.375i 2.25i -tag coord

	button $w.mouse.mid.right.apply -text "Apply" \
		-command [list Mouse_setsettings $win]
	pack $canv $w.mouse.mid.right.apply -side top

	label $w.mouse.bot.mesg \
		-text "Press ? or Alt-H for a list of key bindings" \
		-foreground [$w.mouse.top.title cget -foreground]
	pack $w.mouse.bot.mesg

	Mouse_getsettings $w
}

proc Mouse_activate { win } {
	global mseHelpShown

	set w [winpathprefix $win]
	pack $w.mouse -side top -fill both -expand yes

	set canv $w.mouse.mid.right.canvas
	bind $win <ButtonPress>	    [list $canv itemconfigure mbut%b -fill black]
	bind $win <ButtonRelease>   [list $canv itemconfigure mbut%b -fill white]
	bind $win <ButtonPress-4>   [list $canv itemconfigure mbut4 -fill black;
				   $canv itemconfigure coord -fill white]
	bind $win <ButtonRelease-4> [list $canv itemconfigure mbut4 -fill white;
				   $canv itemconfigure coord -fill black]
	bind $win <Motion>          [list $canv itemconfigure coord -text (%X,%Y)]

	$canv itemconfigure mbut  -fill white
	$canv itemconfigure coord -fill black

	set ifcmd {if { [string compare [focus] %s.mouse.device.entry]
			!= 0 } { %s %s } }
			
	bind $win a [format $ifcmd $w Mouse_setsettings $win ]
	bind $win b [format $ifcmd $w Mouse_nextbaudrate $win ]
	bind $win c [format $ifcmd $w $w.mouse.chdmid invoke ]
	bind $win d [format $ifcmd $w $w.mouse.flags.dtr invoke ]
	bind $win e [format $ifcmd $w $w.mouse.em3but invoke ]
	bind $win n [format $ifcmd $w Mouse_selectentry $win ]
	bind $win p [format $ifcmd $w Mouse_nextprotocol $win ]
	bind $win r [format $ifcmd $w $w.mouse.flags.rts invoke ]
	bind $win s [format $ifcmd $w Mouse_incrsamplerate $win ]
	bind $win t [format $ifcmd $w Mouse_increm3timeout $win ]
	if ![info exists mseHelpShown] {
		Mouse_popup_help $win
		set mseHelpShown yes
	}
}

proc Mouse_deactivate { win } {
	set w [winpathprefix $win]
	pack forget $w.mouse

	bind $win <ButtonPress>		""
	bind $win <ButtonRelease>	""
	bind $win <ButtonPress-4>	""
	bind $win <ButtonRelease-4>	""
	bind $win <Motion>		""

	bind $win a			""
	bind $win b			""
	bind $win c			""
	bind $win d			""
	bind $win e			""
	bind $win n			""
	bind $win p			""
	bind $win r			""
	bind $win s			""
	bind $win t			""
}

proc Mouse_popup_help { win } {
        toplevel .mousehelp -bd 5 -relief ridge
        wm title .mousehelp "Help"
	wm geometry .mousehelp +30+30
        text .mousehelp.text -takefocus 0 -width 90 -height 30
        .mousehelp.text insert end \
{ First select the protocol for your mouse using 'p', then if needed, change the device
 name.  If applicable, also set the baud rate (1200 should work).  Avoid moving the
 mouse or pressing buttons before the correct protocol has been selected.  Press 'a'
 to apply the changes and try moving your mouse around.  If the mouse pointer does
 not move properly, try a different protocol or device name.

   Once the mouse is moving properly, test that the various buttons also work correctly.
 If you have a three button mouse and the middle button does not work, try the buttons
 labeled ChordMiddle and Emulate3Buttons.

   Note: the `Logitech' protocol is only used by older Logitech mice.  Most current
 models use the `Microsoft' or `MouseMan' protocol.

       Key    Function
     ------------------------------------------------------
        a  -  Apply changes
        b  -  Change to next baud rate
        c  -  Toggle the ChordMiddle button
        d  -  Toggle the ClearDTR button
        e  -  Toggle the Emulate3button button
        n  -  Set the name of the device
        p  -  Select the next protocol
        r  -  Toggle the ClearRTS button
        s  -  Increase the sample rate
        t  -  Increase the 3-button emulation timeout
     ------------------------------------------------------
 You can also use Tab, and Shift-Tab to move around and then use Enter to activate
 the selected button.
 
 See the documentation for more information
}

        button .mousehelp.ok -text "Dismiss" -command "destroy .mousehelp"
	focus .mousehelp.ok
	.mousehelp.text configure -state disabled
        pack .mousehelp.text .mousehelp.ok
}

proc Mouse_selectentry { win } {
	set w [winpathprefix $win]
	if { [ $w.mouse.device.entry cget -state] != "disabled" } {
		focus $w.mouse.device.entry
	}
}

proc Mouse_nextprotocol { win } {
	global mseType mseTypeList

	set w [winpathprefix $win]
	set idx [lsearch -exact $mseTypeList $mseType]
	do {
		incr idx
		if { $idx >= [llength $mseTypeList] } {
			set idx 0
		}
		set mseType [lindex $mseTypeList $idx]
		set mtype [string tolower $mseType]
	} while { [$w.mouse.type.$mtype cget -state] == "disabled" }
	Mouse_proto_select $w
}

proc Mouse_nextbaudrate { win } {
	global baudRate

	set w [winpathprefix $win]
	if { [$w.mouse.brate.$baudRate cget -state] == "disabled" } {
		return
	}
	do {
		set baudRate [expr $baudRate*2]
		if { $baudRate > 9600 } {
			set baudRate 1200
		}
	} while { [$w.mouse.brate.$baudRate cget -state] == "disabled" }
}

proc Mouse_incrsamplerate { win } {
	global sampleRate

	set w [winpathprefix $win]
	if { [$w.mouse.srate.scale cget -state] == "disabled" } {
		return
	}

	set max [$w.mouse.srate.scale cget -to]
	set interval [expr [$w.mouse.srate.scale cget -tickinterval]/2.0]
	if { $sampleRate+$interval > $max } {
		set sampleRate 0
	} else {
		set sampleRate [expr $sampleRate+$interval]
	}
}

proc Mouse_increm3timeout { win } {
	global emulate3Timeout

	set w [winpathprefix $win]
	if { [$w.mouse.em3tm.scale cget -state] == "disabled" } {
		return
	}
	set max [$w.mouse.em3tm.scale cget -to]
	set interval [expr [$w.mouse.em3tm.scale cget -tickinterval]/2.0]
	if { $emulate3Timeout+$interval > $max } {
		set emulate3Timeout 0
	} else {
		set emulate3Timeout [expr $emulate3Timeout+$interval]
	}
}

proc Mouse_set_em3but { win } {
	global emulate3Buttons chordMiddle

	set w [winpathprefix $win]
	if { $emulate3Buttons } {
		$w.mouse.em3tm.scale configure -state normal
	} else {
		$w.mouse.em3tm.scale configure -state disabled
	}
	if { $chordMiddle && $emulate3Buttons } {
		$w.mouse.chdmid invoke
	}
}

proc Mouse_set_chdmid { win } {
	global emulate3Buttons chordMiddle

	set w [winpathprefix $win]
	if { $chordMiddle && $emulate3Buttons } {
		$w.mouse.em3but invoke
	}
}

proc Mouse_setsettings { win } {
	global mseType baudRate sampleRate clearDTR Pointer
	global emulate3Buttons emulate3Timeout chordMiddle clearRTS
	global mseDeviceSelected

	set w [winpathprefix $win]
	$w.mouse.bot.mesg configure -foreground black \
		-text "Applying..."
	$win configure -cursor watch
	update idletasks
	set mseDeviceSelected 1
	set msedev [$w.mouse.device.entry get]
	set em3but off
	set chdmid off
	if $emulate3Buttons {set em3but on}
	if $chordMiddle {set chdmid on}
	set flags ""
	if $clearDTR {lappend flags ClearDTR}
	if $clearRTS {lappend flags ClearRTS}
	if { "$mseType" == "MouseSystems" } {lappend flags ReOpen}
	if [string length $msedev] {
		set Pointer(RealDev) $msedev
		check_tmpdirs
		unlink $Pointer(Device)
		if [link $Pointer(RealDev) $Pointer(Device)] {
			lappend flags ReOpen
		}
	}
	check_tmpdirs
	set result [catch { eval [list xf86misc_setmouse \
		$msedev $mseType $baudRate $sampleRate \
		$em3but $emulate3Timeout $chdmid] $flags } ]
	if { $result } {
		bell -displayof $w
	} else {
		set Pointer(Protocol) $mseType
		if { [$w.mouse.brate.1200 cget -state] == "disabled" } {
			set Pointer(BaudRate) ""
		} else {
			set Pointer(BaudRate) $baudRate
		}
		if { [$w.mouse.srate.scale cget -state] == "disabled"
				|| $sampleRate == 0 } {
			set Pointer(SampleRate) ""
		} else {
			set Pointer(SampleRate) $sampleRate
		}
		set Pointer(Emulate3Buttons) [expr $emulate3Buttons?"ON":""]
		set Pointer(Emulate3Timeout) \
			[expr $emulate3Buttons?$emulate3Timeout:""]
		set Pointer(ChordMiddle) [expr $chordMiddle?"ON":""]
		set Pointer(ClearDTR) [expr $clearDTR?"ON":""]
		set Pointer(ClearRTS) [expr $clearRTS?"ON":""]
	}
	$w.mouse.bot.mesg configure \
		-text "Press ? or Alt-H for a list of key bindings" \
		-foreground [$w.mouse.top.title cget -foreground]
	$win configure -cursor top_left_arrow
}

proc Mouse_getsettings { win } {
	global mseType mseTypeList baudRate sampleRate clearDTR Pointer
	global emulate3Buttons emulate3Timeout chordMiddle clearRTS
	global mseDeviceSelected

	set w [winpathprefix $win]
	set initlist	[xf86misc_getmouse]
	set initdev	[lindex $initlist 0]
	set inittype	[lindex $initlist 1]
	set initbrate	[lindex $initlist 2]
	set initsrate	[lindex $initlist 3]
	set initem3btn	[lindex $initlist 4]
	set initem3tm	[lindex $initlist 5]
	set initchdmid	[lindex $initlist 6]
	set initflags	[lrange $initlist 7 end]

	set mseDeviceSelected 1
	if [getuid] {
	    pack forget $w.mouse.device.title
	    pack forget $w.mouse.device.entry
	    pack forget $w.mouse.device.list
	} else {
	    if { [info exists Pointer(RealDev)] } {
		$w.mouse.device.entry insert 0 $Pointer(RealDev)
	    } else {
		set default [Mouse_defaultdevice $inittype]
		if { [string length $default] } {
		    $w.mouse.device.entry insert 0 $default
		    set mseDeviceSelected 0
		} else {
		    pack forget $w.mouse.device.title
		    pack forget $w.mouse.device.entry
		    pack forget $w.mouse.device.list
		}
	    }
	}
	Mouse_setlistbox $w $w.mouse.device.list.lb
	$w.mouse.brate.$initbrate invoke

	set chordMiddle     [expr [string compare $initchdmid on] == 0]
	set emulate3Buttons [expr [string compare $initem3btn on] == 0]
	set emulate3Timeout $initem3tm
	set sampleRate      $initsrate
	set clearDTR  [expr [string first $initflags ClearDTR] >= 0]
	set clearRTS  [expr [string first $initflags ClearRTS] >= 0]

	set mtype [string tolower $inittype]
	if { $mtype == "osmouse" || $mtype == "xqueue" } {
		foreach mse $mseTypeList {
			$w.mouse.type.[string tolower $mse] \
				configure -state disabled
		}
		$w.mouse.type.$mtype  configure -state normal
	} else {
		$w.mouse.type.osmouse configure -state disabled
		$w.mouse.type.xqueue  configure -state disabled
	}
	$w.mouse.type.$mtype invoke
}

proc Mouse_setentry { win lbox } {
	global Pointer mseDeviceSelected

	set w [winpathprefix $win]
	set idx [$lbox curselection]
	if ![string length $idx] {
		return
	}
	$w.mouse.device.entry delete 0 end
	set devname [$lbox get $idx]
	$w.mouse.device.entry insert end $devname
	set Pointer(RealDev) $devname
	set mseDeviceSelected 1
}

proc Mouse_setlistbox { win lbox } {
	global Pointer mseDevices

	set w [winpathprefix $win]
	set devname [$w.mouse.device.entry get]
	if ![string length $devname] {
		return
	}
	set Pointer(RealDev) $devname
	if { [set idx [lsearch $mseDevices $devname]] != -1 } {
		$lbox selection clear 0 end
		$lbox selection set $idx
		$lbox activate $idx
		$lbox see $idx
	}
}

proc Mouse_defaultdevice { mousetype } {
	global mseDevices

	switch $mousetype {
		PS/2	 { set idx [lsearch -regexp $mseDevices \
					{/dev/p[ms].*} ] }
		BusMouse { set idx [lsearch -regexp $mseDevices \
					/dev/.*bm|/dev/mse.* ] }
		OsMouse  -
		Xqueue	 { return "" }
		default	 { set idx [lsearch -regexp $mseDevices \
					/dev/cua*|/dev/tty* ] }
	}
	if { $idx == -1 } {
		set idx 0
	}
	return [lindex $mseDevices $idx]
}

