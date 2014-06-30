// Managed_Types.cpp,v 1.5 1999/10/25 05:27:55 coryan Exp

#include "tao/Managed_Types.h"
#include "tao/ORB.h"

#if !defined (__ACE_INLINE__)
#include "tao/Managed_Types.i"
#endif /* __ACE_INLINE__ */

ACE_RCSID(tao, Managed_Types, "Managed_Types.cpp,v 1.5 1999/10/25 05:27:55 coryan Exp")

// assignment from CORBA::String_var makes a copy
TAO_String_Manager&
TAO_String_Manager::operator= (const CORBA::String_var &var)
{
  CORBA::string_free (this->ptr_);
  this->ptr_ = CORBA::string_dup (var.in ());
  return *this;
}

// assignment from String_var
TAO_SeqElem_String_Manager&
TAO_SeqElem_String_Manager::operator= (const CORBA::String_var &var)
{
  if (this->release_)
    CORBA::string_free (*this->ptr_);
  *this->ptr_ = CORBA::string_dup (var.in ());
  return *this;
}

// ****************************************************************

// assignment from CORBA::WString_var makes a copy
TAO_WString_Manager&
TAO_WString_Manager::operator= (const CORBA::WString_var &var)
{
  CORBA::wstring_free (this->ptr_);
  this->ptr_ = CORBA::wstring_dup (var.in ());
  return *this;
}

// assignment from WString_var
TAO_SeqElem_WString_Manager&
TAO_SeqElem_WString_Manager::operator= (const CORBA::WString_var &var)
{
  if (this->release_)
    CORBA::wstring_free (*this->ptr_);
  *this->ptr_ = CORBA::wstring_dup (var.in ());
  return *this;
}
