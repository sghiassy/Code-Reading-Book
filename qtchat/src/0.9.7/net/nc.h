#ifndef _NETCONNECTION_H
#define _NETCONNECTION_H

#include <iostream>
#include <sys/time.h>	// struct timeval;
#include <unistd.h>
#include "serializable.h"

class NetConnection : public virtual InputStream, public virtual OutputStream {
	public:
		NetConnection(const char *host, int port, double read_timeout=10.0);
		NetConnection(const int *hostip, int port, double read_timeout=10.0);
		virtual ~NetConnection();

		void close();
		virtual int read(char *buffer, size_t size);	// see read(2)
		virtual int write(const char *buffer, size_t size);	// see write(2)

		virtual operator bool() const {	// return validity of conn.
			return isValid();
		}
		virtual bool isValid() const {	// return validity of conn.
			return sock > 0 && isGood();	// also check underlying Stream
		}

		const char *getHost() const {
			return host;
		}
		int getPort() const {
			return port;
		}
		int getSocket() const {
			return sock;
		}
		struct timeval getTimeoutVal() const {
			return sock_timeout;
		}

		bool isDataAvailable() const;

// Mutators
		void setTimeoutVal(struct timeval tv) {
			sock_timeout = tv;
		}

	private:
		int sock;	// connection socket
		char *host;
		int port;
		struct timeval sock_timeout;
};

#endif	// _NETCONNECTION_H
