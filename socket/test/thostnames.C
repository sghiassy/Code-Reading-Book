// socket++ library. thostnames.C
// Copyright (C) 1992-1996 Gnanasekaran Swaminathan <gs4t@virginia.edu>
//
// Permission is granted to use at your own risk and distribute this software
// in source and  binary forms provided  the above copyright notice and  this
// paragraph are  preserved on all copies.  This software is provided "as is"
// with no express or implied warranty.
//
// Version: 12Jan97 1.11

#include <sockinet.h>
#include <ctype.h>
#include <string.h>

static void get_text (iosockstream& s);

int main(int ac, char** av)
{
  if (ac != 2) {
    cerr << "USAGE: " << av[0] << " hostname\n";
    return 1;
  }
	
  iosockinet	sio(sockbuf::sock_stream);
  sio->connect ("nic.ddn.mil", "hostnames", "tcp");

  sio << "HNAME " << av[1] << "\r\n" << flush;
  get_text (sio);

  return 0;
}

void get_text(iosockstream& s)
{
  char buf[1024];
  int tmo = s->recvtimeout(30); // wait for 30 sec before timing out

  if (s.getline (buf, 1023))
    cout << buf << endl;

  if (strncmp (buf, "BEGIN", 5) == 0)
    while (s.getline (buf, 1023)) {
      cout << buf << endl;
      if (strncmp (buf, "END", 3) == 0) break;
    }
  else
    while (s.getline (buf, 1023)) {
      cout << buf << endl;
      if (!isspace (buf [0])) break;
    }
  if ( !s.eof() ) s.clear();
  s->recvtimeout(tmo); //reset timeout to the previous value
}
