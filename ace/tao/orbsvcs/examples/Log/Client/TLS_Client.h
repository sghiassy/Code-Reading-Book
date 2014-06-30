// -*- C++ -*-
// TLS_Client.h,v 1.2 2000/09/01 05:05:49 coryan Exp

#ifndef TLS_CLIENT_H
#define TLS_CLIENT_H

#include "orbsvcs/DsLogAdminC.h"
#include "orbsvcs/CosNamingC.h"

class TLS_Client
{
  // = TITLE
  //   Telecom Log Service Client
  // = DESCRIPTION
  //   This client demonstates how to use the log service.
 public:
  // = Initialization and Termination
  TLS_Client (void);
  ~TLS_Client ();

  void init (int argc, char *argv [], CORBA::Environment &ACE_TRY_ENV);
  // Init the Client.

  void run_tests (CORBA::Environment &ACE_TRY_ENV);
  // Run the tests..

 protected:
  void init_ORB (int argc, char *argv [], CORBA::Environment &ACE_TRY_ENV);
  // Initializes the ORB.

  void resolve_naming_service (CORBA::Environment &ACE_TRY_ENV);
  // Try to get hold of a running naming service.

  void resolve_TLS_Basic_factory (CORBA::Environment &ACE_TRY_ENV);
  // Try to resolve the TLS factory from the Naming service.

  // = Data Members
  CORBA::ORB_var orb_;
  // The ORB that we use.

  CosNaming::NamingContext_var naming_context_;
  // Handle to the name service.

  DsLogAdmin::BasicLogFactory_var basic_log_factory_;
  // The basic log factory from the Log Service.

};
#endif /* TLS_CLIENT_H */
