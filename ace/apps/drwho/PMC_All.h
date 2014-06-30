/* -*- C++ -*- */
// PMC_All.h,v 1.3 1998/10/20 02:34:38 levine Exp

// ============================================================================
//
// = LIBRARY
//    drwho
//
// = FILENAME
//    PMC_All.h
//
// = AUTHOR
//    Douglas C. Schmidt
//
// ============================================================================

#ifndef _PMC_ALL_H
#define _PMC_ALL_H

#include "PM_Client.h"

class PMC_All : public PM_Client
{
  // = TITLE
  //   Provides the client's lookup table abstraction for `all' users...

protected:
  virtual Protocol_Record *insert_protocol_info (Protocol_Record &protocol_record);
  virtual int encode (char *packet, int &total_bytes);
  virtual int decode (char *packet, int &total_bytes);

public:
  PMC_All (void);
  virtual void process (void);
};

#endif /* _PMC_ALL_H */
