// ReceiverImpl.h,v 1.1 1999/06/29 01:36:42 schmidt Exp
//---------------------------------------------------------------------------
#ifndef ReceiverImplH
#define ReceiverImplH
//---------------------------------------------------------------------------
#include "ReceiverS.h"
//---------------------------------------------------------------------------
class TReceiverImplementation : public POA_Receiver
{
public:
  // = Initialization and termination methods.
  TReceiverImplementation (void);
  // Constructor.

  ~TReceiverImplementation (void);
  // Destructor.

  virtual void message (const char* msg) throw (CORBA::SystemException);

  virtual void shutdown (void) throw (CORBA::SystemException);
  // Called when the chat server is going away.  The client
  // implementation should shutdown the chat client in response to
  // this.

  void orb (CORBA::ORB_ptr o);
  // Set the ORB pointer.

private:
  CORBA::ORB_var orb_;
  // ORB pointer.
};
//---------------------------------------------------------------------------
#endif
