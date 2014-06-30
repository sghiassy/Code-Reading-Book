//Pluggable_Messaging_Utils.cpp,v 1.11 2001/07/06 04:39:18 bala Exp

#include "tao/Pluggable_Messaging_Utils.h"
#include "tao/ORB_Core.h"

#if !defined (__ACE_INLINE__)
#include "tao/Pluggable_Messaging_Utils.i"
#endif /* __ACE_INLINE__ */

ACE_RCSID(tao, Pluggable_Messaging_Utils, "Pluggable_Messaging_Utils.cpp,v 1.11 2001/07/06 04:39:18 bala Exp")


TAO_Pluggable_Reply_Params::TAO_Pluggable_Reply_Params (
    TAO_ORB_Core *orb_core
  )
  : input_cdr_ (orb_core->create_input_cdr_data_block (ACE_CDR::DEFAULT_BUFSIZE),
                0,
                TAO_ENCAP_BYTE_ORDER,
                TAO_DEF_GIOP_MAJOR,
                TAO_DEF_GIOP_MINOR,
                orb_core)
{
}
