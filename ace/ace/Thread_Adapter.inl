// Thread_Adapter.inl,v 4.3 2000/11/13 01:04:57 brunsch Exp

ACE_INLINE ACE_Thread_Manager *
ACE_Thread_Adapter::thr_mgr (void)
{
  return this->thr_mgr_;
}
