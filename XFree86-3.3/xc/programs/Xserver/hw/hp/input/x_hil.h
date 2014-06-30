#ifndef X_HIL_H
#define X_HIL_H
/* $XConsortium: x_hil.h,v 1.4 95/01/24 23:29:42 gildea Exp $ */
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


Copyright (c) 1988 by Hewlett-Packard Company

Permission to use, copy, modify, and distribute this software 
and its documentation for any purpose and without fee is hereby 
granted, provided that the above copyright notice appear in all 
copies and that both that copyright notice and this permission 
notice appear in supporting documentation, and that the name of 
Hewlett-Packard not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.

This software is not subject to any license of the American
Telephone and Telegraph Company or of the Regents of the
University of California.

*/
#include "hpext.h"

/* MAXHILEVENTS is the maximum number of X events that can
   be put on the events queue as the result of reading a 
   single HIL data packet.  The HIL definition is that 
   a packet may contain one motion event and up to 8 bytes
   of key data.  If the key device is a barcode reader in 
   ASCII mode, we translate each ASCII code into up to 6
   keycodes.  The maximum number of X events that can be 
   generated from a single HIL packet is therefore 49.

   MAX_EVENTS is the size of the server's internal queue of
   input events.  X input is a two-step process, with the 
   first step consisting of reading input events from the 
   device and putting them on this internal queue.  Later
   in the dispatch loop, that queue is emptied and all the
   events are routed by DIX to the appropriate clients.

   The size of the event queue is not as large as the 
   theoretical maximum, but motion events are compressed
   into a single event until a key or button is seen.

   The worst at all reasonable case is alternating key and
   motion data, which would result in less than 50 events.
   A more reasonable guess is 20 events per device.
   Our queue size therefore allows for the worst case on 
   5 - 10 devices simultaneously.
*/

#define MAXHILEVENTS		49
#define MOTION_BUFFER_SIZE	100
#define MAX_EVENTS		256
#define WR_EVENTS		MAX_EVENTS-1

struct	x11EventQueue
	{
	xHPEvent *events;
	int	size;
	int	head;
	int	tail;
	};		

#endif
