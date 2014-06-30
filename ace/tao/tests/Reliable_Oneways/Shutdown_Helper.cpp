//
// Shutdown_Helper.cpp,v 1.1 2001/04/10 18:06:57 coryan Exp
//
#include "Shutdown_Helper.h"

ACE_RCSID(Reliable_Oneways, Shutdown_Helper, "Shutdown_Helper.cpp,v 1.1 2001/04/10 18:06:57 coryan Exp")

Shutdown_Helper::Shutdown_Helper (CORBA::ORB_ptr orb)
  : orb_ (CORBA::ORB::_duplicate (orb))
{
}

void
Shutdown_Helper::shutdown (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->orb_->shutdown (0, ACE_TRY_ENV);
}
