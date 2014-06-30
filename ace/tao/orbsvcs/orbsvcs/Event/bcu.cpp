// BCU.cpp,v 1.2 1998/08/03 18:58:59 gonzo Exp

#include "ace/ACE.h"
#include "BCU.h"

ACE_RCSID(Event, BCU, "BCU.cpp,v 1.2 1998/08/03 18:58:59 gonzo Exp")

u_long
ACE_BCU (u_long n)
{
  const u_long ACE_BCU_PRIME_NUMBER = 9619;

  u_long retval = 0;

  while (n-- > 0)
    retval = ACE::is_prime (ACE_BCU_PRIME_NUMBER, 2, ACE_BCU_PRIME_NUMBER / 2);

  return retval;
}


u_long
ACE_BCU (u_long number,
         u_long n)
{
  u_long retval = 0;

  while (n-- > 0)
    retval = ACE::is_prime (number, 2, number);

  return retval;
}
