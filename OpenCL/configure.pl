#!/usr/bin/perl -w

# WARNING: This file is machine-generated; any changes will be lost.
# Instead, change mkconfig.pl and the system description files.

# If you find a bug in this program (such as generation of incorrect
# options), please mail lloyd@acm.jhu.edu with details.

# This file is in the public domain.

# ???
# require 5.005;

use strict;
use DirHandle;
use Getopt::Long;
use File::Spec;
use File::Copy;

my $MAJOR_VERSION = 0;
my $MINOR_VERSION = 7;
my $PATCH_VERSION = 6;
my $INCLUDE_DIR = 'include';
my $SRC_DIR = 'src';
my $MOD_DIR = 'modules';
my $CHECK_DIR = 'checks';
my $DOC_DIR = 'doc';

my $CPP_INCLUDE_DIR_DIRNAME = 'opencl';
my $CPP_INCLUDE_DIR = File::Spec->catdir($INCLUDE_DIR,
                                         $CPP_INCLUDE_DIR_DIRNAME);

my $BUILD_DIR = 'build';
my $BUILD_LIB_DIR = File::Spec->catdir($BUILD_DIR, 'lib');
my $BUILD_CHECK_DIR = File::Spec->catdir($BUILD_DIR, 'checks');

my $CONFIG_H_FILE = File::Spec->catfile($INCLUDE_DIR, 'config.h');
my $VERSION_CPP_FILE = File::Spec->catfile($SRC_DIR, 'version.cpp');

my $MAKE_FILE = 'Makefile';

# Headers that are private to the library and don't need to be installed
my @EXCLUDED_HEADERS = ('dynatype.h', 'secqueue.h', 'seal_gam.h');

# Documentation list
my %DOCS = (
   'readme.txt' => undef, # undef = file is in top level directory
   'license.txt' => $DOC_DIR,
   'log.txt' => $DOC_DIR,
   'thanks.txt' => $DOC_DIR,
   'indent.el' => $DOC_DIR,
   'pgpkeys.asc' => $DOC_DIR,
   'opencl.ps' => $DOC_DIR,
   'opencl.tex' => $DOC_DIR,
   'relnotes.txt' => $DOC_DIR,
   'authors.txt' => $DOC_DIR,
   );

my %ARCH = (
   '68020'        => 'm68k',
   '68030'        => 'm68k',
   '68040'        => 'm68k',
   '68060'        => 'm68k',
   'alpha'        => 'alpha',
   'alpha-ev4'    => 'alpha',
   'alpha-ev5'    => 'alpha',
   'alpha-ev56'   => 'alpha',
   'alpha-ev6'    => 'alpha',
   'alpha-ev67'   => 'alpha',
   'alpha-pca56'  => 'alpha',
   'arm'          => 'arm',
   'armv2'        => 'arm',
   'armv2a'       => 'arm',
   'armv3'        => 'arm',
   'armv3m'       => 'arm',
   'armv4'        => 'arm',
   'armv5'        => 'arm',
   'athlon'       => 'ia32',
   'hppa'         => 'hppa',
   'hppa1.0'      => 'hppa',
   'hppa1.1'      => 'hppa',
   'hppa2.0'      => 'hppa',
   'i386'         => 'ia32',
   'i486'         => 'ia32',
   'i586'         => 'ia32',
   'i686'         => 'ia32',
   'ia32'         => 'ia32',
   'ia64'         => 'ia64',
   'k6'           => 'ia32',
   'm68k'         => 'm68k',
   'mip32-r3000'  => 'mips32',
   'mip32-r6000'  => 'mips32',
   'mips32'       => 'mips32',
   'mips64'       => 'mips64',
   'mips64-r10000' => 'mips64',
   'mips64-r4000' => 'mips64',
   'mips64-r4100' => 'mips64',
   'mips64-r4300' => 'mips64',
   'mips64-r4400' => 'mips64',
   'mips64-r4560' => 'mips64',
   'mips64-r4600' => 'mips64',
   'mips64-r5000' => 'mips64',
   'mips64-r8000' => 'mips64',
   'powerpc'      => 'powerpc',
   'ppc601'       => 'powerpc',
   'ppc602'       => 'powerpc',
   'ppc603'       => 'powerpc',
   'ppc604'       => 'powerpc',
   'sparc32'      => 'sparc32',
   'sparc32-v7'   => 'sparc32',
   'sparc32-v8'   => 'sparc32',
   'sparc32-v9'   => 'sparc32',
   'sparc64'      => 'sparc64',
   'sparc64-ultra' => 'sparc64',
   'sparc64-ultra2' => 'sparc64',
);

my %ARCH_ALIAS = (
   '680x0'        => 'm68k',
   '68k'          => 'm68k',
   '80x86'        => 'ia32',
   'alphaaxp'     => 'alpha',
   'axp'          => 'alpha',
   'hp-pa'        => 'hppa',
   'hp-pa-risc'   => 'hppa',
   'hp-parisc'    => 'hppa',
   'ix86'         => 'ia32',
   'merced'       => 'ia64',
   'mips'         => 'mips32',
   'pa-risc'      => 'hppa',
   'parisc'       => 'hppa',
   'ppc'          => 'powerpc',
   'sparc'        => 'sparc32',
   'x86'          => 'ia32',
);

my %DEFAULT_SUBMODEL = (
   'alpha'        => 'alphaev4',
   'arm'          => 'armv2',
   'hppa'         => 'hppa1.0',
   'ia32'         => 'i386',
   'ia64'         => 'ia64',
   'm68k'         => '68020',
   'mips32'       => 'r3000',
   'mips64'       => 'r4400',
   'powerpc'      => 'ppc602',
   'sparc32'      => 'sparc32-v8',
   'sparc64'      => 'sparc64-ultra',
);

my %SUBMODEL_ALIAS = (
   'alphaev4'     => 'alpha-ev4',
   'alphaev5'     => 'alpha-ev5',
   'alphaev56'    => 'alpha-ev56',
   'alphaev6'     => 'alpha-ev6',
   'alphaev67'    => 'alpha-ev67',
   'alphapca56'   => 'alpha-pca56',
   'arm2'         => 'armv2',
   'arm2a'        => 'armv2a',
   'arm3'         => 'armv3',
   'arm3m'        => 'armv3m',
   'arm4'         => 'armv4',
   'arm5'         => 'armv5',
   'cypress'      => 'sparc32-v7',
   'hypersparc'   => 'sparc32-v8',
   'microsparc'   => 'sparc32-v8',
   'mipsbe'       => 'mips3000',
   'mipsle'       => 'mips3000',
   'pentium'      => 'i586',
   'pentium2'     => 'i686',
   'pentium3'     => 'i686',
   'pentium4'     => 'i686',
   'pentium_pro'  => 'i686',
   'pentiumpro'   => 'i686',
   'r10000'       => 'mips64-r10000',
   'r10k'         => 'mips64-r10000',
   'r3000'        => 'mips32-r3000',
   'r3k'          => 'mips32-r3000',
   'r4000'        => 'mips64-r4000',
   'r4100'        => 'mips64-r4100',
   'r4300'        => 'mips64-r4300',
   'r4400'        => 'mips64-r4400',
   'r4560'        => 'mips64-r4560',
   'r4600'        => 'mips64-r4600',
   'r4k'          => 'mips64-r4000',
   'r5000'        => 'mips64-r5000',
   'r5k'          => 'mips64-r5000',
   'r6000'        => 'mips32-r6000',
   'r6k'          => 'mips32-r6000',
   'r8000'        => 'mips64-r8000',
   'r8k'          => 'mips64-r8000',
   'sparc-v7'     => 'sparc32-v7',
   'sparc-v8'     => 'sparc32-v8',
   'sparc-v9'     => 'sparc32-v9',
   'sparclite'    => 'sparc32-v8',
   'sparcv7'      => 'sparc32-v7',
   'sparcv8'      => 'sparc32-v8',
   'sparcv9'      => 'sparc32-v9',
   'supersparc'   => 'sparc32-v8',
);

my %OS_SUPPORTS_ARCH = (
   'aix'        => [ 'ia64', 'powerpc', ],
   'beos'       => [ 'ia32', 'powerpc', ],
   'cygwin'     => [ 'ia32', ],
   'darwin'     => [ 'ia32', 'powerpc', ],
   'freebsd'    => [ 'alpha', 'ia32', 'ia64', ],
   'hpux'       => [ 'hppa', ],
   'irix'       => [ 'mips32', 'mips64', ],
   'linux'      => [ 'alpha', 'arm', 'hppa', 'ia32', 'ia64', 'm68k', 'mips32', 
                     'mips64', 'powerpc', 'sparc32', 'sparc64', ],
   'netbsd'     => [ 'alpha', 'ia32', 'ia64', 'm68k', 'mips32', 'mips64', 'powerpc', 
                     'sparc32', 'sparc64', ],
   'openbsd'    => [ 'alpha', 'ia32', 'm68k', 'mips32', 'powerpc', 'sparc32', 
                     ],
   'solaris'    => [ 'ia32', 'ia64', 'sparc32', 'sparc64', ],
   'sunos'      => [ 'ia32', 'sparc32', ],
   'tru64'      => [ 'alpha', ],
   'win32'      => [ 'alpha', 'ia32', 'ia64', ],
);

my %OS_SUPPORTS_SHARED = (
   'aix'        => [ 'all', ],
   'beos'       => [ 'all', ],
   'darwin'     => [ 'all', ],
   'freebsd'    => [ 'all', ],
   'hpux'       => [ 'all', ],
   'irix'       => [ 'all', ],
   'linux'      => [ 'all', ],
   'netbsd'     => [ 'alpha', 'ia32', ],
   'openbsd'    => [ 'ia32', ],
   'solaris'    => [ 'all', ],
   'sunos'      => [ 'all', ],
   'tru64'      => [ 'all', ],
);

my %OS_SUPPORTS_STD = (
   'aix'        => [ 'posix1', 'posix1b', 'pthreads', ],
   'beos'       => [ 'posix1', 'posix1b', 'pthreads', ],
   'cygwin'     => [ 'posix1', ],
   'darwin'     => [ 'posix1', ],
   'freebsd'    => [ 'posix1', 'posix1b', 'pthreads', ],
   'hpux'       => [ 'posix1', 'posix1b', 'pthreads', ],
   'irix'       => [ 'posix1', 'posix1b', 'pthreads', ],
   'linux'      => [ 'posix1', 'posix1b', 'pthreads', ],
   'netbsd'     => [ 'posix1', 'posix1b', 'pthreads', ],
   'openbsd'    => [ 'posix1', 'posix1b', 'pthreads', ],
   'solaris'    => [ 'posix1', 'posix1b', 'pthreads', ],
   'sunos'      => [ 'posix1', ],
   'tru64'      => [ 'posix1', 'posix1b', 'pthreads', ],
   'win32'      => [ 'posix1', ],
);

my %OS_TYPE = (
   'aix'          => 'unix',
   'beos'         => 'beos',
   'cygwin'       => 'unix',
   'darwin'       => 'unix',
   'freebsd'      => 'unix',
   'hpux'         => 'unix',
   'irix'         => 'unix',
   'linux'        => 'unix',
   'netbsd'       => 'unix',
   'openbsd'      => 'unix',
   'solaris'      => 'unix',
   'sunos'        => 'unix',
   'tru64'        => 'unix',
   'win32'        => 'windows',
);

my %OS_SHARED_SUFFIX = (
   'aix'          => 'so',
   'beos'         => 'so',
   'cygwin'       => '',
   'darwin'       => 'dylib',
   'freebsd'      => 'so',
   'hpux'         => 'sl',
   'irix'         => 'so',
   'linux'        => 'so',
   'netbsd'       => 'so',
   'openbsd'      => 'so',
   'solaris'      => 'so',
   'sunos'        => 'so',
   'tru64'        => 'so',
   'win32'        => 'dll',
);

my %OS_AR_COMMAND = (
   'aix'          => '',
   'beos'         => '',
   'cygwin'       => '',
   'darwin'       => 'ar cr',
   'freebsd'      => '',
   'hpux'         => '',
   'irix'         => '',
   'linux'        => '',
   'netbsd'       => '',
   'openbsd'      => '',
   'solaris'      => '',
   'sunos'        => '',
   'tru64'        => '',
   'win32'        => '',
);

my %OS_AR_NEEDS_RANLIB = (
   'aix'          => '0',
   'beos'         => '0',
   'cygwin'       => '0',
   'darwin'       => '1',
   'freebsd'      => '0',
   'hpux'         => '0',
   'irix'         => '0',
   'linux'        => '0',
   'netbsd'       => '0',
   'openbsd'      => '0',
   'solaris'      => '0',
   'sunos'        => '0',
   'tru64'        => '0',
   'win32'        => '0',
);

my %INSTALL_INFO = (
   'aix'        => {
      'docs'       => 'share/doc',
      'headers'    => 'include',
      'libs'       => 'lib',
      'root'       => '/usr/local',
      },
   'beos'       => {
      'docs'       => 'doc',
      'headers'    => 'headers',
      'libs'       => 'lib',
      'root'       => '/boot/develop',
      },
   'cygwin'     => {
      'docs'       => 'docs',
      'headers'    => 'include',
      'libs'       => 'lib',
      'root'       => 'c:/opencl',
      },
   'darwin'     => {
      'docs'       => 'doc',
      'headers'    => 'include',
      'libs'       => 'lib',
      'root'       => '/usr/local',
      },
   'freebsd'    => {
      'docs'       => 'share/doc',
      'headers'    => 'include',
      'libs'       => 'lib',
      'root'       => '/usr/local',
      },
   'hpux'       => {
      'docs'       => 'share/doc',
      'headers'    => 'include',
      'libs'       => 'lib',
      'root'       => '/usr/local',
      },
   'irix'       => {
      'docs'       => 'share/doc',
      'headers'    => 'include',
      'libs'       => 'lib',
      'root'       => '/usr/local',
      },
   'linux'      => {
      'docs'       => 'share/doc',
      'headers'    => 'include',
      'libs'       => 'lib',
      'root'       => '/usr/local',
      },
   'netbsd'     => {
      'docs'       => 'share/doc',
      'headers'    => 'include',
      'libs'       => 'lib',
      'root'       => '/usr/local',
      },
   'openbsd'    => {
      'docs'       => 'share/doc',
      'headers'    => 'include',
      'libs'       => 'lib',
      'root'       => '/usr/local',
      },
   'solaris'    => {
      'docs'       => 'share/doc',
      'headers'    => 'include',
      'libs'       => 'lib',
      'root'       => '/usr/local',
      },
   'sunos'      => {
      'docs'       => 'share/doc',
      'headers'    => 'include',
      'libs'       => 'lib',
      'root'       => '/usr/local',
      },
   'tru64'      => {
      'docs'       => 'share/doc',
      'headers'    => 'include',
      'libs'       => 'lib',
      'root'       => '/usr/local',
      },
   'win32'      => {
      'docs'       => 'docs',
      'headers'    => 'include',
      'libs'       => 'lib',
      'root'       => 'c:/opencl',
      },
);

my %CC_SUPPORTS_OS = (
   'compaq'     => [ 'linux', 'tru64', ],
   'gcc'        => [ 'aix', 'beos', 'cygwin', 'darwin', 'freebsd', 'hpux', 'irix', 
                     'linux', 'netbsd', 'openbsd', 'solaris', 'sunos', 'tru64', 
                     'win32', ],
   'hpcc'       => [ 'hpux', ],
   'kai'        => [ 'hpux', 'irix', 'linux', 'solaris', 'tru64', ],
   'mipspro'    => [ 'irix', ],
   'pgi'        => [ 'linux', 'solaris', ],
   'sunwspro'   => [ 'solaris', ],
);

my %CC_SUPPORTS_ARCH = (
   'compaq'     => [ 'alpha', ],
   'gcc'        => [ 'alpha', 'arm', 'hppa', 'ia32', 'ia64', 'm68k', 'mips32', 
                     'mips64', 'powerpc', 'sparc32', 'sparc64', ],
   'hpcc'       => [ 'hppa', ],
   'kai'        => [ 'alpha', 'hppa', 'ia32', 'mips32', 'mips64', 'sparc32', 
                     'sparc64', ],
   'mipspro'    => [ 'mips32', 'mips64', ],
   'pgi'        => [ 'ia32', ],
   'sunwspro'   => [ 'ia32', 'sparc32', 'sparc64', ],
);

my %CC_BINARY_NAME = (
   'compaq'       => 'cxx',
   'gcc'          => 'g++',
   'hpcc'         => 'CC',
   'kai'          => 'KCC',
   'mipspro'      => 'CC',
   'pgi'          => 'pgCC',
   'sunwspro'     => 'CC',
);

my %CC_LIB_OPT_FLAGS = (
   'compaq'       => '-O4 -fast',
   'gcc'          => '-O3 -fstrict-aliasing',
   'hpcc'         => '+O3 +Oinlinebudge=150',
   'kai'          => '+K3 --inline_auto_space_time=65 --abstract_pointer',
   'mipspro'      => '-O3 -OPT:alias=TYPED',
   'pgi'          => '-fast -Minline',
   'sunwspro'     => '-xO4',
);

my %CC_CHECK_OPT_FLAGS = (
   'compaq'       => '-O2',
   'gcc'          => '-O2 -fstrict-aliasing',
   'hpcc'         => '+O2',
   'kai'          => '+K3',
   'mipspro'      => '-O3 -OPT:alias=TYPED',
   'pgi'          => '-fast',
   'sunwspro'     => '-xO',
);

my %CC_WARN_FLAGS = (
   'compaq'       => '',
   'gcc'          => '-W -Wall',
   'hpcc'         => '+w',
   'kai'          => '',
   'mipspro'      => '',
   'pgi'          => '',
   'sunwspro'     => '+w',
);

my %CC_LANG_FLAGS = (
   'compaq'       => '-std ansi',
   'gcc'          => '-ansi',
   'hpcc'         => '-ext +eh -z',
   'kai'          => '-D__KAI_STRICT',
   'mipspro'      => '-ansi -LANG:ansi-for-init-scope=ON',
   'pgi'          => '',
   'sunwspro'     => '+p',
);

my %CC_DEBUG_FLAGS = (
   'compaq'       => '-g',
   'gcc'          => '-g',
   'hpcc'         => '-g',
   'kai'          => '-g',
   'mipspro'      => '-g3',
   'pgi'          => '',
   'sunwspro'     => '-g',
);

my %CC_NO_DEBUG_FLAGS = (
   'compaq'       => '',
   'gcc'          => '-fomit-frame-pointer',
   'hpcc'         => '',
   'kai'          => '',
   'mipspro'      => '',
   'pgi'          => '',
   'sunwspro'     => '',
);

my %CC_MACHINE_OPT_FLAGS = (
   'compaq'     => {
      'alpha'      => '-arch=SUBMODEL',
      },
   'gcc'        => {
      'alpha'      => '-mcpu=SUBMODEL',
      'alpha-ev67' => '-march=ev6',
      'arm'        => '-march=SUBMODEL',
      'hppa'       => '-march=SUBMODEL',
      'i386'       => '-mcpu=i686',
      'ia32'       => '-march=SUBMODEL',
      'm68k'       => '-mSUBMODEL',
      'mips32'     => '-mips1 -mcpu=SUBMODEL',
      'mips64'     => '-mips3 -mcpu=SUBMODEL',
      'powerpc'    => '-mcpu=SUBMODEL',
      'ppc601'     => '-march=powerpc',
      'r10000'     => '-mips4',
      'sparc32'    => '-mcpu=SUBMODEL -Wa,-xarch=v8plus',
      'sparc64'    => '-mcpu=v9 -mtune=ultrasparc',
      },
   'hpcc'       => {
      'hppa1.0'    => '+DAportable',
      'hppa1.1'    => '+DA1.1',
      'hppa2.0'    => '+DA2.0W',
      },
   'mipspro'    => {
      'mips32'     => '-mips1',
      'mips64'     => '-mips3',
      'mips64-r10000' => '-mips4 -r10000',
      'mips64-r5000' => '-mips4 -r5000',
      'mips64-r8000' => '-mips4 -r8000',
      },
   'pgi'        => {
      'athlon'     => '-tp k7',
      'i586'       => '-tp p5',
      'i686'       => '-tp p6',
      'ia32'       => '-tp px',
      },
   'sunwspro'   => {
      'i386'       => '-xtarget=386',
      'i486'       => '-xtarget=486',
      'i586'       => '-xtarget=pentium',
      'i686'       => '-xtarget=pentium_pro',
      'sparc32'    => '-xchip=ultra -xarch=SUBMODEL',
      'sparc32-v9' => '-xchip=ultra -xarch=v8',
      'sparc64'    => '-xchip=SUBMODEL',
      },
);

my %CC_MACHINE_OPT_FLAGS_RE = (
   'compaq'     => {
      'alpha'      => 'alpha-',
      },
   'gcc'        => {
      'alpha'      => 'alpha-',
      'hppa'       => 'hppa',
      'mips32'     => 'mips32-',
      'mips64'     => 'mips64-',
      'powerpc'    => 'ppc',
      'sparc32'    => 'sparc32-',
      },
   'sunwspro'   => {
      'sparc32'    => 'sparc32-',
      'sparc64'    => 'sparc64-',
      },
);

my %CC_SO_OBJ_FLAGS = (
   'compaq'       => '',
   'gcc'          => '-fPIC',
   'hpcc'         => '+Z',
   'kai'          => '',
   'mipspro'      => '-KPIC',
   'pgi'          => '-fPIC',
   'sunwspro'     => '-KPIC',
);

my %CC_MACH_ABI_FLAGS = (
   'gcc'        => {
      'mips32'     => '-mabi=n32',
      'mips64'     => '-mabi=64',
      'sparc32'    => '-m32',
      'sparc64'    => '-m64',
      },
   'kai'        => {
      'all'        => '--one_per',
      },
   'mipspro'    => {
      'mips32'     => '-n32',
      'mips64'     => '-64',
      },
   'sunwspro'   => {
      'sparc64'    => '-xarch=v9',
      },
);

my %CC_SO_LINK_FLAGS = (
   'compaq'     => {
      'default'    => '-shared -soname $(SONAME)',
      },
   'gcc'        => {
      'darwin'     => '-dynamiclib -fPIC -install_name $(SONAME)',
      'default'    => '-shared -fPIC -Wl,-soname,$(SONAME)',
      'hpux'       => '-shared -fPIC -Wl,+h,$(SONAME)',
      'solaris'    => '-shared -fPIC -Wl,-h,$(SONAME) -mimpure-text',
      },
   'hpcc'       => {
      'default'    => '-Wl,+h,$(SONAME)',
      },
   'kai'        => {
      'default'    => '--soname $(SONAME)',
      },
   'mipspro'    => {
      'default'    => '-shared -Wl,-soname,$(SONAME)',
      },
   'pgi'        => {
      'linux'      => '-shared -fPIC -Wl,-soname,$(SONAME)',
      'solaris'    => '-G -fPIC -Wl,-h,$(SONAME)',
      },
   'sunwspro'   => {
      'default'    => '-G -h$(SONAME)',
      },
);

my %CC_AR_COMMAND = (
   'compaq'       => '',
   'gcc'          => '',
   'hpcc'         => '',
   'kai'          => 'KCC -o',
   'mipspro'      => '',
   'pgi'          => '',
   'sunwspro'     => '',
);

my %MAKEFILE_STYLE = (
   'compaq'       => 'unix',
   'gcc'          => 'unix',
   'hpcc'         => 'unix',
   'kai'          => 'unix',
   'mipspro'      => 'unix',
   'pgi'          => 'unix',
   'sunwspro'     => 'unix',
);

my %REALNAME = (
   'aix'          => 'AIX',
   'alpha'        => 'DEC Alpha',
   'arm'          => 'ARM',
   'beos'         => 'BeOS',
   'compaq'       => 'Compaq C++',
   'cygwin'       => 'Cygwin',
   'darwin'       => 'Darwin',
   'freebsd'      => 'FreeBSD',
   'gcc'          => 'GNU C++',
   'hpcc'         => 'HP-UX C++',
   'hppa'         => 'HP-PA',
   'hpux'         => 'HP-UX',
   'ia32'         => 'IA-32',
   'ia64'         => 'IA-64',
   'irix'         => 'SGI IRIX',
   'kai'          => 'KAI C++',
   'linux'        => 'Linux',
   'm68k'         => 'Motorola 680x0',
   'mips32'       => 'MIPS',
   'mips64'       => 'MIPS 64-bit',
   'mipspro'      => 'SGI MIPSPro C++',
   'netbsd'       => 'NetBSD',
   'openbsd'      => 'OpenBSD',
   'pgi'          => 'Portland Group C++',
   'posix1'       => 'POSIX.1',
   'posix1b'      => 'POSIX.1b',
   'powerpc'      => 'PowerPC',
   'pthreads'     => 'POSIX Threads',
   'solaris'      => 'Solaris',
   'sparc32'      => 'Sun SPARC',
   'sparc64'      => 'Sun 64-bit SPARC',
   'sunos'        => 'SunOS',
   'sunwspro'     => 'Sun Workshop Pro C++',
   'tru64'        => 'Compaq Tru64',
   'win32'        => 'Win32',
);

my %MODULES = get_modules_list($MOD_DIR);

if($#ARGV < 0) { help(); }

my $debug = 0;
my $no_asm = 0;
my $no_shared = 0;
my $make_style = "";
my @using_mods;

##################################################
# Parse command line options                     #
##################################################
GetOptions('debug' => sub { $debug = 1; $no_asm = 1; },
           'disable-asm' => sub { $no_asm = 1; },
           'disable-shared' => sub { $no_shared = 1; },
           'make-style=s' => \$make_style,
           'modules=s' => \@using_mods,
           'help' => sub { help(); }
           );
@using_mods = split(/,/,join(',',@using_mods));
foreach (@using_mods) {
   die "ERROR: Module $_ isn't known (try --help)\n"
      unless(exists($MODULES{$_}));
}

if($#ARGV != 0) { help(); }

my ($cc,$os,$submodel) = split(/-/,$ARGV[0],3);
if(!defined($cc) or !defined($os) or !defined($submodel)) { help(); }

##################################################
# Check input                                    #
##################################################
if(!defined($CC_BINARY_NAME{$cc}))
   { die "ERROR: Compiler $cc isn't known\n"; }
if(!defined($OS_SUPPORTS_ARCH{$os}) and $os ne 'generic')
   { die "ERROR: OS $os isn't known\n"; }

# Get the canonical submodel name (like r8k -> r8000)
if(defined($SUBMODEL_ALIAS{$submodel}))
   { $submodel = $SUBMODEL_ALIAS{$submodel}; }

my $arch = undef;
# Convert an arch alias to it's real name (like axp -> alpha)
if(defined($ARCH_ALIAS{$submodel}))
   { $arch = $ARCH_ALIAS{$submodel}; $submodel = $arch; }
# If it's a regular submodel type, figure out what arch it is
elsif(defined($ARCH{$submodel}))
   { $arch = $ARCH{$submodel}; }
elsif($submodel eq 'generic') { $arch = 'generic'; }
else { die "ERROR: Arch $submodel isn't known\n"; }

# If we got a generic family name as the model type
if($submodel eq $arch and $submodel ne 'generic') {
   $submodel = $DEFAULT_SUBMODEL{$arch};
   if($submodel ne $arch) {
      print STDERR "WARNING: Using $submodel as default type for family ",
            $REALNAME{$arch},"\n";
   }
}

##################################################
# Does the OS support the arch?                  #
##################################################
unless($arch eq 'generic' or $os eq 'generic' or
       in_array($OS_SUPPORTS_ARCH{$os}, $arch))
   { die "ERROR: $REALNAME{$os} doesn't run on $REALNAME{$arch}\n"; }

##################################################
# Does the compiler support the arch?            #
##################################################
unless($arch eq 'generic' or (in_array($CC_SUPPORTS_ARCH{$cc}, $arch)))
   { die "ERROR: $REALNAME{$cc} doesn't run on $REALNAME{$arch}\n"; }

##################################################
# Does the compiler support the OS?              #
##################################################
unless($os eq 'generic' or (in_array($CC_SUPPORTS_OS{$cc}, $os)))
   { die "ERROR: $REALNAME{$cc} doesn't run on $REALNAME{$os}\n"; }

##################################################
# Check for conflicts in the module selections   #
##################################################
check_for_conflicts(@using_mods);
my (%ignored_src, %ignored_include, %added_src, %added_include, %patched);
foreach (@using_mods) {
   load_module($_, $cc, $os, $arch, $submodel, $no_asm, %{ $MODULES{$_} });
}

##################################################
# Print some source files                        #
##################################################
print_config_h(defines(@using_mods));

unless( -e $VERSION_CPP_FILE ) {
   print_version_cpp($MAJOR_VERSION, $MINOR_VERSION, $PATCH_VERSION);
}

##################################################
# Set up the build tree                          #
##################################################
my %lib_src = list_dir($SRC_DIR, \%ignored_src);
my %check_src = list_dir($CHECK_DIR, undef);

my %include = list_dir($INCLUDE_DIR, \%ignored_include);
# Make sure to remove the directory that's in include/
delete $include{$CPP_INCLUDE_DIR_DIRNAME};

mkdirs(($CPP_INCLUDE_DIR, $BUILD_DIR, $BUILD_LIB_DIR, $BUILD_CHECK_DIR));
clean_out_dirs(($CPP_INCLUDE_DIR));
copy_files($CPP_INCLUDE_DIR, \%include, \%added_include);

# These files should be available to compile the library but are not
# needed outside of it.
foreach (@EXCLUDED_HEADERS) { delete $include{$_}; }

unless($make_style) { $make_style = $MAKEFILE_STYLE{$cc}; }

##################################################
# Print the makefile                             #
##################################################
generate_makefile($make_style,
                  $cc, $os, $submodel, $arch,
                  $debug, $no_asm, $no_shared,
                  \%lib_src, \%check_src, \%include,
                  \%added_src, \%added_include, \%patched,
                  using_libs(@using_mods));

exit;
sub process {
   my $l = $_[0];
   chomp($l);
   $l =~ s/#.*//;
   $l =~ s/^\s*//;
   $l =~ s/\s*$//;
   $l =~ s/\s\s*/ /;
   $l =~ s/\t/ /;
   $l;
}

sub check_for_file {
   my ($file,$mod) = @_;
   unless( -e $file ) { die
     "ERROR: Module $mod requires that file $file exist. This error\n",
     "should never occur; please contact the maintainers with details.\n";
   }
}

sub using_libs {
   my @using = @_;
   my %libs;
   foreach my $mod (@using) {
      foreach my $lib (sort keys %{ $MODULES{$mod}{'libs'} })
         { $libs{$lib} = 1; }
   }
   my @libarray;
   foreach (sort keys %libs) { push @libarray , $_; }
   return @libarray;
   }

sub defines {
   my @using = @_;
   my %defs;
   foreach (@using) {
      if(defined($MODULES{$_}{'define'})) {
         $defs{$MODULES{$_}{'define'}} = 1;
      }
   }
   my @defarray;
   foreach (sort keys %defs)
      { push @defarray , $_; }
   return @defarray;
   }

# Any other alternatives here?
sub portable_symlink {
   my ($from, $to) = @_;

   my $can_symlink = eval { symlink("",""); 1 };
   my $can_link = eval { link("",""); 1 };

   if($can_symlink)
      { symlink $from, $to or die "Can't symlink $from to $to ($!)"; }
   elsif($can_link)
      { link $from, $to or die "Can't link $from to $to ($!)"; }

   # Wow! We're running on a _really_ broken filesystem - hello Windows
   else { 
      # Grrr... copy has different sematics than (sym)link
      # Does this work if the 'volume' thing is used?
      my(undef,$dirs,$file) = File::Spec->splitpath($from, 0);
      # Cut out empty entries and ..
      my $fixed_from = File::Spec->catfile((grep { /./ } grep { !/\.\./ }
                               File::Spec->splitdir($dirs)),
                               $file);
      copy ($fixed_from, $to) or die "Can't copy $fixed_from to $to ($!)";
      }
}

sub copy_files {
   my ($include_dir, $mainline, $modules) = @_;

   my $updir = File::Spec->updir();

   foreach (keys %{ $mainline }) {
      portable_symlink(File::Spec->catfile($updir, $_),
                       File::Spec->catfile($include_dir, $_));
   }
   foreach (keys %{ $modules }) {
      portable_symlink(File::Spec->catfile($updir, $updir, $$modules{$_}, $_),
                       File::Spec->catfile($include_dir, $_));
   }
}

sub list_dir {
  my ($dir, $ignore) = @_;
  opendir DIR, $dir or die "Couldn't open directory $dir ($!)\n";
  my @list = grep { !/.dat$/ } grep { !/^\./ } readdir DIR;

  # If $ignore is set, pull everything in @list that's in $ignore out of it
  if(defined($ignore)) {
     @list = grep { !exists($$ignore{$_}) } @list;
  }
  close DIR;
  my %list = map { $_ => $dir } @list;
  return %list;
}

sub clean_out_dirs {
   my (@dirs) = @_;
   foreach my $dir (@dirs) {
      my %files = list_dir($dir);
      foreach my $file (keys %files) {
         my $path = File::Spec->catfile($dir, $file);
         unlink $path or die "Could not unlink $path ($!)\n";
      }
   }
}

sub mkdirs {
   my (@dirs) = @_;
   foreach my $dir (@dirs) {
      next if( -e $dir and -d $dir ); # skip it if it's already there
      mkdir($dir, 0777) or
         die "ERROR: Could not create directory $dir ($!)\n";
   }
}

sub in_array {
   my($array_ref, $target) = @_;
   if(!defined($array_ref)) { return 0; }
   my @array = @{ $array_ref };
   foreach (@array) { if($_ eq $target) { return 1; } }
   return 0;
}
sub print_config_h
   {
   my (@defines) = @_;

   my $defines = "";
   if(@defines) { $defines .= "\n"; }
   foreach (sort @defines) {
      next if not defined $_ or not $_;
      $defines .= "#define OPENCL_EXT_$_\n"; }

   open CONFIG_H, ">$CONFIG_H_FILE" or
      die "Couldn't write $CONFIG_H_FILE ($!)\n";

   print CONFIG_H <<END_OF_CONFIG_H;
/*************************************************
* Configuration Header File                      *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_CONFIG_H__
#define OPENCL_CONFIG_H__

#include <opencl/types.h>

namespace OpenCL {

const u32bit DEFAULT_BUFFERSIZE = 4*1024;
$defines
}

#endif
END_OF_CONFIG_H
   }

sub print_version_cpp
   {
   my ($major, $minor, $patch) = @_;

   open VERSION_CPP, ">$VERSION_CPP_FILE" or
      die "Couldn't write $VERSION_CPP_FILE ($!)\n";

   print VERSION_CPP <<END_OF_VERSION_CPP;
/*************************************************
* OpenCL Version Information                     *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/opencl.h>

namespace OpenCL {

/*************************************************
* Return the version as a string                 *
*************************************************/
std::string version_string()
   {
   return "OpenCL $major.$minor.$patch";
   }

/*************************************************
* Return parts of the version as numbers         *
*************************************************/
u32bit version_major() { return $major; }
u32bit version_minor() { return $minor; }
u32bit version_patch() { return $patch; }

}
END_OF_VERSION_CPP
   }
sub check_for_conflicts {
   my @mods = @_;
   my (%ignored, %added, %patched, %replaced, %defines,%subsystem);
   foreach my $mod (@mods) {
      if(defined ($MODULES{$mod}{'define'}))
         { conflicts($mod, $MODULES{$mod}{'define'}, 'define', \%defines); }

      if(defined ($MODULES{$mod}{'subsystem'}))
         { conflicts($mod, $MODULES{$mod}{'subsystem'},
                     'use subsystem', \%subsystem); }

      sub check_hash {
         my ($mod, $do_what, $hash) = @_;
         foreach (keys %{ $MODULES{$mod}{$do_what} }) {
            conflicts($mod, $_, $do_what, $hash);
         }
      }

      check_hash($mod, 'replace', %replaced);
      check_hash($mod, 'add', %added);
      check_hash($mod, 'patch', %patched);
      check_hash($mod, 'ignore', %ignored);
   }
}

sub conflicts {
    my ($mod, $item, $do_what, $hashref) = @_;
    return if(!defined($item));
    if(defined($$hashref{$item})) {
       my $other_mod = $$hashref{$item};
       die "ERROR: Both $mod and $other_mod $do_what $item\n";
       }
    else { $$hashref{$item} = $mod }
}

sub get_modules_list
   {
   my $MOD_DIR = $_[0];
   my $dir = new DirHandle $MOD_DIR;
   if(!defined $dir) { return (); }

   my %MOD;

   while(defined($_ = $dir->read))
      {
      next if($_ eq '.' or $_ eq '..');
      my $MODULE = $_;
      my %modinfo = get_module_info($MODULE, $MOD_DIR);
      foreach (keys %modinfo) { $MOD{$MODULE}{$_} = $modinfo{$_}; }
      }
   return %MOD;
   }

sub get_module_info
   {
   my ($MODULE, $MOD_DIR) = @_;
   my %HASH;
   my $mod_dirname = File::Spec->catfile($MOD_DIR,$MODULE);
   my $mod_dir = new DirHandle $mod_dirname;
   if(!defined $mod_dir)
      { die "ERROR: Couldn't open dir $mod_dirname ($!)\n"; }

   my %MODFILES;
   my $have_config_file = 0;
   while(defined($_ = $mod_dir->read))
      {
      if($_ eq $MODULE) { $have_config_file = 1; }
      else { $MODFILES{$_} = undef; }
      }
   die "ERROR: Module $MODULE does not seem to have a description file\n"
      unless $have_config_file;

   my $desc_file = File::Spec->catfile($MOD_DIR,$MODULE,$MODULE);
   open MODFILE, "<$desc_file" or die
      "ERROR: Couldn't open file $desc_file, ($!)\n";

   $HASH{'needs_std'} = {};
   $HASH{'libs'} = {};

   $HASH{'add'} = {};
   $HASH{'replace'} = {};
   $HASH{'ignore'} = {};
   $HASH{'patch'} = {};

   while(<MODFILE>)
      {
      if($_ =~ /^realname \"(.*)\"/) { $HASH{'name'} = $1; }

      if($_ =~ /^patch_file ([\.\w]*) ([\.\w]*)/)
         { $HASH{'patch'}{$1} = $2; }
      if($_ =~ /^add_file ([\.\w]*)/) { $HASH{'add'}{$1} = undef; }
      if($_ =~ /^replace_file ([\.\w]*)/) { $HASH{'replace'}{$1} = undef; }
      if($_ =~ /^ignore_file ([\.\w]*)/) { $HASH{'ignore'}{$1} = undef; }

      if($_ =~ /^define (\w*)/) { $HASH{'define'} = $1; }
      if($_ =~ /^link_to (\w*)/) { $HASH{'libs'}{$1} = undef; }
      if($_ =~ /^subsystem (\w*)/) { $HASH{'subsystem'} = $1; }
      if($_ =~ /^needs_std (\w*)/) { $HASH{'needs_std'}{$1} = undef; }
      if($_ =~ /^uses_asm/) { $HASH{'uses_asm'} = 1; }

      # Read in a list of supported CPU types (archs and/or submodels)
      if($_ =~ /^<arch>$/) {
         while(1) {
            $_ = process($_ = <MODFILE>);
            next unless $_;
            last if ($_ =~ m#^</arch>$#);
            $HASH{'arch'}{$_} = undef;
            }
         }

      # Read in a list of supported OSes
      if($_ =~ /^<os>$/) {
         while(1) {
            $_ = process($_ = <MODFILE>);
            next unless $_;
            last if ($_ =~ m#^</os>$#);
            $HASH{'os'}{$_} = undef;
            }
         }

      # Read in a list of supported compilers
      if($_ =~ /^<cc>$/) {
         while(1) {
            $_ = process($_ = <MODFILE>);
            next unless $_;
            last if ($_ =~ m#^</cc>$#);
            $HASH{'cc'}{$_} = undef;
            }
         }
      }
   return %HASH;
   }

sub load_module {
   my ($modname,$cc,$os,$arch,$sub,$no_asm,%module) = @_;

   # Check to see if everything is cool about system requirements
   if(defined($module{'os'}) and !exists($module{'os'}{$os}) and
         $os ne 'generic')
       { die "ERROR: Module '$modname' does not support $REALNAME{$os}\n"; }

   foreach (sort keys %{ $module{'needs_std'} }) {
       unless($os eq 'generic' or in_array($OS_SUPPORTS_STD{$os}, $_))
       {
          my $stdname = $_;
          if($REALNAME{$stdname}) { $stdname = $REALNAME{$stdname}; }
          die "ERROR: Module '$modname' needs $stdname, but $REALNAME{$os} ",
              "doesn't support it\n";
       }
    }

   if(defined($module{'arch'}) and $arch ne 'generic' and
      !exists($module{'arch'}{$arch}) and !exists($module{'arch'}{$sub}))
       { die "ERROR: Module '$modname' does not support ".
              "$REALNAME{$arch}/$sub\n"; }

   if(defined($module{'cc'}) and !exists($module{'cc'}{$cc}))
       { die "ERROR: Module '$modname' does not support $REALNAME{$cc}\n"; }

   if($no_asm and exists($module{'uses_asm'}))
       { print STDERR "WARNING: Module '$modname' disabled (uses assembly)\n";
         return; }

   handle_files($modname, $module{'replace'}, \&replace_file);
   handle_files($modname, $module{'ignore'},  \&ignore_file);
   handle_files($modname, $module{'add'},     \&add_file);
   handle_files($modname, $module{'patch'} ,  \&patch_file);
   }

sub handle_files {
   my($modname, $hash_scalar, $func) = @_;
   return unless defined($hash_scalar);
   my %hash = %{ $hash_scalar };
   foreach (sort keys %hash) {
      if(defined($hash{$_})) { &$func($modname, $_, $hash{$_}); }
      else                   { &$func($modname, $_); }
    }
}

sub full_path {
   my ($file,$modname) = @_;
   if(defined($modname))
      { return File::Spec->catfile ($MOD_DIR, $modname, $file); }
   else {
      if($file =~ /\.h$/)
         { return File::Spec->catfile ($INCLUDE_DIR, $file); }
      elsif($file =~ /\.icc$/)
         { return File::Spec->catfile ($INCLUDE_DIR, $file); }
      elsif($file =~ /\.cpp$/)
         { return File::Spec->catfile ($SRC_DIR, $file); }
      else { die "ERROR (internal): Not sure where to put $file\n"; }
   }
}

sub patch_file {
   my ($modname,$file,$patch) = @_;
   ignore_file($modname, $file); # ignore the original
   $patch = full_path($patch, $modname);
   check_for_file($patch, $modname);

   my $patched_file = File::Spec->catfile ($MOD_DIR, $modname, $file);

   # NOTE: If the file to be patched already exists, we assume that it has
   # already been patched as well. This could theoretically break if, for
   # example, the system was configured, then one of the files the patch was
   # based on was edited. It seems like the Right Thing, because it prevents
   # the patched files from being remade every time the system is
   # re-configured.
   if( ! -e $patched_file ) { 
      copy(full_path($file), $patched_file);

      my @cmds = ();
      if($OS_TYPE{$os} eq 'unix' or $OS_TYPE{$os} eq 'beos')
         { @cmds = ('patch', '-i', "$patch", "$patched_file"); }
      else { die "Don't know how to patch on $os\n"; }

      if(system(@cmds) != 0) { die "ERROR: Could not execute patch\n"; }
      }

   if($patched_file =~ /.cpp$/)
      { $added_src{$file} = File::Spec->catdir($MOD_DIR, $modname); }
   else { $added_include{$file} = File::Spec->catdir($MOD_DIR, $modname); }
   $patched{$file} = $modname;
   }

sub add_file {
   my ($modname,$file) = @_;
   check_for_file(full_path($file, $modname), $modname);
   if($file =~ /.cpp$/)
        { $added_src{$file} = File::Spec->catdir($MOD_DIR, $modname); }
   else { $added_include{$file} = File::Spec->catdir($MOD_DIR, $modname); }
}

sub ignore_file {
   my ($modname,$file) = @_;
   check_for_file(full_path($file), $modname);
   if($file =~ /.cpp$/) { $ignored_src{$file} = 1; }
   else                 { $ignored_include{$file} = 1; }
}

# This works because ignore file always runs on files in the main source tree,
# and add always works on the file in the modules directory.
sub replace_file {
   my ($modname,$file) = @_;
   ignore_file($modname, $file);
   add_file($modname, $file);
}

sub help
   {
   print <<ENDOFHELP;
Usage: $0 [options] CC-OS-CPU

Options:
  --debug: tune compiler flags for debugging; inferior code can result
  --disable-asm: disable all assembly (implied by --debug)
  --disable-shared: disable building shared libararies
  --make-style=STYLE: override the guess as to what type of makefile to use
  --modules=MODS: add module(s) MODS to the library.

You may use 'generic' for OS or CPU (useful if your OS or CPU isn't listed).

CPU can be a generic family name or a specific model name. Common aliases are
supported but not listed. Choosing a specific submodel will usually result in
code that will not run on earlier versions of that architecture.

ENDOFHELP
   print_listing('CC', %CC_BINARY_NAME);
   print_listing('OS', %OS_SUPPORTS_ARCH);
   print_listing('CPU', %DEFAULT_SUBMODEL);
   if(%MODULES) { print_listing('MODS', %MODULES); }
   exit;
   }

sub print_listing
   {
   my ($header, %hash) = @_;
   print "$header: ";
   my $len = length "$header: ";
   foreach(sort(keys %hash))
      {
      if($len > 65) { print "\n   "; $len = 3; }
      print "$_ ";
      $len += length "$_ ";
      }
   print "\n";
   }

##################################################
# Generate compiler options and print makefile   #
##################################################
sub generate_makefile {
   my($make_style, $cc, $os, $submodel, $arch,
      $debug, $no_asm, $no_shared,
      $lib_src, $check_src, $includes,
      $added_src, $added_includes, $patched,
      @libs_used) = @_;

   my %all_lib_srcs = (%{ $lib_src }, %{ $added_src });
   my %all_includes = (%{ $includes }, %{ $added_includes });

   ##################################################
   # Set basic optimization options                 #
   ##################################################
   my $lib_opt_flags = $CC_LIB_OPT_FLAGS{$cc};
   if(!$debug and ($CC_NO_DEBUG_FLAGS{$cc}))
      { $lib_opt_flags .= ' '.$CC_NO_DEBUG_FLAGS{$cc}; }
   if($debug and ($CC_DEBUG_FLAGS{$cc}))
      { $lib_opt_flags .= ' '.$CC_DEBUG_FLAGS{$cc}; }

   ##################################################
   # Set machine dependent optimization options     #
   ##################################################
   my $mach_opt_flags = "";
   if(defined($CC_MACHINE_OPT_FLAGS{$cc}{$submodel}))
      { $mach_opt_flags = $CC_MACHINE_OPT_FLAGS{$cc}{$submodel}; }
   elsif(defined($CC_MACHINE_OPT_FLAGS{$cc}{$arch})) {
      $mach_opt_flags = $CC_MACHINE_OPT_FLAGS{$cc}{$arch};
      my $processed_modelname = $submodel;
      if(defined($CC_MACHINE_OPT_FLAGS_RE{$cc}{$arch}))
         { $processed_modelname =~
              s/$CC_MACHINE_OPT_FLAGS_RE{$cc}{$arch}//; }
      $mach_opt_flags =~ s/SUBMODEL/$processed_modelname/g;
   }

   ##################################################
   # Figure out static library creation method      #
   ##################################################
   # This is a default that works on most Unix and Unix-like systems
   my $ar_command = "ar crs";
   my $ar_needs_ranlib = 0; # almost no systems need it anymore

   # See if there are any over-riding methods. We presume if CC is creating
   # the static libs, it knows how to create the index itself.
   if($CC_AR_COMMAND{$cc}) { $ar_command = $CC_AR_COMMAND{$cc}; }
   elsif($OS_AR_COMMAND{$os})
       {
       $ar_command = $OS_AR_COMMAND{$os};
       if($OS_AR_NEEDS_RANLIB{$os}) { $ar_needs_ranlib = 1; }
       }

   ##################################################
   # Set shared object options                      #
   ##################################################
   my $so_link_flags = "";
   my $so_obj_flags = $CC_SO_OBJ_FLAGS{$cc};

   if($no_shared or (!in_array($OS_SUPPORTS_SHARED{$os}, 'all') and
                     !in_array($OS_SUPPORTS_SHARED{$os}, $arch)))
      { $so_obj_flags = ""; }
 
  elsif(defined($CC_SO_LINK_FLAGS{$cc}{$os}))
      { $so_link_flags = $CC_SO_LINK_FLAGS{$cc}{$os}; }
   elsif(defined($CC_SO_LINK_FLAGS{$cc}{'default'}))
      { $so_link_flags = $CC_SO_LINK_FLAGS{$cc}{'default'}; }

   my $make_shared = 0;
   if(($so_obj_flags or $so_link_flags) and $OS_SUPPORTS_SHARED{$os})
      { $make_shared = 1; }

   ##################################################
   # Set misc ABI options                           #
   ##################################################
   my $ccopts = "";
   if(defined($CC_MACH_ABI_FLAGS{$cc}{$arch}))
      { $ccopts .= ' '.$CC_MACH_ABI_FLAGS{$cc}{$arch}; }
   if(defined($CC_MACH_ABI_FLAGS{$cc}{'all'}))
      { $ccopts .= ' '.$CC_MACH_ABI_FLAGS{$cc}{'all'}; }

   ##################################################
   # Where to install?                              #
   ##################################################
   # Stupid defaults, but what are you gonna do?
   my $install_root = '/usr/local';
   my $header_dir = 'include';
   my $lib_dir = 'lib';
   my $doc_dir = 'doc';

   unless($os eq 'generic')
      {
      $install_root = $INSTALL_INFO{$os}{'root'},
      $header_dir   = $INSTALL_INFO{$os}{'headers'},
      $lib_dir      = $INSTALL_INFO{$os}{'libs'},
      $doc_dir      = $INSTALL_INFO{$os}{'docs'},
      }

   ##################################################
   # Open the makefile                              #
   ##################################################
   open MAKEFILE, ">$MAKE_FILE"
      or die "Couldn't write $MAKE_FILE ($!)\n";

   ##################################################
   # Ready, set, print!                             #
   ##################################################
   my $cc_bin = $CC_BINARY_NAME{$cc};
   if($os eq "darwin" and $cc eq "gcc") { $cc_bin = "c++"; }

   my @arguments = (\*MAKEFILE,
                    $cc_bin . $ccopts,
                    $lib_opt_flags,
                    $CC_CHECK_OPT_FLAGS{$cc},
                    $mach_opt_flags,
                    $CC_LANG_FLAGS{$cc},
                    $CC_WARN_FLAGS{$cc},
                    $make_shared,
                    $so_obj_flags,
                    $so_link_flags,
                    $OS_SHARED_SUFFIX{$os},
                    $ar_command,
                    $ar_needs_ranlib,
                    \%all_lib_srcs,
                    $check_src,
                    \%all_includes,
                    $patched,
                    \%DOCS,
                    $install_root,
                    $header_dir,
                    $lib_dir,
                    $doc_dir,
                    \@libs_used);

   if($make_style eq 'unix') { print_unix_makefile(@arguments); }
   else {
      die "ERROR: This configure script does not know how to make ",
          "a makefile for makefile style \"$make_style\"\n";
   }
}

##################################################
# Print a header for a makefile                  #
##################################################
sub print_header {
   my ($fh, $comment, $string) = @_;
   print $fh $comment x 50, "\n",
             "$comment $string", ' 'x(47-length($string)), "$comment\n",
             $comment x 50, "\n";
}

##################################################
# Print a Unix style makefile                    #
##################################################
sub print_unix_makefile {
   my ($makefile,
       $cc, $lib_opt, $check_opt, $mach_opt, $lang_flags, $warn_flags, 
       $make_shared, $so_obj, $so_link, $so_suffix, $ar_command, $use_ranlib,
       $src_hash, $check_hash, $include_hash, $patched_hash, $docs, 
       $install_root, $header_dir, $lib_dir, $doc_dir,
       $lib_list) = @_;

   ##################################################
   # Some constants                                 #
   ##################################################
   my $__TAB__ = "\t";
   my $COMMENT_CHAR = '#';

   ##################################################
   # Convert the references to hashes               #
   ##################################################
   my %src = %{ $src_hash };
   my %includes = %{ $include_hash };

   my %check = %{ $check_hash };

   my %patched = %{ $patched_hash };

   my %docs = %{ $docs };

   ##################################################
   # Make the library linking list                  #
   ##################################################
   my $link_to = "";
   foreach (@{ $lib_list }) { $link_to .= " -l$_"; }
   chomp($link_to) if($link_to);

   ##################################################
   # Generate a few variables                       #
   ##################################################
   my $lib_flags   = '$(LIB_OPT) $(MACH_OPT) $(LANG_FLAGS) $(WARN_FLAGS)';

   my $libs = '$(STATIC_LIB)';
   if($make_shared) { $lib_flags .= ' $(SO_OBJ_FLAGS)';
                      $libs .= ' $(SHARED_LIB)'; }

##################### COMMON CODE (PARTIALLY) ######################
   sub file_list {
      my ($spaces, $put_in, $from, $to, %files) = @_;
      my $len = $spaces;
      my $list;
      foreach (sort keys %files) {
         my $file = $_;
         my $dir = $put_in;
         if(!defined($dir)) { $dir = $files{$_}; }
         if($len > 60)
            { $list .= "\\\n" . ' 'x$spaces; $len = $spaces; }
         if(defined($from) and defined($to)) { $file =~ s/$from/$to/; }
         if(defined($dir))
            { $list .= File::Spec->catfile ($dir, $file) . ' ';
              $len += length($file) + length($dir); }
         else
            { $list .= $file . ' ';
              $len += length($file); }
      }
      return $list;
   }

   my $includes = file_list(16, undef, undef, undef, %includes);

   my $lib_obj = file_list(16, $BUILD_LIB_DIR, '.cpp', '.o', %src, %added_src);
   my $check_obj = file_list(16, $BUILD_CHECK_DIR, '.cpp', '.o', %check);

   my $doc_list = file_list(16, undef, undef, undef, %docs);

   my $remove_patched_files = file_list(16, undef, undef, undef,
      map { $_ => File::Spec->catdir($MOD_DIR, $patched{$_}) } keys %patched);
   if(defined($remove_patched_files))
      { $remove_patched_files = "$__TAB__\$(RM) $remove_patched_files\n"; }
   else { $remove_patched_files = ""; }

##################### / COMMON CODE (PARTIALLY) ######################

   print_header($makefile, $COMMENT_CHAR, 'Compiler Options');
   print $makefile <<END_OF_MAKEFILE_HEADER;
CXX           = $cc
LIB_OPT       = $lib_opt
CHECK_OPT     = $check_opt
MACH_OPT      = $mach_opt
LANG_FLAGS    = $lang_flags
WARN_FLAGS    = $warn_flags
SO_OBJ_FLAGS  = $so_obj
SO_LINK_FLAGS = $so_link

END_OF_MAKEFILE_HEADER

   print_header($makefile, $COMMENT_CHAR, 'Version Numbers');
   print $makefile <<END_OF_VERSIONS;
MAJOR         = $MAJOR_VERSION
MINOR         = $MINOR_VERSION
PATCH         = $PATCH_VERSION

VERSION       = \$(MAJOR).\$(MINOR).\$(PATCH)

END_OF_VERSIONS

   print_header($makefile, $COMMENT_CHAR, 'Installation Settings');
   print $makefile <<END_OF_INSTALL_SETTINGS;
INSTALLROOT   = $install_root

LIBDIR        = \$(INSTALLROOT)/$lib_dir
HEADERDIR     = \$(INSTALLROOT)/$header_dir/opencl
DOCDIR        = \$(INSTALLROOT)/$doc_dir/OpenCL-\$(VERSION)

OWNER         = root
GROUP         = root
DATA_MODE     = 644
EXEC_MODE     = 755

END_OF_INSTALL_SETTINGS

   print_header($makefile, $COMMENT_CHAR, 'Aliases for Common Programs');
   print $makefile <<END_OF_COMMAND_ALIASES;
AR            = $ar_command
CD            = \@cd
ECHO          = \@echo
INSTALL       = install
INSTALL_CMD   = \$(INSTALL) -c -o \$(OWNER) -g \$(GROUP) -m
LN            = ln -fs
MKDIR         = \@mkdir
MKDIR_INSTALL = \@umask 022; mkdir -p -m \$(EXEC_MODE)
STRIP         = \@strip
RANLIB        = \@ranlib
RM            = \@rm -f
RM_R          = \@rm -rf

END_OF_COMMAND_ALIASES

   print_header($makefile, $COMMENT_CHAR, 'File Lists');
   print $makefile <<END_OF_FILE_LISTS;
LIB_FLAGS     = $lib_flags
CHECK_FLAGS   = \$(CHECK_OPT) \$(LANG_FLAGS) \$(WARN_FLAGS)

CHECK         = check

DOCS          = $doc_list

HEADERS       = $includes

LIBOBJS       = $lib_obj

CHECKOBJS     = $check_obj

LIBRARIES     = $libs

LIBNAME       = libopencl
STATIC_LIB    = \$(LIBNAME).a

END_OF_FILE_LISTS

   if($make_shared) {
       print $makefile <<END_OF_SHARED_LIB_DECL;
SHARED_LIB    = \$(LIBNAME)-\$(MAJOR).\$(MINOR).\$(PATCH).$so_suffix
SONAME        = \$(LIBNAME)-\$(MAJOR).\$(MINOR).\$(PATCH).$so_suffix

SYMLINK0      = \$(LIBNAME)-\$(MAJOR).\$(MINOR).\$(PATCH).$so_suffix
SYMLINK1      = \$(LIBNAME)-\$(MAJOR).\$(MINOR).$so_suffix
SYMLINK2      = \$(LIBNAME)-\$(MAJOR).$so_suffix
SYMLINK3      = \$(LIBNAME).$so_suffix
SYMLINKS      = \$(SYMLINK1) \$(SYMLINK2) \$(SYMLINK3)

END_OF_SHARED_LIB_DECL
   }

   print $makefile "all: \$(LIBRARIES)\n\n";
   print_header($makefile, $COMMENT_CHAR, 'Build Commands');

   sub print_build_cmds {
      my ($fh, $dir, $flags, %files) = @_;
      foreach (sort keys %files) {
         my $src_file = File::Spec->catfile ($files{$_}, $_);
         my $obj_file = File::Spec->catfile ($dir, $_);
         $obj_file =~ s/.cpp/.o/;
         print $fh "$obj_file: $src_file\n",
            "\t\$(CXX) -Iinclude/ $flags -c \$? -o \$@\n\n";
      }
   }

   print_build_cmds($makefile, $BUILD_LIB_DIR,
                    '$(LIB_FLAGS)', %src, %added_src);

   print_build_cmds($makefile, $BUILD_CHECK_DIR,
                    '$(CHECK_FLAGS)', %check);

   print_header($makefile, $COMMENT_CHAR, 'Link Commands');

   print $makefile <<END_OF_LINK_COMMANDS;
\$(CHECK): \$(LIBRARIES) \$(CHECKOBJS)
$__TAB__\$(CXX) \$(CHECKOBJS) -L. -lm $link_to -lopencl -o \$(CHECK)
$__TAB__\$(STRIP) \$(CHECK)
$__TAB__\@./\$(CHECK) --validate

\$(STATIC_LIB): \$(LIBOBJS)
$__TAB__\$(RM) \$(STATIC_LIB)
$__TAB__\$(AR) \$(STATIC_LIB) \$(LIBOBJS)
END_OF_LINK_COMMANDS

   if($use_ranlib) { print $makefile "$__TAB__\$(RANLIB) \$(STATIC_LIB)\n\n"; }
   else { print $makefile "\n"; }

   if($make_shared) {
      print $makefile <<END_OF_SO_LINK_COMMAND;
\$(SHARED_LIB): \$(LIBOBJS)
$__TAB__\$(CXX) \$(SO_LINK_FLAGS) $link_to \$(LIBOBJS) -o \$(SHARED_LIB)
$__TAB__\$(LN) \$(SYMLINK0) \$(SYMLINK1)
$__TAB__\$(LN) \$(SYMLINK1) \$(SYMLINK2)
$__TAB__\$(LN) \$(SYMLINK2) \$(SYMLINK3)

END_OF_SO_LINK_COMMAND
    }

   print_header($makefile, $COMMENT_CHAR, 'Misc Targets');
   print $makefile "static: \$(STATIC_LIB)\n\n";
   if($make_shared) { print $makefile "shared: \$(SHARED_LIB)\n\n"; }

   print_header($makefile, $COMMENT_CHAR, 'Fake Targets');
   print $makefile <<END_OF_FAKE_TARGETS;
.PHONY = clean distclean install

clean:
$__TAB__\$(RM_R) $BUILD_LIB_DIR/* $BUILD_CHECK_DIR/*
$__TAB__\$(RM) \$(LIBRARIES) \$(SYMLINKS) \$(CHECK)

distclean: clean
$__TAB__\$(RM_R) $BUILD_DIR $CPP_INCLUDE_DIR
$__TAB__\$(RM) $MAKE_FILE $CONFIG_H_FILE $VERSION_CPP_FILE
$remove_patched_files
END_OF_FAKE_TARGETS

   print_header($makefile, $COMMENT_CHAR, 'Install Commands');

   print $makefile <<END_OF_INSTALL_SCRIPTS;
install: \$(LIBRARIES)
$__TAB__\$(ECHO) "Installing OpenCL into \$(INSTALLROOT)... "
$__TAB__\$(MKDIR_INSTALL) \$(DOCDIR)
$__TAB__\$(MKDIR_INSTALL) \$(HEADERDIR)
$__TAB__\$(MKDIR_INSTALL) \$(LIBDIR)
$__TAB__\@for i in \$(DOCS); do \\
$__TAB__   \$(INSTALL_CMD) \$(DATA_MODE) \$\$i \$(DOCDIR); \\
$__TAB__ done
$__TAB__\@for i in \$(HEADERS); do \\
$__TAB__   \$(INSTALL_CMD) \$(DATA_MODE) \$\$i \$(HEADERDIR); \\
$__TAB__ done
$__TAB__\@\$(INSTALL_CMD) \$(DATA_MODE) \$(STATIC_LIB) \$(LIBDIR)
END_OF_INSTALL_SCRIPTS

   if($make_shared) {
      print $makefile <<END_OF_SYMLINKS;
$__TAB__\@\$(INSTALL_CMD) \$(EXEC_MODE) \$(SHARED_LIB) \$(LIBDIR)
$__TAB__\$(CD) \$(LIBDIR); \$(LN) \$(SYMLINK0) \$(SYMLINK1)
$__TAB__\$(CD) \$(LIBDIR); \$(LN) \$(SYMLINK1) \$(SYMLINK2)
$__TAB__\$(CD) \$(LIBDIR); \$(LN) \$(SYMLINK2) \$(SYMLINK3)

END_OF_SYMLINKS
   }
   else { print $makefile "\n"; }

}

