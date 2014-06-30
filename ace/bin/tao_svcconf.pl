eval '(exit $?0)' && eval 'exec perl -pi -S $0 ${1+"$@"}'
    & eval 'exec perl -pi -S $0 $argv:q'
    if 0;

# tao_svcconf.pl,v 1.2 1999/04/01 09:31:25 nanbor Exp
#
# This script can help you convert TAO svc.conf using dynamic services
# to use static services.
#
# You may want to run the "find" command with this script, which maybe
# something like this:
#
# find . -name svc.conf -print | xargs $ACE_ROOT/bin/tao_svcconf.pl

s/^ *dynamic *([A-Za-z_]+) [^\"]+(\"[^\"]+\").*/static $1 $2/;
