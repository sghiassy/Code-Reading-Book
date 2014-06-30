/*
 * $XConsortium: hftQueue.h,v 1.3 91/11/22 17:07:03 eswu Exp $
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

#ifndef HFTQUEUE_H
#define HFTQUEUE_H

	/*
	 *	Dispatches events from the hft queue to handlers appropriate
	 *	to the various devices.
	 *
	 *	int
	 *	hftInitQueue()
	 *		Initializes the hft queue.  
	 *		returns TRUE for success, FALSE on failure (queue
	 *		already initialized or can't open HFT).
	 *	
	 *	hftTermQueue()
	 *		Terminates the hft queue.  
	 *
	 *	hftInterruptAlways()
	 *		causes the queue to interrupt every time an event 
	 *		arrives	present.
	 *
	 *	hftInterruptOnFirst()
	 *		causes the queue to interrupt only when an event
	 *		is added to an empty queue.
	 *
	 *	hftEvHandler
	 *	hftInstallHandler(evType,pHandler)
	 *	int	 	evType;
	 *	hftEvHandler	pHandler;
	 *		Installs handler for events of type 'evType.'
	 *		evType should be one of HFT_LOCATOR, HFT_TABLET,
	 *		HFT_KEYBOARD,HFT_LPFK,HFT_DIAL,HFT_ADAPT, HFT_GRANTED, 
	 *		HFT_RETRACTED, 	HFT_BEFORE_EVENTS, HFT_AFTER_EVENTS, 
	 *		HFT_EVENT_ARRIVED.  Each handler receives an
	 *		hftEvent.  To remove a handler, specify HFT_IGNORE.
	 *		hftInstallDevice returns the previous handler, or
	 *		HFT_ERROR if an error occured (queue not initialized,
	 *		bad argument).
	 *
	 *	void
	 *	hftAddTimeout(pHandler,length)
	 *		Installs a timeout handler.  pHandler will be called
	 *		after 'length' iterations through hftDispatchEvents.
	 *
	 *	void
	 *	hftDispatchEvents()
	 *		Dispatches events from hft queue until queue is
	 *		exhausted or one of the translator functions
	 *		returns FALSE.  An event is dispatched to it's
	 *		device specific handler if one is defined, or to the 
	 *		handler for HFT_ANY if it exists.  If neither handler
	 *		is defined, the event is ignored.
	 *
	 *	int
	 *	hftFD()
	 *		File descriptor (if any) which may be selected
	 *		to determine if input is available on the hft
	 *		queue.
	 */

#define	HFT_KEYBOARD		0
#define	HFT_LOCATOR		1
#define	HFT_ADAPT		2
#define	HFT_TABLET		3
#define	HFT_LPFK		4
#define	HFT_DIAL		5
#define	HFT_GRANTED		6
#define	HFT_RETRACTED		7
#define	HFT_EVENT_ARRIVED	8
#define	HFT_BEFORE_EVENTS	9
#define	HFT_AFTER_EVENTS	10
#define	HFT_NEVENTS		11

#define	HFT_IGNORE		((hftEvHandler)0)
#define	HFT_ERROR		((hftEvHandler)-1)

/* THIS STRUCTURE SHOULD MATCH "struct hfunxlate" IN "sys/hft.h" */

typedef	struct _HFTKEYEV {
	char	kePos;
	char	keScan;
	char	keStatus[2];
#ifdef AIXV3
	char	keSeconds[3];
	char	keSixtyths;
#endif
} hftKeyEv;

/* MUST BE THE SAME AS /usr/include/sys/hft.h */

#ifdef AIXV3     /* HACK ALERT */
#  define	HFT_LBUTTON	0x80
#  define	HFT_RBUTTON	0x20
#  define	HFT_MBUTTON	0x40
#else
#  define	HFT_LBUTTON	0x80
#  define	HFT_RBUTTON	0x40
#  define	HFT_MBUTTON	0x20
#endif

#define	HFT_BUTTONS	(HFT_LBUTTON|HFT_RBUTTON)

#define	HFT_BUTTON1	0x80
#define	HFT_BUTTON2	0x40
#define	HFT_BUTTON3	0x20
#define	HFT_BUTTON4	0x10
#define	HFT_TABLET_BUTTONS (HFT_BUTTON1|HFT_BUTTON2|HFT_BUTTON3|HFT_BUTTON4)

#define HF_SHORT(f)		((signed short)((f)[0]<<8|(f)[1]))
#define	HF_SETSHORT(f,v)	(((f)[1]=(v)&0xff),(f[0]=(v)>>8))

typedef struct _HFTLOCEV {
	short	leDeltaX;
	short	leDeltaY;
	char	leSeconds[3];
	char	leSixtieths;
	char	leButtons;
	char	leStype;
} hftLocEv;

typedef struct _HFTLPFKEV {
	short	lpfkeKeyNo;
	short	lpfkeReserved;
	char	lpfkeSeconds[3];
	char	lpfkeSixtieths;
	char	lpfkeButtons;
	char	lpfkeStype;
} hftLPFKEv;

typedef struct _HFTDIALEV {
	short	deDialNo;
	short	deDelta;
	char	deSeconds[3];
	char	deSixtieths;
	char	deButtons;
	char	deStype;
} hftDialEv;

typedef struct _HFTADPEV {
	char	aeQueue;
	char	aeData[20];
} hftAdaptEv;

typedef	struct {
	int	hftEvType;
	union {
	    hftKeyEv	hftKey;
	    hftLocEv	hftLoc;
	    hftAdaptEv	hftAdapt;
	    hftDialEv	hftDial;
	    hftLPFKEv	hftLpfk;
	} hftEv;
} hftEvent;

#ifdef AIXV3
typedef	void	(*hftEvHandler)();
#else
typedef	int	(*hftEvHandler)();
#endif

extern	int		 hftQFD;
extern	volatile unsigned hftPending;
extern	volatile unsigned hftGrantPending;
extern	volatile unsigned hftRetractPending;
extern	void		 hftInterruptAlways();
extern	void		 hftInterruptOnFirst();
extern	int		 hftInitQueue();
extern	void		 hftTermQueue();
extern	void		 hftAddTimeout();
extern	hftEvHandler	 hftInstallHandler();
extern	void		 hftDispatchEvents();
extern	int		 hftFD();

#endif /* HFTQUEUE_H */
