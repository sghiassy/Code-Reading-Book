// EC_Busy_Lock.i,v 1.1 1999/02/24 02:29:19 coryan Exp

template<class T> ACE_INLINE
TAO_EC_Busy_Lock_Adapter<T>::TAO_EC_Busy_Lock_Adapter (T* adaptee)
  :  adaptee_ (adaptee)
{
}

