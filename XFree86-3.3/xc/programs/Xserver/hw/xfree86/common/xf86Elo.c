/* $XConsortium: xf86Elo.c /main/13 1996/10/25 14:11:31 kaleb $ */
/*
 * Copyright 1995 by Patrick Lecoanet, France. <lecoanet@cenaath.cena.dgac.fr>       
 *                                                                            
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is  hereby granted without fee, provided that
 * the  above copyright   notice appear  in   all  copies and  that both  that
 * copyright  notice   and   this  permission   notice  appear  in  supporting
 * documentation, and that   the  name of  Patrick  Lecoanet not  be  used  in
 * advertising or publicity pertaining to distribution of the software without
 * specific,  written      prior  permission.     Patrick Lecoanet   makes  no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.                   
 *                                                                            
 * PATRICK LECOANET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT  SHALL PATRICK LECOANET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA  OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS  ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/common/xf86Elo.c,v 3.19 1996/12/23 06:43:23 dawes Exp $ */

/*
 *******************************************************************************
 *******************************************************************************
 *
 * This driver is able to deal with Elographics SmartSet serial controllers.
 * It uses only a subset of the functions provided through the protocol.
 *
 *******************************************************************************
 *******************************************************************************
 */

/*
 *******************************************************************************
 *******************************************************************************
 *
 * TODO list and BUGS -- Sun Dec  3 19:28:04 1995
 *
 *	- Second pass over the code needed to suppress "features" that prevent
 *	  using the driver for several similar devices. Hey!! you know, you can
 *	  have more than one screen, so you can have more than one touchscreen.
 *	  Examples of this are: Configuration mecanism can only do for one
 *	  touchscreen, static structures are used to store local device infos,
 *	  etc. This will require some work to be done on other modules as well,
 *	  especially in xf86Xinput.c ;-).
 *
 *	- Need to remember the clients that have this device open. The close
 *	  should be done automatically when the last registered client give up.
 *	  This will require some fixes as far as in OS so that when a client
 *	  connection is closed the input module is given a chance to clean up.
 *
 *******************************************************************************
 *******************************************************************************
 */

#include "Xos.h"
#include <signal.h>

#define	 NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "inputstr.h"
#include "scrnintstr.h"
#include "XI.h"
#include "XIproto.h"

#if defined(sun) && !defined(i386)
#include <errno.h>
#include <termio.h>
#include <fcntl.h>
#include <ctype.h>

#include "extio.h"
#else
#include "compiler.h"
#include "xf86.h"
#include "xf86Procs.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"
#include "xf86Xinput.h"
#include "xf86Version.h"
#endif

#if !defined(sun) || defined(i386)
#include "os.h"
#include "osdep.h"
#include "exevents.h"

#include "extnsionst.h"
#include "extinit.h"
#endif

#if !defined(sun) || defined(i386)
/*
 ***************************************************************************
 *
 * Configuration descriptor.
 *
 ***************************************************************************
 */
#define PORT		1
#define ELO_DEVICE_NAME	2
#define SCREEN_NO	3
#define UNTOUCH_DELAY	4
#define REPORT_DELAY	5
#define MAXX		6
#define MAXY		7
#define MINX		8
#define MINY		9
#define DEBUG_LEVEL     10
#define HISTORY_SIZE	11
#define LINK_SPEED	12
#define ALWAYS_CORE	13

static SymTabRec EloTab[] = {
  { ENDSUBSECTION,     "endsubsection" },
  { PORT,              "port" },
  { ELO_DEVICE_NAME,   "devicename" },
  { SCREEN_NO,	       "screenno" },
  { UNTOUCH_DELAY,     "untouchdelay" },
  { REPORT_DELAY,      "reportdelay"},
  { MAXX,              "maximumxposition" },
  { MAXY,              "maximumyposition" },
  { MINX,              "minimumxposition" },
  { MINY,              "minimumyposition" },
  { DEBUG_LEVEL,       "debuglevel" },
  { HISTORY_SIZE,      "historysize" },
  { LINK_SPEED,        "linkspeed" },
  { ALWAYS_CORE,       "alwayscore" },
  { -1,                "" },
};

#define LS300		1
#define LS1200		2
#define LS2400		3
#define LS9600		4
#define LS19200		5

static SymTabRec LinkSpeedTab[] = {
  { LS300,	"b300" },
  { LS1200,	"b1200" },
  { LS2400,	"b2400" },
  { LS9600,	"b9600" },
  { LS19200,	"b19200" }
};
#endif

/*
 * This struct connects a line speed with
 * a compatible motion packet delay. The
 * driver will attempt to enforce a correct
 * delay (according to this table) in order to
 * avoid losing data in the touchscreen controller.
 * LinkSpeedValues should be kept in sync with
 * LinkSpeedTab.
 */
typedef struct {
  int	speed;
  int	delay;
} LinkParameterStruct;

static LinkParameterStruct	LinkSpeedValues[] = {
  { B300, 64 },
  { B1200, 16 },
  { B2400, 8 },
  { B9600, 4 },
  { B19200, 2 }
};


/*
 ***************************************************************************
 *
 * Default constants.
 *
 ***************************************************************************
 */
#define ELO_MAX_TRIALS	3		/* Number of timeouts waiting for a	*/
					/* pending reply.			*/
#define ELO_MAX_WAIT		100000	/* Max wait time for a reply		*/
#define ELO_UNTOUCH_DELAY	10	/* 100 ms				*/
#define ELO_REPORT_DELAY	4	/* 40 ms or 25 motion reports/s		*/
#define ELO_LINK_SPEED		B9600	/* 9600 Bauds				*/
#define ELO_PORT		"/dev/ttyS1"


/*
 ***************************************************************************
 *
 * Protocol constants.
 *
 ***************************************************************************
 */
#define ELO_PACKET_SIZE		10

#define ELO_SYNC_BYTE		'U'	/* Sync byte. First of a packet.	*/
#define ELO_TOUCH		'T'	/* Report of touchs and motions.	*/
#define ELO_OWNER		'O'	/* Report vendor name.			*/
#define ELO_ID			'I'	/* Report of type and features.		*/
#define ELO_MODE		'M'	/* Set current operating mode.		*/
#define ELO_PARAMETER		'P'	/* Set the serial parameters.		*/
#define ELO_REPORT		'B'	/* Set touch reports timings.		*/
#define ELO_CALIBRATION		'C'	/* Calibration command.			*/
#define ELO_ACK			'A'	/* Acknowledge packet			*/

#define ELO_INIT_CHECKSUM	0xAA	/* Initial value of checksum.		*/

#define	ELO_PRESS		0x01	/* Flags in ELO_TOUCH status byte	*/
#define	ELO_STREAM		0x02
#define ELO_RELEASE		0x04

#define ELO_TOUCH_MODE		0x01	/* Flags in ELO_MODE command		*/
#define ELO_STREAM_MODE		0x02
#define ELO_UNTOUCH_MODE	0x04
#define ELO_RANGE_CHECK_MODE	0x40
#define ELO_TRIM_MODE		0x02
#define ELO_CALIB_MODE		0x04
#define ELO_SCALING_MODE	0x08
#define ELO_TRACKING_MODE	0x40

#define ELO_SERIAL_SPEED	0x06	/* Flags for high speed serial (19200)	*/
#define ELO_SERIAL_MASK		0xF8

#define ELO_SERIAL_IO		'0'	/* Indicator byte for PARAMETER command */


/*
 ***************************************************************************
 *
 * Usefull macros.
 *
 ***************************************************************************
 */
#define WORD_ASSEMBLY(byte1, byte2)	(((byte2) << 8) | (byte1))
#define SYSCALL(call)			while(((call) == -1) && (errno == EINTR))

/* This one is handy, thanx Fred ! */
#ifdef DBG
#undef DBG
#endif
#ifdef DEBUG
#undef DEBUG
#endif

static int      debug_level = 0;
#define DEBUG 1
#if DEBUG
#define DBG(lvl, f) {if ((lvl) <= debug_level) f;}
#else
#define DBG(lvl, f)
#endif


/*
 ***************************************************************************
 *
 * Device private records.
 *
 ***************************************************************************
 */
typedef struct _EloPrivateRec {
  char		*input_dev;		/* The touchscreen input tty			*/
  int		min_x;			/* Minimum x reported by calibration		*/
  int		max_x;			/* Maximum x					*/
  int		min_y;			/* Minimum y reported by calibration		*/
  int		max_y;			/* Maximum y					*/
  int		untouch_delay;		/* Delay before reporting an untouch (in ms)    */
  int		report_delay;		/* Delay between touch report packets		*/
  int		link_speed;		/* Speed of the RS232 link connecting the ts.	*/

  int		screen_no;		/* Screen associated with the device		*/
  int		screen_width;		/* Width of the associated X screen		*/
  int		screen_height;		/* Height of the screen				*/
  Bool		inited;			/* The controller has already been configured ?	*/	
  int		cur_x;			/* Current position in screen coordinates. Used	*/
  int		cur_y;			/* to filter out jitter.			*/
  int		checksum;		/* Current checksum of data in assembly buffer	*/
  int		packet_buf_p;		/* Assembly buffer pointer			*/
  unsigned char	packet_buf[ELO_PACKET_SIZE]; /* Assembly buffer				*/
} EloPrivateRec, *EloPrivatePtr;


#if !defined(sun) || defined(i386)
/*
 ***************************************************************************
 *
 * xf86EloConfig --
 *	Configure the driver from the configuration data.
 *
 ***************************************************************************
 */
static Bool
xf86EloConfig(LocalDevicePtr    *array,
              int               inx,
              int               max,
	      LexPtr            val)
{
  LocalDevicePtr        local = array[inx];
  EloPrivatePtr         priv = (EloPrivatePtr)(local->private);
  int                   token;

  while ((token = xf86GetToken(EloTab)) != ENDSUBSECTION) {
    switch(token) {
      
    case PORT:
      if (xf86GetToken(NULL) != STRING)
	xf86ConfigError("Elographics input port expected");
      priv->input_dev = strdup(val->str);	
      if (xf86Verbose)
	ErrorF("%s Elographics input port: %s\n",
	       XCONFIG_GIVEN, priv->input_dev);
      break;

    case ELO_DEVICE_NAME:
      if (xf86GetToken(NULL) != STRING)
	xf86ConfigError("Elographics device name expected");
      local->name = strdup(val->str);
      if (xf86Verbose)
	ErrorF("%s Elographics X device name: %s\n",
	       XCONFIG_GIVEN, local->name);      
      break;

    case SCREEN_NO:
      if (xf86GetToken(NULL) != NUMBER)
	xf86ConfigError("Elographics screen number expected");
      priv->screen_no = val->num;
      if (xf86Verbose)
	ErrorF("%s Elographics associated screen: %d\n",
	       XCONFIG_GIVEN, priv->screen_no);      
      break;

    case UNTOUCH_DELAY:
      if (xf86GetToken(NULL) != NUMBER)
	xf86ConfigError("Elographics untouch delay expected");
      priv->untouch_delay = val->num;
      if (xf86Verbose)
	ErrorF("%s Elographics untouch delay: %d ms\n",
	       XCONFIG_GIVEN, priv->untouch_delay*10);      
      break;

    case REPORT_DELAY:
      if (xf86GetToken(NULL) != NUMBER)
	xf86ConfigError("Elographics report delay expected");
      priv->report_delay = val->num;
      if (xf86Verbose)
	ErrorF("%s Elographics report delay: %d ms\n",
	       XCONFIG_GIVEN, priv->report_delay*10);      
      break;
      
    case LINK_SPEED:
      {
	int	ltoken = xf86GetToken(LinkSpeedTab);
	if (ltoken == EOF ||
	    ltoken == STRING ||
	    ltoken == NUMBER)
	  xf86ConfigError("Elographics link speed expected");
	priv->link_speed = LinkSpeedValues[ltoken-1].speed;
	if (xf86Verbose)
	  ErrorF("%s Elographics link speed: %s bps\n",
		 XCONFIG_GIVEN, (LinkSpeedTab[ltoken-1].name)+1);
      }
      break;
      
    case MAXX:
      if (xf86GetToken(NULL) != NUMBER)
        xf86ConfigError("Elographics maximum x position expected");
      priv->max_x = val->num;
      if (xf86Verbose)
	ErrorF("%s Elographics maximum x position: %d\n",
	       XCONFIG_GIVEN, priv->max_x);      
     break;
      
    case MAXY:
      if (xf86GetToken(NULL) != NUMBER)
        xf86ConfigError("Elographics maximum y position expected");
      priv->max_y = val->num;
      if (xf86Verbose)
	ErrorF("%s Elographics maximum y position: %d\n",
	       XCONFIG_GIVEN, priv->max_y);      
     break;
      
    case MINX:
      if (xf86GetToken(NULL) != NUMBER)
        xf86ConfigError("Elographics minimum x position expected");
      priv->min_x = val->num;
      if (xf86Verbose)
	ErrorF("%s Elographics minimum x position: %d\n",
	       XCONFIG_GIVEN, priv->min_x);      
     break;
      
    case MINY:
      if (xf86GetToken(NULL) != NUMBER)
        xf86ConfigError("Elographics minimum y position expected");
      priv->min_y = val->num;
      if (xf86Verbose)
	ErrorF("%s Elographics minimum y position: %d\n",
	       XCONFIG_GIVEN, priv->min_y);      
     break;
      
    case DEBUG_LEVEL:
	if (xf86GetToken(NULL) != NUMBER)
	    xf86ConfigError("Option number expected");
	debug_level = val->num;
	if (xf86Verbose) {
#if DEBUG
	    ErrorF("%s Elographics debug level sets to %d\n", XCONFIG_GIVEN,
		   debug_level);      
#else
	    ErrorF("%s Elographics debug level not sets to %d because debugging is not compiled\n",
		   XCONFIG_GIVEN, debug_level);      
#endif
	}
        break;

    case HISTORY_SIZE:
      if (xf86GetToken(NULL) != NUMBER)
	xf86ConfigError("Option number expected");
      local->history_size = val->num;
      if (xf86Verbose)
	ErrorF("%s EloGraphics Motion history size is %d\n", XCONFIG_GIVEN,
	       local->history_size);      
      break;
	    
    case ALWAYS_CORE:
	xf86AlwaysCore(local, TRUE);
	if (xf86Verbose)
	    ErrorF("%s Elographics device always stays core pointer\n",
		   XCONFIG_GIVEN);
	break;

    case EOF:
      FatalError("Unexpected EOF (missing EndSubSection)");
      break;

    default:
      xf86ConfigError("Elographics subsection keyword expected");
      break;
    }
  }

  DBG(2, ErrorF("xf86EloConfig name=%s\n", priv->input_dev))

  return Success;
}
#endif


/*
 ***************************************************************************
 *
 * xf86EloGetPacket --
 *	Read a packet from the port. Try to synchronize with start of
 *	packet and compute checksum.
 *      The packet structure read by this function is as follow:
 *		Byte 0 : ELO_SYNC_BYTE
 *		Byte 1
 *		...
 *		Byte 8 : packet data
 *		Byte 9 : checksum of bytes 0 to 8
 *
 *	This function returns if a valid packet has beed assembled in
 *	buffer or if no more data is available.
 *
 *	Returns Success if a packet is successfully assembled including
 *	testing checksum. If a packet checksum is incorrect, it is discarded.
 *	Bytes preceding the ELO_SYNC_BYTE are also discarded.
 *	Returns !Success if out of data while reading. The start of the
 *	partially assembled packet is left in buffer, buffer_p and
 *	checksum reflect the current state of assembly.
 *
 ***************************************************************************
 */
static Bool
xf86EloGetPacket(unsigned char	*buffer,
		 int		*buffer_p,
		 int		*checksum,
		 int		fd)
{
  int	num_bytes;
  Bool	ok;

  DBG(4, ErrorF("Entering xf86EloGetPacket with checksum == %d and buffer_p == %d\n",
		*checksum, *buffer_p));
  
  /*
   * Try to read enough bytes to fill up the packet buffer.
   */
  DBG(4, ErrorF("buffer_p is %d, Trying to read %d bytes from link\n",
		*buffer_p, ELO_PACKET_SIZE - *buffer_p));
  SYSCALL(num_bytes = read(fd,
			   (char *) (buffer + *buffer_p),
			   ELO_PACKET_SIZE - *buffer_p));
      
  /*
   * Okay, give up.
   */
  if (num_bytes < 0) {
    Error("System error while reading from Elographics touchscreen.");
    return !Success;
  }
  DBG(4, ErrorF("Read %d bytes\n", num_bytes));
    
  while (num_bytes) {
    /*
     * Sync with the start of a packet.
     */
    if ((*buffer_p == 0) && (buffer[0] != ELO_SYNC_BYTE)) {
      /*
       * No match, reset to the start of packet buffer and shift
       * data one byte toward the start of the buffer.
       */
      DBG(4, ErrorF("Dropping one byte in an attempt to synchronize: '%c' 0x%X\n",
		    buffer[0], buffer[0]));
      memcpy(&buffer[0], &buffer[1], num_bytes-1);
    }
    else {
      /*
       * Compute checksum in assembly buffer.
       */
      if (*buffer_p < ELO_PACKET_SIZE-1) {
	*checksum = *checksum + buffer[*buffer_p];
	*checksum = *checksum % 256;
	DBG(4, ErrorF(" 0x%X-->0x%X ", buffer[*buffer_p], *checksum));
      }
      (*buffer_p)++;
    }
    num_bytes--;
  }

  if (*buffer_p == ELO_PACKET_SIZE) {
    /*
     * Got a packet, validate checksum and reset state.
     */
    ok = (*checksum == buffer[ELO_PACKET_SIZE-1]);
    DBG(3, ErrorF("Expecting checksum %d, got %d\n", *checksum, buffer[ELO_PACKET_SIZE-1]));
    *checksum = ELO_INIT_CHECKSUM;
    *buffer_p = 0;

    if (!ok) {
      ErrorF("Checksum error on Elographics touchscreen link\n");
      return !Success;
    }
    
    /*
     * Valid packet received report it.
     */
    return Success;
  }
  else
    return !Success;
}

/*
 ***************************************************************************
 *
 * xf86EloCalibrate --
 *	Calibrate a point with respect to the screen and report
 *	duplicate points.
 *
 *	Return Success if the new point is different from the
 *	previous one, !Success otherwise.
 ***************************************************************************
 */
static Bool
xf86EloCalibrate(EloPrivatePtr	priv,
		 int		*cur_x,
		 int		*cur_y)
{
  int	width, height;

  width = priv->max_x - priv->min_x;
  height = priv->max_y - priv->min_y;
  *cur_x = (priv->screen_width * (*cur_x - priv->min_x)) / width;
  *cur_y = priv->screen_height -
           (priv->screen_height * (*cur_y - priv->min_y)) / height;  

  if ((*cur_x < (priv->cur_x - 1) || *cur_x > (priv->cur_x + 1)) ||
      (*cur_y < (priv->cur_y - 1) || *cur_y > (priv->cur_y + 1))) {
    priv->cur_x = *cur_x;
    priv->cur_y = *cur_y;
    return Success;
  }
  else
    return !Success;
}


/*
 ***************************************************************************
 *
 * xf86EloReadInput --
 *	Read all pending packets from the touchscreen and enqueue them.
 *	If a packet is not fully received it is deferred until the next
 *	call to the function.
 *	Packet types recognized by this function are :
 *
 *		Byte 1 :  ELO_TOUCH
 *		Byte 2 :  Packet type
 *		  Bit 2 : Pen Up   (Release)
 *		  Bit 1 : Position (Stream)
 *		  Bit 0 : Pen Down (Press)
 *		Byte 3 :  X coordinate (lower bits)
 *		Byte 4 :  X coordinate (upper bits)
 *		Byte 5 :  Y coordinate (lower bits)
 *		Byte 6 :  Y coordinate (upper bits)
 *		Byte 7 :  Z coordinate (lower bits)
 *		Byte 8 :  Z coordinates (upper bits)
 *
 *
 ***************************************************************************
 */
static void
xf86EloReadInput(LocalDevicePtr	local)
{
  EloPrivatePtr			priv = (EloPrivatePtr)(local->private);
  int				cur_x, cur_y;
  int				state;

  DBG(4, ErrorF("Entering ReadInput\n"));
  /*
   * Try to get a packet.
   */
  if (xf86EloGetPacket(priv->packet_buf,
		       &priv->packet_buf_p,
		       &priv->checksum,
		       local->fd) != Success)
    return;

  /*
   * Process only ELO_TOUCHs here.
   */
  if (priv->packet_buf[1] == ELO_TOUCH) {
    /*
     * First stick together the various pieces.
     */
    cur_x = WORD_ASSEMBLY(priv->packet_buf[3], priv->packet_buf[4]);
    cur_y = WORD_ASSEMBLY(priv->packet_buf[5], priv->packet_buf[6]);
    state = priv->packet_buf[2] & 0x07;

    /*
     * Send events.
     *
     * We *must* generate a motion before a button change if pointer
     * location has changed as DIX assumes this. This is why we always
     * emit a motion, regardless of the kind of packet processed.
     */
    
    /*
     * Emit a motion.
     */
    ErrorF("before\n");
    if (!xf86IsCorePointer(local->dev) ||
	xf86EloCalibrate(priv, &cur_x, &cur_y) == Success)
      xf86PostMotionEvent(local->dev, TRUE, 0, 2, cur_x, cur_y);
    
    /*
     * Emit a button press or release.
     */
    if (state == ELO_PRESS || state == ELO_RELEASE)
      xf86PostButtonEvent(local->dev, TRUE, 1, state == ELO_PRESS, 0, 2, cur_x, cur_y);

    DBG(3, ErrorF("TouchScreen: x(%d), y(%d), %s\n",
		  priv->cur_x, priv->cur_y,
		  (state == ELO_PRESS) ? "Press" : ((state == ELO_RELEASE) ? "Release" : "Stream")));
  }
}


/*
 ***************************************************************************
 *
 * xf86EloSendPacket --
 *	Emit an height bytes packet to the controller.
 *	The function expects a valid buffer containing the
 *	command to be sent to the controller. It fills it with the
 *	leading sync character an the trailing checksum byte.
 *
 ***************************************************************************
 */
static Bool
xf86EloSendPacket(unsigned char	*packet,
		  int		fd)
{
  int	i, result;
  int	sum = ELO_INIT_CHECKSUM;

  packet[0] = ELO_SYNC_BYTE;
  for (i = 0; i < ELO_PACKET_SIZE-1; i++) {
    sum += packet[i];
    sum %= 256;
  }
  packet[ELO_PACKET_SIZE-1] = sum;

  DBG(4, ErrorF("Sending packet : 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X \n",
		packet[0], packet[1], packet[2], packet[3], packet[4],
		packet[5], packet[6], packet[7], packet[8], packet[9]));
  SYSCALL(result = write(fd, packet, ELO_PACKET_SIZE));
  if (result != ELO_PACKET_SIZE) {
    Error("System error while sending to Elographics touchscreen.");
    return !Success;
  }
  else
    return Success;
}


/*
 ***************************************************************************
 *
 * xf86EloWaitReply --
 *	It is assumed that the reply will be in the few next bytes
 *	read and will be available very soon after the query post. if
 *	these two asumptions are not met, there are chances that the server
 *	will be stuck for a while.
 *	The reply type need to match parameter 'type'.
 *	The reply is left in reply. The function returns Success if the
 *	reply is valid and !Success otherwise.
 *
 ***************************************************************************
 */
static Bool
xf86EloWaitReply(unsigned char	type,
		 unsigned char	*reply,
		 int		fd)
{
  Bool			ok;
  int			i, result;
  int			reply_p = 0;
  int			sum = ELO_INIT_CHECKSUM;
  fd_set		readfds;
  struct timeval	timeout;

  DBG(4, ErrorF("Waiting a '%c' reply\n", type));
  i = ELO_MAX_TRIALS;
  do {
    ok = !Success;
    
    /*
     * Wait half a second for the reply. The fuse counts down each
     * timeout and each wrong packet.
     */
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    timeout.tv_sec = 0;
    timeout.tv_usec = ELO_MAX_WAIT;
    DBG(4, ErrorF("Waiting %d ms before reading port\n",
		  ELO_MAX_WAIT / 1000));
    SYSCALL(result = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout));
    if (result > 0 &&
	FD_ISSET(fd, &readfds)) {
      ok = xf86EloGetPacket(reply, &reply_p, &sum, fd);
      if (ok == Success && reply[1] != type) {
	DBG(2, ErrorF("Wrong reply received\n"));
	ok = !Success;
      }
    }
    else {
      DBG(3, ErrorF("No answer from link : %d\n", result));
    }
    
    i--;
  } while(ok != Success && i);

  return ok;
}


/*
 ***************************************************************************
 *
 * xf86EloWaitAck --
 *	Wait for an acknowledge from the controller. Returns Success if
 *	acknowledge received and reported no errors.
 *
  ***************************************************************************
 */
static Bool
xf86EloWaitAck(int	fd)
{
  unsigned char	packet[ELO_PACKET_SIZE];
  int		i, nb_errors;

  if (xf86EloWaitReply(ELO_ACK, packet, fd) == Success) {
    for (i = 0, nb_errors = 0; i < 4; i++)
      if (packet[2 + i] != '0')
	nb_errors++;
    if (nb_errors != 0) {
      DBG(2, ErrorF("Elographics acknowledge packet reports %d errors\n",
		    nb_errors));
    }
    return Success;
    /*    return (nb_errors < 4) ? Success : !Success;*/
  }
  else
    return !Success;
}


/*
 ***************************************************************************
 *
 * xf86EloSendQuery --
 *	Emit a query to the controller and blocks until the reply and
 *	the acknowledge are read.
 *
 *	The reply is left in reply. The function returns Success if the
 *	reply is valid and !Success otherwise.
 *
 ***************************************************************************
 */
static Bool
xf86EloSendQuery(unsigned char	*request,
		 unsigned char	*reply,
		 int		fd)
{
  Bool			ok;
  
  if (xf86EloSendPacket(request, fd) == Success) {
    ok = xf86EloWaitReply(toupper(request[1]), reply, fd);
    if (ok == Success)
      ok = xf86EloWaitAck(fd);
    return ok;
  }
  else
    return !Success;
}


/*
 ***************************************************************************
 *
 * xf86EloSendControl --
 *	Emit a control command to the controller and wait for acknowledge.
 *
 *	Returns Success if acknowledge received and reported no error.
 *
 ***************************************************************************
 */
static Bool
xf86EloSendControl(unsigned char	*control,
		   int			fd)
{
  if (xf86EloSendPacket(control, fd) == Success) {
    return xf86EloWaitAck(fd);
  }
  else
    return !Success;
}


/*
 ***************************************************************************
 *
 * xf86EloPrintIdent --
 *	Print type of touchscreen and features on controller board.
 *
 ***************************************************************************
 */
static void
xf86EloPrintIdent(unsigned char	*packet)
{
  ErrorF("%s Elographics touchscreen is a ", XCONFIG_PROBED);
  switch(packet[2]) {
  case '0':
    ErrorF("AccuTouch");
    break;
  case '1':
    ErrorF("DuraTouch");
    break;
  case '2':
    ErrorF("Intellitouch");
    break;
  }
  ErrorF(", connected through a ");
  switch(packet[3]) {
  case '0':
    ErrorF("serial link.\n");
    break;
  case '1':
    ErrorF("PC-Bus port.\n");
    break;
  case '2':
    ErrorF("Micro Channel port.\n");
    break;
  }
  ErrorF("%s The controller is a model ", XCONFIG_PROBED);
  if (packet[8] & 1)
    ErrorF("E271-2210");
  else
    ErrorF("E271-2200");
  ErrorF(", firmware revision %d.%d.\n", packet[6], packet[5]);
  
  if (packet[4]) {
    ErrorF("%s Additional features:\n", XCONFIG_PROBED);
    if (packet[4] & 0x10)
      ErrorF("%s	External A/D converter\n", XCONFIG_PROBED);
    if (packet[4] & 0x20)
      ErrorF("%s	32Ko RAM\n", XCONFIG_PROBED);
    if (packet[4] & 0x40)
      ErrorF("%s	RAM onboard\n", XCONFIG_PROBED);
    if (packet[4] & 0x80)
      ErrorF("%s	Z axis active\n", XCONFIG_PROBED);
    ErrorF("\n");
  }
}


/*
 ***************************************************************************
 *
 * xf86EloPtrControl --
 *
 ***************************************************************************
 */
#if 0
static void
xf86EloPtrControl(DeviceIntPtr	dev,
		  PtrCtrl	*ctrl)
{
}
#endif


/*
 ***************************************************************************
 *
 * xf86EloControl --
 *
 ***************************************************************************
 */
static Bool
xf86EloControl(DeviceIntPtr	dev,
	       int		mode)
{
  LocalDevicePtr	local = (LocalDevicePtr) dev->public.devicePrivate;
  EloPrivatePtr		priv = (EloPrivatePtr)(local->private);
  unsigned char		map[] = { 0, 1 };
  unsigned char		req[ELO_PACKET_SIZE];
  unsigned char		reply[ELO_PACKET_SIZE];
  int			i;
  
  switch(mode) {

  case DEVICE_INIT:
    {
#if defined(sun) && !defined(i386)
      char      *name = (char *) getenv("ELO_DEV");
      char      *calib = (char *) getenv("ELO_CALIB");
      char	*speed = (char *) getenv("ELO_SPEED");
      char	*delays = (char *) getenv("ELO_DELAYS");
#endif
      
      DBG(2, ErrorF("Elographics touchscreen init...\n"));

#if defined(sun) && !defined(i386)
      if (name) {
	priv->input_dev = strdup(name);
	ErrorF("Elographics touchscreen port changed to '%s'\n",
	       priv->input_dev);
      }
      if (calib) {
	if (sscanf(calib, "%d %d %d %d",
		   &priv->min_x, &priv->max_x,
		   &priv->min_y, &priv->max_y) != 4) {
	  ErrorF("Incorrect syntax in ELO_CALIB\n");
	  return !Success;
	}
	else if (priv->max_x <= priv->min_x ||
		 priv->max_y <= priv->min_y) {
	  ErrorF("Bogus calibration data in ELO_CALIB\n");
	  return !Success;
	}
	else {
	  ErrorF("Calibration will be done with:\n");
	  ErrorF("x_min=%d, x_max=%d, y_min=%d, y_max=%d\n",
		 priv->min_x, priv->max_x, priv->min_y, priv->max_y);
	}
      }
      if (speed) {
	/* These tests should be kept in sync with the LinkSpeedValues
	 * array. */
	if (strcmp(speed, "B9600") == 0)
	  priv->link_speed = B9600;
	else if (strcmp(speed, "B19200") == 0)
	  priv->link_speed = B19200;
	else if (strcmp(speed, "B2400") == 0)
	  priv->link_speed = B2400;
	else if (strcmp(speed, "B1200") == 0)
	  priv->link_speed = B1200;
	else if (strcmp(speed, "B300") == 0)
	  priv->link_speed = B300;
	else {
	  ErrorF("Bogus speed value in ELO_SPEED\n");
	  return !Success;
	}
      }
      if (delays) {
	if (sscanf(delays, "%d %d",
		   &priv->untouch_delay,
		   &priv->report_delay) != 2) {
	  ErrorF("Bogus delays data in ELO_DELAYS\n");
	}
	else {
	  ErrorF("Untouch delay will be: %d\n", priv->untouch_delay);
	  ErrorF("Report delay will be: %d\n", priv->report_delay);
	}
      }
#endif
      
      if (priv->screen_no >= screenInfo.numScreens ||
	  priv->screen_no < 0)
	priv->screen_no = 0;
      priv->screen_width = screenInfo.screens[priv->screen_no]->width;
      priv->screen_height = screenInfo.screens[priv->screen_no]->height;

      /*
       * Device reports button press for up to 1 button.
       */
      if (InitButtonClassDeviceStruct(dev, 1, map) == FALSE) {
	ErrorF("Unable to allocate Elographics touchscreen ButtonClassDeviceStruct\n");
	return !Success;
      }
      
      /*
       * Device reports motions on 2 axes in absolute coordinates.
       * Axes min and max values are reported in raw coordinates.
       * Resolution is computed roughly by the difference between
       * max and min values scaled from the approximate size of the
       * screen to fit one meter.
       */
      if (InitValuatorClassDeviceStruct(dev, 2, xf86GetMotionEvents, local->history_size, Absolute) == FALSE) {
	ErrorF("Unable to allocate Elographics touchscreen ValuatorClassDeviceStruct\n");
	return !Success;
      }
      else {
	InitValuatorAxisStruct(dev, 0, priv->min_x, priv->max_x,
			       9500,
			       0     /* min_res */,
			       9500  /* max_res */);
	InitValuatorAxisStruct(dev, 1, priv->min_y, priv->max_y,
			       10500,
			       0     /* min_res */,
			       10500 /* max_res */);
      }
      
      /*
       * Allocate the motion events buffer.
       */
      xf86MotionHistoryAllocate(local);
      
      /*
       * This once has caused the server to crash after doing an xalloc & strcpy ??
       */
      AssignTypeAndName(dev, local->atom, local->name);
      
      DBG(2, ErrorF("Done.\n"));
      return Success;
    }
    
  case DEVICE_ON:
    DBG(2, ErrorF("Elographics touchscreen on...\n"));

    if (local->fd < 0) {
      struct termios termios_tty;

      DBG(2, ErrorF("Elographics touchscreen opening : %s\n",
		    priv->input_dev));
      SYSCALL(local->fd = open(priv->input_dev, O_RDWR));
      if (local->fd < 0) {
	Error("Unable to open Elographics touchscreen device");
	return !Success;
      }

      /*
       * Try to see if the link is at the specified rate.
       */
      DBG(3, ErrorF("Try to see if the link is at the specified rate\n"));
      memset(&termios_tty, 0, sizeof(termios_tty));
      termios_tty.c_cflag = priv->link_speed | CS8 | CREAD | CLOCAL;
      termios_tty.c_cc[VMIN] = 1;
      if (tcsetattr(local->fd, TCSANOW, &termios_tty) < 0) {
	Error("Unable to configure Elographics touchscreen port");
	goto not_success;
      }
      memset(req, 0, ELO_PACKET_SIZE);
      req[1] = tolower(ELO_PARAMETER);
      if (xf86EloSendQuery(req, reply, local->fd) != Success) {
	DBG(3, ErrorF("Not at the specified rate, giving up\n"));
	goto not_success;
      }
      
      /*
       * Ask the controller to report various infos.
       */
      if (xf86Verbose) {
	memset(req, 0, ELO_PACKET_SIZE);
	req[1] = tolower(ELO_ID);
	if (xf86EloSendQuery(req, reply, local->fd) == Success) {
	  xf86EloPrintIdent(reply);
	}
	else {
	  ErrorF("Unable to ask Elographics touchscreen identification\n");
	  goto not_success;
	}
      }
      
      /*
       * Set the operating mode: Stream, no scaling, no calibration,
       * no range checking, no trim, tracking enabled.
       */
      memset(req, 0, ELO_PACKET_SIZE);
      req[1] = ELO_MODE;
      req[3] = ELO_TOUCH_MODE | ELO_STREAM_MODE | ELO_UNTOUCH_MODE;
      req[4] = ELO_TRACKING_MODE;
      if (xf86EloSendControl(req, local->fd) != Success) {
	ErrorF("Unable to change Elographics touchscreen operating mode\n");
	goto not_success;
      }

      /*
       * Check if the report delay is compatible with the selected
       * link speed and reset it otherwise.
       */
      for (i = 0; i < sizeof(LinkSpeedValues)/sizeof(LinkParameterStruct); i++) {
	if (LinkSpeedValues[i].speed == priv->link_speed) {
	  if (LinkSpeedValues[i].delay > priv->report_delay) {
	    ErrorF("Changing report delay from %d ms to %d ms to comply with link speed\n",
		   priv->report_delay*10, LinkSpeedValues[i].delay*10);
	    priv->report_delay = LinkSpeedValues[i].delay;
	  }
	}
      }
      /*
       * Set the touch reports timings from configuration data.
       */
      memset(req, 0, ELO_PACKET_SIZE);
      req[1] = ELO_REPORT;
      req[2] = priv->untouch_delay;
      req[3] = priv->report_delay;
      if (xf86EloSendControl(req, local->fd) != Success) {
	ErrorF("Unable to change Elographics touchscreen reports timings\n");

      not_success:
	close(local->fd);
	local->fd = -1;
	return !Success;
      }
      
      AddEnabledDevice(local->fd);
      dev->public.on = TRUE;  
    }
    
    DBG(2, ErrorF("Done\n"));
    return Success;
    
    /*
     * Deactivate the device. After this, the device will not emit
     * events until a subsequent DEVICE_ON. Thus, we can momentarily
     * close the port.
     */
  case DEVICE_OFF:
    DBG(2, ErrorF("Elographics touchscreen off...\n"));
    dev->public.on = FALSE;
    RemoveEnabledDevice(local->fd);
    close(local->fd);
    local->fd = -1;
    DBG(2, ErrorF("Done\n"));
    return Success;
    
    /*
     * Final close before server exit. This is used during server shutdown.
     * Close the port and free all the resources.
     */
  case DEVICE_CLOSE:
    DBG(2, ErrorF("Elographics touchscreen close...\n"));
    dev->public.on = FALSE;
    RemoveEnabledDevice(local->fd);
    close(local->fd);
    local->fd = -1;
    DBG(2, ErrorF("Done\n"));
    return Success;

  default:
      ErrorF("unsupported mode=%d\n", mode);
      return !Success;
  }
}

/*
 ***************************************************************************
 *
 * xf86EloAllocate --
 *
 ***************************************************************************
 */
static LocalDevicePtr
xf86EloAllocate(
#if NeedFunctionPrototypes
	void
#endif
	)
{
  LocalDevicePtr        local = (LocalDevicePtr) xalloc(sizeof(LocalDeviceRec));
  EloPrivatePtr         priv = (EloPrivatePtr) xalloc(sizeof(EloPrivateRec));
  
  priv->input_dev = ELO_PORT;
  priv->link_speed = ELO_LINK_SPEED;
  priv->min_x = 0;
  priv->max_x = 0;
  priv->min_y = 0;
  priv->max_y = 0;
  priv->untouch_delay = ELO_UNTOUCH_DELAY;
  priv->report_delay = ELO_REPORT_DELAY;
  priv->screen_no = 0;
  priv->screen_width = -1;
  priv->screen_height = -1;
  priv->inited = 0;
  priv->cur_x = 0;
  priv->cur_y = 0;
  priv->checksum = ELO_INIT_CHECKSUM;
  priv->packet_buf_p = 0;

  local->name = XI_TOUCHSCREEN;
  local->flags = XI86_NO_OPEN_ON_INIT;
#if !defined(sun) || defined(i386)
  local->device_config = xf86EloConfig;
#endif
  local->device_control = xf86EloControl;
  local->read_input   = xf86EloReadInput;
  local->control_proc = NULL;
  local->close_proc   = NULL;
  local->switch_mode  = NULL;
  local->fd	      = -1;
  local->atom	      = 0;
  local->dev	      = NULL;
  local->private      = priv;
  local->type_name    = "ELO TouchScreen";
  local->history_size = 0;
  
  return local;
}

/*
 ***************************************************************************
 *
 * Elographics device association --
 *
 ***************************************************************************
 */
DeviceAssocRec elographics_assoc =
{
  "elographics",                /* config_section_name */
  xf86EloAllocate               /* device_allocate */
};

#ifdef DYNAMIC_MODULE
/*
 ***************************************************************************
 *
 * entry point of dynamic loading
 *
 ***************************************************************************
 */
int
#ifndef DLSYM_BUG
init_module(unsigned long	server_version)
#else
init_xf86Elo(unsigned long      server_version)
#endif
{
    xf86AddDeviceAssoc(&elographics_assoc);

    if (server_version != XF86_VERSION_CURRENT) {
	ErrorF("Warning: Elographics module compiled for version%s\n", XF86_VERSION);
	return 0;
    } else {
	return 1;
    }
}
#endif
