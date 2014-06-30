/*************************************************
* Utility Functions Header File                  *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_UTIL_H__
#define OPENCL_UTIL_H__

#include <string>
#include <cstring>
#include <opencl/config.h>

namespace OpenCL {

/*************************************************
* Rotation Functions                             *
*************************************************/
template<typename T> inline T rotate_left(T input, u32bit rot)
   { return (T)((input << rot) | (input >> (8*sizeof(T)-rot))); }

template<typename T> inline T rotate_right(T input, u32bit rot)
   { return (T)((input >> rot) | (input << (8*sizeof(T)-rot))); }

/*************************************************
* Byte Extraction Function                       *
*************************************************/
template<typename T> inline byte get_byte(u32bit byte_num, T input)
   { return (byte)(input >> ((sizeof(T)-1-(byte_num&(sizeof(T)-1))) << 3)); }

/*************************************************
* Byte to Word Conversions                       *
*************************************************/
inline u16bit make_u16bit(byte input0, byte input1)
   { return (u16bit)(((u16bit)input0 << 8) | input1); }

inline u32bit make_u32bit(byte input0, byte input1, byte input2, byte input3)
   { return (u32bit)(((u32bit)input0 << 24) | ((u32bit)input1 << 16) |
                     ((u32bit)input2 <<  8) | input3); }

inline u64bit make_u64bit(byte input0, byte input1, byte input2, byte input3,
                          byte input4, byte input5, byte input6, byte input7)
   {
   return (u64bit)(((u64bit)input0 << 56) | ((u64bit)input1 << 48) |
                   ((u64bit)input2 << 40) | ((u64bit)input3 << 32) |
                   ((u64bit)input4 << 24) | ((u64bit)input5 << 16) |
                   ((u64bit)input6 <<  8) | input7);
   }

/*************************************************
* Memory Manipulation Functions                  *
*************************************************/
template<typename T> inline void copy_mem(T* out, const T* in, u32bit n)
   { std::memcpy(out, in, sizeof(T)*n); }

template<typename T> inline void clear_mem(T* ptr, u32bit n)
   { std::memset(ptr, 0, sizeof(T)*n); }

void xor_buf(byte[], const byte[], u32bit);
void xor_buf(byte[], const byte[], const byte[], u32bit);

/*************************************************
* Byte Swapping Functions                        *
*************************************************/
u16bit reverse_bytes(u16bit);
u32bit reverse_bytes(u32bit);
u64bit reverse_bytes(u64bit);

/*************************************************
* System Functions                               *
*************************************************/
u64bit system_time();
u64bit system_clock();

void lock_mem(void*, u32bit);
void unlock_mem(void*, u32bit);

/*************************************************
* Misc Utility Functions                         *
*************************************************/
std::string to_string(u32bit);

}

#endif
