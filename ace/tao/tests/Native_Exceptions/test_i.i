// test_i.i,v 1.1 1999/06/18 16:58:08 coryan Exp

ACE_INLINE
Simple_Server_i::Simple_Server_i (CORBA::ORB_ptr orb)
  :  orb_ (CORBA::ORB::_duplicate (orb))
{
}
