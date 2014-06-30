#!/usr/bin/perl

require "subparseform.lib";
&Parse_Form;

$food = $formdata{'food'};



if ($food eq "spinach") {
	print "Content-type: text/html\n\n";
	print "You ate spinach, so you get dessert!\n";
}