// pch.h,v 1.2 2001/03/07 01:57:29 brunsch Exp
// pch.h

#ifndef i_pch_h
#define i_pch_h

#include "ace/config.h"
#include "wx/wxprec.h"

#ifdef WX_PRECOMP
#include "tao/corba.h"
#endif

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "tao/corba.h"
#endif

#endif
