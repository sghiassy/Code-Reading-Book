eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
    & eval 'exec perl -S $0 $argv:q'
    if 0;

# run_test.pl,v 1.2 2001/05/27 17:08:28 irfan Exp
# -*- perl -*-

use lib "../../../../bin";
use PerlACE::Run_Test;

$iterations = 10000;

print STDERR "================ Reference Creation Test\n";

print STDERR "\nTesting with hints (default configuration)\n";

$CR = new PerlACE::Process ("create_reference", " -i $iterations");
$status = $CR->SpawnWaitKill (120);

if ($status != 0) {
    print STDERR "ERROR: create_reference returned $status\n";
    exit $status;
}

print STDERR "\nTesting without hints (optimized configuration)\n";

$no_active_hint_directive = "-ORBsvcconfdirective \"static Server_Strategy_Factory '-ORBActiveHintInIds 0'\"";

$CR = new PerlACE::Process ("create_reference", " -i $iterations $no_active_hint_directive");
$status = $CR->SpawnWaitKill (120);

if ($status != 0) {
    print STDERR "ERROR: create_reference returned $status\n";
    exit $status;
}

exit $status;
