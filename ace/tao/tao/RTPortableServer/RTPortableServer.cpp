/* -*- C++ -*- */

// RTPortableServer.cpp,v 1.3 2001/09/25 05:12:33 irfan Exp
// =================================================================
//
// = LIBRARY
//    TAO
//
// = FILENAME
//    RTPortableServer.cpp
//
// = AUTHOR
//    Frank Hunleth <fhunleth@cs.wustl.edu>
//
// =================================================================

#include "RTPortableServer.h"
#include "RT_Object_Adapter_Factory.h"

ACE_RCSID (RTPortableServer, RTPortableServer, "RTPortableServer.cpp,v 1.3 2001/09/25 05:12:33 irfan Exp")

TAO_RTPortableServer_Initializer::TAO_RTPortableServer_Initializer (void)
{
  ACE_Service_Config::process_directive (ace_svc_desc_TAO_RT_Object_Adapter_Factory);
}
