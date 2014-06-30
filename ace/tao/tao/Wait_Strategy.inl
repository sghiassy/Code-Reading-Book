//Wait_Strategy.inl,v 1.2 2001/04/11 03:36:35 bala Exp

ACE_INLINE int
TAO_Wait_Strategy::is_registered (void)
{
  return this->is_registered_;
}

ACE_INLINE void
TAO_Wait_Strategy::is_registered (int flag)
{
  this->is_registered_ = flag;
}

