##################################################
# Version numbers and other config options       #
##################################################
%define MAJOR $MAJOR
%define MINOR $MINOR
%define PATCH $PATCH

# This is the list of modules built into the library.
%define MODLIST bzip2,zlib,devrand,gettimeofday,pipe_unixfd

##################################################
# Descriptions                                   #
##################################################
%define VERSION %{MAJOR}.%{MINOR}.%{PATCH}

Name: OpenCL
Summary: A C++ crypto library
Version: %{VERSION}
Release: 1
Copyright: BSD
Group: System Environment/Libraries
Requires: bzip2 >= 1.0.0, zlib >= 1.1.3
Source: http://prdownloads.sf.net/opencl/OpenCL-%{VERSION}.tgz
URL: http://opencl.sf.net
Packager: Jack Lloyd <lloyd@acm.jhu.edu>
BuildRoot: /var/tmp/OpenCL-rpmroot
Prefix: /usr/local

%description
OpenCL is a C++ crypto library, designed for high portability and ease of
use. It includes a wide selection of block and stream ciphers, hash functions,
MACs, and various utility classes and functions, plus a high-level filter based
interface.

%package devel
Summary: Development files for OpenCL
Group: Development/Libraries
Requires: OpenCL = %{VERSION}, zlib-devel
%description devel
The OpenCL-devel package contains the header files and libraries needed to
develop programs that use the OpenCL crypto library.

##################################################
# Main Logic                                     #
##################################################
%prep
%setup -n OpenCL-%{VERSION}

%build
./configure.pl --modules=%{MODLIST} gcc-%{_target_os}-%{_target_cpu}
make shared static

%clean
rm -rf $RPM_BUILD_ROOT

%install
ROOT="$RPM_BUILD_ROOT/usr/local"
make OWNER=`id -u` GROUP=`id -g` INSTALLROOT="$ROOT" install

# Need this since we're installing shared libs...
%post
if ! grep "^$RPM_INSTALL_PREFIX/lib$" /etc/ld.so.conf 2>&1 >/dev/null
then
   echo "$RPM_INSTALL_PREFIX/lib" >>/etc/ld.so.conf
fi
/sbin/ldconfig

%post devel
if [ $RPM_INSTALL_PREFIX != "/usr" -a -d "/usr/include" ]
then
   ln -s $RPM_INSTALL_PREFIX/include/opencl /usr/include/opencl
fi

%postun
/sbin/ldconfig
if [ -d $RPM_INSTALL_PREFIX/share/doc/OpenCL-%{VERSION} ]; then
   rmdir --ignore $RPM_INSTALL_PREFIX/share/doc/OpenCL-%{VERSION}
fi

%postun devel
if [ $RPM_INSTALL_PREFIX != "/usr" ]
then
   if [ -L /usr/include/opencl ]
   then
      rm -f /usr/include/opencl
   fi
fi
if [ -d $RPM_INSTALL_PREFIX/include/opencl ]; then
   rmdir --ignore $RPM_INSTALL_PREFIX/include/opencl
fi

##################################################
# File Lists                                     #
##################################################
%files
%attr(-,root,root) %docdir /usr/local/share/doc/OpenCL-%{VERSION}/
%attr(-,root,root) /usr/local/share/doc/OpenCL-%{VERSION}/license.txt
%attr(-,root,root) /usr/local/share/doc/OpenCL-%{VERSION}/readme.txt
%attr(-,root,root) /usr/local/share/doc/OpenCL-%{VERSION}/log.txt
%attr(-,root,root) /usr/local/share/doc/OpenCL-%{VERSION}/thanks.txt
%attr(-,root,root) /usr/local/share/doc/OpenCL-%{VERSION}/authors.txt
%attr(-,root,root) /usr/local/share/doc/OpenCL-%{VERSION}/relnotes.txt
%attr(-,root,root) /usr/local/share/doc/OpenCL-%{VERSION}/pgpkeys.asc
%attr(-,root,root) /usr/local/lib/libopencl.so
%attr(-,root,root) /usr/local/lib/libopencl-%{MAJOR}.so
%attr(-,root,root) /usr/local/lib/libopencl-%{MAJOR}.%{MINOR}.so
%attr(-,root,root) /usr/local/lib/libopencl-%{MAJOR}.%{MINOR}.%{PATCH}.so

%files devel
%attr(-,root,root) %docdir /usr/local/share/doc/OpenCL-%{VERSION}/
%attr(-,root,root) /usr/local/share/doc/OpenCL-%{VERSION}/indent.el
%attr(-,root,root) /usr/local/share/doc/OpenCL-%{VERSION}/opencl.ps
%attr(-,root,root) /usr/local/share/doc/OpenCL-%{VERSION}/opencl.tex
%attr(-,root,root) /usr/local/lib/libopencl.a
%attr(-,root,root) /usr/local/include/opencl/
