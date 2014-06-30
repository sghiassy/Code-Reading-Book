/*************************************************
* Filter Base Header File                        *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_FILTER_BASE_H__
#define OPENCL_FILTER_BASE_H__

#include <iosfwd>
#include <cstdio>
#include <opencl/opencl.h>

namespace OpenCL {

/*************************************************
* Filter Base Class                              *
*************************************************/
class Filter
   {
   public:
      struct Invalid_Port_Number : public Invalid_Argument
         { Invalid_Port_Number() :
              Invalid_Argument("Filter: Invalid port number was used") {} };
      virtual void write(const byte[], u32bit) = 0;
      virtual void final() {}
      virtual void close();
      virtual ~Filter() {}
   protected:
      void send(const byte[], u32bit);
      void send(byte input) { send(&input, 1); }
      void attach(Filter*);
      void set_port(u32bit) throw(Invalid_Port_Number);
      u32bit total_ports()  const { return port_count; }
      u32bit current_port() const { return port_num; }
      Filter(u32bit = 1);
   private:
      friend class Pipe;
      friend class Fork;
      friend void destroy(Filter*&);
      Filter(const Filter&) : port_count(0) {}
      void operator=(const Filter&) {}
      void add(const class SecureQueue&, Filter*);
      Filter* get_next() const { if(next) return next[port_num]; return 0; }
      const u32bit port_count;
      u32bit port_num;
      Filter** next;
   };

/*************************************************
* Destroy this Filter and it's children          *
*************************************************/
void destroy(Filter*&);

/*************************************************
* Pipe                                           *
*************************************************/
class Pipe
   {
   public:
      void write(const byte input[], u32bit len) { pipe->write(input, len); }
      void write(byte input) { write(&input, 1); }
      void write(const std::string&);

      u32bit read(byte[], u32bit);
      u32bit read(byte& output) { return read(&output, 1); }
      SecureVector<byte> read_all();
      std::string read_all_as_string();

      void finish(const byte in[], u32bit len) { write(in, len); close(); }

      u32bit remaining() const;
      void close() { pipe->close(); }
      void attach(Filter* filter) { pipe->attach(filter); }
      void reset() { destroy(pipe); init(); }

      Pipe(Filter* f1 = 0, Filter* f2 = 0, Filter* f3 = 0, Filter* f4 = 0)
         { init(); attach(f1); attach(f2); attach(f3); attach(f4); }
      Pipe(Filter*[], u32bit);
      ~Pipe() { destroy(pipe); }
   private:
      struct NullFilter : public Filter
         { void write(const byte in[], u32bit len) { send(in, len); } };
      Pipe(const Pipe&) {}
      void operator=(const Pipe&) {}
      void init();
      class SecureQueue* end() const;
      Filter* pipe;
   };

/*************************************************
* Input and Ouput Operators for Pipe             *
*************************************************/
std::ostream& operator<<(std::ostream&, Pipe&);
std::istream& operator>>(std::istream&, Pipe&);

std::FILE* operator<<(std::FILE*, Pipe&);
std::FILE* operator>>(std::FILE*, Pipe&);

/*************************************************
* Fork                                           *
*************************************************/
struct Fork : public Filter
   {
   void write(const byte input[], u32bit length) { send(input, length); }
   void set_port(u32bit n) throw(Invalid_Port_Number) { Filter::set_port(n); }
   u32bit total_ports()  const { return Filter::total_ports(); }
   u32bit current_port() const { return Filter::current_port(); }

   Fork(Filter* f1, Filter* f2, Filter* f3 = 0, Filter* f4 = 0) : Filter(4)
      { next[0] = f1; next[1] = f2; next[2] = f3; next[3] = f4; }
   Fork(Filter*[], u32bit);
   };

/*************************************************
* Chain                                          *
*************************************************/
struct Chain : public Filter
   {
   void write(const byte input[], u32bit length) { send(input, length); }

   Chain(Filter* f1 = 0, Filter* f2 = 0, Filter* f3 = 0, Filter* f4 = 0)
      { attach(f1); attach(f2); attach(f3); attach(f4); }
   Chain(Filter*[], u32bit);
   };

}

#endif
