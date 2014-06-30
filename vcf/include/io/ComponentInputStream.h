// ComponentInputStream.h

#ifndef _COMPONENTINPUTSTREAM_H__
#define _COMPONENTINPUTSTREAM_H__

#include "Stream.h"
#include <string>
#include <iostream>
#include <stdlib.h>
#include <sax/HandlerBase.hpp>
#include <map>

namespace VCF {

#define COMP_TOKEN_UNKNOWN				-1
#define COMP_TOKEN_OBJECT				0
#define COMP_TOKEN_CLASS				1
#define COMP_TOKEN_PROPERTY				2
#define COMP_TOKEN_NAME					3
#define COMP_TOKEN_VALUE				4
#define COMP_TOKEN_CHILDREN				5
#define COMP_TOKEN_OBJECT_PROPERTY		6
#define COMP_TOKEN_CLASSID				7
#define COMP_TOKEN_COMPONENTS			8

#define COMP_OBJECT				"object"
#define COMP_CLASS				"class"
#define COMP_PROPERTY			"property"
#define COMP_NAME				"name"
#define COMP_VALUE				"value"
#define COMP_CHILDREN			"children"
#define COMP_OBJECT_PROPERTY	"objectproperty"
#define COMP_CLASSID			"classid"
#define COMP_COMPONENTS			"components"



class Component;

class Container;

class Property;

class StrX;

class Control;
/**
*This stream is used to load a component from a stream. It is currently
based on the XML format. The DTD for components is extremely simple and is 
*as follows:
*the component is declared as an "object" and can have 0 or more "property"s, or 
*0 or more "object"s nested inside. A "property" must have a "name", which is a 
*string, and a "value" which is a string representation of the property value.
*Basic types (int, double, et. al. ) are converted using the standard sscanf function
*and formatting specs (see CommonDefines.h for the exact formatting strings used). 
*Objects are a bit different
*
*Because the XML specifiaction is used, ComponentInputStream derives also from HandlerBase
*which is a class in the Xerces-C library (see http://xml.apache.org/index.html). HandlerBase 
*provides the basic handling of the XML format. The 
*/

class APPKIT_API ComponentInputStream : public InputStream, public HandlerBase
{
public:
	ComponentInputStream( InputStream* inStream );

	virtual ~ComponentInputStream();

	virtual void seek(const unsigned long& offset, const SeekType& offsetFrom);

	virtual unsigned long getSize();

	virtual char* getBuffer();

	virtual void read( char* bytesToRead, unsigned long sizeOfBytes );

	/**
	*returns the top elevel control found in the stream with all
	*of its children
	*/
	void readControl( Control** controlToReadInto );
	/**
    *Implementations of the SAX DocumentHandler interface
    */
    void endDocument();

    void endElement( const XMLCh* const name );

    void characters( const XMLCh* const chars, const unsigned int length );

    void ignorableWhitespace( const XMLCh* const chars, const unsigned int length );

    void processingInstruction( const XMLCh* const target, const XMLCh* const data );

    void startDocument();

    void startElement( const XMLCh* const name, AttributeList& attributes );

	/**
    *Implementations of the SAX ErrorHandler interface
    */
    void warning( const SAXParseException& exception );

    void error( const SAXParseException& exception );

    void fatalError( const SAXParseException& exception );

    /**
    *Implementation of the SAX DTDHandler interface
    */
    void notationDecl( const XMLCh* const name, const XMLCh* const publicId, 
		               const XMLCh* const systemId );

    void unparsedEntityDecl( const XMLCh* const name, const XMLCh* const publicId, 
		                     const XMLCh* const systemId, const XMLCh* const notationName );

	void init();

	
private:
	InputStream* m_inStream;
	bool m_doEscapes;
	
	long getTokenFromString( const String& string );

	Object* m_component;	
	Container* m_prevContainer;
	Container* m_currentContainer;	
	String m_currentPropertyClassName;
	Object* m_currentObjectProperty;
	Control* m_topLevelControl;
	int m_objectPropertyLevel;	
	int m_collectionPropertyLevel;	
	Property* m_collectionProperty;
	std::map<int, Object*> m_objectPropertyMap;
	std::map<int, Property*> m_collectionPropertyMap;
	std::map<int, Container*> m_containerPropertyMap;
	int m_containerLevel;
	
	bool m_topLevelComponentAlreadyExists;
	void loadProperties( Object* object, AttributeList&  attributes );
};


/**
*This is a simple class that lets us do easy (though not terribly efficient)
*trancoding of XMLCh data to local code page for display.
*
*original source lifted from "Xerces" SaxPrint sample
*
*@author Jim Crafton
*@version 1.0
*/
class StrX {
public :
    
    StrX( const XMLCh* const toTranscode ) {
        // Call the private transcoding method
        m_localForm = XMLString::transcode(toTranscode);
    };

    virtual ~StrX(){
        delete [] m_localForm;
    }
    
    const char* localForm() const
    {
        return m_localForm;
    }

private :
    /**
    *This is the local code page form of the string.
    */
    char*   m_localForm;
};

inline std::ostream& operator<<( std::ostream& target, const StrX& toDump )
{
    target << toDump.localForm();
    return target;
};

inline OutputStream& operator<<( OutputStream& target, const StrX& toDump)
{
    target << String( toDump.localForm() );
    return target;
};


};

#endif 