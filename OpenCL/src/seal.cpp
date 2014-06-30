/*************************************************
* SEAL Source File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/seal.h>
#include <opencl/seal_gam.h>

namespace OpenCL {

/*************************************************
* Generate SEAL Cipher Stream                    *
*************************************************/
void SEAL::cipher(const byte in[], byte out[], u32bit length)
   {
   while(length >= STATESIZE - position)
      {
      xor_buf(out, in, state + position, STATESIZE - position);
      length -= (STATESIZE - position);
      in += (STATESIZE - position);
      out += (STATESIZE - position);
      generate(counter++);
      }
   xor_buf(out, in, state + position, length);
   position += length;
   }

/*************************************************
* Generate the internal state                    *
*************************************************/
void SEAL::generate(u32bit n)
   {
   u32bit A, B, C, D, P, Q;
   SecureBuffer<u32bit, 4> N, X;
   for(u32bit j = 0; j != STATESIZE / 1024; j++)
      {
      A = n ^ R[4*j];                     B = rotate_right(n,  8) ^ R[4*j+1];
      C = rotate_right(n, 16) ^ R[4*j+2]; D = rotate_right(n, 24) ^ R[4*j+3];
      P = A & 0x7FC; A = rotate_right(A, 9); B += T[P/4];
      P = B & 0x7FC; B = rotate_right(B, 9); C += T[P/4];
      P = C & 0x7FC; C = rotate_right(C, 9); D += T[P/4];
      P = D & 0x7FC; D = rotate_right(D, 9); A += T[P/4];
      P = A & 0x7FC; A = rotate_right(A, 9); B += T[P/4];
      P = B & 0x7FC; B = rotate_right(B, 9); C += T[P/4];
      P = C & 0x7FC; C = rotate_right(C, 9); D += T[P/4];
      P = D & 0x7FC; D = rotate_right(D, 9); A += T[P/4];
      N[0] = D; N[1] = B; N[2] = A; N[3] = C;
      P = A & 0x7FC; A = rotate_right(A, 9); B += T[P/4];
      P = B & 0x7FC; B = rotate_right(B, 9); C += T[P/4];
      P = C & 0x7FC; C = rotate_right(C, 9); D += T[P/4];
      P = D & 0x7FC; D = rotate_right(D, 9); A += T[P/4];
      for(u32bit k = 0; k != 64; k++)
         {
         P = A       & 0x7FC; A = rotate_right(A, 9); B = (B + T[P/4]) ^ A;
         Q = B       & 0x7FC; B = rotate_right(B, 9); C = (C ^ T[Q/4]) + B;
         P = (P + C) & 0x7FC; C = rotate_right(C, 9); D = (D + T[P/4]) ^ C;
         Q = (Q + D) & 0x7FC; D = rotate_right(D, 9); A = (A ^ T[Q/4]) + D;
         P = (P + A) & 0x7FC; A = rotate_right(A, 9); B = (B ^ T[P/4]);
         Q = (Q + B) & 0x7FC; B = rotate_right(B, 9); C = (C + T[Q/4]);
         P = (P + C) & 0x7FC; C = rotate_right(C, 9); D = (D ^ T[P/4]);
         Q = (Q + D) & 0x7FC; D = rotate_right(D, 9); A = (A + T[Q/4]);
         X[0] = B + S[4*k  ]; X[1] = C ^ S[4*k+1];
         X[2] = D + S[4*k+2]; X[3] = A ^ S[4*k+3];
         for(u32bit l = 0; l != 16; l++)
            state[1024*j+16*k+l] = get_byte(l%4, X[l/4]);
         if(k % 2 == 0) { A += N[0]; B += N[1]; C ^= N[0]; D ^= N[1]; }
         else           { A += N[2]; B += N[3]; C ^= N[2]; D ^= N[3]; }
         }
      }
   position = 0;
   }

/*************************************************
* SEAL Key Schedule                              *
*************************************************/
void SEAL::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   clear();

   Gamma gamma(key);
   for(u32bit j = 0; j != 512; j++)
      T[j] = gamma(j);
   for(u32bit j = 0; j != 256; j++)
      S[j] = gamma(0x1000 + j);
   for(u32bit j = 0; j != STATESIZE / 256; j++)
      R[j] = gamma(0x2000 + j);
   generate(counter++);
   }

/*************************************************
* Seek a new position in the cipher stream       *
*************************************************/
void SEAL::seek(u32bit new_position)
   {
   counter = (new_position / STATESIZE) + START;
   generate(counter++);
   position = new_position % STATESIZE;
   }

/*************************************************
* Clear memory of sensitive data                 *
*************************************************/
void SEAL::clear() throw()
   {
   state.clear();
   T.clear();
   S.clear();
   R.clear();
   position = 0;
   counter = START;
   }

/*************************************************
* SEAL Constructor                               *
*************************************************/
SEAL::SEAL(u32bit s, u32bit L) : RandomAccessStreamCipher(name(), KEYLENGTH),
                                 START(s), STATESIZE(L*1024),
                                 state(STATESIZE), R(STATESIZE/256)
   {
   if(L == 0 || L > 64)
      throw Invalid_Argument(name() + ": Invalid size for L");
   clear();
   }

}
