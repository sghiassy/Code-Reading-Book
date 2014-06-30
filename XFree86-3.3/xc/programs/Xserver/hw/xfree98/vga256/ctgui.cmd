/* This is OS/2 Rexx, written for OS/2 to avoid need for secondary shell */
/* $XFree86: xc/programs/Xserver/hw/xfree98/vga256/ctgui.cmd,v 3.0 1996/03/29 22:18:58 dawes Exp $ */
/*
 * This script generates vga256Conf.c
 *
 * usage: confvga256 driver1 driver2 ...
 */
'@echo off'
PARSE UPPER ARG all

vgaconf = 'tguiConf.c'

CALL LineOut vgaconf, '/*'
CALL LineOut vgaconf, ' * This file is generated automatically -- DO NOT EDIT'
CALL LineOut vgaconf, ' */'
CALL LineOut vgaconf, ' '
CALL LineOut vgaconf, '#include "xf86.h"'
CALL LineOut vgaconf, '#include "vga.h"'
CALL LineOut vgaconf, ' '
CALL LineOut vgaconf, 'extern vgaVideoChipRec'

DO i=1 TO WORDS(all)
    arg = WORD(all,i)
    IF i = WORDS(all) THEN
	CALL LineOut vgaconf, '        'arg';'
    ELSE
	CALL LineOut vgaconf, '        'arg','
END
CALL LineOut vgaconf, 'vgaVideoChipPtr vgaDrivers[] ='
CALL LineOut vgaconf, '{'
DO i=1 TO WORDS(all)
    arg = WORD(all,i)
	CALL LineOut vgaconf, '        &'arg','
END
CALL LineOut vgaconf, '        NULL'
CALL LineOut vgaconf, '};'
CALL LineOut vgaconf
EXIT
