// test_i.i,v 1.1 2000/09/13 19:31:49 coryan Exp

ACE_INLINE
Simple_Server_i::Simple_Server_i (CORBA::ORB_ptr orb)
  :  orb_ (CORBA::ORB::_duplicate (orb))
{
}
