/*
Validation routines
This file is in the public domain
*/

#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <vector>

#include <opencl/encoder.h>
using namespace OpenCL_types;

extern OpenCL::RandomNumberGenerator& rng;

u32bit random_word(u32bit max)
   {
   u32bit r = 0;
   for(u32bit j = 0; j != 4; j++)
      r = (r << 8) | rng.random();
   return ((r % max) + 1); // return between 1 and max inclusive
   }

OpenCL::Filter* lookup(const std::string&, const std::string&,
                       const std::string&);

bool failed_test(const std::string&, const std::string&,
                 const std::string&, const std::string&,
                 const std::string&, bool);

std::vector<std::string> parse(const std::string&);
void strip(std::string&);

u32bit do_validation_tests(const std::string& filename)
   {
   std::ifstream test_data(filename.c_str());

   if(!test_data)
       {
       std::cout << "Couldn't open test file " << filename << std::endl;
       std::exit(1);
       }

   u32bit errors = 0, alg_count = 0;
   std::string algorithm;
   bool is_extension = false;

   while(!test_data.eof())
      {
      if(test_data.bad() || test_data.fail())
         {
         std::cout << "File I/O error." << std::endl;
         std::exit(1);
         }

      std::string line;
      std::getline(test_data, line);

      strip(line);
      if(line.size() == 0) continue;

      // Do line continuation
      while(line[line.size()-1] == '\\' && !test_data.eof())
         {
         line.replace(line.size()-1, 1, "");
         std::string nextline;
         std::getline(test_data, nextline);
         strip(nextline);
         if(nextline.size() == 0) continue;
         line += nextline;
         }

      if(line[0] == '[' && line[line.size() - 1] == ']')
         {
         const std::string ext_mark = "<EXTENSION>";
         algorithm = line.substr(1, line.size() - 2);
         is_extension = false;
         if(algorithm.find(ext_mark) != std::string::npos)
            {
            is_extension = true;
            algorithm.replace(algorithm.find(ext_mark),
                              ext_mark.length(), "");
            }
         alg_count = 0;
         continue;
         }

      std::vector<std::string> substr = parse(line);

      alg_count++;

      if(failed_test(algorithm, substr[0], substr[1], substr[2], substr[3],
                     is_extension))
         {
         std::cout << "ERROR: \"" << algorithm << "\" failed test #"
                   << alg_count << std::endl;
         errors++;
         }

      }
   return errors;
   }

std::vector<std::string> parse(const std::string& line)
   {
   const char DELIMITER = ':';
   std::vector<std::string> substr;
   std::string::size_type start = 0, end = line.find(DELIMITER);
   while(end != std::string::npos)
      {
      substr.push_back(line.substr(start, end-start));
      start = end+1;
      end = line.find(DELIMITER, start);
      }
   if(line.size() > start)
      substr.push_back(line.substr(start));
   while(substr.size() <= 4) // at least 4 substr, some possibly empty
      substr.push_back("");
   return substr;
   }

/* Strip comments, whitespace, etc */
void strip(std::string& line)
   {
   if(line.find('#') != std::string::npos)
      line = line.erase(line.find('#'), std::string::npos);

   while(line.find(' ') != std::string::npos)
      line = line.erase(line.find(' '), 1);

   while(line.find('\t') != std::string::npos)
      line = line.erase(line.find('\t'), 1);
   }

bool failed_test(const std::string& algo,     const std::string& in,
                 const std::string& expected, const std::string& key,
                 const std::string& iv, bool is_extension)
   {
   OpenCL::Filter* test = lookup(algo, key, iv);
   if(test == 0 && is_extension) return false;
   if(test == 0)
      {
      std::cout << "ERROR: \"" + algo + "\" is not a known algorithm name."
                << std::endl;
      std::exit(1);
      }

   OpenCL::Pipe pipe(new OpenCL::HexDecoder, test, new OpenCL::HexEncoder);

   // this can help catch errors with buffering, etc
   const byte* data = (const byte*)in.c_str();
   u32bit len = in.size();
   while(len)
      {
      u32bit how_much = random_word(len);
      pipe.write(data, how_much);
      data += how_much;
      len -= how_much;
      }
   pipe.close();

   std::string output = pipe.read_all_as_string();
   if(output != expected)
      {
      std::cout << "FAILED: " << expected << " != " << output << std::endl;
      return true;
      }
   return false;
   }
