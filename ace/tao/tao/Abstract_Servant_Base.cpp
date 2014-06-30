// Abstract_Servant_Base.cpp,v 1.4 2000/09/24 17:30:29 corsaro Exp
#include "tao/Abstract_Servant_Base.h"

ACE_RCSID (tao, Abstract_Servant_Base, "Abstract_Servant_Base.cpp,v 1.4 2000/09/24 17:30:29 corsaro Exp")

TAO_Abstract_ServantBase::TAO_Abstract_ServantBase (void)
{
  // No-Op.
}

TAO_Abstract_ServantBase::~TAO_Abstract_ServantBase (void)
{
  // No-Op.
}



void
TAO_Abstract_ServantBase::_add_ref (CORBA_Environment &)
{
}

void
TAO_Abstract_ServantBase::_remove_ref (CORBA_Environment &)
{
}


TAO_Abstract_ServantBase::TAO_Abstract_ServantBase (const TAO_Abstract_ServantBase &)
{
  // No-Op
}

TAO_Abstract_ServantBase & TAO_Abstract_ServantBase::operator= (const TAO_Abstract_ServantBase&)
{
  return *this;
} 




