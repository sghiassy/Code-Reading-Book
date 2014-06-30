/* $XConsortium: tclxfconf.c /main/3 1996/10/23 11:44:07 kaleb $ */






/* $XFree86: xc/programs/Xserver/hw/xfree86/XF86Setup/tclxfconf.c,v 3.15.2.3 1997/06/01 12:33:25 dawes Exp $ */
/*
 * Copyright 1996 by Joseph V. Moss <joe@XFree86.Org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Joseph Moss not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Joseph Moss makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * JOSEPH MOSS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL JOSEPH MOSS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */


/*

  This file contains Tcl bindings to the XF86Config file reading routines

 */

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "servermd.h"
#include "scrnintstr.h"

#define INIT_OPTIONS
#include "xf86_Option.h"

#define NO_COMPILER_H_EXTRAS
#include "xf86Procs.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"

#include "tcl.h"
#define XF86SETUP_NO_FUNC_RENAME
#include "xfsconf.h"

#if NeedVarargsPrototypes
#include <stdarg.h>
#endif

int TCL_XF86FindXF86Config(
#if NeedNestedPrototypes
    ClientData	clientData,
    Tcl_Interp	*interp,
    int		argc,
    char	**argv
#endif
);

int TCL_XF86ReadXF86Config(
#if NeedNestedPrototypes
    ClientData	clientData,
    Tcl_Interp	*interp,
    int		argc,
    char	**argv
#endif
);

static void init_config_vars(
#if NeedNestedPrototypes
    char *path
#endif
);

static int getsection_files(
#if NeedNestedPrototypes
    Tcl_Interp *interp,
    char *varname
#endif
);

static int getsection_server(
#if NeedNestedPrototypes
    Tcl_Interp *interp,
    char *varname
#endif
);

static int getsection_keyboard(
#if NeedNestedPrototypes
    Tcl_Interp *interp,
    char *varname
#endif
);

static int getsection_pointer(
#if NeedNestedPrototypes
    Tcl_Interp *interp,
    char *varname
#endif
);

static int getsection_monitor(
#if NeedNestedPrototypes
    Tcl_Interp *interp,
    char *varname
#endif
);

static int getsection_device(
#if NeedNestedPrototypes
    Tcl_Interp *interp,
    char *varname
#endif
);

static int getsection_screen(
#if NeedNestedPrototypes
    Tcl_Interp *interp,
    char *varname
#endif
);

static char *NonZeroStr(
#if NeedNestedPrototypes
    unsigned long val,
    int base
#endif
);

Tcl_Interp *errinterp;

/* Error handling functions */

#if NeedVarargsPrototypes
void
VErrorF(f, args)
    char *f;
    va_list args;
{
    char tmpbuf[1024];
    vsprintf(tmpbuf, f, args);
    Tcl_AppendResult(errinterp, tmpbuf, (char *) NULL);
}
#endif

/*VARARGS1*/
void
ErrorF(
#if NeedVarargsPrototypes
    char * f, ...)
#else
 f, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9) /* limit of ten args */
    char *f;
    char *s0, *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8, *s9;
#endif
{
#if NeedVarargsPrototypes
    va_list args;
    va_start(args, f);
    VErrorF(f, args);
    va_end(args);
#else
    char tmpbuf[1024];
#ifdef AMOEBA
    mu_lock(&print_lock);
#endif
    sprintf( tmpbuf, f, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9);
    Tcl_AppendResult(errinterp, tmpbuf, (char *) NULL);
#ifdef AMOEBA
    mu_unlock(&print_lock);
#endif
#endif
}

/*VARARGS1*/
int
XF86SetupFatalError(
#if NeedVarargsPrototypes
    char *f, ...)
#else
f, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9) /* limit of ten args */
    char *f;
    char *s0, *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8, *s9;
#endif
{
#if NeedVarargsPrototypes
    va_list args;
#endif
    ErrorF("\nFatal server error:\n");
#if NeedVarargsPrototypes
    va_start(args, f);
    VErrorF(f, args);
    va_end(args);
#else
    ErrorF(f, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9);
#endif
    ErrorF("\n");
    return TCL_ERROR;
}

/* Configuration variables */
xf86InfoRec	xf86Info;
double		xf86rGamma, xf86gGamma, xf86bGamma;
char		xf86ConfigFile[PATH_MAX];
CARD32		defaultScreenSaverTime, ScreenSaverTime;
Bool		xf86fpFlag, xf86coFlag, xf86sFlag;
char		*rgbPath = NULL, *defaultFontPath = NULL;
Bool		xf86ScreensOpen;
Bool		xf86BestRefresh;
extern int	n_devices, n_monitors;
extern GDevPtr	device_list;
extern MonPtr	monitor_list;

ScrnInfoRec	monoInfoRec, vga2InfoRec, vga16InfoRec,
		svgaInfoRec, accelInfoRec;

Bool		xf86AllowMouseOpenFail;
int		xf86bpp, defaultColorVisualClass, xf86Verbose = 2;
xrgb		xf86weight;
PciProbeType	xf86PCIFlags;

/* Note that the order is important. They must be ordered numerically */
int		xf86MaxScreens, xf86ScreenNames[] = {
					SVGA, VGA2, MONO,
					VGA16, ACCEL, -1
				};
ScrnInfoPtr	xf86Screens[] = {
			&svgaInfoRec, &vga2InfoRec, &monoInfoRec,
			&vga16InfoRec, &accelInfoRec
		};

#ifdef XKB
Bool		noXkbExtension;
char		*XkbInitialMap;
#endif
#ifdef DPMSExtension
CARD32 defaultDPMSStandbyTime;
CARD32 defaultDPMSSuspendTime;
CARD32 defaultDPMSOffTime;
CARD32 DPMSStandbyTime;
CARD32 DPMSSuspendTime;
CARD32 DPMSOffTime;
#endif
#ifdef XF86VIDMODE
Bool		xf86VidModeEnabled;
Bool		xf86VidModeAllowNonLocal;
#endif
#ifdef XF86MISC
Bool		xf86MiscModInDevEnabled;
Bool		xf86MiscModInDevAllowNonLocal;
#endif

#define XF86MAXSCREENS	sizeof(xf86Screens) / sizeof(ScrnInfoPtr)

/*
  Set the various config file related variables to their defaults
*/

static void
init_config_vars(xwinhome)
  char *xwinhome;
{
	int i, pathlen;

	xf86Verbose = 2;
	xf86ConfigFile[0] = '\0';
	defaultScreenSaverTime  = ScreenSaverTime = 0;
	defaultColorVisualClass = xf86bpp         = -1;
	xf86AllowMouseOpenFail  = xf86ScreensOpen = FALSE;
	xf86fpFlag = xf86coFlag = xf86sFlag       = FALSE;
	xf86BestRefresh = FALSE;
	xf86MaxScreens = XF86MAXSCREENS;
	xf86weight.red = xf86weight.green = xf86weight.blue = 0;

	pathlen = strlen(xwinhome);
	if (rgbPath == NULL)
		rgbPath = XtMalloc(pathlen + 13);
	else
		rgbPath = XtRealloc(rgbPath, pathlen + 13);
	strcpy(rgbPath, xwinhome);
	strcat(rgbPath, "/lib/X11/rgb");

	if (defaultFontPath == NULL)
		defaultFontPath = XtMalloc(pathlen*2 + 42);
	else
		defaultFontPath = XtRealloc(defaultFontPath, pathlen*2 + 42);
	sprintf(defaultFontPath,
		"%s/lib/X11/fonts/misc,%s/lib/X11/fonts/75dpi",
		xwinhome, xwinhome);
#ifdef XKB
	noXkbExtension = FALSE;
	XkbInitialMap = (char *)NULL;
#endif
#ifdef XF86VIDMODE
	xf86VidModeEnabled = TRUE;
	xf86VidModeAllowNonLocal = FALSE;
#endif
#ifdef XF86MISC
	xf86MiscModInDevEnabled = TRUE;
	xf86MiscModInDevAllowNonLocal = FALSE;
#endif

	bzero(&xf86Info, sizeof(xf86InfoRec));

	xf86rGamma = xf86gGamma = xf86bGamma = 1.0;

	for (i = 0; i < XF86MAXSCREENS; i++) {
		bzero(xf86Screens[i], sizeof(ScrnInfoRec));
		xf86Screens[i]->configured	= FALSE;
		xf86Screens[i]->tmpIndex	= -1;
		xf86Screens[i]->scrnIndex	= -1;
		xf86Screens[i]->BIOSbase	= 0xC0000;
	}
	monoInfoRec.depth	= 1;
	vga2InfoRec.depth	= 1;
	vga16InfoRec.depth	= 4;
	svgaInfoRec.depth	= 8;
	accelInfoRec.depth	= 8;

	vga16InfoRec.weight.red		= 5;
	svgaInfoRec.weight.red		= 5;
	accelInfoRec.weight.red		= 5;
	vga16InfoRec.weight.green	= 6;
	svgaInfoRec.weight.green	= 6;
	accelInfoRec.weight.green	= 6;
	vga16InfoRec.weight.blue	= 5;
	svgaInfoRec.weight.blue		= 5;
	accelInfoRec.weight.blue	= 5;

	monoInfoRec.bitsPerPixel	= 1;
	vga2InfoRec.bitsPerPixel	= 1;
	vga16InfoRec.bitsPerPixel	= 8;
	svgaInfoRec.bitsPerPixel	= 8;
	accelInfoRec.bitsPerPixel	= 8;

	monoInfoRec.defaultVisual	= StaticGray;
	vga2InfoRec.defaultVisual	= StaticGray;
	vga16InfoRec.defaultVisual	= PseudoColor;
	svgaInfoRec.defaultVisual	= PseudoColor;
	accelInfoRec.defaultVisual	= PseudoColor;

	monoInfoRec.name	= "MONO";
	vga2InfoRec.name	= "VGA2";
	vga16InfoRec.name	= "VGA16";
	svgaInfoRec.name	= "SVGA";
	accelInfoRec.name	= "Accel";

}


/* Stub functions */
int
xf86KbdProc(pKeyboard, what)
  DeviceIntPtr pKeyboard;
  int	what;
{
  return 0;
}
int
xf86MseProc(pPointer, what)
  DeviceIntPtr pPointer;
  int	what;
{
  return 0;
}
void
xf86KbdEvents()
{
}
void
xf86MseEvents(mouse)
  MouseDevPtr mouse;
{
}
#ifdef XQUEUE
int
xf86XqueKbdProc(pKeyboard, what)
  DeviceIntPtr pKeyboard;
  int	what;
{
  return 0;
}
int
xf86XqueMseProc(pPointer, what)
  DeviceIntPtr pPointer;
  int	what;
{
  return 0;
}
void
xf86XqueEvents()
{
}
#endif
#if defined(USE_OSMOUSE) || defined(OSMOUSE_ONLY)
int
xf86OsMouseProc(pPointer, what)
  DeviceIntPtr pPointer;
  int	what;
{
  return 0;
}
void
xf86OsMouseEvents()
{
}
void
xf86OsMouseOption(lt,lp)
  int lt;
  pointer lp;
{
}
#endif
#ifdef XINPUT
int xf86ConfigExtendedInputSection(LexPtr val)
{
  return TCL_OK;
}
#endif

int
xf86MouseSupported(mousetype)
  int mousetype;
{
  return(TRUE);
}

char *KeyMapType(key)
  int key;
{
	switch(xf86Info.specialKeyMap[key-LEFTALT]) {
		case KM_META:		return "Meta";
		case KM_COMPOSE:	return "Compose";
		case KM_MODESHIFT:	return "ModeShift";
		case KM_MODELOCK:	return "ModeLock";
		case KM_SCROLLLOCK:	return "ScrollLock";
		case KM_CONTROL:	return "Control";
	}
	return "";
}

/*
   Adds all the Cards database specific commands to the Tcl interpreter
*/

int
XF86Config_Init(interp)
    Tcl_Interp	*interp;
{
	Tcl_CreateCommand(interp, "xf86config_findfile",
		TCL_XF86FindXF86Config, (ClientData) NULL,
		(void (*)()) NULL);

	Tcl_CreateCommand(interp, "xf86config_readfile",
		TCL_XF86ReadXF86Config, (ClientData) NULL,
		(void (*)()) NULL);

	return TCL_OK;
}

/*
  Locate the XF86Config that would be used by the server
*/

int
TCL_XF86FindXF86Config(clientData, interp, argc, argv)
    ClientData	clientData;
    Tcl_Interp	*interp;
    int		argc;
    char	*argv[];
{
	int	retval;
	char	*tmpbuf;
	FILE	*fp;

	if (argc != 1) {
		Tcl_SetResult(interp, "Usage: xf86config_findfile", TCL_STATIC);
		return TCL_ERROR;
	}

	errinterp = interp;
	tmpbuf = XtMalloc(PATH_MAX);
	fp = NULL;
	if ((retval = findConfigFile(tmpbuf, &fp)) != TCL_OK)
		Tcl_SetResult(interp, "", TCL_STATIC);
	else {
		fclose(fp);
		Tcl_SetResult(interp, tmpbuf, TCL_VOLATILE);
	}
	XtFree(tmpbuf);
	return TCL_OK;
}

/*
   Implements the xf86config_readfile command which reads
   in the XF86Config file and set the values from it
*/

static char readconfig_usage[] = "Usage: xf86config_readfile " \
	"<xwinhome> <file_varname> <server_varname> " \
	"<kbd_varname> <pointer_varname> <monitor_varname> " \
	"<device_varname> <scrn_varname>";

#define StrOrNull(xx)	((xx)==NULL? "": (xx))
static char *msetypes[] = { "None", "Microsoft", "MouseSystems", "MMSeries",
		"Logitech", "BusMouse", "Mouseman", "PS/2", "MMHitTab",
		"GlidePoint", "IntelliMouse", "Unknown", "Xqueue", "OSMouse" };


int
TCL_XF86ReadXF86Config(clientData, interp, argc, argv)
    ClientData	clientData;
    Tcl_Interp	*interp;
    int		argc;
    char	*argv[];
{
	int retval;

	if (argc != 9) {
		Tcl_SetResult(interp, readconfig_usage, TCL_STATIC);
		return TCL_ERROR;
	}

	errinterp = interp;
	init_config_vars(argv[1]);
	if ((retval = xf86Config(0)) != TCL_OK)
		return retval;
	getsection_files   (interp, argv[2]);
	getsection_server  (interp, argv[3]);
	getsection_keyboard(interp, argv[4]);
	getsection_pointer (interp, argv[5]);
	getsection_monitor (interp, argv[6]);
	getsection_device  (interp, argv[7]);
	getsection_screen  (interp, argv[8]);
#ifdef NOT_YET
#ifdef XINPUT
	getsection_xinput  (interp, argv[9]);
#endif
	getsection_module  (interp, argv[10]);
#endif
	XtFree((char *) monitor_list);
	XtFree((char *) device_list);
	return TCL_OK;
}

/*
  Set the Tcl variables for the config from the Files section
*/

static int
getsection_files(interp, varname)
  Tcl_Interp *interp;
  char *varname;
{
	Tcl_SetVar2(interp, "files", "FontPath",
		StrOrNull(defaultFontPath), 0);
	if (defaultFontPath)
		XtFree(defaultFontPath);
	Tcl_SetVar2(interp, "files", "RGBPath", StrOrNull(rgbPath), 0);
	if (rgbPath)
		XtFree(rgbPath);
	rgbPath = defaultFontPath = NULL;
	return TCL_OK;
}

/*
  Set the Tcl variables for the config from the ServerFlags section
*/

static int
getsection_server(interp, varname)
  Tcl_Interp *interp;
  char *varname;
{
	Tcl_SetVar2(interp, "server", "NoTrapSignals",
		xf86Info.notrapSignals? "NoTrapSignals": "", 0);
	Tcl_SetVar2(interp, "server", "DontZap",
		xf86Info.dontZap? "DontZap": "", 0);
	Tcl_SetVar2(interp, "server", "DontZoom",
		xf86Info.dontZoom? "DontZoom": "", 0);
#ifdef XF86VIDMODE
	Tcl_SetVar2(interp, "server", "DisableVidModeExtension",
		xf86VidModeEnabled? "": "DisableVidModeExtension", 0);
	Tcl_SetVar2(interp, "server", "AllowNonLocalXvidtune",
		xf86VidModeAllowNonLocal? "AllowNonLocalXvidtune": "", 0);
#endif
#ifdef XF86MISC
	Tcl_SetVar2(interp, "server", "DisableModInDev",
		xf86MiscModInDevEnabled? "": "DisableModInDev", 0);
	Tcl_SetVar2(interp, "server", "AllowNonLocalModInDev",
		xf86MiscModInDevAllowNonLocal?
			"AllowNonLocalModInDev": "", 0);
#endif
	return TCL_OK;
}

/*
  Set the Tcl variables for the config from the Keyboard section
*/

static int
getsection_keyboard(interp, varname)
  Tcl_Interp *interp;
  char *varname;
{
	char	tmpbuf[128], tmpbuf2[16];
	int	i;

#ifdef XQUEUE
	if (xf86Info.kbdProc == xf86XqueKbdProc)
		Tcl_SetVar2(interp, "keyboard", "Protocol", "Xqueue", 0);
#endif
	if (xf86Info.kbdProc == xf86KbdProc)
		Tcl_SetVar2(interp, "keyboard", "Protocol", "Standard", 0);
	Tcl_SetVar2(interp, "keyboard", "ServerNumLock",
		xf86Info.serverNumLock? "ServerNumLock": "", 0);

	sprintf(tmpbuf, "%d %d", xf86Info.kbdDelay, xf86Info.kbdRate);
	Tcl_SetVar2(interp, "keyboard", "AutoRepeat", tmpbuf, 0);

	Tcl_SetVar2(interp, "keyboard", "LeftAlt", KeyMapType(LEFTALT), 0);
	Tcl_SetVar2(interp, "keyboard", "RightAlt", KeyMapType(RIGHTALT), 0);
	Tcl_SetVar2(interp, "keyboard", "ScrollLock", KeyMapType(SCROLLLOCK), 0);
	Tcl_SetVar2(interp, "keyboard", "RightCtl", KeyMapType(RIGHTCTL), 0);
	tmpbuf[0] = '\0';
	for (i = 1; i < 8; i++) {
		if (xf86Info.xleds & (1L << (i-1))) {
			sprintf(tmpbuf2, "%d ", i);
			strcat(tmpbuf, tmpbuf2);
		}
	}
	Tcl_SetVar2(interp, "keyboard", "XLeds", tmpbuf, 0);
	Tcl_SetVar2(interp, "keyboard", "VTSysReq",
		xf86Info.vtSysreq? "VTSysReq": "", 0);
	Tcl_SetVar2(interp, "keyboard", "VTInit",
		StrOrNull(xf86Info.vtinit), 0);
#ifdef XKB
	Tcl_SetVar2(interp, "keyboard", "XkbDisable",
		noXkbExtension? "XkbDisable": "", 0);
	Tcl_SetVar2(interp, "keyboard", "XkbKeycodes",
		StrOrNull(xf86Info.xkbkeycodes), 0);
	Tcl_SetVar2(interp, "keyboard", "XkbTypes",
		StrOrNull(xf86Info.xkbtypes), 0);
	Tcl_SetVar2(interp, "keyboard", "XkbCompat",
		StrOrNull(xf86Info.xkbcompat), 0);
	Tcl_SetVar2(interp, "keyboard", "XkbSymbols",
		StrOrNull(xf86Info.xkbsymbols), 0);
	Tcl_SetVar2(interp, "keyboard", "XkbGeometry",
		StrOrNull(xf86Info.xkbgeometry), 0);
	Tcl_SetVar2(interp, "keyboard", "XkbKeymap",
		StrOrNull(xf86Info.xkbkeymap), 0);
	Tcl_SetVar2(interp, "keyboard", "XkbRules",
		StrOrNull(xf86Info.xkbrules), 0);
	Tcl_SetVar2(interp, "keyboard", "XkbModel",
		StrOrNull(xf86Info.xkbmodel), 0);
	Tcl_SetVar2(interp, "keyboard", "XkbLayout",
		StrOrNull(xf86Info.xkblayout), 0);
	Tcl_SetVar2(interp, "keyboard", "XkbVariant",
		StrOrNull(xf86Info.xkbvariant), 0);
	Tcl_SetVar2(interp, "keyboard", "XkbOptions",
		StrOrNull(xf86Info.xkboptions), 0);
#endif
	return TCL_OK;
}

/*
  Set the Tcl variables for the config from the Pointer section
*/

static int
getsection_pointer(interp, varname)
  Tcl_Interp *interp;
  char *varname;
{
	char tmpbuf[16];

#ifdef XQUEUE
	if (xf86Info.mouseDev->mseProc == xf86XqueMseProc)
		xf86Info.mouseDev->mseType = 11;
#endif
#if defined(USE_OSMOUSE) || defined(OSMOUSE_ONLY)
	if (xf86Info.mouseDev->mseProc == xf86OsMouseProc)
		xf86Info.mouseDev->mseType = 12;
#endif
	Tcl_SetVar2(interp, "mouse", "Protocol",
		msetypes[xf86Info.mouseDev->mseType+1], 0);
	Tcl_SetVar2(interp, "mouse", "Device",
		StrOrNull(xf86Info.mouseDev->mseDevice), 0);
	sprintf(tmpbuf, "%d", xf86Info.mouseDev->baudRate);
	Tcl_SetVar2(interp, "mouse", "BaudRate", tmpbuf, 0);
	sprintf(tmpbuf, "%d", xf86Info.mouseDev->sampleRate);
	Tcl_SetVar2(interp, "mouse", "SampleRate", tmpbuf, 0);
	Tcl_SetVar2(interp, "mouse", "Emulate3Buttons",
		xf86Info.mouseDev->emulate3Buttons? "Emulate3Buttons": "", 0);
	sprintf(tmpbuf, "%d", xf86Info.mouseDev->emulate3Timeout);
	Tcl_SetVar2(interp, "mouse", "Emulate3Timeout", tmpbuf, 0);
	Tcl_SetVar2(interp, "mouse", "ChordMiddle",
		xf86Info.mouseDev->chordMiddle? "ChordMiddle": "", 0);
	Tcl_SetVar2(interp, "mouse", "ClearDTR",
		xf86Info.mouseDev->mouseFlags&MF_CLEAR_DTR? "ClearDTR": "", 0);
	Tcl_SetVar2(interp, "mouse", "ClearRTS",
		xf86Info.mouseDev->mouseFlags&MF_CLEAR_RTS? "ClearRTS": "", 0);
	return TCL_OK;
}

/*
  Set the Tcl variables for the config from the Monitor sections
*/

static int
getsection_monitor(interp, varname)
  Tcl_Interp *interp;
  char *varname;
{
	int		i, j;
	char		*namebuf, *tmpptr, *tmpbuf;
	MonPtr	 	mptr;
	DisplayModePtr	dptr;

	for (i = 0; i < n_monitors; i++) {
		mptr = &monitor_list[i];
		namebuf = (char *) XtMalloc(strlen(varname)+strlen(mptr->id)+2);
		sprintf(namebuf, "%s_%s", varname, mptr->id);

		Tcl_SetVar2(interp, namebuf, "VendorName",
			StrOrNull(mptr->vendor), 0);
		Tcl_SetVar2(interp, namebuf, "ModelName",
			StrOrNull(mptr->model), 0);

		tmpptr = tmpbuf = XtMalloc(mptr->n_hsync*14);
		for (j = 0; j < mptr->n_hsync; j++) {
		    sprintf(tmpptr, "%s%.5g-%.5g", (j? ",": ""),
			mptr->hsync[j].lo, mptr->hsync[j].hi);
		    tmpptr += strlen(tmpptr);
		}
		Tcl_SetVar2(interp, namebuf, "HorizSync", tmpbuf, 0);

		tmpptr = tmpbuf = XtRealloc(tmpbuf, mptr->n_vrefresh*14);
		for (j = 0; j < mptr->n_vrefresh; j++) {
		    sprintf(tmpptr, "%s%.5g-%.5g", (j? ",": ""),
			mptr->vrefresh[j].lo, mptr->vrefresh[j].hi);
		    tmpptr += strlen(tmpptr);
		}
		Tcl_SetVar2(interp, namebuf, "VertRefresh", tmpbuf, 0);

		tmpbuf = XtRealloc(tmpbuf, 256);
		if ((xf86rGamma == xf86gGamma)
				&& (xf86rGamma == xf86bGamma)) {
		    if (xf86rGamma == 1.0)
			Tcl_SetVar2(interp, namebuf, "Gamma", "", 0);
		    else {
			sprintf(tmpbuf, "%.3g", 1.0/xf86rGamma);
			Tcl_SetVar2(interp, namebuf, "Gamma", tmpbuf, 0);
		    }
		} else {
		    sprintf(tmpbuf, "%.3g %.3g %.3g",
			    1.0/xf86rGamma, 1.0/xf86gGamma, 1.0/xf86bGamma);
		    Tcl_SetVar2(interp, namebuf, "Gamma", tmpbuf, 0);
		}
		Tcl_SetVar2(interp, namebuf, "Modelines", "", 0);
		for (dptr = mptr->Modes; ; dptr = dptr->next) {
		    sprintf(tmpbuf, "\"%s\" %.4g %d %d %d %d %d %d %d %d",
			dptr->name, dptr->Clock/1000.0,
			dptr->HDisplay, dptr->HSyncStart,
			dptr->HSyncEnd, dptr->HTotal,
			dptr->VDisplay, dptr->VSyncStart,
			dptr->VSyncEnd, dptr->VTotal);
		    if (dptr->Flags&V_INTERLACE) strcat(tmpbuf, " Interlace");
		    if (dptr->Flags & V_PHSYNC)  strcat(tmpbuf, " +HSync");
		    if (dptr->Flags & V_NHSYNC)  strcat(tmpbuf, " -HSync");
		    if (dptr->Flags & V_PVSYNC)  strcat(tmpbuf, " +VSync");
		    if (dptr->Flags & V_NVSYNC)  strcat(tmpbuf, " -VSync");
		    if (dptr->Flags & V_CSYNC)   strcat(tmpbuf, " Composite");
		    if (dptr->Flags & V_PCSYNC)  strcat(tmpbuf, " +CSync");
		    if (dptr->Flags & V_NCSYNC)  strcat(tmpbuf, " -CSync");
		    if (dptr->Flags & V_DBLSCAN) strcat(tmpbuf, " DoubleScan");
		    if (dptr->Flags & V_HSKEW) {
			char buf[10];
		        sprintf(buf, " HSkew %d", dptr->HSkew);
		        strcat(tmpbuf, buf);
		    }
		    Tcl_SetVar2(interp, namebuf, "Modelines", tmpbuf,
		        TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
		    if (dptr == mptr->Last || dptr->next == NULL)
			break;
		}
		XtFree(tmpbuf);
		XtFree(namebuf);
	}
	return TCL_OK;
}

/*
  Set the Tcl variables for the config from the Device sections
*/

static int
getsection_device(interp, varname)
  Tcl_Interp *interp;
  char *varname;
{
	int	i, j;
	char	*namebuf, tmpbuf[128];
	GDevPtr	dptr;

	for (i = 0; i < n_devices; i++) {
		dptr = &device_list[i];
		namebuf = XtMalloc(strlen(varname)+strlen(dptr->identifier)+2);
		sprintf(namebuf, "%s_%s", varname, dptr->identifier);
		Tcl_SetVar2(interp, namebuf, "VendorName",
			StrOrNull(dptr->vendor), 0);
		Tcl_SetVar2(interp, namebuf, "BoardName",
			StrOrNull(dptr->board), 0);
		Tcl_SetVar2(interp, namebuf, "Chipset",
			StrOrNull(dptr->chipset), 0);
		Tcl_SetVar2(interp, namebuf, "Ramdac",
			StrOrNull(dptr->ramdac), 0);
		Tcl_SetVar2(interp, namebuf, "DacSpeed",
			NonZeroStr(dptr->dacSpeeds[0]/1000,10), 0);
		if (dptr->dacSpeeds[0]/1000 > 0)
		   for (j = 1; j < MAXDACSPEEDS; j++) {
		      if (dptr->dacSpeeds[j]/1000 <= 0)
			 break;
		      sprintf(tmpbuf, " %d", dptr->dacSpeeds[j]/1000);
		      Tcl_SetVar2(interp, namebuf, "DacSpeed",
				  tmpbuf, TCL_APPEND_VALUE);
		   }
		Tcl_SetVar2(interp, namebuf, "Clocks", "", 0);
		for (j = 0; j < dptr->clocks; j++) {
			sprintf(tmpbuf, "%.5g ", dptr->clock[j]/1000.0);
			Tcl_SetVar2(interp, namebuf, "Clocks",
				tmpbuf, TCL_APPEND_VALUE);
		}
		Tcl_SetVar2(interp, namebuf, "ClockProg",
			StrOrNull(dptr->clockprog), 0);
		if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &(dptr->clockOptions))) {
		    j = 0;
		    while (xf86_ClockOptionTab[j].token != -1) {
			if (OFLG_ISSET(xf86_ClockOptionTab[j].token,
					&(dptr->clockOptions))) {
			    Tcl_SetVar2(interp, namebuf, "ClockChip",
				xf86_ClockOptionTab[j].name, 0);
			    break;
			}
			j++;
		    }
		    if (xf86_ClockOptionTab[j].token == -1)
			/* this shouldn't happen */
			Tcl_SetVar2(interp, namebuf, "ClockChip", "", 0);
		} else
			Tcl_SetVar2(interp, namebuf, "ClockChip", "", 0);
		if (OFLG_ISSET(XCONFIG_SPEEDUP, &(dptr->xconfigFlag))) {
		    if (dptr->speedup == 0)
			Tcl_SetVar2(interp, namebuf, "SpeedUp", "none", 0);
		    else if (dptr->speedup == SPEEDUP_BEST)
			Tcl_SetVar2(interp, namebuf, "SpeedUp", "best", 0);
		    else if (dptr->speedup == SPEEDUP_ALL)
			Tcl_SetVar2(interp, namebuf, "SpeedUp", "all", 0);
		    else {
			sprintf(tmpbuf, "%ld", dptr->speedup);
			Tcl_SetVar2(interp, namebuf, "SpeedUp", tmpbuf, 0);
		    }
		} else
			Tcl_SetVar2(interp, namebuf, "SpeedUp", "", 0);

		Tcl_SetVar2(interp, namebuf, "VideoRam",
			NonZeroStr(dptr->videoRam,10), 0);
		Tcl_SetVar2(interp, namebuf, "BIOSBase",
			NonZeroStr(dptr->BIOSbase,16), 0);
		Tcl_SetVar2(interp, namebuf, "Membase",
			NonZeroStr(dptr->MemBase,16), 0);
		Tcl_SetVar2(interp, namebuf, "IOBase",
			NonZeroStr(dptr->IObase,16), 0);
		Tcl_SetVar2(interp, namebuf, "DACBase",
			NonZeroStr(dptr->DACbase,16), 0);
		Tcl_SetVar2(interp, namebuf, "POSBase",
			NonZeroStr(dptr->POSbase,16), 0);
		Tcl_SetVar2(interp, namebuf, "COPBase",
			NonZeroStr(dptr->COPbase,16), 0);
		Tcl_SetVar2(interp, namebuf, "VGABase",
			NonZeroStr(dptr->VGAbase,16), 0);
		Tcl_SetVar2(interp, namebuf, "Instance",
			NonZeroStr(dptr->instance,10), 0);
		if (dptr->s3Madjust || dptr->s3Nadjust) {
			sprintf(tmpbuf, "%d %d",
				dptr->s3Madjust, dptr->s3Nadjust);
			Tcl_SetVar2(interp, namebuf, "S3MNAdjust",
				tmpbuf, 0);
		} else
			Tcl_SetVar2(interp, namebuf, "S3MNAdjust", "", 0);
		sprintf(tmpbuf, "%.5g", dptr->s3MClk/1000.0);
		if (dptr->s3MClk == 0)
			tmpbuf[0] = '\0';
		Tcl_SetVar2(interp, namebuf, "S3MClk", tmpbuf, 0);
		sprintf(tmpbuf, "%.5g", dptr->s3RefClk/1000.0);
		if (dptr->s3RefClk == 0)
			tmpbuf[0] = '\0';
		Tcl_SetVar2(interp, namebuf, "S3RefClock", tmpbuf, 0);
		Tcl_UnsetVar2(interp, namebuf, "Option", 0);
		for (j = 0; xf86_OptionTab[j].token != -1; j++)
		    if (OFLG_ISSET(xf86_OptionTab[j].token,
				&(dptr->options))) {
			Tcl_SetVar2(interp, namebuf, "Option",
			    xf86_OptionTab[j].name,
			    TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
		    }
		if (Tcl_GetVar2(interp, namebuf, "Option", 0) == NULL)
			Tcl_SetVar2(interp, namebuf, "Option", "", 0);
		XtFree(namebuf);
	}
	return TCL_OK;
}

/*
  Convert the given value to a string representation in the specified
  base.  If the value is zero, return an empty string
*/

static char *NonZeroStr(val, base)
  unsigned long val;
  int base;
{
	static char tmpbuf[16];

	if (val) {
		if (base == 16)
			sprintf(tmpbuf, "%#lx", val);
		else
			sprintf(tmpbuf, "%ld", val);
		return tmpbuf;
	} else
		return "";
}

/*
  Set the Tcl variables for the config from the Screen sections
*/

extern int	xf86setup_scrn_ndisps[];
extern DispPtr	xf86setup_scrn_displays[];
extern char    *xf86VisualNames[];

static int
getsection_screen(interp, varname)
  Tcl_Interp *interp;
  char *varname;
{
    int			i, j, dispn, depth;
    char		*namebuf, tmpbuf[128], tmpbuf2[32];
    ScrnInfoPtr		vptr;
    DispPtr		dptr;
    DisplayModePtr	modeptr;

    for (i = 0; i < xf86MaxScreens; i++)
        if (xf86Screens[i]->configured) {
            vptr = xf86Screens[i];
            namebuf = (char *) XtMalloc(strlen(varname)
                        +strlen(vptr->name)+5);
            sprintf(namebuf, "%s_%s", varname, vptr->name);
            for (dispn = 0; dispn < xf86setup_scrn_ndisps[i]; dispn++) {
                dptr = (DispPtr) ((char *)xf86setup_scrn_displays[i]
                                    +dispn*sizeof(DispRec));
                if ((depth = dptr->depth) == -1)
                    depth = vptr->depth;
                sprintf(tmpbuf, "%d", depth);
                sprintf(tmpbuf2, "Depth,%d", depth);
                Tcl_SetVar2(interp, namebuf, tmpbuf2, tmpbuf, 0);
                if (dptr->frameX0 != -1 || dptr->frameX0 != -1) {
                    sprintf(tmpbuf, "%d %d",
                        dptr->frameX0, dptr->frameY0);
                    sprintf(tmpbuf2, "ViewPort,%d", depth);
                    Tcl_SetVar2(interp, namebuf, tmpbuf2, tmpbuf, 0);
                }
                if (dptr->virtualX != -1 || dptr->virtualY != -1) {
                    sprintf(tmpbuf, "%d %d",
                        dptr->virtualX, dptr->virtualY);
                    sprintf(tmpbuf2, "Virtual,%d", depth);
                    Tcl_SetVar2(interp, namebuf, tmpbuf2, tmpbuf, 0);
                }
                if (dptr->depth == 1 &&
                       (dptr->whiteColour.red   != 0x3F ||
                        dptr->whiteColour.green != 0x3F ||
                        dptr->whiteColour.blue  != 0x3F)) {
                    sprintf(tmpbuf, "%d %d %d",
                        dptr->whiteColour.red,
                        dptr->whiteColour.green,
                        dptr->whiteColour.blue);
                    sprintf(tmpbuf2, "White,%d", depth);
                    Tcl_SetVar2(interp, namebuf, tmpbuf2, tmpbuf, 0);
                }
                if (dptr->depth == 1 &&
                       (dptr->blackColour.red   != 0 ||
                        dptr->blackColour.green != 0 ||
                        dptr->blackColour.blue  != 0)) {
                    sprintf(tmpbuf, "%d %d %d",
		        dptr->blackColour.red,
                        dptr->blackColour.green,
			dptr->blackColour.blue);
                    sprintf(tmpbuf2, "Black,%d", depth);
                    Tcl_SetVar2(interp, namebuf, tmpbuf2, tmpbuf, 0);
                }
                if (dptr->depth == 16 && dptr->weight.red > 0) {
                    sprintf(tmpbuf, "%d%d%d",
                        dptr->weight.red,
                        dptr->weight.green,
                        dptr->weight.blue);
                    sprintf(tmpbuf2, "Weight,%d", depth);
                    Tcl_SetVar2(interp, namebuf, tmpbuf2, tmpbuf, 0);
                }
		if (dptr->defaultVisual >= 0) {
                    sprintf(tmpbuf2, "Visual,%d", depth);
                    Tcl_SetVar2(interp, namebuf, tmpbuf2,
		        xf86VisualNames[dptr->defaultVisual], 0);
		}
		modeptr = dptr->modes;
		if (modeptr) {
                    sprintf(tmpbuf2, "Modes,%d", depth);
		    Tcl_SetVar2(interp, namebuf, tmpbuf2, "", 0);
		    do {
		        Tcl_SetVar2(interp, namebuf, tmpbuf2, modeptr->name,
			    TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
		        modeptr = modeptr->next;
		    } while (dptr->modes != modeptr);
		}

                sprintf(tmpbuf2, "Option,%d", depth);
		Tcl_UnsetVar2(interp, namebuf, tmpbuf2, 0);
		for (j = 0; xf86_OptionTab[j].token != -1; j++)
		    if (OFLG_ISSET(xf86_OptionTab[j].token,
				&(dptr->options))) {
			Tcl_SetVar2(interp, namebuf, tmpbuf2,
			    xf86_OptionTab[j].name,
			    TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
		    }
            }
            Tcl_SetVar2(interp, namebuf, "Monitor",
		    StrOrNull(vptr->monitor->id), 0);
            Tcl_SetVar2(interp, namebuf, "Device",
		    StrOrNull(((GDevPtr) vptr->device)->identifier), 0);
            sprintf(tmpbuf, "%ld", ScreenSaverTime/MILLI_PER_MIN);
            Tcl_SetVar2(interp, namebuf, "BlankTime", tmpbuf, 0);
#ifdef DPMSExtension
            sprintf(tmpbuf, "%d", DPMSStandbyTime/MILLI_PER_MIN);
            Tcl_SetVar2(interp, namebuf, "StandbyTime", tmpbuf, 0);
            sprintf(tmpbuf, "%d", DPMSSuspendTime/MILLI_PER_MIN);
            Tcl_SetVar2(interp, namebuf, "SuspendTime", tmpbuf, 0);
            sprintf(tmpbuf, "%d", DPMSOffTime/MILLI_PER_MIN);
            Tcl_SetVar2(interp, namebuf, "OffTime", tmpbuf, 0);
#endif
            if (vptr->tmpIndex >= 0) {
                sprintf(tmpbuf, "%d", vptr->tmpIndex);
                Tcl_SetVar2(interp, namebuf, "ScreenNo", tmpbuf, 0);
            }
	    XtFree(namebuf);
        }
	return TCL_OK;
}

