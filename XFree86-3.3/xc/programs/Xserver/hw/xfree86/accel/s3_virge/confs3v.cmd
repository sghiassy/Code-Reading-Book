/* This is OS/2 Rexx, written for OS/2 to avoid need for secondary shell */
/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/confs3v.cmd,v 3.0 1996/10/03 08:33:09 dawes Exp $ */
/*
 * This script generates s3vConf.c
 *
 * usage: confs3v driver1 driver2 ...
 */
'@echo off'
PARSE UPPER ARG all

s3conf = 's3vConf.c'

CALL LineOut s3conf, '/*'
CALL LineOut s3conf, ' * This file is generated automatically -- DO NOT EDIT'
CALL LineOut s3conf, ' */'
CALL LineOut s3conf, ' '
CALL LineOut s3conf, '#include "s3v.h"'
CALL LineOut s3conf, ' '
CALL LineOut s3conf, 'extern s3VideoChipRec'

DO i=1 TO WORDS(all)
    arg = WORD(all,i)
    IF i = WORDS(all) THEN
	CALL LineOut s3conf, '        'arg';'
    ELSE
	CALL LineOut s3conf, '        'arg','
END
CALL LineOut s3conf, 's3VideoChipPtr s3Drivers[] ='
CALL LineOut s3conf, '{'
DO i=1 TO WORDS(all)
    arg = WORD(all,i)
	CALL LineOut s3conf, '        &'arg','
END
CALL LineOut s3conf, '        NULL'
CALL LineOut s3conf, '};'
CALL LineOut s3conf
EXIT
