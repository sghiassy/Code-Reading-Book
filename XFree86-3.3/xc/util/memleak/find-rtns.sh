#!/bin/sh
#
# $XConsortium Exp $
#
# find-routines - convert leak tracer stack traces into file/lineno traces
# 		  using a modified version of gdb-4.4
#
# Usage: find-routines <program-name> {leak-tracing-output-files}
#
TMP1=find-routine.tmp1
TMP=find-routine.tmp
trap "rm -f $TMP $TMP1" 0
OBJ=$1
shift
grep 'return stack:' $* | 
	tr ' ' '\012' | 
	grep 0x | sort -u | sed 's;^;x/i ;' |
	gdb $OBJ | grep '>:' | 
	sed 's/>.*$/>/' | sed 's/(gdb) //' > $TMP1

awk '/^"/ { printf("s;%s;%s line %s %s;\n", $4, $1, $3, $5) }
/^0/ { printf("s;%s;%s %s;\n", $1, $2, $1);}' $TMP1 > $TMP

awk '/return stack/	{ printf ("return stack\n");
			  for (i = 3; i <= NF; i++)
				printf ("\troutine %s\n", $i); }
     /^[A-Z]/		{ print }' $* |
	sed -f $TMP
