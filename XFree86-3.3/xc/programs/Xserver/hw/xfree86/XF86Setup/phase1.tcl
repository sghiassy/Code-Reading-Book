# $XConsortium: phase1.tcl /main/3 1996/10/28 05:42:26 kaleb $
#
#
#
#
# $XFree86: xc/programs/Xserver/hw/xfree86/XF86Setup/phase1.tcl,v 3.13 1996/12/27 06:54:09 dawes Exp $
#
# Copyright 1996 by Joseph V. Moss <joe@XFree86.Org>
#
# See the file "LICENSE" for information regarding redistribution terms,
# and for a DISCLAIMER OF ALL WARRANTIES.
#

#
# Phase I - Initial text mode interaction w/user and starting of VGA16 server
#

set clicks1 [clock clicks]

# load the autoload stuff
source $tcl_library/init.tcl
# load in our library
source $XF86Setup_library/setuplib.tcl
source $XF86Setup_library/filelist.tcl
source $XF86Setup_library/carddata.tcl
source $XF86Setup_library/mondata.tcl

proc find_dialog {} {
	global env

	foreach dir [split $env(PATH) :] {
		if { [file executable $dir/dialog] } {
			# aha! found one, now let's test to make sure
			set retval [catch {
				exec strings $dir/dialog \
				| grep infobox >& /dev/null }]
			if {$retval == 0} {
				return $dir/dialog
			}
		}
	}
	return ""
}

proc check_for_files { xwinhome } {
	global FilePermsDescriptions FilePermsReadMe

	foreach var [array names FilePermsDescriptions] {
	    global FilePerms$var
	    foreach tmp [array names FilePerms$var] {
		set pattern [lindex $tmp 0]
		set perms   [lindex $tmp 1] ;# ignored (for now at least)
		if ![llength [glob -nocomplain -- $xwinhome/$pattern]] {
			set msg [format "Not all of the %s %s %s %s" \
				$FilePermsDescriptions($var) \
				"are installed. The file" \
				$xwinhome/$pattern "is missing"]
			mesg [parafmt 65 $msg] okay
			exit 1
		}
	    }
	}
	foreach readme [array names FilePermsReadMe] {
	    set pattern [lindex $readme 0]
	    set perms   [lindex $readme 1] ;# ignored (for now at least)
	    if ![llength [glob -nocomplain -- $xwinhome/$pattern]] {
		mesg [parafmt 65 "Warning! Not all of the READMEs are\
			installed. You may not be able to view some of\
			the instructions regarding setting up your card,\
			but otherwise, everything should work correctly"] \
			okay
		break
	    }
	}
}

proc set_xf86config_defaults {} {
    global Xwinhome ConfigFile
    global Files Server Keyboard Pointer MonitorIDs DeviceIDs

    if {![catch {xf86config_readfile $Xwinhome files server \
		keyboard mouse monitor device screen} tmp]} {
	array set Files		[array get files]
	array set Server	[array get Server]
	array set Keyboard	[array get keyboard]
	array set Pointer	[array get mouse]
	foreach drvr { Mono VGA2 VGA16 SVGA Accel } {
	    global Scrn_$drvr
	    if [info exists screen_$drvr] {
		array set Scrn_$drvr	[array get screen_$drvr]
	    } else {
		set Scrn_${drvr}(Driver) ""
	    }
	}
	set MonitorIDs [set DeviceIDs ""]
	set primon 0
	set priname "Primary Monitor"
	global Monitor_$priname
	foreach mon [info vars monitor_*] {
		set id [string range $mon 8 end]
		global Monitor_$id
		if { "$id" == "$priname" } {
			set primon 1
		} else {
			array set Monitor_$id [array get Monitor_$priname]
		}
		lappend MonitorIDs $id
		array set Monitor_$id	[array get monitor_$id]
	}
	if !$primon { global Monitor_$priname; unset Monitor_$priname }
	set pridev 0
	set priname "Primary Card"
	global Device_$priname
	foreach dev [info vars device_*] {
		set id [string range $dev 7 end]
		global Device_$id
		if { "$id" == "$priname" } {
			set pridev 1
		} else {
			array set Device_$id [array get Device_$priname]
		}
		lappend DeviceIDs $id
		array set Device_$id	[array get device_$id]
		set Device_${id}(Options) [set Device_${id}(Option)]
		set Device_${id}(Server) NoMatch
	}
	if !$pridev { global Device_$priname; unset Device_$priname }
	set fd [open $ConfigFile r]
	set ws      "\[ \t\]"
	set nqt     {[^"]}
	set alnum   {[A-Z0-9]}
	set idpat   "^$ws+\[Ii]\[Dd]$nqt+\"($nqt*)\""
	set servpat "\"$nqt*\"$ws+##.*SERVER:$ws*($alnum+)"
	while { [gets $fd line] >= 0 } {
	    set tmp [string toupper [zap_white $line] ]
	    if { [string compare $tmp {SECTION"DEVICE"}] == 0 } {
		while { [gets $fd nextline] >= 0 } {
		    set upper [string toupper $nextline]
		    if { [regexp $idpat $nextline dummy id] } {
			set found [regexp $servpat $upper dummy serv]
			if $found {
				if { [string match XF86_* $serv] } {
				    set serv [string range $serv 5 end]
				}
				set Device_${id}(Server) $serv
			}
			break
		    }
		    if ![string compare [string trim $upper] "ENDSECTION"] {
			break
		    }
		}
	    }
	}
	close $fd
	global ServerList
	foreach devid $DeviceIDs {
	    set varname Device_${devid}(Server)
	    if { ![info exists $varname] ||
		    [lsearch -exact $ServerList [set $varname]] < 0} {
		set filename $Xwinhome/bin/X
		for {set nlinks 0} \
			{[file exists $filename] && \
			 [file type $filename]=="link" && $nlinks<20} \
			{incr nlinks} {
		    set filename [readlink $filename]
		}
		set $varname [string range [file tail $filename] 5 end]
		if { [lsearch -exact $ServerList [set $varname]] < 0
			|| $nlinks == 20} {
		    set $varname SVGA
		}
	    }
	}
    } else {
	mesg "Error encountered reading existing\
		configuration file" okay
	puts $tmp
	exit 0
    }
}

if $NoDialog {
	set Dialog ""
} else {
	set Dialog [find_dialog]
}

check_for_files $Xwinhome

set ConfigFile [xf86config_findfile]
set StartServer 1
set ReConfig 0
set UseConfigFile 0
if { [string length $ConfigFile] > 0 } {
	if [info exists env(DISPLAY)] {
		set msg [format "%s\n \n%s\n \n%s" \
			    [parafmt 65 "It appears that you are currently \
				running under X11. If this is correct \
				and you are interested in making some \
				adjustments to your current setup, \
				answer yes to the following question."] \
			    [parafmt 65 "If this is incorrect or you \
				would like to go through the full \
				configuration process, then answer no."] \
			    "Is this a reconfiguration?" ]
		set ReConfig [mesg $msg yesno]
	}
	if { $ReConfig } {
		set UseConfigFile 1
		set StartServer 0
		if { [getuid] != 0 } {
		    set proceed [mesg "You are not running as\
			root.\n\nSuperuser privileges are usually\
			required to save any changes\nyou make\
			in a directory that is searched by the\
			server and\nare required to change the\
			mouse device.\n\nWould you like\
			to continue anyway?" yesno]
		    if !$proceed {
		        exit 1
		    }
		}
	} else {
		if { [getuid] != 0 } {
		    mesg "You need to be root to set the initial\
			configuration with this program" okay
		    exit 1
		}
		if { ![file exists $Xwinhome/bin/XF86_VGA16] } {
		    mesg "The VGA16 server is required when using\n\
			this program to set the initial configuration" okay
		    exit 1
		}
		set UseConfigFile [mesg "Would you like to use the\
		    existing XF86Config file for defaults?" yesno]
	}
	# initialize the configuration variables
	initconfig $Xwinhome

	if { $UseConfigFile } {
		set_xf86config_defaults
	}
} else {
	set ConfigFile /etc/XF86Config
	if { [getuid] != 0 } {
	    mesg "You need to be root to run this program" okay
	    exit 1
	}
	if { !$ReConfig && ![file exists $Xwinhome/bin/XF86_VGA16] } {
	    mesg "The VGA16 server is required to run this program" okay
	    exit 1
	}
	# initialize the configuration variables
	initconfig $Xwinhome
}

set clicks2 [clock clicks]

if { ![getuid] } {
    if { !$UseConfigFile } {
	# Check for the SysV Xqueue mouse driver
	if { [file exists /etc/conf/pack.d/xque]
		&& [file exists /usr/lib/mousemgr] } {
	    set xque [mesg "Would you like to use the Xqueue driver\n\
			for mouse and keyboard input?" yesno]
	    if $xque {
		set Keyboard(Protocol)	Xqueue
		set Pointer(Protocol)	Xqueue
		set Pointer(Device)	""
	    }
	}

	# Check for the SCO OsMouse
	if { [file exists /etc/conf/pack.d/cn/class.h]
		&& [file exists /etc/conf/pack.d/ev] } {
	    set osmse [mesg "Would you like to use the system event\
			queue for mouse input?" yesno]
	    if $osmse {
		set Pointer(Protocol)	OsMouse
		set Pointer(Device)	""
	    }
	}
    }
}

set PID [pid]
if { [info exists env(TMPDIR)] } {
	set XF86SetupDir $env(TMPDIR)/.XF86Setup$PID
} else {
	set XF86SetupDir /tmp/.XF86Setup$PID
}


if ![mkdir $XF86SetupDir 0700] {
	mesg "Unable to make directory $XF86SetupDir\n\
		 for storing temporary files" okay
	exit 1
}

set rand1 [random 1073741823]
random seed [expr $clicks2-$clicks1]
set rand2 [random 1073741823]

set TmpDir $XF86SetupDir/[format "%x-%x" $rand1 $rand2]
if ![mkdir $TmpDir 0700] {
	mesg "Unable to make directory $TmpDir\n\
		 for storing temporary files" okay
	exit 1
}
check_tmpdirs

if { ![getuid] } {
	if [string length $Pointer(Device)] {
	    if {[file exists $Pointer(Device)]
		    && [file type $Pointer(Device)] == "link" } {
	        set Pointer(RealDev) [readlink $Pointer(Device)]
	        set Pointer(OldLink) $Pointer(Device)
	    } else {
		set Pointer(RealDev) $Pointer(Device)
	    }
	    set Pointer(Device) $TmpDir/mouse
	}
	if [info exists Pointer(RealDev)] {
	    link $Pointer(RealDev) $Pointer(Device)
	}
}

set Confname $TmpDir/Config

if $StartServer {
	# write out a temp XF86Config file
	writeXF86Config $Confname-1 -vgamode -generic

	mesg "Press \[Enter\] to switch to graphics mode.\n\
		\nThis may take a while..." okay

	set ServerPID [start_server VGA16 $Confname-1 ServerOut-1]

	if { $ServerPID == 0 } {
		mesg "Unable to start X server!" info
		exit 1
	}
	if { $ServerPID == -1 } {
		mesg "Unable to communicate with X server!" info
		exit 1
	}
} else {
	mesg "Please wait\n\nThis may take a while..." info
}

