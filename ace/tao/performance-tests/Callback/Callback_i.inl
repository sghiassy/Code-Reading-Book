// Callback_i.inl,v 1.1 2000/10/27 23:13:34 coryan Exp

ACE_INLINE
Callback_i::Callback_i (int expected_samples)
  : history_ (expected_samples)
  , remaining_samples_ (expected_samples)
{
}

ACE_INLINE ACE_Sample_History&
Callback_i::sample_history (void)
{
  return this->history_;
}
