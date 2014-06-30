// ValueFactory.i,v 1.2 2001/02/02 02:32:05 fhunleth Exp

ACE_INLINE
CORBA_ValueFactoryBase::CORBA_ValueFactoryBase ()
  : _tao_reference_count_ (1)
{
}

ACE_INLINE void
CORBA_ValueFactoryBase::_add_ref (void)
{
  ACE_GUARD (TAO_SYNCH_MUTEX, guard, this->_tao_reference_count_lock_);
  ++_tao_reference_count_;
}

ACE_INLINE void
CORBA_ValueFactoryBase::_remove_ref (void)
{
  {
    ACE_GUARD (TAO_SYNCH_MUTEX, guard, this->_tao_reference_count_lock_);
    -- this->_tao_reference_count_;
    if (this->_tao_reference_count_ != 0)
      return;
  }
  delete this;
}
