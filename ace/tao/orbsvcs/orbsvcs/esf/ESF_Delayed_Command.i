// ESF_Delayed_Command.i,v 1.2 2000/04/02 19:22:41 coryan Exp

template<class Target, class Object>
TAO_ESF_Connected_Command<Target,Object>::
      TAO_ESF_Connected_Command (Target *target,
                                Object *object)
  : target_ (target),
    object_ (object)
{
}

// ****************************************************************

template<class Target, class Object>
TAO_ESF_Reconnected_Command<Target,Object>::
      TAO_ESF_Reconnected_Command (Target *target,
                                  Object *object)
  : target_ (target),
    object_ (object)
{
}

// ****************************************************************

template<class Target, class Object>
TAO_ESF_Disconnected_Command<Target,Object>::
      TAO_ESF_Disconnected_Command (Target *target,
                                   Object *object)
  : target_ (target),
    object_ (object)
{
}

// ****************************************************************

template<class Target>
TAO_ESF_Shutdown_Command<Target>::
      TAO_ESF_Shutdown_Command (Target *target)
  : target_ (target)
{
}
