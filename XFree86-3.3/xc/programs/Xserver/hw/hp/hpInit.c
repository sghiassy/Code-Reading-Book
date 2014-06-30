/* $XConsortium: hpInit.c,v 1.5 95/01/24 01:43:05 dpw Exp $ */
/*************************************************************************
 * 
 * (c)Copyright 1992 Hewlett-Packard Co.,  All Rights Reserved.
 * 
 *                          RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the U.S. Government is subject to
 * restrictions as set forth in sub-paragraph (c)(1)(ii) of the Rights in
 * Technical Data and Computer Software clause in DFARS 252.227-7013.
 * 
 *                          Hewlett-Packard Company
 *                          3000 Hanover Street
 *                          Palo Alto, CA 94304 U.S.A.
 * 
 * Rights for non-DOD U.S. Government Departments and Agencies are as set
 * forth in FAR 52.227-19(c)(1,2).
 *
 *************************************************************************/

#include "X.h"
#include "Xproto.h"
#include <servermd.h>

#include "screenint.h"
#include "input.h"
#include "cursor.h"
#include "misc.h"
#include "windowstr.h"
#include "scrnintstr.h"

#include "screentab.h"
#include "gcstruct.h"
#include "errno.h"

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#include "hppriv.h" 
#include "XHPproto.h"

#ifndef LIBDIR
#if OSMAJORVERSION >= 10
#define LIBDIR "/etc/X11"
#else
#define LIBDIR "/usr/lib/X11"
#endif
#endif

extern char    		*display;	/* display number as a string */
extern unsigned char 	*get_display_address();

extern void  		Xfree();

extern char 		*getenv();   /* for reading SB_DISPLAY_ADDR env var  */
extern long 		strtol();

static void 	ParseXnScreensFile();
static void     ResetFields();
static void	FirstTimeInitialization( );
static int      ParseNextScreen( );
static char     yygetc();
static void     ReadToNextWhiteSpace();
static void     SyntaxError();
void 		Exit();

static int 	NumScreens;  /* number of screens found */

PixmapFormatRec	formats[] = {
    1, 1, BITMAP_SCANLINE_PAD,		/* 1-bit deep for all */
    8, 8, BITMAP_SCANLINE_PAD,  	/* 8-bit deep for most color displays */
    24, 32, BITMAP_SCANLINE_PAD,	/* 24-bit deep display */
};
#define NUMFORMATS	(sizeof formats)/(sizeof formats[0])
#define NUM_DEPTHS	16

static hpPrivPtr AllocateDataStructures();

int 	LineNumber = 1;


/*
 * parameters defining the reserved word return values 
 */
#define DEPTH		1
#define DRIVERNAME	2
#define DOUBLEBUFFER	3
#define MONITORSIZE	4

/* 
 * parameters defining the return status from GetToken().
 */
#define END_OF_LINE	1
#define ID		2
#define INTEGER		3
#define RESERVED_WORD	4

/*
 * reserved word table for parsing X*screens file
 */
static struct token {
	char name[16];
	int rvalue;
} tks [] = {
	"depth",	DEPTH,
	"drivername",	DRIVERNAME,
	"doublebuffer",	DOUBLEBUFFER,
	"monitorsize",	MONITORSIZE,
	"", NULL,
};

/************************************************************
 * InitOutput --
 *	This routine is called from main() during the server 
 *      startup initialization, and when the server is to be reset.
 *      The routine does the device dependent initialization for
 *	all of the screens this server is configured to support.
 *
 * Results:
 *	The array of ScreenRec pointers referenced by
 *      pScreenInfo->screen are allocated and initialized.
 *      The X*screens file is parsed and checked for 
 *      validity with reference to the particular screen
 *      hardware.
 *
 ************************************************************/

extern int hpActiveScreen;	/* active screen ndx (Zaphod), in x_hil.c */

Bool (*ddx_driver_info())();
Bool (*ddxScreenInitPointer[MAXSCREENS])();

void
InitOutput(pScreenInfo, argc, argv)
    ScreenInfo 	  *pScreenInfo;
    int     	  argc;
    char    	  **argv;
{
    int i;

    static int firstTime = 1;

	/* Reset some cursor stuff to avoid massive confusion at server
	 *   reset time.
	 */
    hpActiveScreen = 0;
    hpBlottoCursors(); 

    FirstTimeInitialization( pScreenInfo, argc, argv);

    /*************************
	Set up signal handling 
    **/
    OsSignal (SIGPIPE, SIG_IGN);
    OsSignal (SIGHUP, AutoResetServer);
    OsSignal (SIGINT, GiveUp);
    OsSignal (SIGTERM, GiveUp);
}

hpPrivPtr           hpPrivates[MAXSCREENS];
int		    hpNumScreens;

/************************************************************
 * FirstTimeInitialization
 *
 *   This routine handles the first time initialization 
 *   duties.
 *
 ************************************************************/

static void
FirstTimeInitialization( pScreenInfo, argc, argv)
ScreenInfo    *pScreenInfo;
int           argc;
char          **argv;
{
    int i;

    ParseXnScreensFile( pScreenInfo );

    /*
     * Add check of device independent screen conditions -kam 9/13
     */
    
    if(hpNumScreens > MAXSCREENS)
    {
	ErrorF("Number of screens requested exceeds allowable limit of %d screens.\n", MAXSCREENS);
	ErrorF("Please reduce the number of requested screens in your ");
	ErrorF("'%s/X%sscreens' file.\n", LIBDIR, display);
	ErrorF("Server exiting...\n");
	Exit(UNABLE_TO_INITIALIZE_THE_DISPLAY);
    }

    for(i = 0; i < hpNumScreens; i++)
    {
	ddxScreenInitPointer[i] = ddx_driver_info(hpPrivates[i]);
    }


    pScreenInfo->imageByteOrder     = IMAGE_BYTE_ORDER;
    pScreenInfo->bitmapScanlineUnit = BITMAP_SCANLINE_UNIT;
    pScreenInfo->bitmapScanlinePad  = BITMAP_SCANLINE_PAD;
    pScreenInfo->bitmapBitOrder     = BITMAP_BIT_ORDER;
    pScreenInfo->numPixmapFormats   = NUMFORMATS;



    for ( i = 0; i < NUMFORMATS; i++)
    {
	pScreenInfo->formats[i] = formats[i];
    }

    /*
     * call the AddScreen function in dix/main.c
     * This routine will fiddle with the pixmap formats, and
     * then call ngleScreenInit to initialize the screen.
     *
     * WARNING: AddScreen has a bogosity(!) in that it uses the
     * value of pScreenInfo.numScreens to determine which
     * &pScreenInfo.screen[ i ]  to pass to ngleScreenInit.
     * Therefore the value of pScreenInfo.numScreens is manipulated
     * here as a work around.  ( Do we really HAVE TO call AddScreen? ).
     */

    do 
    {
	if (-1 == AddScreen(ddxScreenInitPointer[pScreenInfo->numScreens],
		    argc,argv))
	    FatalError("Could not add screen.\n");

    } while ( pScreenInfo->numScreens != hpNumScreens );

    return;
}


/************************************************************
 *  ParseXnscreensFile
 *
 *     This routine parses the X*screens file.
 *  where * refers to the display number of this server.
 *
 *  1) If no X*screens file exists, just set up the defaults
 *     and return.
 *  2) Each non-comment line in the X*screens file gives a
 *     definition for a screen.
 *
 *
 ************************************************************/

static void
ParseXnScreensFile( pScreenInfo )
ScreenInfo * pScreenInfo;
{
    int 	fd;
    int 	i;
    char 	filename[MAX_PATH_NAME];
    FILE 	*str;
    int 	status;
    hpPrivPtr 	phpPriv;
    int		index;

    hpNumScreens = 0;

    phpPriv = AllocateDataStructures();
    hpPrivates[hpNumScreens] = phpPriv;

    sprintf( filename, "%s/X%sscreens", LIBDIR, display );
    str = fopen( filename, "r");
    if (str == NULL) FatalError("Cannot open the %s file.\n", filename);

    status = ParseNextScreen( phpPriv, filename, str );
    /*
     * if status is -1, then the ParseNextScreen routine
     * could not find ANY useful information in the 
     * Xnscreens file, so just setup the default
     */
    if ( status == -1 )
      FatalError(
	"No valid information exists in your %s/Xnscreens file.\n", LIBDIR);
    /*
     *
     */
    for (;;)
    {
	phpPriv = AllocateDataStructures ();
	hpNumScreens++;
	status = ParseNextScreen( phpPriv, filename, str );
	/*
	 * status of 1 indicates end of file.
	 */
	if ( ( status == 1 ) || ( status == -1 ) )
	{
	    Xfree(phpPriv);
	    break;
	}
	hpPrivates[hpNumScreens] = phpPriv;
    }

    fclose( str );
}

/************************************************************
 *  ParseNextScreen
 *
 * This routine is responsible for parsing one line of the 
 * X*screens file.   It loops until it can return valid 
 * information from one line of the X*screens file, or 
 * until EOF is encountered.  The actual parsing of a
 * line in the file is performed here.  If a syntax error
 * is encountered, the routine throws away the line, and
 * tries to parse the next line.
 *
 * RETURNED VALUES: 0 if valid value filled out in *pScrPriv
 *                  1 if EOF encountered
 *                 -1 if no valid entries (at all) found
 *
 * EXTERNAL VARIABLES: 
 *     display   - set in main.c, points to string giving 
 *                display number for this server.
 ************************************************************/

static 
ParseNextScreen( phpPriv, filename, str )
hpPrivPtr       phpPriv;
char          * filename;
FILE          * str;
{
    int    i;
    int    status;
    char   string[MAX_PATH_NAME];
    int    Error = 0;

    /*
     * loop until we get a valid screen line, or until
     * EOF is seen.
     */
    while ( 1 )
    {
	ResetFields( phpPriv );
	/*
	 * first field must be a "/dev/crt" type field.
	 * read until we get an IDENTIFIER
	 */
	for (;;)
	{
	    status = GetToken( str, filename, &i, string, MAX_PATH_NAME );
	    if (status == -1 )
		    return( -1 );
	    if ( status != ID )
	    {
		if ( status == END_OF_LINE )
		   continue;
		else 
		{
		    SyntaxError( str, filename );
		    Error = 1;
		}
	    }
	    break;
	}
	if ( Error )
	{
	   Error = 0;
	   continue;
	}
	/*
	 * Hokay, we have an IDENTIFIER.  This *should* 
	 * be a device, however it is not checked for validity
	 * here.
	 */

	strncpy( phpPriv->StandardDevice, string, MAX_PATH_NAME );
	phpPriv->LineNumber = LineNumber;
	/*
	 * now parse for reserved words until we see an
	 * IDENTIFIER or END_OF_LINE
	 */
	while ( 1 )
	{
	    status = GetToken( str, filename, &i, string, MAX_PATH_NAME );
	    switch( status )
	    {
	    case -1:
		    return( 1 );
	    case END_OF_LINE:
		    return( 0 );
		    break;

	    case RESERVED_WORD:
		switch( i )
		{
		case MONITORSIZE:
		    if ( GetANumber( str, filename, &i ) == -1 )
		    {
			Error = 1;
			break;
		    }
		    phpPriv->MonitorDiagonal = i;
		    break;
		case DEPTH:
		    if ( GetANumber (str, filename, &i ) == -1 )
		    {
			Error = 1;
			break;
		    }
		    phpPriv->depth = i;
		    break;
		}
		/*
		 * end of switch on result from GetToken
		 */
		break;

	    default:
		FatalError( "An error occurred while processing %s/X*screens.",
			   LIBDIR);
	    }
	    if ( Error )
		break;
	}
	/*
	 * OK, now we either have satisfactorily parsed
	 * the primary screen device AND we have a valid secondary
	 * screen device, OR we have an error.
	 */
	if ( Error )
	{
	    Error = 0;
	    continue;
	}
    }
}

/************************************************************
 *  AllocateDataStructures
 *
 *  Allocate the hp private data structures 
 ************************************************************/

static hpPrivPtr
AllocateDataStructures()
{
    hpPrivPtr        phpPriv; 

    phpPriv = (hpPrivPtr) Xcalloc( sizeof( hpPriv ));

    if ( phpPriv == NULL )
    {
	  FatalError("Cannot allocate enough memory for internal use--sorry");
    }

    return( phpPriv );
}

/************************************************************
 *  GetANumber
 *
 ************************************************************/

int
GetANumber( str, filename, pInt ) 
FILE    * str;
char    * filename;
int     * pInt;
{
    int status;
    int i;
    char string[MAX_PATH_NAME];

    status = GetToken( str, filename, &i, string, MAX_PATH_NAME );
    if (status == -1 )
    {
	SyntaxError( str, filename );
	return( -1 );		/* kam 9/13 */
    }
    if ( status != INTEGER )
    {
	SyntaxError( str, filename );
	return( -1 );		/* kam 9/13 */
    }
    *pInt = i;
    return( 0 );
}

/************************************************************
 *  GetToken
 *    This routine scans the X*screens file as given
 * by fd for the next token.
 *
 * RETURNED VALUES:
 *                    INTEGER  if an integer is seen
 *		      ID       if an identifier is seen
 *		      STRING   if a device pathname is seen
 *		      -1       if EOF is seen
 *
 *		      strptr used to fill pathname when STRING returned
 *		      iptr used to fill id number when ID returned
 *		      iptr used to fill integer when INTEGER returned
 *
 *
 ************************************************************/
int
GetToken( FileStream, FileName, iptr, strptr, MaxStrLen  )
FILE * FileStream;
char * FileName;
int  * iptr;
char * strptr;
int    MaxStrLen;
{
    char c;
    int i;
    char nextid[MAX_PATH_NAME];
    struct token *t;
    char *cp;

    for (;;) {
	c = yygetc( FileStream );
	if (c == '\n')
		return( END_OF_LINE );
	if ( isspace( c ))
		continue;
	if (c == EOF) 
		return(-1);
	if (isdigit(c))
	{
	    i = 1;
	    nextid[0] = c;
	    while ( isdigit( c = yygetc( FileStream ) )
		    || ( c == 'x') || (c == 'X' )) 
		nextid[i++] = c;
	    nextid[i] = 0;
	    *iptr = atoi( nextid );
	    ungetc( c, FileStream );
	    if ( c == '\n' )
		LineNumber--;
	    return( INTEGER );
	}
	else if ( ! isspace ( c ))
	{
	    nextid[0] = c;
	    for (i = 1; i < MaxStrLen; i++)
	    {
		if ( !isspace( c = yygetc( FileStream )))
		{
		    nextid[i] = c;
		}
		else
		{
		    ungetc(c, FileStream);
		    if ( c == '\n' )
			LineNumber--;
		    nextid[i] = 0;
		    /*
		     * search for the identifier
		     * in the reserved word table.
		     */
		    for (t = tks; t->rvalue; t++)
		    {
			if ( strcmp(nextid, t->name) == 0 ) {
				*iptr = t->rvalue;
				return( RESERVED_WORD );
			}
		    }
		    strncpy( strptr, nextid, MaxStrLen );
		    return( ID );
		}
	    }
	    /*
	     * error, string is too long
	     */
ErrorF( "String is too long in file %s on line %d.  Ignoring the line...\n",
	     FileName, LineNumber );
	    ReadToNextWhiteSpace( FileStream);
	    return( GetToken( FileStream, FileName, iptr, strptr, MaxStrLen ));

	}
	else
	{
ErrorF( "Syntax error in file %s on line %d.  Ignoring the line...\n",
	     FileName, LineNumber );
	    ReadToNextWhiteSpace( FileStream);
	    return( GetToken( FileStream, FileName, iptr, strptr, MaxStrLen ));
	}
    } 
}
/************************************************************
 * ResetFields
 *   This routine clears out fields in the hpPriv structure.
 *   This makes sure that any previous settings are cleared if we hit
 *   a syntax error.
 *
 ************************************************************/

static void
ResetFields( phpPriv )
hpPrivPtr   phpPriv;
{
    phpPriv->StandardDevice[0] = 0;
    phpPriv->StandardDriver[0] = 0;
    phpPriv->StandardNumDepths = 0;
    phpPriv->StandardDoubleBuffer = 0;
    phpPriv->StandardDoubleBufferDepth = 0;

    phpPriv->MonitorDiagonal = 0;
    phpPriv->depth = 8;

}
/************************************************************
 * SyntaxError
 *   This routine reports a syntax error regarding the 
 *   X*screens file, and then skips over the rest of the
 *   current line.
 *
 ************************************************************/

static void
SyntaxError( FileStream, filename )
FILE * FileStream;
char * filename;
{
    int    i;
    char   Dummy[MAX_PATH_NAME];
    int    status;

    ErrorF( "Syntax error in file %s on line %d.  Ignoring the line...\n",
	     filename, LineNumber );
    do {
	status = GetToken( FileStream, filename, &i, Dummy, MAX_PATH_NAME );
	if ( status == -1 )
	    return;
    } while ( status != END_OF_LINE );
}

/************************************************************
 * ReadToNextWhiteSpace
 *   This routine reads characters from the Xnscreens file 
 *   until the next whitespace character is reached.
 *   Retain an end of line for parsing purposes if one
 *   is encounterd.
 *
 ************************************************************/

static void
ReadToNextWhiteSpace( FileStream )
FILE * FileStream;
{
    char c;

    while (( c = yygetc( FileStream )) != EOF )
    {
	if ( (c == ' ' ) || ( c == '\t') )
	    return;
	if ( c == '\n' )
	{
	    ungetc(c, FileStream);
	    LineNumber--;
	    return;
	}
    }
}

/************************************************************
 * yygetc
 *   This routine reads characters from the Xnscreens file.
 *   The routine skips over comments ( begin with a # and go to 
 *   the end of the line ).
 *
 *  RETURNED VALUES:  returns the next character in the file.
 *
 *  SIDE EFFECTS:     LineNumber is updated
 ************************************************************/
static char 
yygetc( FileStream )
FILE *FileStream;
{
    char c;

    if ((c = fgetc( FileStream )) == EOF)
	    return((char)EOF);
    if (c == '\n') {
	    LineNumber++;
	    return(c);
    }
    else if ( c == '#' )
    {
	while ( c == '#' )
	{
	    while (( c = fgetc( FileStream )) != EOF)
	    {
		if (c != '\n')
			continue;
		else
		{
			LineNumber++;
			return( c );
		}
	    }
	}
    }
    return(c);
}

/****************************************
* ddxUseMsg()
*
* Called my usemsg from os/utils/c
*
*****************************************/

void ddxUseMsg()
{
	/* Right now, let's just do nothing */
}

void Exit (code)
    int	code;
{
    exit (code);
}

void AbortDDX ()
{
}

/*****************************************************
	The main exit point for the server.
	    - throws us here when the user [Shift][CTRL][Reset]s.
	    - also come here upon kill -1 or kill -2 to the server
***/
void ddxGiveUp()        /* Called by GiveUp() */
{
	/* soft reset the ITE by writing ESC-g to standard error */
	write(2, "\033g", 2);
}

int
ddxProcessArgument (argc, argv, i)
    int argc;
    char *argv[];
    int i;

{
  return(0);
}


/**************************************************************
 *
 *   ErrorLine(phpPriv)
 *
 *   This routine prints out a message that an error occurred on a
 *   particular line in the Xnscreens file.  It does not print the 
 *   error message, just the line number and the screen file name.
 *
 *   Parameters:
 *	
 *	A pointer to an hpPrivRec
 *
 *   Return Value:
 *	None.
 *
 ***************************************************************/

void
ErrorLine(phpPriv)

hpPrivPtr phpPriv;

{

    ErrorF("Error in line %d of your %s/X%sscreens file.\n", 
		phpPriv->LineNumber, LIBDIR, display);
    return;

}
