// Connection_Purging_Strategy.cpp,v 1.3 2001/05/18 19:27:04 oci Exp

#include "tao/Connection_Purging_Strategy.h"

#if !defined (__ACE_INLINE__)
# include "tao/Connection_Purging_Strategy.inl"
#endif /* __ACE_INLINE__ */

ACE_RCSID(tao, Connection_Purging_Strategy, "Connection_Purging_Strategy.cpp,v 1.3 2001/05/18 19:27:04 oci Exp")

TAO_Connection_Purging_Strategy::~TAO_Connection_Purging_Strategy (void)
{
}


int
TAO_Connection_Purging_Strategy::cache_maximum (void) const
{
  return cache_maximum_;
}


