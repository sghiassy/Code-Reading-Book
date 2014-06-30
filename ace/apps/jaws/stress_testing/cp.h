// cp.h,v 1.3 1997/04/29 12:59:48 levine Exp

#include "util.h"
#include "stats.h"

class Client_Parameters {
public:
  Client_Parameters(int);
  URL *url;
  static Stats *stats;
  static int tcp_nodelay;
  static int sockbufsiz;
  int id;
};
