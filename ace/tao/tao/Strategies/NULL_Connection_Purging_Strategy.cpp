// NULL_Connection_Purging_Strategy.cpp,v 1.5 2001/05/23 14:06:37 oci Exp

#include "NULL_Connection_Purging_Strategy.h"
#include "tao/Transport.h"

ACE_RCSID(tao, NULL_Connection_Purging_Strategy, "NULL_Connection_Purging_Strategy.cpp,v 1.5 2001/05/23 14:06:37 oci Exp")

TAO_NULL_Connection_Purging_Strategy::TAO_NULL_Connection_Purging_Strategy (
                                                           int cache_maximum)
 : TAO_Connection_Purging_Strategy (cache_maximum)
{
}


TAO_NULL_Connection_Purging_Strategy::~TAO_NULL_Connection_Purging_Strategy (void)
{
}


void
TAO_NULL_Connection_Purging_Strategy::update_item (TAO_Transport* transport)
{
  ACE_UNUSED_ARG (transport);
}


int
TAO_NULL_Connection_Purging_Strategy::cache_maximum (void) const
{
  return -1;
}
