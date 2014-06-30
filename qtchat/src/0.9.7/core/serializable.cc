#include "serializable.h"
#include <sys/param.h>	// htons() declarations
#include <netinet/in.h>	// htons() declaration on Linux RedHat

StrStream::StrStream() : Stream() {
	str=0;
	len=0;
	static_string=false;
	good = true;
}

StrStream::StrStream(char *ch, int _len) : Stream() {
	static_string = (_len > 0);	// dynamic string or static?
	if(static_string) {
		len = _len;
		str = ch;
	} else {	// dynamic string
		len = 1;
		str = new char[1];
		str[0] = '\0';
	}
	good = true;
}

StrStream::~StrStream() {
	if(!static_string && len > 0) {
		delete str;
	}
}

int StrStream::read(char *buf, size_t buflen) {
	size_t min_sz = (buflen < len ? buflen : len);
	for(size_t i=0; i<min_sz; i++) buf[i] = str[i];
	return min_sz;
}

int StrStream::write(const char *buf, size_t buflen) {
	if(static_string) {
		size_t min_sz = (buflen < len ? buflen : len);
		for(size_t i=0; i<min_sz; i++) str[i] = buf[i];
		str += min_sz;
		len -= min_sz;
		return min_sz;
	} else {
		if(buflen > len) {	// need more room
			delete str;
			str = new char[buflen];
			len = buflen;
		}
		for(size_t i=0; i<buflen; i++) str[i] = buf[i];
	}
	return buflen;
}

bool Serializable::isNetworkOrder() { // is native byte order == network byte order?
	return htons(0xff00) == 0xff00;
}

void Serializable::writeBytes(OutputStream &os, const void *data, size_t len) {
	const char *cp = reinterpret_cast<const char *&>(data);
	if(isNetworkOrder()) {
		os.write(cp, len);
	} else {
		for(size_t i=len-1; i>0; i--)
			os.write(cp+i, 1);
		if(len)
			os.write(cp, 1);
	}
}

void Serializable::readBytes(InputStream &is, void *data, size_t len) {
	char *cp = reinterpret_cast<char *&>(data);
	if(isNetworkOrder()) {
		is.read(cp, len);
	} else {
		for(size_t i=len-1; i>0; i--)
			is.read(cp+i, 1);
		if(len)
			is.read(cp, 1);
	}
}

void Serializable::writeObject(OutputStream &os, const char &x) {
	Serializable::writeBytes(os, &x, sizeof(x));
}

void Serializable::writeObject(OutputStream &os, const signed char &x) {
	Serializable::writeBytes(os, &x, sizeof(x));
}

void Serializable::writeObject(OutputStream &os, const short &x) {
	Serializable::writeBytes(os, &x, sizeof(x));
}

void Serializable::writeObject(OutputStream &os, const int &x) {
	Serializable::writeBytes(os, &x, sizeof(x));
}

void Serializable::writeObject(OutputStream &os, const long &x) {
	Serializable::writeBytes(os, &x, sizeof(x));
}

void Serializable::writeObject(OutputStream &os, const unsigned char &x) {
	Serializable::writeBytes(os, &x, sizeof(x));
}

void Serializable::writeObject(OutputStream &os, const unsigned short &x) {
	Serializable::writeBytes(os, &x, sizeof(x));
}

void Serializable::writeObject(OutputStream &os, const unsigned int &x) {
	Serializable::writeBytes(os, &x, sizeof(x));
}

void Serializable::writeObject(OutputStream &os, const unsigned long &x) {
	Serializable::writeBytes(os, &x, sizeof(x));
}

void Serializable::writeObject(OutputStream &os, const float &x) {
	Serializable::writeBytes(os, &x, sizeof(x));
}

void Serializable::writeObject(OutputStream &os, const double &x) {
	Serializable::writeBytes(os, &x, sizeof(x));
}

void Serializable::writeObject(OutputStream &os, const long double &x) {
	Serializable::writeBytes(os, &x, sizeof(x));
}

void Serializable::writeObject(OutputStream &os, const void *x, int length) {
	os.write(reinterpret_cast<const char *>(x), length);
}

void Serializable::readObject(InputStream &is, char &x) {
	Serializable::readBytes(is, &x, sizeof(x));
}

void Serializable::readObject(InputStream &is, signed char &x) {
	Serializable::readBytes(is, &x, sizeof(x));
}

void Serializable::readObject(InputStream &is, short &x) {
	Serializable::readBytes(is, &x, sizeof(x));
}

void Serializable::readObject(InputStream &is, int &x) {
	Serializable::readBytes(is, &x, sizeof(x));
}

void Serializable::readObject(InputStream &is, long &x) {
	Serializable::readBytes(is, &x, sizeof(x));
}

void Serializable::readObject(InputStream &is, unsigned char &x) {
	Serializable::readBytes(is, &x, sizeof(x));
}

void Serializable::readObject(InputStream &is, unsigned short &x) {
	Serializable::readBytes(is, &x, sizeof(x));
}

void Serializable::readObject(InputStream &is, unsigned int &x) {
	Serializable::readBytes(is, &x, sizeof(x));
}

void Serializable::readObject(InputStream &is, unsigned long &x) {
	Serializable::readBytes(is, &x, sizeof(x));
}

void Serializable::readObject(InputStream &is, float &x) {
	Serializable::readBytes(is, &x, sizeof(x));
}

void Serializable::readObject(InputStream &is, double &x) {
	Serializable::readBytes(is, &x, sizeof(x));
}

void Serializable::readObject(InputStream &is, long double &x) {
	Serializable::readBytes(is, &x, sizeof(x));
}

void Serializable::readObject(InputStream &is, void *x, int length) {
	is.read(reinterpret_cast<char *>(x), length);
}
