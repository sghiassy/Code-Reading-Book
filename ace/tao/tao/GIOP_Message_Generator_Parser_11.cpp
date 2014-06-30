#include "tao/GIOP_Message_Generator_Parser_11.h"


ACE_RCSID(tao, GIOP_Message_Gen_Parser_11, "GIOP_Message_Generator_Parser_11.cpp,v 1.2 2000/12/07 23:39:26 bala Exp")

CORBA::Octet
TAO_GIOP_Message_Generator_Parser_11::minor_version (void)
{
  return (CORBA::Octet) 1;
}
