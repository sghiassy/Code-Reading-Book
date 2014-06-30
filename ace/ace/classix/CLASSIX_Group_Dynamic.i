/* -*- C++ -*- */
// CLASSIX_Group_Dynamic.i,v 1.1 1998/06/23 09:55:24 wchiang Exp

ACE_INLINE
ACE_CLASSIX_Group_Dynamic::ACE_CLASSIX_Group_Dynamic(void)
{
    if (::grpAllocate( K_DYNAMIC, &this->addr_.group_, 0) == 0)
	this->set_config_();
}

ACE_INLINE
ACE_CLASSIX_Addr::Addr_Type
ACE_CLASSIX_Group_Dynamic::is_type(void) const
{
    return ACE_CLASSIX_Addr::DYNAMIC;
}
