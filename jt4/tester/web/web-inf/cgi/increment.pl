#!/usr/bin/perl

require "subparseform.lib";
&Parse_Form;
print "Content-type: text/html\n\n";

$counter = $formdata{'counter'};

++$counter;
print "Your number incremented by 1 is $counter";
$watch = ++$counter;
print "<BR>Your number, incremented again by 1, is now $counter. If we store that operation, its value is also $watch.";

$counter = $formdata{'counter'};
print "<HR>Let's start over, with your original number $counter";
$counter++;
print "<BR>Again, your number incremented by 1 is $counter";
$watch = $counter++;
print "<BR>Now we store the value of your number in a second variable, which is now equal to $watch, and then we increment your number again. It's now $counter.";