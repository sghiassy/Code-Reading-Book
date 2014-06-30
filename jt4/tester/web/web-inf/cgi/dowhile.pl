#!/usr/bin/perl

require "subparseform.lib";
&Parse_Form;

$start = $formdata{'start'};

print "Content-type: text/html\n\n";
print "<P>Starting countdown...";

do {
	print "$start... ";
	--$start;
} while ($start > 0);

print "KABOOM!";