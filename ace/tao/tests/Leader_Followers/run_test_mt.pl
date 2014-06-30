eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
    & eval 'exec perl -S $0 $argv:q'
    if 0;

# run_test_mt.pl,v 1.1 2001/03/05 19:39:21 brunsch Exp
# -*- perl -*-

system 'perl run_test.pl -m ' . join ' ', @ARGV;


