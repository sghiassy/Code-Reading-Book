// Mutex_i.h,v 1.2 1999/02/01 00:19:32 schmidt Exp

#ifndef MUTEX_I_H
#define MUTEX_I_H

#include "Test_T.h"

/* Create a very simple derivative of our Test template.  All we have
   to do is provide our mutex choice and a name.
 */
class Mutex : public Test_T<ACE_Mutex>
{
public:
  Mutex (void) : Test_T<ACE_Mutex> ("Mutex") {}
};

#endif /* MUTEX_I_H */
