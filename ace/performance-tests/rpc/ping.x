/*
 * ping.x,v 1.1 1999/08/23 21:54:22 coryan Exp
 */

/*
 * A small program to test RPC round-trip delays.
 */
program PINGPROG {
  version PINGVERS {
    int PING (int) = 1;
  } = 1;
} = 0x20000001;
