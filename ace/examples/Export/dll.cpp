// dll.cpp,v 1.1 2000/05/09 16:17:31 brunsch Exp
#include "dll.h"

int
test_function ()
{
  test_variable = RETVAL;
  return RETVAL;
}

int 
test_class::method ()
{
  return RETVAL;
}

test_class *
get_dll_singleton ()
{
  return TEST_SINGLETON::instance ();
}
