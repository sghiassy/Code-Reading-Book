eval '(exit $?0)' && eval 'exec perl -pi -S $0 ${1+"$@"}'
    & eval 'exec perl -pi -S $0 $argv:q'
    if 0;

# tao_env.pl,v 4.1 1999/01/10 04:07:04 nanbor Exp
#
# You may want to run the "find" command with this script, which maybe
# something like this:
#
# find . -type f \( -name "*.i" -o -name "*.h" -o -name "*.C" -o -name "*.cc" -o -name "*.c" -o -name "*.cpp" -o -name "*.hpp" -o -name "*.ipp" \) -print | xargs $ACE_ROOT/bin/auto_ptr.perl

# The first three lines above let this script run without specifying the
# full path to perl, as long as it is in the user's PATH.
# Taken from perlrun man page.

s/^_env/TAO_IN_ENV/;
s/([^_A-Za-z])_env/$1TAO_IN_ENV/g;
