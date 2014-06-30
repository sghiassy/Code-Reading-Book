/* -*- C++ -*- */
// CLASSIX_CLD_Connector.i,v 1.1 1998/06/23 09:55:23 wchiang Exp
//
// CLASSIX/CLD_Connector.i
/* ------------------------------------------------------------------------- */

// Do-nothing constructor...

ACE_INLINE
ACE_CLASSIX_CLD_Connector::ACE_CLASSIX_CLD_Connector (void)
{
  ACE_TRACE ("ACE_CLASSIX_CLD_Connector::ACE_CLASSIX_CLD_Connector");
}

ACE_INLINE
int 
ACE_CLASSIX_CLD_Connector::reset_new_handle (ACE_HANDLE /*  handle */)
{
  return 0;
}

/* ------------------------------------------------------------------------- */


