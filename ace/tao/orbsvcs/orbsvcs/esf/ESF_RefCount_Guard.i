// ESF_RefCount_Guard.i,v 1.2 2000/04/18 16:22:25 coryan Exp

template<class T> ACE_INLINE
TAO_ESF_RefCount_Guard<T>::
    TAO_ESF_RefCount_Guard (T &refcount)
  :  refcount_ (refcount)
{
  this->refcount_++;
}

template<class T> ACE_INLINE
TAO_ESF_RefCount_Guard<T>::
    ~TAO_ESF_RefCount_Guard (void)
{
  this->refcount_--;
}
