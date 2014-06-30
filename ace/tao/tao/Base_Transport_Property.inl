/* -*- C++ -*- */
// Base_Transport_Property.inl,v 1.1 2001/03/17 00:53:27 oci Exp

ACE_INLINE
TAO_Base_Transport_Property::TAO_Base_Transport_Property (void)
{

}

ACE_INLINE
TAO_Base_Transport_Property::
    TAO_Base_Transport_Property (TAO_Endpoint *endpoint,
                                 CORBA::Boolean flag)
  : TAO_Transport_Descriptor_Interface (endpoint,
                                         flag)
{
}

ACE_INLINE
TAO_Base_Transport_Property::TAO_Base_Transport_Property (
    const TAO_Base_Transport_Property &rhs)
  : TAO_Transport_Descriptor_Interface (rhs.endpoint_->duplicate (),
                                        1)
{

}
