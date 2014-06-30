include ../config.mk

DISTNAME = libnet
DISTFILES = nc.cc nc.h net.cc net.h GNUmakefile
CLEAN_FILES := *.o *.lo *.la *.o *.core *.moc.cc
DISTCLEAN_FILES := $(DISTNAME).* $(CLEAN_FILES)
SUBDIRS :=

OBJS := nc.o net.o net.moc.o

all: $(DISTNAME).$(LIB_EXTENSION)

$(DISTNAME).$(LIB_EXTENSION): $(OBJS)
ifeq "$(LINK_MODE)" "shared"
	$(LINK_CMD) $(SHARED_LINK_FLAGS) -o $@ $^
else
	$(LINK_CMD) $(SHARED_LINK_FLAGS) $@ $^
	ranlib $@
endif


dist: ${DISTNAME}-${DISTVER}.tar.gz

${DISTNAME}-${DISTVER}.tar.gz: ${DISTFILES}
	tar cvf ${DISTNAME}-${DISTVER}.tar -C $(BASEDIR) $(foreach filename, $(DISTFILES), net/$(filename))
	gzip ${DISTNAME}-${DISTVER}.tar

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

.PRECIOUS: net.moc.cc
