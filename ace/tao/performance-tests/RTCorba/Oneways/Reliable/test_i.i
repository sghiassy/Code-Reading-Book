// test_i.i,v 1.1 1999/11/29 20:11:30 parsons Exp

ACE_INLINE
Test_i::Test_i (CORBA::ORB_ptr orb)
  :  orb_ (CORBA::ORB::_duplicate (orb))
{
}
