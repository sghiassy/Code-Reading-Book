/*
 * $XConsortium: compiler.h,v 1.2 91/07/16 13:04:49 jap Exp $
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

#ifndef COMPILER_H
#define COMPILER_H

#ifdef _ANSI_DECLS_
#undef _ANSI_DECLS_
#endif

#if defined(UNIXCPP) && !defined(ANSICPP)
#define APPEND_STRING(a,b) a/**/b
#else
#if defined(ANSICPP)
#define APPEND_STRING(a,b) a##b
#else
#define APPEND_STRING(a,b) /**/a\
b/**/
#endif
#endif

#ifdef SYSV
#define MOVE( src, dst, length ) memcpy( dst, src, length)
#else
#define MOVE( src, dst, length ) bcopy( src, dst, length )
#endif

#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define ABS(x) (((x)>0)?(x):-(x))

#if !defined(_ANSI_DECLS_) || defined(lint)
/* So that lint doesn't complain about constructs it doesn't understand */
#ifdef volatile
#undef volatile
#endif
#define volatile /**/
#ifdef const
#undef const
#endif
#define const	/**/
#ifdef signed
#undef signed
#endif
#define signed	/**/
#ifdef _ANSI_DECLS_
#undef _ANSI_DECLS_
#endif
#endif

#ifdef AIXV3
#if defined(volatile)
#undef volatile
#endif
#endif

#endif /* COMPILER_H */
