//
// RMCast_Sequencer.cpp,v 1.1 2000/10/11 00:57:08 coryan Exp
//

#include "RMCast_Sequencer.h"

#if !defined (__ACE_INLINE__)
# include "RMCast_Sequencer.i"
#endif /* ! __ACE_INLINE__ */

ACE_RCSID(ace, RMCast_Sequencer, "RMCast_Sequencer.cpp,v 1.1 2000/10/11 00:57:08 coryan Exp")

ACE_RMCast_Sequencer::~ACE_RMCast_Sequencer (void)
{
}

int
ACE_RMCast_Sequencer::data (ACE_RMCast::Data &data)
{
  {
    ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, ace_mon, this->mutex_, -1);
    data.sequence_number = this->sequence_number_generator_++;
  }
  return this->ACE_RMCast_Module::data (data);
}
