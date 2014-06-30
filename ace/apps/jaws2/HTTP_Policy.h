/* -*- c++ -*- */
// HTTP_Policy.h,v 1.1 2000/04/27 23:43:16 jxh Exp

#ifndef HTTP_POLICY_H
#define HTTP_POLICY_H

#include "JAWS/Concurrency.h"
#include "JAWS/Policy.h"

/* create a policy */
class HTTP_Policy : public JAWS_Dispatch_Policy
{
public:
  HTTP_Policy (JAWS_Concurrency_Base *concurrency);
  virtual JAWS_Concurrency_Base * update (void *state = 0);

private:
  JAWS_Concurrency_Base *concurrency_;
};


#endif /* !defined (HTTP_POLICY_H) */
