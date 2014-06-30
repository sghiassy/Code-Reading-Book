// URL.cpp,v 1.2 1999/05/03 21:10:15 kirthika Exp

#include "URL.h"

ACE_RCSID(Web_Crawler, URL, "URL.cpp,v 1.2 1999/05/03 21:10:15 kirthika Exp")

Mem_Map_Stream &
URL::stream (void)
{
  return this->stream_;
}

URL::~URL (void)
{
}

const URL_Status &
URL::reply_status (void)
{
  return this->reply_status_;
}

void 
URL::reply_status (const URL_Status &rs)
{
  this->reply_status_ = rs;
}

const ACE_CString &
URL::content_type (void)
{
  return this->content_type_;
}

void 
URL::content_type (const ACE_CString &ct)
{
  this->content_type_ = ct;
}
