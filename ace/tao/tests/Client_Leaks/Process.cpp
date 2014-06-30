//
// Process.cpp,v 1.1 2001/03/10 10:14:45 coryan Exp
//
#include "Process.h"

ACE_RCSID(Client_Leaks, Process, "Process.cpp,v 1.1 2001/03/10 10:14:45 coryan Exp")

Process::Process (CORBA::ORB_ptr orb)
  : orb_ (CORBA::ORB::_duplicate (orb))
{
}

CORBA::Long
Process::get_process_id (CORBA::Environment &)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  return ACE_OS::getpid ();
}

void
Process::shutdown (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->orb_->shutdown (0, ACE_TRY_ENV);
  ACE_CHECK;
}
