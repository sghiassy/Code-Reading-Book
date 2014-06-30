include ../config.mk

# Distribution variables
DISTNAME := libcore
DISTFILES = GNUmakefile  $(SUBDIRS) chatter.cc chatter.h
DISTFILES = roomtree.cc roomtree.h serializable.cc serializable.h
DISTFILES += signalhandler.cc signalhandler.h state.cc, state.h 
DISTFILES += util.cc util.h defs.h
TAR_DISTFILES = $(foreach i, $(SUBDIRS), $(i)/$(i).tar)
CLEAN_FILES := *.o *.lo *.la *.o *.core
DISTCLEAN_FILES := $(DISTNAME).* $(CLEAN_FILES)

#OBJS := chatter.o emotion.o signalhandler.o state.o util.o
OBJS := roomtree.o chatter.o serializable.o signalhandler.o state.o util.o

# Path configuration
ifndef BASEDIR
BASEDIR := ..
endif
SUBDIRS :=

# Compilation flags
INCLUDE_FLAGS += -I$(BASEDIR)/core

all: $(DISTNAME).$(LIB_EXTENSION)

$(DISTNAME).$(LIB_EXTENSION): $(OBJS)
#	$(LIBTOOL) $(CXX) -o $@ $(SHARED_LINK_FLAGS) $^
ifeq "$(LINK_MODE)" "shared"
	$(LINK_CMD) $(SHARED_LINK_FLAGS) -o $@ $^
else
	$(LINK_CMD) $(SHARED_LINK_FLAGS) $@ $^
	ranlib $@
endif

dist: $(DISTNAME)-$(DISTVER).tar.gz

$(DISTNAME)-$(DISTVER).tar.gz: $(DISTFILES)
	$(MAKE) distclean
	tar cvf $(DISTNAME)-$(DISTVER).tar -C .. $(foreach filename, $(DISTFILES), $(DISTNAME)-$(DISTVER)/$(filename))
	gzip $(DISTNAME)-$(DISTVER).tar

install: $(DISTNAME).$(LIB_EXTENSION)
	cp $@ $(BASEDIR)/lib
ifneq "$(LINK_MODE)" "static"
	ln -sf $(BASEDIR)/lib/$@ $(BASEDIR)/lib/$(DISTNAME).so
endif

uninstall:
	for i in $(SUBDIRS); do \
		(cd $$i && $(MAKE) uninstall) \
	done
	$(RM) $(PREFIX)/lib/$(DISTNAME).$(LIB_EXTENSION)
ifneq "$(LINK_MODE)" "static"
	$(RM) $(PREFIX)/lib/$(DISTNAME).so
endif
