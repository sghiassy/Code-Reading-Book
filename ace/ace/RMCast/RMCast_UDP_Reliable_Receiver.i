// RMCast_UDP_Reliable_Receiver.i,v 1.2 2000/10/25 16:54:33 coryan Exp

ACE_INLINE int
ACE_RMCast_UDP_Reliable_Receiver::init (const ACE_INET_Addr &mcast_group)
{
  return this->io_udp_.subscribe (mcast_group);
}
