








			       Building XFree86

				  David Dawes

				  2 June 1997



				   Abstract

     This document describes how to build XFree86 from the source distri-
     bution.  It covers building from the full source distribution as well
     as from the cut-down source distribution available for building only
     the X servers.  It is designed to be used in conjunction with the OS-
     specific README files.



1.  Building XFree86 From a Source Distribution

     NOTE:  Refer to the appropriate OS-specific README file before
     attempting to build XFree86.  These files contain additional informa-
     tion that you may need to successfully build under your OS.

We highly recommend using GCC-2 to build XFree86.  Do not use version 2.6.0 or
2.4.x versions prior to 2.4.5 due to bugs that cause either build or execution
failures.  (gcc-2 is available from prep.ai.mit.edu and other sites archiving
GNU source.)

1.1  How to get the XFree86 3.3 source

There are a few starting points for getting the XFree86 source.  One option is
to start directly with the XFree86 3.3 source distribution.  In this case, the
procedure is as follows:

   o The XFree86 3.3 source is contained in files X33src-1.tgz, X33src-2.tgz
     and X33src-3.tgz.	These can be found at
     ftp://ftp.xfree86.org/pub/XFree86/3.3/source/ and similar locations on
     XFree86 mirror sites.  X33src-2.tgz contains the fonts and documentation
     source.  X33src-3.tgz contains the hardcopy documentation.  X33src-1.tgz
     contains everything else.	If you don't need the docs or fonts you can get
     by with only X33src-1.tgz.

   o Extract each of these files by running the following from a directory on a
     filesystem containing enough space (the full source requires around 130MB,
     and a similar amount is required in addition to this for the compiled
     binaries):







Building XFree86





Building XFree86



		    gzip -d < X33src-1.tgz | tar vxf -
		    gzip -d < X33src-2.tgz | tar vxf -
		    gzip -d < X33src-3.tgz | tar vxf -



Another option is to start with the X11R6.3 source distribution and patch it up
to XFree86 3.3.  In this case you need to do the following:

   o Start with the X Consortium's X11R6.3 distribution with public patch 1
     applied.  This can be obtained by following the links from the X home page
     <URL:http://www.x.org>.

   o Get the files R6.3pl1-3.3.diff1.gz, R6.3pl1-3.3.diff2.gz,
     R6.3pl1-3.3.diff3.gz, R6.3pl1-3.3.diff4.gz, and cfont33.tgz from
     ftp://ftp.xfree86.org/pub/XFree86/3.3/patches/ (or a similar location on
     mirror sites).  To upgrade the source to XFree86 3.3, run the following
     from directory containing the xc directory of the X11R6.3 pl1 source tree:


		    gzip -d < R6.3pl1-3.3.diff1.gz | patch -p0 -E
		    gzip -d < R6.3pl1-3.3.diff2.gz | patch -p0 -E
		    gzip -d < R6.3pl1-3.3.diff3.gz | patch -p0 -E
		    gzip -d < R6.3pl1-3.3.diff4.gz | patch -p0 -E
		    gzip -d < cfont33.tgz | tar vxf -



     Be sure to do this with a clean unmodified source tree.  If you don't some
     patches may fail.

If you only want to build the XFree86 X servers, you can use a cut-down version
of the XFree86 source tree called the ``servers only'' distribution.  If you
choose this option, do the following:

   o Get the X33servonly.tgz file from
     ftp://ftp.xfree86.org/pub/XFree86/3.3/source/ (or a similar locations on
     mirror sites.

   o Extract this by running the following:


		    gzip -d < X33servonly.tgz | tar vxf -



There is no patch to upgrade from the XFree86 3.2 source to 3.3.  The reason
for this is the large number of changes associated with the move from X11R6.1
to X11R6.3.

XFree86 supports a small subset of the X Consortium X11R6.1 contrib distribu-
tion.  If you wish to build this, you will need at least the following
files/directories from that distribution:









Building XFree86



	  contrib/Imakefile
	  contrib/programs/Imakefile
	  contrib/programs/ico
	  contrib/programs/listres
	  contrib/programs/showfont
	  contrib/programs/viewres
	  contrib/programs/xbiff
	  contrib/programs/xcalc
	  contrib/programs/xditview
	  contrib/programs/xedit
	  contrib/programs/xev
	  contrib/programs/xeyes
	  contrib/programs/xfontsel
	  contrib/programs/xgc
	  contrib/programs/xload
	  contrib/programs/xman
	  contrib/programs/xmessage


You will also need the XFree86 patch contrib-3.3.diff.gz.  To apply the patch,
run the following from the directory containing the contrib directory:

	  gzip -d < contrib-3.3.diff.gz | patch -p0 -E


Alternatively, you can just get the file X33contrib.tgz from the XFree86 source
directory, and extract it by running:

	  gzip -d < X33contrib.tgz | tar vxf -

If you wish to build the xtest distribution, get the source distribution
X33test.tgz from the XFree86 source directory, and extract it by running:

	  gzip -d < X33test.tgz | tar vxf -


Note, xtest is no longer part of the core X11 distribution (since X11R6.3).

1.2  Configuring the source before building

It is recommended that you start the configuration process by going to the
xc/config/cf directory, and copying the file xf86site.def to host.def.	Then
read through the host.def file (which is heavily commented), and set any param-
eters that you want for your configuration.  You can usually find out what the
default settings are by checking the .cf file(s) relevant to your OS.

Unlike previous versions, imake can now automatically detect and set the vari-
ous OS*Version parameters, so you shouldn't need to enter those settings
explicitly.

If you are using just the X33src-1.tgz part of the source dist, you will need
to define BuildFonts to NO.

If you are using the ``servers only'' distribution, you will need to define








Building XFree86



BuildServersOnly to YES.

1.3  Building and installing the distribution

Before building the distribution, read through the OS-specific README file in
xc/programs/Xserver/hw/xfree86/doc that is relevant to you.  Once those OS-spe-
cific details have been taken care of, go the xc directory and run ``make
World'' with the BOOTSTRAPCFLAGS set as described in the OS-specific README (if
necessary).  It is advisable to redirect stdout and stderr to World.Log so that
you can track down problems that might occur during the build.

When the build is finished, you should check World.Log to see if there were any
problems.  If there weren't any then you can install the binaries.  When using
the full source distribution, the installation should be done from the xc
directory.  When using the ``servers only'' distribution, the install should be
done from the xc/programs/Xserver directory.  To do the install, run ``make
install'' and ``make install.man''.  Make sure you have enough space in
/usr/X11R6 for the install to succeed.	If you want to install on a filesystem
other than /usr, make a symbolic link to /usr/X11R6 before installing.

To install the binary LinkKit (in /usr/X11R6/lib/Server), run ``make
install.linkkit'' from the xc directory.

To build the subset of the contrib release supported by XFree86, make sure that
you have first built and installed the core distribution.  Then go to the con-
trib directory and run ``xmkmf -a; make''.  When that is completed, run ``make
install'' and ``make install.man'' to install it.

To build/run the xtest distribution, refer to the instructions in the file
test/xsuite/NOTES.xf86.


2.  Reconfiguring the server (source distribution)

To build a different set of servers or servers with a different set of drivers
installed:

  1.  Make sure the source for any new drivers is in the correct place (e.g.,
      driver source for the SVGA server should be in a subdirectory of xc/pro-
      grams/Xserver/hw/xfree86/vga256/drivers).

  2.  Change the settings of the server defines in host.def to specify which
      servers you wish to build.  Also, change the driver lists to suit your
      needs.

  3.  From xc/programs/Xserver, run:

		make Makefile
		make Makefiles
		make depend
		make











Building XFree86



3.  Reconfiguring the server (binary distribution)

If you have installed the server Binary LinkKit, it is possible to reconfigure
the drivers and some of the extensions in the servers.	For details of how to
do this, please refer to the README.LinkKit file.

     Generated from XFree86: xc/programs/Xserver/hw/xfree86/doc/sgml/BUILD.sgml,v 3.1.2.4 1997/06/02 03:12:42 dawes Exp $























































Building XFree86





































































				   CONTENTS



1. Building XFree86 From a Source Distribution .............................. 1
   1.1 How to get the XFree86 3.3 source .................................... 1
   1.2 Configuring the source before building ............................... 3
   1.3 Building and installing the distribution ............................. 4

2. Reconfiguring the server (source distribution) ........................... 4

3. Reconfiguring the server (binary distribution) ........................... 5












































				       i



$XFree86: xc/programs/Xserver/hw/xfree86/doc/BUILD,v 3.1.2.4 1997/06/02 03:18:20 dawes Exp $
