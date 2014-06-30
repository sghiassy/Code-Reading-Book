eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
    & eval 'exec perl -S $0 $argv:q'
    if 0;

# run_test.pl,v 1.2 2001/03/05 03:40:39 brunsch Exp
# -*- perl -*-

use lib "../../bin";
use PerlACE::Run_Test;

$TEST = new PerlACE::Process ("test");

$status = $TEST->SpawnWaitKill (20);

$status = 1 if ($status < 0);

exit $status;
