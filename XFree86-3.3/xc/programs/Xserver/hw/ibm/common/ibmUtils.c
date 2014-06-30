/*
 * $XConsortium: ibmUtils.c,v 1.5 93/09/26 12:39:35 rws Exp $
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND 
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

#include "X.h"
#include "cursorstr.h"
#include "miscstruct.h"
#include "scrnintstr.h"

#include "ibmScreen.h"
#include "OSio.h"
#include "ibmTrace.h"

#ifdef TRACE_X
int	ibmTrace ;
int	ibmTraceSync ;
#endif

int ibmQuietFlag = 0 ;

void
ibmInfoMsg( str, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 )
char *str ;
{
if ( !ibmQuietFlag )
	(void) ErrorF( str, arg1, arg2, arg3, arg4, arg5,
		       arg6, arg7, arg8, arg9 ) ;
return ;
}


/***==================================================================***/

void
ddxGiveUp()
{

    TRACE(("ddxGiveUp()"));

#ifdef OS_CapsLockFeedback
    OS_CapsLockFeedback(0);	
#endif

    OS_GiveUp();

    return;
}

/*
 * abort the server when someone calls through a bogus pointer, which
 * was usually initialized by the structure assignment method (xxxData.c)
 */

void
ibmAbort()
{
extern void abort() ;

	ErrorF(
   "ibmAbort: invalid call though function pointer\n... FATAL exiting...\n" ) ;
	abort() ;
	/*NOTREACHED*/
}

/***==================================================================***/

void
AbortDDX()
{
    TRACE(("AbortDDX()"));

#ifdef OS_CapsLockFeedback
    OS_CapsLockFeedback(0) ;
#endif

    OS_Abort();
    return;
}

ddxUseMsg()
{
int	i;

    TRACE(("ddxUseMsg()\n"));
#ifdef AIXV3
    AIXUseMsg();
#else
    ErrorF("    -D string	color database file \n");
    ErrorF("    -c #		key-click volume (0-100)\n");
    ErrorF("Recognized screens are:\n");
    ErrorF("    -all		opens all attached, supported screens\n");
    for (i=0;(ibmPossibleScreens[i]!=NULL);i++) {
	ErrorF("    %s\n",ibmPossibleScreens[i]->ibm_ScreenFlag);
    }
    ErrorF("Other device dependent options are:\n");
    ErrorF("    -bs		turn on backing store for windows that request it\n");
    ErrorF("    -D <file>	specify color database file\n");
#if defined(IBM_GSL)
    ErrorF("    -dd <dev>	specify a default display for GSL\n");
#endif /* defined(IBM_GSL) */
#ifdef IBM_SPECIAL_MALLOC
    ErrorF("    -malloc #	set malloc check level (0-5)\n");
#endif
    ErrorF("    -n :num		specify connection number (0-255)\n");
    ErrorF("    -nobs		always deny backing store\n");
    ErrorF("    -nohdwr		use generic functions where applicable\n");
    ErrorF("    -pckeys		swap CAPS LOCK and CTRL (for touch typists)\n");
#ifdef IBM_SPECIAL_MALLOC
    ErrorF("    -plumber string	dump malloc arena to named file\n");
#endif
    ErrorF("    -quiet		do not print informational messages\n");
    ErrorF("    -rtkeys		use CAPS LOCK and CTRL as labelled\n");
    ErrorF("    -T		disable CTRL-ALT_BACKSPACE key sequence\n");
#ifdef TRACE_X
    ErrorF("    -trace		trace execution of IBM specific functions\n");
#endif
    ErrorF("    -verbose	print informational messages\n");
    ErrorF("    -wrap		wrap mouse in both dimensions\n");
    ErrorF("    -wrapx		wrap mouse in X only\n");
    ErrorF("    -wrapy		wrap mouse in Y only\n");
    ErrorF("See Xibm(1) for a more complete description\n");
#endif
}

/***==================================================================***/

#ifdef IBM_SPECIAL_MALLOC
#include <stdio.h>
#include <signal.h>

int	ibmShouldDumpArena= 0;
static	char	*ibmArenaFile= 0;

static
ibmMarkDumpArena(sig)
    int sig;
{

    (void) OsSignal( SIGUSR1, ibmMarkDumpArena ) ;
    ibmShouldDumpArena= 1;

    return 0;
}

ibmDumpArena(sig)
    int sig;
{
FILE  *mfil;
static	char	fileName[100];
static	int	dumpNum= 0;

   (void) OsSignal( SIGUSR2, ibmDumpArena ) ;
   (void) sprintf(fileName,ibmArenaFile,dumpNum++);

   mfil= fopen(fileName,"a");
   if (!mfil) {
	ErrorF("Couldn't open %s to dump arena, ignored\n",fileName);
	return 0 ;
   }
   else {
	ErrorF("Dumping malloc arena to %s\n",fileName);
	plumber(mfil);
	(void) fflush(mfil);
	(void) fclose(mfil);
   }
   ibmShouldDumpArena= 0;
   return 1 ;
}

ibmNoteHit(sig)
    int sig;
{
static int old= 4;

   ErrorF("received SIGTERM\n");
   old= SetMallocCheckLevel(old);
   return 1 ;
}

int
ibmSetupPlumber( name )
register char *name ;
{
extern exit() ;

    ibmInfoMsg( "Setting up plumber to dump to %s\n", name ) ;
    (void) unlink( ibmArenaFile = name ) ;
    (void) OsSignal( SIGUSR1, ibmMarkDumpArena ) ;
    (void) OsSignal( SIGUSR2, ibmDumpArena ) ;
    (void) OsSignal( SIGTERM, ibmNoteHit ) ;
    return 1 ;
}

#endif /* IBM_SPECIAL_MALLOC */
