//=============================================================================
/**
 *  @file   Locator.cpp
 *
 *  Locator.cpp,v 1.1 2000/11/10 20:51:19 brunsch Exp
 *
 *  @brief  Implementation of the ImR's INS Locator class
 *
 *  @author Darrell Brunsch <brunsch@cs.wustl.edu>
 */
//=============================================================================

#include "Locator.h"
#include "ImplRepo_i.h"

ImR_Locator::ImR_Locator (ImplRepo_i *repo)
  :  repo_ (repo)
{
}

char *
ImR_Locator::locate (const char *object_key,
                     CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException, IORTable::NotFound))
{
  ACE_CString key (object_key);
  return this->repo_->find_ior (key, ACE_TRY_ENV);
}


