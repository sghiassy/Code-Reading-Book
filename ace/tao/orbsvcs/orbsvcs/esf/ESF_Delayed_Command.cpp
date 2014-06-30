// ESF_Delayed_Command.cpp,v 1.2 2000/04/02 19:22:41 coryan Exp

#ifndef TAO_ESF_DELAYED_COMMAND_CPP
#define TAO_ESF_DELAYED_COMMAND_CPP

#include "ESF_Delayed_Command.h"

#if ! defined (__ACE_INLINE__)
#include "ESF_Delayed_Command.i"
#endif /* __ACE_INLINE__ */

#include "tao/corba.h"

ACE_RCSID(ESF, ESF_Delayed_Command, "ESF_Delayed_Command.cpp,v 1.2 2000/04/02 19:22:41 coryan Exp")

template<class Target, class Object> int
TAO_ESF_Connected_Command<Target,Object>::execute (void* arg)
{
  CORBA::Environment *env = &TAO_default_environment ();
  if (arg != 0)
    env = ACE_static_cast(CORBA::Environment*, arg);

  this->target_->connected_i (this->object_, *env);
  return 0;
}

// ****************************************************************

template<class Target, class Object> int
TAO_ESF_Reconnected_Command<Target,Object>::execute (void* arg)
{
  CORBA::Environment *env = &TAO_default_environment ();
  if (arg != 0)
    env = ACE_static_cast(CORBA::Environment*, arg);

  this->target_->reconnected_i (this->object_, *env);
  return 0;
}

// ****************************************************************

template<class Target, class Object> int
TAO_ESF_Disconnected_Command<Target,Object>::execute (void* arg)
{
  CORBA::Environment *env = &TAO_default_environment ();
  if (arg != 0)
    env = ACE_static_cast(CORBA::Environment*, arg);

  this->target_->disconnected_i (this->object_, *env);
  return 0;
}

// ****************************************************************

template<class Target> int
TAO_ESF_Shutdown_Command<Target>::execute (void* arg)
{
  CORBA::Environment *env = &TAO_default_environment ();
  if (arg != 0)
    env = ACE_static_cast(CORBA::Environment*, arg);

  this->target_->shutdown_i (*env);
  return 0;
}

#endif /* TAO_ESF_DELAYED_COMMAND_CPP */
