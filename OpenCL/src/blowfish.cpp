/*************************************************
* Blowfish Source File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/blowfish.h>

namespace OpenCL {

/*************************************************
* Blowfish Encryption                            *
*************************************************/
void Blowfish::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit L  = make_u32bit(in[0], in[1], in[2], in[3]),
          R = make_u32bit(in[4], in[5], in[6], in[7]);
   round(L, R, 0); round(R, L, 1); round(L, R, 2); round(R, L, 3);
   round(L, R, 4); round(R, L, 5); round(L, R, 6); round(R, L, 7);
   round(L, R, 8); round(R, L, 9); round(L, R,10); round(R, L,11);
   round(L, R,12); round(R, L,13); round(L, R,14); round(R, L,15);
   L ^= Pbox[16]; R ^= Pbox[17];
   out[0] = get_byte(0, R); out[1] = get_byte(1, R);
   out[2] = get_byte(2, R); out[3] = get_byte(3, R);
   out[4] = get_byte(0, L); out[5] = get_byte(1, L);
   out[6] = get_byte(2, L); out[7] = get_byte(3, L);
   }

/*************************************************
* Blowfish Decryption                            *
*************************************************/
void Blowfish::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit L  = make_u32bit(in[0], in[1], in[2], in[3]),
          R = make_u32bit(in[4], in[5], in[6], in[7]);
   round(L, R,17); round(R, L,16); round(L, R,15); round(R, L,14);
   round(L, R,13); round(R, L,12); round(L, R,11); round(R, L,10);
   round(L, R, 9); round(R, L, 8); round(L, R, 7); round(R, L, 6);
   round(L, R, 5); round(R, L, 4); round(L, R, 3); round(R, L, 2);
   L ^= Pbox[1]; R ^= Pbox[0];
   out[0] = get_byte(0, R); out[1] = get_byte(1, R);
   out[2] = get_byte(2, R); out[3] = get_byte(3, R);
   out[4] = get_byte(0, L); out[5] = get_byte(1, L);
   out[6] = get_byte(2, L); out[7] = get_byte(3, L);
   }

/*************************************************
* Blowfish Round                                 *
*************************************************/
void Blowfish::round(u32bit& L, u32bit& R, u32bit n) const
   {
   L  ^= Pbox[n];
   R ^= ((Sbox1[get_byte(0, L)]  + Sbox2[get_byte(1, L)]) ^
          Sbox3[get_byte(2, L)]) + Sbox4[get_byte(3, L)];
   }

/*************************************************
* Blowfish Key Schedule                          *
*************************************************/
void Blowfish::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   clear();
   for(u32bit j = 0, k = 0; j != 18; j++, k += 4)
      Pbox[j] ^= make_u32bit(key[(k  ) % length], key[(k+1) % length],
                             key[(k+2) % length], key[(k+3) % length]);
   u32bit L = 0, R = 0;
   generate_sbox(Pbox,  18,  L, R);
   generate_sbox(Sbox1, 256, L, R);
   generate_sbox(Sbox2, 256, L, R);
   generate_sbox(Sbox3, 256, L, R);
   generate_sbox(Sbox4, 256, L, R);
   }

/*************************************************
* Generate one of the Sboxes                     *
*************************************************/
void Blowfish::generate_sbox(u32bit Box[], u32bit size,
                             u32bit& L, u32bit& R) const
   {
   for(u32bit j = 0; j != size; j += 2)
      {
      round(L, R, 0); round(R, L, 1); round(L, R, 2); round(R, L, 3);
      round(L, R, 4); round(R, L, 5); round(L, R, 6); round(R, L, 7);
      round(L, R, 8); round(R, L, 9); round(L, R,10); round(R, L,11);
      round(L, R,12); round(R, L,13); round(L, R,14); round(R, L,15);
      u32bit T = R; R = L ^ Pbox[16]; L = T ^ Pbox[17];
      Box[j] = L; Box[j+1] = R;
      }
   }

/*************************************************
* Clear memory of sensitive data                 *
*************************************************/
void Blowfish::clear() throw()
   {
   Pbox.copy(PBOX, 18);
   Sbox1.copy(SBOX1, 256);
   Sbox2.copy(SBOX2, 256);
   Sbox3.copy(SBOX3, 256);
   Sbox4.copy(SBOX4, 256);
   }

}
