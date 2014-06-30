eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
    & eval 'exec perl -S $0 $argv:q'
    if 0;

# run_test_dii.pl,v 1.3 2001/03/05 19:39:21 brunsch Exp
# -*- perl -*-

system 'perl run_test.pl -i dii ' . join ' ', @ARGV;

