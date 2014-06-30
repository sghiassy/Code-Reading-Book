/* This file is in the public domain */

/*
  If defined, make sure decryption actually works. But it makes the
  benchmarks really slow, because of all the overhead for checking the
  decryption. So it's normally disabled (don't worry, it's turned on once in a
  while to make sure nothing is broken).
*/
//#define CHECK_DECRYPTION

#include <iostream>
#include <string>
#include <cstdlib>

#include <opencl/filtbase.h>
#include <opencl/symkey.h>

#include <opencl/blowfish.h>
#include <opencl/cast256.h>
#include <opencl/cast5.h>
#include <opencl/cscipher.h>
#include <opencl/des.h>
#include <opencl/desx.h>
#include <opencl/gost.h>
#include <opencl/idea.h>
#include <opencl/lion.h>
#include <opencl/lubyrack.h>
#include <opencl/misty1.h>
#include <opencl/rc2.h>
#include <opencl/rc5.h>
#include <opencl/rc6.h>
#include <opencl/rijndael.h>
#include <opencl/safer_sk.h>
#include <opencl/serpent.h>
#include <opencl/shark.h>
#include <opencl/skipjack.h>
#include <opencl/square.h>
#include <opencl/tea.h>
#include <opencl/threeway.h>
#include <opencl/twofish.h>
#include <opencl/xtea.h>

#include <opencl/sha1.h> // for Luby-Rackoff
#include <opencl/md5.h> // for Lion
#include <opencl/isaac.h> // for Lion
#include <opencl/seal.h> // for Lion

using namespace OpenCL;

#ifdef CHECK_DECRYPTION
#include <opencl/crc32.h>

typedef OpenCL::CRC32 CHECK_HASH;
#endif

template<typename B>
class ECB_Filter : public Filter
   {
   public:
      void write(const byte[], u32bit);
      void final();
      ECB_Filter(const BlockCipherKey& key)
        { cipher.set_key(key, key.length()); position = 0; }
   private:
      static const u32bit BLOCKSIZE = B::BLOCKSIZE;
      B cipher;
      SecureBuffer<byte, BLOCKSIZE> buffer;
      u32bit position;
#ifdef CHECK_DECRYPTION
      CHECK_HASH hash_input, hash_decrypt;
#endif
   };

template<typename B>
void ECB_Filter<B>::write(const byte input[], u32bit length)
   {
#ifdef CHECK_DECRYPTION
   hash_input.update(input, length);
#endif
   buffer.copy(position, input, length);
   if(position + length >= BLOCKSIZE)
      {
      cipher.encrypt(buffer);
      send(buffer, BLOCKSIZE);
#ifdef CHECK_DECRYPTION
      cipher.decrypt(buffer);
      hash_decrypt.update(buffer, BLOCKSIZE);
#endif
      input += (BLOCKSIZE - position);
      length -= (BLOCKSIZE - position);
      while(length >= BLOCKSIZE)
         {
         cipher.encrypt(input, buffer);
         send(buffer, BLOCKSIZE);
#ifdef CHECK_DECRYPTION
         cipher.decrypt(buffer);
         hash_decrypt.update(buffer, BLOCKSIZE);
#endif
         input += BLOCKSIZE;
         length -= BLOCKSIZE;
         }
       buffer.copy(input, length);
      position = 0;
      }
   position += length;
   }

template<typename B>
void ECB_Filter<B>::final()
   {
#ifdef CHECK_DECRYPTION

   /* We want to check decryption works too, so we just have a hash of the
      decrypted output. If it doesn't equal the hash of the input, we're in
      trouble. It's done internally, so we don't have to export this odd
      inconsistenctey WRT the other functions into the validate code. If it's
      wrong, we just go and die. */

   byte input_hash[CHECK_HASH::HASHLENGTH],
        output_hash[CHECK_HASH::HASHLENGTH];
   hash_input.final(input_hash);
   hash_decrypt.final(output_hash);

   for(u32bit j = 0; j != CHECK_HASH::HASHLENGTH; j++)
      if(input_hash[j] != output_hash[j])
         {
         std::cout << "In " << cipher.name()
                   << " decryption CRC check failed." << std::endl;
         std::exit(1);
         }
#endif
   if(position)
      {
      std::cout << "In ECB_Filter::final, position != 0" << std::endl;
      }
   }

Filter* lookup_block(const std::string& algname, const BlockCipherKey& key)
   {
   if(algname == "Blowfish")         return new ECB_Filter<Blowfish>(key);
   else if(algname == "CAST256")     return new ECB_Filter<CAST256>(key);
   else if(algname == "CAST5")       return new ECB_Filter<CAST5>(key);
   else if(algname == "CS-Cipher")   return new ECB_Filter<CS_Cipher>(key);
   else if(algname == "DES")         return new ECB_Filter<DES>(key);
   else if(algname == "DESX")        return new ECB_Filter<DESX>(key);
   else if(algname == "Triple-DES")  return new ECB_Filter<TripleDES>(key);
   else if(algname == "GOST")        return new ECB_Filter<GOST>(key);
   else if(algname == "IDEA")        return new ECB_Filter<IDEA>(key);

   else if(algname == "Lion<SHA1,SEAL>")
      return new ECB_Filter< Lion<SHA1, SEAL, 256*1024> >(key);
   else if(algname == "Lion<MD5,ISAAC>")
      return new ECB_Filter< Lion<MD5, ISAAC, 256*1024> >(key);

   else if(algname == "Lion<SHA1,SEAL,64>")
      return new ECB_Filter< Lion<SHA1, SEAL, 64> >(key);
   else if(algname == "Luby-Rackoff<SHA1>")
      return new ECB_Filter< LubyRackoff<SHA1> >(key);

   else if(algname == "MISTY1")      return new ECB_Filter<MISTY1>(key);
   else if(algname == "RC2")         return new ECB_Filter<RC2>(key);
   else if(algname == "RC5-12")      return new ECB_Filter<RC5_12>(key);
   else if(algname == "RC5-16")      return new ECB_Filter<RC5_16>(key);
   else if(algname == "RC6")         return new ECB_Filter<RC6>(key);
   else if(algname == "Rijndael")    return new ECB_Filter<Rijndael>(key);
   else if(algname == "SAFER-SK128") return new ECB_Filter<SAFER_SK128>(key);
   else if(algname == "Serpent")     return new ECB_Filter<Serpent>(key);
   else if(algname == "SHARK")       return new ECB_Filter<SHARK>(key);
   else if(algname == "Skipjack")    return new ECB_Filter<Skipjack>(key);
   else if(algname == "Square")      return new ECB_Filter<Square>(key);
   else if(algname == "TEA")         return new ECB_Filter<TEA>(key);
   else if(algname == "ThreeWay")    return new ECB_Filter<ThreeWay>(key);
   else if(algname == "Twofish")     return new ECB_Filter<Twofish>(key);
   else if(algname == "XTEA")        return new ECB_Filter<XTEA>(key);

   else return 0;
   }

