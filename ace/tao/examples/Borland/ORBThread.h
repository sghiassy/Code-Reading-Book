// ORBThread.h,v 1.1 2000/12/03 00:21:56 doccvs Exp
//---------------------------------------------------------------------------
#ifndef ORBThreadH
#define ORBThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <tao/corba.h>
//---------------------------------------------------------------------------
class PACKAGE TORBThread : public TThread
{
  // Class for running the orb in a separate thread.
public: 
  // = Initialization and termination methods.
  __fastcall TORBThread (CORBA::ORB_ptr orb);
  // Constructor.

  __fastcall ~TORBThread ();
  // Destructor.

protected:
  void __fastcall Execute ();
  // Perform the thread's work.

private:
  CORBA::ORB_var orb_;
  // Reference to the orb.
};
//---------------------------------------------------------------------------
#endif
