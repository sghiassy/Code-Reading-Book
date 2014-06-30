include ../config.mk

CLEAN_FILES := *.o *.core
DISTCLEAN_FILES := lib*.* lib*.a $(CLEAN_FILES)
SUBDIRS := 
LIBS := libcore.$(LIB_EXTENSION) libgui.$(LIB_EXTENSION)
LIBS += libnet.$(LIB_EXTENSION) libTree.a
ifneq "$(LINK_MODE)" "static"
LIBS += libcore.so libgui.so libnet.so
endif

all:
	cd $(BASEDIR)/core && $(MAKE) -j 1 libcore.$(LIB_EXTENSION)
	cd $(BASEDIR)/gui && $(MAKE) -j 1 libgui.$(LIB_EXTENSION)
	cd $(BASEDIR)/net && $(MAKE) -j 1 libnet.$(LIB_EXTENSION)
	cd $(BASEDIR)/Tree && $(MAKE) -j 1 libTree.a

	cp $(BASEDIR)/core/libcore.$(LIB_EXTENSION) .
	cp $(BASEDIR)/gui/libgui.$(LIB_EXTENSION) .
	cp $(BASEDIR)/net/libnet.$(LIB_EXTENSION) .
	cp $(BASEDIR)/Tree/libTree.a .

ifeq "$(LINK_MODE)" "shared"
	ln -sf ./libcore.$(LIB_EXTENSION) ./libcore.so
	ln -sf ./libgui.$(LIB_EXTENSION) ./libgui.so
	ln -sf ./libnet.$(LIB_EXTENSION) ./libnet.so
endif

libcore.$(LIB_EXTENSION): $(BASEDIR)/core/libcore.$(LIB_EXTENSION)
	cp $^ $@

libgui.$(LIB_EXTENSION): $(BASEDIR)/gui/libgui.$(LIB_EXTENSION)
	cp $^ $@

libnet.$(LIB_EXTENSION): $(BASEDIR)/net/libnet.$(LIB_EXTENSION)
	cp $^ $@
	
libTree.a: $(BASEDIR)/Tree/libTree.a
	cp $^ $@
	
install: all
	$(INSTALL_DIR_CMD) $(PREFIX)/lib
	for i in core net gui; do \
		$(INSTALL_FILES_CMD) lib$${i}.$(LIB_EXTENSION) $(PREFIX)/lib; \
		ln -sf $(PREFIX)/lib/lib$${i}.$(LIB_EXTENSION) $(PREFIX)/lib/lib$${i}.so; \
	done;
	$(INSTALL_FILES_CMD) libTree.a $(PREFIX)/lib
ifeq "$(shell id -u)" "0"
  ifeq "$(findstring BSD,$(shell uname))" "BSD"
		$(LDCONFIG) -m $(PREFIX)/lib
  else
		$(LDCONFIG) $(PREFIX)/lib
  endif
else
	@echo
	@echo "******************* W A R N I N G **************************"
	@echo "*** You must be root in order to run ldconfig.  Consider ***"
	@echo "*** setting environment variable LD_LIBRARY_PATH to      ***"
	@printf "*** %-52s ***\n" $(PREFIX)/lib
	@echo "******************* W A R N I N G **************************"
	@echo
endif

lib%.so: lib%.so.$(DISTVER)
	ln -sf $^ $@

$(BASEDIR)/core/libcore.$(LIB_EXTENSION):

$(BASEDIR)/net/libnet.$(LIB_EXTENSION):

$(BASEDIR)/gui/libgui.$(LIB_EXTENSION):

$(BASEDIR)/gui/libTree.$(LIB_EXTENSION):

uninstall: 
	$(RM) $(addprefix $(PREFIX)/lib/, $(LIBS))

IGNORE: install uninstall
PHONY: all $(LIBS) libcore.$(LIB_EXTENSION) libnet.$(LIB_EXTENSION) libgui.$(LIB_EXTENSION) libTree.$(LIB_EXTENSION)
