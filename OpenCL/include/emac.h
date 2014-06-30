/*************************************************
* EMAC Header File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_EMAC_H__
#define OPENCL_EMAC_H__

#include <opencl/opencl.h>

namespace OpenCL {

template<typename B>
class EMAC : public MessageAuthCode
   {
   public:
      static std::string name() { return "EMAC<" + B::name() + ">"; }
      static const u32bit MACLENGTH = B::BLOCKSIZE, KEYLENGTH = B::KEYLENGTH;
      void final(byte[MACLENGTH]);
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw()
         { f1.clear(); f2.clear(); state.clear(); position = 0; }
      EMAC() : MessageAuthCode(name(), MACLENGTH, KEYLENGTH) { position = 0; }
   private:
      void update_mac(const byte[], u32bit);
      B f1, f2;
      SecureBuffer<byte, MACLENGTH> state;
      u32bit position;
   };

/*************************************************
* Update an EMAC Calculation                     *
*************************************************/
template<typename B>
void EMAC<B>::update_mac(const byte input[], u32bit length)
   {
   while(length)
      {
      u32bit xored = std::min(MACLENGTH - position, length);
      xor_buf(state + position, input, xored);
      input += xored;
      length -= xored;
      position += xored;
      if(position == MACLENGTH)
         {
         f1.encrypt(state);
         position = 0;
         }
      }
   }

/*************************************************
* Finalize an EMAC Calculation                   *
*************************************************/
template<typename B>
void EMAC<B>::final(byte mac[MACLENGTH])
   {
   for(u32bit j = position; j != MACLENGTH; j++)
      state[j] ^= (byte)(MACLENGTH-position);
   f1.encrypt(state);
   f2.encrypt(state, mac);
   state.clear();
   position = 0;
   }

/*************************************************
* EMAC Key Setup                                 *
*************************************************/
template<typename B>
void EMAC<B>::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   SecureBuffer<byte, 2*KEYLENGTH> subkeys;
   SecureBuffer<byte, MACLENGTH> block, counter;
   u32bit generated = 0;
   f1.set_key(key, length);
   while(generated < 2*KEYLENGTH)
      {
      f1.encrypt(counter, block);
      subkeys.copy(generated, block, MACLENGTH);
      generated += MACLENGTH;
      for(s32bit j = MACLENGTH - 1; j >= 0; j--)
         if(++counter[j])
            break;
      }
   f1.set_key(subkeys, KEYLENGTH);
   f2.set_key(subkeys + KEYLENGTH, KEYLENGTH);
   }

}

#endif
