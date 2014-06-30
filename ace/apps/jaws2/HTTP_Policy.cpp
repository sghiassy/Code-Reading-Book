// HTTP_Policy.cpp,v 1.1 2000/04/27 23:43:16 jxh Exp

#include "HTTP_Policy.h"

ACE_RCSID(PROTOTYPE, HTTP_Policy, "HTTP_Policy.cpp,v 1.1 2000/04/27 23:43:16 jxh Exp")

HTTP_Policy::HTTP_Policy (JAWS_Concurrency_Base *concurrency)
  : concurrency_ (concurrency)
{
}

JAWS_Concurrency_Base *
HTTP_Policy::update (void *)
{
  /* for now, we always return the same concurrency strategy */
  return this->concurrency_;
}
