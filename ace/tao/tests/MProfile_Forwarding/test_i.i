// test_i.i,v 1.1 1999/10/12 22:05:54 bala Exp

ACE_INLINE
Simple_Server_i::Simple_Server_i (CORBA::ORB_ptr orb)

  :  orb_ (CORBA::ORB::_duplicate (orb))
{
}
