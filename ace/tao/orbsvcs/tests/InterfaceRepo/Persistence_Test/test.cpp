// -*- C++ -*-
// test.cpp,v 1.1 2000/07/22 21:34:42 parsons Exp

#include "Ptest.h"

ACE_RCSID(Persistence_Test, test, "test.cpp,v 1.1 2000/07/22 21:34:42 parsons Exp")

int main (int argc, char *argv[])
{
  Ptest ptest;

  int retval = ptest.init (argc, 
                           argv);

  if (retval == -1)
    return 1;

  retval = ptest.run ();

  return retval;
}
