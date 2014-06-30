// -*- C++ -*-
//
// Interceptor_List.inl,v 1.3 2000/11/15 09:51:07 othman Exp

#if TAO_HAS_INTERCEPTORS == 1
ACE_INLINE TAO_ClientRequestInterceptor_List::TYPE &
TAO_ClientRequestInterceptor_List::interceptors (void)
{
  return this->interceptors_;
}

// ------------------

ACE_INLINE TAO_ServerRequestInterceptor_List::TYPE &
TAO_ServerRequestInterceptor_List::interceptors (void)
{
  return this->interceptors_;
}
#endif  /* TAO_HAS_INTERCEPTORS == 1 */

// ------------------

ACE_INLINE TAO_IORInterceptor_List::TYPE &
TAO_IORInterceptor_List::interceptors (void)
{
  return this->interceptors_;
}
