// LF_Follower.inl,v 1.2 2001/08/01 23:39:46 coryan Exp

ACE_INLINE TAO_Leader_Follower &
TAO_LF_Follower::leader_follower (void)
{
  return this->leader_follower_;
}

ACE_INLINE int
TAO_LF_Follower::wait (ACE_Time_Value *tv)
{
  return this->condition_.wait (tv);
}
