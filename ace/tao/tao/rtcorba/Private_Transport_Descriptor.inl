// -*- C++ -*-
// Private_Transport_Descriptor.inl,v 1.1 2001/06/12 18:58:21 fhunleth Exp

ACE_INLINE
TAO_Private_Transport_Descriptor::
TAO_Private_Transport_Descriptor (TAO_Endpoint *endpoint,
                                  long object_id,
                                  CORBA::Boolean flag)
  : TAO_Transport_Descriptor_Interface (endpoint, flag),
    object_id_ (object_id)
{
}

