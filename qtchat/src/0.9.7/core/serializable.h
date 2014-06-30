#ifndef _SERIALIZABLE_H
#define _SERIALIZABLE_H

#include <string>
#include <sys/types.h>

class Stream {
	public:
		Stream() {
			good = false;
		}
		virtual bool isGood() const {	// get state of stream
			return good;
		}
		virtual void invalidate() {	// set stream state to bad
			good = false;
		}

	protected:
		bool good;			// state of stream
};

class InputStream : public virtual Stream {
	public:
		virtual int read(char *buf, size_t len)=0;			
};

class OutputStream : public virtual Stream {
	public:
		virtual int write(const char *buf, size_t len)=0;
};

class StrStream : public virtual InputStream, public virtual OutputStream {
	public:
		StrStream();
		StrStream(char *ch, int strlen=-1);
		virtual ~StrStream();

		virtual int read(char *buf, size_t buflen);
		virtual int write(const char *buf, size_t buflen);

	private:
		char *str;
		size_t len;
		bool static_string;
};

class Serializable {
	public:
		// Pure virtual methods
		virtual const char * getClassID() const = 0;
		virtual void writeObject(OutputStream &os) const = 0;
		virtual void readObject(InputStream &is) = 0;	// for non-virtual class de-serializing

		// Operators
		bool equalsClassID(const Serializable &s) const {
			return getClassID() && s.getClassID() && strcmp(getClassID(), s.getClassID())==0;
		}
		friend OutputStream &operator<<(OutputStream &os, const Serializable &object) {
			object.writeObject(os);
			return os;
		}
		friend InputStream &operator>>(InputStream &is, Serializable &object) {
			object.readObject(is);
			return is;
		}

		static void writeObject(OutputStream &os, const char &x);
		static void writeObject(OutputStream &os, const signed char &x);
		static void writeObject(OutputStream &os, const unsigned char &x);
		static void writeObject(OutputStream &os, const short int &x);
		static void writeObject(OutputStream &os, const unsigned short int &x);
		static void writeObject(OutputStream &os, const int &x);
		static void writeObject(OutputStream &os, const unsigned int &x);
		static void writeObject(OutputStream &os, const long &x);
		static void writeObject(OutputStream &os, const unsigned long &x);
		static void writeObject(OutputStream &os, const float &x);
		static void writeObject(OutputStream &os, const double &x);
		static void writeObject(OutputStream &os, const long double &x);
		static void writeObject(OutputStream &os, const void *x, int length);

		static void readObject(InputStream &is, char &x);
		static void readObject(InputStream &is, signed char &x);
		static void readObject(InputStream &is, unsigned char &x);
		static void readObject(InputStream &is, short &x);
		static void readObject(InputStream &is, int &x);
		static void readObject(InputStream &is, long &x);
		static void readObject(InputStream &is, unsigned short &x);
		static void readObject(InputStream &is, unsigned int &x);
		static void readObject(InputStream &is, unsigned long &x);
		static void readObject(InputStream &is, float &x);
		static void readObject(InputStream &is, double &x);
		static void readObject(InputStream &is, long double &x);
		static void readObject(InputStream &is, void *x, int length);

	protected:
		static inline bool isNetworkOrder();	// is native byte order == network byte order?
		static void writeBytes(OutputStream &os, const void *data, size_t len);
		static void readBytes(InputStream &is, void *data, size_t len);
};

#endif	// _SERIALIZABLE_H
