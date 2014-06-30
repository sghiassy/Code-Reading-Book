/* $XConsortium: compiler.h,v 1.3 94/01/18 19:34:43 rws Exp $ */
/*
 * Copyright 1990,91,92,93 by Thomas Roell, Germany.
 * Copyright 1991,92,93    by SGCS (Snitily Graphics Consulting Services), USA.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this  permission notice appear
 * in supporting documentation, and that the name of Thomas Roell nor
 * SGCS be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * Thomas Roell nor SGCS makes no representations about the suitability
 * of this software for any purpose. It is provided "as is" without
 * express or implied warranty.
 *
 * THOMAS ROELL AND SGCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THOMAS ROELL OR SGCS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _COMPILER_H
#define _COMPILER_H

#ifdef __GNUC__

static __inline__ void
outb(
    const ulong port,
    ulong       val
)
{
  __asm__ volatile("out%B0 (%%dx)" : :"a" (val), "d" (port));
}

static __inline__ void
outw(
    const ulong port,
    ulong       val
)
{
  __asm__ volatile("out%W0 (%%dx)" : :"a" (val), "d" (port));
}

static __inline__ void
outl(
    const ulong port,
    ulong       val
)
{
  __asm__ volatile("out%L0 (%%dx)" : :"a" (val), "d" (port));
}

static __inline__ unchar
inb(
    const ulong port
)
{
  unchar ret;

  __asm__ volatile("in%B0 (%%dx)"
		   : "=a" (ret)
		   : "d" (port));
  return ret;
}

static __inline__ ushort
inw(
    const ulong port
)
{
  ushort ret;

  __asm__ volatile("in%W0 (%%dx)"
		   : "=a" (ret)
		   : "d" (port));
  return ret;
}

static __inline__ ulong
inl(
    const ulong port
)
{
  ulong ret;

  __asm__ volatile("in%L0 (%%dx)"
		   : "=a" (ret)
		   : "d" (port));
  return ret;
}

#else /* ! __GNUC__ */
#ifndef XSVGA_NO_INLINE
#if !defined(NCR)
#define asm __asm
#endif
#include <sys/inline.h>
#else
extern void outb();
extern void outw();
extern void outl();
extern unchar inb();
extern ushort inw();
extern ulong inl();
#endif
#endif

#endif /* _COMPILER_H */
