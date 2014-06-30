// Principal.cpp,v 1.13 1999/07/26 20:25:13 parsons Exp

// Copyright 1994-1995 by Sun Microsystems Inc.
// All Rights Reserved
// ORB:		Principal identifier pseudo-objref

#include "tao/Principal.h"
#include "tao/CDR.h"

#if !defined (__ACE_INLINE__)
#include "tao/Principal.i"
#endif /* __ACE_INLINE__ */

ACE_RCSID(tao, Principal, "Principal.cpp,v 1.13 1999/07/26 20:25:13 parsons Exp")

CORBA_Principal::CORBA_Principal (void)
  : refcount_ (1)
{
}

CORBA_Principal::~CORBA_Principal (void)
{
}

CORBA::Boolean
operator<< (TAO_OutputCDR& cdr, CORBA_Principal* x)
{
  if (x != 0)
    {
      CORBA::ULong length  = x->id.length ();
      cdr.write_long (length);
      cdr.write_octet_array (x->id.get_buffer (), length);
    }
  else
    {
      cdr.write_ulong (0);
    }
  return (CORBA::Boolean) cdr.good_bit ();
}

CORBA::Boolean
operator>> (TAO_InputCDR& cdr, CORBA_Principal*& x)
{
  CORBA::ULong length;
  cdr.read_ulong (length);
  if (length == 0 || !cdr.good_bit ())
    {
      x = 0;
    }
  else
    {
      ACE_NEW_RETURN (x, CORBA::Principal, 0);
      x->id.length (length);
      cdr.read_octet_array (x->id.get_buffer (), length);
    }
  return (CORBA::Boolean) cdr.good_bit ();
}
