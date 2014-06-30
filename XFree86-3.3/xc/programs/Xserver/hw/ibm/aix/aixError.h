/*
 * $XConsortium: aixError.h,v 1.2 91/07/16 12:59:18 jap Exp $
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND 
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

#ifndef AIXERROR_H
#define AIXERROR_H

extern	void	  errMsg();
extern	int	(*errExitFunc)();
extern	int	  errFlags;
extern	int	  errThisLevel;

#define	 ERR_NONE	0x00
#define  ERR_ACTION	0x01
#define  ERR_INFO	0x02
#define  ERR_WARNING	0x04
#define  ERR_ERROR	0x08
#define  ERR_FATAL	0x10
#define  ERR_WSGO	0x20

#define  errSetFunc(f)		(errExitFunc=(f))
#define	 errHeed(t)		(errFlags|=(t))
#define	 errIgnore(t)		(errFlags&=(~(t)))
#define  errAction(m)		(errThisLevel= ERR_ACTION,errMsg m)
#define  errInfo(m)		(errThisLevel= ERR_INFO,errMsg m)
#define  errWarning(m)		(errThisLevel= ERR_WARNING, errMsg m)
#define  errError(m)		(errThisLevel= ERR_ERROR, errMsg m)
#define  errFatal(m)		(errThisLevel= ERR_FATAL, errMsg m)
#define  errWSGO(m)		(errThisLevel= ERR_WSGO, errMsg m)

#endif /* AIXERROR_H */
