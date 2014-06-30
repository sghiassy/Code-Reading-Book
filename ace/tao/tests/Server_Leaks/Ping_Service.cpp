//
// Ping_Service.cpp,v 1.1 2001/03/10 23:37:41 coryan Exp
//
#include "Ping_Service.h"

ACE_RCSID(Server_Leaks, Ping_Service, "Ping_Service.cpp,v 1.1 2001/03/10 23:37:41 coryan Exp")

Ping_Service::Ping_Service (CORBA::ORB_ptr orb)
  : orb_ (CORBA::ORB::_duplicate (orb))
{
}

void
Ping_Service::ping (CORBA::Environment &)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
}

void
Ping_Service::shutdown (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->orb_->shutdown (0, ACE_TRY_ENV);
}
