////FileSearching.cpp

#include "FoundationKit.h"

using namespace VCF;

int main( int argc, char** argv ){

	initFoundationKit();


	Directory c_dir( "C:\\" );

	Directory::Finder* finder = c_dir.findFiles( "*.txt;*.shity" );

	while ( true == finder->hasMoreElements() ) {
		String s = finder->nextElement();

		printf( "file found is \"%s\"\n", s.c_str() );
	}

	delete finder;


	terminateFoundationKit();

	return 0;
}










