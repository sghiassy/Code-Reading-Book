// -*- C++ -*-
//
// Interceptor_Type.h,v 1.1 2000/11/07 20:02:31 coryan Exp
//

#ifndef TAO_INTERCEPTOR_TYPE_H
#define TAO_INTERCEPTOR_TYPE_H
#include "ace/pre.h"

#include "ace/config-all.h"

/// The different type of interceptors have different functionality
/// to perform.
enum Interceptor_Type
{
  /// Do not register an interceptor
  IT_NONE,

  /// Does nothing on all interception points
  IT_NOOP,

  /// Perform service context manipulation
  IT_CONTEXT,

  /// Invoke dynamic interface methods and does extraction from anys.
  IT_DYNAMIC
};

void get_interceptor_type (int argc, char *argv[],
                           int &interceptor_type);

#include "ace/post.h"
#endif /* TAO_INTERCEPTOR_TYPE_H */
