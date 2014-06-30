/*************************************************
* Exceptions Header File                         *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_EXCEPTION_H__
#define OPENCL_EXCEPTION_H__

#include <exception>
#include <string>
#include <opencl/config.h>

namespace OpenCL {

/*************************************************
* Exception Base Class                           *
*************************************************/
class Exception : public std::exception
   {
   public:
      const char* what() const throw() { return msg.c_str(); }
      Exception(const std::string& m = "Unknown Error") { set_msg(m); }
      virtual ~Exception() throw() {}
   protected:
      void set_msg(const std::string& m) { msg = "OpenCL: " + m; }
   private:
      std::string msg;
   };

/*************************************************
* Invalid_Argument Exception                     *
*************************************************/
struct Invalid_Argument : public Exception
   {
   Invalid_Argument(const std::string& str = "") : Exception(str) {}
   };

/*************************************************
* Invalid_KeyLength Exception                    *
*************************************************/
struct InvalidKeyLength : public Invalid_Argument
   {
   InvalidKeyLength(const std::string&, u32bit);
   };

/*************************************************
* Invalid_BlockSize Exception                    *
*************************************************/
struct Invalid_BlockSize : public Invalid_Argument
   {
   Invalid_BlockSize(const std::string&, const std::string&);
   };

/*************************************************
* Invalid_IVLength Exception                     *
*************************************************/
struct Invalid_IVLength : public Invalid_Argument
   {
   Invalid_IVLength(const std::string&, u32bit);
   };

/*************************************************
* Format_Error Exception                         *
*************************************************/
struct Format_Error : public Exception
   {
   Format_Error(const std::string& str = "") : Exception(str) {}
   };

/*************************************************
* Decoding_Error Exception                       *
*************************************************/
struct Decoding_Error : public Format_Error
   {
   Decoding_Error(const std::string& name)
      : Format_Error("A decoding error occcured in " + name) {}
   };

/*************************************************
* Stream_IO_Error Exception                      *
*************************************************/
struct Stream_IO_Error : public Exception
   {
   Stream_IO_Error(const std::string& error)
      : Exception("I/O error: " + error) {}
   };

}

#endif
