/*
An OpenCL example application which emulates a
poorly written version of "uuencode -m"

Written by Jack Lloyd (lloyd@acm.jhu.edu), in maybe an hour scattered
over 2000/2001

This file is in the public domain
*/
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <opencl/encoder.h>

int main(int argc, char* argv[])
   {
   if(argc < 2)
      {
      std::cout << "Usage: " << argv[0] << " [-w] [-c n] [-e|-d] files...\n"
                   "   -e  : Encode input to Base64 strings (default) \n"
                   "   -d  : Decode Base64 input\n"
                   "   -w  : Wrap lines\n"
                   "   -c n: Wrap lines at column n, default 78\n";
      return 1;
      }

   int column = 78;
   bool wrap = false;
   bool encoding = true;
   std::vector<std::string> files;

   for(int j = 1; argv[j] != 0; j++)
      {
      if(std::strcmp(argv[j], "-w") == 0)
         wrap = true;
      else if(std::strcmp(argv[j], "-e") == 0);
      else if(std::strcmp(argv[j], "-d") == 0)
         encoding = false;
      else if(std::strcmp(argv[j], "-c") == 0)
         {
         if(argv[j+1])
            { column = atoi(argv[j+1]); j++; }
         else
            {
            std::cout << "No argument for -c option" << std::endl;
            return 1;
            }
         }
      else files.push_back(argv[j]);
      }

   for(unsigned int j = 0; j != files.size(); j++)
      {
      std::istream* stream;
      if(files[j] == "-") stream = &std::cin;
      else                stream = new std::ifstream(files[j].c_str());

      if(!*stream)
         {
         std::cout << "ERROR, couldn't open " << files[j] << std::endl;
         continue;
         }

      OpenCL::Pipe pipe((encoding) ?
                 ((OpenCL::Filter*) new OpenCL::Base64Encoder(wrap, column)) :
                 ((OpenCL::Filter*) new OpenCL::Base64Decoder));
      *stream >> pipe;
      pipe.close();
      std::cout << pipe;
      if(files[j] != "-") delete stream;
      }
   return 0;
   }
