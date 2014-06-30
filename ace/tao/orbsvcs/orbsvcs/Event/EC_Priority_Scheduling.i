// EC_Priority_Scheduling.i,v 1.1 1999/05/14 14:50:22 coryan Exp

ACE_INLINE
TAO_EC_Priority_Scheduling::TAO_EC_Priority_Scheduling (
       RtecScheduler::Scheduler_ptr scheduler)
  :  scheduler_ (RtecScheduler::Scheduler::_duplicate (scheduler))
{
}
