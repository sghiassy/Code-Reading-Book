// Collocation_Resolver.cpp,v 1.4 2001/09/18 00:05:30 irfan Exp

#include "tao/Collocation_Resolver.h"
#include "ace/Dynamic_Service.h"

ACE_RCSID(tao, Collocation_Resolver, "Collocation_Resolver.cpp,v 1.4 2001/09/18 00:05:30 irfan Exp")

#if !defined (__ACE_INLINE__)
# include "tao/Collocation_Resolver.i"
#endif /* ! __ACE_INLINE__ */

TAO_Collocation_Resolver::~TAO_Collocation_Resolver (void)
{
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class ACE_Dynamic_Service<TAO_Collocation_Resolver>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate ACE_Dynamic_Service<TAO_Collocation_Resolver>

#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
