/*
 * $XConsortium: xdpyinfo.c /main/34 1995/12/08 12:09:32 dpw $
 * $XFree86: xc/programs/xdpyinfo/xdpyinfo.c,v 3.12 1997/01/18 07:02:33 dawes Exp $
 * 
 * xdpyinfo - print information about X display connecton
 *
 * 
Copyright (c) 1988  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.
 *
 * Author:  Jim Fulton, MIT X Consortium
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifdef MULTIBUFFER
#include <X11/extensions/multibuf.h>
#endif
#include <X11/extensions/XIElib.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/sync.h>
#include <X11/Xproto.h>
#include <X11/extensions/Xdbe.h>
#include <X11/extensions/record.h>
#ifdef MITSHM
#include <X11/extensions/XShm.h>
#endif
#ifdef XKB
#include <X11/extensions/XKB.h>
#include <X11/XKBlib.h>
#endif
#ifdef XF86VIDMODE
#include <X11/extensions/xf86vmode.h>
#include <X11/extensions/xf86vmstr.h>
#endif
#ifdef XFreeXDGA
#include <X11/extensions/xf86dga.h>
#include <X11/extensions/xf86dgastr.h>
#endif
#ifdef XF86MISC
#include <X11/extensions/xf86misc.h>
#include <X11/extensions/xf86mscstr.h>
#endif
#ifdef XINPUT
#include <X11/extensions/XInput.h>
#endif
#include <X11/Xos.h>
#include <stdio.h>

char *ProgramName;
Bool queryExtensions = False;

static int StrCmp(a, b)
    char **a, **b;
{
    return strcmp(*a, *b);
}

void
print_extension_info (dpy)
    Display *dpy;
{
    int n = 0;
    char **extlist = XListExtensions (dpy, &n);

    printf ("number of extensions:    %d\n", n);

    if (extlist) {
	register int i;
	int opcode, event, error;

	qsort(extlist, n, sizeof(char *), StrCmp);
	for (i = 0; i < n; i++) {
	    if (!queryExtensions) {
		printf ("    %s\n", extlist[i]);
		continue;
	    }
	    XQueryExtension(dpy, extlist[i], &opcode, &event, &error);
	    printf ("    %s  (opcode: %d", extlist[i], opcode);
	    if (event)
		printf (", base event: %d", event);
	    if (error)
		printf (", base error: %d", error);
	    printf(")\n");
	}
	/* do not free, Xlib can depend on contents being unaltered */
	/* XFreeExtensionList (extlist); */
    }
}

void
print_display_info (dpy)
    Display *dpy;
{
    char dummybuf[40];
    char *cp;
    int minkeycode, maxkeycode;
    int i, n;
    long req_size;
    XPixmapFormatValues *pmf;
    Window focuswin;
    int focusrevert;

    printf ("name of display:    %s\n", DisplayString (dpy));
    printf ("version number:    %d.%d\n",
	    ProtocolVersion (dpy), ProtocolRevision (dpy));
    printf ("vendor string:    %s\n", ServerVendor (dpy));
    printf ("vendor release number:    %d\n", VendorRelease (dpy));
    req_size = XExtendedMaxRequestSize (dpy);
    if (!req_size) req_size = XMaxRequestSize (dpy);
    printf ("maximum request size:  %ld bytes\n", req_size * 4);
    printf ("motion buffer size:  %d\n", XDisplayMotionBufferSize (dpy));

    switch (BitmapBitOrder (dpy)) {
      case LSBFirst:    cp = "LSBFirst"; break;
      case MSBFirst:    cp = "MSBFirst"; break;
      default:    
	sprintf (dummybuf, "unknown order %d", BitmapBitOrder (dpy));
	cp = dummybuf;
	break;
    }
    printf ("bitmap unit, bit order, padding:    %d, %s, %d\n",
	    BitmapUnit (dpy), cp, BitmapPad (dpy));

    switch (ImageByteOrder (dpy)) {
      case LSBFirst:    cp = "LSBFirst"; break;
      case MSBFirst:    cp = "MSBFirst"; break;
      default:    
	sprintf (dummybuf, "unknown order %d", ImageByteOrder (dpy));
	cp = dummybuf;
	break;
    }
    printf ("image byte order:    %s\n", cp);

    pmf = XListPixmapFormats (dpy, &n);
    printf ("number of supported pixmap formats:    %d\n", n);
    if (pmf) {
	printf ("supported pixmap formats:\n");
	for (i = 0; i < n; i++) {
	    printf ("    depth %d, bits_per_pixel %d, scanline_pad %d\n",
		    pmf[i].depth, pmf[i].bits_per_pixel, pmf[i].scanline_pad);
	}
	XFree ((char *) pmf);
    }


    /*
     * when we get interfaces to the PixmapFormat stuff, insert code here
     */

    XDisplayKeycodes (dpy, &minkeycode, &maxkeycode);
    printf ("keycode range:    minimum %d, maximum %d\n",
	    minkeycode, maxkeycode);

    XGetInputFocus (dpy, &focuswin, &focusrevert);
    printf ("focus:  ");
    switch (focuswin) {
      case PointerRoot:
	printf ("PointerRoot\n");
	break;
      case None:
	printf ("None\n");
	break;
      default:
	printf("window 0x%lx, revert to ", focuswin);
	switch (focusrevert) {
	  case RevertToParent:
	    printf ("Parent\n");
	    break;
	  case RevertToNone:
	    printf ("None\n");
	    break;
	  case RevertToPointerRoot:
	    printf ("PointerRoot\n");
	    break;
	  default:			/* should not happen */
	    printf ("%d\n", focusrevert);
	    break;
	}
	break;
    }

    print_extension_info (dpy);

    printf ("default screen number:    %d\n", DefaultScreen (dpy));
    printf ("number of screens:    %d\n", ScreenCount (dpy));
}

void
print_visual_info (vip)
    XVisualInfo *vip;
{
    char errorbuf[40];			/* for sprintfing into */
    char *class = NULL;			/* for printing */

    switch (vip->class) {
      case StaticGray:    class = "StaticGray"; break;
      case GrayScale:    class = "GrayScale"; break;
      case StaticColor:    class = "StaticColor"; break;
      case PseudoColor:    class = "PseudoColor"; break;
      case TrueColor:    class = "TrueColor"; break;
      case DirectColor:    class = "DirectColor"; break;
      default:    
	sprintf (errorbuf, "unknown class %d", vip->class);
	class = errorbuf;
	break;
    }

    printf ("  visual:\n");
    printf ("    visual id:    0x%lx\n", vip->visualid);
    printf ("    class:    %s\n", class);
    printf ("    depth:    %d plane%s\n", vip->depth, 
	    vip->depth == 1 ? "" : "s");
    if (vip->class == TrueColor || vip->class == DirectColor)
	printf ("    available colormap entries:    %d per subfield\n",
		vip->colormap_size);
    else
	printf ("    available colormap entries:    %d\n",
		vip->colormap_size);
    printf ("    red, green, blue masks:    0x%lx, 0x%lx, 0x%lx\n",
	    vip->red_mask, vip->green_mask, vip->blue_mask);
    printf ("    significant bits in color specification:    %d bits\n",
	    vip->bits_per_rgb);
}

void
print_screen_info (dpy, scr)
    Display *dpy;
    int scr;
{
    Screen *s = ScreenOfDisplay (dpy, scr);  /* opaque structure */
    XVisualInfo viproto;		/* fill in for getting info */
    XVisualInfo *vip;			/* retured info */
    int nvi;				/* number of elements returned */
    int i;				/* temp variable: iterator */
    char eventbuf[80];			/* want 79 chars per line + nul */
    static char *yes = "YES", *no = "NO", *when = "WHEN MAPPED";
    double xres, yres;
    int ndepths = 0, *depths = NULL;
    unsigned int width, height;


    /*
     * there are 2.54 centimeters to an inch; so there are 25.4 millimeters.
     *
     *     dpi = N pixels / (M millimeters / (25.4 millimeters / 1 inch))
     *         = N pixels / (M inch / 25.4)
     *         = N * 25.4 pixels / M inch
     */

    xres = ((((double) DisplayWidth(dpy,scr)) * 25.4) / 
	    ((double) DisplayWidthMM(dpy,scr)));
    yres = ((((double) DisplayHeight(dpy,scr)) * 25.4) / 
	    ((double) DisplayHeightMM(dpy,scr)));

    printf ("\n");
    printf ("screen #%d:\n", scr);
    printf ("  dimensions:    %dx%d pixels (%dx%d millimeters)\n",
	    DisplayWidth (dpy, scr), DisplayHeight (dpy, scr),
	    DisplayWidthMM(dpy, scr), DisplayHeightMM (dpy, scr));
    printf ("  resolution:    %dx%d dots per inch\n", 
	    (int) (xres + 0.5), (int) (yres + 0.5));
    depths = XListDepths (dpy, scr, &ndepths);
    if (!depths) ndepths = 0;
    printf ("  depths (%d):    ", ndepths);
    for (i = 0; i < ndepths; i++) {
	printf ("%d", depths[i]);
	if (i < ndepths - 1) { 
	    putchar (',');
	    putchar (' ');
	}
    }
    putchar ('\n');
    if (depths) XFree ((char *) depths);
    printf ("  root window id:    0x%lx\n", RootWindow (dpy, scr));
    printf ("  depth of root window:    %d plane%s\n",
	    DisplayPlanes (dpy, scr),
	    DisplayPlanes (dpy, scr) == 1 ? "" : "s");
    printf ("  number of colormaps:    minimum %d, maximum %d\n",
	    MinCmapsOfScreen(s), MaxCmapsOfScreen(s));
    printf ("  default colormap:    0x%lx\n", DefaultColormap (dpy, scr));
    printf ("  default number of colormap cells:    %d\n",
	    DisplayCells (dpy, scr));
    printf ("  preallocated pixels:    black %d, white %d\n",
	    BlackPixel (dpy, scr), WhitePixel (dpy, scr));
    printf ("  options:    backing-store %s, save-unders %s\n",
	    (DoesBackingStore (s) == NotUseful) ? no :
	    ((DoesBackingStore (s) == Always) ? yes : when),
	    DoesSaveUnders (s) ? yes : no);
    XQueryBestSize (dpy, CursorShape, RootWindow (dpy, scr), 65535, 65535,
		    &width, &height);
    if (width == 65535 && height == 65535)
	printf ("  largest cursor:    unlimited\n");
    else
	printf ("  largest cursor:    %dx%d\n", width, height);
    printf ("  current input event mask:    0x%lx\n", EventMaskOfScreen (s));
    (void) print_event_mask (eventbuf, 79, 4, EventMaskOfScreen (s));
		      

    nvi = 0;
    viproto.screen = scr;
    vip = XGetVisualInfo (dpy, VisualScreenMask, &viproto, &nvi);
    printf ("  number of visuals:    %d\n", nvi);
    printf ("  default visual id:  0x%lx\n", 
	    XVisualIDFromVisual (DefaultVisual (dpy, scr)));
    for (i = 0; i < nvi; i++) {
	print_visual_info (vip+i);
    }
    if (vip) XFree ((char *) vip);
}

/*
 * The following routine prints out an event mask, wrapping events at nice
 * boundaries.
 */

#define MASK_NAME_WIDTH 25

static struct _event_table {
    char *name;
    long value;
} event_table[] = {
    { "KeyPressMask             ", KeyPressMask },
    { "KeyReleaseMask           ", KeyReleaseMask },
    { "ButtonPressMask          ", ButtonPressMask },
    { "ButtonReleaseMask        ", ButtonReleaseMask },
    { "EnterWindowMask          ", EnterWindowMask },
    { "LeaveWindowMask          ", LeaveWindowMask },
    { "PointerMotionMask        ", PointerMotionMask },
    { "PointerMotionHintMask    ", PointerMotionHintMask },
    { "Button1MotionMask        ", Button1MotionMask },
    { "Button2MotionMask        ", Button2MotionMask },
    { "Button3MotionMask        ", Button3MotionMask },
    { "Button4MotionMask        ", Button4MotionMask },
    { "Button5MotionMask        ", Button5MotionMask },
    { "ButtonMotionMask         ", ButtonMotionMask },
    { "KeymapStateMask          ", KeymapStateMask },
    { "ExposureMask             ", ExposureMask },
    { "VisibilityChangeMask     ", VisibilityChangeMask },
    { "StructureNotifyMask      ", StructureNotifyMask },
    { "ResizeRedirectMask       ", ResizeRedirectMask },
    { "SubstructureNotifyMask   ", SubstructureNotifyMask },
    { "SubstructureRedirectMask ", SubstructureRedirectMask },
    { "FocusChangeMask          ", FocusChangeMask },
    { "PropertyChangeMask       ", PropertyChangeMask },
    { "ColormapChangeMask       ", ColormapChangeMask },
    { "OwnerGrabButtonMask      ", OwnerGrabButtonMask },
    { NULL, 0 }};

int print_event_mask (buf, lastcol, indent, mask)
    char *buf;				/* string to write into */
    int lastcol;			/* strlen(buf)+1 */
    int indent;				/* amount by which to indent */
    long mask;				/* event mask */
{
    struct _event_table *etp;
    int len;
    int bitsfound = 0;

    buf[0] = buf[lastcol] = '\0';	/* just in case */

#define INDENT() { register int i; len = indent; \
		   for (i = 0; i < indent; i++) buf[i] = ' '; }

    INDENT ();

    for (etp = event_table; etp->name; etp++) {
	if (mask & etp->value) {
	    if (len + MASK_NAME_WIDTH > lastcol) {
		puts (buf);
		INDENT ();
	    }
	    strcpy (buf+len, etp->name);
	    len += MASK_NAME_WIDTH;
	    bitsfound++;
	}
    }

    if (bitsfound) puts (buf);

#undef INDENT

    return (bitsfound);
}

void
print_standard_extension_info(dpy, extname, majorrev, minorrev)
    Display *dpy;
    char *extname;
    int majorrev, minorrev;
{
    int opcode, event, error;

    printf("%s version %d.%d ", extname, majorrev, minorrev);

    XQueryExtension(dpy, extname, &opcode, &event, &error);
    printf ("opcode: %d", opcode);
    if (event)
	printf (", base event: %d", event);
    if (error)
	printf (", base error: %d", error);
    printf("\n");
}

#ifdef MULTIBUFFER
int
print_multibuf_info(dpy, extname)
    Display *dpy;
    char *extname;
{
    int i, j;			/* temp variable: iterator */
    int nmono, nstereo;		/* count */
    XmbufBufferInfo *mono_info = NULL, *stereo_info = NULL; /* arrays */
    static char *fmt = 
	"    visual id, max buffers, depth:    0x%lx, %d, %d\n";
    int scr = 0;
    int majorrev, minorrev;

    if (!XmbufGetVersion(dpy, &majorrev, &minorrev))
	return 0;

    print_standard_extension_info(dpy, extname, majorrev, minorrev);

    for (i = 0; i < ScreenCount (dpy); i++)
    {
	if (!XmbufGetScreenInfo (dpy, RootWindow(dpy, scr), &nmono, &mono_info,
				 &nstereo, &stereo_info)) {
	    fprintf (stderr,
		     "%s:  unable to get multibuffer info for screen %d\n",
		     ProgramName, scr);
	} else {
	    printf ("  screen %d number of mono multibuffer types:    %d\n", i, nmono);
	    for (j = 0; j < nmono; j++) {
		printf (fmt, mono_info[j].visualid, mono_info[j].max_buffers,
			mono_info[j].depth);
	    }
	    printf ("  number of stereo multibuffer types:    %d\n", nstereo);
	    for (j = 0; j < nstereo; j++) {
		printf (fmt, stereo_info[j].visualid,
			stereo_info[j].max_buffers, stereo_info[j].depth);
	    }
	    if (mono_info) XFree ((char *) mono_info);
	    if (stereo_info) XFree ((char *) stereo_info);
	}
    }
    return 1;
} /* end print_multibuf_info */
#endif


/* XIE stuff */

char *subset_names[] = { NULL, "FULL", "DIS" };
char *align_names[] = { NULL, "Alignable", "Arbitrary" };
char *group_names[] = { /* 0  */ "Default",
			    /* 2  */ "ColorAlloc",
			    /* 4  */ "Constrain",
			    /* 6  */ "ConvertFromRGB",
			    /* 8  */ "ConvertToRGB",
			    /* 10 */ "Convolve",
			    /* 12 */ "Decode",
			    /* 14 */ "Dither",
			    /* 16 */ "Encode",
			    /* 18 */ "Gamut",
			    /* 20 */ "Geometry",
			    /* 22 */ "Histogram",
			    /* 24 */ "WhiteAdjust"
			    };

int
print_xie_info(dpy, extname)
    Display *dpy;
    char *extname;
{
    XieExtensionInfo *xieInfo;
    int i;
    int ntechs;
    XieTechnique *techs;
    XieTechniqueGroup prevGroup;

    if (!XieInitialize(dpy, &xieInfo ))
	return 0;

    print_standard_extension_info(dpy, extname,
	xieInfo->server_major_rev, xieInfo->server_minor_rev);

    printf("  service class: %s\n", subset_names[xieInfo->service_class]);
    printf("  alignment: %s\n", align_names[xieInfo->alignment]);
    printf("  uncnst_mantissa: %d\n", xieInfo->uncnst_mantissa);
    printf("  uncnst_min_exp: %d\n", xieInfo->uncnst_min_exp);
    printf("  uncnst_max_exp: %d\n", xieInfo->uncnst_max_exp);
    printf("  cnst_levels:"); 
    for (i = 0; i < xieInfo->n_cnst_levels; i++)
	printf(" %d", xieInfo->cnst_levels[i]);
    printf("\n");

    if (!XieQueryTechniques(dpy, xieValAll, &ntechs, &techs))
	return 1;

    prevGroup = -1;

    for (i = 0; i < ntechs; i++)
    {
	if (techs[i].group != prevGroup)
	{
	    printf("  technique group: %s\n", group_names[techs[i].group >> 1]);
	    prevGroup = techs[i].group;
	}
	printf("    %s\tspeed: %d  needs_param: %s  number: %d\n",
	       techs[i].name,
	       techs[i].speed, (techs[i].needs_param ? "True " : "False"),
	       techs[i].number);
    }
    return 1;
} /* end print_xie_info */

int
print_xtest_info(dpy, extname)
    Display *dpy;
    char *extname;
{
    int majorrev, minorrev, foo;

    if (!XTestQueryExtension(dpy, &foo, &foo, &majorrev, &minorrev))
	return 0;
    print_standard_extension_info(dpy, extname, majorrev, minorrev);
    return 1;
}

int
print_sync_info(dpy, extname)
    Display *dpy;
    char *extname;
{
    int majorrev, minorrev;
    XSyncSystemCounter *syscounters;
    int ncounters, i;

    if (!XSyncInitialize(dpy, &majorrev, &minorrev))
	return 0;
    print_standard_extension_info(dpy, extname, majorrev, minorrev);

    syscounters = XSyncListSystemCounters(dpy, &ncounters);
    printf("  system counters: %d\n", ncounters);
    for (i = 0; i < ncounters; i++)
    {
	printf("    %s  id: 0x%08x  resolution_lo: %d  resolution_hi: %d\n",
	       syscounters[i].name, syscounters[i].counter,
	       XSyncValueLow32(syscounters[i].resolution),
	       XSyncValueHigh32(syscounters[i].resolution));
    }
    XSyncFreeSystemCounterList(syscounters);
    return 1;
}

int
print_shape_info(dpy, extname)
    Display *dpy;
    char *extname;
{
    int majorrev, minorrev;

    if (!XShapeQueryVersion(dpy, &majorrev, &minorrev))
	return 0;
    print_standard_extension_info(dpy, extname, majorrev, minorrev);
    return 1;
}

#ifdef XFreeXDGA
print_dga_info(dpy, extname)
    Display *dpy;
    char *extname;
{
    int majorrev, minorrev, width, bank, ram, offset;

    if (!XF86DGAQueryVersion(dpy, &majorrev, &minorrev))
	return 0;
    print_standard_extension_info(dpy, extname, majorrev, minorrev);

    if (!XF86DGAGetVideoLL(dpy, DefaultScreen(dpy), &offset,
			    &width, &bank, &ram))
	return 0;
    printf("  Base address = 0x%X, Width = %d, Bank size = %d,"
	   " RAM size = %dk\n", offset, width, bank, ram);

    return 1;
}
#endif

#ifdef XF86VIDMODE
#define V_PHSYNC        0x001 
#define V_NHSYNC        0x002
#define V_PVSYNC        0x004
#define V_NVSYNC        0x008
#define V_INTERLACE     0x010 
#define V_DBLSCAN       0x020
#define V_CSYNC         0x040
#define V_PCSYNC        0x080
#define V_NCSYNC        0x100

print_XF86VidMode_info(dpy, extname)
    Display *dpy;
    char *extname;
{
    int majorrev, minorrev, modecount, i;
    XF86VidModeMonitor monitor;
    XF86VidModeModeInfo **modelines;

    if (!XF86VidModeQueryVersion(dpy, &majorrev, &minorrev))
	return 0;
    print_standard_extension_info(dpy, extname, majorrev, minorrev);

    if (!XF86VidModeGetMonitor(dpy, DefaultScreen(dpy), &monitor))
	return 0;
    printf("  Monitor Information:\n");
    printf("    Vendor: %s, Model: %s\n", monitor.vendor, monitor.model);
    printf("    Num hsync: %d, Num vsync: %d\n", monitor.nhsync, monitor.nvsync);
    for (i = 0; i < monitor.nhsync; i++) {
        printf("    hsync range %d: %6.2f - %6.2f\n", i, monitor.hsync[i].lo,
               monitor.hsync[i].hi);
    }
    for (i = 0; i < monitor.nvsync; i++) {
        printf("    vsync range %d: %6.2f - %6.2f\n", i, monitor.vsync[i].lo,
               monitor.vsync[i].hi);
    }

    if ((majorrev > 0) || (majorrev == 0 && minorrev > 5)) {
      if (!XF86VidModeGetAllModeLines(dpy, DefaultScreen(dpy), &modecount,
				      &modelines))
	return 0;
      printf("  Available Video Mode Settings (current first):\n");
      printf("     Clock   Hdsp Hbeg Hend Httl   Vdsp Vbeg Vend Vttl  Flags\n");
      for (i = 0; i < modecount; i++) {
        printf("    %6.2f   %4d %4d %4d %4d   %4d %4d %4d %4d ",
            (float)modelines[i]->dotclock/1000.0,
            modelines[i]->hdisplay, modelines[i]->hsyncstart,
            modelines[i]->hsyncend, modelines[i]->htotal,
            modelines[i]->vdisplay, modelines[i]->vsyncstart,
            modelines[i]->vsyncend, modelines[i]->vtotal);
        if (modelines[i]->flags & V_PHSYNC)    printf(" +hsync");
        if (modelines[i]->flags & V_NHSYNC)    printf(" -hsync");
        if (modelines[i]->flags & V_PVSYNC)    printf(" +vsync");
        if (modelines[i]->flags & V_NVSYNC)    printf(" -vsync");
        if (modelines[i]->flags & V_INTERLACE) printf(" interlace");
        if (modelines[i]->flags & V_CSYNC)     printf(" composite");
        if (modelines[i]->flags & V_PCSYNC)    printf(" +csync");
        if (modelines[i]->flags & V_PCSYNC)    printf(" -csync");
        if (modelines[i]->flags & V_DBLSCAN)   printf(" doublescan");
        printf("\n");
      }
    }

    return 1;
}
#endif

#ifdef XF86MISC

char *kbdtable[] = { "Unknown", "84-key", "101-key", "Other", "Xqueue" };
char *msetable[] = { "None", "Microsoft", "MouseSystems", "MMSeries",
		     "Logitech", "BusMouse", "Mouseman", "PS/2", "MMHitTab",
		     "GlidePoint", "Unknown", "Xqueue", "OSMouse" };
char *flgtable[] = { "None", "ClearDTR", "ClearRTS",
		     "ClearDTR and ClearRTS" };

print_XF86Misc_info(dpy, extname)
    Display *dpy;
    char *extname;
{
    int majorrev, minorrev;
    XF86MiscMouseSettings mouseinfo;
    XF86MiscKbdSettings kbdinfo;

    if (!XF86MiscQueryVersion(dpy, &majorrev, &minorrev))
	return 0;
    print_standard_extension_info(dpy, extname, majorrev, minorrev);

    if ((majorrev > 0) || (majorrev == 0 && minorrev > 0)) {
      if (!XF86MiscGetKbdSettings(dpy, &kbdinfo))
	return 0;
      printf("  Keyboard Settings-    Type: %s, Rate: %d, Delay: %d, ServerNumLock: %s\n",
	kbdtable[kbdinfo.type], kbdinfo.rate, kbdinfo.delay,
	(kbdinfo.servnumlock? "yes": "no"));

      if (!XF86MiscGetMouseSettings(dpy, &mouseinfo))
	return 0;
      printf("  Mouse Settings-       Device: %s, Type: %s\n",
	strlen(mouseinfo.device) == 0 ? "None": mouseinfo.device,
	msetable[mouseinfo.type+1]);
      printf("                        BaudRate: %d, SampleRate: %d\n",
	mouseinfo.baudrate, mouseinfo.samplerate);
      printf("                        Emulate3Buttons: %s, Emulate3Timeout: %d ms\n",
	mouseinfo.emulate3buttons? "yes": "no", mouseinfo.emulate3timeout);
      printf("                        ChordMiddle: %s, Flags: %s\n",
	mouseinfo.chordmiddle? "yes": "no",
	flgtable[(mouseinfo.flags & MF_CLEAR_DTR? 1: 0)
		+(mouseinfo.flags & MF_CLEAR_RTS? 1: 0)] );
    }

    return 1;
}
#endif

#ifdef MITSHM
print_mitshm_info(dpy, extname)
    Display *dpy;
    char *extname;
{
    int majorrev, minorrev;
    Bool sharedPixmaps;

    if (!XShmQueryVersion(dpy, &majorrev, &minorrev, &sharedPixmaps))
	return 0;
    print_standard_extension_info(dpy, extname, majorrev, minorrev);
    printf("  shared pixmaps: ");
    if (sharedPixmaps)
    {
	int format = XShmPixmapFormat(dpy);
	printf("yes, format: %d\n", format);
    }
    else
    {
	printf("no\n");
    }
    return 1;
}
#endif /* MITSHM */

#ifdef XKB
print_xkb_info(dpy, extname)
    Display *dpy;
    char *extname;
{
    int opcode, eventbase, errorbase, majorrev, minorrev;

    if (!XkbQueryExtension(dpy, &opcode, &eventbase, &errorbase,
			   &majorrev, &minorrev)) {
        return 0;
    }
    printf("%s version %d.%d ", extname, majorrev, minorrev);

    printf ("opcode: %d", opcode);
    if (eventbase)
	printf (", base event: %d", eventbase);
    if (errorbase)
	printf (", base error: %d", errorbase);
    printf("\n");

    return 1;
}
#endif

int
print_dbe_info(dpy, extname)
    Display *dpy;
    char *extname;
{
    int majorrev, minorrev;
    XdbeScreenVisualInfo *svi;
    int numscreens = 0;
    int iscrn, ivis;

    if (!XdbeQueryExtension(dpy, &majorrev, &minorrev))
	return 0;

    print_standard_extension_info(dpy, extname, majorrev, minorrev);
    svi = XdbeGetVisualInfo(dpy, (Drawable *)NULL, &numscreens);
    for (iscrn = 0; iscrn < numscreens; iscrn++)
    {
	printf("  Double-buffered visuals on screen %d\n", iscrn);
	for (ivis = 0; ivis < svi[iscrn].count; ivis++)
	{
	    printf("    visual id 0x%lx  depth %d  perflevel %d\n",
		   svi[iscrn].visinfo[ivis].visual,
		   svi[iscrn].visinfo[ivis].depth,
		   svi[iscrn].visinfo[ivis].perflevel);
	}
    }
    XdbeFreeVisualInfo(svi);
    return 1;
}

int
print_record_info(dpy, extname)
    Display *dpy;
    char *extname;
{
    int majorrev, minorrev;

    if (!XRecordQueryVersion(dpy, &majorrev, &minorrev))
	return 0;
    print_standard_extension_info(dpy, extname, majorrev, minorrev);
    return 1;
}

#ifdef XINPUT
int
print_xinput_info(dpy, extname)
    Display *dpy;
    char *extname;
{
  int           loop, num_extensions, num_devices;
  char          **extensions;
  XDeviceInfo   *devices;
  int		list = 0;
  XExtensionVersion *ext;

  ext = XGetExtensionVersion(dpy, extname);
  
  if (!ext || (ext == (XExtensionVersion*) NoSuchExtension))
      return 0;

  print_standard_extension_info(dpy, extname, ext->major_version,
				ext->minor_version);

  extensions = XListExtensions(dpy, &num_extensions);
  for (loop = 0; loop < num_extensions &&
         (strcmp(extensions[loop], extname) != 0); loop++);
  XFreeExtensionList(extensions);
  if (loop != num_extensions) {
      printf("  Extended devices :\n");
      devices = XListInputDevices(dpy, &num_devices);
      for(loop=0; loop<num_devices; loop++) {
	  printf("	\"%s\"	[", devices[loop].name ? devices[loop].name : "<noname>");
	  switch(devices[loop].use) {
	  case IsXPointer:
	      printf("XPointer]\n");
	      break;
	  case IsXKeyboard:
	      printf("XKeyboard]\n");
	      break;
	  case IsXExtensionDevice:
	      printf("XExtensionDevice]\n");
	      break;
	  default:
	      printf("invalid value]\n");
	      break;
	  }
        }
      XFreeDeviceList(devices);
      return 1;
    }
  else
      return 0;
}
#endif

/* utilities to manage the list of recognized extensions */


typedef int (*ExtensionPrintFunc)(
#if NeedFunctionPrototypes
    Display *, char *
#endif
);

typedef struct {
    char *extname;
    ExtensionPrintFunc printfunc;
    Bool printit;
} ExtensionPrintInfo;

ExtensionPrintInfo known_extensions[] =
{
#ifdef MITSHM
    {"MIT-SHM",	print_mitshm_info, False},
#endif /* MITSHM */
#ifdef XKB
    {XkbName, print_xkb_info, False},
#endif /* XKB */
#ifdef MULTIBUFFER
    {MULTIBUFFER_PROTOCOL_NAME,	print_multibuf_info, False},
#endif
    {"SHAPE", print_shape_info, False},
    {SYNC_NAME, print_sync_info, False},
#ifdef XFreeXDGA
    {XF86DGANAME, print_dga_info, False},
#endif /* XFreeXDGA */
#ifdef XF86VIDMODE
    {XF86VIDMODENAME, print_XF86VidMode_info, False},
#endif /* XF86VIDMODE */
#ifdef XF86MISC
    {XF86MISCNAME, print_XF86Misc_info, False},
#endif /* XF86MISC */
    {xieExtName, print_xie_info, False},
    {XTestExtensionName, print_xtest_info, False},
    {"DOUBLE-BUFFER", print_dbe_info, False},
    {"RECORD", print_record_info, False},
#ifdef XINPUT
    {INAME, print_xinput_info, False}
#endif
    /* add new extensions here */
    /* wish list: PEX */
};

int num_known_extensions = sizeof known_extensions / sizeof known_extensions[0];

void
print_known_extensions(f)
    FILE *f;
{
    int i, col;
    for (i = 0, col = 6; i < num_known_extensions; i++)
    {
	if ((col += strlen(known_extensions[i].extname)+1) > 79)
	{
		col = 6;
		fprintf(f, "\n     ");
	}
	fprintf(f, "%s ", known_extensions[i].extname);
    }
}

void
mark_extension_for_printing(extname)
    char *extname;
{
    int i;

    if (strcmp(extname, "all") == 0)
    {
	for (i = 0; i < num_known_extensions; i++)
	    known_extensions[i].printit = True;
    }
    else
    {
	for (i = 0; i < num_known_extensions; i++)
	{
	    if (strcmp(extname, known_extensions[i].extname) == 0)
	    {
		known_extensions[i].printit = True;
		return;
	    }
	}
	printf("%s extension not supported by %s\n", extname, ProgramName);
    }
}

void
print_marked_extensions(dpy)
    Display *dpy;
{
    int i;
    for (i = 0; i < num_known_extensions; i++)
    {
	if (known_extensions[i].printit)
	{
	    printf("\n");
	    if (! (*known_extensions[i].printfunc)(dpy,
					known_extensions[i].extname))
	    {
		printf("%s extension not supported by server\n",
		       known_extensions[i].extname);
	    }
	}
    }
}

static void usage ()
{
    fprintf (stderr, "usage:  %s [options]\n", ProgramName);
    fprintf (stderr, "-display displayname\tserver to query\n");
    fprintf (stderr, "-queryExtensions\tprint info returned by XQueryExtension\n");
    fprintf (stderr, "-ext all\t\tprint detailed info for all supported extensions\n");
    fprintf (stderr, "-ext extension-name\tprint detailed info for extension-name if one of:\n     ");
    print_known_extensions(stderr);
    fprintf (stderr, "\n");
    exit (1);
}

int main (argc, argv)
    int argc;
    char *argv[];
{
    Display *dpy;			/* X connection */
    char *displayname = NULL;		/* server to contact */
    int i;				/* temp variable:  iterator */
    int mbuf_event_base, mbuf_error_base;

    ProgramName = argv[0];

    for (i = 1; i < argc; i++) {
	char *arg = argv[i];
	int len = strlen(arg);
	
	if (!strncmp("-display", arg, len)) {
	    if (++i >= argc) usage ();
	    displayname = argv[i];
	} else if (!strncmp("-queryExtensions", arg, len)) {
	    queryExtensions = True;
	} else if (!strncmp("-ext", arg, len)) {
	    if (++i >= argc) usage ();
	    mark_extension_for_printing(argv[i]);
	} else
	    usage ();
    }

    dpy = XOpenDisplay (displayname);
    if (!dpy) {
	fprintf (stderr, "%s:  unable to open display \"%s\".\n",
		 ProgramName, XDisplayName (displayname));
	exit (1);
    }

    print_display_info (dpy);
    for (i = 0; i < ScreenCount (dpy); i++) {
	print_screen_info (dpy, i);
    }

    print_marked_extensions(dpy);

    XCloseDisplay (dpy);
    exit (0);
}
