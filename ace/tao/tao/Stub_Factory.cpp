// Stub_Factory.cpp,v 1.4 2001/06/22 14:26:15 fhunleth Exp
#include "Stub_Factory.h"
#include "ace/Dynamic_Service.h"

ACE_RCSID(tao, TAO_Stub_Factory, "Stub_Factory.cpp,v 1.4 2001/06/22 14:26:15 fhunleth Exp")

TAO_Stub_Factory::~TAO_Stub_Factory (void)
{
  // No-Op.
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class ACE_Dynamic_Service<TAO_Stub_Factory>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate ACE_Dynamic_Service<TAO_Stub_Factory>

#endif
