/*************************************************
* Filters Header File                            *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_FILTERS_H__
#define OPENCL_FILTERS_H__

#include <opencl/filtbase.h>
#include <opencl/symkey.h>

namespace OpenCL {

/*************************************************
* Stream Cipher Filter                           *
*************************************************/
template<typename S>
class StreamCipherFilter : public Filter
   {
   public:
      void write(const byte[], u32bit);
      StreamCipherFilter(const StreamCipherKey& key)
         { cipher.set_key(key, key.length()); }
   private:
      static const u32bit BUFFERSIZE = DEFAULT_BUFFERSIZE;
      S cipher;
      SecureBuffer<byte, BUFFERSIZE> buffer;
   };

/*************************************************
* Hash Filter                                    *
*************************************************/
template<typename H>
class HashFilter : public Filter
   {
   public:
      void write(const byte input[], u32bit len) { hash.update(input, len); }
      void final();
      HashFilter(u32bit o_len = H::HASHLENGTH) { out_len = o_len; }
   private:
      H hash;
      u32bit out_len;
   };

/*************************************************
* MAC Filter                                     *
*************************************************/
template<typename M>
class MACFilter : public Filter
   {
   public:
      void write(const byte input[], u32bit len) { mac.update(input, len); }
      void final();
      MACFilter(const MACKey& key, u32bit outsize = M::MACLENGTH)
         { mac.set_key(key, key.length()); out_len = outsize; }
   private:
      M mac;
      u32bit out_len;
   };

/*************************************************
* Fixed Bit Encoder Filter                       *
*************************************************/
template<typename E>
class EncoderFilter : public Filter
   {
   public:
      void write(const byte[], u32bit);
      void final();
      EncoderFilter(bool breaks = false, u32bit length = 78)
         : LINEBREAKS(breaks), LINELENGTH(length)
         { counter = position = 0; }
   private:
      static const u32bit INPUTSIZE = E::INPUTSIZE,
                          OUTPUTSIZE = E::OUTPUTSIZE,
                          BUFFERSIZE = 16*INPUTSIZE;
      void encode(const byte[], u32bit);
      void output_buffer();
      const bool LINEBREAKS;
      const u32bit LINELENGTH;
      SecureBuffer<byte, BUFFERSIZE> buffer;
      SecureBuffer<byte, OUTPUTSIZE> out;
      u32bit counter, position;
   };

/*************************************************
* Fixed Bit Decoder Filter                       *
*************************************************/
template<typename E>
class DecoderFilter : public Filter
   {
   public:
      void write(const byte[], u32bit);
      void final();
      bool is_valid(byte input) { return E::is_valid(input); }
      DecoderFilter() { position = 0; }
   private:
      static const u32bit INPUTSIZE = E::OUTPUTSIZE,
                          OUTPUTSIZE = E::INPUTSIZE,
                          BUFFERSIZE = 16*INPUTSIZE;
      void decode(const byte[], u32bit);
      SecureBuffer<byte, BUFFERSIZE> buffer;
      SecureBuffer<byte, OUTPUTSIZE> out;
      u32bit position;
   };

}

#endif

#ifndef OPENCL_FILTERS_ICC__
#define OPENCL_FILTERS_ICC__
#include <opencl/filters.icc>
#endif
