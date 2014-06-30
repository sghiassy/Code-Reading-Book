//
// test_i.cpp,v 1.1 2001/05/17 14:23:58 oci Exp
//
#include "test_i.h"

void
test_i::send_stuff (const char* string,
                  CORBA::Environment &)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  ACE_DEBUG ((LM_DEBUG, "TAO (%P|%t) - %s\n", string));
}

