/*************************************************
* Filter Source File                             *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/filtbase.h>
#include <opencl/secqueue.h>

namespace OpenCL {

/*************************************************
* Filter Constructor                             *
*************************************************/
Filter::Filter(u32bit count) : port_count(count)
   {
   if(port_count) next = new Filter*[port_count];
   else           next = 0;
   for(u32bit j = 0; j != port_count; j++)
      next[j] = 0;
   port_num = 0;
   }

/*************************************************
* Send data to all ports                         *
*************************************************/
void Filter::send(const byte input[], u32bit length)
   {
   for(u32bit j = 0; j != total_ports(); j++)
      if(next[j])
         next[j]->write(input, length);
   }

/*************************************************
* Close a Filter                                 *
*************************************************/
void Filter::close()
   {
   final();
   for(u32bit j = 0; j != total_ports(); j++)
      if(next[j])
         next[j]->close();
   }

/*************************************************
* Attach a filter to the current port            *
*************************************************/
void Filter::attach(Filter* new_filter)
   {
   if(new_filter)
      {
      Filter* next_to_last = this, *last = this;
      while(last->get_next())
         {
         next_to_last = last;
         last = last->get_next();
         }
      if(dynamic_cast<SecureQueue*>(last))
         {
         SecureQueue queue = *static_cast<SecureQueue*>(last);
         delete last;
         add(queue, new_filter);
         next_to_last->next[next_to_last->current_port()] = new_filter;
         }
      else
         last->next[last->current_port()] = new_filter;
      }
   }

/*************************************************
* Attach the queue onto each leaf of filter      *
*************************************************/
void Filter::add(const SecureQueue& queue, Filter* filter)
   {
   for(u32bit j = 0; j != filter->total_ports(); j++)
      if(!filter->next[j])
         filter->next[j] = new SecureQueue(queue);
      else
         add(queue, filter->next[j]);
   }

/*************************************************
* Set the active port on a filter                *
*************************************************/
void Filter::set_port(u32bit new_port) throw(Invalid_Port_Number)
   {
   if(new_port >= total_ports())
      throw Invalid_Port_Number();
   port_num = new_port;
   }

/*************************************************
* Destroy this Filter and it's children          *
*************************************************/
void destroy(Filter*& to_kill)
   {
   if(!to_kill) return;
   for(u32bit j = 0; j != to_kill->total_ports(); j++)
      destroy(to_kill->next[j]);
   delete to_kill;
   to_kill = 0;
   }

/*************************************************
* Pipe Constructor                               *
*************************************************/
Pipe::Pipe(Filter* filter_array[], u32bit count)
   {
   init();
   for(u32bit j = 0; j != count; j++)
      attach(filter_array[j]);
   }

/*************************************************
* Initialize the Pipe                            *
*************************************************/
void Pipe::init()
   {
   pipe = new NullFilter;
   pipe->attach(new SecureQueue);
   }

/*************************************************
* Find the end of the pipe                       *
*************************************************/
SecureQueue* Pipe::end() const
   {
   Filter* current = pipe;
   while(current->get_next())
      current = current->get_next();
   SecureQueue* queue = static_cast<SecureQueue*>(current);
   return queue;
   }

/*************************************************
* Write a string into a Pipe                     *
*************************************************/
void Pipe::write(const std::string& str)
   {
   write((const byte*)str.c_str(), str.size());
   }

/*************************************************
* Read some data from the pipe                   *
*************************************************/
u32bit Pipe::read(byte output[], u32bit length)
   {
   return end()->read(output, length);
   }

/*************************************************
* Return all data in the pipe                    *
*************************************************/
SecureVector<byte> Pipe::read_all()
   {
   SecureVector<byte> buffer(remaining());
   read(buffer, buffer.size());
   return buffer;
   }

/*************************************************
* Return all data in the pipe as a string        *
*************************************************/
std::string Pipe::read_all_as_string()
   {
   static const u32bit BUFFERSIZE = DEFAULT_BUFFERSIZE;
   SecureBuffer<byte, BUFFERSIZE> buffer;
   std::string str;
   while(remaining())
      {
      u32bit got = read(buffer, BUFFERSIZE);
      str.append((char*)buffer.ptr(), got);
      }
   return str;
   }

/*************************************************
* Find out how many bytes are ready to read      *
*************************************************/
u32bit Pipe::remaining() const
   {
   return end()->size();
   }

/*************************************************
* Fork Constructor                               *
*************************************************/
Fork::Fork(Filter* filters[], u32bit count) : Filter(count)
   {
   for(u32bit j = 0; j != count; j++)
      next[j] = filters[j];
   }

/*************************************************
* Chain Constructor                              *
*************************************************/
Chain::Chain(Filter* filters[], u32bit count)
   {
   for(u32bit j = 0; j != count; j++)
      attach(filters[j]);
   }

}
