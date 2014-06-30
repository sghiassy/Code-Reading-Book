// CEC_Reactive_SupplierControl.i,v 1.2 2000/02/24 02:49:41 coryan Exp

ACE_INLINE
TAO_CEC_Ping_Push_Supplier::
    TAO_CEC_Ping_Push_Supplier (TAO_CEC_SupplierControl *control)
  :  control_ (control)
{
}

// ****************************************************************

ACE_INLINE
TAO_CEC_Ping_Pull_Supplier::
    TAO_CEC_Ping_Pull_Supplier (TAO_CEC_SupplierControl *control)
  :  control_ (control)
{
}
