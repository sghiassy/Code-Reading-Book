/*************************************************
* CRC24 Source File                              *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/crc24.h>

namespace OpenCL {

/*************************************************
* Update a CRC24 Hash                            *
*************************************************/
void CRC24::update_hash(const byte input[], u32bit length)
   {
   while(length >= 16)
      {
      crc = TABLE[((crc >> 16) ^ input[ 0]) & 0xFF] ^ (crc << 8);
      crc = TABLE[((crc >> 16) ^ input[ 1]) & 0xFF] ^ (crc << 8);
      crc = TABLE[((crc >> 16) ^ input[ 2]) & 0xFF] ^ (crc << 8);
      crc = TABLE[((crc >> 16) ^ input[ 3]) & 0xFF] ^ (crc << 8);
      crc = TABLE[((crc >> 16) ^ input[ 4]) & 0xFF] ^ (crc << 8);
      crc = TABLE[((crc >> 16) ^ input[ 5]) & 0xFF] ^ (crc << 8);
      crc = TABLE[((crc >> 16) ^ input[ 6]) & 0xFF] ^ (crc << 8);
      crc = TABLE[((crc >> 16) ^ input[ 7]) & 0xFF] ^ (crc << 8);
      crc = TABLE[((crc >> 16) ^ input[ 8]) & 0xFF] ^ (crc << 8);
      crc = TABLE[((crc >> 16) ^ input[ 9]) & 0xFF] ^ (crc << 8);
      crc = TABLE[((crc >> 16) ^ input[10]) & 0xFF] ^ (crc << 8);
      crc = TABLE[((crc >> 16) ^ input[11]) & 0xFF] ^ (crc << 8);
      crc = TABLE[((crc >> 16) ^ input[12]) & 0xFF] ^ (crc << 8);
      crc = TABLE[((crc >> 16) ^ input[13]) & 0xFF] ^ (crc << 8);
      crc = TABLE[((crc >> 16) ^ input[14]) & 0xFF] ^ (crc << 8);
      crc = TABLE[((crc >> 16) ^ input[15]) & 0xFF] ^ (crc << 8);
      input += 16;
      length -= 16;
      }
   for(u32bit j = 0; j != length; j++)
      crc = TABLE[((crc >> 16) ^ input[j]) & 0xFF] ^ (crc << 8);
   }

/*************************************************
* Finalize a CRC24 Hash                          *
*************************************************/
void CRC24::final(byte output[HASHLENGTH])
   {
   for(u32bit j = 0; j != HASHLENGTH; j++)
      output[j] = get_byte(j+1, crc);
   clear();
   }

}
