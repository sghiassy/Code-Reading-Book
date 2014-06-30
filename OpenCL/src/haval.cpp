/*************************************************
* HAVAL Source File                              *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/haval.h>

namespace OpenCL {

/*************************************************
* HAVAL Hash                                     *
*************************************************/
void HAVAL::hash(const byte input[BLOCKSIZE])
   {
   for(u32bit j = 0; j != 32; j++)
      M[j] = make_u32bit(input[4*j+3], input[4*j+2], input[4*j+1], input[4*j]);

   u32bit A = digest[0], B = digest[1], C = digest[2], D = digest[3],
          E = digest[4], F = digest[5], G = digest[6], H = digest[7];

   H1(H,G,F,E,D,C,B,A,M[ 0],0x00000000); H1(G,F,E,D,C,B,A,H,M[ 1],0x00000000);
   H1(F,E,D,C,B,A,H,G,M[ 2],0x00000000); H1(E,D,C,B,A,H,G,F,M[ 3],0x00000000);
   H1(D,C,B,A,H,G,F,E,M[ 4],0x00000000); H1(C,B,A,H,G,F,E,D,M[ 5],0x00000000);
   H1(B,A,H,G,F,E,D,C,M[ 6],0x00000000); H1(A,H,G,F,E,D,C,B,M[ 7],0x00000000);
   H1(H,G,F,E,D,C,B,A,M[ 8],0x00000000); H1(G,F,E,D,C,B,A,H,M[ 9],0x00000000);
   H1(F,E,D,C,B,A,H,G,M[10],0x00000000); H1(E,D,C,B,A,H,G,F,M[11],0x00000000);
   H1(D,C,B,A,H,G,F,E,M[12],0x00000000); H1(C,B,A,H,G,F,E,D,M[13],0x00000000);
   H1(B,A,H,G,F,E,D,C,M[14],0x00000000); H1(A,H,G,F,E,D,C,B,M[15],0x00000000);
   H1(H,G,F,E,D,C,B,A,M[16],0x00000000); H1(G,F,E,D,C,B,A,H,M[17],0x00000000);
   H1(F,E,D,C,B,A,H,G,M[18],0x00000000); H1(E,D,C,B,A,H,G,F,M[19],0x00000000);
   H1(D,C,B,A,H,G,F,E,M[20],0x00000000); H1(C,B,A,H,G,F,E,D,M[21],0x00000000);
   H1(B,A,H,G,F,E,D,C,M[22],0x00000000); H1(A,H,G,F,E,D,C,B,M[23],0x00000000);
   H1(H,G,F,E,D,C,B,A,M[24],0x00000000); H1(G,F,E,D,C,B,A,H,M[25],0x00000000);
   H1(F,E,D,C,B,A,H,G,M[26],0x00000000); H1(E,D,C,B,A,H,G,F,M[27],0x00000000);
   H1(D,C,B,A,H,G,F,E,M[28],0x00000000); H1(C,B,A,H,G,F,E,D,M[29],0x00000000);
   H1(B,A,H,G,F,E,D,C,M[30],0x00000000); H1(A,H,G,F,E,D,C,B,M[31],0x00000000);

   H2(H,G,F,E,D,C,B,A,M[ 5],0x452821E6); H2(G,F,E,D,C,B,A,H,M[14],0x38D01377);
   H2(F,E,D,C,B,A,H,G,M[26],0xBE5466CF); H2(E,D,C,B,A,H,G,F,M[18],0x34E90C6C);
   H2(D,C,B,A,H,G,F,E,M[11],0xC0AC29B7); H2(C,B,A,H,G,F,E,D,M[28],0xC97C50DD);
   H2(B,A,H,G,F,E,D,C,M[ 7],0x3F84D5B5); H2(A,H,G,F,E,D,C,B,M[16],0xB5470917);
   H2(H,G,F,E,D,C,B,A,M[ 0],0x9216D5D9); H2(G,F,E,D,C,B,A,H,M[23],0x8979FB1B);
   H2(F,E,D,C,B,A,H,G,M[20],0xD1310BA6); H2(E,D,C,B,A,H,G,F,M[22],0x98DFB5AC);
   H2(D,C,B,A,H,G,F,E,M[ 1],0x2FFD72DB); H2(C,B,A,H,G,F,E,D,M[10],0xD01ADFB7);
   H2(B,A,H,G,F,E,D,C,M[ 4],0xB8E1AFED); H2(A,H,G,F,E,D,C,B,M[ 8],0x6A267E96);
   H2(H,G,F,E,D,C,B,A,M[30],0xBA7C9045); H2(G,F,E,D,C,B,A,H,M[ 3],0xF12C7F99);
   H2(F,E,D,C,B,A,H,G,M[21],0x24A19947); H2(E,D,C,B,A,H,G,F,M[ 9],0xB3916CF7);
   H2(D,C,B,A,H,G,F,E,M[17],0x0801F2E2); H2(C,B,A,H,G,F,E,D,M[24],0x858EFC16);
   H2(B,A,H,G,F,E,D,C,M[29],0x636920D8); H2(A,H,G,F,E,D,C,B,M[ 6],0x71574E69);
   H2(H,G,F,E,D,C,B,A,M[19],0xA458FEA3); H2(G,F,E,D,C,B,A,H,M[12],0xF4933D7E);
   H2(F,E,D,C,B,A,H,G,M[15],0x0D95748F); H2(E,D,C,B,A,H,G,F,M[13],0x728EB658);
   H2(D,C,B,A,H,G,F,E,M[ 2],0x718BCD58); H2(C,B,A,H,G,F,E,D,M[25],0x82154AEE);
   H2(B,A,H,G,F,E,D,C,M[31],0x7B54A41D); H2(A,H,G,F,E,D,C,B,M[27],0xC25A59B5);

   H3(H,G,F,E,D,C,B,A,M[19],0x9C30D539); H3(G,F,E,D,C,B,A,H,M[ 9],0x2AF26013);
   H3(F,E,D,C,B,A,H,G,M[ 4],0xC5D1B023); H3(E,D,C,B,A,H,G,F,M[20],0x286085F0);
   H3(D,C,B,A,H,G,F,E,M[28],0xCA417918); H3(C,B,A,H,G,F,E,D,M[17],0xB8DB38EF);
   H3(B,A,H,G,F,E,D,C,M[ 8],0x8E79DCB0); H3(A,H,G,F,E,D,C,B,M[22],0x603A180E);
   H3(H,G,F,E,D,C,B,A,M[29],0x6C9E0E8B); H3(G,F,E,D,C,B,A,H,M[14],0xB01E8A3E);
   H3(F,E,D,C,B,A,H,G,M[25],0xD71577C1); H3(E,D,C,B,A,H,G,F,M[12],0xBD314B27);
   H3(D,C,B,A,H,G,F,E,M[24],0x78AF2FDA); H3(C,B,A,H,G,F,E,D,M[30],0x55605C60);
   H3(B,A,H,G,F,E,D,C,M[16],0xE65525F3); H3(A,H,G,F,E,D,C,B,M[26],0xAA55AB94);
   H3(H,G,F,E,D,C,B,A,M[31],0x57489862); H3(G,F,E,D,C,B,A,H,M[15],0x63E81440);
   H3(F,E,D,C,B,A,H,G,M[ 7],0x55CA396A); H3(E,D,C,B,A,H,G,F,M[ 3],0x2AAB10B6);
   H3(D,C,B,A,H,G,F,E,M[ 1],0xB4CC5C34); H3(C,B,A,H,G,F,E,D,M[ 0],0x1141E8CE);
   H3(B,A,H,G,F,E,D,C,M[18],0xA15486AF); H3(A,H,G,F,E,D,C,B,M[27],0x7C72E993);
   H3(H,G,F,E,D,C,B,A,M[13],0xB3EE1411); H3(G,F,E,D,C,B,A,H,M[ 6],0x636FBC2A);
   H3(F,E,D,C,B,A,H,G,M[21],0x2BA9C55D); H3(E,D,C,B,A,H,G,F,M[10],0x741831F6);
   H3(D,C,B,A,H,G,F,E,M[23],0xCE5C3E16); H3(C,B,A,H,G,F,E,D,M[11],0x9B87931E);
   H3(B,A,H,G,F,E,D,C,M[ 5],0xAFD6BA33); H3(A,H,G,F,E,D,C,B,M[ 2],0x6C24CF5C);

   H4(H,G,F,E,D,C,B,A,M[24],0x7A325381); H4(G,F,E,D,C,B,A,H,M[ 4],0x28958677);
   H4(F,E,D,C,B,A,H,G,M[ 0],0x3B8F4898); H4(E,D,C,B,A,H,G,F,M[14],0x6B4BB9AF);
   H4(D,C,B,A,H,G,F,E,M[ 2],0xC4BFE81B); H4(C,B,A,H,G,F,E,D,M[ 7],0x66282193);
   H4(B,A,H,G,F,E,D,C,M[28],0x61D809CC); H4(A,H,G,F,E,D,C,B,M[23],0xFB21A991);
   H4(H,G,F,E,D,C,B,A,M[26],0x487CAC60); H4(G,F,E,D,C,B,A,H,M[ 6],0x5DEC8032);
   H4(F,E,D,C,B,A,H,G,M[30],0xEF845D5D); H4(E,D,C,B,A,H,G,F,M[20],0xE98575B1);
   H4(D,C,B,A,H,G,F,E,M[18],0xDC262302); H4(C,B,A,H,G,F,E,D,M[25],0xEB651B88);
   H4(B,A,H,G,F,E,D,C,M[19],0x23893E81); H4(A,H,G,F,E,D,C,B,M[ 3],0xD396ACC5);
   H4(H,G,F,E,D,C,B,A,M[22],0x0F6D6FF3); H4(G,F,E,D,C,B,A,H,M[11],0x83F44239);
   H4(F,E,D,C,B,A,H,G,M[31],0x2E0B4482); H4(E,D,C,B,A,H,G,F,M[21],0xA4842004);
   H4(D,C,B,A,H,G,F,E,M[ 8],0x69C8F04A); H4(C,B,A,H,G,F,E,D,M[27],0x9E1F9B5E);
   H4(B,A,H,G,F,E,D,C,M[12],0x21C66842); H4(A,H,G,F,E,D,C,B,M[ 9],0xF6E96C9A);
   H4(H,G,F,E,D,C,B,A,M[ 1],0x670C9C61); H4(G,F,E,D,C,B,A,H,M[29],0xABD388F0);
   H4(F,E,D,C,B,A,H,G,M[ 5],0x6A51A0D2); H4(E,D,C,B,A,H,G,F,M[15],0xD8542F68);
   H4(D,C,B,A,H,G,F,E,M[17],0x960FA728); H4(C,B,A,H,G,F,E,D,M[10],0xAB5133A3);
   H4(B,A,H,G,F,E,D,C,M[16],0x6EEF0B6C); H4(A,H,G,F,E,D,C,B,M[13],0x137A3BE4);

   H5(H,G,F,E,D,C,B,A,M[27],0xBA3BF050); H5(G,F,E,D,C,B,A,H,M[ 3],0x7EFB2A98);
   H5(F,E,D,C,B,A,H,G,M[21],0xA1F1651D); H5(E,D,C,B,A,H,G,F,M[26],0x39AF0176);
   H5(D,C,B,A,H,G,F,E,M[17],0x66CA593E); H5(C,B,A,H,G,F,E,D,M[11],0x82430E88);
   H5(B,A,H,G,F,E,D,C,M[20],0x8CEE8619); H5(A,H,G,F,E,D,C,B,M[29],0x456F9FB4);
   H5(H,G,F,E,D,C,B,A,M[19],0x7D84A5C3); H5(G,F,E,D,C,B,A,H,M[ 0],0x3B8B5EBE);
   H5(F,E,D,C,B,A,H,G,M[12],0xE06F75D8); H5(E,D,C,B,A,H,G,F,M[ 7],0x85C12073);
   H5(D,C,B,A,H,G,F,E,M[13],0x401A449F); H5(C,B,A,H,G,F,E,D,M[ 8],0x56C16AA6);
   H5(B,A,H,G,F,E,D,C,M[31],0x4ED3AA62); H5(A,H,G,F,E,D,C,B,M[10],0x363F7706);
   H5(H,G,F,E,D,C,B,A,M[ 5],0x1BFEDF72); H5(G,F,E,D,C,B,A,H,M[ 9],0x429B023D);
   H5(F,E,D,C,B,A,H,G,M[14],0x37D0D724); H5(E,D,C,B,A,H,G,F,M[30],0xD00A1248);
   H5(D,C,B,A,H,G,F,E,M[18],0xDB0FEAD3); H5(C,B,A,H,G,F,E,D,M[ 6],0x49F1C09B);
   H5(B,A,H,G,F,E,D,C,M[28],0x075372C9); H5(A,H,G,F,E,D,C,B,M[24],0x80991B7B);
   H5(H,G,F,E,D,C,B,A,M[ 2],0x25D479D8); H5(G,F,E,D,C,B,A,H,M[23],0xF6E8DEF7);
   H5(F,E,D,C,B,A,H,G,M[16],0xE3FE501A); H5(E,D,C,B,A,H,G,F,M[22],0xB6794C3B);
   H5(D,C,B,A,H,G,F,E,M[ 4],0x976CE0BD); H5(C,B,A,H,G,F,E,D,M[ 1],0x04C006BA);
   H5(B,A,H,G,F,E,D,C,M[25],0xC1A94FB6); H5(A,H,G,F,E,D,C,B,M[15],0x409F60C4);

   digest[0] += A;   digest[1] += B;   digest[2] += C;   digest[3] += D;
   digest[4] += E;   digest[5] += F;   digest[6] += G;   digest[7] += H;
   }

/*************************************************
* HAVAL H1 Function                              *
*************************************************/
void HAVAL::H1(u32bit& H, u32bit G, u32bit F, u32bit E, u32bit D,
               u32bit C, u32bit B, u32bit A, u32bit msg, u32bit magic)
   {
   H = rotate_right((C & (B ^ G)) ^ (E & F) ^ (A & D) ^ G, 7) +
       rotate_right(H, 11) + msg + magic;
   }

/*************************************************
* HAVAL H2 Function                              *
*************************************************/
void HAVAL::H2(u32bit& H, u32bit G, u32bit F, u32bit E, u32bit D,
               u32bit C, u32bit B, u32bit A, u32bit msg, u32bit magic)
   {
   H = rotate_right(((B&C)|D) ^ (D|F) ^ (D&((E&~A)^G)) ^ (A&C) ^ (B&E), 7) +
       rotate_right(H, 11) + msg + magic;
   }

/*************************************************
* HAVAL H3 Function                              *
*************************************************/
void HAVAL::H3(u32bit& H, u32bit G, u32bit F, u32bit E, u32bit D,
               u32bit C, u32bit B, u32bit A, u32bit msg, u32bit magic)
   {
   H = rotate_right((E & ((B & D) ^ C ^ F)) ^ (A & B) ^ (D & G) ^ F, 7) +
       rotate_right(H, 11) + msg + magic;
   }

/*************************************************
* HAVAL H4 Function                              *
*************************************************/
void HAVAL::H4(u32bit& H, u32bit G, u32bit F, u32bit E, u32bit D,
               u32bit C, u32bit B, u32bit A, u32bit msg, u32bit magic)
   {
   H = rotate_right((((~A & F) ^ (B | C) ^ E ^ G) & D) ^
                    (((A & E) ^ B ^ F) & C) ^ (A & B) ^ G, 7) +
       rotate_right(H, 11) + msg + magic;
   }

/*************************************************
* HAVAL H5 Function                              *
*************************************************/
void HAVAL::H5(u32bit& H, u32bit G, u32bit F, u32bit E, u32bit D,
               u32bit C, u32bit B, u32bit A, u32bit msg, u32bit magic)
   {
   H = rotate_right((((B & E & G) ^ A) & D) ^ ((B ^ E) & F) ^ (C & G) ^ B, 7) +
       rotate_right(H, 11) + msg + magic;
   }

/*************************************************
* Update a HAVAL Hash                            *
*************************************************/
void HAVAL::update_hash(const byte input[], u32bit length)
   {
   count += length;
   buffer.copy(position, input, length);
   if(position + length >= BLOCKSIZE)
      {
      hash(buffer);
      input += (BLOCKSIZE - position);
      length -= (BLOCKSIZE - position);
      while(length >= BLOCKSIZE)
         {
         hash(input);
         input += BLOCKSIZE;
         length -= BLOCKSIZE;
         }
      buffer.copy(input, length);
      position = 0;
      }
   position += length;
   }

/*************************************************
* Finalize a HAVAL Hash                          *
*************************************************/
void HAVAL::final(byte output[])
   {
   buffer[position] = 0x01;
   for(u32bit j = position+1; j != BLOCKSIZE; j++)
      buffer[j] = 0;
   if(position >= BLOCKSIZE - 10)
      { hash(buffer); buffer.clear(); }
   buffer[118] = (byte)((((HASHLENGTH << 3) & 3) << 6) |
                         (PASS << 3) | VERSION);
   buffer[119] = (byte)(HASHLENGTH << 1);
   for(u32bit j = BLOCKSIZE - 8; j != BLOCKSIZE; j++)
      buffer[j] = get_byte(7 - (j % 8), 8 * count);
   hash(buffer);
   tailor_digest(HASHLENGTH);
   for(u32bit j = 0; j != HASHLENGTH; j++)
      output[j] = get_byte(3 - (j % 4), digest[j/4]);
   clear();
   }

/*************************************************
* Tailor the digest to a specific size           *
*************************************************/
void HAVAL::tailor_digest(u32bit len)
   {
   switch(len)
      {
      case 16:
         {
         digest[0] += rotate_right((digest[7] & 0x000000FF) |
                                   (digest[6] & 0xFF000000) |
                                   (digest[5] & 0x00FF0000) |
                                   (digest[4] & 0x0000FF00),  8);
         digest[1] += rotate_right((digest[7] & 0x0000FF00) |
                                   (digest[6] & 0x000000FF) |
                                   (digest[5] & 0xFF000000) |
                                   (digest[4] & 0x00FF0000), 16);
         digest[2] += rotate_right((digest[7] & 0x00FF0000) |
                                   (digest[6] & 0x0000FF00) |
                                   (digest[5] & 0x000000FF) |
                                   (digest[4] & 0xFF000000), 24);
         digest[3] += (digest[7] & 0xFF000000) | (digest[6] & 0x00FF0000) |
                      (digest[5] & 0x0000FF00) | (digest[4] & 0x000000FF);
         break;
         }
      case 20:
         {
         digest[0] += rotate_right((digest[7] & ((u32bit)0x3F      )) |
                                   (digest[6] & ((u32bit)0x7F << 25)) |
                                   (digest[5] & ((u32bit)0x3F << 19)), 19);
         digest[1] += rotate_right((digest[7] & ((u32bit)0x3F <<  6)) |
                                   (digest[6] & ((u32bit)0x3F      )) |
                                   (digest[5] & ((u32bit)0x7F << 25)), 25);
         digest[2] += ((digest[7] & ((u32bit)0x7F << 12)) |
                       (digest[6] & ((u32bit)0x3F <<  6)) |
                       (digest[5] & ((u32bit)0x3F     )));
         digest[3] += ((digest[7] & ((u32bit)0x3F << 19)) |
                       (digest[6] & ((u32bit)0x7F << 12)) |
                       (digest[5] & ((u32bit)0x3F <<  6))) >> 6;
         digest[4] += ((digest[7] & ((u32bit)0x7F << 25)) |
                       (digest[6] & ((u32bit)0x3F << 19)) |
                       (digest[5] & ((u32bit)0x7F << 12))) >> 12;
         break;
         }
      case 24:
         {
         digest[0] += rotate_right((digest[7] & ((u32bit)0x1F      )) |
                                   (digest[6] & ((u32bit)0x3F << 26)), 26);
         digest[1] += (digest[7] & ((u32bit)0x1F <<  5)) |
                      (digest[6] & ((u32bit)0x1F      ));
         digest[2] += ((digest[7] & ((u32bit)0x3F << 10)) |
                       (digest[6] & ((u32bit)0x1F <<  5))) >> 5;
         digest[3] += ((digest[7] & ((u32bit)0x1F << 16)) |
                       (digest[6] & ((u32bit)0x3F << 10))) >> 10;
         digest[4] += ((digest[7] & ((u32bit)0x1F << 21)) |
                       (digest[6] & ((u32bit)0x1F << 16))) >> 16;
         digest[5] += ((digest[7] & ((u32bit)0x3F << 26)) |
                       (digest[6] & ((u32bit)0x1F << 21))) >> 21;
         break;
         }
      case 28:
         {
         digest[0] += (digest[7] >> 27) & 0x1F;
         digest[1] += (digest[7] >> 22) & 0x1F;
         digest[2] += (digest[7] >> 18) & 0x0F;
         digest[3] += (digest[7] >> 13) & 0x1F;
         digest[4] += (digest[7] >>  9) & 0x0F;
         digest[5] += (digest[7] >>  4) & 0x1F;
         digest[6] += (digest[7]      ) & 0x0F;
         break;
         }
      }
   }

/*************************************************
* Clear memory of class member data              *
*************************************************/
void HAVAL::clear() throw()
   {
   buffer.clear();
   digest[0] = 0x243F6A88;
   digest[1] = 0x85A308D3;
   digest[2] = 0x13198A2E;
   digest[3] = 0x03707344;
   digest[4] = 0xA4093822;
   digest[5] = 0x299F31D0;
   digest[6] = 0x082EFA98;
   digest[7] = 0xEC4E6C89;
   count = position = 0;
   }

}
