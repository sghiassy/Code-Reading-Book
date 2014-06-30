#!/pkg/gnu/bin//perl5
#
#killbench.pl,v 1.2 2000/06/04 22:00:03 brunsch Exp
#

push(@INC, "$wd/bin");
require('WebStone-common.pl');

html_begin();

print CLIENT "<P>Killing WebStone processes<PRE>";
system("$wd/bin/killbench");
print CLIENT "</PRE><P>Done.";

html_end();

# end
