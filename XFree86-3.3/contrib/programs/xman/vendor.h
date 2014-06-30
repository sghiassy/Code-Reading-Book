/* $XConsortium: vendor.h,v 1.12 94/04/17 20:44:00 rws Exp $ */
/* $XFree86: contrib/programs/xman/vendor.h,v 3.3 1995/07/15 14:11:20 dawes Exp $ */
/*

Copyright (c) 1991  X Consortium

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

/* Vendor-specific definitions */

#ifdef CSRG_BASED
#include <sys/param.h>
#endif

#define SUFFIX "suffix"
#define FOLD "fold"
#define FOLDSUFFIX "foldsuffix"
#define MNULL 0
#define MSUFFIX 1
#define MFOLD 2
#define MFOLDSUFFIX 3

/*
 * The directories to search.  Assume that the manual directories are more
 * complete than the cat directories.
 */

#if ( defined(UTEK) || defined(apollo) || defined(Lynx) )
#  define SEARCHDIR  CAT
#else
#  define SEARCHDIR  MAN
#endif

#if ( defined(sgi) || (defined(i386) && (defined(SYSV) || defined(SVR4))) || (BSD >= 199103) || defined(linux) )
# define SEARCHOTHER CAT
#endif

#if (BSD >= 199103)
# define MANCONF "/etc/man.conf"
#endif

/*
 * The default manual page directory.
 *
 * The MANPATH enviornment variable will override this.
 */

#ifndef SYSMANPATH

#ifdef macII
#  define SYSMANPATH "/usr/catman/u_man:/usr/catman/a_man"
#endif /* macII */
#if defined(SVR4) || defined(__osf__) || (BSD >= 199103)
#  define SYSMANPATH "/usr/share/man"
#endif /* SVR4 || __osf__ || (BSD >= 199103) */
#ifdef hcx
#  define SYSMANPATH "/usr/catman/local_man:/usr/catman/u_man:/usr/catman/a_man:/usr/catman/p_man:/usr/catman/ada_man"
#endif /* hcx */
#if defined(SYSV) && defined(i386) && !defined(SCO)
#  define SYSMANPATH "/usr/catman/u_man:/usr/catman/p_man"
#endif /* SYSV386 */
#ifdef sgi
#  define SYSMANPATH "/usr/catman/a_man:/usr/catman/g_man:/usr/catman/p_man:/usr/catman/u_man:/usr/man/p_man:/usr/man/u_man:/usr/man"
#endif /* sgi */
#ifdef __bsdi__
#  define SYSMANPATH "/usr/share/man:/usr/contrib/man:/usr/contrib/isode/man:/usr/local/man"
#endif /* __bsdi__ */

#ifndef SYSMANPATH
#  define SYSMANPATH "/usr/man"
#endif

#endif

/*
 * Compression Definitions.
 */

#if defined( macII ) || defined( hcx ) || (defined(SYSV) && defined(i386) && !defined(ISC) && !defined(SCO)) || defined(sgi)
#  define COMPRESSION_EXTENSION   "z"
#  define UNCOMPRESS_FORMAT       "pcat %s > %s"
#  define NO_COMPRESS		/* mac can't handle using pack as a filter and
				   xman needs it to be done that way. */
#else
#  ifdef UTEK
#    define COMPRESSION_EXTENSION "C"
#    define UNCOMPRESS_FORMAT     "ccat < %s > %s"
#    define COMPRESS              "compact"
#  else
#    if defined (ISC) || defined(SCO)
#      define COMPRESSION_EXTENSION   "Z"     /* dummy */
#      ifndef SCO
#        define COMPRESSION_EXTENSIONS  "zZF" /* pack, compress, freeze */
#      else
#        define COMPRESSION_EXTENSIONS  "zZ"  /* pack, compress */
#      endif
#      define UNCOMPRESS_FORMAT       uncompress_format
#      define UNCOMPRESS_FORMAT_1     "pcat %s > %s"
#      define UNCOMPRESS_FORMAT_2     "zcat < %s > %s"
#      define UNCOMPRESS_FORMAT_3     "fcat < %s > %s"
#      define NO_COMPRESS
#    else
#      define COMPRESSION_EXTENSION "Z"
#      define UNCOMPRESS_FORMAT     "zcat < %s > %s"
#      define COMPRESS              "compress"
#      define GZIP_EXTENSION "gz"
#      define GUNZIP_FORMAT "gzip -c -d < %s > %s"
#      define GZIP_COMPRESS "gzip"
#    endif /* ISC */
#  endif /* UTEK */
#endif /* macII, hcx, SYSV386, sgi */



/*
 * The command filters for the manual and apropos searches.
 */

#if ( defined(hpux) || defined(macII) || defined(CRAY) || defined(ultrix) || defined(hcx) )
#  define NO_MANPATH_SUPPORT
#endif

#ifdef NO_MANPATH_SUPPORT
#  define APROPOS_FORMAT ("man -k %s | pr -h Apropos >> %s")
#else
#  define APROPOS_FORMAT ("man -M %s -k %s | pr -h Apropos > %s")
#endif

#ifdef ultrix
#  define FORMAT "| nroff -man"             /* The format command. */
#else
#  if (BSD >= 199103)
#    define FORMAT "| eqn | tbl | nroff -man"
#  else
#    ifdef linux
#      define linux_GROFF             /* undef this to use nroff instead */
#    endif /* linux */
#    ifdef linux_GROFF
#      define FORMAT "| geqn | gtbl | groff -Tascii -man"
#    else
#      define FORMAT "| neqn | nroff -man"      /* The format command. */
#    endif /* linux_GROFF */
#  endif
#endif

/*
 * Names of the man and cat dirs.
 */

#ifdef __bsdi__
#define MAN "cat"
#else
#define MAN "man"
#endif

#if ( defined(macII) || defined(CRAY) || defined(hcx) || (defined(SYSV) && defined(i386)) )
/*
 * The Apple, Cray,, SYSV386, and HCX folks put the preformatted pages in the
 * "man" directories.
 */
#  ifdef SCO
#    define CAT "cat."
#  else
#    define CAT MAN
#  endif
#else
#  define CAT "cat"
#endif

extern void AddStandardSections();
extern void AddNewSection();

typedef struct _SectionList {
  struct _SectionList * next;
  char * label;			/* section label */
  char * directory;		/* section directory */
  int flags;
} SectionList;
