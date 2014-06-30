// socket++ library. tsig-0.C
// Copyright (C) 1992-1996 Gnanasekaran Swaminathan <gs4t@virginia.edu>
//
// Permission is granted to use at your own risk and distribute this software
// in source and  binary forms provided  the above copyright notice and  this
// paragraph are  preserved on all copies.  This software is provided "as is"
// with no express or implied warranty.

#include <iostream.h>
#include <sig.h>
#include <unistd.h>

int main (int ac, char** av)
{
  struct hnd: public sig::hnd {
    int id;

    hnd (int signo): id (signo), sig::hnd (signo) {}
    hnd (int signo, int i): id (i), sig::hnd (signo) {}
    void operator () (int s) {
      cout << "handler for " << id << endl;
      sleep (30);
      cout << "Pending signals = " << hex << sig::nal.pending () << dec << endl;
      cout << "called by " << s << endl;
    }
  };

  hnd sterm0 (SIGTERM);
  hnd sterm1 (SIGTERM, SIGTERM * 100);
  hnd sint (SIGINT);
  hnd susr1 (SIGUSR1);
  hnd susr2 (SIGUSR2);
  hnd squit (SIGQUIT);

  /*
  sig::nal.set (SIGTERM, &sterm0);
  sig::nal.set (SIGTERM, &sterm1);
  sig::nal.set (SIGINT, &sint);
  sig::nal.set (SIGUSR1, &susr1);
  sig::nal.set (SIGUSR2, &susr2);
  sig::nal.set (SIGQUIT, &squit);
  */

  try {
    hnd illegal (300);
  }
  catch (sigerr e) {
    cout << "sigerr exception thrown by illegal(300)\n";
  }

  sig::nal.mask (SIGTERM, SIGUSR1);
  sig::nal.mask (SIGTERM, SIGUSR2);

  pid_t pid = getpid ();

  cout << "send SIGUSR2\n";
  kill (pid, SIGTERM);

  sleep (20);
  sig::nal.unmask (SIGTERM, SIGUSR2);
  
  cout << "send SIGUSR2 again\n";
  kill (pid, SIGTERM);

  sleep (300);
  return 0;
}
