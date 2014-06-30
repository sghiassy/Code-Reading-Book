/* $XConsortium: lcInit.c /main/4 1996/12/05 10:40:35 swick $ */
/* $XFree86: xc/lib/X11/lcInit.c,v 3.2 1996/12/24 02:22:48 dawes Exp $ */
/*
 * Copyright 1992, 1993 by TOSHIBA Corp.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of TOSHIBA not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. TOSHIBA make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * TOSHIBA DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * TOSHIBA BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * Author: Katsuhisa Yano	TOSHIBA Corp.
 *			   	mopi@osa.ilab.toshiba.co.jp
 */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDE/Motif PST.
 *
 *   Modifier: Masayoshi Shimamura      FUJITSU LIMITED 
 *
 */

#include "Xlibint.h"
#include "Xlcint.h"

#define USE_GENERIC_LOADER
#define USE_DEFAULT_LOADER
/*** #define USE_EUC_LOADER ***/
/*** #define USE_SJIS_LOADER ***/
/*** #define USE_JIS_LOADER ***/
/*** #define USE_UTF_LOADER ***/

extern XLCd _XlcDefaultLoader(
#if NeedFunctionPrototypes
    char*
#endif
);

#ifdef DYNAMIC_LOAD
#ifdef sun
extern XLCd _XsunOsDynamicLoad(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif /* sun */

#ifdef AIXV3
extern XLCd _XaixOsDynamicLoad(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif /* AIXV3 */
#endif

#ifdef USE_GENERIC_LOADER
extern XLCd _XlcGenericLoader(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif

#ifdef USE_UTF_LOADER
extern XLCd _XlcUtfLoader(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif

#ifdef USE_EUC_LOADER
extern XLCd _XlcEucLoader(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif

#ifdef USE_SJIS_LOADER
extern XLCd _XlcSjisLoader(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif

#ifdef USE_JIS_LOADER
extern XLCd _XlcJisLoader(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif

#ifdef USE_DYNAMIC_LOADER
extern XLCd _XlcDynamicLoader(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif

/*
 * The _XlcInitLoader function initializes the locale object loader list
 * with vendor specific manner.
 */

void
_XlcInitLoader()
{
#ifdef USE_GENERIC_LOADER
    _XlcAddLoader(_XlcGenericLoader, XlcHead);
#endif

#ifdef USE_DEFAULT_LOADER
    _XlcAddLoader(_XlcDefaultLoader, XlcHead);
#endif

#ifdef USE_EUC_LOADER
    _XlcAddLoader(_XlcEucLoader, XlcHead);
#endif

#ifdef USE_SJIS_LOADER
    _XlcAddLoader(_XlcSjisLoader, XlcHead);
#endif

#ifdef USE_JIS_LOADER
    _XlcAddLoader(_XlcJisLoader, XlcHead);
#endif

#ifdef USE_UTF_LOADER
    _XlcAddLoader(_XlcUtfLoader, XlcHead);
#endif

#ifdef DYNAMIC_LOAD
#ifdef sun
    _XlcAddLoader(_XsunOsDynamicLoad, XlcHead);
#endif /* sun */

#ifdef AIXV3
    _XlcAddLoader(_XaixOsDynamicLoad, XlcHead);
#endif /* AIXV3 */
#endif /* DYNAMIC_LOAD */

#ifdef USE_DYNAMIC_LOADER
    _XlcAddLoader(_XlcDynamicLoader, XlcHead);
#endif
}
