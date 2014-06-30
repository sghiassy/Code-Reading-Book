/**
 * @file Client_Task.h
 *
 * Client_Task.h,v 1.1 2001/08/24 21:46:48 coryan Exp
 *
 */

#ifndef ECL_CLIENT_TASK_H
#define ECL_CLIENT_TASK_H

#include "tao/ORB.h"
#include "ace/Task.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

/**
 * @class ECL_Client_Task
 *
 * @brief Implement a simple ACE_Task to receive the events
 *
 */
class ECL_Client_Task : public ACE_Task_Base
{
public:
  /// Constructor
  ECL_Client_Task (CORBA::ORB_ptr orb);

  /// Run the experiment
  int svc (void);

private:
  /// The consumer
  CORBA::ORB_var orb_;
};


#endif /* ECL_CLIENT_TASK_H */
