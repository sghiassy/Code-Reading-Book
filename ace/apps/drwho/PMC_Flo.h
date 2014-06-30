/* -*- C++ -*- */
// PMC_Flo.h,v 1.3 1998/10/20 02:34:38 levine Exp

// ============================================================================
//
// = LIBRARY
//    drwho
//
// = FILENAME
//    PMC_Flo.h
//
// = AUTHOR
//    Douglas C. Schmidt
//
// ============================================================================

#ifndef _PMC_FLO_H
#define _PMC_FLO_H

#include "PM_Client.h"

class PMC_Flo : public PM_Client
{
  // = TITLE
  //   Provides the client's lookup table abstraction for `flo' users...

public:
  PMC_Flo (void);
  virtual void process (void);

protected:
  virtual Protocol_Record *insert_protocol_info (Protocol_Record &protocol_record);
  virtual int encode (char *packet, int &total_bytes);
  virtual int decode (char *packet, int &total_bytes);
};

#endif /* _PMC_FLO_H */
