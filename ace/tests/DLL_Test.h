// ================================================================
/**
 * @file DLL_Test.h
 *
 * DLL_Test.h,v 1.6 2001/02/24 18:04:29 coryan Exp
 *
 * @author Kirthika Parameswaran  <kirthika@cs.wustl.edu>
 */
// ================================================================

#ifndef ACE_TESTS_DLL_TEST_H
#define ACE_TESTS_DLL_TEST_H

#include "ace/Log_Msg.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

/**
 * @class Hello
 *
 * @brief Define the interface used by the DLL_Test
 */
class Hello
{
public:
  /// Destructor
  virtual ~Hello (void) {}

  /**
   * @name Methods invoked by the test
   *
   * The test invokes two methods, a non-virtual method and a virtual
   * method implemented in the shared library.
   */
  //@{
  void say_hello (void)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("Hello\n")));
  }

  virtual void say_next (void) = 0;
  //@}
};

#endif /* ACE_TESTS_DLL_TEST_H */
