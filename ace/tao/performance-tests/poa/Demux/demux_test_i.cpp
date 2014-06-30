//demux_test_i.cpp,v 1.3 2001/03/26 21:17:07 coryan Exp
// Implementation of TAO's Demux_Test interface

#include "demux_test_i.h"
#include "tao/ORB_Core.h"

// ctor
Demux_Test_i::Demux_Test_i (void)
{}

// dtor
Demux_Test_i::~Demux_Test_i (void)
{}

void Demux_Test_i::M302 (CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
}

void Demux_Test_i::shutdown (CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  TAO_ORB_Core_instance ()->orb ()->shutdown ();
}
