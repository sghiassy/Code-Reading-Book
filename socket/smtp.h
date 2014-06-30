// smtp.h -*- C++ -*- socket library
// Copyright (C) 1992-1996 Gnanasekaran Swaminathan <gs4t@virginia.edu>
//
// Permission is granted to use at your own risk and distribute this software
// in source and  binary forms provided  the above copyright notice and  this
// paragraph are  preserved on all copies.  This software is provided "as is"
// with no express or implied warranty.
//
// Version: 12Jan97 1.11

#ifndef SMTP_H
#define SMTP_H

#include <protocol.h>

class smtp: public protocol {
 public:
  class smtpbuf : public protocol::protocolbuf {
    ostream*            o; // send all the responses to o

    void                send_cmd (const char* cmd, const char* s = 0,
	                          const char* p = 0);
    void                get_response ();

    smtpbuf (smtpbuf&);
    smtpbuf& operator = (smtpbuf&);
  public:
    smtpbuf (ostream* out = 0)
	: protocol::protocolbuf (protocol::tcp), o (out) {}

    void                send_buf (const char* buf, int buflen);

    void                helo ();
    void                quit () { send_cmd ("QUIT"); }
    void                turn () { send_cmd ("TURN"); }
    void                rset () { send_cmd ("RSET"); }
    void                noop () { send_cmd ("NOOP"); }
    void                vrfy (const char* s) { send_cmd ("VRFY ", s); }
    void                expn (const char* s) { send_cmd ("EXPN ", s); }

    void                data () { send_cmd ("DATA"); }
    void                data (const char* buf, int buflen);
    void                data (const char* filename); // filename = 0 => stdin

    void                mail (const char* reverse_path);
    void                rcpt (const char* forward_path);
    void                help (const char* s = 0);

    virtual void        serve_clients (int portno = -1);
    virtual const char* rfc_name () const { return "smtp"; }
    virtual const char* rfc_doc  () const { return "rfc821"; }
  };
    
protected:
  smtp(): ios (0) {}

public:
  smtp (ostream* out): ios (0) { ios::init (new smtpbuf (out)); }
  ~smtp () { delete ios::rdbuf (); ios::init (0); }

  int      get_response (char* buf, int len);

  smtpbuf* rdbuf ()       { return (smtpbuf*) protocol::rdbuf (); }
  smtpbuf* operator -> () { return rdbuf (); }
};

extern ostream& operator << (ostream& o, smtp& s);

#endif // SMTP_H
