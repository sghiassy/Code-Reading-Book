// JAWS.h,v 1.1 2000/04/27 23:43:21 jxh Exp

#if (JAWS_NTRACE == 1)
# define JAWS_TRACE(X)
#else
# define JAWS_TRACE(X) ACE_Trace ____ (ACE_TEXT (X), \
                                       __LINE__, \
                                       ACE_TEXT (__FILE__))
#endif /* JAWS_NTRACE */

#ifndef JAWS_JAWS_H
#define JAWS_JAWS_H

#endif /* JAWS_JAWS_H */
