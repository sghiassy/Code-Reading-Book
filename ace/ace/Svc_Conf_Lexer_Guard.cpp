// -*- C++ -*-

#include "ace/Svc_Conf.h"

#include "ace/Svc_Conf_Lexer_Guard.h"

ACE_RCSID (ace,
           Svc_Conf_Lexer_Guard,
           "Svc_Conf_Lexer_Guard.cpp,v 4.3 2001/07/31 23:49:39 othman Exp")

ACE_Svc_Conf_Lexer_Guard::ACE_Svc_Conf_Lexer_Guard (FILE *file)
{
  // External synchronization is required.

  ::ace_yy_push_buffer (file);
}

ACE_Svc_Conf_Lexer_Guard::ACE_Svc_Conf_Lexer_Guard (const ACE_TCHAR *directive)
{
  // External synchronization is required.

  ::ace_yy_push_buffer (directive);
}


ACE_Svc_Conf_Lexer_Guard::~ACE_Svc_Conf_Lexer_Guard (void)
{
  // External synchronization is required.

  ::ace_yy_pop_buffer ();
}
