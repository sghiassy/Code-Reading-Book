// MCast.i,v 5.1 1999/08/31 23:17:50 naga Exp
//------------------------------------------------------------
// TAO_AV_UDP_MCast_Flow_Handler
//-----------------------------------------------------------
ACE_INLINE ACE_SOCK_Dgram_Mcast *
TAO_AV_UDP_MCast_Flow_Handler::get_mcast_socket (void) const
{
  return this->dgram_mcast_;
}




