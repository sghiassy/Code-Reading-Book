/* $XConsortium: session.c /main/77 1996/11/24 17:32:33 rws $ */
/* $XFree86: xc/programs/xdm/session.c,v 3.11 1996/12/23 07:10:57 dawes Exp $ */
/*

Copyright (c) 1988  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * session.c
 */

#include "dm.h"
#include "greet.h"
#include <X11/Xlib.h>
#include <signal.h>
#include <X11/Xatom.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#ifdef AIXV3
# include <usersec.h>
#endif
#ifdef SECURE_RPC
# include <rpc/rpc.h>
# include <rpc/key_prot.h>
#endif
#ifdef K5AUTH
# include <krb5/krb5.h>
#endif

#ifndef GREET_USER_STATIC
#include <dlfcn.h>
#ifndef RTLD_NOW
#define RTLD_NOW 1
#endif
#endif

#ifdef CSRG_BASED
#include <sys/param.h>
#endif

extern	int	PingServer();
extern	int	SessionPingFailed();
extern	int	Debug();
extern	int	RegisterCloseOnFork();
extern	int	SecureDisplay();
extern	int	UnsecureDisplay();
extern	int	ClearCloseOnFork();
extern	int	SetupDisplay();
extern	int	LogError();
extern	int	SessionExit();
extern	int	DeleteXloginResources();
extern	int	source();
extern	char	**defaultEnv();
extern	char	**setEnv();
extern	char	**parseArgs();
extern	int	printEnv();
extern	char	**systemEnv();
extern	int	LogOutOfMem();
extern	void	setgrent();
extern	struct group	*getgrent();
extern	void	endgrent();
#ifdef USESHADOW
extern	struct spwd	*getspnam();
extern	void	endspent();
#endif
extern	struct passwd	*getpwnam();
extern	char	*crypt();

static	struct dlfuncs	dlfuncs = {
	PingServer,
	SessionPingFailed,
	Debug,
	RegisterCloseOnFork,
	SecureDisplay,
	UnsecureDisplay,
	ClearCloseOnFork,
	SetupDisplay,
	LogError,
	SessionExit,
	DeleteXloginResources,
	source,
	defaultEnv,
	setEnv,
	parseArgs,
	printEnv,
	systemEnv,
	LogOutOfMem,
	setgrent,
	getgrent,
	endgrent,
#ifdef USESHADOW
	getspnam,
	endspent,
#endif
	getpwnam,
	crypt,
	};

#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

static Bool StartClient();

static int			clientPid;
static struct greet_info	greet;
static struct verify_info	verify;

static Jmp_buf	abortSession;

/* ARGSUSED */
static SIGVAL
catchTerm (n)
    int n;
{
    Longjmp (abortSession, 1);
}

static Jmp_buf	pingTime;

/* ARGSUSED */
static SIGVAL
catchAlrm (n)
    int n;
{
    Longjmp (pingTime, 1);
}

static Jmp_buf	tenaciousClient;

/* ARGSUSED */
static SIGVAL
waitAbort (n)
    int n;
{
	Longjmp (tenaciousClient, 1);
}

#if defined(_POSIX_SOURCE) || defined(SYSV) || defined(SVR4)
#define killpg(pgrp, sig) kill(-(pgrp), sig)
#endif

static void
AbortClient (pid)
    int pid;
{
    int	sig = SIGTERM;
#ifdef __STDC__
    volatile int	i;
#else
    int	i;
#endif
    int	retId;
    for (i = 0; i < 4; i++) {
	if (killpg (pid, sig) == -1) {
	    switch (errno) {
	    case EPERM:
		LogError ("xdm can't kill client\n");
	    case EINVAL:
	    case ESRCH:
		return;
	    }
	}
	if (!Setjmp (tenaciousClient)) {
	    (void) Signal (SIGALRM, waitAbort);
	    (void) alarm ((unsigned) 10);
	    retId = wait ((waitType *) 0);
	    (void) alarm ((unsigned) 0);
	    (void) Signal (SIGALRM, SIG_DFL);
	    if (retId == pid)
		break;
	} else
	    (void) Signal (SIGALRM, SIG_DFL);
	sig = SIGKILL;
    }
}

SessionPingFailed (d)
    struct display  *d;
{
    if (clientPid > 1)
    {
    	AbortClient (clientPid);
	source (verify.systemEnviron, d->reset);
    }
    SessionExit (d, RESERVER_DISPLAY, TRUE);
}

/*
 * We need our own error handlers because we can't be sure what exit code Xlib
 * will use, and our Xlib does exit(1) which matches REMANAGE_DISPLAY, which
 * can cause a race condition leaving the display wedged.  We need to use
 * RESERVER_DISPLAY for IO errors, to ensure that the manager waits for the
 * server to terminate.  For other X errors, we should give up.
 */

/*ARGSUSED*/
static
IOErrorHandler (dpy)
    Display *dpy;
{
    LogError("fatal IO error %d (%s)\n", errno, _SysErrorMsg(errno));
    exit(RESERVER_DISPLAY);
}

static int
ErrorHandler(dpy, event)
    Display *dpy;
    XErrorEvent *event;
{
    LogError("X error\n");
    if (XmuPrintDefaultErrorMessage (dpy, event, stderr) == 0) return 0;
    exit(UNMANAGE_DISPLAY);
    /*NOTREACHED*/
}

ManageSession (d)
struct display	*d;
{
    int			pid, code;
    Display		*dpy;
    greet_user_rtn	greet_stat; 
    static GreetUserProc greet_user_proc = NULL;
    void		*greet_lib_handle;

    Debug ("ManageSession %s\n", d->name);
    (void)XSetIOErrorHandler(IOErrorHandler);
    (void)XSetErrorHandler(ErrorHandler);
    SetTitle(d->name, (char *) 0);
    /*
     * Load system default Resources
     */
    LoadXloginResources (d);

#ifdef GREET_USER_STATIC
    greet_user_proc = GreetUser;
#else
    Debug("ManageSession: loading greeter library %s\n", greeterLib);
    greet_lib_handle = dlopen(greeterLib, RTLD_NOW);
    if (greet_lib_handle != NULL)
	greet_user_proc = (GreetUserProc)dlsym(greet_lib_handle, "GreetUser");
    if (greet_user_proc == NULL)
	{
	LogError("%s while loading %s\n", dlerror(), greeterLib);
	exit(UNMANAGE_DISPLAY);
	}
#endif

    /* tell the possibly dynamically loaded greeter function
     * what data structure formats to expect.
     * These version numbers are registered with the X Consortium. */
    verify.version = 1;
    greet.version = 1;
    greet_stat = (*greet_user_proc)(d, &dpy, &verify, &greet, &dlfuncs);

    if (greet_stat == Greet_Success)
    {
	clientPid = 0;
	if (!Setjmp (abortSession)) {
	    (void) Signal (SIGTERM, catchTerm);
	    /*
	     * Start the clients, changing uid/groups
	     *	   setting up environment and running the session
	     */
	    if (StartClient (&verify, d, &clientPid, greet.name, greet.password)) {
		Debug ("Client Started\n");
		/*
		 * Wait for session to end,
		 */
		for (;;) {
		    if (d->pingInterval)
		    {
			if (!Setjmp (pingTime))
			{
			    (void) Signal (SIGALRM, catchAlrm);
			    (void) alarm (d->pingInterval * 60);
			    pid = wait ((waitType *) 0);
			    (void) alarm (0);
			}
			else
			{
			    (void) alarm (0);
			    if (!PingServer (d, (Display *) NULL))
				SessionPingFailed (d);
			}
		    }
		    else
		    {
			pid = wait ((waitType *) 0);
		    }
		    if (pid == clientPid)
			break;
		}
	    } else {
		LogError ("session start failed\n");
	    }
	} else {
	    /*
	     * when terminating the session, nuke
	     * the child and then run the reset script
	     */
	    AbortClient (clientPid);
	}
    }
    /*
     * run system-wide reset file
     */
    Debug ("Source reset program %s\n", d->reset);
    source (verify.systemEnviron, d->reset);
    SessionExit (d, OBEYSESS_DISPLAY, TRUE);
}

LoadXloginResources (d)
struct display	*d;
{
    char	**args, **parseArgs();
    char	**env = 0, **setEnv(), **systemEnv();

    if (d->resources[0] && access (d->resources, 4) == 0) {
	env = systemEnv (d, (char *) 0, (char *) 0);
	args = parseArgs ((char **) 0, d->xrdb);
	args = parseArgs (args, d->resources);
	Debug ("Loading resource file: %s\n", d->resources);
	(void) runAndWait (args, env);
	freeArgs (args);
	freeEnv (env);
    }
}

SetupDisplay (d)
struct display	*d;
{
    char	**env = 0, **setEnv(), **systemEnv();

    if (d->setup && d->setup[0])
    {
    	env = systemEnv (d, (char *) 0, (char *) 0);
    	(void) source (env, d->setup);
    	freeEnv (env);
    }
}

/*ARGSUSED*/
DeleteXloginResources (d, dpy)
struct display	*d;
Display		*dpy;
{
    int i;
    Atom prop = XInternAtom(dpy, "SCREEN_RESOURCES", True);

    XDeleteProperty(dpy, RootWindow (dpy, 0), XA_RESOURCE_MANAGER);
    if (prop) {
	for (i = ScreenCount(dpy); --i >= 0; )
	    XDeleteProperty(dpy, RootWindow (dpy, i), prop);
    }
}

static Jmp_buf syncJump;

/* ARGSUSED */
static SIGVAL
syncTimeout (n)
    int n;
{
    Longjmp (syncJump, 1);
}

SecureDisplay (d, dpy)
struct display	*d;
Display		*dpy;
{
    Debug ("SecureDisplay %s\n", d->name);
    (void) Signal (SIGALRM, syncTimeout);
    if (Setjmp (syncJump)) {
	LogError ("WARNING: display %s could not be secured\n",
		   d->name);
	SessionExit (d, RESERVER_DISPLAY, FALSE);
    }
    (void) alarm ((unsigned) d->grabTimeout);
    Debug ("Before XGrabServer %s\n", d->name);
    XGrabServer (dpy);
    if (XGrabKeyboard (dpy, DefaultRootWindow (dpy), True, GrabModeAsync,
		       GrabModeAsync, CurrentTime) != GrabSuccess)
    {
	(void) alarm (0);
	(void) Signal (SIGALRM, SIG_DFL);
	LogError ("WARNING: keyboard on display %s could not be secured\n",
		  d->name);
	SessionExit (d, RESERVER_DISPLAY, FALSE);
    }
    Debug ("XGrabKeyboard succeeded %s\n", d->name);
    (void) alarm (0);
    (void) Signal (SIGALRM, SIG_DFL);
    pseudoReset (dpy);
    if (!d->grabServer)
    {
	XUngrabServer (dpy);
	XSync (dpy, 0);
    }
    Debug ("done secure %s\n", d->name);
}

UnsecureDisplay (d, dpy)
struct display	*d;
Display		*dpy;
{
    Debug ("Unsecure display %s\n", d->name);
    if (d->grabServer)
    {
	XUngrabServer (dpy);
	XSync (dpy, 0);
    }
}

SessionExit (d, status, removeAuth)
    struct display  *d;
{
    /* make sure the server gets reset after the session is over */
    if (d->serverPid >= 2 && d->resetSignal)
	kill (d->serverPid, d->resetSignal);
    else
	ResetServer (d);
    if (removeAuth)
    {
	setgid (verify.gid);
	setuid (verify.uid);
	RemoveUserAuthorization (d, &verify);
#ifdef K5AUTH
	/* do like "kdestroy" program */
        {
	    krb5_error_code code;
	    krb5_ccache ccache;

	    code = Krb5DisplayCCache(d->name, &ccache);
	    if (code)
		LogError("%s while getting Krb5 ccache to destroy\n",
			 error_message(code));
	    else {
		code = krb5_cc_destroy(ccache);
		if (code) {
		    if (code == KRB5_FCC_NOFILE) {
			Debug ("No Kerberos ccache file found to destroy\n");
		    } else
			LogError("%s while destroying Krb5 credentials cache\n",
				 error_message(code));
		} else
		    Debug ("Kerberos ccache destroyed\n");
		krb5_cc_close(ccache);
	    }
	}
#endif /* K5AUTH */
    }
    Debug ("Display %s exiting with status %d\n", d->name, status);
    exit (status);
}

static Bool
StartClient (verify, d, pidp, name, passwd)
    struct verify_info	*verify;
    struct display	*d;
    int			*pidp;
    char		*name;
    char		*passwd;
{
    char	**f, *home, *getEnv ();
    char	*failsafeArgv[2];
    int	pid;

    if (verify->argv) {
	Debug ("StartSession %s: ", verify->argv[0]);
	for (f = verify->argv; *f; f++)
		Debug ("%s ", *f);
	Debug ("; ");
    }
    if (verify->userEnviron) {
	for (f = verify->userEnviron; *f; f++)
		Debug ("%s ", *f);
	Debug ("\n");
    }
    switch (pid = fork ()) {
    case 0:
	CleanUpChild ();

	/* Do system-dependent login setup here */

#ifdef AIXV3
	/*
	 * Set the user's credentials: uid, gid, groups,
	 * audit classes, user limits, and umask.
	 */
	if (setpcred(name, NULL) == -1)
	{
	    LogError("setpcred for \"%s\" failed, errno=%d\n", name, errno);
	    return (0);
	}
#else /* AIXV3 */
	if (setgid(verify->gid) < 0)
	{
	    LogError("setgid %d (user \"%s\") failed, errno=%d\n",
		     verify->gid, name, errno);
	    return (0);
	}
#if (BSD >= 199103)
	if (setlogin(name) < 0)
	{
	    LogError("setlogin for \"%s\" failed, errno=%d", name, errno);
	    return(0);
	}
#endif
	if (initgroups(name, verify->gid) < 0)
	{
	    LogError("initgroups for \"%s\" failed, errno=%d\n", name, errno);
	    return (0);
	}
	if (setuid(verify->uid) < 0)
	{
	    LogError("setuid %d (user \"%s\") failed, errno=%d\n",
		     verify->uid, name, errno);
	    return (0);
	}
#endif /* AIXV3 */

	/*
	 * for user-based authorization schemes,
	 * use the password to get the user's credentials.
	 */
#ifdef SECURE_RPC
	/* do like "keylogin" program */
	{
	    char    netname[MAXNETNAMELEN+1], secretkey[HEXKEYBYTES+1];
	    int	    nameret, keyret;
	    int	    len;
	    int     key_set_ok = 0;

	    nameret = getnetname (netname);
	    Debug ("User netname: %s\n", netname);
	    len = strlen (passwd);
	    if (len > 8)
		bzero (passwd + 8, len - 8);
	    keyret = getsecretkey(netname,secretkey,passwd);
	    Debug ("getsecretkey returns %d, key length %d\n",
		    keyret, strlen (secretkey));
	    /* is there a key, and do we have the right password? */
	    if (keyret == 1)
	    {
		if (*secretkey)
		{
		    keyret = key_setsecret(secretkey);
		    Debug ("key_setsecret returns %d\n", keyret);
		    if (keyret == -1)
			LogError ("failed to set NIS secret key\n");
		    else
			key_set_ok = 1;
		}
		else
		{
		    /* found a key, but couldn't interpret it */
		    LogError ("password incorrect for NIS principal \"%s\"\n",
			      nameret ? netname : name);
		}
	    }
	    if (!key_set_ok)
	    {
		/* remove SUN-DES-1 from authorizations list */
		int i, j;
		for (i = 0; i < d->authNum; i++)
		{
		    if (d->authorizations[i]->name_length == 9 &&
			memcmp(d->authorizations[i]->name, "SUN-DES-1", 9) == 0)
		    {
			for (j = i+1; j < d->authNum; j++)
			    d->authorizations[j-1] = d->authorizations[j];
			d->authNum--;
			break;
		    }
		}
	    }
	    bzero(secretkey, strlen(secretkey));
	}
#endif
#ifdef K5AUTH
	/* do like "kinit" program */
	{
	    int i, j;
	    int result;
	    extern char *Krb5CCacheName();

	    result = Krb5Init(name, passwd, d);
	    if (result == 0) {
		/* point session clients at the Kerberos credentials cache */
		verify->userEnviron =
		    setEnv(verify->userEnviron,
			   "KRB5CCNAME", Krb5CCacheName(d->name));
	    } else {
		for (i = 0; i < d->authNum; i++)
		{
		    if (d->authorizations[i]->name_length == 14 &&
			memcmp(d->authorizations[i]->name, "MIT-KERBEROS-5", 14) == 0)
		    {
			/* remove Kerberos from authorizations list */
			for (j = i+1; j < d->authNum; j++)
			    d->authorizations[j-1] = d->authorizations[j];
			d->authNum--;
			break;
		    }
		}
	    }
	}
#endif /* K5AUTH */
	bzero(passwd, strlen(passwd));
	SetUserAuthorization (d, verify);
	home = getEnv (verify->userEnviron, "HOME");
	if (home)
	    if (chdir (home) == -1) {
		LogError ("user \"%s\": cannot chdir to home \"%s\" (err %d), using \"/\"\n",
			  getEnv (verify->userEnviron, "USER"), home, errno);
		chdir ("/");
		verify->userEnviron = setEnv(verify->userEnviron, "HOME", "/");
	    }
	if (verify->argv) {
		Debug ("executing session %s\n", verify->argv[0]);
		execute (verify->argv, verify->userEnviron);
		LogError ("Session \"%s\" execution failed (err %d)\n", verify->argv[0], errno);
	} else {
		LogError ("Session has no command/arguments\n");
	}
	failsafeArgv[0] = d->failsafeClient;
	failsafeArgv[1] = 0;
	execute (failsafeArgv, verify->userEnviron);
	exit (1);
    case -1:
	bzero(passwd, strlen(passwd));
	Debug ("StartSession, fork failed\n");
	LogError ("can't start session on \"%s\", fork failed, errno=%d\n",
		  d->name, errno);
	return 0;
    default:
	bzero(passwd, strlen(passwd));
	Debug ("StartSession, fork succeeded %d\n", pid);
	*pidp = pid;
	return 1;
    }
}

int
source (environ, file)
char			**environ;
char			*file;
{
    char	**args, *args_safe[2];
    extern char	**parseArgs ();
    int		ret;

    if (file && file[0]) {
	Debug ("source %s\n", file);
	args = parseArgs ((char **) 0, file);
	if (!args)
	{
	    args = args_safe;
	    args[0] = file;
	    args[1] = NULL;
	}
	ret = runAndWait (args, environ);
	freeArgs (args);
	return ret;
    }
    return 0;
}

int
runAndWait (args, environ)
    char	**args;
    char	**environ;
{
    int	pid;
    waitType	result;

    switch (pid = fork ()) {
    case 0:
	CleanUpChild ();
	execute (args, environ);
	LogError ("can't execute \"%s\" (err %d)\n", args[0], errno);
	exit (1);
    case -1:
	Debug ("fork failed\n");
	LogError ("can't fork to execute \"%s\" (err %d)\n", args[0], errno);
	return 1;
    default:
	while (wait (&result) != pid)
		/* SUPPRESS 530 */
		;
	break;
    }
    return waitVal (result);
}

void
execute (argv, environ)
    char **argv;
    char **environ;
{
    /* give /dev/null as stdin */
    (void) close (0);
    open ("/dev/null", 0);
    /* make stdout follow stderr to the log file */
    dup2 (2,1);
    execve (argv[0], argv, environ);
    /*
     * In case this is a shell script which hasn't been
     * made executable (or this is a SYSV box), do
     * a reasonable thing
     */
    if (errno != ENOENT) {
	char	program[1024], *e, *p, *optarg;
	FILE	*f;
	char	**newargv, **av;
	int	argc;

	/*
	 * emulate BSD kernel behaviour -- read
	 * the first line; check if it starts
	 * with "#!", in which case it uses
	 * the rest of the line as the name of
	 * program to run.  Else use "/bin/sh".
	 */
	f = fopen (argv[0], "r");
	if (!f)
	    return;
	if (fgets (program, sizeof (program) - 1, f) == NULL)
 	{
	    fclose (f);
	    return;
	}
	fclose (f);
	e = program + strlen (program) - 1;
	if (*e == '\n')
	    *e = '\0';
	if (!strncmp (program, "#!", 2)) {
	    p = program + 2;
	    while (*p && isspace (*p))
		++p;
	    optarg = p;
	    while (*optarg && !isspace (*optarg))
		++optarg;
	    if (*optarg) {
		*optarg = '\0';
		do
		    ++optarg;
		while (*optarg && isspace (*optarg));
	    } else
		optarg = 0;
	} else {
	    p = "/bin/sh";
	    optarg = 0;
	}
	Debug ("Shell script execution: %s (optarg %s)\n",
		p, optarg ? optarg : "(null)");
	for (av = argv, argc = 0; *av; av++, argc++)
	    /* SUPPRESS 530 */
	    ;
	newargv = (char **) malloc ((argc + (optarg ? 3 : 2)) * sizeof (char *));
	if (!newargv)
	    return;
	av = newargv;
	*av++ = p;
	if (optarg)
	    *av++ = optarg;
	/* SUPPRESS 560 */
	while (*av++ = *argv++)
	    /* SUPPRESS 530 */
	    ;
	execve (newargv[0], newargv, environ);
    }
}

extern char **setEnv ();

char **
defaultEnv ()
{
    char    **env, **exp, *value;

    env = 0;
    for (exp = exportList; exp && *exp; ++exp)
    {
	value = getenv (*exp);
	if (value)
	    env = setEnv (env, *exp, value);
    }
    return env;
}

char **
systemEnv (d, user, home)
struct display	*d;
char	*user, *home;
{
    char	**env;
    
    env = defaultEnv ();
    env = setEnv (env, "DISPLAY", d->name);
    if (home)
	env = setEnv (env, "HOME", home);
    if (user)
    {
	env = setEnv (env, "USER", user);
	env = setEnv (env, "LOGNAME", user);
    }
    env = setEnv (env, "PATH", d->systemPath);
    env = setEnv (env, "SHELL", d->systemShell);
    if (d->authFile)
	    env = setEnv (env, "XAUTHORITY", d->authFile);
    return env;
}

#if defined(Lynx) || defined(SCO) && !defined(SCO_USA) && !defined(_SCO_DS)
char *crypt(s1, s2)
	char	*s1, *s2;
{
	return(s2);
}
#endif
