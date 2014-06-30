// -*- C++ -*-
// LF_Follower_Auto_Adder.cpp,v 1.2 2001/08/01 23:39:46 coryan Exp

#include "tao/LF_Follower_Auto_Adder.h"

#if !defined (__ACE_INLINE__)
# include "tao/LF_Follower_Auto_Adder.inl"
#endif /* __ACE_INLINE__ */

ACE_RCSID(tao, LF_Follower_Auto_Adder, "LF_Follower_Auto_Adder.cpp,v 1.2 2001/08/01 23:39:46 coryan Exp")

TAO_LF_Follower_Auto_Adder::~TAO_LF_Follower_Auto_Adder (void)
{
  this->leader_follower_.remove_follower (this->follower_);
}
