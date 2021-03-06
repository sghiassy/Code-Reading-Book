// INET_Addr.cpp,v 4.62 2001/08/24 14:11:03 shuston Exp

// Defines the Internet domain address family address format.

#include "ace/INET_Addr.h"
#include "ace/Log_Msg.h"

#if !defined (__ACE_INLINE__)
#include "ace/INET_Addr.i"
#endif /* __ACE_INLINE__ */

ACE_RCSID(ace, INET_Addr, "INET_Addr.cpp,v 4.62 2001/08/24 14:11:03 shuston Exp")

ACE_ALLOC_HOOK_DEFINE(ACE_INET_Addr)

// Transform the current address into string format.

int
ACE_INET_Addr::addr_to_string (ACE_TCHAR s[],
                               size_t size,
                               int ipaddr_format) const
{
  ACE_TRACE ("ACE_INET_Addr::addr_to_string");

  size_t total_len = (ipaddr_format == 0 ?
                      ACE_OS::strlen (this->get_host_name ()) :
                      ACE_OS::strlen (this->get_host_addr ()))
    + ACE_OS::strlen ("65536") // Assume the max port number.
    + sizeof (':')
    + sizeof ('\0'); // For trailing '\0'.

  if (size < total_len)
    return -1;
  else
    {
      ACE_OS::sprintf (s,
                       ACE_LIB_TEXT ("%s:%d"),
                       ACE_TEXT_CHAR_TO_TCHAR (ipaddr_format == 0
                                               ? this->get_host_name ()
                                               : this->get_host_addr ()),
                       this->get_port_number ());
      return 0;
    }
}

void
ACE_INET_Addr::dump (void) const
{
  ACE_TRACE ("ACE_INET_Addr::dump");

  ACE_DEBUG ((LM_DEBUG, ACE_BEGIN_DUMP, this));

  ACE_TCHAR s[ACE_MAX_FULLY_QUALIFIED_NAME_LEN + 16];
  ACE_OS::sprintf (s, ACE_LIB_TEXT ("%s:%d"),
                   this->get_host_addr (),
                   this->get_port_number ());
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("%s"), s));
  ACE_DEBUG ((LM_DEBUG, ACE_END_DUMP));
}

// Compare two addresses for inequality.

int
ACE_INET_Addr::operator != (const ACE_INET_Addr &sap) const
{
  ACE_TRACE ("ACE_INET_Addr::operator !=");
  return !((*this) == sap);
}

// Compare two addresses for equality.

int
ACE_INET_Addr::operator == (const ACE_INET_Addr &sap) const
{
  ACE_TRACE ("ACE_INET_Addr::operator ==");

  return this->inet_addr_.sin_port == sap.inet_addr_.sin_port
      && ACE_OS::memcmp ((void *) &this->inet_addr_.sin_addr,
                         (void *) &sap.inet_addr_.sin_addr,
                         sizeof (this->inet_addr_.sin_addr)) == 0;
}

ACE_INET_Addr::ACE_INET_Addr (void)
  : ACE_Addr (AF_INET, sizeof this->inet_addr_)
{
  // ACE_TRACE ("ACE_INET_Addr::ACE_INET_Addr");
  (void) ACE_OS::memset ((void *) &this->inet_addr_,
                         0,
                         sizeof this->inet_addr_);
  this->inet_addr_.sin_family = AF_INET;
}

int
ACE_INET_Addr::set (const ACE_INET_Addr &sa)
{
  ACE_TRACE ("ACE_INET_Addr::set");

  this->ACE_Addr::base_set (sa.get_type (),
                            sa.get_size ());

  if (sa.get_type () == AF_ANY)
    // Ugh, this is really a base class, so don't copy it.
    (void) ACE_OS::memset ((void *) &this->inet_addr_,
                           0,
                           sizeof this->inet_addr_);
  else
    // It's ok to make the copy.
    (void) ACE_OS::memcpy ((void *) &this->inet_addr_,
                           (void *) &sa.inet_addr_,
                           sizeof this->inet_addr_);
  return 0;
}

// Transform the string into the current addressing format.

int
ACE_INET_Addr::string_to_addr (const char s[])
{
  ACE_TRACE ("ACE_INET_Addr::string_to_addr");
  int result;
  char *t;

  // Need to make a duplicate since we'll be overwriting the string.
  ACE_ALLOCATOR_RETURN (t,
                        ACE_OS::strdup (s),
                        -1);
  char *ip_addr = ACE_OS::strchr (t, ':');

  if (ip_addr == 0) // Assume it's a port number.
    {
      if (ACE_OS::strspn (t, "1234567890")
          == ACE_OS::strlen (t))
        { // port number
          u_short port = (u_short) ACE_OS::atoi (t);
          result = this->set (port,
                              ACE_UINT32 (INADDR_ANY));
        }
      else // port name
        result = this->set (t,
                            ACE_UINT32 (INADDR_ANY));
    }
  else
    {
      *ip_addr = '\0'; ++ip_addr; // skip over ':'

      if (ACE_OS::strspn (ip_addr, "1234567890") ==
          ACE_OS::strlen (ip_addr))
        {
          u_short port =
            (u_short) ACE_OS::atoi (ip_addr);
          result = this->set (port, t);
        }
      else
        result = this->set (ip_addr, t);
    }

  ACE_OS::free (ACE_MALLOC_T (t));
  return result;
}

int
ACE_INET_Addr::set (const char address[])
{
  ACE_TRACE ("ACE_INET_Addr::set");
  return this->string_to_addr (address);
}

ACE_INET_Addr::ACE_INET_Addr (const char address[])
  : ACE_Addr (AF_INET, sizeof this->inet_addr_)
{
  ACE_TRACE ("ACE_INET_Addr::ACE_INET_Addr");
  this->set (address);
}

#if defined (ACE_HAS_WCHAR)
ACE_INET_Addr::ACE_INET_Addr (const wchar_t address[])
  : ACE_Addr (AF_INET, sizeof this->inet_addr_)
{
  ACE_TRACE ("ACE_INET_Addr::ACE_INET_Addr");
  this->set (address);
}

#endif /* ACE_HAS_WCHAR */

// Copy constructor.

ACE_INET_Addr::ACE_INET_Addr (const ACE_INET_Addr &sa)
  : ACE_Addr (AF_INET, sizeof this->inet_addr_)
{
  ACE_TRACE ("ACE_INET_Addr::ACE_INET_Addr");
  this->set (sa);
}

// Initializes a ACE_INET_Addr from a PORT_NUMBER and an Internet
// address.

int
ACE_INET_Addr::set (u_short port_number,
                    ACE_UINT32 inet_address,
                    int encode)
{
  ACE_TRACE ("ACE_INET_Addr::set");
  this->ACE_Addr::base_set (AF_INET, sizeof this->inet_addr_);
  (void) ACE_OS::memset ((void *) &this->inet_addr_,
                         0, sizeof this->inet_addr_);
  this->inet_addr_.sin_family = AF_INET;
#if defined (ACE_HAS_SIN_LEN)
  this->inet_addr_.sin_len = sizeof this->inet_addr_;
#endif /* ACE_HAS_SIN_LEN */

  if (encode)
    {
      inet_address = htonl (inet_address);
      this->inet_addr_.sin_port = htons (port_number);
    }
  else
    this->inet_addr_.sin_port = port_number;

  (void) ACE_OS::memcpy ((void *) &this->inet_addr_.sin_addr,
                         (void *) &inet_address,
                         sizeof this->inet_addr_.sin_addr);
  return 0;
}

// Initializes a ACE_INET_Addr from a PORT_NUMBER and the remote
// HOST_NAME.

int
ACE_INET_Addr::set (u_short port_number,
                    const char host_name[],
                    int encode)
{
  ACE_TRACE ("ACE_INET_Addr::set");
  ACE_UINT32 addr;

  this->ACE_Addr::base_set (AF_INET, sizeof this->inet_addr_);
  (void) ACE_OS::memset ((void *) &this->inet_addr_, 0, sizeof
                         this->inet_addr_);
  this->inet_addr_.sin_family = AF_INET;

  // Yow, someone gave us a NULL host_name!
  if (host_name == 0)
    {
      errno = EINVAL;
      return -1;
    }
  else if (ACE_OS::inet_aton (host_name,
                              (struct in_addr *) &addr) == 1)
    return this->set (port_number,
                      encode ? ntohl (addr) : addr,
                      encode);
  else
    {
#if defined (VXWORKS) || defined (CHORUS)
      hostent *hp = ACE_OS::gethostbyname (host_name);
#else
      hostent hentry;
      ACE_HOSTENT_DATA buf;
      int error;

      hostent *hp = ACE_OS::gethostbyname_r (host_name, &hentry,
                                             buf, &error);
#endif /* VXWORKS */

      if (hp == 0)
        {
          errno = EINVAL;
          return -1;
        }
      else
        {
          (void) ACE_OS::memcpy ((void *) &addr,
                                 hp->h_addr,
                                 hp->h_length);
          return this->set (port_number,
                            encode ? ntohl (addr) : addr,
                            encode);
        }
    }
}

// Initializes a ACE_INET_Addr from a <port_name> and the remote
// <host_name>.

int
ACE_INET_Addr::set (const char port_name[],
                    const char host_name[],
                    const char protocol[])
{
  ACE_TRACE ("ACE_INET_Addr::set");

#if defined (VXWORKS) || defined (CHORUS) || defined (ACE_LACKS_GETSERVBYNAME)
  ACE_UNUSED_ARG (port_name);
  ACE_UNUSED_ARG (host_name);
  ACE_UNUSED_ARG (protocol);
  ACE_NOTSUP_RETURN (-1);
#else
  servent sentry;
  ACE_SERVENT_DATA buf;

  servent *sp = ACE_OS::getservbyname_r (port_name,
                                         protocol,
                                         &sentry,
                                         buf);
  if (sp == 0)
    return -1;
  else
    return this->set (sp->s_port, host_name, 0);
#endif /* VXWORKS */
}

// Initializes a ACE_INET_Addr from a <port_name> and an Internet
// address.

int
ACE_INET_Addr::set (const char port_name[],
                    ACE_UINT32 inet_address,
                    const char protocol[])
{
  ACE_TRACE ("ACE_INET_Addr::set");

#if defined (VXWORKS) || defined (CHORUS) || defined (ACE_LACKS_GETSERVBYNAME)
  ACE_UNUSED_ARG (port_name);
  ACE_UNUSED_ARG (inet_address);
  ACE_UNUSED_ARG (protocol);
  ACE_NOTSUP_RETURN (-1);
#else
  servent sentry;
  ACE_SERVENT_DATA buf;

  servent *sp = ACE_OS::getservbyname_r (port_name,
                                         protocol,
                                         &sentry,
                                         buf);
  if (sp == 0)
    return -1;
  else
    return this->set (sp->s_port, inet_address, 0);
#endif /* VXWORKS */
}

// Creates a ACE_INET_Addr from a PORT_NUMBER and the remote
// HOST_NAME.


ACE_INET_Addr::ACE_INET_Addr (u_short port_number,
                              const char host_name[])
{
  ACE_TRACE ("ACE_INET_Addr::ACE_INET_Addr");
  if (this->set (port_number,
                 host_name) == -1)
#if defined (ACE_HAS_BROKEN_CONDITIONAL_STRING_CASTS)
    ACE_ERROR ((LM_ERROR,
                (char *)"ACE_INET_Addr::ACE_INET_Addr: %p\n",
                (((char *) host_name == 0) ?
                 ((char *) "<unknown>") :
                 ((char *) (host_name)))));
#else /* ! defined (ACE_HAS_BROKEN_CONDITIONAL_STRING_CASTS) */
    ACE_ERROR ((LM_ERROR,
                ACE_LIB_TEXT ("ACE_INET_Addr::ACE_INET_Addr: %p\n"),
                ACE_TEXT_CHAR_TO_TCHAR ((host_name == 0) ?
                                        "<unknown>" : host_name)));
#endif /* ! defined (ACE_HAS_BROKEN_CONDITIONAL_STRING_CASTS) */
}

#if defined (ACE_HAS_WCHAR)
ACE_INET_Addr::ACE_INET_Addr (u_short port_number,
                              const wchar_t host_name[])
{
  ACE_TRACE ("ACE_INET_Addr::ACE_INET_Addr");
  if (this->set (port_number,
                 host_name) == -1)
#if defined (ACE_HAS_BROKEN_CONDITIONAL_STRING_CASTS)
    ACE_ERROR ((LM_ERROR,
                (char *)"ACE_INET_Addr::ACE_INET_Addr: %p\n",
                (((char *) host_name == 0) ?
                 ((char *) "<unknown>") :
                 ((char *) (host_name)))));
#else /* ! defined (ACE_HAS_BROKEN_CONDITIONAL_STRING_CASTS) */
    ACE_ERROR ((LM_ERROR,
                ACE_LIB_TEXT ("ACE_INET_Addr::ACE_INET_Addr: %p\n"),
                ACE_TEXT_WCHAR_TO_TCHAR ((host_name == 0) ?
                                         ACE_TEXT_WIDE ("<unknown>") :
                                         host_name)));
#endif /* ! defined (ACE_HAS_BROKEN_CONDITIONAL_STRING_CASTS) */
}
#endif /* ACE_HAS_WCHAR */

// Creates a ACE_INET_Addr from a sockaddr_in structure.

int
ACE_INET_Addr::set (const sockaddr_in *addr, int len)
{
  ACE_TRACE ("ACE_INET_Addr::set");
  this->ACE_Addr::base_set (AF_INET, len);
  ACE_OS::memcpy ((void *) &this->inet_addr_,
                  (void *) addr, len);
  this->inet_addr_.sin_family = AF_INET;
  return 0;
}

// Set a pointer to the address.
void
ACE_INET_Addr::set_addr (void *addr, int len)
{
  ACE_TRACE ("ACE_INET_Addr::set_addr");

  this->ACE_Addr::base_set (AF_INET, len);
  ACE_OS::memcpy ((void *) &this->inet_addr_,
                  (void *) addr, len);
  this->inet_addr_.sin_family = AF_INET;
}

// Creates a ACE_INET_Addr from a sockaddr_in structure.


ACE_INET_Addr::ACE_INET_Addr (const sockaddr_in *addr, int len)
{
  ACE_TRACE ("ACE_INET_Addr::ACE_INET_Addr");
  this->set (addr, len);
}

// Creates a ACE_INET_Addr from a PORT_NUMBER and an Internet address.


ACE_INET_Addr::ACE_INET_Addr (u_short port_number,
                              ACE_UINT32 inet_address)
{
  ACE_TRACE ("ACE_INET_Addr::ACE_INET_Addr");
  if (this->set (port_number, inet_address) == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_LIB_TEXT ("%p\n"),
                ACE_LIB_TEXT ("ACE_INET_Addr::ACE_INET_Addr")));
}

// Creates a ACE_INET_Addr from a PORT_NAME and the remote
// HOST_NAME.

ACE_INET_Addr::ACE_INET_Addr (const char port_name[],
                              const char host_name[],
                              const char protocol[])
{
  ACE_TRACE ("ACE_INET_Addr::ACE_INET_Addr");
  if (this->set (port_name,
                 host_name,
                 protocol) == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_LIB_TEXT ("ACE_INET_Addr::ACE_INET_Addr")));
}

#if defined (ACE_HAS_WCHAR)
ACE_INET_Addr::ACE_INET_Addr (const wchar_t port_name[],
                              const wchar_t host_name[],
                              const wchar_t protocol[])
{
  ACE_TRACE ("ACE_INET_Addr::ACE_INET_Addr");
  if (this->set (port_name,
                 host_name,
                 protocol) == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_LIB_TEXT ("ACE_INET_Addr::ACE_INET_Addr")));
}
#endif /* ACE_HAS_WCHAR */

// Creates a ACE_INET_Addr from a PORT_NAME and an Internet address.


ACE_INET_Addr::ACE_INET_Addr (const char port_name[],
                              ACE_UINT32 inet_address,
                              const char protocol[])
{
  ACE_TRACE ("ACE_INET_Addr::ACE_INET_Addr");
  if (this->set (port_name,
                 inet_address,
                 protocol) == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_LIB_TEXT ("ACE_INET_Addr::ACE_INET_Addr")));
}

#if defined (ACE_HAS_WCHAR)
ACE_INET_Addr::ACE_INET_Addr (const wchar_t port_name[],
                              ACE_UINT32 inet_address,
                              const wchar_t protocol[])
{
  ACE_TRACE ("ACE_INET_Addr::ACE_INET_Addr");
  if (this->set (port_name,
                 inet_address,
                 protocol) == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_LIB_TEXT ("ACE_INET_Addr::ACE_INET_Addr")));
}
#endif /* ACE_HAS_WCHAR */

int
ACE_INET_Addr::get_host_name (char hostname[],
                              size_t len) const
{
  ACE_TRACE ("ACE_INET_Addr::get_host_name");

  int result;
  if (len > 1)
    {
      result = get_host_name_i(hostname,len);
      if (result < 0)
        {
          if (result == -2)
            {
              result = -1;
              // We know that hostname is nul-terminated
            }
          else
            {
              //result == -1;
              // This could be worse than hostname[len -1] = '\0'?
              hostname[0] = '\0';   
            }
        }
    }
  else
    {
      if (len == 1)
        {
          hostname[0] = '\0';
        }
      result = -1;
    }
  return result;
}

#if defined (ACE_HAS_WCHAR)
int
ACE_INET_Addr::get_host_name (wchar_t hostname[],
                              size_t len) const
{
  ACE_TRACE ("ACE_INET_Addr::get_host_name");

  char char_hostname [MAXHOSTNAMELEN + 1];

  // We have a build in limitation of MAXHOSTNAMELEN
  if (len > MAXHOSTNAMELEN + 1)
    len = MAXHOSTNAMELEN + 1;

  // Call the char version
  int result = this->get_host_name (char_hostname, len);

  // And copy it over, if successful
  if (result == 0)
    ACE_OS_String::strcpy (hostname,
                           ACE_Ascii_To_Wide (char_hostname).wchar_rep ());

  return result;
}
#endif /* ACE_HAS_WCHAR */

// Return the character representation of the hostname.

const char *
ACE_INET_Addr::get_host_name (void) const
{
  ACE_TRACE ("ACE_INET_Addr::get_host_name");

  static char name[MAXHOSTNAMELEN + 1];
  if (this->get_host_name (name, MAXHOSTNAMELEN + 1) == -1)
    ACE_OS::strcpy (name, "<unknown>");
  return name;
}

void
ACE_INET_Addr::set_port_number (u_short port_number,
                                int encode)
{
  ACE_TRACE ("ACE_INET_Addr::set_port_number");

  if (encode)
    port_number = htons (port_number);

  this->inet_addr_.sin_port = port_number;
}

// returns -2 when the hostname is truncated
int
ACE_INET_Addr::get_host_name_i (char hostname[], size_t len) const
{
  ACE_TRACE ("ACE_INET_Addr::get_host_name_i");

  if (this->inet_addr_.sin_addr.s_addr == INADDR_ANY)
    {
      if (ACE_OS::hostname (hostname, len) == -1)
        return -1;
      else
        return 0;
    }
  else
    {
#if defined (VXWORKS)
      ACE_UNUSED_ARG (len);
      int error =
        ::hostGetByAddr ((int) this->inet_addr_.sin_addr.s_addr,
                         hostname);
      if (error == OK)
        return 0;
      else
        {
          errno = error;
          return -1;
        }
#else
#  if !defined(_UNICOS)
      int a_len = sizeof this->inet_addr_.sin_addr.s_addr;
#  else /* _UNICOS */
      int a_len = sizeof this->inet_addr_.sin_addr;
#  endif /* ! _UNICOS */
      int error = 0;

#if defined (CHORUS) || (defined (DIGITAL_UNIX) && defined (__GNUC__))
      hostent *hp = ACE_OS::gethostbyaddr ((char *)
&this->inet_addr_.sin_addr,
                                           a_len,
                                           this->addr_type_);
      if (hp == 0)
        error = errno;  // So that the errno gets propagated back; it is
                        // loaded from error below.
#else
      hostent hentry;
      ACE_HOSTENT_DATA buf;
      hostent *hp =
        ACE_OS::gethostbyaddr_r ((char *)&this->inet_addr_.sin_addr,
                                 a_len,
                                 this->addr_type_,
                                 &hentry,
                                 buf,
                                 &error);
#endif /* CHORUS */

      if (hp == 0)
        {
          errno = error;
          return -1;
        }

      if (hp->h_name == 0)
        return -1;

      if (ACE_OS::strlen (hp->h_name) >= len)
        {        
          // We know the length, so use memcpy
          if (len > 0)
            {
              ACE_OS::memcpy(hostname,hp->h_name,len - 1);
              hostname[len-1]= '\0';
            }
          errno = ENOSPC;
          return -2;  // -2 Means that we have a good string
          // Using errno looks ok, but ENOSPC could be set on 
          // other places.
        }

      ACE_OS::strcpy (hostname, hp->h_name);
      return 0;
#endif /* VXWORKS */
    }
}
