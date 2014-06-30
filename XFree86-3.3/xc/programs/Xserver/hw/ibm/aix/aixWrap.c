/*
 * $XConsortium: aixWrap.c /main/4 1995/12/05 15:44:00 matt $
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

#ifndef _IBM_LFT
#include <sys/hft.h>
#endif

#define NEED_EVENTS
#define NEED_REPLIES
#include "X.h"
#include "Xproto.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"
#include "aixInput.h"
#include "input.h"
#include "inputstr.h"

#include "compiler.h"

#include "ibmIO.h"
#include "ibmMouse.h"
#include "ibmKeybd.h"
#include "ibmScreen.h"
#include "ibmTrace.h"

#include "OSio.h"
#ifndef _IBM_LFT
#include "hftQueue.h"
#endif


#define FIELDWIDTH	(10)
#define FIELDHEIGHT	(10)

typedef struct {
	int  x,y;
	int  connected;
} wrap_entry;

static wrap_entry  entry_list[MAXSCREENS];

static int  field[FIELDWIDTH][FIELDHEIGHT];

static int  need_to_init = 1;

static void
aixInitWrap()
{
    int  i,x,y;

    if( ! need_to_init ){
	ErrorF("aix Wrap already initialized\n");
	exit(0);
    }

    for(i=0;i<MAXSCREENS;i++){
	entry_list[i].x = -1;
    }
    for(x=0;x<FIELDWIDTH;x++){
	for(y=0;y<FIELDHEIGHT;y++){
	    field[x][y] = -1;
	}
    }

    need_to_init = 0;
}

/*
	Return the screen index of the next screen.
	Return -1 if there is no next screen.  (Stick the cursor
	on the screen edge.)
	Return the same index if wrap to the other side of the
	same screen.
*/
int
aixWrapUp(index)
    int  index;
{
    int  x, y;

    if( need_to_init ){
	ErrorF("aix Wrap not initialized\n");
	exit(0);
    }

    x = entry_list[index].x;
    y = entry_list[index].y;
    y++;
    while( y < FIELDHEIGHT ){
	if( field[x][y] >= 0 )
	    return(field[x][y]);
	y++;
    }
    if( ! ibmYWrapScreen )
	return -1;
    y = 0;
    while( y < entry_list[index].y ){
	if( field[x][y] >= 0 )
	    return(field[x][y]);
	y++;
    }
    return(index);
}

/*
	Return the screen index of the next screen.
	Return -1 if there is no next screen.  (Stick the cursor
	on the screen edge.)
	Return the same index if wrap to the other side of the
	same screen.
*/
int
aixWrapDown(index)
    int  index;
{
    int  x, y;

    if( need_to_init ){
	ErrorF("aix Wrap not initialized\n");
	exit(0);
    }

    x = entry_list[index].x;
    y = entry_list[index].y;
    y--;
    while( y >= 0 ){
	if( field[x][y] >= 0 )
	    return(field[x][y]);
	y--;
    }
    if( ! ibmYWrapScreen )
	return -1;
    y = FIELDHEIGHT-1;
    while( y > entry_list[index].y ){
	if( field[x][y] >= 0 )
	    return(field[x][y]);
	y--;
    }
    return(index);
}

/*
	Return the screen index of the next screen.
	Return -1 if there is no next screen.  (Stick the cursor
	on the screen edge.)
	Return the same index if wrap to the other side of the
	same screen.
*/
int
aixWrapLeft(index)
    int  index;
{
    int  x, y;

    if( need_to_init ){
	ErrorF("aix Wrap not initialized\n");
	exit(0);
    }

    x = entry_list[index].x;
    y = entry_list[index].y;
    x--;
    while( x >= 0 ){
	if( field[x][y] >= 0 )
	    return(field[x][y]);
	x--;
    }
    if( ! ibmXWrapScreen )
	return -1;
    x = FIELDWIDTH-1;
    while( x > entry_list[index].x ){
	if( field[x][y] >= 0 )
	    return(field[x][y]);
	x--;
    }
    return(index);
}

/*
	Return the screen index of the next screen.
	Return -1 if there is no next screen.  (Stick the cursor
	on the screen edge.)
	Return the same index if wrap to the other side of the
	same screen.
*/
int
aixWrapRight(index)
    int  index;
{
    int  x, y;

    if( need_to_init ){
	ErrorF("aix Wrap not initialized\n");
	exit(0);
    }

    x = entry_list[index].x;
    y = entry_list[index].y;
    x++;
    while( x < FIELDWIDTH ){
	if( field[x][y] >= 0 )
	    return(field[x][y]);
	x++;
    }
    if( ! ibmXWrapScreen )
	return -1;
    x = 0;
    while( x < entry_list[index].x ){
	if( field[x][y] >= 0 )
	    return(field[x][y]);
	x++;
    }
    return(index);
}

void
aixPutScreenAt(index,x,y)
    int  index, x, y;
{
    if( need_to_init )
	aixInitWrap();

    if( entry_list[index].x >= 0 ){
	ErrorF("aixPutScreenAt() screen %d already has a postion %d %d\n",field[x][y],entry_list[index].x,entry_list[index].y);
	return;
    }

    if( field[x][y] < 0 ){
	field[x][y] = index;
	entry_list[index].x = x;
	entry_list[index].y = y;
    }
    else{
	ErrorF("aixPutScreenAt() Position %d %d already in use by screen %d\n",x,y,field[x][y]);
	while( field[x][y] >= 0 ){
	    x++;
	    if( x > 9 ){
		x = 0;
		y++;
		if( y > 9 ){
		    y = 0;
		}
	    }
	}
	field[x][y] = index;
	entry_list[index].x = x;
	entry_list[index].y = y;
	ErrorF("aixPutScreenAt() moved %d to position %d %d instead.\n",index,x,y);
    }
}

static int
isConnected(i)
    int  i;
{
    int  nexti;

    if( entry_list[i].connected == -1 ){
	return(FALSE);
    }
    if( entry_list[i].connected )
	return(TRUE);
    entry_list[i].connected = -1;

    if( (nexti=aixWrapLeft(i)) >= 0 ){
	if( isConnected(nexti) )
	    return(TRUE);
    }
    if( (nexti=aixWrapRight(i)) >= 0 ){
	if( isConnected(nexti) )
	    return(TRUE);
    }
    if( (nexti=aixWrapUp(i)) >= 0 ){
	if( isConnected(nexti) )
	    return(TRUE);
    }
    if( (nexti=aixWrapDown(i)) >= 0 ){
	if( isConnected(nexti) )
	    return(TRUE);
    }
    entry_list[i].connected = 0;
    return(FALSE);
}

static int  connectedrow[FIELDHEIGHT];
static int  connectcount;

static void
markConnected(i)
    int  i;
{
    int  nexti;

    if( entry_list[i].connected )
	return;

    entry_list[i].connected = TRUE;
    connectcount++;
    connectedrow[entry_list[i].y] = TRUE;

    if( (nexti=aixWrapLeft(i)) >= 0 ){
	markConnected(nexti);
    }
    if( (nexti=aixWrapRight(i)) >= 0 ){
	markConnected(nexti);
    }
    if( (nexti=aixWrapUp(i)) >= 0 ){
	markConnected(nexti);
    }
    if( (nexti=aixWrapDown(i)) >= 0 ){
	markConnected(nexti);
    }
}
/*
	Make sure all screens can be reached.
*/
void
aixConnectScreens()
{
    int  i,x,y, screencount;
    int  x2,y2;

    if( need_to_init ){
	ErrorF("aix Wrap not initialized\n");
	exit(0);
    }

    screencount = 0;
    for(i=0;i<MAXSCREENS;i++){
	if( entry_list[i].x >= 0 ){
	    screencount++;
	}
    }
    for(x=0;x<FIELDWIDTH;x++){
	for(y=0;y<FIELDHEIGHT;y++){
	    if( field[x][y] >= 0 ){
	    }
	}
    }
    
    if( screencount < 2 )
	return;

    for(y=0;y<FIELDHEIGHT;y++){
	connectedrow[y] = FALSE;
    }
    for(i=0;i<MAXSCREENS;i++){
	entry_list[i].connected = FALSE;
    }
    connectcount = 0;

/*
	Mark the first one as connected, then
	find the screens connected to it.
*/
    for(i=0;i<MAXSCREENS;i++){
	if( entry_list[i].x >= 0 ){
	    markConnected(i);
	    break;
	}
    }

    if( connectcount == screencount ){
	TRACE(("screens are connected\n"));
	return;
    }

    TRACE(("some screens are not connected\n"));

/*
	Re-connect by moving screens down, or up.
*/
    for(;i<MAXSCREENS;i++){
	if( entry_list[i].x >= 0 && !(entry_list[i].connected) ){
		/* see if some other change has made this connected */
	    if( isConnected(i) ){
		markConnected(i);
	    }
	    else{
			/* try to move it down */
		x2 = entry_list[i].x;
		for(y2=entry_list[i].y-1; y2 >=0 ; y2--){
		    if( field[x2][y2] >= 0 ){
			/* this spot already taken */
			break;
		    }
		    if( connectedrow[y2] ){
			/* move screen to this row */
			TRACE(("moving screen %d from (%d,%d) to (%d,%d)\n",i,entry_list[i].x,entry_list[i].y,x2,y2));
			field[x2][y2] = i;
			field[entry_list[i].x][entry_list[i].y] = -1;
			entry_list[i].x = x2;
			entry_list[i].y = y2;
			markConnected(i);
			break;
		    }
		}

			/* we were able to move it down */
		if( entry_list[i].connected ){
		    continue;
		}

			/* can't move down, so try to move it up */
		for(y2=entry_list[i].y+1; y2 < FIELDHEIGHT; y2++){
		    if( field[x2][y2] >= 0 ){
			/* this spot already taken */
			break;
		    }
		    if( connectedrow[y2] ){
			/* move screen to this row */
			TRACE(("moving screen %d from (%d,%d) to (%d,%d)\n",i,entry_list[i].x,entry_list[i],y,x2,y2));
			field[x2][y2] = i;
			field[entry_list[i].x][entry_list[i].y] = -1;
			entry_list[i].x = x2;
			entry_list[i].y = y2;
			markConnected(i);
			break;
		    }
		}

		TRACE(("couldn't reconnect screen %d by moving it\n",i));

	    }
	}
    }

    if( connectcount == screencount ){
	TRACE(("screens are connected\n"));
	return;
    }

    TRACE(("some screens couldn't be connected\n"));
}
