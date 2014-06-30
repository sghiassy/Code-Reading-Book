#!/usr/bin/perl

$div = 40 / 5 + 3;
$parens = 40 / (5 + 3);
$subadd = 9 - 3 + 5;
$parens_subadd = 9 - (3 + 5);

print "Content-type: text/html\n\n";
print "div is $div, parens is $parens, subadd is $subadd and parens_subadd is $parens_subadd";