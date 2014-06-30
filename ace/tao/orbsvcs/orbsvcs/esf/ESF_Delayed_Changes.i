// ESF_Delayed_Changes.i,v 1.2 2000/04/02 19:22:41 coryan Exp

template<class PROXY, class C, class I,ACE_SYNCH_DECL> void
TAO_ESF_Delayed_Changes<PROXY,C,I,ACE_SYNCH_USE>::
    connected_i (PROXY *proxy,
               CORBA::Environment &ACE_TRY_ENV)
{
  this->collection_.connected (proxy, ACE_TRY_ENV);
}

template<class PROXY, class C, class I,ACE_SYNCH_DECL> void
TAO_ESF_Delayed_Changes<PROXY,C,I,ACE_SYNCH_USE>::
    reconnected_i (PROXY *proxy,
                 CORBA::Environment &ACE_TRY_ENV)
{
  this->collection_.reconnected (proxy, ACE_TRY_ENV);
}

template<class PROXY, class C, class I,ACE_SYNCH_DECL> void
TAO_ESF_Delayed_Changes<PROXY,C,I,ACE_SYNCH_USE>::
    disconnected_i (PROXY *proxy,
                  CORBA::Environment &ACE_TRY_ENV)
{
  this->collection_.disconnected (proxy, ACE_TRY_ENV);
}

template<class PROXY, class C, class I,ACE_SYNCH_DECL> void
TAO_ESF_Delayed_Changes<PROXY,C,I,ACE_SYNCH_USE>::
    shutdown_i (CORBA::Environment &ACE_TRY_ENV)
{
  this->collection_.shutdown (ACE_TRY_ENV);
}
