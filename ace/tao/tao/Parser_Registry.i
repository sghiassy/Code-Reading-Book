// Parser_Registry.i,v 1.2 2000/09/14 23:41:02 doccvs Exp

ACE_INLINE TAO_Parser_Registry::Parser_Iterator
TAO_Parser_Registry::begin (void) const
{
  return this->parsers_;
}

ACE_INLINE TAO_Parser_Registry::Parser_Iterator
TAO_Parser_Registry::end (void) const
{
  return this->parsers_ + this->size_;
}
