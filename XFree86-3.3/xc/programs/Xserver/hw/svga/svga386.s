/ $XConsortium: svga386.s,v 1.1 93/09/19 09:43:13 rws Exp $
/ Copyright 1990,91,92,93 by Thomas Roell, Germany.
/ Copyright 1991,92,93    by SGCS (Snitily Graphics Consulting Services), USA.
/
/ Permission to use, copy, modify, distribute, and sell this software
/ and its documentation for any purpose is hereby granted without fee,
/ provided that the above copyright notice appear in all copies and
/ that both that copyright notice and this  permission notice appear
/ in supporting documentation, and that the name of Thomas Roell nor
/ SGCS be used in advertising or publicity pertaining to distribution
/ of the software without specific, written prior permission.
/ Thomas Roell nor SGCS makes no representations about the suitability
/ of this software for any purpose. It is provided "as is" without
/ express or implied warranty.
/
/ THOMAS ROELL AND SGCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
/ SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
/ FITNESS, IN NO EVENT SHALL THOMAS ROELL OR SGCS BE LIABLE FOR ANY
/ SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
/ RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
/ CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
/ CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
/

	.globl	outb
	.globl	outw
	.globl	outl
	.globl	inb
	.globl	inw
	.globl	inl

	.text

	.align	16
outb:	movl	4(%esp), %edx
	movl	8(%esp), %eax
	outb	(%dx)
	ret

	.align	16
outw:	movl	4(%esp), %edx
	movl	8(%esp), %eax
	outw	(%dx)
	ret

	.align	16
outl:	movl	4(%esp), %edx
	movl	8(%esp), %eax
	outl	(%dx)
	ret

	.align	16
inb:	movl	4(%esp), %edx
	inb	(%dx)
	ret

	.align	16
inw:	movl	4(%esp), %edx
	inw	(%dx)
	ret

	.align	16
inl:	movl	4(%esp), %edx
	inl	(%dx)
	ret
