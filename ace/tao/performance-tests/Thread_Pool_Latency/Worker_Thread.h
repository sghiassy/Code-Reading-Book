//
// Worker_Thread.h,v 1.1 2001/05/09 22:38:26 coryan Exp
//

#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H
#include "ace/pre.h"

#include "tao/corba.h"
#include "ace/Task.h"

/// Implement the Test::Worker_Thread interface
class Worker_Thread : public ACE_Task_Base
{
public:
  /// Constructor
  Worker_Thread (CORBA::ORB_ptr orb);

  // = The service method
  virtual int svc (void);

private:
  CORBA::ORB_var orb_;
};

#include "ace/post.h"
#endif /* WORKER_THREAD_H */
