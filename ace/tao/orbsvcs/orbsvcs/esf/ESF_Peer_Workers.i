// ESF_Peer_Workers.i,v 1.2 2000/04/02 19:22:41 coryan Exp

template<class P, class R>
TAO_ESF_Peer_Connected<P,R>::TAO_ESF_Peer_Connected (R *peer)
  :  peer_ (peer)
{
}

// ****************************************************************

template<class P, class R>
TAO_ESF_Peer_Reconnected<P,R>::TAO_ESF_Peer_Reconnected (R *peer)
  :  peer_ (peer)
{
}

// ****************************************************************

template<class P, class R>
TAO_ESF_Peer_Disconnected<P,R>::TAO_ESF_Peer_Disconnected (R *peer)
  :  peer_ (peer)
{
}

// ****************************************************************
