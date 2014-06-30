// ESF_Proxy_List.i,v 1.2 2000/04/02 19:22:41 coryan Exp

template<class PROXY> ACE_INLINE ACE_Unbounded_Set_Iterator<PROXY*>
TAO_ESF_Proxy_List<PROXY>::begin (void)
{
  return this->impl_.begin ();
}

template<class PROXY> ACE_INLINE ACE_Unbounded_Set_Iterator<PROXY*>
TAO_ESF_Proxy_List<PROXY>::end (void)
{
  return this->impl_.end ();
}

template<class PROXY> ACE_INLINE size_t
TAO_ESF_Proxy_List<PROXY>::size (void) const
{
  return this->impl_.size ();
}
