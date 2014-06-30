// ============================================================================
//
// Scheduler_Factory.i,v 1.3 1998/12/31 04:55:06 schmidt Exp
//
// ============================================================================

// This helper function allows the application to determine whether
// the factory is uninitialized, or in a config or runtime mode of
// operation.

ACE_INLINE ACE_Scheduler_Factory::Factory_Status 
ACE_Scheduler_Factory::status (void)
{
  return status_;
}
