#include <string>
#include <opencl/types.h>
using namespace OpenCL;

struct algorithm
   {
      algorithm(const std::string& t, const std::string& n,
                u32bit k = 0, u32bit i = 0) :
         type(t), name(n), filtername(n), keylen(k), ivlen(i) {}
      algorithm(const std::string& t, const std::string& n,
                const std::string& f, u32bit k = 0, u32bit i = 0) :
         type(t), name(n), filtername(f), keylen(k), ivlen(i) {}
      std::string type, name, filtername;
      u32bit keylen, ivlen, weight;
   };

extern const std::string END = "END";

algorithm algorithms[] = {
   algorithm("Block Cipher", "Blowfish", 16),
   algorithm("Block Cipher", "CAST256", 16),
   algorithm("Block Cipher", "CAST5", 16),
   algorithm("Block Cipher", "CS-Cipher", 16),
   algorithm("Block Cipher", "DES", 8),
   algorithm("Block Cipher", "DESX", 24),
   algorithm("Block Cipher", "Triple-DES", 24),
   algorithm("Block Cipher", "GOST", 32),
   algorithm("Block Cipher", "IDEA", 16),
   algorithm("Block Cipher", "Lion<MD5,ISAAC>", 16),
   algorithm("Block Cipher", "Lion<SHA1,SEAL>", 20),
   algorithm("Block Cipher", "Luby-Rackoff<SHA1>", 16),
   algorithm("Block Cipher", "MISTY1", 16),
   algorithm("Block Cipher", "RC2", 16),
   algorithm("Block Cipher", "RC5 (r = 12)", "RC5-12", 16),
   algorithm("Block Cipher", "RC5 (r = 16)", "RC5-16", 16),
   algorithm("Block Cipher", "RC6", 32),
   algorithm("Block Cipher", "Rijndael [AES] (r = 10)", "Rijndael", 16),
   algorithm("Block Cipher", "Rijndael [AES] (r = 12)", "Rijndael", 24),
   algorithm("Block Cipher", "Rijndael [AES] (r = 14)", "Rijndael", 32),
   algorithm("Block Cipher", "SAFER-SK128", 16),
   algorithm("Block Cipher", "Serpent", 32),
   algorithm("Block Cipher", "SHARK", 16),
   algorithm("Block Cipher", "Skipjack", 10),
   algorithm("Block Cipher", "Square", 16),
   algorithm("Block Cipher", "TEA", 16),
   algorithm("Block Cipher", "ThreeWay", 12),
   algorithm("Block Cipher", "Twofish", 32),
   algorithm("Block Cipher", "XTEA", 16),

   algorithm("Cipher Mode", "CBC<DES>", "CBC_Encryption<DES,PKCS7>", 8, 8),
   algorithm("Cipher Mode", "CFB<DES>(8)", "CFB_Encryption<DES>(8)", 8, 8),
   algorithm("Cipher Mode", "CFB<DES>(4)", "CFB_Encryption<DES>(4)", 8, 8),
   algorithm("Cipher Mode", "CFB<DES>(2)", "CFB_Encryption<DES>(2)", 8, 8),
   algorithm("Cipher Mode", "CFB<DES>(1)", "CFB_Encryption<DES>(1)", 8, 8),
   algorithm("Cipher Mode", "OFB<DES>", 8, 8),
   algorithm("Cipher Mode", "Counter<DES>", 8, 8),

   algorithm("Stream Cipher", "ARC4", 16),
   algorithm("Stream Cipher", "ISAAC", 16),
   algorithm("Stream Cipher", "SEAL", "SEAL", 20),

   algorithm("Hash", "Adler32"),
   algorithm("Hash", "CRC24"),
   algorithm("Hash", "CRC32"),
   algorithm("Hash", "HAS-V", "HASV-320"),
   algorithm("Hash", "HAVAL", "HAVAL-256"),
   algorithm("Hash", "MD2"),
   algorithm("Hash", "MD4"),
   algorithm("Hash", "MD5"),
   algorithm("Hash", "RIPE-MD128"),
   algorithm("Hash", "RIPE-MD160"),
   algorithm("Hash", "SHA-1"),
   algorithm("Hash", "SHA2-256"),
   algorithm("Hash", "SHA2-512"),
   algorithm("Hash", "Tiger"),

   algorithm("MAC", "EMAC<Square>", 16),
   algorithm("MAC", "HMAC-SHA1", 16),
   algorithm("MAC", "MD5-MAC", 16),

   algorithm("RNG", "Randpool"),
   algorithm("RNG", "X917<Square>"),

   algorithm(END, END)
};
