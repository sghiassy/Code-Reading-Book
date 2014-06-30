//

//InvalidPeer.h

#ifndef INVALID_PEER_H__
#define INVALID_PEER_H__



namespace VCF
{

class InvalidPeer : public BasicException{
public:

	InvalidPeer( const String & message ):
	  BasicException( message ){};

	InvalidPeer():
		BasicException( NO_PEER ){};

	InvalidPeer( const String & message, const int lineNumber ):
		BasicException(message, lineNumber){};

	virtual ~InvalidPeer(){};
};


};


#endif