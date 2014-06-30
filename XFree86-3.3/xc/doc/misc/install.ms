.\" $XConsortium: INSTALL.ms /main/7 1996/12/22 20:44:54 swick $
.\" X11R6.3 Installation instructions.  Use troff -ms macros
.ds Ts March 5, 1996\" updated by Emacs
.\"
.ie t \{
.nr LL 6.5i
.\}
.el \{
.nr LL 72n
.na
.if n .pl 9999v		\" no page breaks in nroff
.\}
.nr FL \n(LLu
.nr LT \n(LLu
.ll \n(LLu
.nr PS 11
.de nH
.NH \\$1
\\$2
.XS
\\*(SN \\$2
.XE
.LP
..
.de Ip
.IP "\fB\\$1\fP" \\$2
..
.\"
.ds CH \" empty center heading
.sp 8
.TL
\s+2\fBBuilding and Installing the X Window System\fP\s-2
.AU
.sp 6
\fIStephen Gildea\fP
.AI

X Consortium
.sp 6
\*(Ts

Updated For Release 6.3
.LP
.if t \{\
.bp
\&
.sp 3
.\}
.sp 5
Copyright \(co 1995, 1996 X Consortium
.nr PS 9
.nr VS 11
.LP
Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
\*QSoftware\*U), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:
.LP
The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
.LP
THE SOFTWARE IS PROVIDED \*QAS IS\*U, WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
.LP
Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.
.LP
\fIX Window System\fP is a trademark of X Consortium, Inc.
.\"
.if t \{
.OH 'X Version 11, Release 6.3 Snapshot''X Window System Installation'
.EH 'X Window System Installation Version 11, Release 6.3 Snapshot'
.bp 1
.ds CF \\n(PN
.\}
.nr PS 11
.nr VS 13

.nH 1 "Easy Build Instructions"
.\"
This quick summary is no substitute for reading the full build
instructions later in this document.
.LP
Edit \fBxc/config/cf/site.def\fP for local preferences.
If you want to build with \fIgcc\fP
uncomment the \fBHasGcc2\fP line.
If you want to install somewhere other than \fB/usr/X11R6.3\fP,
change
\fBProjectRoot\fP.  (Do \fInot\fP use \fBDESTDIR\fP.)
.LP
If any fixes have been released by the X Consortium, 
stop here and follow the instructions at the top of each patch,
but don't do any of the \fImake\fP
commands suggested in the patches.  Then continue here.
.LP
.\" (never say "vendor.cf", because people look for that literally)
Check the appropriate vendor-specific \fB.cf\fP file in
\fBxc/config/cf/\fP to
make sure that \fBOSMajorVersion\fP and \fBOSMinorVersion\fP are
set correctly for your system.  Override them in \fBsite.def\fP if
necessary.
.LP
See if there is a \fBBootstrapCFlags\fP mentioned in the comments
in the vendor-specific \fB.cf\fP file.
If there isn't one, \fIcd\fP to the \fBxc\fP directory and type:
.ID
% make World >& world.log
.DE
.LP
If there is a \fBBootstrapCFlags\fP, take its value
and type:
.ID
% make World BOOTSTRAPCFLAGS="\fIvalue\fP" >& world.log
.DE
.LP
Do not call the output file \*Qmake.log\*U when doing \*Qmake World\*U.
After a successful build, you can install with:
.ID
% make install >& install.log
.DE
.LP
You can install manual pages with:
.ID
% make install.man >& man.log
.DE
.LP
While the system is building (or if things fail), read the rest of
these installation instructions.


.nH 1 "Building X"

This document gives detailed instructions for building Release 6:
getting it off the
distribution medium, configuring,
compiling, installing, running, and updating.
.LP
Release Notes are in \fBxc/RELNOTES.*\fP (various formats)
in the distribution.
.LP
More recent information about newly-discovered problems may be found
in the \fIFrequently Asked Questions\fP posting appearing monthly on
the comp.windows.x newsgroup and xpert mailing list.  It is also
available via anonymous FTP
on \fBftp.x.org\fP in the file \fBcontrib/faqs/FAQ.Z\fP,
or on your local X mirror site.

.nH 2 "Preparing the Site"

If you are unpacking tar files, you will need about 130 Mb to hold the
\fBxc/\fP part.
To install requires
30-50 Mb assuming you have shared libraries (80-100 Mb without).
You will need an equivalent amount of extra space to build, since you
also need room for all the object files.
.LP
Distributed as tar files, Release 6.3 core is divided into parts as follows:
.LP
.KS
.DS
.ta 1.5i
xc-1.tar	contains everything in xc/ that isn't in the other tar files
xc-2.tar	contains xc/fonts, xc/doc/specs, xc/util
xc-3.tar	contains xc/doc/hardcopy
.DE
.KE
.LP
If you define BuildFonts to NO, you only need to unpack xc-1.tar to
build.  If you build fonts, then you will also need xc-2.tar to build.

.nH 2 "Unpacking the Distribution"

The distribution normally comes as multiple tar files, either on
tape or across a network, or as a CD-ROM.

.nH 3 "Unpacking a Compressed FTP Distribution"

If you have obtained compressed tar files over the network,
create a directory to hold the sources and \fIcd\fP into it:
.ID
mkdir \fIsourcedir\fP
cd \fIsourcedir\fP
.DE
Then for each tar file \fBxc\-*.tar.Z\fP, execute this:
.ID
zcat\0\fIftp-dir\fP/xc\-\fIN\fP.tar.Z | tar xf\0\-
.DE

.nH 3 "Unpacking a gzipped FTP Distribution"

If you have obtained gzipped tar files over the network,
create a directory to hold the sources and \fIcd\fP into it:
.ID
mkdir \fIsourcedir\fP
cd \fIsourcedir\fP
.DE
Then for each tar file \fBxc\-*.tar.gz\fP, execute this:
.ID
gunzip\0\-c\0\fIftp-dir\fP/xc\-\fIN\fP.tar.gz | tar xf\0\-
.DE

.nH 3 "Unpacking a Split Compressed FTP Distribution"

If you have obtained compressed and split tar files over the network,
create a directory to hold the sources:
.ID
mkdir \fIsourcedir\fP
.DE
Then for each directory \fBxc\-*\fP:
.ID
cd \fIftp-dir\fP/xc\-\fIN\fP
cat xc\-\fIN\fP.?? | uncompress | (cd \fIsourcedir\fP\|; tar xf\0\-\|)
.DE

.nH 3 "Unpacking the Tape Distribution"

If you have obtained a tape,
create a directory to hold the sources and untar everything into that
directory:
.ID
mkdir \fIsourcedir\fP
cd \fIsourcedir\fP
tar xf \fItape-device\fP
.DE

.nH 3 "Using the CD-ROM"

If you have obtained a CD-ROM, you don't have to do anything to unpack
it.  However, you will have to create a symbolic link tree to build X.
See the next section.
.LP
To mount the CD-ROM, see the mount(8) manual page on your system or
the liner notes that came with the CD-ROM.
Some systems, e.g., Solaris 2, can automatically mount the CD-ROM for you.

.nH 2 "Apply Patches"

If there are fixes released that are more recent than your distribution,
apply them now.
Follow the instructions at the top
of each patch, but don't do any make commands.
See the section \*QPublic Patches\*U later in this document.
Then continue here.

.nH 2 "Symbolic Link Trees"

If you expect to build the distribution on more than one machine using
a shared source tree,
or you are building from CD-ROM,
or you just want to keep the source tree pure,
you may want to use the program \fBxc/config/util/lndir.c\fP to create
a symbolic link tree on each build machine.
The links may use an additional 10 megabytes, but it is cheaper
than having multiple copies of the source tree.
.LP
It may be tricky to compile \fIlndir\fP before the distribution is
built.  If you have a copy from a previous release, use that.
\fBMakefile.ini\fP can be used for building \fIlndir\fP the first time.
You may have to specify \fBOSFLAGS=\-D\fP\fIsomething\fP to
get it to compile.
What you would pass as \fBBOOTSTRAPCFLAGS\fP might work.
The command line looks something like this:
.ID
make\0\-f\0Makefile.ini\0OSFLAGS=\-D\fIflag\fP
.DE
.LP
To use a symbolic link tree, create a directory for the build, \fIcd\fP
to it, and type this:
.ID
lndir \fIsourcedir\fP
.DE
.LP
where \fIsourcedir\fP is the pathname of the
directory where you stored the sources.  All of the build instructions
given below should then be done in the build directory on each machine,
rather than in the source directory.
.LP
\fBxc/config/util/mkshadow/\fP contains \fImkshadow\fP, an alternative
program to \fIlndir\fP.

.nH 2 "Configuration Parameters"

Build information for each source directory is in files called
\fBImakefile\fP.  An \fBImakefile\fP, along with local configuration
information in \fBxc/config/cf/\fP, is used by the program \fIimake\fP
to generate a \fBMakefile\fP.
.LP
Most of the configuration work prior to building the release is to
set parameters so that \fIimake\fP will generate correct files.
Most of those parameters are set in \fBxc/config/cf/site.def\fP.
You will also need to check
the appropriate vendor-specific \fB.cf\fP file in \fBxc/config/cf/\fP
to make sure that
OSMajorVersion, OSMinorVersion, and OsTeenyVersion are set correctly
for your system.
Override them in \fBsite.def\fP if necessary.
.LP
The \fBsite.def\fP file has two parts, one protected with
\*Q#ifdef BeforeVendorCF\*U and one with \*Q#ifdef AfterVendorCF\*U.
The file is actually processed twice, once before the \fB.cf\fP file
and once after.  About the only thing you need to set in the \*Qbefore\*U
section is \fBHasGcc2\fP; just about everything else can be set in the
\*Qafter\*U section.
.LP
The sample \fBsite.def\fP also has commented out support to include another 
file, \fBhost.def\fP.  This scheme may be useful if you want to set most
parameters site-wide, but some parameters vary from machine to machine.
If you use a symbolic link tree, you can share \fBsite.def\fP across
all machines, and give each machine its own copy of \fBhost.def\fP.
.LP
The config parameters are listed in \fBxc/config/cf/README\fP, but
here are some of the more common parameters that you may wish to set in
\fBsite.def\fP.
.Ip "ProjectRoot"
The destination where X will be installed.  This variable needs to be
set before you build, as some programs that read files at run-time
have the installation directory compiled in to them.  Assuming you
have set the variable to some value /\fIpath\fP, files will be
installed into /\fIpath\fP/bin, /\fIpath\fP/include/X11,
/\fIpath\fP/lib, and /\fIpath\fP/man.
.Ip "HasGcc"
Set to \fBYES\fP to build with \fIgcc\fP version 1.
.Ip "HasGcc2"
Set to \fBYES\fP to build with \fIgcc\fP version 2.
Both this option and \fBHasGcc\fP look for a compiler named \fIgcc\fP,
but \fBHasGcc2\fP will cause the build to use more features of
\fIgcc\fP 2, such as the ability to compile shared libraries.
.Ip BuildXInputExt
Set to \fBYES\fP to build the X Input Extension.  This extension
requires device-dependent support in the X server, which exists only
in \fIXhp\fP in our implementation.
.Ip BuildPexExt
Set to \fBNO\fP to not build the PEX server extension and fonts.
.Ip "DefaultUsrBin"
This is a directory where programs will be found even if PATH
is not set in the environment.
It is independent of ProjectRoot and defaults to \fB/usr/bin\fP.
It is used, for example, when connecting from a remote system via \fIrsh\fP.
The \fIrstart\fP program installs its server in this directory.
.Ip "InstallServerSetUID"
Some systems require the X server to run as root to access the devices
it needs.  If you are on such a system and will not be using
\fIxdm\fP, you can set this variable to \fBYES\fP to install the X
server setuid to root.  Note that the X server has not been analyzed
by the X Consortium for security in such an installation;
talk to your system manager before setting this variable.
.Ip InstallXdmConfig
By default set to NO, which suppresses installing xdm config files
over existing ones.  Leave it set to NO if your site has customized
the files in \fB/usr/X11R6.3/lib/X11/xdm\fP, as many sites do.
If you don't install the new files, merge any changes
present in the new files.
.Ip "MotifBC"
Causes Xlib and Xt to work around some bugs in older versions of Motif.
Set to \fBYES\fP only if you will be linking with Motif version 1.1.1,
1.1.2, or 1.1.3.
.Ip "GetValuesBC"
Setting this variable to \fBYES\fP allows illegal XtGetValues requests
with NULL ArgVal to usually succeed, as R5 did.  Some applications
erroneously rely on this behavior.  Support for this will be removed
in a future release.
.LP
The following vendor-specific \fB.cf\fP files are in the release but have
not been tested recently and hence probably need changes to work:
\fBapollo.cf\fP, \fBbsd.cf\fP,
\fBconvex.cf\fP,
\fBDGUX.cf\fP,
\fBluna.cf\fP,
\fBmacII.cf\fP,
\fBMips.cf\fP,
\fBmoto.cf\fP,
\fBOki.cf\fP,
\fBpegasus.cf\fP,
\fBx386.cf\fP.
\fBAmoeba.cf\fP is known to require additional patches.
.LP
The file \fBxc/lib/Xdmcp/Wraphelp.c\fP, for XDM-AUTHORIZATION-1, is not
included in this release.

.nH 2 "System Build Notes"

This section contains hints on building X with specific compilers and
operating systems.
.LP
If the build isn't finding things right, make
sure you are using a compiler for your operating system.  For example, a
pre-compiled \fIgcc\fP for a different OS will not have right symbols
defined, so \fIimake\fP will not work correctly.

.nH 3 "gcc"
.\"
\fIgcc\fP version 2 is in regular use at the X Consortium on Sparc
platforms.
Set the variable \fBHasGcc2\fP.
X will not compile on some systems with \fIgcc\fP version 2.5, 2.5.1, or
2.5.2 because of an incorrect declaration of memmove() in a gcc
include file.
.LP
If you are using a \fIgcc\fP version older than 2.7 on Solaris x86,
you need to specify
.nh
\fBBOOTSTRAPCFLAGS="\-Dsun"\fP
.hy
in the \*Qmake World\*U command.

.nH 3 "Other GNU tools"
.\"
Use of the GNU assembler, \fIas\fP, or linker, \fIld\fP, is not supported.
GNU \fImake\fP is not supported.
.\"
.\"We broke clearmake between R6.1 and R6.3 and didn't get to fix it.
.\".nH 3 "clearmake"
.\"
.\"Atria's \fIclearmake\fP make program, part of their ClearCase product,
.\"was supported in R6.1.
.\"You will need patches to ClearCase version
.\"2.0.2 or 2.0.3.
.\"You need one of 2.0.3-61 through 2.0.3-69, as
.\"appropriate for your platform, or any later patch that fixes bug #7250.
.\"Even with these patches there is still a bug in clearmake that
.\"prevents it from correctly building the X server on HP-UX (the problem
.\"is building the HP ddx).
.\".LP
.\"To use clearmake, set the variable \fBHasClearmake\fP to \fBYES\fP.
.\"Once you make Makefiles with HasClearmake, you
.\"cannot go back and use regular make with the same Makefiles.
.\"You can use clearmake without setting HasClearmake,
.\"but you won't be able to take advantage of clearmake's
.\"file-sharing abilities.

.nH 3 "SparcWorks 2.0"

If you have a non-threaded
program and want to debug it with the old SparcWorks 2.0 dbx,
you will need to use the thread stubs library in
\fBxc/util/misc/thr_stubs.c\fP.
Compile it as follows:
.ID
cc -c thr_stubs.c
ar cq libthr_stubs.a thr_stubs.o
ranlib libthr_stubs.a
.DE
Install libthr_stubs.a in the same directory with your X libraries
(e.g., \fB/usr/X11R6.3/lib/libthr_stubs.a\fP).
Add the following line to \fBsite.def\fP:
.ID
#define ExtraLibraries\0\-lsocket\0\-lnsl $(CDEBUGFLAGS:\-g=\-lthr_stubs)
.DE
This example uses a \fImake\fP macro substitution; not all \fImake\fP
implementations support this feature.

.nH 3 "CenterLine C under Solaris 2"

If you are using the CenterLine C compiler to compile the distribution
under Solaris 2,
place the following line in your \fBsite.def\fP:
.ID
#define HasCenterLineC YES
.DE
If clcc is not in your default search path, add this line to \fBsite.def\fP:
.ID
#define CcCmd /path/to/your/clcc
.DE
.LP
If you are using CodeCenter 4.0.4 or earlier, the following files 
trigger bugs in the \fIclcc\fP optimizer:
.ID
xc/programs/Xserver/cfb16/cfbgetsp.c
xc/programs/Xserver/cfb16/cfbfillsp.c
xc/programs/Xserver/cfb/cfbgetsp.c
.DE
.LP
Thus to build the server, you will have to compile these files by hand
with the \fB\-g\fP flag:
.ID
% cd xc/programs/Xserver/cfb16
% make CDEBUGFLAGS="\-g" cfbgetsp.o cfbfillsp.o
% cd ../cfb 
% make CDEBUGFLAGS="\-g" cfbgetsp.o
.DE
This optimizer bug appears to be fixed in CodeCenter 4.0.6.

.\"No idea if the following is still needed on newer version of AIX
.nH 3 "IBM AIX 4.1.4"

On AIX 4.1.4, the file \fBlib/font/Type1/objects.c\fP must be compiled
without optimization (\fB\-O\fP) else the X server will exit when Type 1
fonts are used.

.nH 3 "SunOS 4"

SunOS 4.0 and earlier need BOOTSTRAPCFLAGS=-DNOSTDHDRS because they do
not have unistd.h nor stdlib.h.  Do \fInot\fP supply a BOOTSTRAPCFLAGS
when building any SunOS 4.1 version.

.nH 3 "Microsoft Windows NT"

All of the base libraries are
supported, including multi-threading in Xlib and Xt, but some of the more
complicated applications, specifically \fIxterm\fP and \fIxdm\fP,
are not supported.
.LP
There are also some other rough edges in the
implementation, such as lack of support for non-socket file descriptors as Xt
alternate inputs and not using the registry for configurable parameters like
the system filenames and search paths.
.LP
The \fIXnest\fP server has been made to run on NT.  It requires a real
X server for output still.

.nH 3 "Omron Luna"

The Omron Luna platform is no longer supported.
The Luna version of the \fImake\fP program doesn't define the standard
macro MAKE, so you must run it as \*Qmake MAKE=make\*U at top level,
e.g., \*Qmake MAKE=make World\*U.

.nH 2 "The Build"

On NT, type
.ID
nmake World.Win32 > world.log
.DE
On other systems, find the BootstrapCFlags line, if any, in the
vendor-specific \fB.cf\fP file.  If there isn't one, type
.ID
% make World >& world.log
.DE
otherwise type
.ID
% make World BOOTSTRAPCFLAGS="value" >& world.log
.DE
.LP
You can call the output file something other than \*Qworld.log\*U, but
do not call it \*Qmake.log\*U because files with this name are
automatically deleted during the \*Qcleaning\*U stage of the build.
.LP
Because the build can take several hours to complete, you will probably
want to run it in the background and keep a watch on the output.
For example:
.ID
% make World >& world.log &
% tail\0\-f\0world.log
.DE
.LP
If something goes wrong, the easiest thing is to just start over
(typing \*Qmake World\*U again) once you have corrected the problem.

.nH 2 "Installing X"

If everything is built successfully, you can install the software
by typing the following as root:
.ID
% make install >& install.log
.DE
.LP
Again, you might want to run this in the background and use \fItail\fP
to watch the progress.
.LP
You can install the manual pages by typing the following as root:
.ID
% make install.man >& man.log
.DE

.nH 3 "System Installation Notes"

This section contains hints on installing and using X with specific
compilers and operating systems.

.nH 4 "The X Server on AIX 4"

For IBM's AIX 4, you need to make sure the LFT device is associated
with the correct graphics adapter.  It's a one-time setup that does
\fInot\fP happen automatically, even if there's only one graphics
adapter in the system.  To configure the LFT device properly, become
root and start SMIT.  Go to the \*QDevices\*U category, choose
\*QLFT\*U, then \*QDisplays\*U, then \*QMove the LFT to Another
Display\*U.
.LP
Select \*QBoth\*U for when the change should take effect, then select the
display adapter where you want to run the X server.  Confirm the
changes and exit SMIT; from now on, you should be able to run the
server just fine.
.LP
To run \fIXibm\fP from \fIxdm\fP,
you must provide the \*Q\-force\*U flag on the server command line in
the \fBXservers\fP file.

.nH 2 "Shared Libraries"

The version number of some of the the shared libraries has been
changed.
On SunOS 4, which supports minor version numbers for shared libraries,
programs linked with the R6 libraries will use the new libraries with
no special action required.
On other platforms you have the following choices:
.IP 1.
Keep the old versions of the libraries around.
.IP 2.
Relink all applications with the new libraries.
.IP 3.
Create a link from
the old name to the new name.
.IP
For example, to have programs that were linked against libX11.so.6.0 use
libX11.so.6.3, make this link:
.ID
ln\0\-s libX11.so.6.3 libX11.so.6.0
.DE

.nH 2 "Setting Up xterm"

If your \fB/etc/termcap\fP and \fB/usr/lib/terminfo\fP databases do
not have correct entries for \fIxterm\fP, use the sample entries
provided in the directory \fBxc/programs/xterm/\fP.  System V users
may need to compile and install the \fBterminfo\fP entry with the
\fItic\fP utility.
.LP
Since each \fIxterm\fP will need a separate pseudoterminal,
you need a reasonable number of them for normal execution.
You probably will want at least 32 on a small, multiuser system.
On most systems, each pty has two devices, a master and a slave,
which are usually named /dev/tty[pqrstu][0-f] and /dev/pty[pqrstu][0-f].
If you don't have at least the \*Qp\*U and \*Qq\*U sets configured
(try typing \*Qls /dev/?ty??\*U), you should have your system administrator
add them.  This is commonly done by running the \fIMAKEDEV\fP script in
the \fB/dev\fP directory with appropriate arguments.

.nH 2 "Starting Servers at System Boot"

The \fIxfs\fP and \fIxdm\fP programs are designed to be run
automatically at system startup.  Please read the manual pages for
details on setting up configuration files; reasonable sample files are
in \fBxc/programs/xdm/config/\fP and \fBxc/programs/xfs/\fP.

.nH 3 "On BSD-based systems using /etc/rc"

If your system uses an \fB/etc/rc\fP file at boot time, you can
usually enable these programs by placing the following at or near the end
of the file:
.ID
if [ \-f /usr/X11R6.3/bin/xfs ]; then
        /usr/X11R6.3/bin/xfs & echo \-n ' xfs'
fi

if [ \-f /usr/X11R6.3/bin/xdm ]; then
        /usr/X11R6.3/bin/xdm; echo \-n ' xdm'
fi
.DE
.LP
Since \fIxfs\fP can serve fonts over the network,
you do not need to run a font server on every machine with
an X display.  You should start \fIxfs\fP before \fIxdm\fP, since
\fIxdm\fP may start an X server which is a client of the font server.
.LP
The examples here use \fB/usr/X11R6.3/bin\fP, but if you have installed into
a different directory by setting (or unsetting) \fBProjectRoot\fP then you
need to substitute the correct directory.
.LP
If you are unsure about how system boot works, or if your system does
not use \fB/etc/rc\fP, consult your system administrator for help.

.nH 3 "On SystemV-based systems"

There are two ways you can get \
On systems with a \fB/etc/inittab\fP file, you can edit this file to
add the lines
.ID
xfs:3:once:/usr/X11R6.3/bin/xfs
xdm:3:once:/usr/X11R6.3/bin/xdm
.DE
.LP
On some systems, you can edit a file in /etc/init.d to run
the X Consortium xdm instead of the vendor's product xdm.
On Sony this file is /etc/init.d/consxdm.
On IRIX edit /etc/init.d/xdm.

.nH 2 "Using OPEN LOOK applications"

You can use the X11R6 Xsun server with OPEN LOOK applications, but you
must pass the \fB\-swapLkeys\fP flag to the server on startup, or the 
OPEN LOOK Undo, Copy, Paste, Find, and Cut keys may not work correctly.
For example, to run Sun's OpenWindows 3.3 desktop environment with an
X11R6 server, use the command:
.ID
% openwin\0\-server\0/usr/X11R6.3/bin/Xsun\0\-swapLkeys
.DE
.LP
The keysyms reported by keys on the numeric keypad have also changed 
since X11R5; if you find that OpenWindows applications do not respond 
to keypad keys and cursor control keys when using the R6 server, you 
can remap the keypad to generate R5 style keysyms using the following 
\fIxmodmap\fP commands:
.ID
keysym Pause = F21
keysym Print = F22
keysym Break = F23
keysym KP_Equal = F24
keysym KP_Divide = F25
keysym KP_Multiply = F26
keysym KP_Home = F27
keysym KP_Up = Up
keysym KP_Prior = F29
keysym KP_Left = Left
keycode 100 = F31
keysym KP_Right = Right
keysym KP_End = F33
keysym KP_Down = Down
keysym KP_Next = F35
keysym KP_Insert = Insert
keysym KP_Delete = Delete
.DE

.nH 2 "Rebuilding after Patches"

You shouldn't need this right away, but eventually you are probably
going to make changes to the sources, for example by applying
any public patches that may be released.
.\"See the section \*QPublic Patches\*U later in this document.
.LP
Each patch comes with explicit instructions at the top of it saying
what to do.  Thus the procedure here is only an overview of the types
of commands that might be necessary to rebuild X after changing it.
.LP
If you are building from CD-ROM, apply the patches to the symbolic
link tree.  The links to changed files will be replaced with local
files containing the new contents.
.LP
If only source files are
changed, you should be able to rebuild just by going to the \fBxc\fP
directory in your build tree and typing:
.ID
% make >& make.log
.DE
.LP
If configuration files are changed, the safest thing to do is type:
.ID
% make Everything >& every.log
.DE
.LP
\*QEverything\*U is similar to \*QWorld\*U in that it rebuilds every
\fBMakefile\fP, but unlike \*QWorld\*U it does not delete the
existing objects, libraries, and executables, and only rebuilds
what is out of date.

.nH 2 "Formatting the Documentation"

The PostScript files in \fBxc/doc/hardcopy\fP can be generated from the
sources in \fBxc/doc/specs\fP.  Most of the documentation is in troff using
the \|\-ms macros.  The easiest way to format it is to use the Imakefiles
provided.
.LP
Set the name of your local troff program by
setting the variable \fBTroffCmd\fP in \fBxc/config/cf/site.def\fP.
Then build the Makefiles:
.ID
cd xc/doc
make SUBDIRS=specs Makefiles
.DE
.LP
Finally, go to the directory you are interested in and type \*Qmake\*U
there.  This command will generate \fB.PS\fP files.
You can also generate text files by specifying the document name with
a \fB.txt\fP extension as a \fImake\fP target, e.g., \*Qmake
icccm.txt\*U.

.\"Perhaps the Open Group will continue patches... who knows?
.\".nH 1 "Public Patches"
.\"
.\"X Consortium may from time to time issue public patches to the latest
.\"public release to fix any
.\"serious problems that are discovered.
.\"Such fixes are a subset of fixes available to
.\"Consortium members.  Public patches are available over the Internet in
.\"two ways: via anonymous
.\"FTP, and via the xstuff mail server.
.\".LP
.\"Fixes are available via anonymous FTP to ftp.x.org,
.\"in the directory \fB/pub/R6.3/fixes/\fP, or from your local X mirror site.
.\"Check the site closest to you first.
.\".LP
.\"You can determine which public patches have been applied to your
.\"source tree by examining the \*QVERSION\*U line of \fBxc/bug-report\fP.
.\"The distribution you get might already have some patches applied; you
.\"only need to apply later patches.  If you try to apply patches out of
.\"order or apply patches that
.\"are already in your tree, \fIpatch\fP will tell you that you have the
.\"wrong version and not apply the patch.
.\".LP
.\"A copy of the \fIpatch\fP program is in \fBxc/util/patch/\fP.
.\".\"
.\".nH 2 "The xstuff Mail Server"
.\"For those without FTP access, individual fixes can be obtained by
.\"electronic mail by sending a message to
.\".ID
.\"xstuff@x.org
.\".DE
.\".LP
.\"In the usual case,
.\"the message should have a subject line and no body, or a single-line body and
.\"no subject, in either case the line looking like:
.\".ID
.\"send fixes \fInumber\fP
.\".DE
.\".LP
.\"where \fInumber\fP is a decimal number, starting from one.
.\"Large patches are broken up into parts for the convenience of mailers
.\"that cannot handle large messages, for example
.\"\*Q2a\*U, \*Q2b\*U, \*Q2c\*U, \*Q2d\*U, and \*Q2e\*U.
.\"Ask for each part separately,
.\"for example, use this line:
.\".ID
.\"send fixes 2a
.\".DE
.\"to get part 2a.  Concatenate all the parts together before applying,
.\"e.g.,
.\".ID
.\"cat 2[abcde] | patch \|\-p \|\-s
.\".DE
.\".LP
.\"To get a
.\"summary of available fixes, email this line to xstuff:
.\".ID
.\"index fixes
.\".DE
.\".LP
.\"If you need help, make the line:
.\".ID
.\"help
.\".DE
.\".LP
.\"Some mailers produce mail headers that are unusable for extracting
.\"return addresses.  If you use such a mailer, you won't get any
.\"response.  If you happen to know an explicit return path, you can
.\"include include one in the body of your message, and the daemon will
.\"use it.  For example:
.\".ID
.\"path \fIuser\fP%\fIhost\fP.bitnet@mitvma.mit.edu
.\".DE
.\"
.ie t \{
.\"
.\" print Table of Contents
.if o .bp \" blank page to make count even
.bp 1
.af PN i
.PX
.\}
.el .pl \n(nlu+1v
.\"
.\" Local Variables:
.\" time-stamp-start: "^\\.ds Ts "
.\" time-stamp-end: "\\\\\""
.\" time-stamp-format: "%b %d, %y"
.\" End:
