/*
 * $XConsortium: aixMsg.c /main/3 1995/12/19 13:05:36 matt $
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

/*              include file for message texts          */
#include "X_msg.h" 
nl_catd  scmc_catd = NULL;   /* Cat descriptor for scmc conversion */

#ifndef DONOTUSELOCALE
#include <locale.h>
#endif

#include <stdio.h>


/* Default error messages used if catalog file is not found */

static char *err_msgs[] = {
        "1362-001 The X Server cannot open the default font %s.\n\tMake sure that the font or default font path is valid.\n",
	"1362-002 The X Server cannot open the default cursor font %s.\n\tMake sure that the cursor font or default cursor font path is valid.\n",
	"1362-003 The X Server cannot open RGB database %s.\n\
\tCheck the database %s. If valid, try invoking server again.\n",
	"1362-004 Usage: The \"X\" command accepts the following flags:\n\
\t   -a  <accel>     specify the mouse acceleration\n\
\t   -bp <color>     specify a BlackPixel color\n\
\t   -c  <volume>    specify the key click volume\n\
\t   -D  <file>      specify the color definition data base file\n\
\t   -f  <volume>    specify the beep volume\n\
\t   -fc <font>      specify the font for cursor glyphs and masks\n\
\t   -fn <font>      specify the default text font\n\
\t   -fp <path>      specify the search path for fonts\n\
\t   -m              use monochrome display characteristics\n\
\t   -n :<num>       specify the connection number\n\
\t   -p  <interval>  specify the screen saver interval\n\
\t   -r              disables auto repeat\n\
\t   -s <delay>      specify the screen saver delay\n\
\t   -t <threshold>  specify the mouse threshold\n\
\t   -to <time>      specify the time between connection checks\n\
\t   -T              disable the Ctrl-Alt-Backspace key sequence\n\
\t   -force          enables running X Server from a non-lft terminal\n\
\t   -v              activate screen saver with background color\n\
\t   -wp <color>     specify a WhitePixel color\n",
        "1362-005 Error number %s was received while attempting to bind a UNIX\n\
\tdomain socket.\n\tCheck permissions in /dev/sock file.\n",
	"1362-006 An error was detected while attempting to bind the TCP socket.\n\
\tBe sure the network is available, the network is configured correctly,\n\
\tand any necessary devices are configured correctly.\n",
	"1362-007 The X Server is unable to query current keyboard.\n\
\tCheck that the keyboard is in working condition and firmly attached\n\
\tto the machine.\n",
	"1362-008 The X Server is unable to query display device IDs.\n\
\tCheck that the device of the specified type is attached and configured\n\
\tcorrectly.\n",
	"1362-009 The X Server is unable to locate display %s.\n\
\tBe sure that the device specified is attached to the X server and is\n\
\tconfigured correctly.\n",
	"1362-010 The X Server cannot configure the locator device.\n\
\tBe sure the device is attached to the computer and configured\n\
\tcorrectly.\n"};


void aixErrMsg(err_nbr, x_parm)
int err_nbr;
char *x_parm;
{

#ifndef DONOTUSELOCALE
	setlocale(LC_ALL, "");
#endif
/* open the message catalog file descriptor */
/*
	if(!scmc_catd)
	        scmc_catd = NLcatopen(MF_X);
*/

	/* Print the error message from the NL message catalog */
	fprintf(stderr, NLcatgets(scmc_catd, MS_X, err_nbr, err_msgs[err_nbr-1]), 
		x_parm, x_parm);
}
