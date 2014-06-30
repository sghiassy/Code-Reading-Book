// LF_Event_Binder.inl,v 1.2 2001/08/01 23:39:46 coryan Exp

ACE_INLINE
TAO_LF_Event_Binder::TAO_LF_Event_Binder (TAO_LF_Event *event,
                                          TAO_LF_Follower *follower)
  : event_ (event)
{
  this->event_->bind (follower);
}


ACE_INLINE
TAO_LF_Event_Binder::~TAO_LF_Event_Binder (void)
{
  this->event_->unbind ();
}
