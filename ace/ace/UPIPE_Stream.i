/* -*- C++ -*- */
// UPIPE_Stream.i,v 4.0.32.1 1996/10/21 21:42:24 levine Exp

// UPIPE_Stream.i

ACE_INLINE ACE_HANDLE 
ACE_UPIPE_Stream::get_handle (void) const
{
  ACE_TRACE ("ACE_UPIPE_Stream::get_handle");
  return this->ACE_SPIPE::get_handle ();
}

