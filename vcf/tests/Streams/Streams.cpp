////Streams.cpp

#include "FoundationKit.h"
#include <iostream>

using namespace VCF;

using namespace std;

ostream& operator <<( ostream& lhs, OutputStream* os )
{
	lhs.write( os->getBuffer(), os->getSize() );
	return lhs;
}

istream& operator >>( istream& lhs, InputStream* is )
{
	int sz = lhs.gcount();

	int tg = lhs.tellg();
	char buf[256];
	while ( ! lhs.eof() ) {
		memset( buf, 0, 256 );
		lhs.get( buf, 256 );
		sz = lhs.gcount();
	}

	cout << "sz = " << sz << endl;
	//is->read( lhs.get(

	return lhs;
}

OutputStream& operator << ( OutputStream& lhs, ostream& os ) 
{

	return lhs;
}

int main( int argc, char** argv ){

	initFoundationKit();
	
	cout << "Hello World" << endl;

	BasicOutputStream bos;
	bos << String("Hello from BasicOutputStream") ;

	cout << &bos;

	BasicInputStream bis;
	
	String s;
	cin >> s >> &bis;

	return 0;
}










