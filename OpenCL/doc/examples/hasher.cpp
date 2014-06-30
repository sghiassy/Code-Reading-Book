/*
An OpenCL example application which emulates a
poorly written version of "gpg --print-md"

Written by Jack Lloyd (lloyd@acm.jhu.edu), quite a while ago (as of June 2001)

This file is in the public domain
*/
#include <fstream>
#include <iostream>
#include <string>
#include <opencl/md5.h>
#include <opencl/sha1.h>
#include <opencl/rmd160.h>
#include <opencl/encoder.h>

int main(int argc, char* argv[])
{
   if(argc < 2)
      {
      std::cout << "Usage: hasher <filenames>" << std::endl;
      return 1;
      }

   const int COUNT = 3;
   OpenCL::Filter* hash[COUNT] = {
      new OpenCL::Chain(new OpenCL::HashFilter<OpenCL::MD5>,
                        new OpenCL::HexEncoder),
      new OpenCL::Chain(new OpenCL::HashFilter<OpenCL::SHA1>,
                        new OpenCL::HexEncoder),
      new OpenCL::Chain(new OpenCL::HashFilter<OpenCL::RIPEMD160>,
                        new OpenCL::HexEncoder) };
   std::string name[COUNT] = { "MD5", "SHA-1", "RIPE-MD160" };

   OpenCL::Fork* fork = new OpenCL::Fork(hash, COUNT);
   OpenCL::Pipe pipe(fork);

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
      for(int k = 0; k != COUNT; k++)
         {
         fork->set_port(k);
         std::cout << name[k] << "(" << argv[j] << ") = " << pipe << std::endl;
         }
      }
   return 0;
}
