//
// Roundtrip.cpp,v 1.1 2001/05/09 22:38:25 coryan Exp
//
#include "Roundtrip.h"

ACE_RCSID(DII_Latency, Roundtrip, "Roundtrip.cpp,v 1.1 2001/05/09 22:38:25 coryan Exp")

Roundtrip::Roundtrip (CORBA::ORB_ptr orb)
  : orb_ (CORBA::ORB::_duplicate (orb))
{
}

Test::Timestamp
Roundtrip::test_method (Test::Timestamp send_time,
                        CORBA::Environment &)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  return send_time;
}

void
Roundtrip::shutdown (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->orb_->shutdown (0, ACE_TRY_ENV);
}
