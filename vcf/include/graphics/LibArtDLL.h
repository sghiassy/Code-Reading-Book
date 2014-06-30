
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBARTDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBARTDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef WIN32
	#ifdef LIBART_DLL
		#ifdef LIBARTDLL_EXPORTS
			#define LIBARTDLL_API __declspec(dllexport)
		#else
			#define LIBARTDLL_API __declspec(dllimport)
		#endif
	#else
		#define LIBARTDLL_API	
	#endif
#else
	#define LIBARTDLL_API
#endif

