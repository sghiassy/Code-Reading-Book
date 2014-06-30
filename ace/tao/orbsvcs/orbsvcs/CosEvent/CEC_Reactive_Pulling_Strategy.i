// CEC_Reactive_Pulling_Strategy.i,v 1.1 2000/02/17 23:11:34 coryan Exp

ACE_INLINE
TAO_CEC_Pull_Event::TAO_CEC_Pull_Event (TAO_CEC_ConsumerAdmin *consumer_admin,
                                        TAO_CEC_SupplierControl *control)
  :  consumer_admin_ (consumer_admin),
     supplier_control_ (control)
{
}
