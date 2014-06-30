#----------------------------------------------------------------------------
#       Makefile,v 1.3 2001/01/03 16:02:56 joeh Exp
#
#       Makefile for the PACE html man pages
#       The man pages are generated from the POSIX header files.
#----------------------------------------------------------------------------

PACE_DOC++ = /project/doc/pkg/bin/doc++
index.html:
	$(PACE_DOC++) -H -F -B banner ../*.h

clean:
	$(RM) *.html *.gif *.class *.tex *.db .html
