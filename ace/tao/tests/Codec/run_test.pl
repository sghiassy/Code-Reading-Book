eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
    & eval 'exec perl -S $0 $argv:q'
    if 0;

# -*- perl -*-
# run_test.pl,v 1.2 2001/03/05 03:23:47 brunsch Exp

use lib '../../../bin';
use PerlACE::Run_Test;

print STDERR "\n\n==== Running Codec test\n";

$T = new PerlACE::Process ("client");

$test = $T->SpawnWaitKill (15);

if ($test != 0) {
    print STDERR "ERROR: Codec test returned $test\n";
    exit 1;
}

exit 0;
