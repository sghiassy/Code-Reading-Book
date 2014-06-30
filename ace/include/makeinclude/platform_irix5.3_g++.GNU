# platform_irix5.3_g++.GNU,v 4.10 1997/09/22 00:37:52 levine Exp

# Irix 5.3 with GNU C++

debug = 1
optimize = 1

CC              = gcc
CXX             = g++
CCFLAGS		+= $(CFLAGS) -fno-implicit-templates
DCFLAGS		+= -g
DLD             = $(CXX)
LD		= $(CXX) 
LDFLAGS         += -L$(ACE_ROOT)/ace
OCFLAGS		+= -O2
PIC             = -fpic
RANLIB          = echo
SOFLAGS         = -shared $(CPPFLAGS)
SOBUILD         = $(COMPILE.cc) $(PIC) -o $(VSHDIR)$*.o $<; \
                  $(SOLINK.cc) -o $@ $(LDFLAGS) $(VSHDIR)$*.o

