/* -*- C++ -*- */


//=============================================================================
/**
 *  @file    UPIPE_Addr.h
 *
 *  UPIPE_Addr.h,v 4.7 2000/11/01 22:17:19 coryan Exp
 *
 *  @author Doug Schmidt
 */
//=============================================================================


#ifndef ACE_UPIPE_ADDR_H
#define ACE_UPIPE_ADDR_H
#include "ace/pre.h"

#include "ace/SPIPE_Addr.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

typedef ACE_SPIPE_Addr ACE_UPIPE_Addr;

#if 0
// We need this "class" to make the class2man documentation utility
// happy.
/**
 * @class ACE_UPIPE_Addr
 *
 * @brief Defines the ACE "user pipe" address family address format.
 *
 * This class has an identical interface to the <ACE_SPIPE_Addr>
 * class.  In fact, it's simply a typedef!
 */
class ACE_Export ACE_UPIPE_Addr
{
public:
  // = Same interface as <ACE_SPIPE_Addr>.
};
#endif /* 0 */
#include "ace/post.h"
#endif /* ACE_UPIPE_ADDR_H */
