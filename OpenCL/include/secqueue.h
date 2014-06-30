/*************************************************
* SecureQueue Header File                        *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_SECURE_QUEUE_H__
#define OPENCL_SECURE_QUEUE_H__

#include <opencl/filtbase.h>

namespace OpenCL {

class SecureQueue : public Filter
   {
   public:
      void write(const byte[], u32bit);
      u32bit read(byte[], u32bit);
      u32bit size() const;
      void close() {}

      SecureQueue& operator=(const SecureQueue&);
      SecureQueue();
      SecureQueue(const SecureQueue&);
      ~SecureQueue() { destroy(); }
   private:
      void destroy();
      class SecureQueueNode* head;
      class SecureQueueNode* tail;
   };

}

#endif
