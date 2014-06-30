/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/chips/ct_config.c,v 3.1 1996/12/09 11:53:56 dawes Exp $ */

#include "X.h"

#include "input.h"
#include "screenint.h"
#include "xf86.h"
#include "xf86_Option.h"
#include "xf86_Config.h"
#include "xf86Priv.h"
#include "compiler.h"
#include "xf86DCConf.h"

#include "vga.h"

/* This is a sample implementation of a driver config file */

OptFlagRec tab[] = {
  { "test",    1 },
  { "",       -1 },
};

static SymTabRec ct_tab[] = {
  { 1,  "test" },
  { -1, ""},
};


void ctConfig()
{
  char *DCPointer, *OPointer;
  int lineno;
  int token;
  OFlagSet *OFlags; 
  
  DCPointer = vga256InfoRec.DCConfig;
  OPointer = vga256InfoRec.DCOptions;
  while ((token = xf86DCGetToken(DCPointer,ct_tab,DeviceTab)) != EOF){
    switch (token) {
    case STRING:
      ErrorF("string: %s\n",DCval.str);
      break;
    case NUMBER:
      ErrorF("number: int: %i; float %f\n",DCval.num,DCval.realnum);
      break;
    case ERROR_TOKEN:
      ErrorF("error_token\n");
      xf86DCConfigError("Error Token\n");
      break;
    case DASH:
      ErrorF("dash\n");
      break;
    case COMMA:
      ErrorF("Comma\n");
      break;
    case 1:
      ErrorF("valid token:  ");
      token = xf86DCGetToken(DCPointer,NULL,DeviceTab);
      if(token == NUMBER)
	ErrorF("argument: %i, %f\n",DCval.num, DCval.realnum);
      else {
	DCpushToken = token;
	ErrorF("\n");
      }
      break;
    default:
      ErrorF("default\n");
    }
 }

OFlags = xf86DCGetOption(OPointer, tab);
vga256InfoRec.DCConfig = NULL;
vga256InfoRec.DCOptions = NULL;
}


