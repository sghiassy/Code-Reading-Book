// RTCORBA.cpp,v 1.4 2001/09/25 05:12:33 irfan Exp

#include "RTCORBA.h"
#include "RT_ORB_Loader.h"

ACE_RCSID (RTCORBA, RTCORBA, "RTCORBA.cpp,v 1.4 2001/09/25 05:12:33 irfan Exp")

TAO_RTCORBA_Initializer::TAO_RTCORBA_Initializer (void)
{
  ACE_Service_Config::process_directive (ace_svc_desc_TAO_RT_ORB_Loader);
}
