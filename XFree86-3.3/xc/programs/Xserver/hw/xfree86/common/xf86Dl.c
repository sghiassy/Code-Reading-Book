/* $XFree86: xc/programs/Xserver/hw/xfree86/common/xf86Dl.c,v 3.10 1996/12/29 13:50:59 dawes Exp $ */

/*    
 * Copyright 1995 by Frederic Lepied, France. <fred@sugix.frmug.fr.net>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is  hereby granted without fee, provided that
 * the  above copyright   notice appear  in   all  copies and  that both  that
 * copyright  notice   and   this  permission   notice  appear  in  supporting
 * documentation, and that   the  name of  Frederic   Lepied not  be  used  in
 * advertising or publicity pertaining to distribution of the software without
 * specific,  written      prior  permission.     Frederic  Lepied   makes  no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * FREDERIC  LEPIED DISCLAIMS ALL   WARRANTIES WITH REGARD  TO  THIS SOFTWARE,
 * INCLUDING ALL IMPLIED   WARRANTIES OF MERCHANTABILITY  AND   FITNESS, IN NO
 * EVENT  SHALL FREDERIC  LEPIED BE   LIABLE   FOR ANY  SPECIAL, INDIRECT   OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA  OR PROFITS, WHETHER  IN  AN ACTION OF  CONTRACT,  NEGLIGENCE OR OTHER
 * TORTIOUS  ACTION, ARISING    OUT OF OR   IN  CONNECTION  WITH THE USE    OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: xf86Dl.c /main/10 1996/12/28 15:40:19 kaleb $ */

#include <os.h>			/* for Error() */
#include <dlfcn.h>

#define NO_OSLIB_PROTOTYPES
#include "xf86_OSlib.h"

#include "xf86Version.h"
#include "xf86.h"
#include "xf86_Config.h"
#include "xf86Priv.h"

#ifdef CSRG_BASED
#define PREPEND_UNDERSCORE
#endif

#ifdef RTLD_NOW
#define DLOPEN_FLAGS RTLD_NOW	/* Linux, SVR4 */
#else
#ifdef DL_LAZY
#define DLOPEN_FLAGS DL_LAZY	/* NetBSD */
#else
#define DLOPEN_FLAGS 1	/* FreeBSD */
#endif
#endif

typedef int (*InitModule)(
#if NeedFunctionPrototypes
	unsigned long	/* server_version */
#endif
);

void*
xf86LoadModule(const char *	file,
	       const char *	path)
{
    void	*module = NULL;
    char	*dir_elem, *path_elem, *keep;

    /* allocate a copy even for the absolute path, for consistency in
     * error reporting/recovery.
     */
    keep = dir_elem = (char *) xcalloc(1, strlen(path) + 1);
    strcpy(dir_elem, path);

    /* absolute path */
    if (file[0] == '/') {
	module = dlopen((char *)file, DLOPEN_FLAGS);
    } else { /* look for file in path */
	struct stat	stat_buf;

	dir_elem = strtok(dir_elem, ",");
	while (!module && (dir_elem != NULL)) {
	    /* only allow fully specified path */
	    if (*dir_elem == '/') {
		if (dir_elem[strlen(dir_elem) - 1] == '/') {
		    path_elem = (char*) xcalloc(1, strlen(file) +
					        strlen(dir_elem) + 1);
		    strcpy(path_elem, dir_elem);
		} else {
		    path_elem = (char*) xcalloc(1, strlen(file) +
						strlen(dir_elem) + 2);
		    strcpy(path_elem, dir_elem);
		    path_elem[strlen(dir_elem)] = '/';
		    path_elem[strlen(dir_elem)+1] = '\0';
		}
		strcat(path_elem, file);
		if ((stat(path_elem, &stat_buf) == 0) &&
		    ((S_IFMT & stat_buf.st_mode) == S_IFREG)) {
		    module = dlopen(path_elem, DLOPEN_FLAGS);
		}
		xfree(path_elem);
	    }
	    if (!module) {
		dir_elem = strtok(NULL, ",");
	    }
	}
    }
    
    if (!module) {
	const char *err = dlerror();

	ErrorF("%s: %s\n", file, err ? err : "Unknown error loading module");
    } else {
#ifdef PREPEND_UNDERSCORE
#ifndef DLSYM_BUG
	InitModule init_module = (InitModule)dlsym(module, "_init_module");
#else
	/* Work around a bug (?) in dlsym() which finds the symbols
	   globally, instead of inside the specified object.
	   Here we name the init func after the module's file name */
	InitModule init_module;
	char *init_module_name;
	const char *module_name, *p;
	int l;

	if (file[0] == '/') {
	    module_name = strrchr(file, '/') + 1;
	} else {
	    module_name = &file[0];
	}
	/* remove file suffix */
	p = index(module_name, '.');
	if (p != NULL) {
	    l = p - module_name;
	} else {
	    l = strlen(module_name);
	}
	/* build the symbol name  */
	init_module_name = (char *)xalloc(7+l);
	strcpy(init_module_name, "_init_");
	strncpy(init_module_name + 6, module_name, l);
	init_module_name[6+l] = '\0';

	init_module = (InitModule)dlsym(module, init_module_name);
	xfree(init_module_name);
#endif /* DLSYM_BUG */
#else
	InitModule init_module = (InitModule)dlsym(module, "init_module");
#endif

	if (init_module) {
	   if (!(*init_module)(XF86_VERSION_CURRENT)) {
		ErrorF("Warning: the module %s doesn't match server "
		       "version%s\n", file, XF86_VERSION);
	    }
	   else {
	       if (xf86Verbose)
		   if (file[0] == '/')
		       ErrorF("%s module %s successfully loaded\n",
			      XCONFIG_GIVEN, file);
		   else
		       ErrorF("%s module %s successfully loaded from %s\n",
			      XCONFIG_GIVEN, file, dir_elem);
	   }
	} else {
	    ErrorF("Unable to find init hook in module %s\n", file);
	    xfree(keep);
	    return NULL;
	}
    }
    
    xfree(keep);

    return module;
}

/* end of xf86dl.c */
