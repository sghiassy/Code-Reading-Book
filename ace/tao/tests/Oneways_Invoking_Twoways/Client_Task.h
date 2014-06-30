//
// Client_Task.h,v 1.1 2001/06/24 13:54:49 bala Exp
//

#ifndef ONEWAYS_INVOKING_TWOWAYS_CLIENT_TASK_H
#define ONEWAYS_INVOKING_TWOWAYS_CLIENT_TASK_H
#include "ace/pre.h"

#include "TestC.h"
#include "ace/Task.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

/// Implement a Task to run the experiments using multiple threads.
class Client_Task : public ACE_Task_Base
{
public:
  /// Constructor
  Client_Task (Test::Sender_ptr sender,
               Test::Receiver_ptr us,
               ACE_Thread_Manager *thr_mgr);

  /// Thread entry point
  int svc (void);

private:

  /// Reference to the test interface
  Test::Sender_var sender_;

  /// Reference to the test interface
  Test::Receiver_var us_;
};

#include "ace/post.h"
#endif /* ONEWAYS_INVOKING_TWOWAYS_CLIENT_TASK_H */
