OpenCL, a Cryptographic Class Library in C++
Version 0.7.6: October 13, 2001

*NOTE*: OpenCL is currently in early beta. Thus, your comments about OpenCL
(most especially the application interface) are needed, and your comments can
have a direct effect on how OpenCL changes in the future.

OpenCL aims to be a portable, easy to use library for performing common
cryptographic tasks. OpenCL currently includes:

Block Ciphers: Blowfish, CAST256, CAST5, CS-Cipher, DES, DESX, TripleDES, GOST,
   IDEA, Lion, Luby-Rackoff, MISTY1, RC2, RC5, RC6, Rijndael, SAFER-SK128,
   Serpent, SHARK, Skipjack, Square, TEA, ThreeWay, Twofish, XTEA
Block Cipher Modes: CBC, CFB, OFB, and Counter
Stream Ciphers: ISAAC, ARC4, SEAL
Hash Functions: HAVAL, MD2, MD4, MD5, RIPE-MD128, RIPE-MD160, SHA-1, SHA2-256,
   SHA2-512, Tiger
Checksums: Adler32, CRC24, CRC32
MACs: EMAC, HMAC, MD5MAC
RNGs: Randpool, X9.17 RNG

OpenCL also includes a large number of system dependent modules that you can
compile into OpenCL, at your option, to provide additional functionality.

For information on building OpenCL, read the section "Building the Library" in
doc/opencl.ps. Read doc/relnotes.txt for notes related to issues specific to
this release. The license is in doc/license.txt, and the ChangeLog is in
doc/log.txt.

Check http://opencl.sourceforge.net for news and new versions. If you have any
questions, comments, or requests, send email.

Thanks,

Jack Lloyd (lloyd@acm.jhu.edu)
