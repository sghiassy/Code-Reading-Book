// test_i.i,v 1.1 1999/06/16 17:41:18 coryan Exp

ACE_INLINE
Database_i::Database_i (CORBA::ORB_ptr orb,
                        Test::Index max_range)
  :  orb_ (CORBA::ORB::_duplicate (orb)),
     max_range_ (max_range)
{
  ACE_NEW (this->elements_, Test::OctetSeq[this->max_range_]);
}
