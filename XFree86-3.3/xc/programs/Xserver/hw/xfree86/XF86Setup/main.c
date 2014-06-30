/* $XConsortium: main.c /main/2 1996/10/23 13:12:09 kaleb $ */




/* $XFree86: xc/programs/Xserver/hw/xfree86/XF86Setup/main.c,v 3.9 1996/12/27 06:54:05 dawes Exp $ */
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
 * Main procedure for XF86Setup, by Joe Moss
 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/Xos.h>
#include <tcl.h>
#include <tk.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>

#if !defined(SYSV) && !defined(AMOEBA) && !defined(_MINIX)
#include <sys/resource.h>
#endif

#if TK_MAJOR_VERSION < 4
#error You must use Tk 4.0 or newer
#endif

static Tcl_Interp *interp;

static char *name = NULL;		/* name of application */
static char *display = NULL;		/* display to connect to */
static char *geometry = NULL;		/* initial geometry spec */
static char *statefile = NULL;		/* file containing state vars */
static char *LibDir;			/* where the tcl src files are */
static int  rest = 0;			/* arg after options */
static int  synchronize = 0;		/* sync X connection */
static int  nodialog = 0;		/* Don't use Dialog */
static int  notk = 0;			/* Don't add Tk to interp */
static int  usescriptdir = 0;		/* Use script dir, not PATH */

#define PHASE1	"phase1.tcl"
#define PHASE2	"phase2.tcl"
#define PHASE3	"phase3.tcl"
#define PHASE4	"phase4.tcl"
#define PHASE5	"phase5.tcl"

/*
  Initialization code - sets the Xwinhome, XF86Setup_library,
      tcl_library, and tk_library variables and checks for the
      the existence of the startup file (phase1)
*/

#ifndef PROJECTROOT
#define PROJECTROOT ""
#endif

static char Set_InitVars[] =
	"if [info exists env(XWINHOME)] {\n"
	"    set Xwinhome $env(XWINHOME)\n"
	"} else {\n"
	"    set xdirs [list " PROJECTROOT " /usr/X11R6 /usr/X11 "
		"/usr/X /var/X11R6 /var/X11 /var/X /usr/X11R6.1 "
		"/usr/local/X11R6 /usr/local/X11 /usr/local/X]\n"
	"    foreach dir $xdirs {\n"
	"        if {[llength [glob -nocomplain $dir/bin/XF86_*]] } {\n"
	"            set Xwinhome $dir\n"
	"            break\n"
	"        }\n"
	"    }\n"
	"    if ![info exists Xwinhome] {\n"
	"        error \"Couldn't determine where you have XFree86 installed.\\n"
		"If you have XFree86 properly installed, set the "
		"XWINHOME environment\\n variable to point to the "
		"parent directory of the XFree86 bin directory.\\n\"\n"
	"    }\n"
	"    unset xdirs dir\n"
	"}\n"
	"if [info exists env(XF86SETUPLIB)] {\n"
	"    set XF86Setup_library $env(XF86SETUPLIB)\n"
	"} else {\n"
	"    set XF86Setup_library $Xwinhome/lib/X11/XF86Setup\n"
	"}\n"
	"set tk_library [set tcl_library $XF86Setup_library/tcllib]\n"
	"set XF86Setup_startup $XF86Setup_library/" PHASE1 "\n"
	"if ![file exists $XF86Setup_startup] {\n"
	"    error \"The startup file for this program ($XF86Setup_startup)\\n"
		"is missing. You need to install it before running "
		"this program.\\n\"\n"
	"} else {\n"
	"    if ![file readable $XF86Setup_startup] {\n"
	"        error \"The startup file for this program "
		"($XF86Setup_startup)\\ncan't be accessed. "
		"Perhaps a permission problem?\\n\"\n"
	"    }\n"
	"}\n"
	;

static char usage_msg[] =
	"Usage: %s [options] [filename] [--] [arg ...]\n"
	"Options always available:\n"
	"   -sync		Use synchronous mode for display server\n"
	"   -name <name>	Name to use for application\n"
	"\n"
	"Options available only when a filename is specified:\n"
	"   -display <disp>	Display to use\n"
	"   -geometry <geom>	Initial geometry for window\n"
	"   -notk		Don't open a connection to the X server\n"
	"   -script		Look for filename in script directory\n"
	"\n"
	"Options available only when a filename is not specified:\n"
	"   -nodialog      	Don't use Dialog for user interaction\n"
	"Any args after the double dashes (--) are passed to the script\n"
	;

static Tk_ArgvInfo argTable[] = {
    {"-display", TK_ARGV_STRING, (char *) NULL, (char *) &display,
        "Display to use"},
    {"-geometry", TK_ARGV_STRING, (char *) NULL, (char *) &geometry,
        "Initial geometry for window"},
    {"-name", TK_ARGV_STRING, (char *) NULL, (char *) &name,
        "Name to use for application"},
    {"-sync", TK_ARGV_CONSTANT, (char *) 1, (char *) &synchronize,
        "Use synchronous mode for display server"},
    {"-nodialog", TK_ARGV_CONSTANT, (char *) 1, (char *) &nodialog,
        "Don't use the Dialog program for interaction with user"},
    {"-notk", TK_ARGV_CONSTANT, (char *) 1, (char *) &notk,
        "Don't open a connection to the X server or load Tk widgets"},
    {"-script", TK_ARGV_CONSTANT, (char *) 1, (char *) &usescriptdir,
        "Look for filename in the scripts directory"},
    {"--", TK_ARGV_REST, (char *) 1, (char *) &rest,
        "Pass all remaining arguments through to script"},
    /* This one is undocumented - it's used when execing a 2nd copy */
    {"-statefile", TK_ARGV_STRING, (char *) NULL, (char *) &statefile, ""},
    {(char *) NULL, TK_ARGV_END, (char *) NULL, (char *) NULL,
        (char *) NULL}
};

extern int	XF86Other_Init(
#if NeedFunctionProtoTypes
	Tcl_Interp *interp
#endif
);

extern int	XF86TkOther_Init(
#if NeedFunctionProtoTypes
	Tcl_Interp *interp
#endif
);

extern int	Cards_Init(
#if NeedFunctionProtoTypes
	Tcl_Interp *interp
#endif
);

extern int	XF86Config_Init(
#if NeedFunctionProtoTypes
	Tcl_Interp *interp
#endif
);

extern int	XF86vid_Init(
#if NeedFunctionProtoTypes
	Tcl_Interp *interp
#endif
);

extern int	XF86Misc_Init(
#if NeedFunctionProtoTypes
	Tcl_Interp *interp
#endif
);

extern int	XF86Kbd_Init(
#if NeedFunctionProtoTypes
	Tcl_Interp *interp
#endif
);

static void	XF86Setup_TclEvalFile(
#if NeedFunctionProtoTypes
	Tcl_Interp *interp,
	char *filename
#endif
);

static void	XF86Setup_TclRunScript(
#if NeedFunctionProtoTypes
	Tcl_Interp *interp,
	char *filename
#endif
);

static void	XF86Setup_TkInit(
#if NeedFunctionProtoTypes
	Tcl_Interp *interp
#endif
);

static void	kill_server (
#if NeedFunctionProtoTypes
	Tcl_Interp *interp
#endif
);

/*
  Runs the commands in the specified file in the lib directory

  If an error occurs while processing, the error message is printed
     and the program exits
*/
static void
XF86Setup_TclEvalFile(interp, filename)
    Tcl_Interp	*interp;
    char	*filename;
{
    int retval;
    char *msg;

    retval = Tcl_VarEval(interp, "source ",
			    LibDir, "/", filename, (char *) NULL);
    if (retval != TCL_OK) {
	msg = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
	if (msg == NULL) {
	    msg = interp->result;
	}
	fprintf(stderr, "%s\n", msg);
	Tcl_Eval(interp, "exit 1");
    }
    fflush(stdout);
    Tcl_ResetResult(interp);
}

/*
  Runs the commands in the specified file - search PATH, if needed

  If an error occurs while processing, the error message is printed
     and the program exits
*/
static void
XF86Setup_TclRunScript(interp, filename)
    Tcl_Interp	*interp;
    char	*filename;
{
    int retval;
    char *msg;

    if (!usescriptdir) {
	retval = Tcl_VarEval(interp,
	    "if {[string first ", filename, " /] != -1} {",
		"source ", filename,
	    "} else {",
		"foreach dir [split $env(PATH) :] {",
		    "if [file executable $dir/", filename, "] {",
			"source $dir/", filename, "; return 0",
		    "}",
		"}\n",
		"error {File not found: ", filename, "}\n",
	    "}",
	    (char *) NULL);
    } else {
	retval = Tcl_VarEval(interp,
	    "if {[string first ", filename, " /] != -1} {",
		"source ", filename,
	    "} else {",
		"source $XF86Setup_library/scripts/", filename,
	    "}",
	    (char *) NULL);
    }

    if (retval != TCL_OK) {
	msg = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
	if (msg == NULL) {
	    msg = interp->result;
	}
	fprintf(stderr, "%s\n", msg);
	Tcl_Eval(interp, "exit 1");
    }
    fflush(stdout);
    Tcl_ResetResult(interp);
}

#define print_result_and_exit {fprintf(stderr,"%s\n",interp->result); exit(1);}

/*

  Main function for XF86Setup.

  Creates a Tcl interpreter and adds various extensions to it.
  Then if a filename is not given on the command line, it executes
     the commands in either phase1.tcl or phase3.tcl
  Tk is then added to the interpreter, as well as various X related
     extensions
  If a filename was given, it is then executed, otherwise either
     phase2.tcl or phase4.tcl is run
  If phase2.tcl was run, the server is shutdown and then a new
     invocation of XF86Setup is exec-ed
  If phase4.tcl was run, the server is shutdown and phase5.tcl run

*/

void
main(argc, argv)
    int		argc;
    char	**argv;
{
    char *tmpptr, *filename, *argv0, tmpbuf[20], buf[128];
    int   Phase2FallBack = 0;

#ifdef RLIMIT_STACK
#define STACK_SIZE	(2048*1024)

    struct rlimit rlim;

    if (!getrlimit(RLIMIT_STACK, &rlim))
    {
	if (STACK_SIZE < rlim.rlim_max)
	    rlim.rlim_cur = STACK_SIZE;
	else
	    rlim.rlim_cur = rlim.rlim_max;
	(void)setrlimit(RLIMIT_STACK, &rlim);
    }
#undef STACK_SIZE
#endif

    /****  Create the Tcl interpreter  ****/
    interp = Tcl_CreateInterp();

    /****  Parse the command line args  ****/
    if (Tk_ParseArgv(interp, (Tk_Window) NULL, &argc, argv, argTable, 0)
	    != TCL_OK) {
	fprintf(stderr, usage_msg, argv[0]);
	exit(1);
    }

    if (argc == 1)
	filename = NULL;
    else if (argc == 2)
	filename = argv[1];
    else {
	if (rest == 0 || rest == 2)
	    filename = argv[1];
	else {
	    fprintf(stderr, usage_msg, argv[0]);
	    exit(1);
	}
    }

    /****  Add the commands to the Tcl interpreter for the
           convenience functions ****/

    if (XF86Other_Init(interp) == TCL_ERROR)
	print_result_and_exit;

    /****  Add the commands to the Tcl interpreter that interface
           with the Cards database ****/

    if (Cards_Init(interp) == TCL_ERROR)
	print_result_and_exit;

    /****  Add the commands to the Tcl interpreter that interface
           with the XF86Config reading routines ****/

    if (XF86Config_Init(interp) == TCL_ERROR)
	print_result_and_exit;

    /****  Find where things are installed ****/
    if (Tcl_Eval(interp, Set_InitVars) != TCL_OK)
	print_result_and_exit;

    LibDir = Tcl_GetVar(interp, "XF86Setup_library", TCL_GLOBAL_ONLY);

    /**** This program will not be used interactively as a shell ****/
    Tcl_SetVar(interp, "tcl_interactive", "0", TCL_GLOBAL_ONLY);

    /****  Make command-line arguments available to scripts  ****/

    argv0 = argv[0];
    if (filename != NULL) {
        tmpptr = Tcl_Merge(argc-2, argv+2);
        sprintf(tmpbuf, "%d", argc-2);
    } else {
        tmpptr = Tcl_Merge(argc-1, argv+1);
        sprintf(tmpbuf, "%d", argc-1);
    }
    Tcl_SetVar(interp, "argv", tmpptr, TCL_GLOBAL_ONLY);
    XtFree(tmpptr);
    Tcl_SetVar(interp, "argc", tmpbuf, TCL_GLOBAL_ONLY);
    Tcl_SetVar(interp, "argv0", argv0, TCL_GLOBAL_ONLY);

    if (filename == NULL) {
	Tcl_LinkVar(interp, "Phase2FallBack",
		(char *)&Phase2FallBack, TCL_LINK_BOOLEAN);
	if (statefile == NULL) {
		Tcl_LinkVar(interp, "NoDialog",
			(char *)&nodialog, TCL_LINK_BOOLEAN);
		/****  Execute the Phase I Tcl code  ****/
		XF86Setup_TclEvalFile(interp, PHASE1);
	} else {
		Tcl_SetVar(interp, "StateFileName",
			statefile, TCL_GLOBAL_ONLY);
		/****  Execute the Phase III Tcl code  ****/
		XF86Setup_TclEvalFile(interp, PHASE3);
	}
	tmpptr = Tcl_GetVar(interp, "ExitStatus", TCL_GLOBAL_ONLY);
	if (tmpptr) {
		Tcl_VarEval(interp, "exit ", tmpptr, (char *)0);
		exit(1);
	}
    } else {
	if (notk) {
            /****  Load the default bindings ****/
	    strcpy(buf, "source $tk_library/init.tcl");
	    if (Tcl_Eval(interp, buf) != TCL_OK)
		print_result_and_exit;
	    XF86Setup_TclRunScript(interp, filename);
	    Tcl_Eval(interp, "exit 0");
	    exit(1);
	}
    }

    /* If the app name wasn't given, use the filename or argv[0] */
    if (name == NULL) {
	if (filename == NULL) {
            name = strrchr(argv0, '/');
            if (name != NULL)
                name++;
            else
                name = argv0;
	} else
            name = filename;
    }

    if (filename == NULL)
	display = NULL;

    /**** Here is the first routine that needs to have an X
	  server running.  It tries to create a window and will,
	  of course, fail if the server isn't running ****/

    XF86Setup_TkInit(interp, display, name);

    if (filename != NULL) {
        /*
         * Set the geometry of the main window, if requested.  Put the
         * requested geometry into the "geometry" variable.
         */

        if (geometry != NULL) {
            Tcl_SetVar(interp, "geometry", geometry, TCL_GLOBAL_ONLY);
            if (Tcl_VarEval(interp, "wm geometry . ", geometry, (char *) NULL)
		    != TCL_OK) {
                fprintf(stderr, "%s\n", interp->result);
            }
        }

	/****  set the DISPLAY environment variable  ****/
        if (display != NULL)
            Tcl_SetVar2(interp, "env", "DISPLAY", display, TCL_GLOBAL_ONLY);
    }

    /****  Add the commands to the Tcl interpreter that interface
           with the XFree86-VidModeExtension ****/

    if (XF86vid_Init(interp) == TCL_ERROR)
	print_result_and_exit;

    /****  Add the commands to the Tcl interpreter that interface
           with the XFree86-Misc extension ****/

    if (XF86Misc_Init(interp) == TCL_ERROR)
	print_result_and_exit;

    /****  Add the commands to the Tcl interpreter that interface
           with the XKEYBOARD extension and library ****/

    if (XF86Kbd_Init(interp) == TCL_ERROR)
	print_result_and_exit;

    /****  Add the commands to the Tcl interpreter for the
           Tk convenience functions ****/

    if (XF86TkOther_Init(interp) == TCL_ERROR)
	print_result_and_exit;

    if (filename != NULL) {
        /****  Load the default bindings ****/
	strcpy(buf, "source $tk_library/init.tcl");
	if (Tcl_Eval(interp, buf) != TCL_OK)
		print_result_and_exit;
	strcpy(buf, "source $tk_library/tk.tcl");
	if (Tcl_Eval(interp, buf) != TCL_OK)
		print_result_and_exit;
	XF86Setup_TclRunScript(interp, filename);
        Tk_MainLoop();
    } else {
	if (statefile == NULL || Phase2FallBack) {
            /****  Now execute the Phase II commands ****/
            XF86Setup_TclEvalFile(interp, PHASE2);
	} else {
            /****  Now execute the Phase IV commands ****/
            XF86Setup_TclEvalFile(interp, PHASE4);
	}
        /**** Enter the event loop until Phase II/IV is completed (last
	        window destroyed) ****/

        Tk_MainLoop();

	kill_server(interp);

	tmpptr = Tcl_GetVar(interp, "ExitStatus", TCL_GLOBAL_ONLY);
	if (tmpptr) {
		Tcl_VarEval(interp, "exit ", tmpptr, (char *)0);
		exit(1);
	}
	if (statefile == NULL || Phase2FallBack) {
	    statefile = Tcl_GetVar(interp, "StateFileName", TCL_GLOBAL_ONLY);
#ifdef DEBUG
	    fprintf(stderr,
		"Executing second copy of XF86Setup (%s -statefile %s)...\n",
		argv[0], statefile);
#endif
	    if (statefile) {
	        execlp(argv[0], argv[0], "-statefile", statefile, (char *)0);
		fprintf(stderr,
	            "Exec of 2nd XF86Setup failed! Returned error #%d\n",
		    errno);
		Tcl_Eval(interp, "exit 1");
		exit(1);
	    }
#ifdef DEBUG
	    else
		fprintf(stderr, "The StateFileName variable isn't set!\n");
#endif
	} else {
            /****  Lastly, execute the Phase V commands ****/
            XF86Setup_TclEvalFile(interp, PHASE5);
	}
    }

    Tcl_Eval(interp, "exit 0");
    exit(1);
}

void keypress() {
/*
 * The parse_database routine (in cards.c) calls keypress() when it
 * finds a problem with the database (after printing an error message)
 *
 * We just ignore the error
 */
}

/*
  Kill the process whose id is in the ServerPID variable, if any

  This is used to kill the X server and switch back to text mode
*/
void kill_server(interp)
    Tcl_Interp	*interp;
{
	int pid;
	char *pidstr;

	pidstr = Tcl_GetVar(interp, "ServerPID", TCL_GLOBAL_ONLY);
	if (pidstr == NULL)
		return;
	pid = atoi(pidstr);
	if (!pid)
		return;
	kill(pid,SIGTERM);
	sleep(2);
	return;
}

#if TK_MAJOR_VERSION > 4 || TK_MINOR_VERSION > 0
extern int	TkCreateFrame (
#if NeedFunctionProtoTypes
	ClientData	clientData,
	Tcl_Interp	*interp,
	int		argc,
	char		**argv,
	int		toplevel,
	char		*appName
#endif
);

extern int	TkPlatformInit (
#if NeedFunctionProtoTypes
	Tcl_Interp	*interp
#endif
);
#endif  /* TK_VERSION > 4.0 */

extern int	XkbUIWinCmd (
#if NeedFunctionProtoTypes
	ClientData	clientData,
	Tcl_Interp	*interp,
	int		argc,
	char		**argv
#endif
);

/*
  Open a connection to the X server and create the main Tk window
*/
static void
XF86Setup_TkInit(interp, display, appName)
    Tcl_Interp	*interp;
    char	*display;
    char	*appName;
{
    Tk_Window	mainWindow;
    char	*class;
#if TK_MAJOR_VERSION > 4 || TK_MINOR_VERSION > 0
    char	*av[10];
    int		ac;
#endif

    if (appName == NULL)
	appName = "XF86Setup";
    if (display == NULL)
	display = Tcl_GetVar2(interp, "env", "DISPLAY", TCL_GLOBAL_ONLY);
    class = (char *) XtMalloc((unsigned) (strlen(appName) + 1));
    strcpy(class, appName);
    if (islower(class[0]))
	class[0] = toupper((unsigned char) class[0]);

#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION == 0
    mainWindow = Tk_CreateMainWindow(interp, display, appName, class);
    if (mainWindow == NULL)
	print_result_and_exit;
#else
    av[0] = "toplevel";
    av[1] = ".";
    av[2] = "-class";
    av[3] = class;
    ac = 4;
    if (display != NULL) {
	av[4] = "-screen";
	av[5] = display;
	ac = 6;
    }
    av[ac] = NULL;
    if (TkCreateFrame((ClientData) NULL, interp, ac, av, 1, appName) != TCL_OK)
	print_result_and_exit;

    Tcl_ResetResult(interp);
    mainWindow = Tk_MainWindow(interp);
    TkPlatformInit(interp);
#endif
    XtFree(class);

    if (synchronize)
	XSynchronize(Tk_Display(mainWindow), True);

    /**** Add the xkbview widget command to the interpreter ****/
    Tcl_CreateCommand(interp, "xkbview", XkbUIWinCmd,
	(ClientData) mainWindow, (void (*)()) NULL);
}

