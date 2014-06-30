# $XConsortium: srvflags.tcl /main/2 1996/10/25 10:21:38 kaleb $
#
#
#
#
# $XFree86: xc/programs/Xserver/hw/xfree86/XF86Setup/srvflags.tcl,v 3.4 1996/12/27 06:54:14 dawes Exp $
#
# Copyright 1996 by Joseph V. Moss <joe@XFree86.Org>
#
# See the file "LICENSE" for information regarding redistribution terms,
# and for a DISCLAIMER OF ALL WARRANTIES.
#

#
# Configuration of misc server flags
#

proc Other_create_widgets { win } {
	global ServerFlags otherZap otherZoom otherTrapSignals
	global otherXvidtune otherInpDevMods

	set w [winpathprefix $win]
	frame $w.other -width 640 -height 420 \
		-relief ridge -borderwidth 5

	frame $w.srvflags -bd 2 -relief sunken
	pack  $w.srvflags -in $w.other \
		-fill both -expand yes -padx 20m -pady 20m
	label $w.srvflags.title -text "Optional server settings\n\n\
		These should be set to reasonable values, by default,\n\
		so you probably don't need to change anything"
	pack  $w.srvflags.title -side top -fill both -expand yes
	frame $w.srvflags.line -height 2 -bd 2 -relief sunken
	pack  $w.srvflags.line -side top -fill x -pady 2m
	checkbutton $w.srvflags.zap         -indicatoron true \
		-text "Allow server to be killed with\
		hotkey sequence (Ctrl-Alt-Backspace)" \
		-variable otherZap -anchor w
	checkbutton $w.srvflags.zoom        -indicatoron true \
		-text "Allow video mode switching" \
		-variable otherZoom -anchor w
	checkbutton $w.srvflags.trapsignals -indicatoron true \
		-text "Don't Trap Signals\
			- prevents the server from exitting cleanly" \
		-variable otherTrapSignals -anchor w
	checkbutton $w.srvflags.nonlocalxvidtune -indicatoron true \
		-text "Allow video mode changes from other hosts" \
		-variable otherXvidtune -anchor w
	checkbutton $w.srvflags.nonlocalmodindev -indicatoron true \
		-text "Allow changes to keyboard and mouse settings\
		from other hosts" -variable otherInpDevMods -anchor w
	pack $w.srvflags.zap $w.srvflags.zoom $w.srvflags.trapsignals \
		-anchor w -expand yes -fill x -padx 15m
	pack $w.srvflags.nonlocalxvidtune $w.srvflags.nonlocalmodindev \
		-anchor w -expand yes -fill x -padx 15m
	set otherZap	[expr [string length $ServerFlags(DontZap)] == 0]
	set otherZoom	[expr [string length $ServerFlags(DontZoom)] == 0]
	set otherTrapSignals	[expr [string length \
				$ServerFlags(NoTrapSignals)] > 0]
	set otherXvidtune	[expr [string length \
				$ServerFlags(AllowNonLocalXvidtune)] > 0]
	set otherInpDevMods	[expr [string length \
				$ServerFlags(AllowNonLocalModInDev)] > 0]

}

proc Other_activate { win } {
	set w [winpathprefix $win]
	pack $w.other -side top -fill both -expand yes
}

proc Other_deactivate { win } {
	global ServerFlags otherZap otherZoom otherTrapSignals
	global otherXvidtune otherInpDevMods

	set w [winpathprefix $win]
	pack forget $w.other
	set ServerFlags(DontZap)  [expr $otherZap?"":"DontZap"]
	set ServerFlags(DontZoom) [expr $otherZoom?"":"DontZoom"]
	set ServerFlags(NoTrapSignals) \
		[expr $otherTrapSignals?"NoTrapSignals":""]
	set ServerFlags(AllowNonLocalXvidtune) \
		[expr $otherXvidtune?"AllowNonLocalXvidtune":""]
	set ServerFlags(AllowNonLocalModInDev) \
		[expr $otherInpDevMods?"AllowNonLocalModInDev":""]
}

proc Other_popup_help { win } {
	catch {destroy .otherhelp}
        toplevel .otherhelp -bd 5 -relief ridge
        wm title .otherhelp "Help"
	wm geometry .otherhelp +30+30
        text   .otherhelp.text
        .otherhelp.text insert 0.0 "\n\n\
		On this screen you can select the settings of various\
		server options.\n\
		These should already be set to the values most often used\n\
		and generally you don't need to make any changes.\n\n\
		If you would like more information regarding what each\
		of these do,\n\
		read the XF86Config man page."
        .otherhelp.text configure -state disabled
        button .otherhelp.ok -text "Dismiss" -command "destroy .otherhelp"
        focus  .otherhelp.ok
        pack   .otherhelp.text .otherhelp.ok
}

