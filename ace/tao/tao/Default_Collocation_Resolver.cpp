// Default_Collocation_Resolver.cpp,v 1.4 2001/09/18 00:05:30 irfan Exp

#include "tao/Default_Collocation_Resolver.h"

ACE_RCSID(tao, Default_Collocation_Resolver, "Default_Collocation_Resolver.cpp,v 1.4 2001/09/18 00:05:30 irfan Exp")

#include "tao/Object.h"

#if !defined (__ACE_INLINE__)
# include "tao/Default_Collocation_Resolver.i"
#endif /* ! __ACE_INLINE__ */

CORBA::Boolean
TAO_Default_Collocation_Resolver::is_collocated (CORBA::Object_ptr object,
                                                 CORBA::Environment &) const
{
  return object->_is_collocated ();
}

ACE_STATIC_SVC_DEFINE (TAO_Default_Collocation_Resolver,
                       ACE_TEXT ("Default_Collocation_Resolver"),
                       ACE_SVC_OBJ_T,
                       &ACE_SVC_NAME (TAO_Default_Collocation_Resolver),
                       ACE_Service_Type::DELETE_THIS | ACE_Service_Type::DELETE_OBJ,
                       0)
ACE_FACTORY_DEFINE (TAO, TAO_Default_Collocation_Resolver)
