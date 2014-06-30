/*
 * $XConsortium: skySave.c,v 1.4 91/12/11 21:25:50 eswu Exp $
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

/*
 * skySave.c - save and restore functions
 *             copied from gaiStub.c and gaiInit.c
 */

#include <sys/types.h>
#include <sys/rcm_win.h>
#include <sys/aixgsc.h>

#include "X.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "screenint.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "cursorstr.h"
#include "scrnintstr.h"
#include "dixfontstr.h"

#include "skyProcs.h"
#include "ibmTrace.h"

#include "ibmScreen.h"
extern Bool skyDisplayCursor();

extern unsigned int skyHandle ;

void
skySaveState(pScreen,saveBits)
ScreenPtr       pScreen;
int             saveBits;
{
    int         index ;

    TRACE(("skySaveState(0x%x,%d)\n",pScreen,saveBits));

    index       = pScreen->myNum ;

}

void
skyRestoreState(pScreen,restoreBits)
ScreenPtr       pScreen;
int             restoreBits;
{
    int         index ;

    TRACE(("skyRestoreState(0x%x,%d)\n",pScreen,restoreBits));

    index       = pScreen->myNum ;

    skyHdwrInit(index);
    skyRefreshColormaps(pScreen);
    skyDisplayCursor( pScreen, ibmCurrentCursor(index) );
}
