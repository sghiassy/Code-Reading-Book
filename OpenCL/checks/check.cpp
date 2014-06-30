/*
 * Test Driver for OpenCL
 * This file is in the public domain; to claim copyright on a piece of
 * code this bad would be shameful.
 */

#include <vector>
#include <string>

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <opencl/exceptn.h>
#include <opencl/version.h>

/* Not on by default as many compilers (including egcs and gcc 2.95.x)
 * do not have the C++ <limits> header.
 */
//#define TEST_TYPES

#if defined(TEST_TYPES)
   #include <limits>
   #include <opencl/types.h>
   using namespace OpenCL;
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
const std::string VALIDATION_FILE = "checks\\validate.dat";
#else
const std::string VALIDATION_FILE = "checks/validate.dat";
#endif

void benchmark(const std::string&, bool html, double seconds);
void bench_algo(const std::string&);
void validate(const std::string&);
void print_help(double);

int main(int argc, char* argv[])
   {
   bool html = false; /* default to text output */
   double seconds = 1.5;

   std::vector<std::string> args;
   for(int j = 1; j != argc; j++)
      args.push_back(argv[j]);

   if(!args.size()) { print_help(seconds); return 2; }

   for(unsigned int j = 0; j != args.size(); j++)
      {
      if(args[j] == "--help") { print_help(seconds); return 1; }
      if(args[j] == "--html") html = true;
      if(args[j] == "--bench-algo")
         {
         if(j != args.size() - 1)
            bench_algo(args[j+1]);
         else
            {
            std::cout << "Option --bench-algo needs an argument\n";
            return 2;
            }
         }
      if(args[j] == "--seconds")
         {
         if(j != args.size() - 1) /* another arg remains */
            {
            seconds = std::strtod(args[j+1].c_str(), 0);
            /* sanity check; we allow zero for testing porpoises */
            if((seconds < 0.1 || seconds > 30) && seconds != 0)
               {
               std::cout << "Invalid argument to --seconds\n";
               return 2;
               }
            }
         else
            {
            std::cout << "Option --seconds needs an argument\n";
            return 2;
            }
         }
      }

   for(unsigned int j = 0; j != args.size(); j++)
     {
     if(args[j] == "--validate")     validate(VALIDATION_FILE);
     if(args[j] == "--benchmark")    benchmark("All", html, seconds);
     if(args[j] == "--bench-all")    benchmark("All", html, seconds);
     if(args[j] == "--bench-block")  benchmark("Block Cipher", html, seconds);
     if(args[j] == "--bench-mode")   benchmark("Cipher Mode", html, seconds);
     if(args[j] == "--bench-stream") benchmark("Stream Cipher", html, seconds);
     if(args[j] == "--bench-hash")   benchmark("Hash", html, seconds);
     if(args[j] == "--bench-mac")    benchmark("MAC", html, seconds);
     if(args[j] == "--bench-rng")    benchmark("RNG", html, seconds);
     }

   //   print_help(seconds);

   return 0;
   }

void print_help(double seconds)
   {
   std::cout << OpenCL::version_string() << " test driver" << std::endl
      << "Usage:\n"
      << "   --validate: Check test vectors\n"
      << "   --benchmark: Benchmark everything\n"
      << "   --bench-{block,mode,stream,hash,mac,rng}: Benchmark only this\n"
      << "   --html: Produce HTML output for benchmarks\n"
      << "   --seconds n: Benchmark for n seconds (default is "
      <<       seconds << ")\n"
      << "   --help: Print this message\n";
   }

void validate(const std::string& validation_file)
   {
   void test_types();
   int do_validation_tests(const std::string&);

   std::cout << "Beginning validation tests..." << std::endl;
   test_types();
   int errors = 0;
   try
      {
      errors = do_validation_tests(validation_file);
      }
   catch(OpenCL::Exception& e)
      {
      std::cout << "Exception caught: " << e.what() << std::endl;
      std::exit(1);
      }
   catch(std::exception& e)
      {
      std::cout << "Standard library exception caught: "
                << e.what() << std::endl;
      std::exit(1);
      }
   catch(...)
      {
      std::cout << "Unknown exception caught." << std::endl;
      std::exit(1);
      }
   if(errors == 0)
      std::cout << "All tests passed!" << std::endl;
   else
      std::cout << errors << " test"  << ((errors == 1) ? "" : "s")
                << " failed." << std::endl;
   std::exit(errors ? 1 : 0);
   }

#if defined(TEST_TYPES)
template<typename T>
bool test(const char* type, int digits, bool is_signed)
   {
   bool passed = true;
   if(std::numeric_limits<T>::is_specialized == false)
      {
      std::cout << "WARNING: Could not check parameters of " << type
                << " in std::numeric_limits" << std::endl;
      return true;
      }

   if(std::numeric_limits<T>::digits != digits && digits != 0)
      {
      std::cout << "ERROR: numeric_limits<" << type << ">::digits != "
                << digits << std::endl;
      passed = false;
      }
   if(std::numeric_limits<T>::is_signed != is_signed)
      {
      std::cout << "ERROR: numeric_limits<" << type << ">::is_signed != "
                << std::boolalpha << is_signed << std::endl;
      passed = false;
      }
   if(std::numeric_limits<T>::is_integer == false)
      {
      std::cout << "ERROR: numeric_limits<" << type
                << ">::is_integer == false " << std::endl;
      passed = false;
      }
   return passed;
   }
#endif

void test_types()
   {
   bool passed = true;

#if defined(TEST_TYPES)
   passed = passed && test<byte  >("byte",    8, false);
   passed = passed && test<u16bit>("u16bit", 16, false);
   passed = passed && test<u32bit>("u32bit", 32, false);
   passed = passed && test<u64bit>("u64bit", 64, false);
   passed = passed && test<s32bit>("s32bit", 31,  true);
#endif

   if(!passed)
      {
      std::cout << "Important settings in config.h are wrong. Please fix "
                   "and recompile." << std::endl;
      std::exit(1);
      }
   }
