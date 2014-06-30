/* -*- C++ -*- */

// =================================================================
/**
 * @file IORTable.cpp
 *
 * IORTable.cpp,v 1.3 2001/09/25 05:12:33 irfan Exp
 *
 * @author Carlos O'Ryan <coryan@uci.edu>
 *
 */
// =================================================================

#include "IORTable.h"
#include "Table_Adapter.h"

ACE_RCSID (IORTable, IORTable, "IORTable.cpp,v 1.3 2001/09/25 05:12:33 irfan Exp")

int
TAO_IORTable_Initializer::init (void)
{
  return ACE_Service_Config::process_directive (ace_svc_desc_TAO_Table_Adapter_Factory);
}
