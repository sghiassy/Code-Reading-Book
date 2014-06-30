/* -*- C++ -*- */
// SM_Client.h,v 1.3 1998/10/20 02:34:40 levine Exp

// ============================================================================
//
// = LIBRARY
//    drwho
//
// = FILENAME
//    SM_Client.h
//
// = AUTHOR
//    Douglas C. Schmidt
//
// ============================================================================

#ifndef _SM_CLIENT_H
#define _SM_CLIENT_H

#include "PM_Client.h"
#include "CM_Client.h"
#include "Select_Manager.h"

class SM_Client : public Select_Manager, public CM_Client
{
public:
  SM_Client (void);
  virtual ~SM_Client (void);

  virtual int mux (char *packet, int &packet_length);
  virtual int demux (char *packet, int &packet_length);
  virtual void process (void);

private:
  PM_Client *pm_client;
};

#endif /* _SM_CLIENT_H */
