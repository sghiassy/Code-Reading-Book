#!/usr/bin/perl
#
# Find a pattern in a the book's source collection (DOS/Windows version)
#
# (C) Copyright 2000-2002 Diomidis Spinellis
# 
# Permission to use, copy, and distribute this software and its
# documentation for any purpose and without fee is hereby granted,
# provided that the above copyright notice appear in all copies and that
# both that copyright notice and this permission notice appear in
# supporting documentation.
# 
# THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
# MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#

# Heuristic to see if we are running under Windows / DOS or Unix
$windows = (-r 'nul' && !-r '/dev/null');

if ($#ARGV != 2) {
	print STDERR "Usage:\t\t$0 source-directory file-suffix-regex code-regex\n";
	if ($windows) {
		print STDERR "Example:\tperl $0 d:\\ace \\.[ch] main\n";
	} else {
		print STDERR "Example:\t$0 d:\\booksrc\\ace '\\.[ch]' main\n";
	}
	exit 1;
}

$path = shift(@ARGV);
$filepat = shift(@ARGV);
$pat = shift(@ARGV);

# Open pipe to get the list of files
if ($windows) {
	# Probably DOS/Windows
	open(FLIST, "dir /b/s $path |") || die "Unable to get file list using dir: $!\n";
} else {
	# Hope it's Unix
	open(FLIST, "find $path -print |") || die "Unable to get file list using find: $!\n";
}

file: while (<FLIST>) {
	chop;
	next unless (/${filepat}$/);
	if ($windows) {
		# Exclude implicit device names (e.g. /foo/prn.c)
		next if (/\\com\d/i);
		next if (/\\lpt\d/i);
		next if (/\\prn/i);
		next if (/\\aux/i);
	}
	next unless (-f $_);
	if (!open(FCONT, $fname = $_)) {
		print STDERR "Unable to open file $_:$!\n";
		next file;
	}
	while (<FCONT>) {
		print "$fname: $_" if ($_ =~ m/$pat/o);
	}
}
