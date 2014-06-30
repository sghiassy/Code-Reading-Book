// FIFO-Msg-server.cpp,v 4.6 2000/11/08 22:16:24 coryan Exp

#include "ace/FIFO_Recv_Msg.h"
#include "ace/Log_Msg.h"

ACE_RCSID(FIFO_SAP, FIFO_Msg_server, "FIFO-Msg-server.cpp,v 4.6 2000/11/08 22:16:24 coryan Exp")

#if defined (ACE_HAS_STREAM_PIPES)

int
main (int, char *[])
{
  ACE_OS::unlink (ACE_DEFAULT_RENDEZVOUS);
  ACE_FIFO_Recv_Msg server (ACE_DEFAULT_RENDEZVOUS);
  char buf[BUFSIZ];
  ACE_Str_Buf msg (buf, 0, sizeof buf);
  int flags = MSG_ANY;
  int band = 0;
  int n;

  while ((n = server.recv (&band, &msg, (ACE_Str_Buf *) 0, &flags)) >= 0)
    {
      if (msg.len == 0)
	break;
      else
	ACE_DEBUG ((LM_DEBUG, "%4d (%4d): %*s",
		    msg.len, band, msg.len, msg.buf));
      flags = MSG_ANY;
      band = 0;
    }

  if (n == -1)
    ACE_OS::perror ("recv"), ACE_OS::exit (1);

  return 0;
}
#else
#include <stdio.h>
int main (int, char *[])
{
  ACE_OS::fprintf (stderr, "This feature is not supported\n");
  return 0;
}
#endif /* ACE_HAS_STREAM_PIPES */
