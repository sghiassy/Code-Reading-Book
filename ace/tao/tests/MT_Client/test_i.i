// test_i.i,v 1.1 1999/03/20 20:30:57 coryan Exp

ACE_INLINE
Simple_Server_i::Simple_Server_i (CORBA::ORB_ptr orb)
  :  orb_ (CORBA::ORB::_duplicate (orb))
{
}
