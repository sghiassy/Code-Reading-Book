//
// Hello.cpp,v 1.1 2001/03/23 17:09:17 coryan Exp
//
#include "Hello.h"

ACE_RCSID(Hello, Hello, "Hello.cpp,v 1.1 2001/03/23 17:09:17 coryan Exp")

Hello::Hello (CORBA::ORB_ptr orb)
  : orb_ (CORBA::ORB::_duplicate (orb))
{
}

char *
Hello::get_string (CORBA::Environment &)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  return CORBA::string_dup ("Hello there!");
}

void
Hello::shutdown (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->orb_->shutdown (0, ACE_TRY_ENV);
}
