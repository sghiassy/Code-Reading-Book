/* -*- C++ -*- */
// Process_Manager.i,v 4.5 2000/03/29 20:55:39 schmidt Exp

ACE_INLINE size_t
ACE_Process_Manager::managed (void) const
{
  return current_count_;
}
