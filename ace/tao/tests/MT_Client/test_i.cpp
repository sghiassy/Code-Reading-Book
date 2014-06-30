// test_i.cpp,v 1.7 2001/03/29 18:17:52 coryan Exp

#include "test_i.h"

#if !defined(__ACE_INLINE__)
#include "test_i.i"
#endif /* __ACE_INLINE__ */

ACE_RCSID(MT_Client, test_i, "test_i.cpp,v 1.7 2001/03/29 18:17:52 coryan Exp")

void
Simple_Server_i::test_method (CORBA::Environment&)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
}

void
Simple_Server_i::shutdown (CORBA::Environment &ACE_TRY_ENV)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->orb_->shutdown (0, ACE_TRY_ENV);
}
