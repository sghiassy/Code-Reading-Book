eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
    & eval 'exec perl -S $0 $argv:q'
    if 0;

# run_test.pl,v 1.1 2001/07/12 22:51:34 fhunleth Exp
# -*- perl -*-

use lib '../../../../bin';
use PerlACE::Run_Test;

print STDERR "\n********** RTCORBA ORB_init Unit Test **********\n\n";

$T = new PerlACE::Process ("ORB_init");

$test = $T->SpawnWaitKill (60);

if ($test != 0) {
    print STDERR "ERROR: test returned $test\n";
    exit 1;
}

exit 0;
