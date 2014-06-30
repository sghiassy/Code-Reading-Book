// ESF_Peer_Workers.cpp,v 1.2 2000/04/02 19:22:41 coryan Exp

#ifndef TAO_ESF_PEER_WORKERS_CPP
#define TAO_ESF_PEER_WORKERS_CPP

#include "ESF_Peer_Workers.h"

#if ! defined (__ACE_INLINE__)
#include "ESF_Peer_Workers.i"
#endif /* __ACE_INLINE__ */

ACE_RCSID(ESF, ESF_Peer_Workers, "ESF_Peer_Workers.cpp,v 1.2 2000/04/02 19:22:41 coryan Exp")

template<class P, class R> void
TAO_ESF_Peer_Connected<P,R>::work (P *proxy,
                                   CORBA::Environment &ACE_TRY_ENV)
{
  proxy->connected (this->peer_, ACE_TRY_ENV);
  ACE_CHECK; // Shouldn't happen, just following the discipline

  this->peer_->connected (proxy, ACE_TRY_ENV);
  ACE_CHECK; // Shouldn't happen, just following the discipline
}

// ****************************************************************

template<class P, class R> void
TAO_ESF_Peer_Reconnected<P,R>::work (P *proxy,
                                     CORBA::Environment &ACE_TRY_ENV)
{
  proxy->reconnected (this->peer_, ACE_TRY_ENV);
  ACE_CHECK; // Shouldn't happen, just following the discipline

  this->peer_->reconnected (proxy, ACE_TRY_ENV);
  ACE_CHECK; // Shouldn't happen, just following the discipline
}

// ****************************************************************

template<class P, class R> void
TAO_ESF_Peer_Disconnected<P,R>::work (P *proxy,
                                      CORBA::Environment &ACE_TRY_ENV)
{
  proxy->disconnected (this->peer_, ACE_TRY_ENV);
  ACE_CHECK; // Shouldn't happen, just following the discipline

  this->peer_->disconnected (proxy, ACE_TRY_ENV);
  ACE_CHECK; // Shouldn't happen, just following the discipline
}

#endif /* TAO_ESF_PEER_WORKERS_CPP */
