include ../config.mk

DISTNAME = libgui
DISTFILES = chatmain.cc chatmain.h chardialog.cc chardialog.h \
            chatterslist.cc chatterslist.h debugwidget.cc debugwidget.h \
            fontdialog.cc fontdialog.h lineedit.cc lineedit.h \
				lineentry.cc lineentry.h login.cc login.h \
				mainwidget.cc mainwidget.h menubar.cc menubar.h \
				prefsdialog.cc prefsdialog.h roomlistdialog.cc roomlistdialog.h \
				simpleindicator.cc simpleindicator.cc statusbar.cc statusbar.h \
				GNUmakefile
CLEAN_FILES := *.o *.lo *.la *.o *.core *.moc.cc
DISTCLEAN_FILES := $(DISTNAME).* $(CLEAN_FILES)

SUBDIRS :=

3RDPARTYOBJS =

#OBJS = chatmain.o chatterslist.o debugwidget.o fontdialog.o
#OBJS += lineedit.o lineentry.o login.o mainwidget.o menubar.o
#OBJS += prefsdialog.o roomlistdialog.o simpleindicator.o statusbar.o
OBJS := chardialog.o chatmain.o chatterslist.o debugwidget.o fontdialog.o
OBJS += lineedit.o lineentry.o login.o mainwidget.o menubar.o
OBJS += prefsdialog.o roomlistdialog.o simpleindicator.o statusbar.o

#OBJS += chatmain.moc.o chatterslist.moc.o debugwidget.moc.o
#OBJS += fontdialog.moc.o lineentry.moc.o login.moc.o mainwidget.moc.o
#OBJS += menubar.moc.o prefsdialog.moc.o simpleindicator.moc.o
#OBJS += $(3RDPARTYOBJS)
OBJS += chardialog.moc.o chatmain.moc.o chatterslist.moc.o debugwidget.moc.o
OBJS += fontdialog.moc.o lineentry.moc.o login.moc.o mainwidget.moc.o
OBJS += menubar.moc.o prefsdialog.moc.o roomlistdialog.moc.o statusbar.moc.o
OBJS += simpleindicator.moc.o
OBJS += $(3RDPARTYOBJS)

all: $(DISTNAME).$(LIB_EXTENSION)

$(DISTNAME).$(LIB_EXTENSION): $(OBJS)
ifeq "$(LINK_MODE)" "shared"
	$(LINK_CMD) $(SHARED_LINK_FLAGS) -o $@ $^
else
	$(LINK_CMD) $(SHARED_LINK_FLAGS) $@ $^
	ranlib $@
endif

dist: ${DISTNAME}-${DISTVER}.tar.gz

install: 
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

$(3RDPARTYOBJS):
	cd $(BASEDIR)/3rdparty && $(MAKE)
	cp $(BASEDIR)/3rdparty/*/*.o .

.PRECIOUS: chatmain.moc.cc chatterslist.moc.cc debugwidget.moc.cc \
           fontdialog.moc.cc lineentry.moc.cc login.moc.cc mainwidget.moc.cc \
			  menubar.moc.cc prefsdialog.moc.cc
