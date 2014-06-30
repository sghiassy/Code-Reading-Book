/*************************************************
* Skipjack Source File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/skipjack.h>

namespace OpenCL {

/*************************************************
* Skipjack Encryption                            *
*************************************************/
void Skipjack::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u16bit W1 = make_u16bit(in[0], in[1]), W2 = make_u16bit(in[2], in[3]),
          W3 = make_u16bit(in[4], in[5]), W4 = make_u16bit(in[6], in[7]);

   step_A(W1,W4, 1); step_A(W4,W3, 2); step_A(W3,W2, 3); step_A(W2,W1, 4);
   step_A(W1,W4, 5); step_A(W4,W3, 6); step_A(W3,W2, 7); step_A(W2,W1, 8);

   step_B(W1,W2, 9); step_B(W4,W1,10); step_B(W3,W4,11); step_B(W2,W3,12);
   step_B(W1,W2,13); step_B(W4,W1,14); step_B(W3,W4,15); step_B(W2,W3,16);

   step_A(W1,W4,17); step_A(W4,W3,18); step_A(W3,W2,19); step_A(W2,W1,20);
   step_A(W1,W4,21); step_A(W4,W3,22); step_A(W3,W2,23); step_A(W2,W1,24);

   step_B(W1,W2,25); step_B(W4,W1,26); step_B(W3,W4,27); step_B(W2,W3,28);
   step_B(W1,W2,29); step_B(W4,W1,30); step_B(W3,W4,31); step_B(W2,W3,32);

   out[0] = get_byte(0, W1); out[1] = get_byte(1, W1);
   out[2] = get_byte(0, W2); out[3] = get_byte(1, W2);
   out[4] = get_byte(0, W3); out[5] = get_byte(1, W3);
   out[6] = get_byte(0, W4); out[7] = get_byte(1, W4);
   }

/*************************************************
* Skipjack Decryption                            *
*************************************************/
void Skipjack::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u16bit W1 = make_u16bit(in[0], in[1]), W2 = make_u16bit(in[2], in[3]),
          W3 = make_u16bit(in[4], in[5]), W4 = make_u16bit(in[6], in[7]);

   step_Bi(W2,W3,32); step_Bi(W3,W4,31); step_Bi(W4,W1,30); step_Bi(W1,W2,29);
   step_Bi(W2,W3,28); step_Bi(W3,W4,27); step_Bi(W4,W1,26); step_Bi(W1,W2,25);

   step_Ai(W1,W2,24); step_Ai(W2,W3,23); step_Ai(W3,W4,22); step_Ai(W4,W1,21);
   step_Ai(W1,W2,20); step_Ai(W2,W3,19); step_Ai(W3,W4,18); step_Ai(W4,W1,17);

   step_Bi(W2,W3,16); step_Bi(W3,W4,15); step_Bi(W4,W1,14); step_Bi(W1,W2,13);
   step_Bi(W2,W3,12); step_Bi(W3,W4,11); step_Bi(W4,W1,10); step_Bi(W1,W2, 9);

   step_Ai(W1,W2, 8); step_Ai(W2,W3, 7); step_Ai(W3,W4, 6); step_Ai(W4,W1, 5);
   step_Ai(W1,W2, 4); step_Ai(W2,W3, 3); step_Ai(W3,W4, 2); step_Ai(W4,W1, 1);

   out[0] = get_byte(0, W1); out[1] = get_byte(1, W1);
   out[2] = get_byte(0, W2); out[3] = get_byte(1, W2);
   out[4] = get_byte(0, W3); out[5] = get_byte(1, W3);
   out[6] = get_byte(0, W4); out[7] = get_byte(1, W4);
   }

/*************************************************
* Skipjack Stepping Rule 'A'                     *
*************************************************/
void Skipjack::step_A(u16bit& W1, u16bit& W4, u32bit round) const
   {
   byte G1 = get_byte(0, W1), G2 = get_byte(1, W1), G3;
   G3 = FTABLE[(4 * round - 4) % 10][G2] ^ G1;
   G1 = FTABLE[(4 * round - 3) % 10][G3] ^ G2;
   G2 = FTABLE[(4 * round - 2) % 10][G1] ^ G3;
   G3 = FTABLE[(4 * round - 1) % 10][G2] ^ G1;
   W1 =  make_u16bit(G2, G3);
   W4 ^= W1 ^ round;
   }

/*************************************************
* Skipjack Stepping Rule 'B'                     *
*************************************************/
void Skipjack::step_B(u16bit& W1, u16bit& W2, u32bit round) const
   {
   W2 ^= W1 ^ round;
   byte G1 = get_byte(0, W1), G2 = get_byte(1, W1), G3;
   G3 = FTABLE[(4 * round - 4) % 10][G2] ^ G1;
   G1 = FTABLE[(4 * round - 3) % 10][G3] ^ G2;
   G2 = FTABLE[(4 * round - 2) % 10][G1] ^ G3;
   G3 = FTABLE[(4 * round - 1) % 10][G2] ^ G1;
   W1 =  make_u16bit(G2, G3);
   }

/*************************************************
* Skipjack Invserse Stepping Rule 'A'            *
*************************************************/
void Skipjack::step_Ai(u16bit& W1, u16bit& W2, u32bit round) const
   {
   W1 ^= W2 ^ round;
   byte G1 = get_byte(1, W2), G2 = get_byte(0, W2), G3;
   G3 = FTABLE[(4 * round - 1) % 10][G2] ^ G1;
   G1 = FTABLE[(4 * round - 2) % 10][G3] ^ G2;
   G2 = FTABLE[(4 * round - 3) % 10][G1] ^ G3;
   G3 = FTABLE[(4 * round - 4) % 10][G2] ^ G1;
   W2 = make_u16bit(G3, G2);
   }

/*************************************************
* Skipjack Invserse Stepping Rule 'B'            *
*************************************************/
void Skipjack::step_Bi(u16bit& W2, u16bit& W3, u32bit round) const
   {
   byte G1 = get_byte(1, W2), G2 = get_byte(0, W2), G3;
   G3 = FTABLE[(4 * round - 1) % 10][G2] ^ G1;
   G1 = FTABLE[(4 * round - 2) % 10][G3] ^ G2;
   G2 = FTABLE[(4 * round - 3) % 10][G1] ^ G3;
   G3 = FTABLE[(4 * round - 4) % 10][G2] ^ G1;
   W2 = make_u16bit(G3, G2);
   W3 ^= W2 ^ round;
   }

/*************************************************
* Skipjack Key Schedule                          *
*************************************************/
void Skipjack::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   for(u32bit j = 0; j != 10; j++)
      for(u32bit k = 0; k != 256; k++)
         FTABLE[j][k] = F[k ^ key[j]];
   }

/*************************************************
* Clear memory of sensitive data                 *
*************************************************/
void Skipjack::clear() throw()
   {
   for(u32bit j = 0; j != 10; j++)
      FTABLE[j].clear();
   }

}
