/* -*- C++ -*- */
// CLASSIX_OS.i,v 1.1 1998/06/23 09:55:24 wchiang Exp

/* ------------------------------------------------------------------------- */
ACE_INLINE
const KnUniqueId&
ACE_CLASSIX_OS::null_KnUniqueId(void)
{
    return null_KnUniqueId_;
}


ACE_INLINE
ACE_Recursive_Thread_Mutex&
ACE_CLASSIX_OS::get_lock_(void)
{
    return lock_;
}

/* ------------------------------------------------------------------------- */
ACE_INLINE
ACE_CLASSIX_Msg::ACE_CLASSIX_Msg(void)
{
    // Make a null-filled message
    this->msg_.flags = 0;
    this->msg_.bodySize = 0;
    this->msg_.bodyAddr = 0;
    this->msg_.annexAddr = 0;
    this->msg_.seqNum = 0;
}

ACE_INLINE
ACE_CLASSIX_Msg::ACE_CLASSIX_Msg(const void* theBuf, int theLen)
{
    this->msg_.flags = 0;
    this->msg_.annexAddr = 0;
    this->msg_.seqNum = 0;

    this->msg_.bodySize = theLen;
    this->msg_.bodyAddr = (VmAddr) theBuf;
}

ACE_INLINE
void
ACE_CLASSIX_Msg::set(const void* theBuf, int theLen)
{
    this->msg_.bodySize = theLen;
    this->msg_.bodyAddr = (VmAddr) theBuf;
}

ACE_INLINE
KnMsgDesc*
ACE_CLASSIX_Msg::get(void)
{
    return &this->msg_;
}
