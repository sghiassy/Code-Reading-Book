// sockinet.C  -*- C++ -*- socket library
// Copyright (C) 1992-1996 Gnanasekaran Swaminathan <gs4t@virginia.edu>
//
// Permission is granted to use at your own risk and distribute this software
// in source and  binary forms provided  the above copyright notice and  this
// paragraph are  preserved on all copies.  This software is provided "as is"
// with no express or implied warranty.
//
// Version: 12Jan97 1.11

#include <config.h>
#include <sockinet.h>

EXTERN_C_BEGIN
#include <netdb.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/tcp.h>
EXTERN_C_END

void	herror(const char*);

sockinetaddr::sockinetaddr () 
{
  sin_family	  = sockinetbuf::af_inet;
  sin_addr.s_addr = htonl(INADDR_ANY);
  sin_port	  = 0;
}

sockinetaddr::sockinetaddr(unsigned long addr, int port_no)
// addr and port_no are in host byte order
{
  sin_family      = sockinetbuf::af_inet;
  sin_addr.s_addr = htonl(addr);
  sin_port	  = htons(port_no);
}

sockinetaddr::sockinetaddr(unsigned long addr, const char* sn, const char* pn)
// addr is in host byte order
{
  sin_family      = sockinetbuf::af_inet;
  sin_addr.s_addr = htonl (addr); // Added by cgay@cs.uoregon.edu May 29, 1993
  setport(sn, pn);
}

sockinetaddr::sockinetaddr (const char* host_name, int port_no)
// port_no is in host byte order
{
  setaddr(host_name);
  sin_port = htons(port_no);
}

sockinetaddr::sockinetaddr(const char* hn, const char* sn, const char* pn)
{
  setaddr(hn);
  setport(sn, pn);
}

sockinetaddr::sockinetaddr (const sockinetaddr& sina)
{
  sin_family      = sockinetbuf::af_inet;
  sin_addr.s_addr = sina.sin_addr.s_addr;
  sin_port	  = sina.sin_port;
}   

void sockinetaddr::setport(const char* sn, const char* pn)
{
  servent* sp = getservbyname(sn, pn);
  if (sp == 0) throw sockerr (EADDRNOTAVAIL);
  sin_port = sp->s_port;
}

int sockinetaddr::getport () const
{
  return ntohs (sin_port);
}

void sockinetaddr::setaddr(const char* host_name)
{
  if ( (sin_addr.s_addr = inet_addr(host_name)) == -1) {
    hostent* hp = gethostbyname(host_name);
    if (hp == 0) throw sockerr (EADDRNOTAVAIL);
    memcpy(&sin_addr, hp->h_addr, hp->h_length);
    sin_family = hp->h_addrtype;
  } else
    sin_family = sockinetbuf::af_inet;
}

const char* sockinetaddr::gethostname () const
{
  if (sin_addr.s_addr == htonl(INADDR_ANY)) {
    static char hostname[64];
    if (::gethostname(hostname, 63) == -1) return "";
    return hostname;		
  }
  
  hostent* hp = gethostbyaddr((const char*) &sin_addr,
			      sizeof(sin_addr),
			      family());
  if (hp == 0) return "";
  if (hp->h_name) return hp->h_name;
  return "";
}

sockinetbuf::sockinetbuf (const sockbuf::sockdesc& sd)
  : sockbuf (sd.sock)
{}

sockinetbuf::sockinetbuf(sockbuf::type ty, int proto)
  : sockbuf (af_inet, ty, proto)
{}

sockinetbuf& sockinetbuf::operator = (const sockinetbuf& si)
{
  sockbuf::operator = (si);
  return *this;
}

sockinetaddr sockinetbuf::localaddr() const
{
  sockinetaddr sin;
  int len = sin.size();
  if (::getsockname(rep->sock, sin.addr (), &len) == -1)
    throw sockerr (errno);
  return sin;
}

int sockinetbuf::localport() const
{
  sockinetaddr sin = localaddr();
  if (sin.family() != af_inet) return -1;
  return sin.getport();
}

const char* sockinetbuf::localhost() const
{
  sockinetaddr sin = localaddr();
  if (sin.family() != af_inet) return "";
  return sin.gethostname();
}

sockinetaddr sockinetbuf::peeraddr() const
{
  sockinetaddr sin;
  int len = sin.size();
  if (::getpeername(rep->sock, sin.addr (), &len) == -1)
    throw sockerr (errno);
  return sin;
}

int sockinetbuf::peerport() const
{
  sockinetaddr sin = peeraddr();
  if (sin.family() != af_inet) return -1;
  return sin.getport();
}

const char* sockinetbuf::peerhost() const
{
  sockinetaddr sin = peeraddr();
  if (sin.family() != af_inet) return "";
  return sin.gethostname();
}

void sockinetbuf::bind_until_success (int portno)
// a. bind to (INADDR_ANY, portno)
// b. if success return
// c. if failure and errno is EADDRINUSE, portno++ and go to step a.
{
  for (;;) {
    try {
      bind (portno++);
    }
    catch (sockerr e) {
      if (e.errno () != EADDRINUSE) throw;
      continue;
    }
    break;
  }
}

void sockinetbuf::bind (sockAddr& sa)
{
  sockbuf::bind (sa);
}

void sockinetbuf::bind (int port_no)
{
  sockinetaddr sa (INADDR_ANY, port_no);
  bind (sa);
}

void sockinetbuf::bind (unsigned long addr, int port_no)
// address and portno are in host byte order
{
  sockinetaddr sa (addr, port_no);
  bind (sa);
}

void sockinetbuf::bind (const char* host_name, int port_no)
{
  sockinetaddr sa (host_name, port_no);
  bind (sa);
}

void sockinetbuf::bind (unsigned long addr,
			const char* service_name,
			const char* protocol_name)
{
  sockinetaddr sa (addr, service_name, protocol_name);
  bind (sa);
}

void sockinetbuf::bind (const char* host_name,
			const char* service_name,
			const char* protocol_name)
{
  sockinetaddr sa (host_name, service_name, protocol_name);
  bind (sa);
}

void sockinetbuf::connect (sockAddr& sa)
{
  sockbuf::connect (sa);
}

void sockinetbuf::connect (unsigned long addr, int port_no)
// address and portno are in host byte order
{
  sockinetaddr sa (addr, port_no);
  connect (sa);
}

void sockinetbuf::connect (const char* host_name, int port_no)
{
  sockinetaddr sa (host_name, port_no);
  connect (sa);
}

void sockinetbuf::connect (unsigned long addr,
			   const char* service_name,
			   const char* protocol_name)
{
  sockinetaddr sa (addr, service_name, protocol_name);
  connect (sa);
}

void sockinetbuf::connect (const char* host_name,
			   const char* service_name,
			   const char* protocol_name)
{
  sockinetaddr sa (host_name, service_name, protocol_name);
  connect (sa);
}

sockbuf::sockdesc sockinetbuf::accept ()
{
  return sockbuf::accept ();
}

sockbuf::sockdesc sockinetbuf::accept (sockAddr& sa)
{
  return sockbuf::accept (sa);
}

sockbuf::sockdesc sockinetbuf::accept (unsigned long addr,
				      int port_no)
{
  sockinetaddr sa (addr, port_no);
  return accept (sa);
}

sockbuf::sockdesc sockinetbuf::accept (const char* host_name,
				      int port_no)
{
  sockinetaddr sa (host_name, port_no);
  return accept (sa);
}

bool sockinetbuf::tcpnodelay () const
{
  struct protoent* proto = getprotobyname ("tcp");
  if (proto == 0) throw sockerr (ENOPROTOOPT);

  int old = 0;
  getopt (TCP_NODELAY, &old, sizeof (old), proto->p_proto);
  return old;
}

bool sockinetbuf::tcpnodelay (bool set) const
{
  struct protoent* proto = getprotobyname ("tcp");
  if (proto == 0) throw sockerr (ENOPROTOOPT);

  int old = 0;
  int opt = set;
  getopt (TCP_NODELAY, &old, sizeof (old), proto->p_proto);
  setopt (TCP_NODELAY, &opt, sizeof (opt), proto->p_proto);
  return old;
}

isockinet::isockinet (const sockbuf::sockdesc& sd)
  : ios (new sockinetbuf (sd))
{}

isockinet::isockinet (sockbuf::type ty, int proto)
  : ios (new sockinetbuf (ty, proto))
{}

isockinet::isockinet (const sockinetbuf& sb)
  : ios (new sockinetbuf (sb))
{}

isockinet::~isockinet ()
{
  delete ios::rdbuf ();
}

osockinet::osockinet (const sockbuf::sockdesc& sd)
  : ios (new sockinetbuf (sd))
{}

osockinet::osockinet (sockbuf::type ty, int proto)
  : ios (new sockinetbuf (ty, proto))
{}

osockinet::osockinet (const sockinetbuf& sb)
  : ios (new sockinetbuf (sb))
{}

osockinet::~osockinet ()
{
  delete ios::rdbuf ();
}

iosockinet::iosockinet (const sockbuf::sockdesc& sd)
  : ios (new sockinetbuf (sd))
{}

iosockinet::iosockinet (sockbuf::type ty, int proto)
  : ios (new sockinetbuf (ty, proto))
{}

iosockinet::iosockinet (const sockinetbuf& sb)
  : ios (new sockinetbuf (sb))
{}

iosockinet::~iosockinet ()
{
  delete ios::rdbuf ();
}
