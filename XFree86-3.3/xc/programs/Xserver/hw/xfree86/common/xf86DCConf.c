/* $XFree86: xc/programs/Xserver/hw/xfree86/common/xf86DCConf.c,v 3.2 1997/01/05 11:58:05 dawes Exp $ */

#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#else
extern double atof();
#endif

#include "X.h"
#include "Xproto.h"
#include "Xmd.h"
#include "input.h"
#include "servermd.h"
#include "scrnintstr.h"
#include "compiler.h"
#include "xf86Procs.h"
#include "xf86_OSlib.h"

#include "xf86.h"
#include "xf86_Config.h"
#define DCConfig
#include "xf86DCConf.h"

static char *configPath;
static char *configBuf;
static char *configRBuf;
static int configStart, configPos, configLineNo ;

#define CONFIG_BUF_LEN     1024

/* 
 * 3 functions to aid the drivers interpreting the driver config 
 * areas. xf86DCGetToken() works the same way as xf86GetToken().
 * It has to be called by the device driver with three arguments:
 * 1. a pointer to the area containing the config strings,
 * 2. a table containing the tokens recognized by the driver,
 * 3. a table containing the tokens for the specific section which 
 *    have already been interpreted in xf86Config.c.
 * XF86DCGetOption() will return a pointer of type OFlagSet. All the 
 * Flags contained in pointer and specified in tab will be set.
 * Both functions will free the memory areas pointed to by pointer
 * when end mark (EOF) is encountered.
 * xf86DCConfigError() replaces xf86ConfigError().
 */

int
xf86DCGetToken(Pointer, tab, otab)
char* Pointer;
SymTabRec tab[];
SymTabRec otab[];
{
  int          c, i;

  static int pos = 0;
  char *currpointer;
  char dummy ='\0';

  int  token = 0;
  Bool err = FALSE;

  if(Pointer == NULL) return(EOF);

  if(!pos){  /* first time: initialize */
    configPath = Pointer;
    pos = strlen(configPath) + 1;
    DCpushToken = LOCK_TOKEN;
    configRBuf = (char*)xalloc(CONFIG_BUF_LEN);
    configPos = 0;  /* reset counter */
    configBuf = &dummy; /* invalidize buffer */
  }

  /* 
   * Our strategy is simple: find next token, see if we have a token
   * already interpreted in xf86Config if yes ignore it as well as
   * any subsequent STRING, NUMBER, DASH, COMMA tokens until we encounter
   * a new token.
   */
  do{
    token = 0;
    if (DCpushToken == EOF) return(EOF);  /* handle EOF */
    else if (DCpushToken == LOCK_TOKEN)   /* handle push token */
      {
	c = configBuf[configPos];
	do {  /* get next token */
	  if (!c)  {
	    currpointer = Pointer + pos;  /* locate current position */
	    if(* currpointer == EOF){       /* handle EOF */ 
	      xfree(Pointer);
	      xfree(configRBuf);
	      pos = 0;
	      return( DCpushToken = EOF );
	    }
	    memcpy(&configLineNo, currpointer, sizeof(int));
	    currpointer += sizeof(int);
	    pos += (sizeof(int) + strlen(currpointer) + 1);
	    configBuf = currpointer;      /* copy line       */
	    configStart = configPos = 0;
#ifdef DEBUG
	    ErrorF("line: %i: %s",configLineNo,configBuf);
#endif
	  }
#ifndef __EMX__
	while (((c=configBuf[configPos++])==' ') || ( c=='\t') || ( c=='\n'));
#else
	while (((c=configBuf[configPos++])==' ') || ( c=='\t') || ( c=='\n') 
		|| (c=='\r'));
#endif
	  if (c == '#') c = '\0';  /* ignore comments */ 
	} while (!c);
	/* handle '-' and ','  */
	if ( (c == ',') && !isalpha(configBuf[configPos]) ) {
	  configStart = configPos; 
	  token = COMMA;
	} else if ( (c == '-') && !isalpha(configBuf[configPos]) ) {
	  configStart = configPos; 
	  token = DASH;
	} else {
	  configStart = configPos;
	  if (isdigit(c))  /* Numbers */
	    {
	      int base;
	      
	      if (c == '0')
		if ((configBuf[configPos] == 'x') || 
		    (configBuf[configPos] == 'X'))
		  base = 16;
		else
		  base = 8;
	      else
		base = 10;

	      configRBuf[0] = c; i = 1;
	      while (isdigit(c = configBuf[configPos++]) || 
		     (c == '.') || (c == 'x') || 
		     ((base == 16) && (((c >= 'a') && (c <= 'f')) ||
				       ((c >= 'A') && (c <= 'F')))))
		configRBuf[i++] = c;
	      configPos--; /* GJA -- one too far */
	      configRBuf[i] = '\0';
	      DCval.num = StrToUL(configRBuf);
	      DCval.realnum = atof(configRBuf);
	      token = NUMBER; 
	  } else if (c == '\"')   /* Strings */
	    {
	      i = -1;
	      do {
		configRBuf[++i] = (c = configBuf[configPos++]);
#ifndef __EMX__
	      } while ((c != '\"') && (c != '\n') && (c != '\0'));
#else
	    } while ((c != '\"') && (c != '\n') && (c != '\r') && (c != '\0'));
#endif
	      configRBuf[i] = '\0';
	      DCval.str = (char *)xalloc(strlen(configRBuf) + 1);
	      strcpy(DCval.str, configRBuf);      /* private copy ! */
	      token = STRING;
	  } else { 	   /* ... and now we MUST have a valid token. */
	      configRBuf[0] = c;
	      i = 0;
	      do {
		configRBuf[++i] = (c = configBuf[configPos++]);;
#ifndef __EMX__
	     } while ((c != ' ') && (c != '\t') && (c != '\n') && (c != '\0'));
#else
	     } while ((c != ' ') && (c != '\t') && (c != '\n') && (c != '\r') && (c != '\0') );
#endif
	      configRBuf[i] = '\0'; i=0;
	    }
	}
      } else {  /* pushed tokens */
    
      int temp = DCpushToken;
      DCpushToken = LOCK_TOKEN;
    
      if (temp == COMMA || temp == DASH) {
	token = temp;
      } else if (temp == NUMBER || temp == STRING) {
	token = temp;
      }
    }
 
  /* look up tokens */
  if (tab && !token) /* see if token is in tab */
    {
      i = 0;
      while (tab[i].token != -1)
	if (StrCaseCmp(configRBuf,tab[i].name) == 0){
	  token = tab[i].token;
	  err = FALSE;     /* ok, this is a valid token */
	  break;
	}
	else
	  i++;
    }
/* we got a token _and_ it isn't a STRING, NUMBER, DASH, COMMA 
 * following a otab[] token */
if(!err && token)
{
#ifdef DEBUG
  switch(token){
  case DASH:
    ErrorF("Token: DASH\n");
    break;
  case COMMA:
    ErrorF("Token: COMMA\n");
    break;
  case NUMBER:
    ErrorF("Token: NUMBER = %i, %f\n", DCval.num, DCval.realnum);
    break;
  case STRING:
    ErrorF("Token: STRING = %s\n",DCval.str);
    break;
  default:
    ErrorF("Token: TOKEN = %s\n",configRBuf);
  }
#endif
 return (token); 
};

/* still no token - see if this was interpreted already in xf86Config.c: */
/* if so eat it up including any subsequent STRING, NUMBER, DASH, COMMA  */

if(otab && !token)  
  {         
    err = FALSE;          /* assume unknown token */
    i = 0;
while ( otab[i].token != -1)
	if (StrCaseCmp(configRBuf,otab[i].name) == 0){
	  err = TRUE;     /* found it: earmark to skip it */
	  break;
	}
	else
	  i++;
  }
  } while(err == TRUE);           /* skip any unwanted tokens      */
  return(ERROR_TOKEN);       /* Error catcher */
}

OFlagSet *
xf86DCGetOption(Pointer, tab)
char* Pointer;
OptFlagRec tab[]; 
{
    char *currpointer;
    OFlagSet *Flag;
    int addlen; 
    int pos = 0;
    char *configPath;
    int lineno;
    int i;

    Flag = (OFlagSet *)xalloc(sizeof(OFlagSet));
    OFLG_ZERO(Flag);
    if (Pointer == NULL)
      return(Flag);
    
    configPath = Pointer;
    pos = strlen(configPath) + 1;

    while(1)
      {
	currpointer = Pointer + pos;
	if (* currpointer == EOF)
	  break;
	lineno = *(int *)currpointer;
	currpointer += sizeof(int);
	addlen = sizeof(int) + strlen(currpointer) + 1;
	i = 0;
	while (tab && tab[i].token != -1) 
	  {
	    if (StrCaseCmp(currpointer, tab[i].name) == 0)
	      {
		OFLG_SET(tab[i].token, Flag);
		break;
	      }
	    i++;
	  }
#ifdef DEBUG
	ErrorF("line: %i token: %i ;%s\n",lineno,tab[i].token,currpointer);
#endif
	if (!tab || tab[i].token == -1)
	  ErrorF("\nConfig Error: %s:%i:\n    Unknown option string: \"%s\"\n",
		 configPath,lineno,currpointer);
	pos += addlen;
      };
    xfree(Pointer);
    return(Flag);
  }

void 
xf86DCConfigError(msg)
     char *msg;
{
  int i,j;

  ErrorF( "\nConfig Error: %s:%d\n\n%s", configPath, configLineNo, configBuf);
  for (i = 1, j = 1; i < configStart; i++, j++) 
    if (configBuf[i-1] != '\t')
      ErrorF(" ");
    else
      do
	ErrorF(" ");
      while (((j++)%8) != 0);
  for (i = configStart; i <= configPos; i++) ErrorF("^");
  ErrorF("\n%s\n", msg);
  return;
}

