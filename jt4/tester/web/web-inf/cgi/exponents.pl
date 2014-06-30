#!/usr/bin/perl

require "subparseform.lib";
&Parse_Form;

$number = $formdata{'number'};
$power = $formdata{'power'};

$result = $number ** $power;

print "Content-type: text/html\n\n";
print "<P>You entered $number with an exponent of $power";
print "<P>$number raised to the $power power is $result.";