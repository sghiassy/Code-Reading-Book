/* This is OS/2 Rexx, written for OS/2 to avoid need for secondary shell */
/* $XFree86: xc/programs/Xserver/hw/xfree86/mono/confmono.cmd,v 3.0 1995/03/11 14:14:19 dawes Exp $ */
/*
 * This script generates monoConf.c
 *
 * usage: confmono driver1 driver2 ...
 */
'@echo off'
PARSE UPPER ARG all

monoconf = 'monoConf.c'

CALL LineOut monoconf, '/*'
CALL LineOut monoconf, ' * This file is generated automatically -- DO NOT EDIT'
CALL LineOut monoconf, ' */'
CALL LineOut monoconf, ' '
CALL LineOut monoconf, '#include "mono.h"'
CALL LineOut monoconf, ' '
CALL LineOut monoconf, 'extern monoVideoChipRec'

DO i=1 TO WORDS(all)
    arg = WORD(all,i)
    IF i = WORDS(all) THEN
	CALL LineOut monoconf, '        'arg';'
    ELSE
	CALL LineOut monoconf, '        'arg','
END
CALL LineOut monoconf, 'monoVideoChipPtr monoDrivers[] ='
CALL LineOut monoconf, '{'
DO i=1 TO WORDS(all)
    arg = WORD(all,i)
	CALL LineOut monoconf, '        &'arg','
END
CALL LineOut monoconf, '        NULL'
CALL LineOut monoconf, '};'
CALL LineOut monoconf
EXIT
