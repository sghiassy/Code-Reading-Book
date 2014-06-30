// Interpreter_Utils_T.h,v 1.3 2000/03/23 20:52:34 nanbor Exp

#ifndef TAO_INTERPRETER_UTILS_T_H
#define TAO_INTERPRETER_UTILS_T_H
#include "ace/pre.h"

#include "tao/corba.h"

template <class OPERAND_TYPE>
CORBA::Boolean TAO_find (const CORBA::Any& sequence, const OPERAND_TYPE& operand);

#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
#include "Interpreter_Utils_T.cpp"
#endif  /* ACE_TEMPLATES_REQUIRE_SOURCE */

#include "ace/post.h"
#endif /* TAO_INTERPRETER_UTILS_T_H */
