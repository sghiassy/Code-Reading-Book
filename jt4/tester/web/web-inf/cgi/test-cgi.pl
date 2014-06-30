#!/usr/local/bin/perl
#
# @(#)test-cgi.pl	1.2 98/05/28
#
# Copyright (c) 1996-1998 Sun Microsystems, Inc. All Rights Reserved.
#
# Permission to use, copy, modify, and distribute this software
# and its documentation for NON-COMMERCIAL purposes and without
# fee is hereby granted provided that this copyright notice
# appears in all copies. Please refer to the file "copyright.html"
# for further important copyright and licensing information.
#
# SUN MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF
# THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
# TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
# PARTICULAR PURPOSE, OR NON-INFRINGEMENT. SUN SHALL NOT BE LIABLE FOR
# ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR
# DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.
#

print <<EOM;
Content-type: text/html

<html>
<head><title>CGI Test</title></head>
<body>
<h1>CGI Test</h1>
<pre>
EOM

print "argc is " . ($#ARGV + 1) . "\n\n";

print "argv is\n";

for($i = 0; $i<($#ARGV+1); $i++) {
	print $ARGV[$i] . "\n";
}

print "\n";
	
foreach $key (sort keys %ENV) {

	print("$key: $ENV{$key}\n");

}

print <<EOM;
</pre>
</body>
</html>
EOM

close OUT;