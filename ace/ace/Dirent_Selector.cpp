// Dirent_Selector.cpp,v 4.3 2001/07/09 23:48:07 othman Exp

#include "ace/OS.h"
#include "ace/Dirent_Selector.h"

#if !defined (__ACE_INLINE__)
#include "ace/Dirent_Selector.inl"
#endif /* __ACE_INLINE__ */

ACE_RCSID (ace,
           Dirent_Selector,
           "Dirent_Selector.cpp,v 4.3 2001/07/09 23:48:07 othman Exp")

// Construction/Destruction

ACE_Dirent_Selector::ACE_Dirent_Selector (void)
  : namelist_ (0),
    n_ (0)
{
}

ACE_Dirent_Selector::~ACE_Dirent_Selector (void)
{
}

int
ACE_Dirent_Selector::open (const ACE_TCHAR *dir,
                           int (*sel)(const dirent *d),
                           int (*cmp) (const dirent **d1,
                                       const dirent **d2))
{
  n_ = ACE_OS::scandir (dir, &this->namelist_, sel, cmp);
  return n_;
}

int
ACE_Dirent_Selector::close (void)
{
  for (--n_; n_>=0; --n_)
    ACE_OS::free (this->namelist_[n_]);

  ACE_OS::free (this->namelist_);
  return 0;
}
