//
// Session_Task.h,v 1.4 2001/04/24 08:02:57 coryan Exp
//

#ifndef BIG_ONEWAYS_SESSION_TASK_H
#define BIG_ONEWAYS_SESSION_TASK_H
#include "ace/pre.h"

#include "ace/Task.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

class Session;

/// Implement a Task to run the experiments using multiple threads.
class Session_Task : public ACE_Task_Base
{
public:
  /// Constructor
  Session_Task (Session *session);

  /// Thread entry point
  int svc (void);

private:
  /// Reference to the test interface
  Session *session_;
};

#include "ace/post.h"
#endif /* BIG_ONEWAYS_SESSION_TASK_H */
