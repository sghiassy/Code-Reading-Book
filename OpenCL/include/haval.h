/*************************************************
* HAVAL Header File                              *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_HAVAL_H__
#define OPENCL_HAVAL_H__

#include <opencl/opencl.h>

namespace OpenCL {

class HAVAL : public HashFunction
   {
   public:
      static std::string name() { return "HAVAL"; }
      static const u32bit BLOCKSIZE = 128, VERSION = 1, PASS = 5;
      const u32bit HASHLENGTH;
      void final(byte[]);
      void clear() throw();
   protected:
      HAVAL(u32bit hashlen) : HashFunction(name(), HASHLENGTH),
                              HASHLENGTH(hashlen) { clear(); }
   private:
      void update_hash(const byte[], u32bit);
      void hash(const byte[BLOCKSIZE]);
      static void H1(u32bit&, u32bit, u32bit, u32bit, u32bit,
                     u32bit,  u32bit, u32bit, u32bit, u32bit);
      static void H2(u32bit&, u32bit, u32bit, u32bit, u32bit,
                     u32bit,  u32bit, u32bit, u32bit, u32bit);
      static void H3(u32bit&, u32bit, u32bit, u32bit, u32bit,
                     u32bit,  u32bit, u32bit, u32bit, u32bit);
      static void H4(u32bit&, u32bit, u32bit, u32bit, u32bit,
                     u32bit,  u32bit, u32bit, u32bit, u32bit);
      static void H5(u32bit&, u32bit, u32bit, u32bit, u32bit,
                     u32bit,  u32bit, u32bit, u32bit, u32bit);
      void tailor_digest(u32bit);
      SecureBuffer<byte, BLOCKSIZE> buffer;
      SecureBuffer<u32bit, 32> M;
      SecureBuffer<u32bit, 8> digest;
      u64bit count, position;
   };

struct HAVAL_128 : public HAVAL
   {
   static std::string name() { return "HAVAL-128"; }
   static const u32bit HASHLENGTH = 16;
   HAVAL_128() : HAVAL(HASHLENGTH) {}
   };

struct HAVAL_160 : public HAVAL
   {
   static std::string name() { return "HAVAL-160"; }
   static const u32bit HASHLENGTH = 20;
   HAVAL_160() : HAVAL(HASHLENGTH) {}
   };

struct HAVAL_192 : public HAVAL
   {
   static std::string name() { return "HAVAL-192"; }
   static const u32bit HASHLENGTH = 24;
   HAVAL_192() : HAVAL(HASHLENGTH) {}
   };

struct HAVAL_224 : public HAVAL
   {
   static std::string name() { return "HAVAL-224"; }
   static const u32bit HASHLENGTH = 28;
   HAVAL_224() : HAVAL(HASHLENGTH) {}
   };

struct HAVAL_256 : public HAVAL
   {
   static std::string name() { return "HAVAL-256"; }
   static const u32bit HASHLENGTH = 32;
   HAVAL_256() : HAVAL(HASHLENGTH) {}
   };

}

#endif
