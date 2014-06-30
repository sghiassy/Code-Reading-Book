// test_i.h,v 1.3 2001/05/20 15:46:57 bala Exp

// ============================================================================
//
// = LIBRARY
//   TAO/tests/Leader_Followers/
//
// = FILENAME
//   test_i.h
//
// = AUTHOR
//   Irfan Pyarali
//
// ============================================================================

#include "testS.h"

class test_i : public POA_test
{
  // = TITLE
  //   Simple test implementation.
  //
public:
  test_i (CORBA::ORB_ptr orb);
  // ctor.

  // = The test interface methods.
  CORBA::ULong method (CORBA::ULong work,
                       CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException));

  void shutdown (CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException));

private:
  CORBA::ORB_var orb_;
  // The ORB.
};
