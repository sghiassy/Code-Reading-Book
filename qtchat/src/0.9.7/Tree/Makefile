DISTNAME = Tree
DISTVER  = 0.6
DISTFILE = ${DISTNAME}-${DISTVER}.tar.gz
DISTFILES = Menu.cc Menu.h Tree.cc Tree.h test.cc Makefile
INCLUDE_FLAGS =


CXXFLAGS := -Wall

all: test

libtree.a: Tree.o Menu.o
	ar rcv libtree.a $^
	ranlib libtree.a

Tree.o: Tree.cc Tree.h

Menu.o: Menu.cc Menu.h

test: test.o libtree.a
	g++ -static ${CXXFLAGS} -o $@ test.o -L. -ltree

ftpsend:
	echo '#!/bin/sh'	> ftpsend
	echo	>> ftpsend
	echo 'ftp -v -n $$1 <<-EOF' >> ftpsend
	echo '	user $$3 $$4' >> ftpsend
	echo '	cd $$2' >> ftpsend
	echo '	put $$5' >> ftpsend
	echo '	bye' >> ftpsend
	echo 'EOF' >> ftpsend
	chmod 755 ftpsend

clean:
	rm -f *.o *.core

distclean: clean
	rm -f ${DISTNAME}-${DISTVER}.tar.gz *.a test

dist: ${DISTFILE}

${DISTNAME}-${DISTVER}.tar.gz: ${DISTFILES}
	tar cvf ${DISTNAME}-${DISTVER}.tar -C .. $(foreach filename, $(DISTFILES), $(DISTNAME)/$(filename))
	gzip ${DISTNAME}-${DISTVER}.tar

ftp: dist ftpsend
	@read -p "ftp [ftp.mindspring.com]: " address; \
	if [ "X$${address}" = "X" ]; then \
		export address="ftp.mindspring.com"; \
	fi; \
	read -p "Destination path [www]: " ftppath; \
	if [ "X$${ftppath}" = "X" ]; then \
		export ftppath=www; \
	fi; \
	read -p "Username [qtchat]: " ftpuser; \
	if [ "X$${ftpuser}" = "X" ]; then \
		export ftpuser=qtchat; \
	fi; \
	read -p "Password: " ftppass; \
	echo Sending tarball to ftp://$${address}/\~$${ftpuser}/$${ftppath}/$(DISTFILE)...; \
	./ftpsend $${address} "$${ftppath}" "$${ftpuser}" "$${ftppass}" $(DISTFILE)


mailto: dist
	@read -p mailto: address; \
	if [ "X$${address}" = "X" ]; then \
		echo Aborted.; \
	else \
		echo -n Sending uuencoded distribution to $${address}...; \
		uuencode $(DISTFILE) $(DISTFILE) | mail -v -s "$(DISTFILE) `date`" $${address}; \
		echo Done.; \
	fi

%.o: %.cc
	${CXX} -c ${CXXFLAGS} ${INCLUDE_FLAGS} $< 

%.h:
