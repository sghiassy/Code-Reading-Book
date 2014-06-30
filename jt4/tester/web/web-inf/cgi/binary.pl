#!/usr/bin/perl

require "subparseform.lib";
&Parse_Form;

$counter = $formdata{'counter'};

$counter +=1;

print "Content-type: text/html\n\n";
print "<FONT SIZE=+2>If you add one to your number, the result is <B>$counter</B>";

$counter +=5;
print "<P>If you add 5 to that, the result is <B>$counter</B></FONT>";