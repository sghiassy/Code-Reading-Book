#!/usr/bin/perl

@days = qw(Sunday Monday Tuesday Wednesday Thursday Friday Saturday);

print "Content-type: text/html\n\n";
print "The first day of the week is $days[0]";

print "<P>The third day of the week is $days[2]";