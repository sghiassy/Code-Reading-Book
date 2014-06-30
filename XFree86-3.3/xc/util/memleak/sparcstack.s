# $XConsortium: sparcstack.s,v 1.2 92/04/08 17:19:18 keith Exp $
	.seg	"text"
	.proc	16
	.globl	_getStackPointer
_getStackPointer:
	retl
	mov	%sp,%o0
	.globl	_getFramePointer
_getFramePointer:
	retl
	mov	%fp,%o0
