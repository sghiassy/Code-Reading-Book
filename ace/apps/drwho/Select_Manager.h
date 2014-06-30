/* -*- C++ -*- */
// Select_Manager.h,v 1.3 1998/10/20 02:34:41 levine Exp

// ============================================================================
//
// = LIBRARY
//    drwho
//
// = FILENAME
//    Select_Manager.h
//
// = AUTHOR
//    Douglas C. Schmidt
//
// ============================================================================

#ifndef _SELECT_MANAGER_H
#define _SELECT_MANAGER_H

class Select_Manager
{
public:
  virtual int mux (char *packet,
                   int &packet_length) = 0 ;

  virtual int demux (char *packet,
                     int &packet_length) = 0;
};

#endif /* _SELECT_MANAGER_H */
