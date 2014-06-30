// AddrServer.cpp,v 1.2 2001/05/02 00:02:53 bala Exp

#include "AddrServer.h"

ACE_RCSID(EC_Examples, AddrServer, "AddrServer.cpp,v 1.2 2001/05/02 00:02:53 bala Exp")

AddrServer::AddrServer (const RtecUDPAdmin::UDP_Addr& addr)
  : addr_ (addr)
{
}

void
AddrServer::get_addr (const RtecEventComm::EventHeader&,
                      RtecUDPAdmin::UDP_Addr_out addr,
                      CORBA::Environment&)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  addr = this->addr_;
}
