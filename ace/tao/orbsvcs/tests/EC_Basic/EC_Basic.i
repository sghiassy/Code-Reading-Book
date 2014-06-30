// EC_Basic.i,v 1.3 1999/01/02 21:14:41 schmidt Exp

RtecScheduler::handle_t
ECB_Consumer::rt_info (void) const
{
  return this->rt_info_;
}

RtecScheduler::handle_t
ECB_Supplier::rt_info (void) const
{
  return this->rt_info_;
}


