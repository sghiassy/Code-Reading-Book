
# Makefile,v 1.6 1999/09/22 03:13:28 jcej Exp

all clean realclean UNSHAR SHAR HTML CLEAN : #
	for i in ??? ; do \
		[ -f $$i/Makefile ] || continue ; \
		( cd $$i ; $(MAKE) $@ ) ; \
	done

