/* $XFree86: xc/programs/Xserver/hw/xfree86/common/xf86Lock.c,v 3.11 1997/01/18 06:55:34 dawes Exp $ */

/*
 * Explicit support for a server lock file like the ones used for UUCP.
 * For architectures with virtual terminals that can run more than one
 * server at a time.  This keeps the servers from stomping on each other
 * if the user forgets to give them different display numbers.
 */
/* $XConsortium: xf86Lock.c /main/9 1996/04/01 18:07:58 kaleb $ */

#define LOCK_PATH "/tmp/.X"
#define LOCK_TMPPATH "/tmp/.tX"
#define LOCK_SUFFIX "-lock"

#include "Xos.h"
#include <sys/stat.h>
#include <stdio.h>
#include "misc.h"

#ifdef _MINIX
#include <limits.h>	/* For PATH_MAX */
#endif

#ifdef __EMX__
#define link rename
#endif

#ifndef PATH_MAX
#ifndef Lynx
#include <sys/param.h>
#else
#include <param.h>
#endif
#ifndef PATH_MAX
#ifdef MAXPATHLEN
#define PATH_MAX MAXPATHLEN
#else
#define PATH_MAX 1024
#endif
#endif
#endif

#include <errno.h>
extern int errno;

#ifdef USE_XF86_SERVERLOCK
static Bool StillLocking = FALSE;

extern char* display;

/*
 * LockServer --
 *      Check if the server lock file exists.  If so, check if the PID
 *      contained inside is valid.  If so, then die.  Otherwise, create
 *      the lock file containing the PID.
 */
void
xf86LockServer()
{
#ifndef AMOEBA
  char tmp[PATH_MAX], lock[PATH_MAX], pid_str[12];
  int lfd, i, haslock, l_pid, t;

  /*
   * Path names
   */
#ifndef __EMX__
  (void) sprintf(tmp, "%s%s%s", LOCK_TMPPATH, display, LOCK_SUFFIX);
  (void) sprintf(lock, "%s%s%s", LOCK_PATH, display, LOCK_SUFFIX);
#else
  /* OS/2 uses TMP directory, must also prepare for 8.3 names */
  char *tmppath = getenv("TMP");
  if (!tmppath)
    FatalError("No TMP dir found\n", tmp);
  (void) sprintf(tmp, "%s/xf86$%s.lck",tmppath, display);
  (void) sprintf(lock, "%s/xf86_%s.lck",tmppath, display);
#endif

  /*
   * Create a temporary file containing our PID.  Attempt three times
   * to create the file.
   */
  StillLocking = TRUE;
  i = 0;
  do {
    i++;
    lfd = open(tmp, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (lfd < 0)
       sleep(2);
    else
       break;
  } while (i < 3);
  if (lfd < 0) {
    unlink(tmp);
    i = 0;
    do {
      i++;
      lfd = open(tmp, O_CREAT | O_EXCL | O_WRONLY, 0644);
      if (lfd < 0)
         sleep(2);
      else
         break;
    } while (i < 3);
  }
  if (lfd < 0)
    FatalError("Could not create lock file in %s\n", tmp);
  (void) sprintf(pid_str, "%10d\n", getpid());
  (void) write(lfd, pid_str, 11);
#ifndef __EMX__
#ifndef USE_CHMOD
  (void) fchmod(lfd, 0444);
#else
/* ISC prior Version 4.1 doesn't have fchmod :-( */
  (void) chmod(tmp, 0444);
#endif
#endif
  (void) close(lfd);

  /*
   * OK.  Now the tmp file exists.  Try three times to move it in place
   * for the lock.
   */
  i = 0;
  haslock = 0;
  while ((!haslock) && (i++ < 3)) {

    haslock = (link(tmp,lock) == 0);
    if (haslock) {
      /*
       * We're done.
       */
      break;
    }
    else {
      /*
       * Read the pid from the existing file
       */
      lfd = open(lock, O_RDONLY);
      if (lfd < 0) {
        unlink(tmp);
        FatalError("Can't read lock file %s\n", lock);
      }
      pid_str[0] = '\0';
      if (read(lfd, pid_str, 11) != 11) {
        /*
         * Bogus lock file.
         */
        unlink(lock);
        close(lfd);
        continue;
      }
      pid_str[11] = '\0';
      sscanf(pid_str, "%d", &l_pid);
      close(lfd);

      /*
       * Now try to kill the PID to see if it exists.
       */
      errno = 0;
      t = kill(l_pid, 0);
      if ((t< 0) && (errno == ESRCH)) {
        /*
         * Stale lock file.
         */
        unlink(lock);
        continue;
      }
      else if (((t < 0) && (errno == EPERM)) || (t == 0)) {
        /*
         * Process is still active.
         */
        unlink(tmp);
	FatalError("Server is already active for display %s\n%s %s\n%s\n",
		   display, "\tIf this server is no longer running, remove",
		   lock, "\tand start again.");
      }
    }
  }
  unlink(tmp);
  if (!haslock)
    FatalError("Could not create server lock file: %s\n", lock);
  StillLocking = FALSE;
#endif /* !AMOEBA */
}

/*
 * Unlock_Server --
 *      Remove the server lock file.
 */
void
xf86UnlockServer()
{
#ifndef AMOEBA
  char buf[PATH_MAX];

  if (StillLocking)
    return;
  (void)sprintf(buf, "%s%s%s", LOCK_PATH, display, LOCK_SUFFIX);
  (void) unlink(buf);
#endif
}
#endif /* USE_XF86_SERVERLOCK */
