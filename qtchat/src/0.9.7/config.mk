####################
# Path Definitions #
####################

# Where to install Qt.  Qt installs to $(PREFIX)/bin and $(PREFIX)/lib.
# Default is the value of environment variable PREFIX if this is commented
# out, else the top qtchat/ directory of this archive if neither has been set.
#PREFIX = /usr/local

# Where X is installed.
export X11DIR := /usr/X11R6

# Where local programs are installed.
export LOCALDIR := /usr/local

# Where Qt is installed.  $(QT_DIR), $(QT_DIR)/lib and $(QT_DIR)/include
# are each searched for libraries and include files.  Environment
# variable QTDIR (no underscore) is often pre-set on many systems; it will
# be used iff this variable isn't set.
#QT_DIR = /usr/local/qt

# If defined, where Qt libraries are installed,
# else, search for libraries in well-known directories.
#QT_LIB_DIR := $(QTDIR)/lib

# If defined, where Qt include files are installed,
# else, search for include files in well-known directories.
#QT_INCLUDE_DIR := $(QTDIR)/include

###############################
# Build Options / Linux hacks #
###############################

# If "1", build qtchat with debug symbols and using extra warning flags.
# Increases executable/library size.  Set to "0" to disable the -Werror
# flag which has caused problems on some platforms.
DEBUG_MODE = 1

# If "static", build qtchat as statically linked executable,
# else build qtchat as dynamically linked executable with shared libs.
# (static linking is currently broken)
#LINK_MODE = shared
LINK_MODE = static

#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
#=#=#=#=#=#=#=#=#=#    Configuration Failsafe Section     #=#=#=#=#=#=#=#=#=# 
#=#=#=#=#=#=#=#=#=#    Defaults Generally Acceptable      #=#=#=#=#=#=#=#=#=# 
#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#

# Possible choices for the Qt include directory if QT_INCLUDE_DIR
# is not defined.  These work on FreeBSD and RedHat; email diffs for
# your platform to me.
QT_INCLUDE_DIR_CHOICES := \
	$(X11DIR)/include/X11/qt \
	$(X11DIR)/include \
	$(LOCALDIR)/include \
	$(LOCALDIR)/qt/include \
	/usr/include/qt \
	/usr/include/Qt \
	/usr/lib/qt

# Possible choices for the Qt lib directory if QT_LIB_DIR is not
# defined.  These work on FreeBSD and RedHat; email diffs for
# your platform to me.
QT_LIB_DIR_CHOICES := \
	$(X11DIR)/lib/X11/qt \
	$(X11DIR)/lib \
	$(LOCALDIR)/lib \
	$(LOCALDIR)/qt/lib \
	/usr/lib/qt \
	/usr/lib/Qt

#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
#=#=#=#=#=#=#=#=#=#     End User Configurable Section     #=#=#=#=#=#=#=#=#=# 
#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#

# Toplevel source directory (containing this file).  Usually doesn't
# have to be configured; may be changed to build a different version 
# of QtChat in another directory.
ifndef BASEDIR
BASEDIR = $(shell pwd)
endif
export BASEDIR

ifndef PREFIX
PREFIX = $(BASEDIR)/../..
endif
export PREFIX

ifeq "X$(QT_DIR)" "X"
ifeq "X$(QTDIR)" "X"
QT_DIR = .
else
QT_DIR = $(QTDIR)
endif
endif

QT_INCLUDE_DIR_CHOICES := $(QT_DIR) $(QT_DIR)/include $(QT_INCLUDE_DIR_CHOICES)
QT_LIB_DIR_CHOICES := $(QT_DIR) $(QT_DIR)/lib $(QT_LIB_DIR_CHOICES)

# find the correct paths from the lists
ifeq "X$(QT_INCLUDE_DIR)" "X"
QT_INCLUDE_DIR = $(word 1, $(dir $(wildcard $(addsuffix /qwizard.h,$(QT_INCLUDE_DIR_CHOICES)))))
endif
export QT_INCLUDE_DIR

ifeq "X$(QT_LIB_DIR)" "X"
QT_LIB_DIR = $(word 1, $(dir $(wildcard $(addsuffix /libqt.so.2,$(QT_LIB_DIR_CHOICES)))))
endif
export QT_LIB_DIR


#######################
# Default Paths/Files #
#######################

SUBDIRS :=
CLEAN_FILES := *.o *.lo *.moc.cc *.core

#####################
# Compilation flags #
#####################

INCLUDE_FLAGS := -I$(QT_INCLUDE_DIR) -I$(BASEDIR)/core -I$(BASEDIR)/Tree
INCLUDE_FLAGS += -I$(BASEDIR)/gui -I$(BASEDIR)/net -I$(BASEDIR)
export INCLUDE_FLAGS
LIB_FLAGS := -L$(BASEDIR)/lib -L$(QT_LIB_DIR) -L$(X11DIR)/lib
LIB_FLAGS += -lm -lX11 -lXext
LIB_FLAGS += -lqt
LIB_FLAGS += -lgui -lnet -lcore 
export LIB_FLAGS
ifeq ($(DEBUG_MODE), 1)
export CXXFLAGS := -Wall -Werror -ansi -pedantic -g
else
export CXXFLAGS :=
endif
ifneq "X$(wildcard $(QT_INCLUDE_DIR)/qnetworkprotocol.h)" "X"
export QT_2_1 := 1
else
export QT_2_1 := 0
endif

ifeq "$(LINK_MODE)" "static"
export SHARED_FLAGS := -static
export SHARED_LINK_FLAGS := cr
export LIB_EXTENSION := a
export LINK_CMD := ar
else
export SHARED_FLAGS := -shared
export SHARED_LINK_FLAGS := -shared -Bdynamic
export LIB_EXTENSION := so.$(DISTVER)
export LINK_CMD := ld
endif

##################
# Implicit Rules #
##################

phony: all

%.so: %.so.$(DISTVER)
	ln -sf $^ $@

%.o %.lo: %.cc
#	$(LIBTOOL) $(CXX) -c $(CXXFLAGS) $(SHARED_FLAGS) $(INCLUDE_FLAGS) -o $@ $^
	$(CXX) -c -DAP_VER_STR=$(AP_VER_STR) -DQT_2_1=$(QT_2_1) $(CXXFLAGS) $(SHARED_FLAGS) $(INCLUDE_FLAGS) -o $@ $^

%.o %.lo: %.cpp
#	$(LIBTOOL) $(CXX) -c $(CXXFLAGS) $(SHARED_FLAGS) $(INCLUDE_FLAGS) -o $@ $^
	$(CXX) -c $(CXXFLAGS) $(SHARED_FLAGS) $(INCLUDE_FLAGS) -o $@ $^

%.moc.cc: %.h
	$(MOC) $< -o $@

###################
# Default targets #
###################

clean:
	for i in $(SUBDIRS); do \
		(cd $$i && $(MAKE) clean); \
	done
	$(RM) $(CLEAN_FILES)

distclean:
	for i in $(SUBDIRS); do \
		(cd $$i && $(MAKE) distclean); \
	done
	$(RM) $(CLEAN_FILES) $(DISTCLEAN_FILES)

#######################
# Program definitions #
#######################

INSTALL_CMD := /usr/bin/install
INSTALL_FILES_CMD := $(INSTALL_CMD) -c -m 0755
INSTALL_DIR_CMD := $(INSTALL_CMD) -d -m 0755
RM := rm -f
ifeq ("$(findstring BSD,$(shell uname))",  "BSD")
LDCONFIG := ldconfig -m
else
LDCONFIG := ldconfig
endif
LIBTOOL := libtool
ifeq ("$(QT_DIR)", "")
MOC := $(QT_DIR)/bin/moc
else
MOC := $(QT_INCLUDE_DIR)/../bin/moc
endif

.SUFFIXES: .so .so.$(DISTVER) .cc .a
