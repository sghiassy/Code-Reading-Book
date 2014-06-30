/* -*- C++ -*- */
// CLASSIX_Port_Default.i,v 1.1 1998/06/23 09:55:25 wchiang Exp

ACE_INLINE
ACE_CLASSIX_Port_Default::ACE_CLASSIX_Port_Default(void)
    : ACE_CLASSIX_Port()
{
}

ACE_INLINE
void
ACE_CLASSIX_Port_Default::set_addr(void*, int)
{
}

ACE_INLINE
ACE_CLASSIX_Port const&  
ACE_CLASSIX_Port_Default::operator =(ACE_CLASSIX_Port const&)
{
    return *this;
}

ACE_INLINE
ACE_CLASSIX_Port const&  
ACE_CLASSIX_Port_Default::operator =(ACE_Addr const&)
{
    return *this;
}
