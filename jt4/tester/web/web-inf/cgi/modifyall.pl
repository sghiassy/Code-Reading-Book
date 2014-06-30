#!/usr/bin/perl

require "subparseform.lib";
&Parse_Form;

@numbers = split(/,/, $formdata{'number'});
print "Content-type: text/html\n\n";

print "The numbers you entered were:";
foreach $number (@numbers) {
	print "<LI>$number";
	}
	
foreach $number(@numbers) {
	$number = sqrt($number);
	}

print "<P>The square roots of those numbers are: ";

foreach $number(@numbers) {
	print "<LI>$number";
	}