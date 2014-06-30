include ../config.mk

CLEAN_FILES := 
DISTCLEAN_FILES := qtchat qtchat.sh

all: qtchat qtchat.sh

qtchat: $(BASEDIR)/qtchat
	cp $(BASEDIR)/qtchat .

qtchat.sh:
	echo "#!/bin/sh" > qtchat.sh
	echo >> qtchat.sh
	echo "if [ \"X$${LD_LIBRARY_PATH}\" = \"X\" ]; then" >> qtchat.sh
	echo "export LD_LIBRARY_PATH=$(PREFIX)/lib;" >> qtchat.sh
	echo "else" >> qtchat.sh
	echo "export LD_LIBRARY_PATH=$(PREFIX)/lib:$${LD_LIBRARY_PATH};" >> qtchat.sh
	echo "fi" >> qtchat.sh
	chmod 755 qtchat.sh

$(BASEDIR)/qtchat:
	(cd $(BASEDIR) && $(MAKE) -f $(BASEDIR)/GNUmakefile qtchat)

install: qtchat qtchat.sh
	$(INSTALL_DIR_CMD) $(PREFIX)/bin
	$(INSTALL_FILES_CMD) qtchat $(PREFIX)/bin
	$(INSTALL_FILES_CMD) qtchat.sh $(PREFIX)/bin

