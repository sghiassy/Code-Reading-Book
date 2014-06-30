#include <stdio.h>
/* This file is in the public domain */

#include <iostream>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <string>
#include <exception>

#include <opencl/filters.h>
using namespace OpenCL_types;

extern OpenCL::RandomNumberGenerator& rng;

struct algorithm
   {
      algorithm(const std::string& t, const std::string& n,
                u32bit k = 0, u32bit i = 0) :
         type(t), name(n), filtername(n), keylen(k), ivlen(i) {}
      algorithm(const std::string& t, const std::string& n,
                const std::string& f, u32bit k = 0, u32bit i = 0) :
         type(t), name(n), filtername(f), keylen(k), ivlen(i) {}
      std::string type, name, filtername;
      u32bit keylen, ivlen, weight;
   };

extern const std::string END;
extern algorithm algorithms[];

/* Discard output to reduce overhead */
struct BitBucket : public OpenCL::Filter
   {
   void write(const byte[], u32bit) {}
   };

OpenCL::Filter* lookup(const std::string&, const std::string&,
                       const std::string&);

double bench_filter(std::string name, OpenCL::Filter* filter,
                    bool html, double seconds)
   {
   OpenCL::Pipe pipe(filter, new BitBucket);

   static const u32bit BUFFERSIZE = OpenCL::DEFAULT_BUFFERSIZE;
   byte buf[BUFFERSIZE];

   rng.randomize(buf, BUFFERSIZE);

   u32bit iterations = 0;
   std::clock_t start = std::clock(), clocks_used = 0;

   while(clocks_used < seconds * CLOCKS_PER_SEC)
      {
      iterations++;
      pipe.write(buf, BUFFERSIZE);
      clocks_used = std::clock() - start;
      }

   double bytes_per_sec = ((double)iterations * BUFFERSIZE) /
                          ((double)clocks_used / CLOCKS_PER_SEC);
   double mbytes_per_sec = bytes_per_sec / (1024.0 * 1024.0);

   std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
   std::cout.precision(2);
   if(html)
      {
      if(name.find("<") != std::string::npos)
         name = name.replace(name.find("<"), 1, "&lt;");
      if(name.find(">") != std::string::npos)
         name = name.replace(name.find(">"), 1, "&gt;");
      std::cout << "   <TR><TH>" << name
                << std::string(25 - name.length(), ' ') << "   <TH>";
      std::cout.width(6);
      std::cout << mbytes_per_sec << std::endl;
      }
   else
      {
      std::cout << name << ": " << std::string(25 - name.length(), ' ');
      std::cout.width(6);
      std::cout << mbytes_per_sec << " Mbytes/sec" << std::endl;
      }
   return (mbytes_per_sec);
   }

double bench(const std::string& name, const std::string& filtername, bool html,
             double seconds, u32bit keylen, u32bit ivlen)
   {
   OpenCL::Filter* filter = lookup(filtername, std::string(int(2*keylen), 'A'),
                                               std::string(int(2*ivlen), 'A'));
   if(!filter) return 0;

   return bench_filter(name, filter, html, seconds);
   }

void benchmark(const std::string& what, bool html, double seconds)
   {
   try {
      if(html)
         {
         std::cout << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD "
                   << "HTML 4.0 Transitional//EN\">\n"
                   << "<HTML>\n\n"
                   << "<TITLE>OpenCL Benchmarks</TITLE>\n\n"
                   << "<BODY>\n\n"
                   << "<P><TABLE BORDER CELLSPACING=1>\n"
                   << "<THEAD>\n"
                   << "<TR><TH>Algorithm                      "
                   << "<TH>Mbytes / second\n"
                   << "<TBODY>\n";
         }

      double sum = 0;
      u32bit how_many = 0;
      for(u32bit j = 0; algorithms[j].type != END; j++)
         if(what == "All" || what == algorithms[j].type)
            {
            double speed = bench(algorithms[j].name, algorithms[j].filtername,
                                 html, seconds, algorithms[j].keylen,
                                 algorithms[j].ivlen);
            if(speed > .00001) /* log(0) == -inf -> messed up average */
               sum += std::log(speed);
            how_many++;
            }

      if(html)
         std::cout << "</TABLE>\n\n";

      double average = std::exp(sum / (double)how_many);

      if(what == "All" && html)
         std::cout << "\n<P>Overall speed average: " << average
                   << "\n\n";
      else if(what == "All")
          std::cout << "\nOverall speed average: " << average
                    << std::endl;

      if(html) std::cout << "</BODY></HTML>\n";
      }
   catch(OpenCL::Exception& e)
      {
      std::cout << "OpenCL exception caught: " << e.what() << std::endl;
      std::exit(1);
      }
   catch(std::exception& e)
      {
      std::cout << "Standard library exception caught: " << e.what()
                << std::endl;
      std::exit(1);
      }
   catch(...)
      {
      std::cout << "Unknown exception caught." << std::endl;
      std::exit(1);
      }
   }

/* Oh my god I can't believe I'm writing such crappy code in here */
void bench_algo(const std::string& name)
   {
   try {
   for(u32bit j = 0; algorithms[j].type != END; j++)
      {
      if(algorithms[j].name == name)
         {
         bench(algorithms[j].name, algorithms[j].filtername, false, 3.0,
               algorithms[j].keylen, algorithms[j].ivlen);
         return;
         }
      }
      }
   catch(OpenCL::Exception& e)
      {
      std::cout << "OpenCL exception caught: " << e.what() << std::endl;
      std::exit(1);
      }
   catch(std::exception& e)
      {
      std::cout << "Standard library exception caught: " << e.what()
                << std::endl;
      std::exit(1);
      }
   catch(...)
      {
      std::cout << "Unknown exception caught." << std::endl;
      std::exit(1);
      }
   }
