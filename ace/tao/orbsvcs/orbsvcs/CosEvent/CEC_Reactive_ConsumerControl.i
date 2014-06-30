// CEC_Reactive_ConsumerControl.i,v 1.2 2000/02/24 02:49:41 coryan Exp

ACE_INLINE
TAO_CEC_Ping_Push_Consumer::
    TAO_CEC_Ping_Push_Consumer (TAO_CEC_ConsumerControl *control)
  :  control_ (control)
{
}

// ****************************************************************

ACE_INLINE
TAO_CEC_Ping_Pull_Consumer::
    TAO_CEC_Ping_Pull_Consumer (TAO_CEC_ConsumerControl *control)
  :  control_ (control)
{
}
