//
// Simple.cpp,v 1.1 2001/05/09 21:43:06 coryan Exp
//
#include "Simple.h"

ACE_RCSID(Activation, Simple, "Simple.cpp,v 1.1 2001/05/09 21:43:06 coryan Exp")

Simple::Simple (void)
{
}

void
Simple::destroy (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  PortableServer::POA_var poa = this->_default_POA (ACE_TRY_ENV);
  ACE_CHECK;

  PortableServer::ObjectId_var oid =
    poa->servant_to_id (this, ACE_TRY_ENV);
  ACE_CHECK;

  poa->deactivate_object (oid.in (), ACE_TRY_ENV);
  ACE_CHECK;
}
