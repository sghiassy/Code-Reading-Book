/*************************************************
* CBC Padding Methods Source File                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/mode_pad.h>
#include <opencl/util.h>

namespace OpenCL {

/*************************************************
* Pad with PKCS #7 Method                        *
*************************************************/
void PKCSPadding::pad(byte block[], u32bit size, u32bit position)
   {
   for(u32bit j = 0; j != size; j++)
      block[j] = (size - position);
   }

/*************************************************
* Unpad with PKCS #7 Method                      *
*************************************************/
u32bit PKCSPadding::unpad(const byte block[], u32bit size)
   {
   u32bit position = block[size-1];
   if(position > size)
      throw Decoding_Error(name());
   return (size - position);
   }

/*************************************************
* Query if the size is valid for this method     *
*************************************************/
bool PKCSPadding::valid_blocksize(u32bit size)
   {
   if(size > 0 && size < 256) return true;
   else                       return false;
   }

/*************************************************
* Pad with One and Zeros Method                  *
*************************************************/
void OneAndZeros::pad(byte block[], u32bit size, u32bit)
   {
   block[0] = 0x80;
   for(u32bit j = 1; j != size; j++)
      block[j] = 0x00;
   }

/*************************************************
* Unpad with One and Zeros Method                *
*************************************************/
u32bit OneAndZeros::unpad(const byte block[], u32bit size)
   {
   while(size)
      {
      if(block[size - 1] == 0x80) break;
      if(block[size - 1] != 0x00) throw Decoding_Error(name());
      size--;
      }
   if(!size) throw Decoding_Error(name());
   return (size - 1);
   }

/*************************************************
* Query if the size is valid for this method     *
*************************************************/
bool OneAndZeros::valid_blocksize(u32bit size)
   {
   if(size) return true;
   else     return false;
   }

}
