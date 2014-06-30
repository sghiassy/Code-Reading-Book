/* $XConsortium: omGeneric.c /main/20 1996/12/05 10:40:40 swick $ */
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
 * Copyright 1995 by FUJITSU LIMITED
 * This is source code modified by FUJITSU LIMITED under the Joint
 * Development Agreement for the CDE/Motif PST.
 *
 * Modifier: Takanori Tateno   FUJITSU LIMITED
 *
 */
/* $XFree86: xc/lib/X11/omGeneric.c,v 3.5 1996/12/26 01:37:33 dawes Exp $ */

#include "Xlibint.h"
#include "XomGeneric.h"
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXFONTS		100
#define	PIXEL_SIZE_FIELD	 7
#define	POINT_SIZE_FIELD	 8
#define	CHARSET_ENCODING_FIELD	14

extern int _XmbDefaultTextEscapement(), _XwcDefaultTextEscapement();
extern int _XmbDefaultTextExtents(), _XwcDefaultTextExtents();
extern Status _XmbDefaultTextPerCharExtents(), _XwcDefaultTextPerCharExtents();
extern int _XmbDefaultDrawString(), _XwcDefaultDrawString();
extern void _XmbDefaultDrawImageString(), _XwcDefaultDrawImageString();

extern int _XmbGenericTextEscapement(), _XwcGenericTextEscapement();
extern int _XmbGenericTextExtents(), _XwcGenericTextExtents();
extern Status _XmbGenericTextPerCharExtents(), _XwcGenericTextPerCharExtents();
extern int _XmbGenericDrawString(), _XwcGenericDrawString();
extern void _XmbGenericDrawImageString(), _XwcGenericDrawImageString();

extern void _XlcDbg_printValue();

/* For VW/UDC start */

static FontData
init_fontdata(font_data, font_data_count)
    FontData	font_data;
    int		font_data_count;
{
    FontData	fd;
    int		i;

    fd = (FontData)Xmalloc(sizeof(FontDataRec) * font_data_count);
    if(fd == (FontData) NULL)
	return False;

    memset(fd, 0x00, sizeof(FontData) * font_data_count);
    for(i = 0 ; i < font_data_count ; i++)
	fd[i] = font_data[i];

    return fd;
}

static VRotate
init_vrotate(font_data, font_data_count, type, code_range, code_range_num)
    FontData	font_data;
    int		font_data_count;
    int		type;
    CodeRange	code_range;
    int		code_range_num;
{
    VRotate	vrotate;
    int		i;

    if(type == VROTATE_NONE)
	return (VRotate)NULL;

    vrotate = (VRotate)Xmalloc(sizeof(VRotateRec) * font_data_count);
    if(vrotate == (VRotate) NULL)
	return False;

    memset(vrotate, 0x00, sizeof(VRotateRec) * font_data_count);
    for(i = 0 ; i < font_data_count ; i++) {
	vrotate[i].charset_name = font_data[i].name;
	vrotate[i].side = font_data[i].side;
	if(type == VROTATE_PART) {
	    vrotate[i].num_cr = code_range_num;
	    vrotate[i].code_range = code_range;
	}
    }

    return vrotate;
}

static Bool
init_fontset(oc)
    XOC oc;
{
    XOCGenericPart *gen;
    FontSet font_set;
    OMData data;
    int count;

    count = XOM_GENERIC(oc->core.om)->data_num;
    data = XOM_GENERIC(oc->core.om)->data;

    font_set = (FontSet) Xmalloc(sizeof(FontSetRec) * count);
    if (font_set == NULL)
	return False;
    memset((char *) font_set, 0x00, sizeof(FontSetRec) * count);

    gen = XOC_GENERIC(oc);
    gen->font_set_num = count;
    gen->font_set = font_set;

    for ( ; count-- > 0; data++, font_set++) {
	font_set->charset_count = data->charset_count;
	font_set->charset_list = data->charset_list;

	if((font_set->font_data = init_fontdata(data->font_data,
				  data->font_data_count)) == NULL)
	    goto err;
	font_set->font_data_count = data->font_data_count;
	if((font_set->substitute = init_fontdata(data->substitute,
				   data->substitute_num)) == NULL)
	    goto err;
	font_set->substitute_num = data->substitute_num;
	if((font_set->vmap = init_fontdata(data->vmap,
			     data->vmap_num)) == NULL)
	    goto err;
	font_set->vmap_num       = data->vmap_num;

	if(data->vrotate_type != VROTATE_NONE) {
	    /* A vrotate member is specified primary font data */
	    /* as initial value.                               */
	    if((font_set->vrotate = init_vrotate(data->font_data,
						 data->font_data_count,
						 data->vrotate_type,
						 data->vrotate,
						 data->vrotate_num)) == NULL)
		goto err;
	    font_set->vrotate_num = data->font_data_count;
	}
    }
    return True;

err:
    if(font_set->font_data)
	Xfree(font_set->font_data);
    if(font_set->substitute)
	Xfree(font_set->substitute);
    if(font_set->vmap)
	Xfree(font_set->vmap);
    if(font_set->vrotate)
	Xfree(font_set->vrotate);
    if(font_set)
	Xfree(font_set);
    gen->font_set = (FontSet) NULL;
    gen->font_set_num = 0;
    return False;
}

/* For VW/UDC end */

static char *
get_prop_name(dpy, fs)
    Display *dpy;
    XFontStruct	*fs;
{
    unsigned long fp;

    if (XGetFontProperty(fs, XA_FONT, &fp))
	return XGetAtomName(dpy, fp); 

    return (char *) NULL;
}

static FontData
check_charset(font_set, font_name)
    FontSet font_set;
    char *font_name;
{
    FontData font_data;
    char *last;
    int count, length, name_len;

    name_len = strlen(font_name);
    last = font_name + name_len;

    count = font_set->font_data_count;
    font_data = font_set->font_data;
    for ( ; count-- > 0; font_data++) {
	length = strlen(font_data->name);
	if (length > name_len)
	    continue;
	
	if (_XlcCompareISOLatin1(last - length, font_data->name) == 0)
	    return font_data;
    }

    return (FontData) NULL;
}

static int
check_fontname(oc, name, found_num)
    XOC oc;
    char *name;
    int found_num;
{
    Display *dpy = oc->core.om->core.display;
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontData data;
    FontSet font_set;
    XFontStruct *fs_list;
    char **fn_list, *fname, *prop_fname = NULL;
    int list_num, font_set_num, i;
    int list2_num;
    char **fn2_list = NULL;

    fn_list = XListFonts(dpy, name, MAXFONTS, &list_num);
    if (fn_list == NULL)
	return found_num;

    for (i = 0; i < list_num; i++) {
	fname = fn_list[i];

	font_set = gen->font_set;
	font_set_num = gen->font_set_num;

	for ( ; font_set_num-- > 0; font_set++) {
	    if (font_set->font_name)
		continue;

	    if ((data = check_charset(font_set, fname)) == NULL) {
		if ((fn2_list = XListFontsWithInfo(dpy, name, MAXFONTS,
					      &list2_num, &fs_list))
		    && (prop_fname = get_prop_name(dpy, fs_list))
		    && (data = check_charset(font_set, prop_fname)))
		    fname = prop_fname;
	    }
	    if (data) {
		font_set->side = data->side;
		font_set->font_name = (char *) Xmalloc(strlen(fname) + 1);
		if (font_set->font_name) {
		    strcpy(font_set->font_name, fname);
		    found_num++;
		}
	    }
	    if (fn2_list) {
		XFreeFontInfo(fn2_list, fs_list, list2_num);
		fn2_list = NULL;
		if (prop_fname) {
		    Xfree(prop_fname);
		    prop_fname = NULL;
		}
	    }
	    if (found_num == gen->font_set_num)
		break;
	}
    }
    XFreeFontNames(fn_list);
    return found_num;
}


/* For VW/UDC start */

static Bool
load_fontdata(oc, font_data, font_data_num)
    XOC		oc;
    FontData	font_data;
    int		font_data_num;
{
    Display	*dpy = oc->core.om->core.display;
    FontData	fd = font_data;

    if(font_data == NULL) return(True);
    for( ; font_data_num-- ; fd++) {
	if(fd->xlfd_name != (char *) NULL && fd->font == NULL) {
	    fd->font = XLoadQueryFont(dpy, fd->xlfd_name);
	    if (fd->font == NULL){
		return False;
	    }
	}
    }
    return True;
}

static Bool
load_font(oc)
    XOC oc;
{
    int i;
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set = gen->font_set;
    int num = gen->font_set_num;

    for ( ; num-- > 0; font_set++) {
	if (font_set->font_name == NULL)
	    continue;

	if(load_fontdata(oc, font_set->font_data,
			 font_set->font_data_count) != True)
	    return False;

	if(load_fontdata(oc, font_set->substitute,
			 font_set->substitute_num) != True)
	    return False;

	if(font_set->font_data_count > 0 && font_set->font_data->font) {
	    font_set->font = font_set->font_data->font;
	} else if(font_set->substitute_num > 0 ) {
	    for(i=0;i<font_set->substitute_num;i++){
		if(font_set->substitute[i].font != NULL){
		    font_set->font = font_set->substitute[i].font;
		    break ;
		}
	    }
	}

/* Add 1996.05.20 */
        if( oc->core.orientation == XOMOrientation_TTB_RTL ||
            oc->core.orientation == XOMOrientation_TTB_LTR ){
	    if (font_set->vpart_initialize == 0) {
	       load_fontdata(oc, font_set->vmap, font_set->vmap_num);
	       load_fontdata(oc, (FontData) font_set->vrotate,
			 font_set->vrotate_num);
                font_set->vpart_initialize = 1;
	    }
        }

	if (font_set->font->min_byte1 || font_set->font->max_byte1)
	    font_set->is_xchar2b = True;
	else
	    font_set->is_xchar2b = False;
    }

    return True;
}

/* For VW/UDC end */

static Bool
load_font_info(oc)
    XOC oc;
{
    Display *dpy = oc->core.om->core.display;
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set = gen->font_set;
    char **fn_list;
    int fn_num, num = gen->font_set_num;

    for ( ; num-- > 0; font_set++) {
	if (font_set->font_name == NULL)
	    continue;

	if (font_set->info == NULL) {
	    fn_list = XListFontsWithInfo(dpy, font_set->font_name, 1, &fn_num,
					 &font_set->info);
	    if (font_set->info == NULL)
		return False;
	    
	    XFreeFontNames(fn_list);
	}
    }

    return True;
}

/* For Vertical Writing start */

static void
check_fontset_extents(overall, logical_ascent, logical_descent, font)
    XCharStruct		*overall;
    int			*logical_ascent, *logical_descent;
    XFontStruct		*font;
{
    overall->lbearing = min(overall->lbearing, font->min_bounds.lbearing);
    overall->rbearing = max(overall->rbearing, font->max_bounds.rbearing);
    overall->ascent   = max(overall->ascent,   font->max_bounds.ascent);
    overall->descent  = max(overall->descent,  font->max_bounds.descent);
    overall->width    = max(overall->width,    font->max_bounds.width);
    *logical_ascent   = max(*logical_ascent,   font->ascent);
    *logical_descent  = max(*logical_descent,  font->descent);
}

/* For Vertical Writing end */

static void
set_fontset_extents(oc)
    XOC oc;
{
    XRectangle *ink = &oc->core.font_set_extents.max_ink_extent;
    XRectangle *logical = &oc->core.font_set_extents.max_logical_extent;
    XFontStruct **font_list, *font;
    XCharStruct overall;
    int logical_ascent, logical_descent;
    int	num = oc->core.font_info.num_font;

    font_list = oc->core.font_info.font_struct_list;
    font = *font_list++;
    overall = font->max_bounds;
    overall.lbearing = font->min_bounds.lbearing;
    logical_ascent = font->ascent;
    logical_descent = font->descent;

    /* For Vertical Writing start */

    while (--num > 0) {
	font = *font_list++;
	check_fontset_extents(&overall, &logical_ascent, &logical_descent,
			      font);
    }

    {
	XOCGenericPart  *gen = XOC_GENERIC(oc);
	FontSet		font_set = gen->font_set;
	FontData	font_data;
	int		font_set_num = gen->font_set_num;
	int		font_data_count;

	for( ; font_set_num-- ; font_set++) {
	    if(font_set->vmap_num > 0) {
		font_data = font_set->vmap;
		font_data_count = font_set->vmap_num;
		for( ; font_data_count-- ; font_data++) {
		    if(font_data->font != NULL) {
			check_fontset_extents(&overall, &logical_ascent,
					      &logical_descent,
					      font_data->font);
		    }
		}
	    }

	    if(font_set->vrotate_num > 0) {
		font_data = (FontData) font_set->vrotate;
		font_data_count = font_set->vrotate_num;
		for( ; font_data_count-- ; font_data++) {
		    if(font_data != NULL)
		    if(font_data->font != NULL) {
			check_fontset_extents(&overall, &logical_ascent,
					      &logical_descent,
					      font_data->font);
		    }
		}
	    }
	}
    }

    /* For Vertical Writing start */

    ink->x = overall.lbearing;
    ink->y = -(overall.ascent);
    ink->width = overall.rbearing - overall.lbearing;
    ink->height = overall.ascent + overall.descent;

    logical->x = 0;
    logical->y = -(logical_ascent);
    logical->width = overall.width;
    logical->height = logical_ascent + logical_descent;
}

static Bool
init_core_part(oc)
    XOC oc;
{
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set;
    int font_set_num;
    XFontStruct **font_struct_list;
    char **font_name_list, *font_name_buf;
    int	count, length;

    font_set = gen->font_set;
    font_set_num = gen->font_set_num;
    count = length = 0;

    for ( ; font_set_num-- > 0; font_set++) {
	if (font_set->font_name == NULL)
	    continue;

	length += strlen(font_set->font_name) + 1;
	count++;
    }
    if (count == 0)
        return False;

    font_struct_list = (XFontStruct **) Xmalloc(sizeof(XFontStruct *) * count);
    if (font_struct_list == NULL)
	return False;

    font_name_list = (char **) Xmalloc(sizeof(char *) * count);
    if (font_name_list == NULL)
	goto err;

    font_name_buf = (char *) Xmalloc(length);
    if (font_name_buf == NULL)
	goto err;

    oc->core.font_info.num_font = count;
    oc->core.font_info.font_name_list = font_name_list;
    oc->core.font_info.font_struct_list = font_struct_list;

    font_set = gen->font_set;
    font_set_num = gen->font_set_num;

    for (count = 0; font_set_num-- > 0; font_set++, count++) {
	if (font_set->font_name == NULL)
	    continue;

	font_set->id = count;
	if (font_set->font)
	    *font_struct_list++ = font_set->font;
	else
	    *font_struct_list++ = font_set->info;
	strcpy(font_name_buf, font_set->font_name);
	Xfree(font_set->font_name);
	*font_name_list++ = font_set->font_name = font_name_buf;
	font_name_buf += strlen(font_name_buf) + 1;
    }

    set_fontset_extents(oc);

    return True;

err:
    if (font_name_list)
	Xfree(font_name_list);
    Xfree(font_struct_list);

    return False;
}

static char *
get_font_name(oc, pattern)
    XOC oc;
    char *pattern;
{
    char **list, *name;
    int count = 0;

    list = XListFonts(oc->core.om->core.display, pattern, 1, &count);
    if (list == NULL)
	return NULL;

    name = (char *) Xmalloc(strlen(*list) + 1);
    if (name)
	strcpy(name, *list);
    
    XFreeFontNames(list);

    return name;
}

/* For VW/UDC start*/

static char
*get_rotate_fontname(font_name)
    char *font_name;
{
    char *pattern = NULL, *ptr = NULL;
    char *fields[CHARSET_ENCODING_FIELD];
    char str_pixel[32], str_point[4];
    char rotate_font[256];
    char *rotate_font_ptr = NULL;
    int pixel_size = 0;
    int field_num = 0, len = 0;

    if(font_name == (char *) NULL || (len = strlen(font_name)) <= 0)
	return NULL;

    pattern = (char *)Xmalloc(len + 1);
    if(!pattern)
	return NULL;
    strcpy(pattern, font_name);

    memset(fields, '\0', sizeof(char *) * 14);
    ptr = pattern;
    while(isspace(*ptr)) {
	ptr++;
    }
    if(*ptr == '-')
	ptr++;

    for(field_num = 0 ; field_num < CHARSET_ENCODING_FIELD && ptr && *ptr ;
			ptr++, field_num++) {
	fields[field_num] = ptr;

	if(ptr = strchr(ptr, '-')) {
	    *ptr = '\0';
	}
    }

    if(field_num < CHARSET_ENCODING_FIELD)
	return NULL;

    /* Pixel Size field : fields[6] */
    for(ptr = fields[PIXEL_SIZE_FIELD - 1] ; ptr && *ptr; ptr++) {
	if(!isdigit(*ptr)) {
	    if(*ptr == '['){ /* 960730 */
	        strcpy(pattern, font_name);
		return(pattern);
	    }
	    if(pattern)
		Xfree(pattern);
	    return NULL;
	}
    }
    pixel_size = atoi(fields[PIXEL_SIZE_FIELD - 1]);
    sprintf(str_pixel, "[ 0 ~%d %d 0 ]", pixel_size, pixel_size);
    fields[6] = str_pixel;

    /* Point Size field : fields[7] */
    strcpy(str_point, "*");
    fields[POINT_SIZE_FIELD - 1] = str_point;

    rotate_font[0] = '\0';
    for(field_num = 0 ; field_num < CHARSET_ENCODING_FIELD &&
			fields[field_num] ; field_num++) {
	sprintf(rotate_font, "%s-%s", rotate_font, fields[field_num]);
    }

    if(pattern)
	Xfree(pattern);

    rotate_font_ptr = (char *)Xmalloc(strlen(rotate_font) + 1);
    if(!rotate_font_ptr)
	return NULL;
    strcpy(rotate_font_ptr, rotate_font);
	
    return rotate_font_ptr;
}

static Bool
is_match_charset(font_data, font_name)
    FontData	font_data;
    char	*font_name;
{
    char *last;
    int length, name_len;

    name_len = strlen(font_name);
    last = font_name + name_len;

    length = strlen(font_data->name);
    if (length > name_len)
	return False;
	
    if (_XlcCompareISOLatin1(last - length, font_data->name) == 0)
	return True;

    return False;
}

static int
parse_all_name(oc, font_data, pattern)
    XOC		oc;
    FontData	font_data;
    char	*pattern;
{

#ifdef OLDCODE
    if(is_match_charset(font_data, pattern) != True)
 	return False;

    font_data->xlfd_name = (char *)Xmalloc(strlen(pattern)+1);
    if(font_data->xlfd_name == NULL)
	return (-1);

    strcpy(font_data->xlfd_name, pattern);
    return True;
#else  /* OLDCODE */
    Display *dpy = oc->core.om->core.display;
    char **fn_list = NULL, *prop_fname = NULL;
    int list_num;
    XFontStruct *fs_list;
    if(is_match_charset(font_data, pattern) != True) {
	if ((fn_list = XListFontsWithInfo(dpy, pattern,
				      MAXFONTS,
				      &list_num, &fs_list))
	    && (prop_fname = get_prop_name(dpy, fs_list))
	    && (is_match_charset(font_data, prop_fname) != True)) {
	    if (fn_list) {
	        XFreeFontInfo(fn_list, fs_list, list_num);
	        fn_list = NULL;
	    }
	    return False;
        }
	font_data->xlfd_name = prop_fname;
        if (fn_list) {
	    XFreeFontInfo(fn_list, fs_list, list_num);
	}
	return True;
    }

    font_data->xlfd_name = (char *)Xmalloc(strlen(pattern)+1);
    if(font_data->xlfd_name == NULL)
	return (-1);

    strcpy(font_data->xlfd_name, pattern);
    return True;
#endif /* OLDCODE */
}

static int
parse_omit_name(oc, font_data, pattern)
    XOC		oc;
    FontData	font_data;
    char	*pattern;
{
    char	*font_name = (char *) NULL;
    char	*last = (char *) NULL;
    char	*base_name;
    char	buf[BUFSIZE];
    int		length = 0;
    int		num_fields;

    if(is_match_charset(font_data, pattern) == True) {
	strcpy(buf, pattern);
	length = strlen(pattern);
	if (font_name = get_font_name(oc, buf)) {
	    font_data->xlfd_name = (char *)Xmalloc(strlen(font_name) + 1);
	    if(font_data->xlfd_name == NULL) {
		Xfree(font_name);
		return (-1);
	    }
	    strcpy(font_data->xlfd_name, font_name);
	    Xfree(font_name);
	    return True;
	}
    }

    strcpy(buf, pattern);
    length = strlen(pattern);
    last = buf + length - 1;

    /* 
     * Plug in the charset/encoding specified in the XLC_LOCALE file
     * into the *right* place in the XLFD name.
     */
    for (num_fields = 0, base_name = buf; *base_name != '\0'; base_name++)
	if (*base_name == '-') num_fields++;

    switch (num_fields) {
    case 12:
	/* this is the best way to have specifed the fontset */
	*++last = '-';
	break;
    case 13:
	/* got the charset, not the encoding, zap the charset */
	last = strrchr (buf, '-');
	num_fields = 12;
	break;
    case 14:
	/* got the charset and the encoding, zap 'em */
	last = strrchr (buf, '-');
	*last = '\0';
	last = strrchr (buf, '-');
	num_fields = 12;
	break;
    default:
	/* punt */
	if (length > 1 && *last == '*' && *(last - 1) == '-') {
	    if (length > 3 && *(last - 2) == '*' && *(last - 3) == '-')
		last -= 3;
	    else
		last--;
	} else {
	    *++last = '-';
	}
	break;
    }
    last++;

    strcpy(last, font_data->name);
    if (font_name = get_font_name(oc, buf)) {
	font_data->xlfd_name = (char *)Xmalloc(strlen(font_name) + 1);
	if(font_data->xlfd_name == NULL) {
	    Xfree(font_name);
	    return (-1);
	}
	strcpy(font_data->xlfd_name, font_name);
	Xfree(font_name);
	return True;
    }

    while (num_fields < 12) {
	*last = '*';
	*(last + 1) = '-';
	strcpy(last + 2, font_data->name);
	num_fields++; 
	last+=2;
	if (font_name = get_font_name(oc, buf)) {
	    font_data->xlfd_name = (char *)Xmalloc(strlen(font_name) + 1);
	    if(font_data->xlfd_name == NULL) {
		Xfree(font_name);
		return (-1);
	    }
	    strcpy(font_data->xlfd_name, font_name);
	    Xfree(font_name);
	    return True;
	}
    }

    return False;
}


typedef enum{C_PRIMARY, C_SUBSTITUTE, C_VMAP, C_VROTATE } ClassType;

static int
parse_fontdata(oc, font_data, font_data_count, name_list, name_list_count,
		class)
    XOC		oc;
    FontData	font_data;
    int		font_data_count;
    char	**name_list;
    int		name_list_count;
    ClassType	class;
{
    char	**cur_name_list = name_list;
    char	*font_name = (char *) NULL;
    char	*pattern = (char *) NULL;
    int		found_num = 0, ret = 0;
    int		fd_count = font_data_count;
    int		count = name_list_count;
    Bool	is_found = False;

    if(name_list == NULL || count <= 0) {
	return False;
    }

    if(font_data == NULL || font_data_count <= 0) {
	return False;
    }

    for ( ; font_data_count-- > 0; font_data++) {
	is_found = False;
	font_name = (char *) NULL;
	count = name_list_count;
	cur_name_list = name_list;
	while (count-- > 0) {
            pattern = *cur_name_list++;
	    if (pattern == NULL || *pattern == '\0')
		continue;


	    /* When the font name is specified a full name. */
	    if (strchr(pattern, '*') == NULL &&
		(font_name = get_font_name(oc, pattern))) {


		ret = parse_all_name(oc, font_data, font_name);
		Xfree(font_name);


		if(ret == -1)
		    return ret;
		else if (ret == True) {
		    found_num++;
		    is_found = True;
		    break;
		} else
		    continue;
	    }

	    /* When the font name is specified a omited name. */
	    ret = parse_omit_name(oc, font_data, pattern);
	    if(ret == -1)
		    return ret;
	    else if (ret == True) {
		    found_num++;
		    is_found = True;
		    break;
	    } else
		    continue;
	}

	switch(class) {
	  case C_PRIMARY:
	    if(is_found != True)
		return False;
		break;
	  case C_SUBSTITUTE:
	  case C_VMAP:
	    if(is_found == True)
		return True;
	    break;
	  case C_VROTATE:
	    if(is_found == True) {
		char	*rotate_name;

		if((rotate_name = get_rotate_fontname(font_data->xlfd_name)) !=
				  NULL) {
		    Xfree(font_data->xlfd_name);
		    font_data->xlfd_name = rotate_name;
		    return True;
		}
		Xfree(font_data->xlfd_name);
		return False;
	    }
	}
    }

    if(class == C_PRIMARY && found_num == fd_count)
	return True;

    return False;
}


static int
parse_vw(oc, font_set, name_list, count)
    XOC		oc;
    FontSet	font_set;
    char	**name_list;
    int		count;
{
    FontData	vmap = font_set->vmap;
    VRotate	vrotate = font_set->vrotate;
    int		vmap_num = font_set->vmap_num;
    int		vrotate_num = font_set->vrotate_num;
    int		ret = 0, i = 0;

    if(vmap_num > 0) {
	if(parse_fontdata(oc, vmap, vmap_num, name_list, count, C_VMAP) == -1)
	    return (-1);
    }

    if(vrotate_num > 0) {
	ret = parse_fontdata(oc, (FontData) vrotate, vrotate_num,
			     name_list, count, C_VROTATE);
	if(ret == -1) {
	    return (-1);
	} else if(ret == False) {
	    CodeRange	code_range;
	    int		num_cr;
	    int		sub_num = font_set->substitute_num;

	    code_range = vrotate[i].code_range;
	    num_cr = vrotate[i].num_cr;
	    for(i = 0 ; i < vrotate_num ; i++) {
		if(vrotate[i].xlfd_name)
		    Xfree(vrotate[i].xlfd_name);
	    }
	    Xfree(vrotate);

	    if(sub_num > 0) {
		vrotate = font_set->vrotate = (VRotate)Xmalloc
						(sizeof(VRotateRec) * sub_num);
		if(font_set->vrotate == (VRotate)NULL)
		    return (-1);

		for(i = 0 ; i < sub_num ; i++) {
		    vrotate[i].charset_name = font_set->substitute[i].name;
		    vrotate[i].side = font_set->substitute[i].side;
		    vrotate[i].code_range = code_range;
		    vrotate[i].num_cr = num_cr;
		}
		vrotate_num = font_set->vrotate_num = sub_num;
	    } else {
		font_set->vrotate = (VRotate)NULL;
	    }

	    ret = parse_fontdata(oc, (FontData) vrotate, vrotate_num,
				 name_list, count, C_VROTATE);
	    if(ret == -1)
		return (-1);
	}
    }

    return True;
}

/* static */ int
parse_fontname(oc)
    XOC oc;
{
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set;
    char *base_name, **name_list;
    int font_set_num = 0;
    int found_num = 0;
    int count = 0;
    int	ret;
    int i;

    name_list = _XParseBaseFontNameList(oc->core.base_name_list, &count);
    if (name_list == NULL)
	return -1;

    font_set = gen->font_set;
    font_set_num = gen->font_set_num;

    for( ; font_set_num-- > 0 ; font_set++) {
	if(font_set->font_name)
	    continue;

	if(font_set->font_data_count > 0) {
	    ret = parse_fontdata(oc, font_set->font_data,
				 font_set->font_data_count,
				 name_list, count, C_PRIMARY);
	    if(ret == -1) {
		goto err;
	    } else if(ret == True) {
		font_set->font_name = (char *)Xmalloc
			(strlen(font_set->font_data->xlfd_name) + 1);
		if(font_set->font_name == (char *) NULL)
		    goto err;
		strcpy(font_set->font_name, font_set->font_data->xlfd_name);
		font_set->side = font_set->font_data->side;

		if(parse_vw(oc, font_set, name_list, count) == -1)
		    goto err;
		found_num++;

	    /* The substitute font is serched, when the primary fonts */
	    /* is not found. */
	    } else {
		/* The primary information is free from FontSet structure */
		font_set->font_data_count = 0;
		if(font_set->font_data) {
		    Xfree(font_set->font_data);
		    font_set->font_data = (FontData) NULL;
		}

		/* A vrotate member is replaced to substitute information */
		/* from primary information. */
		font_set->vrotate_num = 0;
		if(font_set->vrotate) {
		    Xfree(font_set->vrotate);
		    font_set->vrotate = (VRotate) NULL;
		}

		ret = parse_fontdata(oc, font_set->substitute,
				     font_set->substitute_num,
				     name_list, count, C_SUBSTITUTE);
		if(ret == -1) {
		    goto err;
		} else if(ret == True) {
		    for(i=0;i<font_set->substitute_num;i++){
			if(font_set->substitute[i].xlfd_name != NULL){
				break;
			}
		    }
		    font_set->font_name = (char *)Xmalloc
				(strlen(font_set->substitute[i].xlfd_name) + 1);
		    if(font_set->font_name == (char *) NULL)
			goto err;
		    strcpy(font_set->font_name,font_set->substitute[i].xlfd_name);
		    font_set->side = font_set->substitute[i].side;
		    if(parse_vw(oc, font_set, name_list, count) == -1)
			goto err;
		    found_num++;
		}
	    }
	} else if(font_set->substitute_num > 0) {
	    ret = parse_fontdata(oc, font_set->substitute,
				 font_set->substitute_num,
				 name_list, count, C_SUBSTITUTE);
	    if(ret == -1) {
		goto err;
	    } else if(ret == True) {
		for(i=0;i<font_set->substitute_num;i++){
		    if(font_set->substitute[i].xlfd_name != NULL){
			break;
		    }
		}
		font_set->font_name = (char *)Xmalloc
		    	(strlen(font_set->substitute[i].xlfd_name) + 1);
		if(font_set->font_name == (char *) NULL)
		    goto err;
		strcpy(font_set->font_name,font_set->substitute[i].xlfd_name);
		font_set->side = font_set->substitute[i].side;
		if(parse_vw(oc, font_set, name_list, count) == -1)
		    goto err;

		found_num++;
	    }
	}
    }

    base_name = (char *) Xmalloc(strlen(oc->core.base_name_list) + 1);
    if (base_name == NULL)
	goto err;

    strcpy(base_name, oc->core.base_name_list);
    oc->core.base_name_list = base_name;

    XFreeStringList(name_list);		

    return found_num;

err:
    XFreeStringList(name_list);		

    return -1;
}

/* For VW/UDC end*/

static Bool
set_missing_list(oc)
    XOC oc;
{
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set;
    char **charset_list, *charset_buf;
    int	count, length, font_set_num;
    int result = 1;

    font_set = gen->font_set;
    font_set_num = gen->font_set_num;
    count = length = 0;

    for ( ; font_set_num-- > 0; font_set++) {
	if (font_set->info || font_set->font) {
	    continue;
	}
	
	/* Change 1996.01.23 start */
	if(font_set->font_data_count <= 0 ||
	   font_set->font_data == (FontData)NULL) {
	    if(font_set->substitute_num <= 0 ||
	       font_set->substitute == (FontData)NULL) {
		if(font_set->charset_list != NULL){
		 length += 
		  strlen(font_set->charset_list[0]->encoding_name) + 1;
		} else {
		  length += 1;
		}
	    } else {
		length += strlen(font_set->substitute->name) + 1;
	    }
	} else {
	    length += strlen(font_set->font_data->name) + 1;
	}
	/* Change 1996.01.23 end */
	count++;
    }

    if (count < 1) {
	return True;
    }

    charset_list = (char **) Xmalloc(sizeof(char *) * count);
    if (charset_list == NULL) {
	return False;
    }

    charset_buf = (char *) Xmalloc(length);
    if (charset_buf == NULL) {
	Xfree(charset_list);
	return False;
    }

    oc->core.missing_list.charset_list = charset_list;
    oc->core.missing_list.charset_count = count;

    font_set = gen->font_set;
    font_set_num = gen->font_set_num;

    for ( ; font_set_num-- > 0; font_set++) {
	if (font_set->info || font_set->font) {
	    continue;
	}

	/* Change 1996.01.23 start */
	if(font_set->font_data_count <= 0 ||
	   font_set->font_data == (FontData)NULL) {
	    if(font_set->substitute_num <= 0 ||
	       font_set->substitute == (FontData)NULL) {
		if(font_set->charset_list != NULL){
		 strcpy(charset_buf,
			font_set->charset_list[0]->encoding_name);
		} else {
		 strcpy(charset_buf, "");
		}
		result = 0;
	    } else {
		strcpy(charset_buf, font_set->substitute->name);
	    }
	} else {
	    strcpy(charset_buf, font_set->font_data->name);
	}
	/* Change 1996.01.23 end */
	*charset_list++ = charset_buf;
	charset_buf += strlen(charset_buf) + 1;
    } 

    if(result == 0) {
	return(False);
    }

    return True;
}

static Bool
create_fontset(oc)
    XOC oc;
{
    XOMGenericPart *gen = XOM_GENERIC(oc->core.om);
    int found_num;

    if (init_fontset(oc) == False)
        return False;

    found_num = parse_fontname(oc);
    if (found_num <= 0) {
	if (found_num == 0)
	    set_missing_list(oc);
	return False;
    }

    if (gen->on_demand_loading == True) {
	if (load_font_info(oc) == False)
	    return False;
    } else {
	if (load_font(oc) == False)
	    return False;
    }

    if (init_core_part(oc) == False)
	return False;

    if (set_missing_list(oc) == False)
	return False;

    return True;
}

/* For VW/UDC start */
static void
free_fontdataOC(dpy,font_data, font_data_count)
    Display	*dpy;
    FontData	font_data;
    int		font_data_count;
{
    for( ; font_data_count-- ; font_data++) {
	if(font_data->xlfd_name){
	    Xfree(font_data->xlfd_name);
	    font_data->xlfd_name = NULL;
	}
	if(font_data->font){				/* ADD 1996.01.7 */
	    if(font_data->font->fid)			/* Add 1996.01.23 */
		XFreeFont(dpy,font_data->font);		/* ADD 1996.01.7 */
	    else					/* Add 1996.01.23 */
		XFreeFontInfo(NULL, font_data->font, 1);/* Add 1996.01.23 */
	    font_data->font = NULL;
	}
/* XOM to kyoutuu shite shiyou sushiteiru ryouiki
   kokoha free_fontdataOM() de free sareru

	if(font_data->scopes){
	    Xfree(font_data->scopes);
	    font_data->scopes = NULL;
	}
	if(font_data->name){
	    Xfree(font_data->name);
	    font_data->name = NULL;
	}
*/
    }
}
void destroy_fontdata(gen,dpy)
    XOCGenericPart *gen ;
    Display *dpy ;
{
    FontSet	font_set = (FontSet) NULL;
    int		font_set_num = 0;

    if (gen->font_set) {
	font_set = gen->font_set;
	font_set_num = gen->font_set_num;
	for( ; font_set_num-- ; font_set++) {
	    if(font_set->font_data) {
		free_fontdataOC(dpy,
			font_set->font_data, font_set->font_data_count);
		Xfree(font_set->font_data);
		font_set->font_data = NULL;
	    }
	    if(font_set->substitute) {
		free_fontdataOC(dpy,
			font_set->substitute, font_set->substitute_num);
		Xfree(font_set->substitute);
		font_set->substitute = NULL;
	    }
	    if(font_set->vmap) {
		free_fontdataOC(dpy,
			font_set->vmap, font_set->vmap_num);
		Xfree(font_set->vmap);
		font_set->vmap = NULL;
	    }
	    if(font_set->vrotate) {
		free_fontdataOC(dpy,
			(FontData)font_set->vrotate,
			      font_set->vrotate_num);
		Xfree(font_set->vrotate);
		font_set->vrotate = NULL;
	    }
	}
	Xfree(gen->font_set);
	gen->font_set = NULL;
    }
}
/* For VW/UDC end */

static void
destroy_oc(oc)
    XOC oc;
{
    Display *dpy = oc->core.om->core.display;
    XOCGenericPart *gen = XOC_GENERIC(oc);
    XFontStruct **font_list, *font;
    FontSet	font_set = (FontSet) NULL;
    int		font_set_num = 0;
    int		count = 0;

    if (gen->mbs_to_cs)
	_XlcCloseConverter(gen->mbs_to_cs);

    if (gen->wcs_to_cs)
	_XlcCloseConverter(gen->wcs_to_cs);

/* For VW/UDC start */ /* Change 1996.01.8 */
    destroy_fontdata(gen,dpy); 
/*
*/
/* For VW/UDC end */

    if (oc->core.base_name_list)
	Xfree(oc->core.base_name_list);

    if (oc->core.font_info.font_name_list)
	XFreeStringList(oc->core.font_info.font_name_list);

    if (font_list = oc->core.font_info.font_struct_list) {
	Xfree(oc->core.font_info.font_struct_list);
    }

    if (oc->core.missing_list.charset_list)
	XFreeStringList(oc->core.missing_list.charset_list);

#ifdef notdef
    if (oc->core.res_name)
	Xfree(oc->core.res_name);
    if (oc->core.res_class)
	Xfree(oc->core.res_class);
#endif
    
    Xfree(oc);
}

static char *
set_oc_values(oc, args, num_args)
    XOC oc;
    XlcArgList args;
    int num_args;
{
    int i;
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set = gen->font_set;
    char *ret;
    int num = gen->font_set_num;

    if (oc->core.resources == NULL)
	return NULL;

    ret = _XlcSetValues((XPointer) oc, oc->core.resources,
			oc->core.num_resources, args, num_args, XlcSetMask);
    if(ret != NULL){
	return(ret);
    } else {
	for ( ; num-- > 0; font_set++) {
	    if (font_set->font_name == NULL)
	        continue;
	    if (font_set->vpart_initialize != 0)
	        continue;
	    if( oc->core.orientation == XOMOrientation_TTB_RTL ||
		oc->core.orientation == XOMOrientation_TTB_LTR ){
	    	load_fontdata(oc, font_set->vmap, font_set->vmap_num);
		load_fontdata(oc, (FontData) font_set->vrotate,
			    font_set->vrotate_num);
		font_set->vpart_initialize = 1;
	    }
	}
	return(NULL);
    }
}

static char *
get_oc_values(oc, args, num_args)
    XOC oc;
    XlcArgList args;
    int num_args;
{
    if (oc->core.resources == NULL)
	return NULL;

    return _XlcGetValues((XPointer) oc, oc->core.resources,
			 oc->core.num_resources, args, num_args, XlcGetMask);
}

static XOCMethodsRec oc_default_methods = {
    destroy_oc,
    set_oc_values,
    get_oc_values,
    _XmbDefaultTextEscapement,
    _XmbDefaultTextExtents,
    _XmbDefaultTextPerCharExtents,
    _XmbDefaultDrawString,
    _XmbDefaultDrawImageString,
    _XwcDefaultTextEscapement,
    _XwcDefaultTextExtents,
    _XwcDefaultTextPerCharExtents,
    _XwcDefaultDrawString,
    _XwcDefaultDrawImageString
};

static XOCMethodsRec oc_generic_methods = {
    destroy_oc,
    set_oc_values,
    get_oc_values,
    _XmbGenericTextEscapement,
    _XmbGenericTextExtents,
    _XmbGenericTextPerCharExtents,
    _XmbGenericDrawString,
    _XmbGenericDrawImageString,
    _XwcGenericTextEscapement,
    _XwcGenericTextExtents,
    _XwcGenericTextPerCharExtents,
    _XwcGenericDrawString,
    _XwcGenericDrawImageString
};

typedef struct _XOCMethodsListRec {
    char *name;
    XOCMethods methods;
} XOCMethodsListRec, *XOCMethodsList;

static XOCMethodsListRec oc_methods_list[] = {
    { "default", &oc_default_methods },
    { "generic", &oc_generic_methods }
};

static XlcResource oc_resources[] = {
    { XNBaseFontName, NULLQUARK, sizeof(char *),
      XOffsetOf(XOCRec, core.base_name_list), XlcCreateMask | XlcGetMask },
    { XNOMAutomatic, NULLQUARK, sizeof(Bool),
      XOffsetOf(XOCRec, core.om_automatic), XlcGetMask },
    { XNMissingCharSet, NULLQUARK, sizeof(XOMCharSetList),
      XOffsetOf(XOCRec, core.missing_list), XlcGetMask },
    { XNDefaultString, NULLQUARK, sizeof(char *),
      XOffsetOf(XOCRec, core.default_string), XlcGetMask },
    { XNOrientation, NULLQUARK, sizeof(XOrientation),
      XOffsetOf(XOCRec, core.orientation), XlcDefaultMask | XlcSetMask | XlcGetMask },
    { XNResourceName, NULLQUARK, sizeof(char *),
      XOffsetOf(XOCRec, core.res_name), XlcSetMask | XlcGetMask },
    { XNResourceClass, NULLQUARK, sizeof(char *),
      XOffsetOf(XOCRec, core.res_class), XlcSetMask | XlcGetMask },
    { XNFontInfo, NULLQUARK, sizeof(XOMFontInfo),
      XOffsetOf(XOCRec, core.font_info), XlcGetMask }
};

static XOC
create_oc(om, args, num_args)
    XOM om;
    XlcArgList args;
    int num_args;
{
    XOC oc;
    XOMGenericPart *gen = XOM_GENERIC(om);
    XOCMethodsList methods_list = oc_methods_list;
    int count;

    oc = (XOC) Xmalloc(sizeof(XOCGenericRec));
    if (oc == NULL)
	return (XOC) NULL;
    bzero((char *) oc, sizeof(XOCGenericRec));
    
    oc->core.om = om;

    if (oc_resources[0].xrm_name == NULLQUARK)
	_XlcCompileResourceList(oc_resources, XlcNumber(oc_resources));
    
    if (_XlcSetValues((XPointer) oc, oc_resources, XlcNumber(oc_resources),
		      args, num_args, XlcCreateMask | XlcDefaultMask))
	goto err;

    if (oc->core.base_name_list == NULL)
	goto err;

    oc->core.resources = oc_resources;
    oc->core.num_resources = XlcNumber(oc_resources);

    if (create_fontset(oc) == False)
	goto err;

    oc->methods = &oc_generic_methods;

    if (gen->object_name) {
	count = XlcNumber(oc_methods_list);

	for ( ; count-- > 0; methods_list++) {
	    if (!_XlcCompareISOLatin1(gen->object_name, methods_list->name)) {
		oc->methods = methods_list->methods;
		break;
	    }
	}
    }

    return oc;

err:
    destroy_oc(oc);

    return (XOC) NULL;
}

static void
free_fontdataOM(font_data, font_data_count)
    FontData	font_data;
    int		font_data_count;
{
    for( ; font_data_count-- ; font_data++) {
	if(font_data->name){
	    Xfree(font_data->name);
	    font_data->name = NULL;
	}
	if(font_data->scopes){
	    Xfree(font_data->scopes);
	    font_data->scopes = NULL;
	}
    }
}

static Status
close_om(om)
    XOM om;
{
    XOMGenericPart *gen = XOM_GENERIC(om);
    OMData data;
    int count;

    if (data = gen->data) {
	for (count = gen->data_num; count-- > 0; data++) {
	    if (data->charset_list){
		Xfree(data->charset_list);
		data->charset_list = NULL;
	    }
	    /* free font_data for om */
	    if (data->font_data) {
		free_fontdataOM(data->font_data,data->font_data_count);
		Xfree(data->font_data);
		data->font_data = NULL;
	    }
	    /* free substitute for om */
	    if (data->substitute) {
		free_fontdataOM(data->substitute,data->substitute_num);
		Xfree(data->substitute);
		data->substitute = NULL;
	    }
	    /* free vmap for om */
	    if (data->vmap) {
		free_fontdataOM(data->vmap,data->vmap_num);
		Xfree(data->vmap);
		data->vmap = NULL;
	    }
	    /* free vrotate for om */
	    if (data->vrotate) {
		Xfree(data->vrotate);
		data->vrotate = NULL;
	    }
	}
	Xfree(gen->data);
	gen->data = NULL;
    }

    if (gen->object_name){
	Xfree(gen->object_name);
	gen->object_name = NULL;
    }

    if (om->core.res_name){
	Xfree(om->core.res_name);
	om->core.res_name = NULL;
    }
    if (om->core.res_class){
	Xfree(om->core.res_class);
	om->core.res_class = NULL;
    }
    if (om->core.required_charset.charset_list &&
	om->core.required_charset.charset_count > 0){
	XFreeStringList(om->core.required_charset.charset_list);
	om->core.required_charset.charset_list = NULL;
    } else {
	Xfree((char*)om->core.required_charset.charset_list);
	om->core.required_charset.charset_list = NULL;
    }
    if (om->core.orientation_list.orientation){
	Xfree(om->core.orientation_list.orientation);
	om->core.orientation_list.orientation = NULL;
    }

    Xfree(om);

    return 1;
}

static char *
set_om_values(om, args, num_args)
    XOM om;
    XlcArgList args;
    int num_args;
{
    if (om->core.resources == NULL)
	return NULL;

    return _XlcSetValues((XPointer) om, om->core.resources,
			 om->core.num_resources, args, num_args, XlcSetMask);
}

static char *
get_om_values(om, args, num_args)
    XOM om;
    XlcArgList args;
    int num_args;
{
    if (om->core.resources == NULL)
	return NULL;

    return _XlcGetValues((XPointer) om, om->core.resources,
			 om->core.num_resources, args, num_args, XlcGetMask);
}

static XOMMethodsRec methods = {
    close_om,
    set_om_values,
    get_om_values,
    create_oc
};

static XlcResource om_resources[] = {
    { XNRequiredCharSet, NULLQUARK, sizeof(XOMCharSetList),
      XOffsetOf(XOMRec, core.required_charset), XlcGetMask },
    { XNQueryOrientation, NULLQUARK, sizeof(XOMOrientation),
      XOffsetOf(XOMRec, core.orientation_list), XlcGetMask },
    { XNDirectionalDependentDrawing, NULLQUARK, sizeof(Bool),
      XOffsetOf(XOMRec, core.directional_dependent), XlcGetMask },
    { XNContextualDrawing, NULLQUARK, sizeof(Bool),
      XOffsetOf(XOMRec, core.contextual_drawing), XlcGetMask }
};

static XOM
create_om(lcd, dpy, rdb, res_name, res_class)
    XLCd lcd;
    Display *dpy;
    XrmDatabase rdb;
    char *res_name;
    char *res_class;
{
    XOM om;

    om = (XOM) Xmalloc(sizeof(XOMGenericRec));
    if (om == NULL)
	return (XOM) NULL;
    bzero((char *) om, sizeof(XOMGenericRec));
    
    om->methods = &methods;
    om->core.lcd = lcd;
    om->core.display = dpy;
    om->core.rdb = rdb;
    if (res_name) {
	om->core.res_name = (char *) Xmalloc(strlen(res_name) + 1);
	if (om->core.res_name == NULL)
	    goto err;
	strcpy(om->core.res_name, res_name);
    }
    if (res_class) {
	om->core.res_class = (char *) Xmalloc(strlen(res_class) + 1);
	if (om->core.res_class == NULL)
	    goto err;
	strcpy(om->core.res_class, res_class);
    }

    if (om_resources[0].xrm_name == NULLQUARK)
	_XlcCompileResourceList(om_resources, XlcNumber(om_resources));
    
    om->core.resources = om_resources;
    om->core.num_resources = XlcNumber(om_resources);

    return om;

err:
    close_om(om);

    return (XOM) NULL;
}

static OMData
add_data(om)
    XOM om;
{
    XOMGenericPart *gen = XOM_GENERIC(om);
    OMData new;
    int num;

    if (num = gen->data_num)
        new = (OMData) Xrealloc(gen->data, (num + 1) * sizeof(OMDataRec));
    else
        new = (OMData) Xmalloc(sizeof(OMDataRec));

    if (new == NULL)
        return NULL;

    gen->data_num = num + 1;
    gen->data = new;

    new += num;
    bzero((char *) new, sizeof(OMDataRec));

    return new;
}

/* For VW/UDC */

extern FontScope _XlcParse_scopemaps();

FontData
read_EncodingInfo(count,value)
int count;
char **value;
{
    FontData font_data,ret;
    char *buf, *bufptr,*scp;
    FontScope scope;
    int len;
    font_data = (FontData) Xmalloc(sizeof(FontDataRec) * count);
    if (font_data == NULL)
        return NULL;
    bzero((char *) font_data, sizeof(FontDataRec) * count);

    ret = font_data;
    for ( ; count-- > 0; font_data++) {
/*
        strcpy(buf, *value++);
*/
	buf = *value; value++;
        if (bufptr = strchr(buf, ':')){
	    len = (int)(bufptr - buf);
            bufptr++ ;
	}
        font_data->name = (char *) Xmalloc(len + 1);
        if (font_data->name == NULL)
            return NULL;
        strncpy(font_data->name, buf,len);
	font_data->name[len] = 0;
        if (bufptr && _XlcCompareISOLatin1(bufptr, "GL") == 0)
            font_data->side = XlcGL;
        else if (bufptr && _XlcCompareISOLatin1(bufptr, "GR") == 0)
            font_data->side = XlcGR;
        else
            font_data->side = XlcGLGR;

        if (bufptr && (scp = strchr(bufptr, '['))){
            font_data->scopes = _XlcParse_scopemaps(scp,&(font_data->scopes_num));
        }
    }
    return(ret);
}

static CodeRange read_vrotate(count,value,type,vrotate_num)
int count;
char **value;
int *type;
int *vrotate_num;
{
    FontData font_data,ret;
    char *buf, *bufptr,*scp;
    CodeRange   range;
    if(!strcmp(value[0],"all")){
	*type 	     = VROTATE_ALL ;
	*vrotate_num = 0 ;
	return (NULL);
    } else if(*(value[0]) == '['){
	*type 	     = VROTATE_PART ;
        range = (CodeRange) _XlcParse_scopemaps(value[0],vrotate_num);
	return (range);
    } else {
	*type 	     = VROTATE_NONE ;
	*vrotate_num = 0 ;
	return (NULL);
    }
}

static void read_vw(lcd,font_set,num)
XLCd    lcd;
OMData  font_set;
int num;
{
    char **value, buf[BUFSIZ], *bufptr;
    int count,i;

    sprintf(buf, "fs%d.font.vertical_map", num);
    _XlcGetResource(lcd, "XLC_FONTSET", buf, &value, &count);
    if (count > 0){
        _XlcDbg_printValue(buf,value,count);
        font_set->vmap_num = count;
        font_set->vmap = read_EncodingInfo(count,value);
    }

    sprintf(buf, "fs%d.font.vertical_rotate", num);
    _XlcGetResource(lcd, "XLC_FONTSET", buf, &value, &count);
    if (count > 0){
        _XlcDbg_printValue(buf,value,count);
        font_set->vrotate = read_vrotate(count,value,&(font_set->vrotate_type),
				&(font_set->vrotate_num));
    }
}
/* VW/UDC end */
static Bool
init_om(om)
    XOM om;
{
    XLCd lcd = om->core.lcd;
    XOMGenericPart *gen = XOM_GENERIC(om);
    OMData data;
    XlcCharSet *charset_list;
    FontData font_data;
    char **required_list;
    XOrientation *orientation;
    char **value, buf[BUFSIZ], *bufptr;
    int count = 0, num = 0, length = 0;

    _XlcGetResource(lcd, "XLC_FONTSET", "on_demand_loading", &value, &count);
    if (count > 0 && _XlcCompareISOLatin1(*value, "True") == 0)
	gen->on_demand_loading = True;

    _XlcGetResource(lcd, "XLC_FONTSET", "object_name", &value, &count);
    if (count > 0) {
	gen->object_name = (char *) Xmalloc(strlen(*value) + 1);
	if (gen->object_name == NULL)
	    return False;
	strcpy(gen->object_name, *value);
    }

    for (num = 0; ; num++) {

        sprintf(buf, "fs%d.charset.name", num);
        _XlcGetResource(lcd, "XLC_FONTSET", buf, &value, &count);

        if( count < 1){
            sprintf(buf, "fs%d.charset", num);
            _XlcGetResource(lcd, "XLC_FONTSET", buf, &value, &count);
            if (count < 1)
                break;
        }

	data = add_data(om);
	if (data == NULL)
	    return False;
	
	charset_list = (XlcCharSet *) Xmalloc(sizeof(XlcCharSet) * count);
	if (charset_list == NULL)
	    return False;
	data->charset_list = charset_list;
	data->charset_count = count;

	while (count-- > 0){
	    *charset_list++ = _XlcGetCharSet(*value++);
        }
        sprintf(buf, "fs%d.charset.udc_area", num);
        _XlcGetResource(lcd, "XLC_FONTSET", buf, &value, &count);
        if( count > 0){
            UDCArea udc;
            int i,flag = 0;
            udc = (UDCArea)Xmalloc(count * sizeof(UDCAreaRec));
	    if (udc == NULL)
	        return False;
            for(i=0;i<count;i++){
                sscanf(value[i],"\\x%x,\\x%x", &(udc[i].start), &(udc[i].end));
            }
            for(i=0;i<data->charset_count;i++){
		if(data->charset_list[i]->udc_area == NULL){
		    data->charset_list[i]->udc_area     = udc;
		    data->charset_list[i]->udc_area_num = count;
		    flag = 1;
		}
            }
	    if(flag == 0){
		Xfree(udc);
	    }
        }

        sprintf(buf, "fs%d.font.primary", num);
        _XlcGetResource(lcd, "XLC_FONTSET", buf, &value, &count);
        if (count < 1){
            sprintf(buf, "fs%d.font", num);
            _XlcGetResource(lcd, "XLC_FONTSET", buf, &value, &count);
            if (count < 1)
                return False;
        }

	font_data = read_EncodingInfo(count,value);
	if (font_data == NULL)
	    return False;

	data->font_data = font_data;
	data->font_data_count = count;

        sprintf(buf, "fs%d.font.substitute", num);
        _XlcGetResource(lcd, "XLC_FONTSET", buf, &value, &count);
        if (count > 0){
            font_data = read_EncodingInfo(count,value);
            if (font_data == NULL)
	        return False;
            data->substitute      = font_data;
            data->substitute_num = count;
        } else {
            sprintf(buf, "fs%d.font", num);
            _XlcGetResource(lcd, "XLC_FONTSET", buf, &value, &count);
            if (count < 1) {
                data->substitute      = NULL;
                data->substitute_num = 0;
	    } else {
                font_data = read_EncodingInfo(count,value);
                data->substitute      = font_data;
                data->substitute_num = count;
	    }
	}
        read_vw(lcd,data,num);
	length += strlen(data->font_data->name) + 1;
    }

    /* required charset list */
    required_list = (char **) Xmalloc(sizeof(char *) * gen->data_num);
    if (required_list == NULL)
	return False;

    bufptr = (char *) Xmalloc(length);
    if (bufptr == NULL) {
	Xfree(required_list);
	return False;
    }

    om->core.required_charset.charset_list = required_list;
    om->core.required_charset.charset_count = gen->data_num;

    count = gen->data_num;
    data = gen->data;

    for ( ; count-- > 0; data++) {
	strcpy(bufptr, data->font_data->name);
	*required_list++ = bufptr;
	bufptr += strlen(bufptr) + 1;
    }

    /* orientation list */
    orientation = (XOrientation *) Xmalloc(sizeof(XOrientation) * 2);
    if (orientation == NULL)
	return False;

    orientation[0] = XOMOrientation_LTR_TTB;
    orientation[1] = XOMOrientation_TTB_RTL;
    om->core.orientation_list.orientation = orientation;
    om->core.orientation_list.num_orientation = 2;

    /* directional dependent drawing */
    om->core.directional_dependent = False;

    /* contexual drawing */
    om->core.contextual_drawing = False;

    /* context dependent */
    om->core.context_dependent = False;

    return True;
}

XOM
#if NeedFunctionPrototypes
_XomGenericOpenOM(XLCd lcd, Display *dpy, XrmDatabase rdb,
		  _Xconst char *res_name, _Xconst char *res_class)
#else
_XomGenericOpenOM(lcd, dpy, rdb, res_name, res_class)
    XLCd lcd;
    Display *dpy;
    XrmDatabase rdb;
    char *res_name;
    char *res_class;
#endif
{
    XOM om;

    om = create_om(lcd, dpy, rdb, res_name, res_class);
    if (om == NULL)
	return (XOM) NULL;
    
    if (init_om(om) == False)
	goto err;

    return om;

err:
    close_om(om);

    return (XOM) NULL;
}

Bool
_XInitOM(lcd)
    XLCd lcd;
{
    lcd->methods->open_om = _XomGenericOpenOM;

    return True;
}
