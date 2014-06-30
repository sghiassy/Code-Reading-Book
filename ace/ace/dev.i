/* -*- C++ -*- */
// DEV.i,v 4.2 1998/04/30 23:25:45 nanbor Exp

// DEV.i

ASYS_INLINE int
ACE_DEV::disable (int signum) const
{
#if defined (ACE_WIN32)
  ACE_UNUSED_ARG (signum) ;
  return 0 ;
#else  /* ACE_WIN32 */
  return ACE_IO_SAP::disable (signum) ;
#endif /* ACE_WIN32 */
}
