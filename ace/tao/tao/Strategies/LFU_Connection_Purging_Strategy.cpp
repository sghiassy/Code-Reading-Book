// LFU_Connection_Purging_Strategy.cpp,v 1.4 2001/05/23 14:06:37 oci Exp

#include "LFU_Connection_Purging_Strategy.h"
#include "tao/Transport.h"

ACE_RCSID(tao, LFU_Connection_Purging_Strategy, "LFU_Connection_Purging_Strategy.cpp,v 1.4 2001/05/23 14:06:37 oci Exp")

TAO_LFU_Connection_Purging_Strategy::TAO_LFU_Connection_Purging_Strategy (
                                                           int cache_maximum)
: TAO_Connection_Purging_Strategy (cache_maximum)
{
}


TAO_LFU_Connection_Purging_Strategy::~TAO_LFU_Connection_Purging_Strategy (void)
{
}


void
TAO_LFU_Connection_Purging_Strategy::update_item (TAO_Transport* transport)
{
  transport->purging_order (transport->purging_order () + 1);
}

