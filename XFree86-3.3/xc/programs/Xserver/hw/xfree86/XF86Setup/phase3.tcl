# $XConsortium: phase3.tcl /main/1 1996/09/21 14:17:37 kaleb $
#
#
#
#
# $XFree86: xc/programs/Xserver/hw/xfree86/XF86Setup/phase3.tcl,v 3.4 1996/12/27 06:54:11 dawes Exp $
#
# Copyright 1996 by Joseph V. Moss <joe@XFree86.Org>
#
# See the file "LICENSE" for information regarding redistribution terms,
# and for a DISCLAIMER OF ALL WARRANTIES.
#

#
# Phase III - Commands run after switching back to text mode
#     - responsible for starting second server
#

source $tcl_library/init.tcl
source $XF86Setup_library/setuplib.tcl
source $XF86Setup_library/carddata.tcl
source $XF86Setup_library/mondata.tcl
source $StateFileName

mesg "Attempting to start server..." info
sleep 2
writeXF86Config $Confname-2 -defaultmodes

set devid [lindex $DeviceIDs 0]
global Device_$devid
set server [set Device_${devid}(Server)]

set ServerPID [start_server $server $Confname-2 ServerOut-2 ]

if { $ServerPID == -1 } {
	set msg "Unable to communicate with X server"
}

if { $ServerPID == 0 } {
	set msg "Unable to start X server"
}

if { $ServerPID < 1 } {
	mesg "$msg\n\nPress \[Enter\] to try configuration again" okay
	set Phase2FallBack 1
	set ServerPID [start_server $server $Confname-1 ServerOut-1Bis]
	if { $ServerPID < 1 } {
		mesg "Ack! Unable to get the VGA16 server going again!" info
		exit 1
	}
}

if { ![string length [set Device_${devid}(ClockChip)]] } {
	set fd [open $TmpDir/ServerOut-2 r]
	set clockrates ""
	set zerocount 0
	while {[gets $fd line] >= 0} {
		if {[regexp {\(.*: clocks: (.*)$} $line dummy clocks]} {
			set clocks [string trim [squash_white $clocks]]
			foreach clock [split $clocks] {
				lappend clockrates $clock
				if { $clock < 0.1 } {
					incr zerocount
				}
			}
		}
	}
	close $fd
	set clockcount [llength $clockrates]
	if { $clockcount != 0 && 1.0*$zerocount/$clockcount < 0.25 } {
		set Device_${devid}(Clocks) $clockrates
	}
}

