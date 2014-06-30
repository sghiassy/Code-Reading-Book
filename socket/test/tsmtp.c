// socket++ library. tsmtp.C
// Copyright (C) 1992-1996 Gnanasekaran Swaminathan <gs4t@virginia.edu>
//
// Permission is granted to use at your own risk and distribute this software
// in source and  binary forms provided  the above copyright notice and  this
// paragraph are  preserved on all copies.  This software is provided "as is"
// with no express or implied warranty.
//
// Version: 12Jan97 1.11

#include <sockinet.h>
#include <stdio.h>

static void send_cmd(iosockstream&, const char* cmd=0);
static void get_text(iosockstream&);

int main(int ac, char** av)
{
  if (ac != 3) {
    cerr << "USAGE: " << av[0] << " hostname userid\n";
    return 1;
  }
    
  iosockinet sio (sockbuf::sock_stream);
    
  sio->connect(av[1], "smtp", "tcp");
    
  send_cmd(sio, 0);
  send_cmd(sio, "HELO kelvin.seas.virginia.edu");
  send_cmd(sio, "HELP");
  send_cmd(sio, "MAIL FROM:<test@test>");
    
  char buf[512];
  sprintf(buf, "RCPT TO:%s", av[2]);
  send_cmd(sio, buf);
    
  send_cmd(sio, "DATA");
  cout << "terminate your input with cntrl-D\n\n";
  while(cin.getline(buf, 511)) {
    if (buf[0] == '.') sio << '.';
    sio <<  buf << "\r\n";
  }
  sio << "\r\n.\r\n" << flush;
  send_cmd(sio, 0);
    
    
  send_cmd(sio, "HELP");
    
  send_cmd(sio, "QUIT");

  return 0;
}

void send_cmd(iosockstream& s, const char* cmd)
{
  if (cmd) {
    s << cmd << "\r\n" << flush;
  }
  get_text (s);
}

void get_text(iosockstream& s)
{
  char buf[256];
  int tmo = s->recvtimeout(1); // wait for 1 sec before timing out
  while ( s.getline(buf, 255) ) {
    if (buf[0] == '.' && buf[1] == '\r' && buf[2] == '\n') break;
    cout << buf << endl;
  }
  if ( !s.eof() ) s.clear();
  s->recvtimeout(tmo); //reset timeout to the previous value
}
