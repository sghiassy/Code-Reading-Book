DISTNAME = libwc
DISTVER = 0.1
DISTFILES = GNUmakefile wc.cc wc.h test.cc
CLEAN_FILES := *.o *.lo *.la *.core
DISTCLEAN_FILES := $(DISTNAME).* $(CLEAN_FILES) test

SUBDIRS :=

OBJS = wc.o

all:

$(DISTNAME).$(LIB_EXTENSION): $(OBJS)
ifeq "$(LINK_MODE)" "shared"
	$(LINK_CMD) $(SHARED_LINK_FLAGS) -o $@ $^
else
	$(LINK_CMD) $(SHARED_LINK_FLAGS) $@ $^
	ranlib $@
endif

test: test.o
	$(CXX) $(CXXFLAGS) -o $@ $^

dist: ${DISTNAME}-${DISTVER}.tar.gz

install: 

uninstall:
	for i in $(SUBDIRS); do \
		(cd $$i && $(MAKE) uninstall) \
	done
	$(RM) $(PREFIX)/lib/$(DISTNAME).$(LIB_EXTENSION)
ifneq "$(LINK_MODE)" "static"
	$(RM) $(PREFIX)/lib/$(DISTNAME).so
endif

clean:
	rm -f $(CLEAN_FILES)

distclean:
	rm -f $(DISTCLEAN_FILES)
