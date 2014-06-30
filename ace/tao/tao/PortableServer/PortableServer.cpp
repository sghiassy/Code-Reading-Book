/* -*- C++ -*- */

// PortableServer.cpp,v 1.3 2001/09/25 05:12:33 irfan Exp
// =================================================================
//
// = LIBRARY
//    TAO
//
// = FILENAME
//    PortableServer.cpp
//
// = AUTHOR
//    Carlos O'Ryan <coryan@uci.edu>
//
// =================================================================

#include "PortableServer.h"
#include "Object_Adapter.h"

ACE_RCSID (PortableServer, PortableServer, "PortableServer.cpp,v 1.3 2001/09/25 05:12:33 irfan Exp")

int
TAO_POA_Initializer::init (void)
{
  return ACE_Service_Config::process_directive (ace_svc_desc_TAO_Object_Adapter_Factory);
}
