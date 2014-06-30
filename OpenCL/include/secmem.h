/*************************************************
* Secure Memory Buffers Header File              *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_SECURE_MEMORY_BUFFERS_H__
#define OPENCL_SECURE_MEMORY_BUFFERS_H__

#include <algorithm>
#include <opencl/util.h>
#include <opencl/secalloc.h>

namespace OpenCL {

/*************************************************
* Fixed Length Memory Buffer                     *
*************************************************/
template<typename T, u32bit L, typename A = SecureAllocator<T> >
struct SecureBuffer
   {
   u32bit size() const { return L; }

   operator T* () { return buf; }
   operator const T* () const { return buf; }

   T* ptr() { return buf; }
   const T* ptr() const { return buf; }

   T* end() { return (buf + size()); }
   const T* end() const { return (buf + size()); }

   SecureBuffer& operator=(const SecureBuffer<T,L>& in)
      { if(this != &in) copy(in, in.size()); return *this; }

   void copy(const T in[], u32bit n)
      { copy_mem(buf, in, std::min(size(), n)); }
   void copy(u32bit off, const T in[], u32bit n)
      { copy_mem(buf + off, in, std::min(size() - off, n)); }
   void set(const T& in, u32bit n = L)
      { for(u32bit j = 0; j != n; j++) buf[j] = in; }

   void clear() { A::clear(buf, size()); }

   SecureBuffer() { clear(); A::lock(buf, size()); }
   SecureBuffer(const SecureBuffer<T, L>& in)
      { clear(); A::lock(buf, size()); copy(in.buf, in.size()); }
   SecureBuffer(const T in[], u32bit n)
      { clear(); A::lock(buf, size()); copy(in, n); }

   ~SecureBuffer() { clear(); A::unlock(buf, size()); }

   T buf[L];
   };

/*************************************************
* Variable Length Secure Memory Buffer           *
*************************************************/
template<typename T, typename A = SecureAllocator<T> >
class SecureVector
   {
   public:
      u32bit size() const { return units; }

      operator T* () { return buf; }
      operator const T* () const { return buf; }

      T* ptr() { return buf; }
      const T* ptr() const { return buf; }

      T* end() { return (buf + size()); }
      const T* end() const { return (buf + size()); }

      SecureVector& operator=(const SecureVector<T>& in)
         { if(this != &in) resize_and_copy(in, in.size()); return *this; }

      void resize_and_copy(const T in[], u32bit n)
         { create(n); copy(in, n); }

      void copy(const T in[], u32bit n)
         { copy_mem(buf, in, std::min(size(), n)); }
      void copy(u32bit off, const T in[], u32bit n)
         { copy_mem(buf + off, in, std::min(size() - off, n)); }

      void set(const T& in, u32bit n)
         { create(n); for(u32bit j = 0; j != n; j++) buf[j] = in; }

      void clear() { A::clear(buf, units); }

      void create(u32bit);
      void grow_to(u32bit) const;
      void grow_by(u32bit n) { grow_to(n + size()); }

      void swap(SecureVector<T>&);

      SecureVector(u32bit n = 0) { init(); create(n); }
      SecureVector(const T in[], u32bit n) { init(); resize_and_copy(in, n); }
      SecureVector(const SecureVector<T>& in)
         { init(); resize_and_copy(in, in.size()); }
      template<u32bit L> SecureVector(const SecureBuffer<T,L>& in)
         { init(); resize_and_copy(in, in.size()); }
      ~SecureVector() { A::deallocate(buf, units); }
   private:
      // FIXME: make version of init that takes T[] and n
      void init() { buf = 0; units = 0; }
      mutable T* buf;
      mutable u32bit units;
   };

/*************************************************
* Create a new buffer                            *
*************************************************/
template<typename T, typename A>
void SecureVector<T,A>::create(u32bit n)
   {
   if(n == units) { clear(); return; }
   A::deallocate(buf, units);
   buf = A::allocate(n);
   units = n;
   }

/*************************************************
* Increase the size of the buffer                *
*************************************************/
template<typename T, typename A>
void SecureVector<T,A>::grow_to(u32bit n) const
   {
   if(n <= units) return;
   T* new_buf = A::allocate(n);
   copy_mem(new_buf, buf, units);
   A::deallocate(buf, units);
   buf = new_buf;
   units = n;
   }

/*************************************************
* Swap this buffer with another one              *
*************************************************/
template<typename T, typename A>
void SecureVector<T,A>::swap(SecureVector<T>& x)
   {
   std::swap(buf, x.buf);
   std::swap(units, x.units);
   }

}

#endif
