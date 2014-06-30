#	$NetBSD: Makefile,v 1.3 1997/10/18 04:38:06 lukem Exp $

PROG=	sa
MAN=	sa.8
SRCS=	main.c pdb.c usrdb.c

.if (${MACHINE_ARCH} == "arm32")
CFLAGS+= -O0		# XXX arm32 gcc loses when optimization turned on
.endif

.include <bsd.prog.mk>
