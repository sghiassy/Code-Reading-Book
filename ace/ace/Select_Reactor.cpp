// Select_Reactor.cpp,v 4.43 2000/04/19 02:49:39 brunsch Exp

#include "ace/Select_Reactor.h"

ACE_RCSID(ace, Select_Reactor, "Select_Reactor.cpp,v 4.43 2000/04/19 02:49:39 brunsch Exp")


#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
# if defined (ACE_MT_SAFE) && (ACE_MT_SAFE != 0)
#   if defined (ACE_SELECT_REACTOR_HAS_DEADLOCK_DETECTION)
template class ACE_Select_Reactor_Token_T<ACE_Local_Mutex>;
template class ACE_Select_Reactor_T< ACE_Select_Reactor_Token_T<ACE_Local_Mutex> >;
template class ACE_Lock_Adapter< ACE_Select_Reactor_Token_T<ACE_Local_Mutex> >;
template class ACE_Guard< ACE_Select_Reactor_Token_T<ACE_Local_Mutex> >;
#   else
template class ACE_Select_Reactor_Token_T<ACE_Token>;
template class ACE_Select_Reactor_T< ACE_Select_Reactor_Token_T<ACE_Token> >;
template class ACE_Lock_Adapter< ACE_Select_Reactor_Token_T<ACE_Token> >;
template class ACE_Guard< ACE_Select_Reactor_Token_T<ACE_Token> >;
#   endif /* ACE_SELECT_REACTOR_HAS_DEADLOCK_DETECTION */
# else
template class ACE_Select_Reactor_Token_T<ACE_Noop_Token>;
template class ACE_Select_Reactor_T< ACE_Select_Reactor_Token_T<ACE_Noop_Token> >;
template class ACE_Lock_Adapter< ACE_Select_Reactor_Token_T<ACE_Noop_Token> >;
# endif /* ACE_MT_SAFE && ACE_MT_SAFE != 0 */
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
# if defined (ACE_MT_SAFE) && (ACE_MT_SAFE != 0)
#   if defined (ACE_SELECT_REACTOR_HAS_DEADLOCK_DETECTION)
#     pragma instantiate ACE_Select_Reactor_Token_T<ACE_Local_Mutex>
#     pragma instantiate ACE_Select_Reactor_T< ACE_Select_Reactor_Token_T<ACE_Local_Mutex> >
#     pragma instantiate ACE_Lock_Adapter< ACE_Select_Reactor_Token_T<ACE_Local_Mutex> >
#     pragma instantiate ACE_Guard< ACE_Select_Reactor_Token_T<ACE_Local_Mutex> >
#   else
#     pragma instantiate ACE_Select_Reactor_Token_T<ACE_Token>
#     pragma instantiate ACE_Select_Reactor_T< ACE_Select_Reactor_Token_T<ACE_Token> >
#     pragma instantiate ACE_Lock_Adapter< ACE_Select_Reactor_Token_T<ACE_Token> >
#     pragma instantiate ACE_Guard< ACE_Select_Reactor_Token_T<ACE_Token> >
#   endif /* ACE_SELECT_REACTOR_HAS_DEADLOCK_DETECTION */
# else
#   pragma instantiate ACE_Select_Reactor_Token_T<ACE_Noop_Token>
#   pragma instantiate ACE_Select_Reactor_T< ACE_Select_Reactor_Token_T<ACE_Noop_Token> >
#   pragma instantiate ACE_Lock_Adapter< ACE_Select_Reactor_Token_T<ACE_Noop_Token> >
# endif /* ACE_MT_SAFE && ACE_MT_SAFE != 0 */
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
