#	$NetBSD: Makefile,v 1.2 1997/08/02 21:30:21 perry Exp $

CRUNCHED=   fixit

# below is boiler-plate to make $(CRUNCHED) from $(CRUNCHED).conf
# I'd use PROG instead of CRUNCHED, but the system makefiles REALLY want
# to build things in the normal way if you use PROG.

CONF=	$(CRUNCHED).conf

OUTMK=	$(CRUNCHED).mk
OUTPUTS=  $(OUTMK) $(CRUNCHED).c $(CRUNCHED).cache

NOMAN=
CLEANFILES+=$(CRUNCHED) $(OUTPUTS) *.o *.lo *_stub.c

all: $(CRUNCHED)
exe: $(CRUNCHED)

$(OUTPUTS): $(CONF)
	crunchgen ${.CURDIR}/$(CONF)

$(CRUNCHED): $(OUTPUTS) submake

submake:
	make -f $(OUTMK)
objs:
	make -f $(OUTMK) objs

.include <bsd.prog.mk>
