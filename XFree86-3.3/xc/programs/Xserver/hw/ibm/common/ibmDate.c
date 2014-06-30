/*
 * $XConsortium: ibmDate.c,v 1.2 91/07/16 13:05:43 jap Exp $
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

extern void ibmInfoMsg() ;

#ifndef DATE 
#define DATE ""
#endif
#ifndef DEVELOPER
#define DEVELOPER ""
#endif
#ifndef HOST
#define HOST ""
#endif

void
ibmPrintBuildDate()
{
static char buildDate[] = DATE ;
static char buildDeveloper[] = DEVELOPER ;
static char buildHost[] = HOST ;

ibmInfoMsg( "Server linked %s by %s on %s \n",
	    buildDate, buildDeveloper, buildHost );
return ;
}
