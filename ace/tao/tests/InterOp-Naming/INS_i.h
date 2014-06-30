// -*- C++ -*-
// INS_i.h,v 1.2 1999/06/16 18:23:19 vishal Exp

// ============================================================================
//
// = LIBRARY
//    TAO/tests/InterOp-Naming
//
// = FILENAME
//    INS_i.h
//
// = DESCRIPTION
//    This class implements the INS interface.
//
// = AUTHOR
//    Vishal Kachroo <vishal@cs.wustl.edu>
//
// ============================================================================

#ifndef INS_I_H
#define INS_I_H

#include "INSS.h"

class INS_i : public POA_INS
{

public:
  // = Initialization and termination methods.
  INS_i (void);
  // Constructor.

  ~INS_i (void);
  // Destructor.

  char * test_ins (CORBA::Environment &env)
  ACE_THROW_SPEC (( CORBA::SystemException ));  
  // test the INS.

  void orb (CORBA::ORB_ptr o);
  // Set the ORB pointer.

private:
  CORBA::ORB_var orb_;
  // ORB pointer.

};

#endif /* INS_I_H */


