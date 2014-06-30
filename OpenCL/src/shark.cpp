/*************************************************
* SHARK Source File                              *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/shark.h>

namespace OpenCL {

/*************************************************
* SHARK Encryption                               *
*************************************************/
void SHARK::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u64bit B = make_u64bit(in[0], in[1], in[2], in[3],
                          in[4], in[5], in[6], in[7]) ^ EK[0];
   for(u32bit j = 1; j != 6; j++)
      B = TE0[get_byte(0, B)] ^ TE1[get_byte(1, B)] ^ TE2[get_byte(2, B)] ^
          TE3[get_byte(3, B)] ^ TE4[get_byte(4, B)] ^ TE5[get_byte(5, B)] ^
          TE6[get_byte(6, B)] ^ TE7[get_byte(7, B)] ^ EK[j];
   B = make_u64bit(SE[get_byte(0, B)], SE[get_byte(1, B)], SE[get_byte(2, B)],
                   SE[get_byte(3, B)], SE[get_byte(4, B)], SE[get_byte(5, B)],
                   SE[get_byte(6, B)], SE[get_byte(7, B)]) ^ EK[6];
   out[0] = get_byte(0, B); out[1] = get_byte(1, B);
   out[2] = get_byte(2, B); out[3] = get_byte(3, B);
   out[4] = get_byte(4, B); out[5] = get_byte(5, B);
   out[6] = get_byte(6, B); out[7] = get_byte(7, B);
   }

/*************************************************
* SHARK Decryption                               *
*************************************************/
void SHARK::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u64bit B = make_u64bit(in[0], in[1], in[2], in[3],
                          in[4], in[5], in[6], in[7]) ^ DK[0];
   for(u32bit j = 1; j != 6; j++)
      B = TD0[get_byte(0, B)] ^ TD1[get_byte(1, B)] ^ TD2[get_byte(2, B)] ^
          TD3[get_byte(3, B)] ^ TD4[get_byte(4, B)] ^ TD5[get_byte(5, B)] ^
          TD6[get_byte(6, B)] ^ TD7[get_byte(7, B)] ^ DK[j];
   B = make_u64bit(SD[get_byte(0, B)], SD[get_byte(1, B)], SD[get_byte(2, B)],
                   SD[get_byte(3, B)], SD[get_byte(4, B)], SD[get_byte(5, B)],
                   SD[get_byte(6, B)], SD[get_byte(7, B)]) ^ DK[6];
   out[0] = get_byte(0, B); out[1] = get_byte(1, B);
   out[2] = get_byte(2, B); out[3] = get_byte(3, B);
   out[4] = get_byte(4, B); out[5] = get_byte(5, B);
   out[6] = get_byte(6, B); out[7] = get_byte(7, B);
   }

/*************************************************
* SHARK Key Schedule                             *
*************************************************/
void SHARK::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   SecureBuffer<byte, 56> subkeys;
   SecureBuffer<byte, 8> buffer;
   for(u32bit j = 0; j != 56; j++)
      subkeys[j] = key[j % length];
   EK.copy(TE0, 7); EK[6] = transform(EK[6]);
   for(u32bit j = 0; j != 7; j++)
      {
      encrypt(buffer);
      xor_buf(subkeys + 8*j, buffer, 8);
      buffer.copy(subkeys + 8*j, 8);
      }
   for(u32bit j = 0; j != 56; j++)
      EK[j/8] = (EK[j/8] << 8) + subkeys[j];
   DK[0] = EK[6] = transform(EK[6]);
   for(u32bit j = 1; j != 6; j++)
      DK[j] = transform(EK[6 - j]);
   DK[6] = EK[0];
   }

/*************************************************
* SHARK's Transformation Function                *
*************************************************/
u64bit SHARK::transform(u64bit input)
   {
   static const byte G[8][8] = {
      { 0xE7, 0x30, 0x90, 0x85, 0xD0, 0x4B, 0x91, 0x41 },
      { 0x53, 0x95, 0x9B, 0xA5, 0x96, 0xBC, 0xA1, 0x68 },
      { 0x02, 0x45, 0xF7, 0x65, 0x5C, 0x1F, 0xB6, 0x52 },
      { 0xA2, 0xCA, 0x22, 0x94, 0x44, 0x63, 0x2A, 0xA2 },
      { 0xFC, 0x67, 0x8E, 0x10, 0x29, 0x75, 0x85, 0x71 },
      { 0x24, 0x45, 0xA2, 0xCF, 0x2F, 0x22, 0xC1, 0x0E },
      { 0xA1, 0xF1, 0x71, 0x40, 0x91, 0x27, 0x18, 0xA5 },
      { 0x56, 0xF4, 0xAF, 0x32, 0xD2, 0xA4, 0xDC, 0x71 } };
   SecureBuffer<byte, 8> A, B;
   for(u32bit j = 0; j != 8; j++)
      A[j] = get_byte(j, input);
   for(u32bit j = 0; j != 8; j++)
      for(u32bit k = 0; k != 8; k++)
         B[j] ^= mul(A[k], G[j][k]);
   return make_u64bit(B[0], B[1], B[2], B[3], B[4], B[5], B[6], B[7]);
   }

/*************************************************
* Multiply in GF(2^8)                            *
*************************************************/
byte SHARK::mul(byte a, byte b)
   {
   if(a && b)
      return ALog[(Log[a] + Log[b]) % 255];
   else
      return 0;
   }

}
