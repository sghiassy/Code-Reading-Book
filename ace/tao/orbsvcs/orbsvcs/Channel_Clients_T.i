//
// Channel_Clients_T.i,v 1.2 1998/02/17 16:59:12 coryan Exp
//

template<class TARGET> ACE_INLINE
ACE_PushConsumer_Adapter<TARGET>::ACE_PushConsumer_Adapter (TARGET *t)
  :  target_ (t)
{
}

template<class TARGET> ACE_INLINE
ACE_PushSupplier_Adapter<TARGET>::ACE_PushSupplier_Adapter (TARGET *t)
  :  target_ (t)
{
}


