#	$NetBSD: Makefile,v 1.13 1997/10/17 09:44:39 mrg Exp $
#	from: @(#)Makefile	8.2 (Berkeley) 5/10/94

# The following modules do not build/install:
# 10.named, 13.amd

# Missing:
# 02.config 11.timedop 12.timed

# Missing from 4.4BSD-Lite:
# 14.uucpimpl 15.uucpnet 16.security 17.password

BINDIR=	/usr/share/doc/smm
FILES=	00.contents Makefile Title
SUBDIR=	01.setup 04.quotas 05.fastfs 06.nfs 18.net
.if exists(03.fsck_ffs)
SUBDIR+= 03.fsck_ffs
.endif
.if exists(07.lpd)
SUBDIR+= 07.lpd
.endif
.if exists(08.sendmailop)
SUBDIR+= 08.sendmailop
.endif
.if exists(09.sendmail)
SUBDIR+= 09.sendmail
.endif
#SUBDIR=	01.setup 02.config 03.fsck 04.quotas 05.fastfs 06.nfs 07.lpd \
#	08.sendmailop 09.sendmail 11.timedop 12.timed 14.uucpimpl \
#	15.uucpnet 16.security 17.password 18.net 19.perl

Title.ps: ${FILES}
	groff -ms Title > ${.TARGET}

contents.ps: ${FILES}
	groff -ms 00.contents > ${.TARGET}

.include <bsd.prog.mk>
.include <bsd.subdir.mk>
