/* -*- C++ -*- */
// SPIPE_Connector.i,v 4.3 1998/04/30 23:25:52 nanbor Exp

// SPIPE_Connector.i

ASYS_INLINE int
ACE_SPIPE_Connector::reset_new_handle (ACE_HANDLE handle)
{
  ACE_UNUSED_ARG (handle);
  // Nothing to do here since the handle is not a socket
  return 0;
}

