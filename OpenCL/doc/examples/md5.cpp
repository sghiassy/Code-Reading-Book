/*
An OpenCL example application which emulates a poorly written version of md5sum
(though it is significantly faster than md5sum, at least on my machine).

Interestingly enough, most of the execution time seems to be taken by the
iostreams library. Versions using stdio(3) and Unix file I/O are 3.7 and 4.2
times faster, respectively, for a 25 megabyte file. Compare the speeds of
md5 and md5_fd and see. 

Written by Jack Lloyd (lloyd@acm.jhu.edu), on date or dates unknown

This file is in the public domain
*/

#include <iostream>
#include <fstream>
#include <opencl/md5.h>
#include <opencl/encoder.h>
#include <opencl/filters.h>

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
      std::ifstream file(argv[j]);
      if(!file)
         {
         std::cout << "ERROR: could not open " << argv[j] << std::endl;
         continue;
         }
      file >> pipe;
      file.close();
      pipe.close();
      std::cout << pipe << "  " << argv[j] << std::endl;
      }
   return 0;
   }
