// Handle_L_Stream.cpp,v 4.3 2000/10/26 14:05:24 schmidt Exp

#include "Handle_L_Stream.h"

ACE_RCSID(server, Handle_L_Stream, "Handle_L_Stream.cpp,v 4.3 2000/10/26 14:05:24 schmidt Exp")

#if !defined (ACE_LACKS_UNIX_DOMAIN_SOCKETS)

// Static variables.

const char *Handle_L_Stream::DEFAULT_RENDEZVOUS = "/tmp/foo_stream";
char *Handle_L_Stream::login_name = 0;
char Handle_L_Stream::login[ACE_MAX_USERID];

#if !defined (__ACE_INLINE__)
#include "Handle_L_Stream.i"
#endif /* __ACE_INLINE__ */

Handle_L_Stream local_stream;
ACE_Service_Object_Type ls (&local_stream, "Local_Stream");

#endif /* ACE_LACKS_UNIX_DOMAIN_SOCKETS */
