// simple_test_i.i,v 1.1 2000/12/07 20:07:21 doccvs Exp

ACE_INLINE
Simple_Server_i::Simple_Server_i (CORBA::ORB_ptr orb)
  :  orb_ (CORBA::ORB::_duplicate (orb))
{
}

