#!/usr/bin/perl

require "subparseform.lib";
&Parse_Form;

$start = $formdata{'start'};

print "Content-type: text/html\n\n";
print "<P>Starting countdown...";

for ($i = $start; $i > 0; --$i) {
	print "$i... ";
} 

print "KABOOM!";