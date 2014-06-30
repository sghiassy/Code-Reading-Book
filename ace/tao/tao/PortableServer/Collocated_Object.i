// -*- C++ -*-
//
// Collocated_Object.i,v 1.4 2000/09/20 00:16:08 othman Exp


ACE_INLINE
TAO_Collocated_Object::TAO_Collocated_Object (TAO_Stub *p,
                                              CORBA::Boolean collocated,
                                              TAO_ServantBase *servant)
  : CORBA_Object (p, collocated, servant)
{
}
