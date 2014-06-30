// ping_i.i,v 1.2 1999/11/12 02:40:17 coryan Exp

ACE_INLINE
PingObject_i::PingObject_i (CORBA::ORB_ptr orb,
                            PortableServer::POA_ptr poa)
  :  orb_ (CORBA::ORB::_duplicate (orb)),
     poa_ (PortableServer::POA::_duplicate (poa))
{
}
