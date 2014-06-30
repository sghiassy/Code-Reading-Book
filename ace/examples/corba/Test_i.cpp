// Test_i.cpp,v 4.2 1998/07/31 22:55:09 gonzo Exp

#include "Test_i.h"

ACE_RCSID(CORBA, Test_i, "Test_i.cpp,v 4.2 1998/07/31 22:55:09 gonzo Exp")

void
Test_i::method (long input,
		ACE_CORBA_1 (Environment) &)
{
  ACE_DEBUG ((LM_DEBUG, "received a number %d\n", input));
}
