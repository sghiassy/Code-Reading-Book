/* -*- C++ -*- */
// IDLType_i.cpp,v 1.1 2000/07/06 17:42:56 parsons Exp

#include "IDLType_i.h"

ACE_RCSID(IFR_Service, IDLType_i, "IDLType_i.cpp,v 1.1 2000/07/06 17:42:56 parsons Exp")

TAO_IDLType_i::TAO_IDLType_i (TAO_Repository_i *repo,
                              ACE_Configuration_Section_Key section_key)
  : TAO_IRObject_i (repo, section_key)
{
}

TAO_IDLType_i::~TAO_IDLType_i (void)
{
}

