//
// Blocking_Sync_None.cpp,v 1.1 2001/04/11 16:59:59 coryan Exp
//
#include "Blocking_Sync_None.h"

ACE_RCSID(Blocking_Sync_None, Blocking_Sync_None, "Blocking_Sync_None.cpp,v 1.1 2001/04/11 16:59:59 coryan Exp")

Blocking_Sync_None::Blocking_Sync_None (CORBA::ORB_ptr orb)
  : orb_ (CORBA::ORB::_duplicate (orb))
{
}

void
Blocking_Sync_None::slow_operation (const Test::Payload &,
                                    CORBA::ULong sleep_microseconds,
                                    CORBA::Environment &)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  ACE_Time_Value sleep_time (0, sleep_microseconds);
  ACE_OS::sleep (sleep_time);
}

void
Blocking_Sync_None::shutdown (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->orb_->shutdown (0, ACE_TRY_ENV);
}
