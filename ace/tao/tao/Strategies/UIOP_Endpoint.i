// -*- C++ -*-
// UIOP_Endpoint.i,v 1.3 2001/02/14 19:01:40 bala Exp

# if TAO_HAS_UIOP == 1

ACE_INLINE const ACE_UNIX_Addr &
TAO_UIOP_Endpoint::object_addr (void) const
{
  return this->object_addr_;
}


ACE_INLINE const char *
TAO_UIOP_Endpoint::rendezvous_point (void) const
{
  return this->object_addr_.get_path_name ();
}


#endif  /* TAO_HAS_UIOP == 1 */
