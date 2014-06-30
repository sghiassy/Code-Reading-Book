// Supplier.cpp,v 1.1 1999/07/28 18:51:44 coryan Exp

#include "Supplier.h"

ACE_RCSID(EC_Examples, Supplier, "Supplier.cpp,v 1.1 1999/07/28 18:51:44 coryan Exp")

Supplier::Supplier (void)
{
}

void
Supplier::disconnect_push_supplier (CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
}

// ****************************************************************

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
#elif defined(ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
