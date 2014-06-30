//

//InvalidImplementer.h

#ifndef INVALID_IMPLEMENTER_H__
#define INVALID_IMPLEMENTER_H__

#include "BasicException.h"

namespace VCF
{

class InvalidImplementer : public BasicException{
public:

	InvalidImplementer( const VCF::String & message ):
	  VCF::BasicException( message ){};

	InvalidImplementer():
		VCF::BasicException( NO_IMPLEMENTER ){};

	InvalidImplementer( const VCF::String & message, const int lineNumber ):
		VCF::BasicException(message, lineNumber){};

	virtual ~InvalidImplementer(){};
};


};


#endif