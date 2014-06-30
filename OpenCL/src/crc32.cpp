/*************************************************
* CRC32 Source File                              *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/crc32.h>

namespace OpenCL {

/*************************************************
* Update a CRC32 Hash                            *
*************************************************/
void CRC32::update_hash(const byte input[], u32bit length)
   {
   while(length >= 16)
      {
      crc = TABLE[(crc ^ input[ 0]) & 0xFF] ^ (crc >> 8);
      crc = TABLE[(crc ^ input[ 1]) & 0xFF] ^ (crc >> 8);
      crc = TABLE[(crc ^ input[ 2]) & 0xFF] ^ (crc >> 8);
      crc = TABLE[(crc ^ input[ 3]) & 0xFF] ^ (crc >> 8);
      crc = TABLE[(crc ^ input[ 4]) & 0xFF] ^ (crc >> 8);
      crc = TABLE[(crc ^ input[ 5]) & 0xFF] ^ (crc >> 8);
      crc = TABLE[(crc ^ input[ 6]) & 0xFF] ^ (crc >> 8);
      crc = TABLE[(crc ^ input[ 7]) & 0xFF] ^ (crc >> 8);
      crc = TABLE[(crc ^ input[ 8]) & 0xFF] ^ (crc >> 8);
      crc = TABLE[(crc ^ input[ 9]) & 0xFF] ^ (crc >> 8);
      crc = TABLE[(crc ^ input[10]) & 0xFF] ^ (crc >> 8);
      crc = TABLE[(crc ^ input[11]) & 0xFF] ^ (crc >> 8);
      crc = TABLE[(crc ^ input[12]) & 0xFF] ^ (crc >> 8);
      crc = TABLE[(crc ^ input[13]) & 0xFF] ^ (crc >> 8);
      crc = TABLE[(crc ^ input[14]) & 0xFF] ^ (crc >> 8);
      crc = TABLE[(crc ^ input[15]) & 0xFF] ^ (crc >> 8);
      input += 16;
      length -= 16;
      }
   for(u32bit j = 0; j != length; j++)
      crc = TABLE[(crc ^ input[j]) & 0xFF] ^ (crc >> 8);
   }

/*************************************************
* Finalize a CRC32 Hash                          *
*************************************************/
void CRC32::final(byte output[HASHLENGTH])
   {
   crc ^= 0xFFFFFFFF;
   for(u32bit j = 0; j != HASHLENGTH; j++)
      output[j] = get_byte(j, crc);
   clear();
   }

}
