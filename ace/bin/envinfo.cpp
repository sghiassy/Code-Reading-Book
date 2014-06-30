// envinfo.cpp,v 4.3 2001/05/11 18:45:10 schmidt Exp

#include "ace/ACE.h"

ACE_RCSID(bin, envinfo, "envinfo.cpp,v 4.3 2001/05/11 18:45:10 schmidt Exp")

int
main (int, ACE_TCHAR *[])
{
  cerr << "ACE: "
       << ACE::major_version() << "."
       << ACE::minor_version() << "."
       << ACE::beta_version() << "\n";

  ACE_utsname uname;
  ACE_OS::uname(&uname);
  cerr << "OS: "
       << uname.sysname << " "
       << uname.release << "\n";

  cerr << "Compiler: "
       << ACE::compiler_name() << " "
       << ACE::compiler_major_version() << "."
       << ACE::compiler_minor_version() << "."
       << ACE::compiler_beta_version() << "\n";

  return 0;
}

