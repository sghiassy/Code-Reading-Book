/* -*- C++ -*- */
// SV_Message.i,v 4.0.32.1 1996/10/21 21:42:08 levine Exp

// SV_Message.i

ACE_INLINE
ACE_SV_Message::ACE_SV_Message (long t)
  : type_ (t)
{
  ACE_TRACE ("ACE_SV_Message::ACE_SV_Message");
}

ACE_INLINE
ACE_SV_Message::~ACE_SV_Message (void)
{
  ACE_TRACE ("ACE_SV_Message::~ACE_SV_Message");
}

ACE_INLINE long
ACE_SV_Message::type (void) const
{
  ACE_TRACE ("ACE_SV_Message::type");
  return this->type_;
}

ACE_INLINE void
ACE_SV_Message::type (long t)
{
  ACE_TRACE ("ACE_SV_Message::type");
  this->type_ = t;
}
