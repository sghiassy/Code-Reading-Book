// test_i.cpp,v 1.3 2001/01/28 03:57:27 othman Exp

#include "test_i.h"

ACE_RCSID (Service_Context_Manipulation,
           test_i,
           "test_i.cpp,v 1.3 2001/01/28 03:57:27 othman Exp")


Visual_i::Visual_i (CORBA::ORB_ptr orb)
  : orb_ (CORBA::ORB::_duplicate (orb))
{
}

void
Visual_i::normal (CORBA::Long arg,
                  CORBA::Environment &)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  ACE_DEBUG ((LM_DEBUG, "Visual::normal called with %d\n", arg));
}

void
Visual_i::nothing (CORBA::Environment &)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  ACE_DEBUG ((LM_DEBUG, "Visual::nothing\n"));
}

void
Visual_i::user (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException,
                   Test_Interceptors::Silly))
{
  ACE_DEBUG ((LM_DEBUG, "Visual::user, throwing Silly\n"));
  ACE_THROW (Test_Interceptors::Silly ());
}

void
Visual_i::system (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  ACE_DEBUG ((LM_DEBUG, "Visual::user, throwing INV_OBJREF\n"));
  ACE_THROW (CORBA::INV_OBJREF ());
}

void
Visual_i::shutdown (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->orb_->shutdown (0, ACE_TRY_ENV);
  ACE_CHECK;
}
