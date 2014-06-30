/* -*- C++ -*- */
//=============================================================================
/**
 *  @file    Obstack.h
 *
 *  Obstack.h,v 4.14 2001/09/25 19:06:33 nanbor Exp
 *
 *  @author Doug Schmidt
 */
//=============================================================================


#ifndef ACE_OBSTACK_H
#define ACE_OBSTACK_H
#include "ace/pre.h"

#include "ace/Obstack_T.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

typedef ACE_Obstack_T<char> ACE_Obstack;

ACE_SINGLETON_DECLARATION (ACE_Obstack_T <char>;)

#include "ace/post.h"
#endif /* ACE_OBSTACK_H */
