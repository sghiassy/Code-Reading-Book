#!/usr/bin/perl

require "subparseform.lib";
&Parse_Form;

$first = $formdata{'first_name'};
$married = $formdata{'fiance_last'};

$fullname = $first . " " . $married;

print "Content-type: text/html\n\n";
print "Congratulations! Your married name would be $fullname.";