#!/usr/bin/perl

print "Content-type: text/html\n\n";

@days = ("Monday", 
         "Tuesday", 
         "Wednesday", 
         "Thursday", 
         "Friday", 
         "Saturday", 
         "Sunday");

print "These are the days of the week:";

print "<p>";
print "@days";