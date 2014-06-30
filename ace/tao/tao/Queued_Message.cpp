// -*- C++ -*-
// Queued_Message.cpp,v 1.5 2001/08/01 23:39:53 coryan Exp

#include "Queued_Message.h"

#if !defined (__ACE_INLINE__)
# include "Queued_Message.inl"
#endif /* __ACE_INLINE__ */

ACE_RCSID(tao, Queued_Message, "Queued_Message.cpp,v 1.5 2001/08/01 23:39:53 coryan Exp")

TAO_Queued_Message::TAO_Queued_Message (void)
  : next_ (0)
  , prev_ (0)
{
}

TAO_Queued_Message::~TAO_Queued_Message (void)
{
}

void
TAO_Queued_Message::remove_from_list (TAO_Queued_Message *&head,
                                      TAO_Queued_Message *&tail)
{
  if (this->prev_ != 0)
    this->prev_->next_ = this->next_;
  else
    head = this->next_;

  if (this->next_ != 0)
    this->next_->prev_ = this->prev_;
  else
    tail = this->prev_;

  this->next_ = 0;
  this->prev_ = 0;
}

void
TAO_Queued_Message::push_back (TAO_Queued_Message *&head,
                               TAO_Queued_Message *&tail)
{
  if (tail == 0)
    {
      tail = this;
      head = this;
      this->next_ = 0;
      this->prev_ = 0;
      return;
    }

  tail->next_ = this;
  this->prev_ = tail;
  this->next_ = 0;
  tail = this;
}

void
TAO_Queued_Message::push_front (TAO_Queued_Message *&head,
                                TAO_Queued_Message *&tail)
{
  if (head == 0)
    {
      tail = this;
      head = this;
      this->next_ = 0;
      this->prev_ = 0;
      return;
    }

  head->prev_ = this;
  this->next_ = head;
  this->prev_ = 0;
  head = this;
}
