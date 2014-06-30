/*************************************************
* OpenCL Basic Interface Header File             *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_OPENCL_H__
#define OPENCL_OPENCL_H__

#include <string>
#include <opencl/exceptn.h>
#include <opencl/secmem.h>
#include <opencl/version.h>

namespace OpenCL {

/*************************************************
* Algorithm Base Class                           *
*************************************************/
class Algorithm
   {
   public:
      std::string name() const { return alg_name; }
      virtual void clear() throw() = 0;
      Algorithm(const std::string& n) : alg_name(n) {}
      virtual ~Algorithm() {}
   private:
      const std::string alg_name;
   };

/*************************************************
* Symmetric Algorithm Base Class                 *
*************************************************/
class SymmetricAlgorithm : public Algorithm
   {
   public:
      virtual void set_key(const byte[], u32bit) throw(InvalidKeyLength) = 0;
      virtual bool valid_keylength(u32bit) const;
      SymmetricAlgorithm(const std::string&, u32bit, u32bit, u32bit);
      virtual ~SymmetricAlgorithm() {}
   private:
      const u32bit max, min, mod;
   };

/*************************************************
* Block Cipher Base Class                        *
*************************************************/
class BlockCipher : public SymmetricAlgorithm
   {
   public:
      const u32bit BLOCKSIZE, KEYLENGTH;
      virtual void encrypt(const byte[], byte[]) const = 0;
      virtual void decrypt(const byte[], byte[]) const = 0;
      virtual void encrypt(byte block[]) const = 0;
      virtual void decrypt(byte block[]) const = 0;
      BlockCipher(const std::string&, u32bit, u32bit, u32bit = 0, u32bit = 1);
      virtual ~BlockCipher() {}
   };

/*************************************************
* Stream Cipher Base Class                       *
*************************************************/
class StreamCipher : public SymmetricAlgorithm
   {
   public:
      const u32bit KEYLENGTH;
      void encrypt(const byte[], byte[], u32bit);
      void decrypt(const byte[], byte[], u32bit);
      void encrypt(byte[], u32bit);
      void decrypt(byte[], u32bit);
      StreamCipher(const std::string&, u32bit, u32bit = 0, u32bit = 1);
      virtual ~StreamCipher() {}
   private:
      virtual void cipher(const byte[], byte[], u32bit) = 0;
   };

/*************************************************
* Random Access Stream Cipher Base Class         *
*************************************************/
class RandomAccessStreamCipher : public StreamCipher
   {
   public:
      virtual void seek(u32bit) = 0;
      RandomAccessStreamCipher(const std::string&, u32bit,
                               u32bit = 0, u32bit = 1);
      virtual ~RandomAccessStreamCipher() {}
   };

/*************************************************
* Hash Function Base Class                       *
*************************************************/
class HashFunction : public Algorithm
   {
   public:
      const u32bit HASHLENGTH;
      void process(const byte[], u32bit, byte[]);
      void process(const std::string&, byte[]);
      void update(const byte[], u32bit);
      void update(const std::string&);
      virtual void final(byte[]) = 0;
      HashFunction(const std::string& n, u32bit hashlen) :
         Algorithm(n), HASHLENGTH(hashlen) {}
      virtual ~HashFunction() {}
   private:
      virtual void update_hash(const byte[], u32bit) = 0;
   };

/*************************************************
* MAC Base Class                                 *
*************************************************/
class MessageAuthCode : public SymmetricAlgorithm
   {
   public:
      const u32bit MACLENGTH, KEYLENGTH;
      void process(const byte[], u32bit, byte[]);
      void process(const std::string&, byte[]);
      void update(const byte[], u32bit);
      void update(const std::string&);
      virtual void final(byte[]) = 0;
      MessageAuthCode(const std::string&, u32bit, u32bit,
                      u32bit = 0, u32bit = 1);
      virtual ~MessageAuthCode() {}
   private:
      virtual void update_mac(const byte[], u32bit) = 0;
   };

/*************************************************
* Random Number Generator Base Class             *
*************************************************/
class RandomNumberGenerator : public Algorithm
   {
   public:
      virtual byte random() = 0;
      virtual void randomize(byte[], u32bit);
      virtual void add_entropy(const byte[], u32bit) throw() = 0;
      virtual void add_entropy(class EntropySource&, bool) = 0;
      RandomNumberGenerator(const std::string& n) : Algorithm(n) {}
      virtual ~RandomNumberGenerator() {}
   };

/*************************************************
* Entropy Source Base Class                      *
*************************************************/
class EntropySource
   {
   public:
      virtual u32bit fast_poll(byte[], u32bit) = 0;
      virtual u32bit slow_poll(byte[], u32bit) = 0;
      virtual ~EntropySource() {}
   };

}

#endif
