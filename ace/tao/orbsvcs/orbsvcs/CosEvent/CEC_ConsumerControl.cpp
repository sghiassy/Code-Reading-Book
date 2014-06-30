// CEC_ConsumerControl.cpp,v 1.2 2000/02/24 02:49:41 coryan Exp

#include "CEC_ConsumerControl.h"

#if ! defined (__ACE_INdLINE__)
#include "CEC_ConsumerControl.i"
#endif /* __ACE_INLINE__ */

ACE_RCSID(CosEvent, CEC_ConsumerControl, "CEC_ConsumerControl.cpp,v 1.2 2000/02/24 02:49:41 coryan Exp")

TAO_CEC_ConsumerControl::TAO_CEC_ConsumerControl (void)
{
}

TAO_CEC_ConsumerControl::~TAO_CEC_ConsumerControl (void)
{
}

int
TAO_CEC_ConsumerControl::activate (void)
{
  return 0;
}

int
TAO_CEC_ConsumerControl::shutdown (void)
{
  return 0;
}

void
TAO_CEC_ConsumerControl::consumer_not_exist (TAO_CEC_ProxyPushSupplier *,
                                             CORBA::Environment &)
{
}

void
TAO_CEC_ConsumerControl::consumer_not_exist (TAO_CEC_ProxyPullSupplier *,
                                             CORBA::Environment &)
{
}

void
TAO_CEC_ConsumerControl::system_exception (TAO_CEC_ProxyPushSupplier *,
                                           CORBA::SystemException &,
                                           CORBA::Environment &)
{
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

#elif defined(ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
