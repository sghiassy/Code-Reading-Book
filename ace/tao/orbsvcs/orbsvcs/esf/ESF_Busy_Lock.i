// ESF_Busy_Lock.i,v 1.2 2000/04/02 19:22:41 coryan Exp

template<class T> ACE_INLINE
TAO_ESF_Busy_Lock_Adapter<T>::TAO_ESF_Busy_Lock_Adapter (T* adaptee)
  :  adaptee_ (adaptee)
{
}

