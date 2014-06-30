// Endpoint_Selector_Factory.cpp,v 1.3 2001/06/12 18:57:58 fhunleth Exp

#include "tao/Endpoint_Selector_Factory.h"
#include "ace/Dynamic_Service.h"

ACE_RCSID(tao, Endpoint_Selector_Factory, "Endpoint_Selector_Factory.cpp,v 1.3 2001/06/12 18:57:58 fhunleth Exp")

TAO_Endpoint_Selector_Factory::~TAO_Endpoint_Selector_Factory (void)
{
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class ACE_Dynamic_Service<TAO_Endpoint_Selector_Factory>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate ACE_Dynamic_Service<TAO_Endpoint_Selector_Factory>

#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
