








			   Information for SCO Users

		       J. Kean Johnston (hug@netcom.com)

			       30 November 1996



1.  Binary Distribution

The following files are provided in the binary distribution:


	   README.SCO
		 This file.

	   gunzip.Z
		 The GNU uncompress utility.

	   *X312Xdoc.tgz
		 The XFree86 specific documentation.

	   X312Mono.tgz
		 The Mono server

	   X312VG16.tgz
		 The 16 colour VGA server

	   X312SVGA.tgz
		 The Super VGA server

	   X312S3.tgz
		 The S3 server

	   X3128514.tgz
		 The 8514 server

	   X312AGX.tgz
		 The AGX server

	   X312Mc32.tgz
		 The Mach 32 server

	   X312Mc64.tgz
		 The Mach 64 server

	   X312Mc8.tgz
		 The Mach 8 server

	   X312P9k.tgz
		 The P9000 server



Information for SCO Users





Information for SCO Users



	   X312W32.tgz
		 The ET4000/W32 server

	   *X312cfg.tgz
		 The local configuration files for xdm/fs/xinit.

	   *X312bin.tgz
		 The bin directory, contains most executables.

	   *X312lib.tgz
		 The shared and unshared libraries.

	   *X312fnt1.tgz
		 75dpi and misc fonts.

	   X312fnt2.tgz
		 100dpi and Speedo fonts.

	   *X312inc.tgz
		 The X11 include files.

	   X312man.tgz
		 The formatted man pages.

	   X312lkit.tgz
		 The server link kit (all drivers + PEX).

	   X312util.tgz
		 Lots of PD utilities provided as is.

	   X312pex.tgz
		 All files relating to PEX including libraries and header
		 files.  The LinkKit is required to obtain servers capable
		 of running PEX.

To obtain a minimum XFree86 installation you will require the archives marked
with a `*' above, the server binary best suited to your machine and optionally
"gunzip.Z".  All the files are compressed with "gzip" except of course "gun-
zip.Z" which is compressed using the conventional compress program.

To install the XFree86 binaries just follow these steps.

  1.  Obtain the files you require.


      The rest of this procedure must be done as root.	If you do not run the
      extraction as root the permissions on the files will not be correct.  For
      example,	the `X' server is s-bit root and will not function correctly if
      extracted as an ordinary user.

  2.  create a directory /usr/X11R6, permissions 755 should do nicely.

  3.  cd /usr/X11R6









Information for SCO Users



  4.  extract the archives, for example:

		       gunzip < X312bin.tgz | tar xvpf -



  5.  if you have installed man pages see the later section on setting up man
      pages.

  6.  Look through /usr/X11R6/lib/X11/doc/INSTALL, especially section 2 on con-
      figuring and using XFree86.   This should allow you to get a server up
      and running.  Before starting the server check in the later section
      Before Running XFree86 (section 3., page 3), in this document, to see if
      there are any system requirements you have to make for the server to
      operate correctly.


2.  Source Distribution

The SCO port comes as part of the standard XFree86 distribution.  Consult the
XFree86 README for more information on the location of sources.

Please note that as of XFree86 3.2, Only SCO Open Server Release 5 and onwards
are supported.	If you are using a previous version of SCO UNIX and you want to
use XFree86, use the 3.1 series, or be prepared for build failures.

For people who want and need to look around the source, there are now two files
in ``xc/config/cf''. Firstly, ``sco.cf'' is the old original SCO configuration
file, and ``sco5.cf'', which is the currently used configuration file.


3.  Before Running XFree86

The SCO xterm terminfo description is not compatible with the xterm in the R5
distribution.

To use a Bus/Keyboard or PS2 mouse you should configure the mouse drivers under
SCO as above using 'mkdev mouse'. You may then use the OsMouse option in your
XF86Config to specify that XFree86 should use the SCO mouse drivers.  To do
this, set the Protocol to "OsMouse" in the Pointer section of your XF86Config
file.  You can also use "OsMouse" for your serial mouse, especially if you are
having trouble getting your mouse to work using the XFree86 mouse drivers.

If you do not have the SCO TCP/IP package installed do not panic.  XFree86 will
work fine without TCP/IP but you will most likely have to do some or all of
these things:

   o Do not worry about errors from the X server complaining about
     ``/dev/socksys''.	The X server is configured to run on systems with and
     without TCP/IP.  This error is just pointing out that you do not have
     TCP/IP and that this method of connecting to the server has been disabled.











Information for SCO Users



   o Do worry about errors involving ``/dev/spx'' or the ``sco'' connection
     type.  This means something is wrong with the streams pipes that are used
     for connections on the local machine.  First be sure that your server has
     the ``s-bit'' set.  You can do this by running this command for the X
     server you are using:

     ls -al /usr/X11R6/bin/XF86_XXXXXX

     The output should contain the `s' character instead of the `x' character.
     For example:


	       -rwsr-xr-x   1 root     bin	1074060 Jul 24 11:54 XF86_W32




     is correct while:


	       -rwxr-xr-x   1 root     bin	1074060 Jul 24 11:54 XF86_W32




     is not.


   o you may have to install streams into the kernel with ``mkdev streams''
     Check the SCO Manuals for more information on this.


   o you may have to configure some devices in /dev,  check in the "Trouble
     Shooting" section of this document for the entry which comments on
     ``/dev/spx'' and ``Xsco''.


   o Your streams resources may be configured too low.	You should check your
     streams parameters against the following values,  if the are higher then
     you do not need to changes them.  To check these values, login as root,
     change directory to ``/etc/conf/cf.d'' and then run ``./configure''.

     Once you are running configure,  choose the ``Streams Data'' option and
     step through the entries.	Just press <ENTER> at each prompt unless you
     want to change a value.  The values to look for,  and their minimum val-
     ues, are:
















Information for SCO Users



				  NSTREAM      128
				  NQUEUE       512
				  NBLK4096     4
				  NBLK2048     32
				  NBLK1024     32
				  NBLK512      32
				  NBLK256      64
				  NBLK128      256
				  NBLK64       256
				  NBLK16       256
				  NBLK4        128
				  NUMSP        128





     You will not normally need to change any of these,  if however you do have
     to change some,  configure will confirm that you want to save the changes
     before exiting,  and will give you further instructions on rebuilding the
     unix kernel.


4.  Switching Consoles

XFree86 uses similar console switching keys as the SCO R4 and R5 servers.  That
is, Ctrl-PrntScr takes you to the next console along from the one X is running
on.  If this is the last console it will take you to console 1. Ctrl-Alt-FXX,
where XX is a function key between F1 and F12 will switch you to the console
number assigned to that function key. F1 corresponds to tty01 (or console 1),
F2 corresponds to tty02 (or console 2) etc.  Those interested in modifying the
console switching should look in xc/programs/Xserver/hw/xfree86/com-
mon/xf86Events.c.


5.  Setting up Man Pages

After compiling the tree, or after installing the binary distribution you can
get man to recognise the XFree86 man pages by adding /usr/X11R6/man to the MAN-
PATH in /etc/default/man, the line should look similar to:

	 MANPATH=/usr/man:/usr/X11R6/man


This allows all users to view the X man pages.	You may change your own MANPATH
environment variable if you do not want everyone to access the man pages.

By default the man pages are compressed using ``compress'' to conserve space.
If you do not want to compress the man pages change CompressManPages to NO in
your ``xf86site.def'' file.  Those using the binary distribution can use
``uncompress'' to uncompress the man pages.











Information for SCO Users



6.  Using SCO binaries/servers.

XFree86 will accept connections from SCO binaries (R3 upwards) and the SCO R5
server will also accept connections from XFree86 binaries. This means you may
mix and match the two if you have ODT.	For example you may still use the Motif
window manager (mwm) if you prefer.


7.  Compiling XFree86 under Open Server 5

As of GCC version 2.8.0, Open Server is supported. Configure it by using the
following:

	 ./configure i486-sco3.2v5.0

There is no reason to modify gcc in any way.  It compiles cleanly on Open
Server 5.

SCO Open Server 5.0 is recognised automatically by XFree86. You do not need to
specify any BOOTSTRAPCFLAGS parameters when doing a make World. You can ignore
the warning message about BOOTSTRAPCFLAGS at the very beginning of a make
World.

  1.  Fine tune ``site.def/xf86site.def''

      Use GCC if you can. XFree should compile with the DevSys cc, but GCC has
      better optimizations, and is guaranteed to work.

  2.  SCO Open Server comes with Visual TCL, which is an old (and incompatible)
      version of TCL. If you want to use XF86Setup you will have to compile Tcl
      and Tk yourself. Both are supported well on SCO Open Server 5. Tcl 7.6
      and Tk 4.2 are available from ftp://ftp.smli.com/pub/tcl.

  3.  You may want to disable dynamic loading support. Several users have
      reported trouble with this. XIE and PEX5 definitely do not work. If you
      want to experiment, try enabling this. Please report successes or fail-
      ures to me.

  4.  Do not enable the HasSVR3mmapDrv as you may have done in older versions
      of SCO. Open Server 5 has full mmap() support, and this is used for
      direct frame buffer access.

  5.  If you know you will not ever be using COFF binaries, and you are short
      of space, set ForceNormalLib to NO. Doing this will cause only the ELF
      versions of the libraries to be built.  ``sco5.cf'' sets this to YES by
      default, so you must explicitly set it to NO in ``xf86site.def''.  All
      binaries are compiled in ELF mode to reduce space.


8.  Relevant Documentation

Some relevant documentation for SCO Users and Developers can be found in the
following files.









Information for SCO Users



      README
	    the standard XFree86 README (/usr/X11R6/lib/X11/doc)

      README.SVR3
	    Although a lot of this readme is based on Interactive a substantial
	    proportion is still relevant.

      All of the VGA/Config documentation.
	    /usr/X11R6/lib/X11/doc/VideoModes.doc and the README files for par-
	    ticular video cards.


9.  Known Problems

   o After running the server you may see some strange characters in your input
     to the shell.  This is due to some unprocessed scancodes and is of no con-
     cern.  This will be fixed in a future release.

   o Not all of the applications in /usr/X11R6/bin have been debugged.


10.  Trouble Shooting

      Problem:

		 The server does not start up,	and I cannot tell what is
		 going wrong as it did not print any error messages.

      Causes:

		 There can be any number of causes why the server doesn't
		 start.  The first step is to find out what the server has
		 to say.  To do this we have to catch the error output of
		 the server into a file.  This output contains a log of
		 what the server is finding/doing as it starts up.  To get
		 this output run:

				     startx 2> /tmp/errs



		 The output of the server will now be in "/tmp/errs".  You
		 should look through this output for possible problems,
		 and then check here in this document for any references
		 to the problems you are seeing.

      Problem:

		 The server starts up,	the screen goes blank,	and I
		 never see anything else.  It appears that my machine has
		 hung.

      Causes:









Information for SCO Users



		 Again this can have many causes.  Most likely your
		 XF86Config is wrong.  You should be able to kill the
		 server by typing Ctrl-Alt-BackSpace,  if it is still run-
		 ning.	If this does not restore your display then you may
		 have to drive your system blind.  Always keep another
		 login running at the shell prompt so that you may switch
		 to that screen and run commands even if you cannot see
		 anything on the screen.  Try these things,  usually in
		 the order given:

		    o log out of the login where you started ``X'' and
		      then change consoles.  This will cause the SCO
		      screen switching code to try to reset the card.

		    o run ``vidi v80x25'',  this command will also try to
		      set your card into a viewable mode.

		    o shutdown the machine cleanly with ``shutdown'' and
		      try again.

		 When first trying to get XFree86 to run,  be sure to use
		 a simple setup.  Get 640x480 working first then move on
		 to higher resolutions.  Always trap the output of the
		 server as shown earlier.  Once you have the valid clocks
		 for your video card (as provided in the server output),
		 hard code them into your XF86Config as this will take
		 some strain off your monitor during XFree86 startup where
		 it usually probes the various clock frequencies.  Getting
		 the ``X'' screen to appear can be a painfully slow task.
		 Be patient and read as much of the doco as you can han-
		 dle.  You will get it to work.

      Problem:


				 Fatal server error:
				 xf86MapVidMem:No class map defined for (XXXXX,XXXXX)


      Causes:


		   1.  Your system does not have the correct
		       /etc/conf/pack.d/cn/class.h, You can confirm this
		       by editing the file and looking for the string
		       "SVGA", if it is not there then you should re-
		       install this file from the "Extended Utilities"
		       diskettes provided with your OS.  If this is not
		       possible then installing the "dmmap" driver from
		       the distribution may allow the server to operate
		       correctly.

      Problem:









Information for SCO Users



		 xf86install does not work.

      Causes:

		 You should not be running xf86install when using the
		 XFree86 server under SCO.  It is used for Interactive
		 (ISC) installations.

      Problem:

		 The server starts but the screen is not aligned correctly
		 or is shaky and impossible to view.

      Causes:

		 This is most likely due to an incorrect XF86Config setup.
		 Look for the files README.Config  VideoModes.doc (in
		 /usr/X11R6/lib/X11/doc with the binary distribution).
		 These files explains how to fix up your video modes.

      Problem:

	      1.  Can only run a limited number of xterms.

	      2.  xterm does not work but other programs like xclock do work.

      Causes:

		 Not enough or no pseudo ttys devices are present on your
		 system.  Run "mkdev ptty" and increase the number of
		 ptty's.

      Problem:

		 When running curses/termcap applications in an xterm the
		 output gets corrupted especially when scrolling.

      Causes:

	      1.  You are running an original 1.3 distribution of XFree86.
		  Update to the latest version (3.2 or greater).

	      2.  You have resized the window and not ran "eval `resize`"
		  before using your application.  The SCO operating system does
		  not support dynamic resizing of xterms fully so this command
		  must be run after resizing an xterm in order for curses/term-
		  cap applications to operate correctly.

      Problem:

	      1.  When starting X it dies with an error "Cannot access a needed
		  shared library".










Information for SCO Users



	      2.  When starting an X application is dies with the above error.

      Causes:

	      1.  You do not have the binaries installed in the correct direc-
		  tory.  Check that they are in /usr/X11R6

	      2.  You have upgraded to a new binary distribution which has a
		  new version of the shared libraries which are not compatible
		  with your old binaries.  To fix this you will need to re-
		  install the old shared libraries or recompile your applica-
		  tion against the new libraries.

      Problem:


		 When linking against the SCO motif library I get an unre-
		 solved external for "XtDisplayStringConversionWarning"
		 when using gcc.

      Causes:


		 The SCO library is compiled with limited length identi-
		 fiers.  To work around this add the following code to
		 your application when compiling under XFree86 with gcc
		 and SCO motif.

			  #ifdef SCO
			  void XtDisplayStringConversionWarnin(dpy, from, toType)
			      Display*	dpy;
			      String	from;
			      String	toType;
			  { XtDisplayStringConversionWarning(dpy, from, toType); }
			  #endif


      Problem:

		 The server fails to run and prints out a line similar to:

		 XFree86: Cannot open /dev/spx for ???? listener: No such
		 file or directory

      Causes:

















Information for SCO Users



		 All SCO unix installations appear to have the Streams
		 pseudo tty driver installed, but not all the devices are
		 present.

		   1.  there should be a /etc/conf/pack.d/sp directory,

		   2.  /etc/conf/sdevice.d/sp should have a 'Y' in it.

		   3.  You need a file in /etc/conf/node.d which contains
		       something like:

				    clone  spx	     c	  sp
				    sp	   X0S	     c	  127
				    sp	   X0R	     c	  126
				    sp	   X1S	     c	  125
				    sp	   X1R	     c	  124
				    sp	   X2S	     c	  123
				    sp	   X2R	     c	  122
				    sp	   X3S	     c	  121
				    sp	   X3R	     c	  120
				    sp	   X4S	     c	  119
				    sp	   X4R	     c	  118
				    sp	   X5S	     c	  117
				 sp   X5R	c    116
				    sp	   X6S	     c	  115
				    sp	   X6R	     c	  114
				    sp	   X7S	     c	  113
				    sp	   X7R	     c	  112


		 if you don't have something like this (maybe called
		 "Xsco") then create one and that should fix your problem.
		 As far as I can tell the streams pseudo tty driver should
		 be there.

		 The simplest way to get the devices if you had to create
		 this file is to rebuild the kernel and the environment.
		 If you don't want to do this then:


			      touch /etc/.new_unix
			      cd /etc/conf/bin
			      ./idmkenv



		 and try it out.


11.  Acknowledgements

Thanks to the Core team for their previous and continuing help with the SCO
work.  Many thanks to Stacey Campbell at SCO for all the advice and insights
provided.  Thanks to SCO in general for making information available for








Information for SCO Users



XFree86 development.

Thanks also to Peter Eubert (peter.eubert@iwb.mw.tu-muenchen.dbp.de) and Kent
Hamilton (kenth@stl.scscom.COM) for input on compiling under 3.2.4 systems.
Larry Plona (faxi@world.std.com) and Didier Poirot (dp@chorus.fr) for their
input on xdm and 3.2.4 compilation under 3.1.	And of course the beta list for
its input on everything.

Special thanks to Jerry Whelan (guru@stasi.bradley.edu) for providing an ftp
site for the binary distribution.


     Generated from XFree86: xc/programs/Xserver/hw/xfree86/doc/sgml/SCO.sgml,v 3.16 1997/01/25 03:22:12 dawes Exp $





     $XConsortium: SCO.sgml /main/11 1996/10/23 11:45:55 kaleb $











































Information for SCO Users





































































				   CONTENTS



1.  Binary Distribution ..................................................... 1

2.  Source Distribution ..................................................... 3

3.  Before Running XFree86 .................................................. 3

4.  Switching Consoles ...................................................... 5

5.  Setting up Man Pages .................................................... 5

6.  Using SCO binaries/servers. ............................................. 6

7.  Compiling XFree86 under Open Server 5 ................................... 6

8.  Relevant Documentation .................................................. 6

9.  Known Problems .......................................................... 7

10. Trouble Shooting ........................................................ 7

11. Acknowledgements ....................................................... 11































				       i



$XFree86: xc/programs/Xserver/hw/xfree86/doc/README.SCO,v 3.26 1997/01/27 22:12:53 dawes Exp $
