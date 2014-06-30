// w32_test_impl.cpp,v 1.3 1999/11/15 21:56:50 schmidt Exp

#include "w32_test_impl.h"

W32_Test_Impl::~W32_Test_Impl (void)
{
}

W32_Test_Impl::W32_Test_Impl (void)
{
  blabla = 1;
}


char *
W32_Test_Impl::getresponse (CORBA::Long respnr)
{
  char *myreturn;

  switch (respnr)
    {
    case 1:
      myreturn = CORBA::string_dup ("Parameter 1");
      break;
    case 2: 
      myreturn = CORBA::string_dup ("Parameter 2");
      break;
    default: 
      myreturn = CORBA::string_dup ("Parameter unknown!");
      break;
    }

  return myreturn;
}
