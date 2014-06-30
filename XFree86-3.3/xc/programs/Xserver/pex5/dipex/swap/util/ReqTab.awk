# $XConsortium: ReqTab.awk,v 5.2 94/04/17 20:36:38 rws Exp $
###########################################################################
## Copyright 1990, 1991 by Sun Microsystems, Inc. and the X Consortium.
## 
##                         All Rights Reserved
## 
## Permission to use, copy, modify, and distribute this software and its 
## documentation for any purpose and without fee is hereby granted, 
## provided that the above copyright notice appear in all copies and that
## both that copyright notice and this permission notice appear in 
## supporting documentation, and that the names of Sun Microsystems
## and the X Consortium not be used in advertising or publicity 
## pertaining to distribution of the software without specific, written 
## prior permission.  
## 
## SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
## INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
## SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
## DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
## WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
## ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
## SOFTWARE.
## 
###########################################################################
## Usage:
##	awk -f ReqTab.awk <pex-include-path>/PEX.h > <output_file>
##
BEGIN { num = 0; lend = 0;
    print "/* Automatically generated Request table"
    print " */"
    print "/******************************************************************"
    print "Copyright 1990, 1991 by Sun Microsystems, Inc. and the X Consortium."
    print ""
    print "                        All Rights Reserved"
    print ""
    print "Permission to use, copy, modify, and distribute this software and its "
    print "documentation for any purpose and without fee is hereby granted, "
    print "provided that the above copyright notice appear in all copies and that"
    print "both that copyright notice and this permission notice appear in "
    print "supporting documentation, and that the names of Sun Microsystems "
    print "and the X Consortium not be used in advertising or publicity "
    print "pertaining to distribution of the software without specific, written "
    print "prior permission.  "
    print ""
    print "SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, "
    print "INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT "
    print "SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL "
    print "DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,"
    print "WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,"
    print "ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS"
    print "SOFTWARE."
    print ""
    print "******************************************************************/"
    print "#ifdef SWAP_FUNC_PREFIX"
    print "#undef SWAP_FUNC_PREFIX"
    print "#endif"
    print "#define SWAP_FUNC_PREFIX(nm) nm"
    }

 $1 == "#define" {
    lend = index($2,"PEX_")
    if ((lend > 0) && ($2 != "PEX_H") && ($2 !~ /NAME/) && ($2 !~ /PROTO/) && ($2 !~ /BIT/) && ($2 !~ /MASK/)) {
	names[num] = substr($2,(lend+4),(length($2)-4))
	num++ } }
END {  
    num--
    print "\nLOCAL_FLAG ErrorCode"
    print "\tSWAP_FUNC_PREFIX(PEXRequestUnused) (),"
    for (i=0; i<num; i++) {
	print "\tSWAP_FUNC_PREFIX(PEX" names[i] ") ()," }
    print "\tSWAP_FUNC_PREFIX(PEX" names[num] ") ();\n"
    print "RequestFunction SWAP_FUNC_PREFIX(PEXRequest)[] = {"
    print "/*   0\t*/  SWAP_FUNC_PREFIX(PEXRequestUnused),"
    for (i=0; i<num; i++) {
	print "/*   " (i+1) "\t*/  SWAP_FUNC_PREFIX(PEX" names[i] ")," }
    print "/*   " (num+1) "\t*/  SWAP_FUNC_PREFIX(PEX" names[num] ")"
    print "};"
    print "\n#undef SWAP_FUNC_PREFIX\n"
}
