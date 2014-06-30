// Method_Request.cpp
// Method_Request.cpp,v 4.4 2000/11/25 19:37:48 schmidt Exp

#include "ace/Method_Request.h"

ACE_RCSID(ace, Method_Request, "Method_Request.cpp,v 4.4 2000/11/25 19:37:48 schmidt Exp")

ACE_Method_Request::ACE_Method_Request (u_long prio)
  : priority_ (prio)
{
}

ACE_Method_Request::~ACE_Method_Request (void)
{
}

u_long
ACE_Method_Request::priority (void) const
{
  return this->priority_;
}

void
ACE_Method_Request::priority (u_long prio)
{
  this->priority_ = prio;
}
