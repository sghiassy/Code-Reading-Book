/* -*- C++ -*- */
// Gateway.h,v 4.5 1998/10/20 02:34:18 levine Exp

// ============================================================================
//
// = LIBRARY
//    gateway
//
// = FILENAME
//    Gateway.h
//
// = DESCRIPTION
//    Since the Gateway is an <ACE_Service_Object>, this file defines
//    the entry point into the Service Configurator framework.
//
// = AUTHOR
//    Doug Schmidt
//
// ============================================================================

#ifndef ACE_GATEWAY
#define ACE_GATEWAY

#include "ace/OS.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

ACE_SVC_FACTORY_DECLARE (Gateway)

#endif /* ACE_GATEWAY */

