// RMCast_Ack_Worker.i,v 1.1 2000/10/11 00:57:08 coryan Exp

ACE_INLINE
ACE_RMCast_Ack_Worker::
ACE_RMCast_Ack_Worker (ACE_RMCast::Ack &ack,
                       ACE_RMCast_Retransmission::Messages::Write_Guard &g,
                       ACE_RMCast_Retransmission::Messages *messages)
  : ack_ (ack)
  , ace_mon_ (g)
  , messages_ (messages)
{
}
