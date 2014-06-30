/*
 * $XConsortium: skyPriv.h,v 1.2 91/12/11 21:40:52 eswu Exp $
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute	this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both	that copyright notice and this permission notice appear	in
 * supporting documentation, and that the name of IBM not be
 * used	in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES	WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND 
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO	EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION	OF CONTRACT, NEGLIGENCE	OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN	CONNECTION WITH	THE USE	OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

#ifndef	SKYPRIV_H
#define	SKYPRIV_H

#include <sys/types.h>

extern unsigned long ddpGCPrivateIndex;

typedef enum _skyFillMode
{
	punt,
	fill_solid,
	fill_tile,
	fill_stipple
} skyFillMode;


typedef struct _skyStipple
{
    ulong	size;			/* Bytes alloc'ed to ``bits''  */
    char	*bits;			/* Pointer to stipple bits */
    ushort	width, height;
    ulong	fgPixel, bgPixel;
    int		packed:1;
    int		freeBits:1;
} skyStippleRec, *skyStipplePtr;


typedef struct _skyPrivGC
{
    skyFillMode		fillMode;	/* punt, solid, tile, stipple   */
    ushort		BgFgMix;	/* ALU for bg, fg pixels        */
    uint		tileStipID;	/* Unique TileStipple ID	*/
    skyStippleRec	stipple;	/* Stipple data			*/
} skyPrivGCRec, *skyPrivGCPtr;


#define skyGetGCPrivate(g) \
	((skyPrivGCPtr) (g)->devPrivates[ddpGCPrivateIndex].ptr)

#endif /* SKYPRIV_H */
