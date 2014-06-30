// IOR_Parser.cpp,v 1.2 2000/09/14 23:40:48 doccvs Exp

#include "tao/IOR_Parser.h"
#include "ace/Dynamic_Service.h"

#if !defined(__ACE_INLINE__)
#include "tao/IOR_Parser.i"
#endif /* __ACE_INLINE__ */

ACE_RCSID(tao, IOR_Parser, "IOR_Parser.cpp,v 1.2 2000/09/14 23:40:48 doccvs Exp")

TAO_IOR_Parser::~TAO_IOR_Parser (void)
{
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class ACE_Dynamic_Service<TAO_IOR_Parser>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate ACE_Dynamic_Service<TAO_IOR_Parser>

#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
