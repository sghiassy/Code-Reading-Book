// RMCast_UDP_Proxy.i,v 1.2 2000/09/24 00:03:24 coryan Exp

ACE_INLINE const ACE_INET_Addr&
ACE_RMCast_UDP_Proxy::peer_addr (void) const
{
  return this->peer_addr_;
}
