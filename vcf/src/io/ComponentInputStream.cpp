// ComponentInputStream.cpp
#include "ApplicationKit.h"
#include "VCFChar.h"
#include "ComponentInputStream.h"

#include <util/XMLUni.hpp>
#include <sax/AttributeList.hpp>
#include <framework/MemBufInputSource.hpp>
#include <util/PlatformUtils.hpp>
#include<util/XMLUniDefs.hpp>
#include <parsers/SAXParser.hpp>




using namespace VCF;
using namespace std;

ComponentInputStream::ComponentInputStream(  InputStream* inStream  )
{
	this->init();
	this->m_inStream = inStream;
	m_doEscapes = false;	
}

ComponentInputStream::~ComponentInputStream()
{
	this->m_collectionPropertyMap.clear();
	this->m_containerPropertyMap.clear();
	this->m_objectPropertyMap.clear();
}

void ComponentInputStream::init()
{
	this->m_component = NULL;	
	this->m_currentContainer = NULL;
	this->m_prevContainer = NULL;
	this->m_inStream = NULL;
	m_objectPropertyLevel = 0;
	m_collectionPropertyLevel = 0;
	m_containerLevel = 0;
	m_currentObjectProperty = NULL;
	m_topLevelControl = NULL;
	m_topLevelComponentAlreadyExists = false;
}


void ComponentInputStream::seek(const unsigned long& offset, const SeekType& offsetFrom)
{
	
}

unsigned long ComponentInputStream::getSize()
{
	if ( NULL != m_inStream ){
		return m_inStream->getSize();
	}
	return 0;
}

char* ComponentInputStream::getBuffer()
{
	return NULL;
}

void ComponentInputStream::read( char* bytesToRead, unsigned long sizeOfBytes )
{
	if ( NULL != m_inStream ){
		m_inStream->read( bytesToRead, sizeOfBytes );
	}
}

void ComponentInputStream::error( const SAXParseException& e )
{
	std::cout << "\nError at (file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "): " << StrX(e.getMessage()) << endl;
}

void ComponentInputStream::fatalError( const SAXParseException& e )
{
    cout << "\nFatal Error at (file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "): " << StrX(e.getMessage()) << endl;
}

void ComponentInputStream::warning( const SAXParseException& e )
{
    cout << "\nWarning at (file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "): " << StrX(e.getMessage()) << endl;
}

void ComponentInputStream::unparsedEntityDecl(const XMLCh* const name, const XMLCh* const publicId, 
											  const XMLCh* const systemId, const XMLCh* const notationName )
{
    // Not used at this time
}


void ComponentInputStream::notationDecl( const XMLCh* const name, const XMLCh* const publicId, 
										 const XMLCh* const systemId )
{
    // Not used at this time
}

void ComponentInputStream::characters(const XMLCh* const chars, const unsigned int length )
{
    // Transcode to UTF-8 for portable display
    StrX tmpXCode(chars);
    const char* tmpText = tmpXCode.localForm();

    //
    //  If we were asked to escape special chars, then do that. Else just
    //  display it as normal text.
    //
    if (m_doEscapes)
    {
        for (unsigned int index = 0; index < length; index++)
        {
            switch (tmpText[index])
            {
            case chAmpersand :
                cout << "&amp;";
                break;

            case chOpenAngle :
                cout << "&lt;";
                break;

            case chCloseAngle:
                cout << "&gt;";
                break;

            case chDoubleQuote :
                cout << "&quot;";
                break;

            default:
                cout << tmpText[index];
                break;
            }
        }
    }
     else
    {
        cout << tmpText;
    }
}


void ComponentInputStream::endDocument()
{
}

void ComponentInputStream::endElement( const XMLCh* const name )
{
    cout << "</" << StrX(name) << ">";
	String tokenName = StrX(name).localForm();
	switch ( getTokenFromString( tokenName ) ){
		case COMP_TOKEN_UNKNOWN:{

		}
		break;

		case COMP_TOKEN_OBJECT:{
			
		}
		break;

		case COMP_TOKEN_CHILDREN:{
			this->m_containerPropertyMap.erase( m_containerLevel );			
			m_containerLevel --;
			std::map<int,Container*>::iterator found = m_containerPropertyMap.find( m_containerLevel );
			if ( found != m_containerPropertyMap.end() ){
				m_currentContainer = found->second;
			}
		}
		break;

		case COMP_TOKEN_PROPERTY:{

		}
		break;

		case COMP_TOKEN_OBJECT_PROPERTY:{
			this->m_objectPropertyMap.erase( m_objectPropertyLevel );			
			m_currentObjectProperty = NULL;
			m_objectPropertyLevel --;
			std::map<int,Object*>::iterator found = m_objectPropertyMap.find( m_objectPropertyLevel );
			if ( found != m_objectPropertyMap.end() ){
				m_currentObjectProperty = found->second;
			}
		}
		break;

		case COMP_TOKEN_CLASSID:{
			
		}
		break;
	}
}

void ComponentInputStream::ignorableWhitespace( const XMLCh* const chars, const unsigned int length )
{
    
}

void ComponentInputStream::processingInstruction(const XMLCh* const target, const XMLCh* const data )
{
    
}

void ComponentInputStream::startDocument()
{

}

void ComponentInputStream::loadProperties( Object* object, AttributeList&  attributes )
{
	String propertyName = "";
	String propValue = "";
	Property* property = NULL;
	Class* clazz = NULL;
	clazz = object->getClass();
	if ( NULL != clazz ){
		int index = 0;
		unsigned int propCount = attributes.getLength();
		while ( index < propCount ) {
			propertyName = StrX( attributes.getValue(index) ).localForm();
			if ( clazz->hasProperty( propertyName ) ){
				property = clazz->getProperty( propertyName );
				if ( NULL != property ){
					propValue = StrX( attributes.getValue(index+1) ).localForm();
					if ( true == property->isCollection() ){						
						ClassRegistry* registry = ClassRegistry::getClassRegistry();
						VariantData itemVal;
						Object* item = NULL;
						if ( NULL != registry ){									
							registry->createNewInstance( propValue, &item );
							itemVal = item;									
						}
						if ( NULL != item ){
							property->add( &itemVal );
							m_objectPropertyLevel ++;
							this->m_objectPropertyMap[ m_objectPropertyLevel ] = item;
						}
					}
					else if ( PROP_OBJECT != property->getType() ){
						if ( false == property->isReadOnly() ){
							property->set( propValue );
						}
					}
					else {
						if ( 0 != stricmp( "null", propValue.c_str() ) ){ // now we have a class name							
							VariantData* objVal = property->get();
							if ( NULL != objVal ){
								this->m_currentObjectProperty = *objVal;
								if ( NULL == m_currentObjectProperty ){//go ahead and create one
									ClassRegistry* registry = ClassRegistry::getClassRegistry();
									if ( NULL != registry ){									
										registry->createNewInstance( propValue, &m_currentObjectProperty );
										*objVal = m_currentObjectProperty;
										property->set( objVal );
									}
								}
								m_objectPropertyLevel ++;
								this->m_objectPropertyMap[ m_objectPropertyLevel ] = m_currentObjectProperty;								
								this->m_currentPropertyClassName = propValue;
							}							
						}
					}
				}
			}
			index += 2;
		}		
	}
	else{
		
		//throw exception					
	}
}

void ComponentInputStream::startElement(const XMLCh* const name, AttributeList&  attributes )
{
	String tokenName = StrX(name).localForm();
	
	unsigned int index = 0;
	switch ( getTokenFromString( tokenName ) ){
		case COMP_TOKEN_UNKNOWN:{
			
		}
		break;

		/**
		*Object token is found. The class name needs to be pulled off and then 
		*the class instantiated
		*/
		case COMP_TOKEN_OBJECT:{			
			ClassRegistry* registry = ClassRegistry::getClassRegistry();
			if ( NULL != registry ){
				bool topLevelComp = (NULL == m_component) ? true : false;
				
				String className = StrX( attributes.getValue(index) ).localForm();
				//this should only be true once
				if ( (true == this->m_topLevelComponentAlreadyExists) && (NULL == m_component) ){
					//can't compare classnames here have ot assume that the m_topLevelControl
					//passed into the readControl() method is OK and matches the className
					m_component = m_topLevelControl;
				}
				else{
					registry->createNewInstance( className, &m_component );
					if ( topLevelComp ){
						m_topLevelControl = dynamic_cast<Control*>( m_component );
					}
				}
				std::map<int,Container*>::iterator it = m_containerPropertyMap.find( m_containerLevel );
				if ( it != m_containerPropertyMap.end() ){				
					it->second->add( (Control*)m_component );
				}
			}
			else {
				//throw exception !
			}
		}
		break;

		case COMP_TOKEN_CHILDREN:{
			Container* tmp = dynamic_cast<Container*>(m_component);
			if ( NULL != tmp ){//means this m_component is a container				
				m_containerLevel ++;
				this->m_containerPropertyMap[ m_containerLevel ] = tmp;
			}
		}
		break;

		case COMP_TOKEN_PROPERTY:{			
			unsigned int propCount = attributes.getLength();
			/**
			*we two cases  - either we are editing the component ( m_component!=NULL ) or
			*we are editing a property of the Component (m_currentObjectProperty!=NULL)
			*/
			
			if ( (NULL != m_component) && (NULL == m_currentObjectProperty) ){				
				loadProperties( m_component, attributes );					
			}
			else {
				std::map<int,Object*>::iterator it = m_objectPropertyMap.find( m_objectPropertyLevel );
				if ( it != m_objectPropertyMap.end() ){	
					m_currentObjectProperty = it->second;
					loadProperties( m_currentObjectProperty, attributes );
					
				}
			}
		}
		break;

		case COMP_TOKEN_OBJECT_PROPERTY:{
			
		}
		break;

		case COMP_TOKEN_CLASSID:{
			
		}
		break;
	}
}

long ComponentInputStream::getTokenFromString( const String& string )
{
	long result = COMP_TOKEN_UNKNOWN;	
	
	if ( 0 == _stricmp( COMP_OBJECT, string.c_str() ) ){
		result = COMP_TOKEN_OBJECT;
	}
	else if ( 0 == _stricmp( COMP_CLASS, string.c_str() ) ){
		result = COMP_TOKEN_CLASS;
	}
	else if ( 0 == _stricmp( COMP_PROPERTY, string.c_str() ) ){
		result = COMP_TOKEN_PROPERTY;
	}
	else if ( 0 == _stricmp( COMP_NAME, string.c_str() ) ){
		result = COMP_TOKEN_NAME;
	}
	else if ( 0 == _stricmp( COMP_VALUE, string.c_str() ) ){
		result = COMP_TOKEN_VALUE;
	}
	else if ( 0 == _stricmp( COMP_CHILDREN, string.c_str() ) ){
		result = COMP_TOKEN_CHILDREN;
	}
	else if ( 0 == _stricmp( COMP_OBJECT_PROPERTY, string.c_str() ) ){
		result = COMP_TOKEN_OBJECT_PROPERTY;
	}
	else if ( 0 == _stricmp( COMP_CLASSID, string.c_str() ) ){
		result = COMP_TOKEN_CLASSID;
	}
	else if ( 0 == _stricmp( COMP_COMPONENTS, string.c_str() ) ){
		result = COMP_TOKEN_COMPONENTS;
	}

	return result;
}

void ComponentInputStream::readControl( Control** controlToReadInto )
{	
	m_topLevelComponentAlreadyExists = (*controlToReadInto != NULL) ? true : false;

	if ( true == m_topLevelComponentAlreadyExists ){
		m_topLevelControl = *controlToReadInto;
	}
	
	unsigned long size = this->getSize();
	char* bufData = NULL;
	try {		
		XMLPlatformUtils::Initialize();		
		bufData = new char[size];
		memset( bufData, 0, size );
		this->read( bufData, size );
		MemBufInputSource* buf = new MemBufInputSource( (const XMLByte*)bufData, size, "MEM_BUF", false );
		
		SAXParser parser;
		parser.setValidationScheme( SAXParser::Val_Auto );
		parser.setDoNamespaces(false);

		parser.setDocumentHandler( this );
		parser.setErrorHandler( this );

		parser.parse( *buf );

		XMLPlatformUtils::Terminate();
	}
	catch (const XMLException& toCatch){
		String s = StrX(toCatch.getMessage()).localForm();
		throw BasicException( "XML Error message: " +  s );
	}
	delete [] bufData;	

	if ( NULL != m_topLevelControl ){
		*controlToReadInto = m_topLevelControl;
	}
}