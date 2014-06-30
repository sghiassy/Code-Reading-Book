/*************************************************
* Pipe I/O Source File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <cstdio>
#include <iostream>
#include <opencl/filtbase.h>

namespace OpenCL {

/*************************************************
* Write data from a pipe into an ostream         *
*************************************************/
std::ostream& operator<<(std::ostream& stream, Pipe& pipe)
   {
   static const u32bit BUFFERSIZE = DEFAULT_BUFFERSIZE;
   SecureBuffer<byte, BUFFERSIZE> buffer;
   while(stream.good() && pipe.remaining())
      {
      u32bit got = pipe.read(buffer, BUFFERSIZE);
      stream.write((char*)buffer.ptr(), got);
      }
   if(!stream.good())
      throw Stream_IO_Error("Pipe output operator (iostream) has failed");
   return stream;
   }

/*************************************************
* Write data from a pipe into a stdio FILE       *
*************************************************/
std::FILE* operator<<(std::FILE* stream, Pipe& pipe)
   {
   static const u32bit BUFFERSIZE = DEFAULT_BUFFERSIZE;
   SecureBuffer<byte, BUFFERSIZE> buffer;
   while(!ferror(stream) && pipe.remaining())
      {
      u32bit got = pipe.read(buffer, BUFFERSIZE);
      fwrite(buffer, 1, got, stream);
      }
   if(ferror(stream))
      throw Stream_IO_Error("Pipe output operator (stdio) has failed");
   return stream;
   }

/*************************************************
* Read data from an istream into a pipe          *
*************************************************/
std::istream& operator>>(std::istream& stream, Pipe& pipe)
   {
   static const u32bit BUFFERSIZE = DEFAULT_BUFFERSIZE;
   SecureBuffer<byte, BUFFERSIZE> buffer;
   while(stream.good())
      {
      stream.read((char*)buffer.ptr(), BUFFERSIZE);
      pipe.write(buffer, stream.gcount());
      }
   if(stream.bad() || (stream.fail() && !stream.eof()))
      throw Stream_IO_Error("Pipe input operator (iostream) has failed");
   return stream;
   }

/*************************************************
* Read data from a stdio FILE into a pipe        *
*************************************************/
std::FILE* operator>>(std::FILE* stream, Pipe& pipe)
   {
   static const u32bit BUFFERSIZE = DEFAULT_BUFFERSIZE;
   SecureBuffer<byte, BUFFERSIZE> buffer;
   while(!ferror(stream) && !feof(stream))
      {
      u32bit got = fread(buffer, 1, BUFFERSIZE, stream);
      pipe.write(buffer, got);
      }
   if(ferror(stream))
      throw Stream_IO_Error("Pipe input operator (stdio) has failed");
   return stream;
   }

}
