// -*- C++ -*-
//
// Dirent_Selector.inl,v 4.2 2001/07/09 23:48:07 othman Exp

ACE_INLINE int
ACE_Dirent_Selector::length (void) const
{
  return n_;
}

ACE_INLINE dirent *
ACE_Dirent_Selector::operator[] (const int n) const
{
  return this->namelist_[n];
}
