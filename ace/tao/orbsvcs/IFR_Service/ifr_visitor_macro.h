/* -*- C++ -*- */
// ifr_visitor_macro.h,v 1.6 2001/05/25 14:15:54 parsons Exp

#ifndef TAO_IFR_VISITOR_MACRO_H
#define TAO_IFR_VISITOR_MACRO_H
#include "ace/pre.h"

#define TAO_IFR_VISITOR_WRITE_GUARD \
  ACE_WRITE_GUARD_THROW_EX (ACE_Lock, \
                            monitor, \
                            visitor.lock (), \
                            CORBA::INTERNAL ( \
                              CORBA_SystemException::_tao_minor_code ( \
                                TAO_GUARD_FAILURE, \
                                0), \
                              CORBA::COMPLETED_NO)); \
  ACE_TRY_CHECK

#include "ace/post.h"
#endif /* TAO_IFR_VISITOR_MACRO_H */
