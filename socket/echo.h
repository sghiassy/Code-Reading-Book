// echo.h -*- C++ -*- socket library
// Copyright (C) 1992-1996 Gnanasekaran Swaminathan <gs4t@virginia.edu>
//
// Permission is granted to use at your own risk and distribute this software
// in source and  binary forms provided  the above copyright notice and  this
// paragraph are  preserved on all copies.  This software is provided "as is"
// with no express or implied warranty.
//
// Version: 12Jan97 1.11

#ifndef ECHO_H
#define ECHO_H

#include <protocol.h>

class echo: public protocol
{
public:
  class echobuf: public protocol::protocolbuf {
  public:
    echobuf (sockinetbuf& si): protocol::protocolbuf (si) {}
    echobuf (protocol::p_name pname) : protocol::protocolbuf (pname) {}

    virtual void        serve_clients (int portno = -1);
    virtual const char* rfc_name () const { return "echo"; }
    virtual const char* rfc_doc  () const { return "rfc862"; }
  };

protected:
  echo (): ios(0) {}

public:
  echo (protocol::p_name pname)
    : ios (0)
      {
        ios::init (new echobuf (pname));
      }
  ~echo () { delete ios::rdbuf (); ios::init (0); }

  echobuf* rdbuf () { return (echobuf*) protocol::rdbuf (); }
  echobuf* operator -> () { return rdbuf (); }
};

#endif // !ECHO_H
