/* $XConsortium: XlcUTF.h,v 1.5 94/03/31 21:59:49 rws Exp $ */
/******************************************************************

              Copyright 1993 by SunSoft, Inc.

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of SunSoft, Inc.
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.
SunSoft, Inc. makes no representations about the suitability of
this software for any purpose.  It is provided "as is" without
express or implied warranty.

SunSoft Inc. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL SunSoft, Inc. BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

  Author: Hiromu Inukai (inukai@Japan.Sun.COM) SunSoft, inc.

******************************************************************/
#include "Xlibint.h"
#include "XlcGeneric.h"
#include <X11/Xos.h>
#include <stdio.h>

typedef unsigned short Rune;		/* 16 bits */
#define		Runeerror	0x80	/* decoding error in UTF */
#define		Runeself	0x80	/* rune and UTF sequences are the same (<) */
#define		UTFmax		3	/* maximum bytes per rune */

#define		BADMAP	(0xFFFD)
#define		ESC	033
#define		NRUNE	65536
#define		NLATIN	0x6ff
#define		LATINMAX 256
#define		KUTENMAX 8407
#define		KSCMAX	8743
#define		GBMAX	8795
#define		tab8859_1	"tab8859_1"
#define		tab8859_2	"tab8859_2"
#define		tab8859_3	"tab8859_3"
#define		tab8859_4	"tab8859_4"
#define		tab8859_5	"tab8859_5"
#define		tab8859_6	"tab8859_6"
#define		tab8859_7	"tab8859_7"
#define		tab8859_8	"tab8859_8"
#define		tab8859_9	"tab8859_9"
#define		jis0208		"jis0208"
#define		ksc5601		"ksc5601"
#define		gb2312		"gb2312"

#define emit(x)    *r = (Rune)x;

typedef enum {
	N11n_none,		/* No need to normalize (1byte) */
	N11n_ja,		/* Normalize for ja */
	N11n_ko,		/* Normalize for ko */
	N11n_zh			/* Normalize for zh */
} NormalizeType;

typedef struct  _UtfDataRec {
	XlcCharSet		charset;
	void			(*initialize)( /* Table Initializer */
#if NeedNestedPrototypes
					      long *tbl,
					      long fallback
#endif
						);
	long			*fromtbl;	/* UTF -> CharSet */
	NormalizeType		type;		/* Normalize type */
	void			(*cstorune)(   /* CharSet -> UTF */
#if NeedNestedPrototypes
					    unsigned char c,
					    Rune *r
#endif
					    );
	Bool			already_init;
        struct _UtfDataRec	*next;		/* next entry     */
} UtfDataRec, *UtfData;

typedef struct _XlcUTFDataRec {
    char	*name;
    XlcSide	side;
    void	(*initialize)();
    void	(*cstorune)(
#if NeedNestedPrototypes
			    unsigned char c,
			    Rune *r
#endif
			    );
    NormalizeType	type;
    long		fallback_value;
} XlcUTFDataRec, *XlcUTFData;

typedef struct _StateRec {
    XlcCharSet charset;
    XlcCharSet GL_charset;
    XlcCharSet GR_charset;
} StateRec, *State;

#define MAX_UTF_CHARSET	(sizeof(default_utf_data)/sizeof(XlcUTFDataRec))

#define Char1 Runeself
#define Rune1 Runeself
#define Char21 0xA1
#define Rune21 0x0100
#define Char22 0xF6
#define Rune22 0x4016
#define Char3 0xFC
#define Rune3 0x10000	/* really 0x38E2E */
#define Esc 0xBE
#define Bad Runeerror

#define T1	0x00
#define	Tx	0x80
#define T2	0xC0
#define T3	0xE0
#define T4	0xF0
#define T5	0xF8
#define T6	0xFC

#define Bit1	7
#define Bitx	6
#define Bit2	5
#define Bit3	4
#define Bit4	3
#define Bit5	2
#define Bit6	2

#define Mask1	(1<<Bit1)-1
#define Maskx	(1<<Bitx)-1
#define Mask2	(1<<Bit2)-1
#define Mask3	(1<<Bit3)-1
#define Mask4	(1<<Bit4)-1
#define Mask5	(1<<Bit5)-1
#define Mask6	(1<<Bit6)-1

#define Wchar1	(1<<Bit1)-1
#define Wchar2	(1<<(Bit2+Bitx))-1
#define Wchar3	(1<<(Bit3+2*Bitx))-1
#define Wchar4	(1<<(Bit4+3*Bitx))-1
#define Wchar5	(1<<(Bit5+4*Bitx))-1

#ifndef	EILSEQ
#define EILSEQ  123
#endif

#define J2S(_h, _l) { \
        /* lower: 21-7e >> 40-9d,9e-fb >> 40-7e,(skip 7f),80-fc */ \
        if (((_l) += (((_h)-- % 2) ? 0x1f : 0x7d)) > 0x7e) (_l)++; \
        /* upper: 21-7e >> 81-af >> 81-9f,(skip a0-df),e0-ef */ \
        if (((_h) = ((_h) / 2 + 0x71)) > 0x9f) (_h) += 0x40; \
}
#define S2J(_h, _l) { \
        /* lower: 40-7e,80-fc >> 21-5f,61-dd >> 21-7e,7f-dc */ \
        if (((_l) -= 0x1f) > 0x60) (_l)--; \
        /* upper: 81-9f,e0-ef >> 00-1e,5f-6e >> 00-2e >> 21-7d */ \
        if (((_h) -= 0x81) > 0x5e) (_h) -= 0x40; (_h) *= 2, (_h) += 0x21; \
        /* upper: ,21-7d >> ,22-7e ; lower: ,7f-dc >> ,21-7e */ \
        if ((_l) > 0x7e) (_h)++, (_l) -= 0x5e; \
}
#define ISJKANA(_b)     (0xa0 <= (_b) && (_b) < 0xe0)
#define CANS2JH(_h)     ((0x81 <= (_h) && (_h) < 0xf0) && !ISJKANA(_h))
#define CANS2JL(_l)     (0x40 <= (_l) && (_l) < 0xfd && (_l) != 0x7f)
#define CANS2J(_h, _l)  (CANS2JH(_h) && CANS2JL(_l))

