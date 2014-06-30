# platform_sco-nothread.GNU,v 4.16 1997/09/22 00:37:57 levine Exp

debug = 1
optimize = 1

CC              = gcc
CXX		= g++
CFLAGS		+= -w
CCFLAGS		+= $(CFLAGS) -fno-implicit-templates -fno-strict-prototypes
DCFLAGS		+= -g #### should this be -gstabs?
DLD             = $(CXX)
LD		= $(CXX)
LIBS            = -lsocket
OCFLAGS		= -O2
PIC             =
AR              = ar
ARFLAGS         = ruv
RANLIB          = /bin/true

SOFLAGS = $(CPPFLAGS)
SOBUILD = $(COMPILE.cc) $(PIC) -o $(VSHDIR)$*.so $<
PRELIB  = @true
