/* $XConsortium: Rx.h /main/4 1996/09/25 20:23:54 lehors $ */
/*

Copyright (C) 1996 X Consortium

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Soft-
ware"), to deal in the Software without restriction, including without
limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to
whom the Software is furnished to do so, subject to the following condi-
tions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABIL-
ITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT
SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABIL-
ITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization from
the X Consortium.

*/

#ifndef _Rx_h
#define _Rx_h

/* a fancy Boolean type */
typedef enum { RxUndef=-1, RxFalse=0, RxTrue=1 } RxBool;


/* various enums, starting from index 1,
   with a dummy last value allowing to easily get the size,
   new values must be inserted before the last one */
typedef enum { UI=1, Print, LASTService } RxService;
typedef enum { XUI=1, LASTUIProtocol } RxUIProtocol;
typedef enum { XPrint=1, LASTPrintProtocol } RxPrintProtocol;

typedef enum { MitMagicCookie1=1, LASTXAuthentication } RxXAuthentication;

/* for each of the above enum define the corresponding max array size,
   counting 1 for NULL terminating */
#define MAX_SERVICES LASTService
#define MAX_UIPROTOS LASTUIProtocol
#define MAX_PRINTPROTOS LASTPrintProtocol

#define MAX_XAUTHENTICATIONS LASTXAuthentication

/* RxParams structure,
   designed so it can handle whatever parameter is defined in an RX document */
typedef struct {
    short version;		/* document version number */
    short revision;		/* document revision number */

    char *action;		/* URL */
    RxBool embedded;
    RxBool auto_start;
    int width;
    int height;
    char *app_group;		/* application group name */

    /* NULL terminated "lists" */
    RxService required_services[MAX_SERVICES];
    RxUIProtocol ui[MAX_UIPROTOS];
    RxPrintProtocol print[MAX_PRINTPROTOS];

    /* private params */
    RxBool x_ui_input_method;
    char *x_ui_input_method_url;

    RxBool x_ui_lbx;
    RxBool x_print_lbx;

    RxXAuthentication x_auth[MAX_XAUTHENTICATIONS];
    char *x_auth_data[MAX_XAUTHENTICATIONS];

    RxXAuthentication x_ui_auth[MAX_XAUTHENTICATIONS];
    char *x_ui_auth_data[MAX_XAUTHENTICATIONS];

    RxXAuthentication x_print_auth[MAX_XAUTHENTICATIONS];
    char *x_print_auth_data[MAX_XAUTHENTICATIONS];

    RxXAuthentication x_ui_lbx_auth[MAX_XAUTHENTICATIONS];
    char *x_ui_lbx_auth_data[MAX_XAUTHENTICATIONS];

    RxXAuthentication x_print_lbx_auth[MAX_XAUTHENTICATIONS];
    char *x_print_lbx_auth_data[MAX_XAUTHENTICATIONS];

} RxParams;


typedef struct {
    RxBool embedded;
    int width;
    int height;
    char *action;
    char *ui;			/* URL */
    char *print;		/* URL */

    /* private params */
    RxBool x_ui_lbx;
    char *x_ui_lbx_auth;
    RxBool x_print_lbx;
    char *x_print_lbx_auth;
} RxReturnParams;


/* functions */
extern int
RxReadParams(char *stream,
	     char **argn_ret[], char **argv_ret[], int *argc_ret);

extern void RxInitializeParams(RxParams *params);

extern int
RxParseParams(char *argn[], char *argv[], int argc, RxParams *params,
	      int debug);	/* (1/0) */

extern char *RxBuildRequest(RxReturnParams *params);

extern int RxFreeParams(RxParams *params);
extern int RxFreeReturnParams(RxReturnParams *params);

#endif /* _Rx_h */
