//
// Session_Task.cpp,v 1.4 2001/04/24 08:02:57 coryan Exp
//

#include "Session_Task.h"
#include "Session.h"

ACE_RCSID(Big_Oneways, Session_Task, "Session_Task.cpp,v 1.4 2001/04/24 08:02:57 coryan Exp")

Session_Task::Session_Task (Session *session)
  : session_ (session)
{
}

int
Session_Task::svc (void)
{
  return this->session_->svc ();
}
