# Suck in common stuff
include config.mk

# Distribution variables
export DISTNAME := qtchat
export DISTVER  := 0.9.7
export AP_VER_STR := \"$(DISTVER)\"
DISTFILES =  COPYRIGHT README INSTALL CHANGES GNUmakefile config.mk
DISTFILES += qtchat.cc qtchat.h ftpsend includecheck $(SUBDIRS)
DISTFILE = $(DISTNAME)-$(DISTVER).tar.gz
CLEAN_FILES := *.la *.o *.o *.moc.cc *.core prototypes.h bugreport.txt
export DISTCLEAN_FILES := $(CLEAN_FILES) qtchat $(DISTNAME)-$(DISTVER).tar.gz
export LIBS := libcore.$(LIB_EXTENSION) libnet.$(LIB_EXTENSION) libgui.$(LIB_EXTENSION) 
INSTALLED_TARGETS := $(LIBS) $(DISTNAME)

# General variables
export TESTPROGS := test/strduptest.c
QTCHAT_MAINTAINER = qtchat@mindspring.com

# Path configuration
export VPATH := bin:core:gui:lib:net:libwc
SUBDIRS := bin core gui lib libwc net test 3rdparty Tree

# Targets
all: configcheck qtcheck includecheck lib qtchat
	@echo
	@echo "*************************************************************"
	@printf "****         QtChat v%7s has been built.             ****\n" $(DISTVER)
	@printf "****         Type '%5s install' to install to          ****\n" ${MAKE} 
	@printf "****    %-44s     ****\n" $(PREFIX)
	@echo "****         or change the PREFIX variable in            ****"
	@printf "****   %37s/config.mk   ****\n" $(BASEDIR)
	@echo "*************************************************************"
	@echo

configcheck:
ifeq "$(DEBUG_MODE)" "1"
	@echo; \
	echo	"************** WARNING **************"; \
	echo	"DEBUG_MODE set to \"1\" in config.mk."; \
	echo	"Linux platforms have had problems"; \
	echo	"compiling with this flag set."; \
	echo "************** WARNING **************"; \
	echo
endif

qtcheck:
	@echo "Searching for Qt-2.0.x libs/include files..."
	@if [ "X$(QT_INCLUDE_DIR)" = "X" -o "X$(QT_LIB_DIR)" = "X" ]; then \
		echo "Unable to autodetect Qt library version 2.0.0 or higher."; \
		echo "          ----------"; \
		echo "Directories searched: " $(QT_INCLUDE_DIR_CHOICES); \
		echo "   " $(QT_LIB_DIR_CHOICES); \
		echo "          ----------"; \
		echo "Please edit the file 'config.mk' to set the path for"; \
		echo "Qt-2.0.x, or download/install Qt-2.0.x from http://www.troll.no/ ."; \
		echo "If library is properly installed, please notify abjenkins@mindspring.com"; \
		echo "with information about your particular configuration."; \
		exit 1; \
	else \
		echo "Include directory: " $$QT_INCLUDE_DIR; \
		echo "Library directory: " $$QT_LIB_DIR; \
		sleep 1; \
	fi

# Check for presence of certain prototypes in standard include files
# and patch config.mk if necessary
includecheck: $(TESTPROGS)
	@echo "Testing standard include files..."
	@if [ -x $(BASEDIR)/includecheck ]; then \
		export CXX=$(CXX); \
		$(BASEDIR)/includecheck; \
		sleep 1; \
	else \
		echo "  Unable to run $(BASEDIR)/includecheck-- distribution possibly corrupt."; \
		exit 1; \
	fi
		
ftp: dist
	@echo -n "name [$(DISTFILE)]: "; read distname; \
	if [ "X$${distname}" = "X" ]; then \
		export distname="$(DISTFILE)"; \
	fi; \
	echo -n "ftp [ftp.mindspring.com]: "; read address; \
	if [ "X$${address}" = "X" ]; then \
		export address="ftp.mindspring.com"; \
	fi; \
	echo -n "Destination path [www]: "; read ftppath; \
	if [ "X$${ftppath}" = "X" ]; then \
		export ftppath=www; \
	fi; \
	echo -n "Username [qtchat]: "; read ftpuser; \
	if [ "X$${ftpuser}" = "X" ]; then \
		export ftpuser=qtchat; \
	fi; \
	echo -n "Password: "; read ftppass; \
	echo Sending tarball to ftp://$${address}/\~$${ftpuser}/$${ftppath}/$(DISTFILE)...; \
	./ftpsend $${address} "$${ftppath}" "$${ftpuser}" "$${ftppass}" $(DISTFILE) $${distname};


mailto: dist
	@echo -n "mailto: "; read address; \
	if [ "X$${address}" = "X" ]; then \
		echo Aborted.; \
	else \
		echo -n Sending uuencoded distribution to $${address}...; \
		uuencode $(DISTFILE) $(DISTFILE) | mail -v -s "$(DISTFILE) `date`" $${address}; \
		echo Done.; \
	fi

bugreport: distclean
	@if which -s dialog; then \
		if dialog --yesno "`uname -a`   Include in report?" 15 65; then \
			echo "uname -a:" 2>&1 1> $(BASEDIR)/bugreport.txt; \
			uname -a 2>&1 1>> $(BASEDIR)/bugreport.txt; \
			echo 2>&1 1>> $(BASEDIR)/bugreport.txt; \
		fi; \
	else \
		echo "uname -msr:" 2>&1 1>> $(BASEDIR)/bugreport.txt; \
		uname -msr 2>&1 1>> $(BASEDIR)/bugreport.txt; \
		echo 2>&1 1>> $(BASEDIR)/bugreport.txt; \
	fi;

	@echo "$(CC) -v:" 2>&1 2>>  $(BASEDIR)/bugreport.txt;
	@$(CC) -v 2>&1 2>> $(BASEDIR)/bugreport.txt;
	@echo 2>&1 1>> $(BASEDIR)/bugreport.txt;

	@echo "$(MAKE) -v nothing:" 2>&1 1>> $(BASEDIR)/bugreport.txt;
	@$(MAKE) -v nothing 2>&1 1>> $(BASEDIR)/bugreport.txt;
	@echo 2>&1 1>> $(BASEDIR)/bugreport.txt

	@if which -s dialog; then \
		dialog --msgbox "Building qtchat to\nobtain messages..." 7 30; \
	else \
		echo "Building qtchat to obtain messages..."; \
		sleep 2; \
	fi
	@echo $(MAKE) all: 2>&1 1>> $(BASEDIR)/bugreport.txt
	@$(MAKE) all 2>&1 1| tee -a $(BASEDIR)/bugreport.txt
	@echo 2>&1 1>> $(BASEDIR)/bugreport.txt

	@if which -s dialog; then \
		dialog --msgbox "Installing qtchat to\nobtain messages..." 7 30; \
	else \
		echo "Installing qtchat to obtain messages..."; \
		sleep 2; \
	fi
	@echo $(MAKE) install: 2>&1 1>> $(BASEDIR)/bugreport.txt
	@$(MAKE) install 2>&1 1| tee -a $(BASEDIR)/bugreport.txt
	@echo 2>&1 1>> $(BASEDIR)/bugreport.txt

	@if which -s dialog; then \
		if dialog --yesno "Send?" 5 20; then \
			echo Sending...; \
			cat $(BASEDIR)/bugreport.txt | mail -v -s "QtChat $(DISTVER) Bug Report" \
				$(QTCHAT_MAINTAINER); \
		fi; \
	else \
		cat $(BASEDIR)/bugreport.txt; \
		echo; \
		echo -n "Send\? "; read send; \
		if [ "X$${send}" \!= "X" ]; then \
			echo Sending...; \
			cat $(BASEDIR)/bugreport.txt | mail -v -s "QtChat $(DISTVER) Bug Report" \
				$(QTCHAT_MAINTAINER); \
		fi; \
	fi
	
qtchat: qtchat.o lib 
#	$(LIBTOOL) $(CXX) $(SHARED_LINK_FLAGS) $(LIB_FLAGS) qtchat.o $(CXXFLAGS) -o qtchat 
	$(CXX) -ITree Tree/Tree.cc qtchat.o $(LIB_FLAGS) $(CXXFLAGS) -o qtchat 

testprog: testprog.o lib
#	$(LIBTOOL) $(CXX) $(SHARED_LINK_FLAGS) $(LIB_FLAGS) testprog.o $(CXXFLAGS) -o testprog
	$(CXX) $(SHARED_LINK_FLAGS) $(LIB_FLAGS) testprog.o $(CXXFLAGS) -o testprog

dist: distclean $(DISTFILE)

$(DISTNAME)-$(DISTVER).tar.gz:
#	for i in $(SUBDIRS); do \
#		(cd $$i && $(MAKE) distclean) \
#	done
	tar cvf $(DISTNAME)-$(DISTVER).tar -C ../../.. $(foreach filename, $(DISTFILES), $(DISTNAME)/src/$(DISTVER)/$(filename))
	gzip $(DISTNAME)-$(DISTVER).tar

$(BASEDIR)/lib/lib%.$(LIB_EXTENSION):
	cd $(dir $@) && $(MAKE) $(notdir $@)

install: all
	(cd $(BASEDIR)/bin && $(MAKE) install)
	(cd $(BASEDIR)/lib && $(MAKE) install)
	@echo
	@echo "*******************************************************"
	@echo "*** QtChat  has  been   successfully  installed  to ***"
	@printf "*** %-47s ***\n" $(PREFIX)
	@echo "*** This  is currently  alpha  quality  software at ***"
	@echo "*** best;  Use of this software  constitutes agree- ***"
	@echo "*** ment to the  terms and conditions  set forth in ***"
	@printf "*** %-47s ***\n" $(BASEDIR)/COPYRIGHT
	@echo "*** and designates  you as an  alpha tester,  which ***"
	@echo "*** requires  bug  reports  to  be  returned to the ***"
	@echo "*** author at  abjenkins@mindspring.com.   You  can ***"
	@printf "*** type  '%-5s bugreport'  to  automatically send ***\n" $(MAKE)
	@echo "*** one to me.   HAPPY TESTING!!!                   ***"
	@echo "*******************************************************"
	@echo

uninstall:
	for i in $(SUBDIRS); do \
		(cd $$i && $(MAKE) uninstall) \
	done
	$(RM) $(PREFIX)/bin/qtchat

depend:
	mkdep $(CXXFLAGS) qtchat.cc

$(SUBDIRS):
	cd $@  && $(MAKE)

.PHONY: $(SUBDIRS) qtcheck includecheck
.IGNORE: install uninstall
