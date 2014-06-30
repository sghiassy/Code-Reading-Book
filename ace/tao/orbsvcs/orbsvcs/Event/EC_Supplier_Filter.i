// EC_Supplier_Filter.i,v 1.2 2000/01/31 04:25:58 coryan Exp

ACE_INLINE
TAO_EC_Filter_Worker::TAO_EC_Filter_Worker (RtecEventComm::EventSet &event,
                                            const TAO_EC_QOS_Info &event_info)
  :  event_ (event),
     event_info_ (event_info)
{
}
