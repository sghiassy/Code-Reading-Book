! $XConsortium: sparcsolstack.s,v 1.1 93/12/03 09:26:24 kaleb Exp $
	.seg	"text"
	.proc	16
	.globl	getStackPointer
getStackPointer:
	retl
	mov	%sp,%o0
	.globl	getFramePointer
getFramePointer:
	retl
	mov	%fp,%o0
