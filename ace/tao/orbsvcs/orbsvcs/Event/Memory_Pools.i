/* -*- C++ -*- */
// Memory_Pools.i,v 1.4 1998/10/12 04:37:27 coryan Exp

ACE_INLINE void *
ACE_ES_Memory_Pools::new_Dispatch_Request (void)
{
  return Dispatch_Request_::instance ()->malloc (sizeof (ACE_ES_Dispatch_Request));
}

ACE_INLINE void
ACE_ES_Memory_Pools::delete_Dispatch_Request (void *mem)
{
  Dispatch_Request_::instance ()->free (mem);
}
