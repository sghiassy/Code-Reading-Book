/*************************************************
* S2K Base Class Header File                     *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_S2K_H__
#define OPENCL_S2K_H__

#include <string>
#include <opencl/symkey.h>

namespace OpenCL {

class S2K
   {
   public:
      virtual SymmetricKey derive(const std::string&, u32bit) const = 0;

      bool can_change_iterations() const { return variable_iterations; }
      u32bit iterations() const { return iter; }
      void set_iterations(u32bit);

      virtual void change_salt(const byte[], u32bit);
      void change_salt(const SecureVector<byte>&);
      void change_salt(RandomNumberGenerator&, u32bit);

      SecureVector<byte> current_salt() const { return salt; }

      S2K(bool var_iter) : variable_iterations(var_iter) { iter = 1; }
      virtual ~S2K() {}
   protected:
      SecureVector<byte> salt;
      u32bit iter;
   private:
      const bool variable_iterations;
   };

}

#endif
