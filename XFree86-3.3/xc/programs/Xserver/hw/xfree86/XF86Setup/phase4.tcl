# $XConsortium: phase4.tcl /main/1 1996/09/21 14:17:41 kaleb $
#
#
#
#
# $XFree86: xc/programs/Xserver/hw/xfree86/XF86Setup/phase4.tcl,v 3.8 1996/12/27 06:54:11 dawes Exp $
#
# Copyright 1996 by Joseph V. Moss <joe@XFree86.Org>
#
# See the file "LICENSE" for information regarding redistribution terms,
# and for a DISCLAIMER OF ALL WARRANTIES.
#

#
# Phase IV - Commands run after second server is started
#

if $StartServer {
	set_resource_defaults

	wm withdraw .

	create_main_window [set w .xf86setup]

	# put up a message ASAP so the user knows we're still alive
	label $w.waitmsg -text "Loading  -  Please wait..."
	pack  $w.waitmsg -expand yes -fill both
	update idletasks

	mesg "The program is running on a different virtual terminal\n\n\
		Please switch to the correct virtual terminal" info

	source $tk_library/tk.tcl
	set_default_arrow_bindings
	set msg "Congratulations, you've got a running server!\n\n"
} else {
	set msg ""
}

proc Phase4_run_xvidtune { win } {
	global Xwinhome

	exec $Xwinhome/bin/xvidtune
}

proc Phase4_nextphase { win } {
	global Confname StartServer

	set w [winpathprefix $win]
	set saveto [$w.saveto.entry get]
	check_tmpdirs
	writeXF86Config $Confname-3 -displayof $w -realdevice
	set backupmsg ""
	if [file exists $saveto] {
	    if {[catch {exec mv $saveto $saveto.bak} ret] != 0} {
		bell
		$w.mesg configure -text \
		    "Unable to backup $saveto as $saveto.bak!\n\
		    The configuration has not been saved!\n\
		    Try again, with a different file name"
		return
	    }
	    set backupmsg "A backup of the previous configuration has\n\
		been saved to the file $saveto.bak"
	}
	if {[catch {exec cp $Confname-3 $saveto} ret] != 0} {
	    bell
            $w.mesg configure -text \
		"Unable to save the configuration to\n\
		the file $saveto.\n\n\
		Try again, with a different file name"
	    return
	}
	$w.text configure -text \
		"The configuration has been completed.\n\n\
		$backupmsg"
	pack forget $w.buttons $w.mesg $w.saveto
	if $StartServer {
		set cmd {mesg "Just a moment..." info; shutdown}
	} else {
		set cmd {shutdown;source $XF86Setup_library/phase5.tcl}
	}
	button $w.okay -text "Okay"  -command $cmd
	pack   $w.text $w.okay -side top
	focus  $w.okay
}

label  $w.text -text " $msg\
	You can now run xvidtune to adjust your display settings,\n\
	if you want to change the size or placement of the screen image\n\n\
	If not, go ahead and exit\n\n\n\
	If you choose to save the configuration, a backup copy will be\n\
	made, if the file already exists"
frame  $w.saveto
label  $w.saveto.title -text "Save configuration to:"
entry  $w.saveto.entry -bd 2 -width 40
pack   $w.saveto.title $w.saveto.entry -side left
if [getuid] {
	if [info exists env(HOME)] {
		$w.saveto.entry insert end $env(HOME)/XF86Config
	} else {
		$w.saveto.entry insert end /tmp/XF86Config.$PID
	}
} else {
	$w.saveto.entry insert end $ConfigFile
}
label  $w.mesg -text ""
frame  $w.buttons
button $w.buttons.xvidtune -text "Run xvidtune" \
	-command [list Phase4_run_xvidtune $w]
button $w.buttons.save -text "Save the configuration and exit" \
	-command [list Phase4_nextphase $w]
button $w.buttons.abort -text "Abort - Don't save the configuration" \
	-command "clear_scrn;puts stderr Aborted;shutdown 1"
pack   $w.buttons.xvidtune $w.buttons.save $w.buttons.abort -side top \
	-pady 5m -fill x

catch {destroy $w.waitmsg}
pack    $w.text $w.saveto $w.buttons $w.mesg -side top -pady 8m
focus   $w.buttons.save

