// EC_Timeout_Filter.i,v 1.2 1999/03/10 22:44:43 coryan Exp

ACE_INLINE const TAO_EC_QOS_Info&
TAO_EC_Timeout_Filter::qos_info (void) const
{
  return this->qos_info_;
}

ACE_INLINE RtecEventComm::EventType
TAO_EC_Timeout_Filter::type (void) const
{
  return this->type_;
}
