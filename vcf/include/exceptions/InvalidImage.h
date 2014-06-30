// InvalidImage.h

#ifndef _INVALIDIMAGE_H__
#define _INVALIDIMAGE_H__



namespace VCF
{

class GRAPHICSKIT_API InvalidImage : public BasicException  
{
public:

	InvalidImage(const VCF::String & message):
	  VCF::BasicException( message ){};

	InvalidImage():
		VCF::BasicException( INVALID_IMAGE ){};

	InvalidImage( const VCF::String & message, const int lineNumber ):
		VCF::BasicException(message, lineNumber){};

	virtual ~InvalidImage(){};

};

};

#endif 
 
