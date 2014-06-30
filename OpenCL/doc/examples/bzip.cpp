/*
An OpenCL example application which emulates a poorly written version of bzip2

Written by Jack Lloyd (lloyd@acm.jhu.edu), Jun 9, 2001

This file is in the public domain
*/
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>
#include <opencl/config.h>

#ifdef OPENCL_EXT_BZIP2
#include <opencl/bzip2.h>
#else
#error "You didn't compile the bzip module into OpenCL"
#endif

const std::string SUFFIX = ".bz2";

int main(int argc, char* argv[])
{
   if(argc < 2)
      {
      std::cout << "Usage: bzip [-s] [-d] [-1...9] <filenames>" << std::endl;
      return 1;
      }

   std::vector<std::string> files;
   bool decompress = false, small = false;
   int level = 9;

   for(int j = 1; argv[j] != 0; j++)
      {
      if(std::strcmp(argv[j], "-d") == 0) { decompress = true; continue; }
      if(std::strcmp(argv[j], "-s") == 0) { small = true; continue; }
      if(std::strcmp(argv[j], "-1") == 0) { level = 1; continue; }
      if(std::strcmp(argv[j], "-2") == 0) { level = 2; continue; }
      if(std::strcmp(argv[j], "-3") == 0) { level = 3; continue; }
      if(std::strcmp(argv[j], "-4") == 0) { level = 4; continue; }
      if(std::strcmp(argv[j], "-5") == 0) { level = 5; continue; }
      if(std::strcmp(argv[j], "-6") == 0) { level = 6; continue; }
      if(std::strcmp(argv[j], "-7") == 0) { level = 7; continue; }
      if(std::strcmp(argv[j], "-8") == 0) { level = 8; continue; }
      if(std::strcmp(argv[j], "-9") == 0) { level = 9; continue; }
      files.push_back(argv[j]);
      }

   try {

   OpenCL::Filter* bzip;
   if(decompress)
       bzip = new OpenCL::Bzip_Decompress(small);
   else
      bzip = new OpenCL::Bzip_Compress(level);

   OpenCL::Pipe pipe(bzip);

   for(unsigned int j = 0; j != files.size(); j++)
      {
      std::string infile = files[j], outfile = files[j];
      if(!decompress)
         outfile = outfile += SUFFIX;
      else
         outfile = outfile.replace(outfile.find(SUFFIX), SUFFIX.length(), "");

      std::ifstream in(infile.c_str());
      std::ofstream out(outfile.c_str());
      if(!in)
         {
         std::cout << "ERROR: could not read " << infile << std::endl;
         continue;
         }
      if(!out)
         {
         std::cout << "ERROR: could not write " << outfile << std::endl;
         continue;
         }

      in >> pipe;
      pipe.close();
      out << pipe;

      in.close();
      out.close();
      }
   return 0;

   }
   catch(OpenCL::Exception& e)
      {
      std::cout << "Exception caught: " << e.what() << std::endl;
      return 1;
      }
   catch(std::exception& e)
      {
      std::cout << "Standard library exception caught: "
                << e.what() << std::endl;
      return 1;
      }
   catch(...)
      {
      std::cout << "Unknown exception caught." << std::endl;
      return 1;
      }
}
