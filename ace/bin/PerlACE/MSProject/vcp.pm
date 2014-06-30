# VCP.pm,v 1.1 2001/03/01 21:39:06 brunsch Exp

package PerlACE::MSProject::VCP;

use strict;
use PerlACE::MSProject;

our @ISA = ("PerlACE::MSProject");

###############################################################################

# Constructor

sub new  
{
    my $proto = shift;
    my $class = ref ($proto) || $proto;
    my $self = $class->SUPER::new (@_);
    
    $self->{COMPILER} = "evc.com";

    bless ($self, $class);
    return $self;
}

###############################################################################

# Accessors

1;