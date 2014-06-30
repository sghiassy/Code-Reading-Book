/*
Written by Jack Lloyd (lloyd@acm.jhu.edu), on Prickle-Prickle,
the 10th of Bureaucracy, 3167. 

This file is in the public domain

This is just like the normal MD5 application, but it offers "proof" that the
iostreams library is really slow. This version (using gcc 3.0, Linux/x86) is
about 4.25 times faster than the iostreams version, because it uses the "bare"
Unix I/O system calls. The executable is also about half the size.

Nicely enough, doing the change required changing only 3 lines of code.

Note that this requires you to be on a machine running some sort of Unix. Well,
I guess just a POSIX.1 compliant OS, actually. 
*/

#include <iostream>
#include <opencl/md5.h>
#include <opencl/encoder.h>
#include <opencl/filters.h>

#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
   {
   if(argc < 2)
      {
      std::cout << "Usage: md5 <filenames>" << std::endl;
      return 1;
      }

   OpenCL::Pipe pipe(new OpenCL::HashFilter<OpenCL::MD5>,
                     new OpenCL::HexEncoder);

   for(int j = 1; argv[j] != 0; j++)
      {
      int file = open(argv[j], O_RDONLY);
      if(file == -1)
         {
         std::cout << "ERROR: could not open " << argv[j] << std::endl;
         continue;
         }
      file >> pipe;
      close(file);
      pipe.close();
      std::cout << pipe << "  " << argv[j] << std::endl;
      }
   return 0;
   }
