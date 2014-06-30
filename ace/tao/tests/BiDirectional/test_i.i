// test_i.i,v 1.1 2000/12/16 15:46:52 bala Exp

ACE_INLINE
Callback_i::Callback_i (CORBA::ORB_ptr orb)
  :  orb_ (CORBA::ORB::_duplicate (orb))
{
}

ACE_INLINE
Simple_Server_i::Simple_Server_i (CORBA::ORB_ptr orb,
                                  int no_iterations)
  :  orb_ (CORBA::ORB::_duplicate (orb)),
     flag_ (0),
     callback_ (0),
     no_iterations_ (no_iterations)
{
}
