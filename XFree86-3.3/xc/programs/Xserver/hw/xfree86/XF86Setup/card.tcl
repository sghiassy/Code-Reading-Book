# $XConsortium: card.tcl /main/5 1996/10/28 04:55:06 kaleb $
#
#
#
#
# $XFree86: xc/programs/Xserver/hw/xfree86/XF86Setup/card.tcl,v 3.12 1996/12/27 06:54:00 dawes Exp $
#
# Copyright 1996 by Joseph V. Moss <joe@XFree86.Org>
#
# See the file "LICENSE" for information regarding redistribution terms,
# and for a DISCLAIMER OF ALL WARRANTIES.
#

#
# Card configuration routines
#


proc Card_create_widgets { win } {
	global ServerList XF86Setup_library cardDevNum DeviceIDs
	global cardDetail cardReadmeWasSeen UseConfigFile

	set w [winpathprefix $win]
	set cardDevNum 0
	frame $w.card -width 640 -height 420 \
		-relief ridge -borderwidth 5
	frame $w.card.top
	pack  $w.card.top -side top -fill x -padx 5m
	if { [llength $DeviceIDs] > 1 } {
		label $w.card.title -text "Card selected:" -anchor w
		pack  $w.card.title -side left -fill x -padx 5m -in $w.card.top
		combobox $w.card.cardselect -state disabled -bd 2
		pack  $w.card.cardselect -side left -in $w.card.top
		eval [list $w.card.cardselect linsert end] $DeviceIDs
		Card_cbox_setentry $w.card.cardselect [lindex $DeviceIDs 0]
		bind $w.card.cardselect.popup.list <ButtonRelease-1> \
			"+[list Card_cardselect $win]"
		bind $w.card.cardselect.popup.list <Return> \
			"+[list Card_cardselect $win]"
	} else {
		label $w.card.title -text "Card selected: None" -anchor w
		pack  $w.card.title -side left -fill x -padx 5m -in $w.card.top
	}

	frame $w.card.list
	scrollbar $w.card.list.sb -command [list $w.card.list.lb yview] \
		-repeatdelay 1200 -repeatinterval 800
	listbox   $w.card.list.lb -yscroll [list $w.card.list.sb set] \
		-setgrid true -height 20
	bind  $w.card.list.lb <Return> \
		[list Card_selected $win $w.card.list.lb]
	bind  $w.card.list.lb <ButtonRelease-1> \
		[list Card_selected $win $w.card.list.lb]
	eval  $w.card.list.lb insert 0 [xf86cards_getlist]
	pack  $w.card.list.lb -side left -fill both -expand yes
	pack  $w.card.list.sb -side left -fill y

	image create bitmap cardpic -foreground yellow -background black \
		-file $XF86Setup_library/pics/vidcard.xbm \
		-maskfile $XF86Setup_library/pics/vidcard.msk
	label $w.card.list.pic -image cardpic
	pack  $w.card.list.pic -side left -padx 3m -pady 3m


	frame $w.card.bot -borderwidth 5
	pack  $w.card.bot -side bottom -fill x
	label $w.card.bot.message
	pack  $w.card.bot.message -side top -fill x

	frame $w.card.buttons
	pack  $w.card.buttons -side bottom -fill x

	button $w.card.readme -text "Read README file" \
		-command [list Card_display_readme $win]
	pack  $w.card.readme -side left -expand yes \
		-in $w.card.buttons

	button $w.card.modebutton -text "Detailed Setup" \
		-command [list Card_switchdetail $win]
	pack  $w.card.modebutton -side left -expand yes \
		-in $w.card.buttons

	frame $w.card.detail -bd 2 -relief sunken

	frame $w.card.server
	pack  $w.card.server -side top -fill x -in $w.card.detail
	label $w.card.server.title -text Server:
	pack  $w.card.server.title -side left
	foreach serv $ServerList {
		set lcserv [string tolower $serv]
		radiobutton $w.card.server.$lcserv -indicatoron no \
			-text $serv -variable cardServer -value $serv \
			-command [list Card_set_cboxlists $win]
		pack $w.card.server.$lcserv -anchor w -side left \
			-expand yes -fill x
	}

	frame $w.card.detail.cboxen
	pack  $w.card.detail.cboxen -side top

	frame $w.card.chipset
	pack  $w.card.chipset -side left -expand yes -fill x \
		-in $w.card.detail.cboxen -padx 5m
	label $w.card.chipset.title -text Chipset
	combobox $w.card.chipset.cbox -state disabled -bd 2
	pack  $w.card.chipset.title $w.card.chipset.cbox

	frame $w.card.ramdac
	pack  $w.card.ramdac -side left -expand yes -fill x \
		-in $w.card.detail.cboxen -padx 5m
	label $w.card.ramdac.title -text RamDac
	combobox $w.card.ramdac.cbox -state disabled -bd 2
	pack  $w.card.ramdac.title $w.card.ramdac.cbox

	frame $w.card.clockchip
	pack  $w.card.clockchip -side left -expand yes -fill x \
		-in $w.card.detail.cboxen -padx 5m
	label $w.card.clockchip.title -text ClockChip
	combobox $w.card.clockchip.cbox -state disabled -bd 2
	pack  $w.card.clockchip.title $w.card.clockchip.cbox

	set extr $w.card.extra
	frame $extr
	pack  $extr -side bottom -padx 5m \
		-fill x -expand yes -in $w.card.detail
	frame $extr.dacspeed
	pack  $extr.dacspeed -side left -fill x -expand yes
	label $extr.dacspeed.title -text "RAMDAC Max Speed"
	checkbutton $extr.dacspeed.probe -width 15 -text "Probed" \
		-variable cardDacProbe -indicator off \
		-command [list Card_dacspeed $win] \
		-highlightthickness 0
	scale $extr.dacspeed.value -variable cardDacSpeed \
		-orient horizontal -from 60 -to 300 -resolution 5
	bind  $extr.dacspeed.value <ButtonPress> \
		"set cardDacProbe 0; [list Card_dacspeed $win]"
	pack  $extr.dacspeed.title -side top -fill x -expand yes
	pack  $extr.dacspeed.probe -side top -expand yes
	pack  $extr.dacspeed.value -side top -fill x -expand yes
	frame $extr.videoram
	pack  $extr.videoram -side left -fill x -expand yes
	label $extr.videoram.title -text "Video RAM"
	pack  $extr.videoram.title -side top -fill x -expand yes
	radiobutton $extr.videoram.mprobed -indicator off -width 15 \
		-variable cardRamSize -value 0 -text "Probed" \
		-highlightthickness 0
	pack  $extr.videoram.mprobed -side top -expand yes
	frame $extr.videoram.cols
	pack  $extr.videoram.cols -side top -fill x -expand yes
	frame $extr.videoram.col1
	frame $extr.videoram.col2
	pack  $extr.videoram.col1 $extr.videoram.col2 \
		-side left -fill x -expand yes \
		-in $extr.videoram.cols
	radiobutton $extr.videoram.m256k \
		-variable cardRamSize -value 256 -text "256K" \
		-highlightthickness 0
	radiobutton $extr.videoram.m512k \
		-variable cardRamSize -value 512 -text "512K" \
		-highlightthickness 0
	radiobutton $extr.videoram.m1m \
		-variable cardRamSize -value 1024 -text "1Meg" \
		-highlightthickness 0
	radiobutton $extr.videoram.m2m \
		-variable cardRamSize -value 2048 -text "2Meg" \
		-highlightthickness 0
	radiobutton $extr.videoram.m3m \
		-variable cardRamSize -value 3072 -text "3Meg" \
		-highlightthickness 0
	radiobutton $extr.videoram.m4m \
		-variable cardRamSize -value 4096 -text "4Meg" \
		-highlightthickness 0
	radiobutton $extr.videoram.m6m \
		-variable cardRamSize -value 6144 -text "6Meg" \
		-highlightthickness 0
	radiobutton $extr.videoram.m8m \
		-variable cardRamSize -value 8192 -text "8Meg" \
		-highlightthickness 0
	pack  $extr.videoram.m256k $extr.videoram.m512k \
	      $extr.videoram.m1m $extr.videoram.m2m \
		-side top -fill x -expand yes \
		-in $extr.videoram.col1
	pack  $extr.videoram.m3m $extr.videoram.m4m \
	      $extr.videoram.m6m $extr.videoram.m8m \
		-side top -fill x -expand yes \
		-in $extr.videoram.col2

	frame $w.card.options
	pack  $w.card.options -side bottom -fill x -in $w.card.detail \
		-pady 2m

	frame $w.card.options.list
	pack  $w.card.options.list -side top
	combobox $w.card.options.list.cbox -state disabled -width 80 -bd 2
	label $w.card.options.list.title -text "Selected options:"
	$w.card.options.list.cbox.popup.list configure \
		-selectmode multiple
	pack  $w.card.options.list.title -side left
	pack  $w.card.options.list.cbox -fill x -expand yes -side left

	frame $w.card.options.text
	pack  $w.card.options.text -side top
	text  $w.card.options.text.text -yscroll [list $w.card.options.text.sb set] \
		-setgrid true -height 4 -background white
	scrollbar $w.card.options.text.sb -command \
		[list $w.card.options.text.text yview]
	label $w.card.options.text.title -text "Additional lines to\
		add to Device section of the XF86Config file:"
	pack  $w.card.options.text.title -fill x -expand yes -side top
	pack  $w.card.options.text.text -fill x -expand yes -side left
	pack  $w.card.options.text.sb -side left -fill y

	$w.card.readme configure -state disabled
	for {set idx 0} {$idx < [llength $DeviceIDs]} {incr idx} {
		set cardReadmeWasSeen($idx)	0
	}
	if { $UseConfigFile } {
		set cardDetail		std
		Card_switchdetail $win
		#$w.card.modebutton configure -state disabled
	} else {
		set cardDetail		detail
		Card_switchdetail $win
	}
}

proc Card_activate { win } {
	set w [winpathprefix $win]
	Card_get_configvars $win
	pack $w.card -side top -fill both -expand yes
}

proc Card_deactivate { win } {
	set w [winpathprefix $win]
	pack forget $w.card
	Card_set_configvars $win
}

proc Card_dacspeed { win } {
	global cardDacSpeed cardDacProbe

	set w [winpathprefix $win]
	if { $cardDacProbe } {
		#$w.card.extra.dacspeed.probe configure -text "Probed: Yes"
		$w.card.extra.dacspeed.value configure \
			-foreground [option get $w.card background *] ;# -state disabled
	} else {
		#$w.card.extra.dacspeed.probe configure -text "Probed: No"
		$w.card.extra.dacspeed.value configure \
			-foreground [option get $w.card foreground *] -state normal
	}
}

proc Card_switchdetail { win } {
	global cardDetail cardDevNum

	set w [winpathprefix $win]
	if { $cardDetail == "std" } {
		set cardDetail detail
		$w.card.modebutton configure -text "Card List"
		pack forget $w.card.list
		pack $w.card.detail -expand yes -side top -fill both
		$w.card.bot.message configure -text \
			"First make sure the correct server is selected,\
			then make whatever changes are needed\n\
			If the Chipset, RamDac, or ClockChip entries\
			are left blank, they will be probed"
	} else {
		set cardDetail std
		$w.card.modebutton configure -text "Detailed Setup"
		pack forget $w.card.detail
		pack $w.card.list   -expand yes -side top -fill both
		$w.card.bot.message configure -text \
			"Select your card from the list.\n\
			If your card is not listed,\
			click on the Detailed Setup button"
	}
}

proc Card_cbox_setentry { cb text } {
	$cb econfig -state normal
	$cb edelete 0 end
	if [string length $text] {
		$cb einsert 0 $text
	}
	$cb econfig -state disabled
	set cblist [$cb lget 0 end]
	if { [string match *.options.cbox $cb] } {
		$cb lselection clear 0 end
		foreach option [split $text ,] {
			set idx [lsearch $cblist $option]
			if { $idx != -1 } {
				$cb see $idx
				$cb lselection set $idx
				$cb activate $idx
			}
		}
	} else {
		set idx [lsearch $cblist $text]
		if { $idx != -1 } {
			$cb see $idx
			$cb lselection clear 0 end
			$cb lselection set $idx
			$cb activate $idx
		}
	}
}

proc Card_selected { win lbox } {
	global cardServer cardReadmeWasSeen cardDevNum

	set w [winpathprefix $win]
	if { ![string length [$lbox curselection]] } return
	set cardentry	[$lbox get [$lbox curselection]]
	set carddata	[xf86cards_getentry $cardentry]
	$w.card.title configure -text "Card selected: $cardentry"
	$w.card.options.text.text delete 0.0 end
	if { [lsearch [lindex $carddata 7] UNSUPPORTED] == -1 } {
	    #Card_cbox_setentry $w.card.chipset.cbox  [lindex $carddata 1]
	    set cardServer			      [lindex $carddata 2]
	    Card_cbox_setentry $w.card.ramdac.cbox    [lindex $carddata 3]
	    Card_cbox_setentry $w.card.clockchip.cbox [lindex $carddata 4]
	    $w.card.options.text.text insert 0.0      [lindex $carddata 6]
	    if { $cardReadmeWasSeen($cardDevNum) } {
	        $w.card.bot.message configure -text \
		    "That's all there is to configuring your card\n\
		    unless you would like to make changes to the\
		    standard settings (by pressing Detailed Setup)"
	    } else {
	        $w.card.bot.message configure -text \
		    "That's probably all there is to configuring\
		    your card, but you should probably check the\n\
		    README to make sure. If any changes are needed,\
		    press the Detailed Setup button"
	    }
	} else {
	    set cardServer			      VGA16
	    Card_cbox_setentry $w.card.chipset.cbox   generic
	    Card_cbox_setentry $w.card.ramdac.cbox    ""
	    Card_cbox_setentry $w.card.clockchip.cbox ""
	    $w.card.bot.message configure -text \
		"You have selected a card which is not fully\
		supported by XFree86, however all of the proper\n\
		configuration options have been set such that it\
		should work in standard VGA mode"
	}
	Card_set_cboxlists $win cardselected
}

proc Card_set_cboxlists { win args } {
	global CardChipSets CardRamDacs CardClockChips cardServer
	global CardReadmes cardReadmeWasSeen CardOptions Xwinhome

	set w [winpathprefix $win]
	if ![file exists $Xwinhome/bin/XF86_$cardServer] {
	    if ![string compare $args cardselected] {
		$w.card.bot.message configure -text \
		    "*** The server required by your card is not\
		    installed!  Please abort, install the\
		    $cardServer server as\n\
		    $Xwinhome/bin/XF86_$cardServer and\
		    run this program again ***"
	    } else {
		$w.card.bot.message configure -text \
		    "*** The selected server is not\
		    installed!  Please abort, install the\
		    $cardServer server as\n\
		    $Xwinhome/bin/XF86_$cardServer and\
		    run this program again ***"
	    }
	    bell
	}
	if { [llength $CardReadmes($cardServer)] > 0 } {
		$w.card.readme configure -state normal
	} else {
		$w.card.readme configure -state disabled
	}
	$w.card.chipset.cbox ldelete 0 end
	if [llength $CardChipSets($cardServer)] {
		$w.card.chipset.cbox.button configure -state normal
		$w.card.chipset.cbox linsert end "<Probed>"
		eval [list $w.card.chipset.cbox linsert end] \
			$CardChipSets($cardServer)
	} else {
		$w.card.chipset.cbox.button configure -state disabled
	
	}
	set chipset [$w.card.chipset.cbox eget]
	if { [string length $chipset] && [lsearch \
			$CardChipSets($cardServer) $chipset] < 0} {
		Card_cbox_setentry $w.card.chipset.cbox ""
	}

	$w.card.ramdac.cbox ldelete 0 end
	if [llength $CardRamDacs($cardServer)] {
		$w.card.ramdac.cbox.button configure -state normal
		$w.card.ramdac.cbox linsert end "<Probed>"
		eval [list $w.card.ramdac.cbox linsert end] \
			$CardRamDacs($cardServer)
	} else {
		$w.card.ramdac.cbox.button configure -state disabled
	}
	set ramdac [$w.card.ramdac.cbox eget]
	if { [string length $ramdac] && [lsearch \
			$CardRamDacs($cardServer) $ramdac] < 0} {
		Card_cbox_setentry $w.card.ramdac.cbox ""
	}


	$w.card.clockchip.cbox ldelete 0 end
	if [llength $CardClockChips($cardServer)] {
		$w.card.clockchip.cbox.button configure -state normal
		$w.card.clockchip.cbox linsert end "<Probed>"
		eval [list $w.card.clockchip.cbox linsert end] \
			$CardClockChips($cardServer)
	} else {
		$w.card.clockchip.cbox.button configure -state disabled
	}
	set clockchip [$w.card.clockchip.cbox eget]
	if { [string length $clockchip] && [lsearch \
			$CardClockChips($cardServer) $clockchip] < 0} {
		Card_cbox_setentry $w.card.clockchip.cbox ""
	}

	$w.card.options.list.cbox ldelete 0 end
	if [llength $CardOptions($cardServer)] {
		$w.card.options.list.cbox.button configure -state normal
		eval [list $w.card.options.list.cbox linsert end] \
			$CardOptions($cardServer)
	} else {
		$w.card.options.list.cbox.button configure -state disabled
	}
	set options ""
	foreach option [split [$w.card.options.list.cbox eget] ,] {
		if { [string length $option] && [lsearch \
			$CardOptions($cardServer) $option] != -1} {
		    lappend options $option
		}
	}
	Card_cbox_setentry $w.card.options.list.cbox [join $options ,]
}

proc Card_display_readme { win } {
	global cardServer CardReadmes cardReadmeWasSeen
	global cardDevNum Xwinhome

	set w [winpathprefix $win]
	catch {destroy .cardreadme}
        toplevel .cardreadme -bd 5 -relief ridge
	wm title .cardreadme "Chipset Specific README"
	wm geometry .cardreadme +30+30
	frame .cardreadme.file
        text .cardreadme.file.text -setgrid true \
		-xscroll ".cardreadme.horz.hsb set" \
		-yscroll ".cardreadme.file.vsb set"
	foreach file $CardReadmes($cardServer) {
		set fd [open $Xwinhome/lib/X11/doc/$file r]
		.cardreadme.file.text insert end [read $fd]
		close $fd
	}
        .cardreadme.file.text configure -state disabled
	frame .cardreadme.horz
	scrollbar .cardreadme.horz.hsb -orient horizontal \
		-command ".cardreadme.file.text xview" \
		-repeatdelay 1200 -repeatinterval 800
	scrollbar .cardreadme.file.vsb \
		-command ".cardreadme.file.text yview" \
		-repeatdelay 1200 -repeatinterval 800
        button .cardreadme.ok -text "Dismiss" -command "destroy .cardreadme"
	focus .cardreadme.ok
        pack .cardreadme.file -side top -fill both
        pack .cardreadme.file.text -side left
        pack .cardreadme.file.vsb -side left -fill y
	#update idletasks
	#.cardreadme.horz configure -width [winfo width .cardreadme.file.text] \
		-height [winfo width .cardreadme.file.vsb]
        #pack propagate .cardreadme.horz 0
        #pack .cardreadme.horz -side top -anchor w
        #pack .cardreadme.horz.hsb -fill both
        pack .cardreadme.ok -side bottom
	set cardReadmeWasSeen($cardDevNum) 1
}

proc Card_cardselect { win } {
	global cardDevNum

	set w [winpathprefix $win]
	if { ![string length [$w.card.cardselect curselection]] } return
	Card_set_configvars $win
	set cardDevNum [$w.card.cardselect curselection]
	Card_get_configvars $win
}

proc Card_set_configvars { win } {
	global DeviceIDs cardServer ServerList cardDevNum
	global AccelServerList CardChipSets CardRamDacs CardClockChips
	global cardDacSpeed cardDacProbe cardRamSize UseConfigFile

	set w [winpathprefix $win]
	set devid [lindex $DeviceIDs $cardDevNum]
	global Device_$devid

	set Device_${devid}(Server)	$cardServer
	set Device_${devid}(Chipset)	[$w.card.chipset.cbox eget]
	set Device_${devid}(Ramdac)	[$w.card.ramdac.cbox eget]
	set Device_${devid}(ClockChip)	[$w.card.clockchip.cbox eget]
	set Device_${devid}(ExtraLines)	[$w.card.options.text.text get 0.0 end]
	set Device_${devid}(Options)	[split [$w.card.options.list.cbox eget] ,]
	if {[llength $DeviceIDs] == 1} {
		set Device_${devid}(BoardName)	[string range \
			[$w.card.title cget -text] 15 end]
	}
	if { $UseConfigFile } {
	    if { $cardRamSize } {
	        set Device_${devid}(VideoRam)	$cardRamSize
	    } else {
	        set Device_${devid}(VideoRam)	""
	    }
	    if { $cardDacProbe } {
	        set Device_${devid}(DacSpeed)	""
	    } else {
	        set Device_${devid}(DacSpeed)	[expr $cardDacSpeed*1000]
	    }
	}
}

proc Card_get_configvars { win } {
	global DeviceIDs cardServer ServerList cardDevNum
	global AccelServerList CardChipSets CardRamDacs CardClockChips
	global cardDacSpeed cardDacProbe cardRamSize UseConfigFile

	set w [winpathprefix $win]
	set devid [lindex $DeviceIDs $cardDevNum]
	global Device_$devid

	set cardServer		[set Device_${devid}(Server)]
	Card_cbox_setentry $w.card.chipset.cbox [set Device_${devid}(Chipset)]
	Card_cbox_setentry $w.card.ramdac.cbox [set Device_${devid}(Ramdac)]
	Card_cbox_setentry $w.card.clockchip.cbox [set Device_${devid}(ClockChip)]
	$w.card.options.text.text delete 0.0 end
	$w.card.options.text.text insert 0.0 [set Device_${devid}(ExtraLines)]
	Card_cbox_setentry $w.card.options.list.cbox \
		[join [set Device_${devid}(Options)] ,]
	Card_set_cboxlists $win
	if { $UseConfigFile } {
	    set ram [set Device_${devid}(VideoRam)]
	    if { [string length $ram] > 0 } {
	        set cardRamSize	$ram
	    } else {
	        set cardRamSize	0
	    }
	    set speed [set Device_${devid}(DacSpeed)]
	    if { [string length $speed] > 0 } {
	        set cardDacSpeed	[expr int($speed/1000)]
	        set cardDacProbe	0
	    } else {
	        set cardDacSpeed	60
	        set cardDacProbe	1
	    }
	    Card_dacspeed $win
	}
}

proc Card_popup_help { win } {
	global cardDetail

	catch {destroy .cardhelp}
        toplevel .cardhelp -bd 5 -relief ridge
        wm title .cardhelp "Help"
	wm geometry .cardhelp +30+30
        text .cardhelp.text
	if { $cardDetail == "std" } {
	    .cardhelp.text insert 0.0 "\n\n\
		Pick your card from the list.\n\n\
		If there are README files that may pertain to your card\n\
		the 'Read README file' button will then be usable (i.e. not\
		greyed out).\n\
		Please read them.\n\n\
		If your card is not in the list, or if there are any\
		special settings\n\
		listed in the README file as required by your card, you\
		can press the\n\
		'Detailed Setup' button to make sure that\
		they have been selected."
	} else {
	    global DeviceIDs
	    if { [llength $DeviceIDs] > 1 } {
		.cardhelp.text insert 0.0 \
		    " If you picked a card from the Card list, at least most\
		    things should\n\
		    already be set properly.\n\n"
	    } else {
		.cardhelp.text insert 0.0 "\n\n"
	    }
	    .cardhelp.text insert end \
		" First select the appropriate server for your card.\n\
		Then read the README file corresponding to the selected\
		server by pressing\n\
		the 'Read README file' button (it won't do anything, if\
		there is no README).\n\n\
		Next, pick the chipset, and Ramdac of your card, if\
		directed by the README\n\
		file.  In most cases, you don't need to select these,\
		as the server will\n\
		detect (probe) them automatically.\n\n\
		The clockchip should generally be picked, if your card\
		has one, as these\n\
		are often impossible to probe (the exception is when\
		the clockchip is built\n\
		into one of the other chips).\n\n\
		Choose whatever options are appropriate (again,\
		according to the README).\n\n\
		You can also set the maximum speed of your Ramdac. Some\
		Ramdacs are available\n\
		with various speed ratings.  The max speed cannot be\
		detected by the server\n\
		so it will use the speed rating of the slowest version\
		of the specified Ramdac.\n\n\
		Additionally, you can also specify the amount of RAM on\
		your card, though\n\
		the server will usually be able to detect this."
	}
	.cardhelp.text configure -state disabled
        button .cardhelp.ok -text "Dismiss" -command "destroy .cardhelp"
	focus .cardhelp.ok
        pack .cardhelp.text .cardhelp.ok
}

