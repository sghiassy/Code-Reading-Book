// test_i.cpp,v 1.7 2001/03/26 21:17:09 coryan Exp

#include "test_i.h"

ACE_RCSID(MT_Client, test_i, "test_i.cpp,v 1.7 2001/03/26 21:17:09 coryan Exp")

Simple_Server_i::Simple_Server_i (CORBA::ORB_ptr orb)
  : orb_ (CORBA::ORB::_duplicate (orb)),
    val_ (1)
{
}

Simple_Server_i::Simple_Server_i (void)
  : orb_ (0)
{
  // no-op
}

CORBA::Long
Simple_Server_i::remote_call (CORBA::Environment &)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  ACE_DEBUG ((LM_DEBUG,
              "Print out from process id (%P) hosting the servant \n"));

  return this->val_++;
}


void
Simple_Server_i::shutdown (CORBA::Environment&)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->orb_->shutdown (0);
}
