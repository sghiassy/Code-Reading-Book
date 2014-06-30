# $XConsortium: phase5.tcl /main/4 1996/10/28 05:42:32 kaleb $
#
#
#
#
# $XFree86: xc/programs/Xserver/hw/xfree86/XF86Setup/phase5.tcl,v 3.7 1997/01/02 11:58:41 dawes Exp $
#
# Copyright 1996 by Joseph V. Moss <joe@XFree86.Org>
#
# See the file "LICENSE" for information regarding redistribution terms,
# and for a DISCLAIMER OF ALL WARRANTIES.
#

#
# Phase V - Final commands after return to text mode
#

check_tmpdirs
clear_scrn
foreach fname [glob -nocomplain $TmpDir/*] {
	unlink $fname
}
rmdir $TmpDir
rmdir $XF86SetupDir

if { ![getuid] } {
    set idx -1
    if { [llength $DeviceIDs] == 1 } {
	set idx 0
    } else {
	if [string length $Scrn_Accel(Device)] {
	    set idx [lsearch $DeviceIDs $Scrn_Accel(Device)]
	} else {
	    set idx [lsearch $DeviceIDs $Scrn_SVGA(Device)]
	}
    }
    if { $idx >= 0 } {
	set devid [lindex $DeviceIDs $idx]
        global Device_$devid
        set server [set Device_${devid}(Server)]
	set linkname $Xwinhome/bin/X
	set lastlink $linkname
	for {set nlinks 0} \
		{[file exists $linkname] && [file type $linkname]=="link" \
		 && $nlinks<20} \
		{incr nlinks} {
	    set lastlink $linkname
	    set linkname [readlink $linkname]
	}
	if { $nlinks < 20 } {
	    set servname [string range [file tail $linkname] 5 end]
	    if ![string compare $servname $server] {
		unset linkname
	    }
	} else {
	    set linkname $Xwinhome/bin/X
	}
    }
    if [info exists linkname] {
	set linkdir [file dirname $lastlink]
	set mklink [mesg "Do you want to create an 'X' link\
		to the $server server?\n\n(the link will be\
		created in the directory: $linkdir)" yesno]
	if $mklink {
	    set CWD [pwd]
	    cd $linkdir
	    catch "unlink X" ret
	    if [catch "link $Xwinhome/bin/XF86_$server X" ret] {
		mesg "Link creation failed!\n\
			You'll have to do it yourself" okay
	    } else {
		mesg "Link created successfully." okay
	    }
	    cd $CWD
	}
    }
}

clear_scrn
puts "\n\nConfiguration complete."

exit 0

