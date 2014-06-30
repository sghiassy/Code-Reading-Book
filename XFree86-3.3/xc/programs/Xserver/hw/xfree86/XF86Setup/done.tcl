# $XConsortium: done.tcl /main/4 1996/10/25 10:21:11 kaleb $
#
#
#
#
# $XFree86: xc/programs/Xserver/hw/xfree86/XF86Setup/done.tcl,v 3.7 1996/12/27 06:54:02 dawes Exp $
#
# Copyright 1996 by Joseph V. Moss <joe@XFree86.Org>
#
# See the file "LICENSE" for information regarding redistribution terms,
# and for a DISCLAIMER OF ALL WARRANTIES.
#

#
# Routines run to end the main configuration phase (phase 2)
#

proc Done_create_widgets { win } {
	set w [winpathprefix $win]
	frame $w.done -width 640 -height 420 \
		-relief ridge -borderwidth 5
	frame $w.done.pad -relief raised -bd 3
	pack  $w.done.pad -padx 20 -pady 15 -expand yes
	label $w.done.pad.text
	$w.done.pad.text configure -text "\n\n\
		If you've finished configuring everything press the\n\
		Okay button to start the X server using the\
		configuration you've selected.\n\n\
		If you still wish to configure some things,\n\
		press one of the buttons at the top and then\n\
		press \"Done\" again, when you've finished."
	pack  $w.done.pad.text -fill both -expand yes
	button $w.done.pad.okay -text "Okay" \
		-command [list Done_nextphase $w]
	pack  $w.done.pad.okay -side bottom -pady 10m
	focus $w.done.pad.okay
}

proc Done_activate { win } {
	set w [winpathprefix $win]
	pack $w.done -side top -fill both -expand yes
	focus $w.done.pad.okay
}

proc Done_deactivate { win } {
	set w [winpathprefix $win]
	pack forget $w.done
}

proc Done_popup_help { win } {
	catch {destroy .donehelp}
	toplevel .donehelp -bd 5 -relief ridge
	wm title .donehelp "Help"
	wm geometry .donehelp +30+30
	text .donehelp.text
	.donehelp.text insert 0.0 "\n\n\
		If you've finished configuring everything, select the\
		'Okay' button.\n\n\
		If there are still some configuration screens you\
		have not completed,\n\
		pick the appropriate button from the row across the top\
		and then press\n\
		'Done' again, when you've finished all of the\
		configuration screens."
	.donehelp.text configure -state disabled
	button .donehelp.ok -text "Dismiss" -command "destroy .donehelp"
	focus .donehelp.ok
	pack .donehelp.text .donehelp.ok
}

proc Done_execute { win } {
	global CfgSelection

	set w [winpathprefix $win]
	set CfgSelection Done
	config_select $w
}

proc Done_nextphase { win } {
	global StartServer XF86Setup_library env

	set w [winpathprefix $win]
	if $StartServer {
		mesg "Just a moment..." info
		catch {destroy .}
		catch {server_running -close $env(DISPLAY)}
		save_state
	} else {
		destroy $w.menu $w.done $w.buttons
		uplevel #0 source $XF86Setup_library/phase4.tcl
	}
}

