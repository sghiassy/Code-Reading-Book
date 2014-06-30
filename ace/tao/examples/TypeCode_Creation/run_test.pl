eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
    & eval 'exec perl -S $0 $argv:q'
    if 0;

# run_test.pl,v 1.1 2001/04/03 01:24:26 parsons Exp
# -*- perl -*-

use lib "../../../bin";
use PerlACE::Run_Test;

$status = 0;

$CL = new PerlACE::Process ("create_tc");

$client = $CL->SpawnWaitKill (60);

if ($client != 0) {
    print STDERR "ERROR: client returned $client\n";
    $status = 1;
}

exit $status;

