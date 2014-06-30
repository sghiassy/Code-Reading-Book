#include <iostream>
#include <streambuf.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <math.h>	// modf();
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>	// select();
#include "nc.h"

#define SOCK_READ_TIMEOUT_DFL 10.0	// seconds

NetConnection::NetConnection(const char *_host, int _port, double read_timeout) :
	Stream() {
	struct sockaddr_in server;
	struct hostent *hp;
	char buf[256];

	// pre-init data members
	host = 0;
	port = -1;
	sock = -1;
	if(read_timeout < 0) {
		read_timeout = SOCK_READ_TIMEOUT_DFL;
	}
	double i,f;	// integral/fractional parts of read_timeout
	f = modf(read_timeout, &i);
	sock_timeout.tv_sec = (long)i;	// seconds
	sock_timeout.tv_usec = (long)(f*1.0e6);

	// check arguments
	if(!_host || _port <= 0) {
		cerr << "NetConnection(): Invalid argument(s)." << endl;
		return;
	}

	// set arguments
	host = strdup(_host);
	port = _port;

	// open socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		cerr << "NetConnection(): Error opening control socket." << endl;
		perror("NetConnection()");
		return;
	}

	// get IP from hostname
	snprintf(buf, 256, "Looking up %s... ", host);
	cerr << buf << flush;
	hp = gethostbyname(host);
	if (hp == (struct hostent *) 0) {
		cerr << endl << host << ": Unknown host." << endl;
		close();
		return;
	}
	cerr << "done." << endl;

	// connect to IP:port
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	memcpy((char *) &server.sin_addr, (char *) hp->h_addr, hp->h_length);
	snprintf(buf, 256, "Connecting to %s:%d... ", host, port);
	cerr << buf << flush;
	if (::connect(sock, (struct sockaddr *) &server, sizeof server) == -1) {
		cerr << endl << "NetConnection(): Error connecting socket." << endl;
		perror("NetConnection()");
		close();
		return;
	}
	good = true;
	cerr << "done." << endl;
}

NetConnection::NetConnection(const int *hostip, int _port, double read_timeout) : Stream() {
	struct sockaddr_in server;
	struct hostent *hp;
	char buf[256];

	// pre-init data members
	host = 0;
	port = -1;
	sock = -1;
	if(read_timeout < 0) {
		read_timeout = SOCK_READ_TIMEOUT_DFL;
	}
	double i,f;	// integral/fractional parts of read_timeout
	f = modf(read_timeout, &i);
	sock_timeout.tv_sec = (long)i;	// seconds
	sock_timeout.tv_usec = (long)(f*1.0e6);

	// check arguments
	for(int i=0; i<4; i++) {
		if(hostip[i] < 0 || hostip[i] > 255) {
			cerr << "NetConnection(): Invalid argument(s)." << endl;
			return;
		}
	}
	if(_port <= 0) {
		cerr << "NetConnection(): Invalid argument(s)." << endl;
		return;
	}

	// set arguments
	host = (char *)malloc(16);
	snprintf(host, 16, "%d.%d.%d.%d", hostip[0], hostip[1], hostip[2], hostip[3]);
	port = _port;

	// open socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		cerr << "NetConnection(): Error opening control socket." << endl;
		perror("NetConnection()");
		return;
	}

	// get IP from hostname
	snprintf(buf, 256, "Looking up %s... ", host);
	cerr << buf << flush;
	hp = gethostbyaddr(host, strlen(host), AF_INET);
	if (hp == (struct hostent *) 0) {
		cerr << endl << host << ": Unknown host." << endl;
		free(host);
		host = 0;
		close();
		return;
	}
	cerr << "done." << endl;

	// connect to IP:port
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	memcpy((char *) &server.sin_addr, (char *) hp->h_addr, hp->h_length);
	snprintf(buf, 256, "Connecting to %s:%d... ", host, port);
	cerr << buf << flush;
	if (::connect(sock, (struct sockaddr *) &server, sizeof server) == -1) {
		cerr << endl << "NetConnection(): Error connecting socket." << endl;
		perror("NetConnection()");
		close();
		return;
	}
	good = true;
	cerr << "done." << endl;

}

NetConnection::~NetConnection() {
	if(host) free(host);
	close();
}

void NetConnection::close() {
	if(isValid()) {
		char buf[256];
		snprintf(buf, 256, "Disconnecting from %s:%d... ", host, port);
		cerr << buf << flush;
		::close(sock);
		cerr << "done." << endl;
	}
	sock = -1;
	port = -1;
	invalidate();
}

int NetConnection::read(char *buffer, size_t size) {
	if(isValid()) {
		fd_set readfds;
		FD_ZERO(&readfds);	// init descriptor set
		FD_SET(sock, &readfds);
		if(select(sock+1, &readfds, 0, 0, &sock_timeout) == 1) {
			return recv(sock, buffer, size, 0);
		}
	}
	return -1;
}

int NetConnection::write(const char *buffer, size_t size) {
	if(isValid())
		return ::send(sock, buffer, size, 0);
	else
		return -1;
}

bool NetConnection::isDataAvailable() const {
		fd_set readfds;
		struct timeval tv;
		FD_ZERO(&readfds);	// init descriptor set
		FD_SET(sock, &readfds);
		tv.tv_sec=0;	// set to 0 timeout for polling
		tv.tv_usec=100000;

		return select(sock+1, &readfds, 0, 0, &tv);
}
