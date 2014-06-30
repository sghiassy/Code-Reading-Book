// Handle_L_CODgram.cpp,v 4.3 1999/07/23 20:04:41 schmidt Exp

#include "Handle_L_CODgram.h"

#if !defined (ACE_LACKS_UNIX_DOMAIN_SOCKETS)

ACE_RCSID(server, Handle_L_CODgram, "Handle_L_CODgram.cpp,v 4.3 1999/07/23 20:04:41 schmidt Exp")

const char *Handle_L_CODgram::DEFAULT_RENDEZVOUS = "/tmp/foo_codgram";

#if !defined (__ACE_INLINE__)
#include "Handle_L_CODgram.i"
#endif /* __ACE_INLINE__ */

Handle_L_CODgram local_codgram;
ACE_Service_Object_Type lc (&local_codgram, "Local_CODgram");

#endif /* ACE_LACKS_UNIX_DOMAIN_SOCKETS */
