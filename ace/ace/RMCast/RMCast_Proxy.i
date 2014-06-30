// RMCast_Proxy.i,v 1.2 2000/10/02 18:40:02 coryan Exp

ACE_INLINE
ACE_RMCast_Proxy::ACE_RMCast_Proxy (void)
  : next_expected_ (0)
  , highest_received_ (0)
{
}
