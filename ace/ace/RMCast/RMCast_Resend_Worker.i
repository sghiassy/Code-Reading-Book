// RMCast_Resend_Worker.i,v 1.1 2000/10/11 00:57:08 coryan Exp

ACE_INLINE

ACE_RMCast_Resend_Worker::
ACE_RMCast_Resend_Worker (ACE_RMCast_Module *next,
                          ACE_UINT32 max_sequence_number)
  : n (0)
  , next_ (next)
  , max_sequence_number_ (max_sequence_number)
{
}
