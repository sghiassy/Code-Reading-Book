// Smart_Proxy_Impl.h,v 1.1 1999/12/25 05:34:22 kirthika Exp

#include "testC.h"

class Smart_Test_Proxy : public virtual TAO_Test_Smart_Proxy_Base 
{
public:
  Smart_Test_Proxy (Test_ptr proxy);

  virtual CORBA::Short method (CORBA::Short boo,
                               CORBA::Environment &ACE_TRY_ENV = 
                               TAO_default_environment ())
    ACE_THROW_SPEC ((CORBA::SystemException, 
                     Test::Oops));
  // "Smartify" the method call!

  // Don't need to add any extra functionality into <shutdown>, hence
  // we don't define it.
};
