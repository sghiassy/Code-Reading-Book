// SL_Client.cpp,v 1.5 1998/09/13 03:44:51 schmidt Exp

#include "Options.h"
#include "SL_Client.h"

SL_Client::SL_Client (const char *usr_name)
  : Single_Lookup (usr_name)
{
}

Protocol_Record *
SL_Client::insert (const char *, int)
{
  return this->prp_;
}
