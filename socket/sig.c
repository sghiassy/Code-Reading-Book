// socket++ library. sig.C 
// Copyright (C) 1992-1996 Gnanasekaran Swaminathan <gs4t@virginia.edu>
//
// Permission is granted to use at your own risk and distribute this software
// in source and  binary forms provided  the above copyright notice and  this
// paragraph are  preserved on all copies.  This software is provided "as is"
// with no express or implied warranty.

#include <sig.h>

//explicit template instantiation.
typedef sig::phnd phnd;
typedef sig::phndlist phndlist;
template class list<phnd>;
template class map<int, phndlist, less<int> >;

static sigerr se;

siginit siginit::init;
sig& sig::nal = *siginit::init.s;

typedef void (*sighnd_type) (...);

extern "C" {
  static void sighandler (int signo) {
    sig::nal.kill (signo);
  }
}

sig::hnd::hnd (int s)
  : signo (s)
{
  sig::nal.set (signo, this);
}

sig::hnd::~hnd ()
{
  sig::nal.unset (signo, this);
}

bool sig::set (int signo, sig::hnd* hnd)
{
  if (hnd == 0) return false;

  phndlist& v = smap [signo];
  
  if (v.empty ()) {
    struct sigaction sa;
    if (sigaction (signo, 0, &sa) == -1) throw se;
    if (sa.sa_handler != sighnd_type (&sighandler)) {
      // setting for the first time
      sa.sa_handler = sighnd_type (&sighandler);
      if (sigemptyset (&sa.sa_mask) == -1) throw se;
      sa.sa_flags = 0;
      if (sigaction (signo, &sa, 0) == -1) throw se;
    }
    v.push_back (hnd);
    return true;
  }

  phndlist::iterator j = find (v.begin(), v.end (), hnd);
  if (j == v.end ()) {
    v.push_back (hnd);
    return true;
  }
  return false;
}

bool sig::unset (int signo, sig::hnd* hnd)
{
  if (hnd == 0) return false;
  
  phndlist& v = smap [signo];

  phndlist::iterator j = find (v.begin(), v.end (), hnd);
  if (j != v.end ()) {
    v.erase (j);
    return true;
  }

  return false;
}

void sig::unset (int signo)
{
  phndlist& v = smap [signo];
  v.erase (v.begin (), v.end ());
  
  struct sigaction sa;
  if (sigaction (signo, 0, &sa) == -1) throw se;
  if (sa.sa_handler == sighnd_type (&sighandler)) {
    sa.sa_handler = sighnd_type (SIG_DFL);
    if (sigemptyset (&sa.sa_mask) == -1) throw se;
    sa.sa_flags = 0;
    if (sigaction (signo, &sa, 0) == -1) throw se;
  }
}

void sig::mask (int signo) const
{
  sigset_t s;
  if (sigemptyset (&s) == -1) throw se;
  if (sigaddset (&s, signo) == -1) throw se;

  if (sigprocmask (SIG_BLOCK, &s, 0) == -1) throw se;
}

void sig::unmask (int signo) const
{
  sigset_t s;
  if (sigemptyset (&s) == -1) throw se;
  if (sigaddset (&s, signo) == -1) throw se;

  if (sigprocmask (SIG_UNBLOCK, &s, 0) == -1) throw se;
}

void sig::mask (int siga, int sigb) const
{
  struct sigaction sa;
  if (sigaction (siga, 0, &sa) == -1) throw se;
  if (sa.sa_handler != sighnd_type (&sighandler)) {
    sa.sa_handler = sighnd_type (&sighandler);
    if (sigemptyset (&sa.sa_mask) == -1) throw se;
    sa.sa_flags = 0;
  }
  if (sigaddset (&sa.sa_mask, sigb) == -1) throw se;
  if (sigaction (siga, &sa, 0) == -1) throw se;
}

void sig::unmask (int siga, int sigb) const
{
  struct sigaction sa;
  if (sigaction (siga, 0, &sa) == -1) throw se;
  if (sa.sa_handler != sighnd_type (&sighandler)) {
    sa.sa_handler = sighnd_type (&sighandler);
    if (sigemptyset (&sa.sa_mask) == -1) throw se;
    sa.sa_flags = 0;
  } else {
    if (sigdelset (&sa.sa_mask, sigb) == -1) throw se;
  }
  if (sigaction (siga, &sa, 0) == -1) throw se;
}

void sig::sysresume (int signo, bool set) const
{
  struct sigaction sa;
  if (sigaction (signo, 0, &sa) == -1) throw se;
  if (sa.sa_handler != sighnd_type (&sighandler)) {
    sa.sa_handler = sighnd_type (&sighandler);
    if (sigemptyset (&sa.sa_mask) == -1) throw se;
    sa.sa_flags = 0;
  }
  if (set == false)
    sa.sa_flags |= SA_INTERRUPT;
  else
    sa.sa_flags &= ~SA_INTERRUPT;
  if (sigaction (signo, &sa, 0) == -1) throw se;
}

void sig::kill (int signo)
{
  phndlist& v = smap [signo];

  struct procsig {
    int signo;
    procsig (int s): signo (s) {}
    void operator () (phnd& ph) { (*ph) (signo); }
  };

  for_each (v.begin (), v.end (), procsig (signo));
}

sigset_t sig::pending () const
{
  sigset_t s;
  if (sigemptyset (&s) == -1) throw se;
  if (sigpending (&s) == -1) throw se;
  return s;
}

bool sig::ispending (int signo) const
{
  sigset_t s = pending ();
  switch (sigismember (&s, signo)) {
  case 0: return false;
  case 1: return true;
  }
  throw se;
}
