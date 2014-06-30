// -*- C++ -*-
//
// ClientInterceptorAdapter.inl,v 1.2 2001/07/06 04:39:03 bala Exp

ACE_INLINE
TAO_ClientRequestInterceptor_Adapter::TAO_ClientRequestInterceptor_Adapter
  (TAO_ClientRequestInterceptor_List::TYPE &interceptors,
   TAO_GIOP_Invocation *invocation,
   int &invoke_status)
    : interceptors_ (interceptors),
      invocation_ (invocation),
      invoke_status_ (invoke_status),
      len_ (0),
      stack_size_ (0)
{
  this->len_ = interceptors.size ();
}
