/* -*- C++ -*- */
// Dynamic.i,v 4.3 2001/03/04 00:55:26 brunsch Exp

// Dynamic.i

ACE_INLINE
ACE_Dynamic::~ACE_Dynamic (void)
{
  ACE_TRACE ("ACE_Dynamic::~ACE_Dynamic");
}

ACE_INLINE void
ACE_Dynamic::set (void)
{ 
  ACE_TRACE ("ACE_Dynamic::set");
  this->is_dynamic_ = 1; 
}

ACE_INLINE int 
ACE_Dynamic::is_dynamic ()
{ 
  ACE_TRACE ("ACE_Dynamic::is_dynamic");
  return this->is_dynamic_;
}

ACE_INLINE void
ACE_Dynamic::reset (void)
{ 
  ACE_TRACE ("ACE_Dynamic::reset");
  this->is_dynamic_ = 0; 
}

