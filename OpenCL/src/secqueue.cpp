/*************************************************
* SecureQueue Source File                        *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/secqueue.h>

namespace OpenCL {

/*************************************************
* SecureQueueNode                                *
*************************************************/
class SecureQueueNode
   {
   public:
      u32bit write(const byte input[], u32bit length)
         {
         u32bit copied = std::min(length, BUFFERSIZE - end);
         copy_mem(buffer + end, input, copied);
         end += copied;
         return copied;
         }
      u32bit read(byte output[], u32bit length)
         {
         u32bit copied = std::min(length, end - start);
         copy_mem(output, buffer + start, copied);
         start += copied;
         return copied;
         }
      u32bit size() const { return (end - start); }
      SecureQueueNode()  { next = 0; start = end = 0; }
      ~SecureQueueNode() { next = 0; start = end = 0; }
   private:
      friend class SecureQueue;
      static const u32bit BUFFERSIZE = DEFAULT_BUFFERSIZE;
      SecureQueueNode* next;
      SecureBuffer<byte, BUFFERSIZE> buffer;
      u32bit start, end;
   };

/*************************************************
* Create a SecureQueue                           *
*************************************************/
SecureQueue::SecureQueue() : Filter(0)
   {
   head = tail = new SecureQueueNode;
   }

/*************************************************
* Create a SecureQueue                           *
*************************************************/
SecureQueue::SecureQueue(const SecureQueue& input) : Filter(0)
   {
   head = tail = new SecureQueueNode;
   SecureQueueNode* temp = input.head;
   while(temp)
      {
      write(temp->buffer + temp->start, temp->end - temp->start);
      temp = temp->next;
      }
   }

/*************************************************
* Destroy this SecureQueue                       *
*************************************************/
void SecureQueue::destroy()
   {
   SecureQueueNode* temp = head;
   while(temp)
      {
      SecureQueueNode* holder = temp->next;
      delete temp;
      temp = holder;
      }
   }

/*************************************************
* Copy a SecureQueue                             *
*************************************************/
SecureQueue& SecureQueue::operator=(const SecureQueue& input)
   {
   destroy();
   head = tail = new SecureQueueNode;
   SecureQueueNode* temp = input.head;
   while(temp)
      {
      write(temp->buffer + temp->start, temp->end - temp->start);
      temp = temp->next;
      }
   return *this;
   }

/*************************************************
* Add some items to the queue                    *
*************************************************/
void SecureQueue::write(const byte input[], u32bit length)
   {
   while(length)
      {
      u32bit temp = tail->write(input, length);
      input += temp;
      length -= temp;
      if(length)
         {
         tail->next = new SecureQueueNode;
         tail = tail->next;
         }
      }
   }

/*************************************************
* Read some items from the queue                 *
*************************************************/
u32bit SecureQueue::read(byte output[], u32bit length)
   {
   u32bit got = 0;
   while(length && head)
      {
      u32bit temp = head->read(output, length);
      output += temp;
      got += temp;
      length -= temp;
      if(head->size() == 0)
         {
         SecureQueueNode* holder = head->next;
         delete head;
         head = holder;
         }
      }
   if(!head)
      head = tail = new SecureQueueNode;
   return got;
   }

/*************************************************
* Return how many items the queue holds          *
*************************************************/
u32bit SecureQueue::size() const
   {
   SecureQueueNode* current = head;
   u32bit count = 0;
   while(current)
      {
      count += current->size();
      current = current->next;
      }
   return count;
   }

}
