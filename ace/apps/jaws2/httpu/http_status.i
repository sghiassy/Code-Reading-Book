// -*- c++ -*-
// http_status.i,v 1.1 2000/04/27 23:43:18 jxh Exp

#if !defined (ACE_HAS_INLINED_OSCALLS)
# undef ACE_INLINE
# define ACE_INLINE
#endif /* ACE_HAS_INLINED_OSCALLS */

ACE_INLINE
HTTP_SCode_Node::operator int (void) const
{
  return this->code_;
}

ACE_INLINE
HTTP_SCode_Node::operator const char * (void) const
{
  return this->code_str_;
}
