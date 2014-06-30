/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/os2/os2_diag.c,v 3.1.2.1 1997/05/12 12:52:34 hohndel Exp $ */
/*
 * (c) Copyright 1997 by Holger Veit
 *			<Holger.Veit@gmd.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * HOLGER VEIT  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Except as contained in this notice, the name of Holger Veit shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from Holger Veit.
 *
 */
/* $XConsortium$ */

/* This file checks whether the user has installed the system correctly, 
 * to avoid the numerous questions why this or that does not work
 */

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "scrnintstr.h"

#include "compiler.h"

#define I_NEED_OS2_H
#define INCL_DOSFILEMGR
#define INCL_KBD
#define INCL_VIO
#define INCL_DOSMISC
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_DOSMODULEMGR
#define INCL_DOSFILEMGR
#include "xf86.h"
#include "xf86Procs.h"
#include "xf86_OSlib.h"

static BOOL diag_checks = FALSE;

/* from Eberhard to check for the right EMX version */
static void check_emx (void)
{
	ULONG rc;
	HMODULE hmod;
	char name[CCHMAXPATH];
	char fail[9];

	if (_emx_rev < 50) {
		ErrorF("xf86-OS/2: This program requires emx.dll revision 50 (0.9c fix 2) "
			"or later.\n");
		rc = DosLoadModule (fail, sizeof (fail), "emx", &hmod);
		if (rc == 0) {
			rc = DosQueryModuleName (hmod, sizeof (name), name);
			if (rc == 0)
				ErrorF("Please delete or update `%s'.\n", name);
			DosFreeModule (hmod);
		}
		exit (2);
        }
}

static void check_bsl(const char *var)
{
	char *t1 = strrchr(var,'\\');
	if (strchr(var,'/')) {
		ErrorF("xf86-OS/2: \"%s\" must exclusively use backward slashes \"\\\"\n",
			var);
	}
	if (t1 && *(t1+1)=='\0') {
		ErrorF("xf86-OS/2: \"%s\" mustn't end with \"\\\"\n",var);
		*t1 = '\0';
	}
}

static void check_long(const char* path)
{
	FILE *f;
	char n[300];

	sprintf(n,"%s\\xf86_test_for_very_long_filename",path);
	f = fopen(n,"w");
	if (f==NULL) {
	ErrorF("xf86-OS/2: \"%s\" does not accept long filenames\nmust reside on HPFS or similar\n",
		path);
	} else {
		fclose(f);
		unlink(n);
	}
}

char *check_env_present(const char *env)
{
	char *e = getenv(env);
	if (!e) {
		ErrorF("xf86-OS/2: You have no \"%s\" environment variable, but need one\n",
			env);
		return 0;
	}
	return e;
}

void os2_checkinstallation(void)
{
	char *emxopt, *tmp, *home, *logname, *termcap;
	int i;

	if (diag_checks) return;
	diag_checks = TRUE;

	/* test whether the EMX version is okay */
	check_emx();

	/* Check a number of environment variables */
	emxopt = getenv("EMXOPT");
	if (emxopt) {
		for (i=0; i<strlen(emxopt); i++) {
			if (emxopt[i]=='-') {
				switch (emxopt[++i]) {
				case 't':
					ErrorF("xf86-OS/2: Remove -t option from EMXOPT variable!\n");
					break;
				case 'r':
					ErrorF("xf86-OS/2: Remove -r option from EMXOPT variable!\n");
				}
			}
		}
	}

	tmp = check_env_present("TMP");
	if (tmp) {
	        check_bsl(tmp);
	        check_long(tmp);
	}

	home = check_env_present("HOME");
	if (home) {
	        check_bsl(home);
	        check_long(home);
	}

	logname = check_env_present("LOGNAME");
	termcap = check_env_present("TERMCAP");
	if (termcap)
	        check_bsl(termcap);
}
