//
// Receiver_Factory.cpp,v 1.1 2001/04/19 02:20:56 coryan Exp
//
#include "Receiver_Factory.h"
#include "Receiver.h"

ACE_RCSID(Throughput, Receiver_Factory, "Receiver_Factory.cpp,v 1.1 2001/04/19 02:20:56 coryan Exp")

Receiver_Factory::Receiver_Factory (CORBA::ORB_ptr orb)
  : orb_ (CORBA::ORB::_duplicate (orb))
{
}

Test::Receiver_ptr
Receiver_Factory::create_receiver (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  Receiver *receiver_impl;
  ACE_NEW_THROW_EX (receiver_impl,
                    Receiver,
                    CORBA::NO_MEMORY ());
  ACE_CHECK_RETURN (Test::Receiver::_nil ());
  PortableServer::ServantBase_var transfer_ownership(receiver_impl);

  return receiver_impl->_this (ACE_TRY_ENV);
}

void
Receiver_Factory::shutdown (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->orb_->shutdown (0, ACE_TRY_ENV);
}
