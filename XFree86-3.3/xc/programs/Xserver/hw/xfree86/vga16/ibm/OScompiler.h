/* $XFree86: xc/programs/Xserver/hw/xfree86/vga16/ibm/OScompiler.h,v 3.5 1996/12/23 06:52:34 dawes Exp $ */
/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/* $XConsortium: OScompiler.h /main/4 1996/02/21 17:56:09 kaleb $ */

#ifndef __COMPILER_DEPENDANCIES__
#define __COMPILER_DEPENDANCIES__

#ifdef __GNUC__
#define APPEND_STRING(a,b) a##b
#else
#define APPEND_STRING(a,b) /**/a\
b/**/
#endif

#define MOVE( src, dst, length ) memcpy( dst, src, length)

#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define ABS(x) (((x)>0)?(x):-(x))

#include "misc.h"
#include "compiler.h"
#include "../vga/vgaBank.h"

#ifdef UNBANKED_VGA16

#define VSETR(x)
#define VSETW(x)
#define VSETRW(x)
#define VCHECKRO(x)
#define VCHECKRU(x)
#define VCHECKWO(x)
#define VCHECKWU(x)
#define VCHECKRWO(x)
#define VCHECKRWU(x)
#define VCHECKRWONEXT(x)
#define VPUSHR()
#define VPOPR()

#define ADDR(x,y) {(x) += (y);}
#define ADDW(x,y) {(x) += (y);}
#define ADDRW(x,y) {(x) += (y);}
#define VINCR(x) {(x)++;}
#define VINCW(x) {(x)++;}
#define VINCRW(x) {(x)++;}
#define VDECR(x) {(x)--;}
#define VDECW(x) {(x)--;}
#define VDECRW(x) {(x)--;}

#else /* UNBANKED_VGA16 */

/* GJA -- This are equivalents of the SETR... macros, but omitting
 * the vgaReadFlag/vgaWriteFlag. We know for sure that they are always
 * applied to windows. If they by accident aren't, the results are useless
 * anyway: A planar VGA does not look like a packed pixmap.
 */
/* GJA -- Actually, they are added because I have not yet tidied up
 * my include file structure (mfb/mfb.banked). Be careful or it will break.
 */
#define VSETR(x)  { x = vgaSetRead((void *)x); }
#define VSETW(x)  { x = vgaSetWrite((void *)x); }
#define VSETRW(x) { x = vgaSetReadWrite((void *)x); }
#define VCHECKRO(x) { if(((void *)x >= vgaReadTop)) \
			 x = vgaReadNext((void *)x); }
#define VCHECKRU(x) { if(((void *)x < vgaReadBottom)) \
			 x = vgaReadPrev((void *)x); }
#define VCHECKWO(x) { if(((void *)x >= vgaWriteTop)) \
			 x = vgaWriteNext((void *)x); }
#define VCHECKWU(x) { if(((void *)x < vgaWriteBottom)) \
			 x = vgaWritePrev((void *)x); }
#define VCHECKRWO(x) { if(((void *)x >= vgaWriteTop)) \
			  x = vgaReadWriteNext((void *)x); }
#define VCHECKRWU(x) { if(((void *)x < vgaWriteBottom)) \
			  x = vgaReadWritePrev((void *)x); }
#define VCHECKRWONEXT(x) { x++; VCHECKRWO(x); x--; }
#define VPUSHR()         { vgaPushRead(); }
#define VPOPR()          { vgaPopRead(); }

#define ADDR(x,y) {(x) += (y); if ((y) > 0) {VCHECKRO(x) }else{ VCHECKRU(x); }}
#define ADDW(x,y) {(x) += (y); if ((y) > 0) {VCHECKWO(x) }else{ VCHECKWU(x); }}
#define ADDRW(x,y) {(x) += (y); if ((y) > 0) {VCHECKRWO(x) }else{ VCHECKRWU(x); }}
#define VINCR(x) {(x)++; VCHECKRO(x); }
#define VINCW(x) {(x)++; VCHECKWO(x); }
#define VINCRW(x) {(x)++; VCHECKRWO(x); }
#define VDECR(x) {(x)--; VCHECKRU(x); }
#define VDECW(x) {(x)--; VCHECKWU(x); }
#define VDECRW(x) {(x)--; VCHECKRWU(x); }
#endif /* ! UNBANKED_VGA16 */

#define VMAPR(x) x
#define VMAPW(x) x
#define VMAPRW(x) x
  
#ifdef lint
/* So that lint doesn't complain about constructs it doesn't understand */
#ifdef volatile
#undef volatile
#endif
#define volatile
#ifdef const
#undef const
#endif
#define const
#ifdef signed
#undef signed
#endif
#define signed
#ifdef _ANSI_DECLS_
#undef _ANSI_DECLS_
#endif
#endif

#endif /* !__COMPILER_DEPENDANCIES__ */
