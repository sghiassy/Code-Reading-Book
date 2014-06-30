/* $XConsortium: lcUTF.c /main/12 1996/12/04 10:12:29 swick $ */
/* $XFree86: xc/lib/X11/lcUTF.c,v 3.3 1996/12/24 02:22:50 dawes Exp $ */
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
#include "XlcUTF.h"

static long	getutfrune();
static void our_wctomb(
#if NeedFunctionPrototypes
		       unsigned short r, 
		       char **bufptr, 
		       int *buf_len
#endif
);
static int our_mbtowc(
#if NeedFunctionPrototypes   
		      unsigned long *p, 
		      char *s, 
		      size_t n
#endif
);
static void	latin2rune(
#if NeedFunctionPrototypes
			   unsigned char c, 
			   Rune *r
#endif
);
static void	jis02012rune(
#if NeedFunctionPrototypes
			     unsigned char c, 
			     Rune *r
#endif
);
static void	jis02082rune(
#if NeedFunctionPrototypes
			     unsigned char c, 
			     Rune *r
#endif
);
static void	ksc2rune(
#if NeedFunctionPrototypes
			 unsigned char c, 
			 Rune *r
#endif
);
static void	gb2rune(
#if NeedFunctionPrototypes
			unsigned char c, 
			Rune *r
#endif
);
static void	init_latin1tab();
static void	init_latin2tab();
static void	init_latin3tab();
static void	init_latin4tab();
static void	init_latin5tab();
static void	init_latin6tab();
static void	init_latin7tab();
static void	init_latin8tab();
static void	init_latin9tab();
static void	init_jis0201tab();
static void	init_jis0208tab();
static void	init_ksc5601tab();
static void	init_gb2312tab();


static long	*tabkuten = NULL;
static long	*tabksc = NULL;
static long	*tabgb = NULL;

static UtfData utfdata_list = (UtfData)NULL;

static XlcUTFDataRec default_utf_data[] = 
{
    {"ISO8859-1", XlcGL, init_latin1tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGR, init_latin1tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGL, init_latin2tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGR, init_latin2tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGL, init_latin3tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGR, init_latin3tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGL, init_latin4tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGR, init_latin4tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGL, init_latin5tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGR, init_latin5tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGL, init_latin6tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGR, init_latin6tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGL, init_latin7tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGR, init_latin7tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGL, init_latin8tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGR, init_latin8tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGL, init_latin9tab, latin2rune, N11n_none, 0x20},
    {"ISO8859-1", XlcGR, init_latin9tab, latin2rune, N11n_none, 0x20},
    {"JISX0201.1976-0", XlcGL, init_jis0201tab, jis02012rune, N11n_none, 0x20},
    {"JISX0201.1976-0", XlcGR, init_jis0201tab, jis02012rune, N11n_none, 0x20},
    {"JISX0208.1983-0", XlcGL, init_jis0208tab, jis02082rune, N11n_ja, 0x2222},
    {"JISX0208.1983-0", XlcGR, init_jis0208tab, jis02082rune, N11n_ja, 0x2222},
    {"KSC5601.1987-0", XlcGL, init_ksc5601tab, ksc2rune, N11n_ko, 0x2160},
    {"KSC5601.1987-0", XlcGR, init_ksc5601tab, ksc2rune, N11n_ko, 0x2160},
    {"GB2312.1980-0", XlcGL, init_gb2312tab, gb2rune, N11n_zh, 0x2175},
    {"GB2312.1980-0", XlcGR, init_gb2312tab, gb2rune, N11n_zh, 0x2175},
};


static void
set_latin_nop(table, default_val)
long	*table;
long	default_val;
{
    register int i;
    for(i = 0; i < 0x1fff; i++)
	table[i] = default_val;
    return;
}

static void
set_cjk_nop(to_tbl, to_max, default_val)
long	**to_tbl;
long	default_val;
int	to_max;
{
    register int i;
    for(i = 0; i < to_max; i++)
	(*to_tbl)[i] = default_val;
    return;
}

static void
set_latin_tab(fptr, table, fb_default)
FILE	*fptr;
long	*table;
long	fb_default;
{
    int		i = 0;
    int		j = 0;
    int		rv = 0;
    long	value;

    for(i = 0; i < NRUNE; i++)
	table[i] = -1;
    while((rv = fscanf(fptr, "%lx", &value)) != EOF) {
	if(rv != 0 && value >= 0) {
	    table[value] = j++;
	} else {
	    set_latin_nop(table, fb_default);
	    return;
	}
    }
}

static void
set_cjk_tab(fptr, to_tbl, from_tbl, to_max, fb_default)
FILE	*fptr;
long	**to_tbl, *from_tbl;
int	to_max;
long	fb_default;
{
    register int	i = 0;
    int		j = 0;
    int		rv = 0;
    long	value;

    for(i = 0; i < NRUNE; i++)
	from_tbl[i] = -1;
    while((rv = fscanf(fptr, "%lx", &value)) != EOF) {
	if(rv != 0) {
	    (*to_tbl)[j++] = value;
	} else {
	    set_cjk_nop(to_tbl, to_max, fb_default);
	    break;
	}
    }
    for(i = 0; i < to_max; i++) {
	if((value = (*to_tbl)[i]) != -1){
	    from_tbl[abs(value)] = i;
	}
    }
}

extern int _XlcResolveI18NPath();
static char TBL_DATA_DIR[] = "tbl_data";

static void
init_latin_tab(tbl, fb_default, which)
    long*	tbl;
    long	fb_default;
    char*	which;
{
    FILE*	fp = NULL;
    char	dirname[BUFSIZE];
    char	filename[BUFSIZE];
    char*	p;
    char*	q;

    _XlcResolveI18NPath(dirname, BUFSIZE);
    p = dirname;
    while(p) {
	q = strchr(p, ':');
	if(q) {
	    *q = '\0';
	}

	if ((3 + (p ? strlen(p) : 0) + 
	    strlen (TBL_DATA_DIR) + strlen (which)) < BUFSIZE) {
	    sprintf(filename, "%s/%s/%s", p, TBL_DATA_DIR, which);
	    fp = fopen(filename, "r");
	}
	if(fp) {
	    set_latin_tab(fp, tbl, fb_default);
	    fclose(fp);
	    return;
	}
	if(q) {
	    p = q + 1;
	} else {
	    p = q;
	}
    }
    if(!fp) {
	set_latin_nop(tbl, fb_default);
    }
}

static void
init_cjk_tab(tbl, fb_default, which, tab, max)
    long*	tbl;
    long	fb_default;
    char*	which;
    long**	tab;
    long	max;
{
    FILE*	fp = NULL;
    char	dirname[BUFSIZE];
    char	filename[BUFSIZE];
    char*	p;
    char*	q;

    if((*tab = (long *)Xmalloc(max * sizeof(long))) == NULL) {
	return;
    }
    _XlcResolveI18NPath(dirname, BUFSIZE);
    p = dirname;
    while(p) {
	q = strchr(p, ':');
	if(q) {
	    *q = '\0';
	}
	if ((3 + (p ? strlen(p) : 0) + 
	    strlen (TBL_DATA_DIR) + strlen (which)) < BUFSIZE) {
	    sprintf(filename, "%s/%s/%s", p, TBL_DATA_DIR, which);
	    fp = fopen(filename, "r");
	}
	if(fp) {
	    set_cjk_tab(fp, tab, tbl, max, fb_default);
	    fclose(fp);
	    return;
	}
	if(q) {
	    p = q + 1;
	} else {
	    p = q;
	}
    }
    if(!fp) {
	set_cjk_nop(tab, max, fb_default);
    }
}

static void
init_latin1tab(tbl, fb_default)
long	*tbl;
long	fb_default;
{
    init_latin_tab (tbl, fb_default, tab8859_1);
}

static void
init_latin2tab(tbl, fb_default)
long	*tbl;
long	fb_default;
{
    init_latin_tab (tbl, fb_default, tab8859_2);
}

static void
init_latin3tab(tbl, fb_default)
long	*tbl;
long	fb_default;
{
    init_latin_tab (tbl, fb_default, tab8859_3);
}

static void
init_latin4tab(tbl, fb_default)
long	*tbl;
long	fb_default;
{
    init_latin_tab (tbl, fb_default, tab8859_4);
}

static void
init_latin5tab(tbl, fb_default)
long	*tbl;
long	fb_default;
{
    init_latin_tab (tbl, fb_default, tab8859_5);
}

static void
init_latin6tab(tbl, fb_default)
long	*tbl;
long	fb_default;
{
    init_latin_tab (tbl, fb_default, tab8859_6);
}

static void
init_latin7tab(tbl, fb_default)
long	*tbl;
long	fb_default;
{
    init_latin_tab (tbl, fb_default, tab8859_7);
}

static void
init_latin8tab(tbl, fb_default)
long	*tbl;
long	fb_default;
{
    init_latin_tab (tbl, fb_default, tab8859_8);
}

static void
init_latin9tab(tbl, fb_default)
long	*tbl;
long	fb_default;
{
    init_latin_tab (tbl, fb_default, tab8859_9);
}

static void
init_jis0201tab(tbl, fb_default)
long	*tbl;
long	fb_default;
{
	int i;
	for(i = 0; i < NRUNE; i++)
	    tbl[i] = -1;
}

static void
init_jis0208tab(tbl, fb_default)
long	*tbl;
long	fb_default;
{
    init_cjk_tab (tbl, fb_default, jis0208, &tabkuten, KUTENMAX);
}

static void
init_ksc5601tab(tbl, fb_default)
long	*tbl;
long	fb_default;
{
    init_cjk_tab (tbl, fb_default, ksc5601, &tabksc, KSCMAX);
}

static void
init_gb2312tab(tbl, fb_default)
long	*tbl;
long	fb_default;
{
    init_cjk_tab (tbl, fb_default, gb2312, &tabgb, GBMAX);
}

static UtfData
make_entry()
{
    UtfData tmp = (UtfData)Xmalloc(sizeof(UtfDataRec));
    bzero(tmp, sizeof(UtfDataRec));
    return tmp;
}

static int	once = 0;
static int
_XlcInitUTFInfo(lcd)
XLCd	lcd;
{
    if(!once) {
	int	i;
	CodeSet	*codeset_list = XLC_GENERIC(lcd, codeset_list);
	int	codeset_num = XLC_GENERIC(lcd, codeset_num);
	UtfData	pdata;

	if(!utfdata_list) {
	    utfdata_list = make_entry();
	}
	pdata = utfdata_list;
	for(i=0; i < codeset_num; i++) {
	    XlcCharSet	charset = *codeset_list[i]->charset_list;
	    while(pdata->next) {
		if(charset == pdata->charset) {
		    break;
		}
		pdata = pdata->next;
	    }
	    if(pdata->next) {
		continue;
	    } else {
		int j;
		for(j = 0; j < MAX_UTF_CHARSET; j++) {
		    if(_XlcCompareISOLatin1(charset->encoding_name, default_utf_data[j].name) ||
		       charset->side != default_utf_data[j].side) {
			continue;
		    } else {
 			pdata->initialize = default_utf_data[j].initialize;
			pdata->fromtbl = (long *)Xmalloc(NRUNE * sizeof(long));
			(*pdata->initialize)(pdata->fromtbl, default_utf_data[j].fallback_value);
			pdata->already_init = True;
			pdata->charset = charset;
			pdata->cstorune = default_utf_data[j].cstorune;
			pdata->type = default_utf_data[j].type;
			pdata->next = make_entry();
			break;
		    }
		}
	    }
	}
	once = 1;
    }
    return 1;
}

static int
utftocs(conv, from, from_left, to, to_left, args, num_args)
    XlcConv	conv;
    char	**from;
    int		*from_left;
    char	**to;
    int		*to_left;
    XPointer	*args;
    int		num_args;
{
    char	*utfptr;
    char 	*bufptr;
    int		utf_len, buf_len;
    long	l;
    XlcCharSet	tmpcharset = (XlcCharSet)NULL;
    UtfData	pdata = utfdata_list;

    if (from == NULL || *from == NULL)
	return 0;

    utfptr = *from;
    bufptr = *to;
    utf_len = *from_left;
    buf_len = *to_left;

    while(utf_len > 0 && buf_len > 0) {
	char *p = utfptr;
	if((l = getutfrune(&p, &utf_len)) == -2) {
	    return -1;
	} else {
	    while(pdata->next) {
		long	r;
		long	*tbl;

		tbl = pdata->fromtbl;
		tbl += l;
		if((r = *tbl) == -1) {
		    if(tmpcharset) {
			    goto end;
		    } else {
			pdata = pdata->next;
			continue;
		    }
		} else {
		    utfptr = p;
		    if(!tmpcharset) tmpcharset = pdata->charset;
		}
		if(r < 128) {
		    *bufptr++ = r;
		    buf_len--;
		} else {
		    switch(pdata->type) {
			case N11n_ja:
			    *bufptr++ = (r/100 + ' ');
			    *bufptr++ = (r%100 + ' ');
			    break;
			case N11n_ko:
			    *bufptr++ = (r/94 + 0x21);
			    *bufptr++ = (r%94 + 0x21);
			    break;
			case N11n_zh:
			    *bufptr++ = 0x20 + (r/100);
			    *bufptr++ = 0x20 + (r%100);
			    break;
			default:
			    break;
		    }
		    buf_len -= 2;
		}
		break;
	    }
	    if(!tmpcharset) return (-1); /* Unknown Codepoint */
	}
    }
end:
    if((num_args > 0) && tmpcharset)
	*((XlcCharSet *) args[0]) = tmpcharset;

    *from_left -= utfptr - *from;
    *from = utfptr;

    *to_left -= bufptr - *to;
    *to = bufptr;

    return 0;
}

static int
utf1tocs(conv, from, from_left, to, to_left, args, num_args)
    XlcConv	conv;
    char	**from;
    int		*from_left;
    char	**to;
    int		*to_left;
    XPointer	*args;
    int		num_args;
{
    char	**ptr = NULL;
    char	char_ptr[UTFmax];
    int		i = 0;
    unsigned long	dummy = (unsigned long)0;

    if (from == NULL || *from == NULL)
	return utftocs(conv, from, from_left, to, to_left, args, num_args);

    ptr = from;
    for(i = 0; i < UTFmax; char_ptr[i++] = *(*ptr)++);
    i=0;
    while(our_mbtowc(&dummy, (char*)&char_ptr[0], i) <= 0)
	i++;
    return utftocs(conv, from, &i, to, to_left, args, num_args);
}

static int
ucstocs(conv, from, from_left, to, to_left, args, num_args)
    XlcConv	conv;
    XPointer	*from;
    int		*from_left;
    char	**to;
    int		*to_left;
    XPointer	*args;
    int		num_args;
{
    wchar_t	*ucsptr;
    char 	*bufptr;
    int		ucs_len, buf_len;
    XlcCharSet	tmpcharset = (XlcCharSet)NULL;
    UtfData	pdata = utfdata_list;

    if (from == NULL || *from == NULL)
	return 0;

    ucsptr = (wchar_t *)*from;
    bufptr = *to;
    ucs_len = *from_left;
    buf_len = *to_left;

    while(ucs_len > 0 && buf_len > 0) {
	while(pdata->next) {
	    long	r;
	    long	*tbl;

	    tbl = pdata->fromtbl;
	    tbl += *ucsptr;
	    if((r = *tbl) == -1) {
		if(tmpcharset) {
		    goto end;
		} else {
		    pdata = pdata->next;
		    continue;
		}
	    } else {
		if(!tmpcharset) tmpcharset = pdata->charset;
	    }
	    ucsptr++;
	    if(r < 128) {
		*bufptr++ = r;
		ucs_len--;
		buf_len--;
	    } else {
		switch(pdata->type) {
		case N11n_ja:
		    *bufptr++ = (r/100 + ' ');
		    *bufptr++ = (r%100 + ' ');
		    break;
		case N11n_ko:
		    *bufptr++ = (r/94 + 0x21);
		    *bufptr++ = (r%94 + 0x21);
		    break;
		case N11n_zh:
		    *bufptr++ = 0x20 + (r/100);
		    *bufptr++ = 0x20 + (r%100);
		    break;
		default:
		    break;
		}
		ucs_len--;
		buf_len -= 2;
	    }
	    break;
	}
	if(!tmpcharset) return (-1); /* Unknown Codepoint */
    }
end:
    if((num_args > 0) && tmpcharset)
	*((XlcCharSet *) args[0]) = tmpcharset;

    *from_left -= ucsptr - (wchar_t *)*from;
    *from = (XPointer)ucsptr;

    *to_left -= bufptr - *to;
    *to = bufptr;

    return 0;
}

static long
#if NeedFunctionPrototypes
getutfrune(char **read_from, int *from_len)
#else
getutfrune(read_from, from_len)
char **read_from;
int *from_len;
#endif
{
    int c, i;
    char str[UTFmax]; /* MB_LEN_MAX really */
    unsigned long l;
    int n;

    str[0] = '\0';
    for(i = 0; i <= UTFmax;) {
	c = **read_from;
	(*read_from)++;
	str[i++] = c;
	n = our_mbtowc(&l, str, i);
	if(n == -1)
	    return(-2);
	if(n > 0) {
	    *from_len -= n;
	    return(l);
	}
    }
    return(-2);
}

static
cstoutf(conv, from, from_left, to, to_left, args, num_args)
    XlcConv conv;
    char **from;
    int *from_left;
    char **to;
    int *to_left;
    XPointer *args;
    int num_args;
{
    XlcCharSet charset;
    char *csptr, *utfptr;
    int csstr_len, utf_len;
    int               cmp_len = 0;
    void	(*putrune)(
#if NeedFunctionPrototypes
                            unsigned char c,
                            Rune *r
#endif
			   ) = NULL;
    Rune	r = (Rune)0;
    UtfData	pdata = utfdata_list;

    if (from == NULL || *from == NULL)
	return 0;
    
    if (num_args < 1)
        return -1;

    csptr = *from;
    utfptr = *to;
    csstr_len = *from_left;
    utf_len = *to_left;

    charset = (XlcCharSet)args[0];
    cmp_len = strchr(charset->name, ':') - charset->name;
    while(pdata->next) {
        if(!_XlcNCompareISOLatin1(charset->name, pdata->charset->name, cmp_len)) {
	    putrune = pdata->cstorune;
	    break;
	} else {
	    pdata = pdata->next;
	}
    }
    if(!putrune)
	return -1;

    while(csstr_len-- > 0 && utf_len > 0) {
	(*putrune)(*csptr++, &r);
	if(!r) {
	    continue;
	}
	our_wctomb(r, &utfptr, &utf_len);
	r = 0;
    }
 
    *from_left -= csptr - *from;
    *from = csptr;
 
    *to_left -= utfptr - *to;
    *to = utfptr;
 
    return 0;
}

static
cstoucs(conv, from, from_left, to, to_left, args, num_args)
    XlcConv conv;
    char **from;
    int *from_left;
    XPointer *to;
    int *to_left;
    XPointer *args;
    int num_args;
{
    XlcCharSet charset;
    char *csptr;
    wchar_t *ucsptr;
    int csstr_len, ucs_len;
    int               cmp_len = 0;
    void	(*putrune)(
#if NeedFunctionPrototypes
                            unsigned char c,
                            Rune *r
#endif
			   ) = NULL;
    Rune	r = (Rune)0;
    UtfData	pdata = utfdata_list;

    if (from == NULL || *from == NULL)
	return 0;
    
    if (num_args < 1)
        return -1;

    csptr = *from;
    ucsptr = (wchar_t *)*to;
    csstr_len = *from_left;
    ucs_len = *to_left;
    charset = (XlcCharSet)args[0];
    cmp_len = strchr(charset->name, ':') - charset->name;

    while(pdata->next) {
        if(!_XlcNCompareISOLatin1(charset->name, pdata->charset->name, cmp_len)) {
	    putrune = pdata->cstorune;
	    break;
	} else {
	    pdata = pdata->next;
	}
    }
    if(!putrune)
	return -1;

    while(csstr_len-- > 0 && ucs_len > 0) {
	(*putrune)(*csptr++, &r);
	if(!r) {
	    continue;
	}
	*ucsptr = (long)r;
	ucsptr++;
	ucs_len--;
	r = 0;
    }
 
    *from_left -= csptr - *from;
    *from = csptr;
 
    *to_left -= ucsptr - (wchar_t *)*to;
    *to = (XPointer)ucsptr;
 
    return 0;
}

static void
#if NeedFunctionPrototypes
our_wctomb(Rune r, char **utfptr, int *utf_len)
#else
our_wctomb(r, utfptr, utf_len)
Rune r;
char **utfptr;
int *utf_len;
#endif
{
    long l = (long)r;

    if(!utfptr || !*utfptr)
	return;               /* no shift states */
    if(l & ~Wchar2) {
	if(l & ~Wchar4) {
	    if(l & ~Wchar5) {
		/* 6 bytes */
		*(*utfptr)++ = T6 | ((l >> 5*Bitx) & Mask6);
		*(*utfptr)++ = Tx | ((l >> 4*Bitx) & Maskx);
		*(*utfptr)++  = Tx | ((l >> 3*Bitx) & Maskx);
		*(*utfptr)++  = Tx | ((l >> 2*Bitx) & Maskx);
		*(*utfptr)++  = Tx | ((l >> 1*Bitx) & Maskx);
		*(*utfptr)++  = Tx |  (l & Maskx);
		*utf_len -= 6;
		return;
	    }
	    /* 5 bytes */
	    *(*utfptr)++ = T5 |  (l >> 4*Bitx);
	    *(*utfptr)++ = Tx | ((l >> 3*Bitx) & Maskx);
	    *(*utfptr)++ = Tx | ((l >> 2*Bitx) & Maskx);
	    *(*utfptr)++ = Tx | ((l >> 1*Bitx) & Maskx);
	    *(*utfptr)++ = Tx |  (l & Maskx);
	    *utf_len -= 5;
	    return;
	}
	if(l & ~Wchar3) {
	    /* 4 bytes */
	    *(*utfptr)++ = T4 |  (l >> 3*Bitx);
	    *(*utfptr)++ = Tx | ((l >> 2*Bitx) & Maskx);
	    *(*utfptr)++ = Tx | ((l >> 1*Bitx) & Maskx);
	    *(*utfptr)++ = Tx |  (l & Maskx);
	    *utf_len -= 4;
	    return;
	}
	/* 3 bytes */
	*(*utfptr)++ = T3 |  (l >> 2*Bitx);
	*(*utfptr)++ = Tx | ((l >> 1*Bitx) & Maskx);
	*(*utfptr)++ = Tx |  (l & Maskx);
	*utf_len -= 3;
	return;
    }
    if(l & ~Wchar1) {
	/* 2 bytes */
	*(*utfptr)++ = T2 | (l >> 1*Bitx);
	*(*utfptr)++ = Tx | (l & Maskx);
	*utf_len -= 2;
	return;
    }
    /* 1 byte */
    *(*utfptr)++ = T1 | l;
    *utf_len -= 1;
    return;
}

static void
#if NeedFunctionPrototypes
latin2rune(unsigned char c, Rune *r)
#else
latin2rune(c, r)
unsigned char c;
Rune *r;
#endif
{
    *r = (Rune)c;
    return;
}

static void
#if NeedFunctionPrototypes
ksc2rune(unsigned char c, Rune *r)
#else
ksc2rune(c, r)
unsigned char c;
Rune *r;
#endif
{
    static enum { init, cs1last} state = init;
    static int korean646 = 1; /* fixed to 1 for now. */
    static int lastc;
    unsigned char	ch = (c|0x80); /* XXX */
    int n;
    long l;

    switch(state) {
    case init:
	if (ch < 128){
	    if(korean646 && (ch=='\\')){
		emit(0x20A9);
	    } else {
		emit(ch);
	    }
	}else{
	    lastc = ch;
	    state = cs1last;
	}
	return;
        
    case cs1last: /* 2nd byte of codeset 1 (KSC 5601) */
	n = ((lastc&0x7f)-33)*94 + (ch&0x7f)-33;
	if((l = tabksc[n]) == 0){
	    emit(BADMAP);
	} else {
	    emit(l);
	}
	state = init;
	return;
    }
}

static void
#if NeedFunctionPrototypes
jis02012rune(unsigned char c, Rune *r)
#else
jis02012rune(c, r)
    unsigned char c;
    Rune *r;
#endif
{
/* To Be Implemented */
}

static void
#if NeedFunctionPrototypes
gb2rune(unsigned char c, Rune *r)
#else
gb2rune(c, r)
    unsigned char c;
    Rune *r;
#endif
{
    static enum { state0, state1 } state = state0;
    static int lastc;
    long n, ch;
    unsigned char	ch1 = (c|0x80); /* XXX */

    switch(state) {
    case state0:    /* idle state */
	if(ch1 >= 0xA1){
	    lastc = ch1;
	    state = state1;
	    return;
	}
        emit(ch1);
        return;

    case state1:    /* seen a font spec */
	if(ch1 >= 0xA1)
	    n = (lastc-0xA0)*100 + (ch1-0xA0);
        else {
	    emit(BADMAP);
	    state = state0;
	    return;
        }
        ch = tabgb[n];
        if(ch < 0){
	    emit(BADMAP);
        } else
	    emit(ch);
        state = state0;
    }
}

static void
#if NeedFunctionPrototypes
jis02082rune(unsigned char c, Rune *r)
#else
jis02082rune(c, r)
    unsigned char c;
    Rune *r;
#endif
{
    static enum { state0, state1} state = state0;
    static int lastc;
    unsigned char	ch = (c|0x80); /* XXX */
    int n;
    long l;

again:
    switch(state) {
    case state0:    /* idle state */
	lastc = ch;
	state = state1;
	return;

    case state1:    /* two part char */
	if((lastc&0x80) != (ch&0x80)){
	    emit(lastc);
	    state = state0;
	    goto again;
	}
	if(CANS2J(lastc, ch)){
	    int h = lastc, l = ch;
	    S2J(h, l);
	    n = h*100 + l - 3232;
	} else
	    n = (lastc&0x7F)*100 + (ch&0x7f) - 3232; /* kuten */
	if((l = tabkuten[n]) == -1){
	    emit(BADMAP);
	} else {          
	    if(l < 0){
		l = -l;
	    }
	    emit(l);
	}        
	state = state0;
    }
}

static int
#if NeedFunctionPrototypes
our_mbtowc(unsigned long *p, char *s, size_t n)
#else
our_mbtowc(p, s, n)
    unsigned long *p;
    char *s;
    size_t n;
#endif
{
    unsigned char *us;
    int c0, c1, c2, c3, c4, c5;
    unsigned long wc;

    if(s == 0)
	return 0;		/* no shift states */

    if(n < 1)
	goto badlen;
    us = (unsigned char*)s;
    c0 = us[0];
    if(c0 >= T3) {
	if(n < 3)
	    goto badlen;
	c1 = us[1] ^ Tx;
	c2 = us[2] ^ Tx;
	if((c1|c2) & T2) {
	    goto bad;
	}
	if(c0 >= T5) {
	    if(n < 5)
		goto badlen;
	    c3 = us[3] ^ Tx;
	    c4 = us[4] ^ Tx;
	    if((c3|c4) & T2) {
		goto bad;
	    }
	    if(c0 >= T6) {
		/* 6 bytes */
		if(n < 6)
		    goto badlen;
		c5 = us[5] ^ Tx;
		if(c5 & T2) {
		    goto bad;
		}
		wc = ((((((((((c0 & Mask6) << Bitx) | c1) 
					   << Bitx) | c2) 
					   << Bitx) | c3) 
					   << Bitx) | c4) 
					   << Bitx) | c5;
		if(wc <= Wchar5) {
		    goto bad;
		}
		*p = wc;
		return 6;
	    }
	    /* 5 bytes */
	    wc = ((((((((c0 & Mask5) << Bitx) | c1) 
				     << Bitx) | c2) 
				     << Bitx) | c3) 
				     << Bitx) | c4;
	    if(wc <= Wchar4) {
				goto bad;
	    }
	    *p = wc;
	    return 5;
	}
	if(c0 >= T4) {
	    /* 4 bytes */
	    if(n < 4)
		goto badlen;
	    c3 = us[3] ^ Tx;
	    if(c3 & T2) {
		goto bad;
	    }
	    wc = ((((((c0 & Mask4) << Bitx) | c1) 
				   << Bitx) | c2) 
				   << Bitx) | c3;
	    if(wc <= Wchar3) {
		goto bad;
	    }
	    *p = wc;
	    return 4;
	}
	/* 3 bytes */
	wc = ((((c0 & Mask3) << Bitx) | c1) 
			     << Bitx) | c2;
	if(wc <= Wchar2) {
	    goto bad;
	}
	*p = wc;
	return 3;
    }
    if(c0 >= T2) {
	/* 2 bytes */
	if(n < 2)
	    goto badlen;
	c1 = us[1] ^ Tx;
	if(c1 & T2) {
	    goto bad;
	}
	wc = ((c0 & Mask2) << Bitx) | c1;
	if(wc <= Wchar1) {
	    goto bad;
	}
	*p = wc;
	return 2;
    }
    /* 1 byte */
    if(c0 >= Tx) {
	goto bad;
    }
    *p = c0;
    return 1;

bad:
    errno = EILSEQ;
    return -1;
badlen:
    return -2;
}

static void
close_converter(conv)
    XlcConv conv;
{
    Xfree((char *) conv);
}

static XlcConv
create_conv(lcd, methods)
    XLCd	lcd;
    XlcConvMethods	methods;
{
    XlcConv conv;

    conv = (XlcConv) Xmalloc(sizeof(XlcConvRec));
    if (conv == (XlcConv) NULL)
	return (XlcConv) NULL;

    conv->methods = methods;

    conv->state = NULL;
    _XlcInitUTFInfo(lcd);

    return conv;
}

static XlcConvMethodsRec mbtocs_methods = {
    close_converter,
    utf1tocs,
    NULL
};

static XlcConv
open_mbtocs(from_lcd, from, to_lcd, to)
    XLCd from_lcd;
    char *from;
    XLCd to_lcd;
    char *to;
{
    return create_conv(from_lcd, &mbtocs_methods);
}

static XlcConvMethodsRec mbstocs_methods = {
    close_converter,
    utftocs,
    NULL
};

static XlcConv
open_mbstocs(from_lcd, from, to_lcd, to)
    XLCd from_lcd;
    char *from;
    XLCd to_lcd;
    char *to;
{
    return create_conv(from_lcd, &mbstocs_methods);
}

static XlcConvMethodsRec wcstocs_methods = {
    close_converter,
    ucstocs,
    NULL
};

static XlcConv
open_wcstocs(from_lcd, from, to_lcd, to)
    XLCd from_lcd;
    char *from;
    XLCd to_lcd;
    char *to;
{
    return create_conv(from_lcd, &wcstocs_methods);
}

static XlcConvMethodsRec cstombs_methods = {
    close_converter,
    cstoutf,
    NULL
};

static XlcConv
open_cstombs(from_lcd, from, to_lcd, to)
    XLCd from_lcd;
    char *from;
    XLCd to_lcd;
    char *to;
{
    return create_conv(from_lcd, &cstombs_methods);
}

static XlcConvMethodsRec cstowcs_methods = {
    close_converter,
    cstoucs,
    NULL
};

static XlcConv
open_cstowcs(from_lcd, from, to_lcd, to)
    XLCd from_lcd;
    char *from;
    XLCd to_lcd;
    char *to;
{
    return create_conv(from_lcd, &cstowcs_methods);
}


XLCd
_XlcUtfLoader(name)
    char *name;
{
    XLCd lcd;

    lcd = _XlcCreateLC(name, _XlcGenericMethods);
    if (lcd == (XLCd) NULL)
	return lcd;
    
    if ((_XlcCompareISOLatin1(XLC_PUBLIC_PART(lcd)->codeset, "utf"))) {
	_XlcDestroyLC(lcd);
	return (XLCd) NULL;
    }

    _XlcSetConverter(lcd, XlcNMultiByte, lcd, XlcNCharSet, open_mbstocs);
    _XlcSetConverter(lcd, XlcNWideChar, lcd, XlcNCharSet, open_wcstocs);

    _XlcSetConverter(lcd, XlcNMultiByte, lcd, XlcNChar, open_mbtocs);

    _XlcSetConverter(lcd, XlcNCharSet, lcd, XlcNMultiByte, open_cstombs);
    _XlcSetConverter(lcd, XlcNCharSet, lcd, XlcNWideChar, open_cstowcs);

    return lcd;
}
