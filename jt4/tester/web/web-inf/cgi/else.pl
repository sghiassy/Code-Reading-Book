#!/usr/bin/perl

require "subparseform.lib";
&Parse_Form;

$food = $formdata{'food'};

print "Content-type: text/html\n\n";

if ($food eq "spinach") {
	
	print "You ate spinach, so you get dessert!";
} else {
	print "No spinach, no dessert!";
}