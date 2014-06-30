//VFFInputStream.cpp

#include "ApplicationKit.h"
#include "VFFInputStream.h"
#include "Parser.h"

using namespace VCF;


VFFInputStream::VFFInputStream( InputStream* stream )
{
	m_atTopLevel = true;

	m_stream = stream;

	m_parser = new Parser( this );

	m_componentInputLevel = -1;
}


VFFInputStream::~VFFInputStream()
{
	delete m_parser;
	m_parser = NULL;
}

void VFFInputStream::getOuterClassNameAndUUID( String& className, String& UUID )
{
	className = "";
	UUID = "";
	
	m_parser->resetStream();

	if ( true == m_parser->tokenSymbolIs( "object" ) ) {		
		String currentSymbol;
		while ( (TO_EOF != m_parser->getToken()) && (false == m_parser->tokenSymbolIs( "end" )) ) {
			currentSymbol = m_parser->tokenString();
			try {				
				// do object
				VCFChar token = m_parser->nextToken();
				switch ( token ) {
					case ':' : {
						m_parser->nextToken();
						className = m_parser->tokenString();
						m_parser->nextToken();
						m_parser->checkToken( ',' );
						m_parser->nextToken();
						UUID = m_parser->tokenString();
					}
					break;
				}
			}

			catch ( BasicException& e ) {
				StringUtils::trace( e.getMessage() + "\n" );
				break;
			}
		}
	}

	m_parser->resetStream();
}

void VFFInputStream::readChildComponents( Component* component )
{
	String s;
	Control* control = NULL;
	Class* clazz = NULL;
	Container* controlContainer = NULL;

	Component* childComponent = NULL;
	if ( true == m_parser->tokenSymbolIs( "object" ) ) {		
		String currentSymbol;
		while ( (TO_EOF != m_parser->getToken()) && (false == m_parser->tokenSymbolIs( "end" )) ) {
			currentSymbol = m_parser->tokenString();
			try {				
				// do object				
				VCFChar token = m_parser->nextToken();
				switch ( token ) {
					case ':' : {
						controlContainer = dynamic_cast<Container*>( component );
						if ( NULL != controlContainer ) {
							childComponent = controlContainer->findControl( currentSymbol );
						}
						if ( childComponent == NULL ) {
							childComponent = component->findComponent( currentSymbol );
						}
						controlContainer = NULL;

						m_parser->nextToken();
						String className = m_parser->tokenString();
						m_parser->nextToken();
						m_parser->checkToken( ',' );
						m_parser->nextToken();
						String classID = m_parser->tokenString();
						
						if ( NULL != childComponent ) {							
							clazz = childComponent->getClass();
							if ( clazz->getID() != classID ) {
								m_parser->error( MAKE_ERROR_MSG_2("ClassID of passed in component doesn't match ClassID encountered in parsing stream") );
							}
							control = dynamic_cast<Control*>(childComponent);
							if ( NULL != control ) {
								controlContainer = dynamic_cast<Container*>( control );
							}
						}
						else {
							throw InvalidPointerException( MAKE_ERROR_MSG_2("component is NULL, can't write properties of stream to NULL component") );
						}
						m_parser->nextToken();
					}
					break;

					case '=' : case '.' : case '[' : {
						processAsignmentTokens( token, currentSymbol, clazz );
					}
					break;
				}

				while ( true == m_parser->tokenSymbolIs( "object" ) ) {
					
					readChildComponents( childComponent );						
					
					m_parser->nextToken();
				}
			}
			catch ( BasicException& e ) {
				StringUtils::trace( e.getMessage() + "\n" );
				break;
			}
		}
	}
}

void VFFInputStream::processAsignmentTokens( const VCFChar& token, const String& currentSymbol, Class* clazz )
{
	switch ( token ) {
		case '=' : {
			VCFChar assignmentToken = m_parser->nextToken();
			String value = m_parser->tokenString();
			switch ( assignmentToken ) {
				default : {
					Property* prop = clazz->getProperty( currentSymbol );
					if ( NULL != prop ) {
						//need to be a bit smarter here
						//look at the proeprty type. If the type is 
						//is PROP_OBJECT, then first assume that value
						//is actually a string that represents the name of
						//variable. Search for the value, if something is
						//found then pass this into the prop->set() call.
						//If nothing is found then go for it and pass
						//in the value as is.
						if ( (PROP_OBJECT == prop->getType()) && ('@' == value[0]) ) {
							//OK our value is actually a component name so we are
							//going to store it in a list for later, because we may not 
							//have an existing instance of the component yet. To store
							//this successfully we need the name of the property to set,
							//the actualy object instance that we are going to set the 
							//property on, and the name of the component to try and retreive
							//once we get to that point.	
							m_parser->nextToken();
							String tmp = m_parser->tokenString();
							m_deferredProperties.push_back( new DeferredPropertySetter( tmp, prop->getName(), prop->getSource() ) );
						}
						else {
							prop->set( value );
						}
					}
				}
				break;
				case '[': {
					while ( ']' != m_parser->nextToken() ) {
				
					}						
				}
				break;

				case '{': {
					//m_parser->nextToken();
					String binValue = m_parser->binHexToString();								
					m_parser->nextToken();
					m_parser->checkToken( '}' );
					Property* prop = clazz->getProperty( currentSymbol );
					if ( NULL != prop ) {
						VariantData* value = prop->get();
						if ( value->type == PROP_OBJECT ) {
							Object* obj = *value;
							if ( NULL != obj ) {
								Persistable* persistable = dynamic_cast<Persistable*>(obj);
								if ( NULL != persistable ) {
									hexToBin( binValue, persistable );
								}
							}
						}
					}								
				}
				break;
			}
			m_parser->nextToken();
		}
		break;

		case '.' : {
			m_parser->nextToken();
			String objPropName = m_parser->tokenString();
			m_parser->nextToken();
			m_parser->checkToken( '=' );
			m_parser->nextToken();
			String s = m_parser->tokenString();
			m_parser->nextToken();	

			Property* prop = clazz->getProperty( currentSymbol );
			VariantData* value = prop->get();
			if ( value->type == PROP_OBJECT ) {
				Object* obj = *value;
				Class* objClass = obj->getClass();
				Property* objProp = objClass->getProperty( objPropName );
				if ( NULL != objProp ) {
					objProp->set( s );
				}
			}
			else {
				this->m_parser->errorStr( "Object type expected" );
			}						
		}
		break;

		case '[' : {
			
		}
		break;
	}
}

void VFFInputStream::readComponentInstance( Component* component )
{
	m_componentInputLevel = 0;
	String s;
	Control* control = NULL;
	Class* clazz = NULL;
	Container* controlContainer = NULL;
	if ( true == m_parser->tokenSymbolIs( "object" ) ) {		
		String currentSymbol;
		while ( (TO_EOF != m_parser->getToken()) && (false == m_parser->tokenSymbolIs( "end" )) ) {
			currentSymbol = m_parser->tokenString();
			try {				
				// do object
				VCFChar token = m_parser->nextToken();
				switch ( token ) {
					case ':' : {
						m_parser->nextToken();
						String className = m_parser->tokenString();
						m_parser->nextToken();
						m_parser->checkToken( ',' );
						m_parser->nextToken();
						String classID = m_parser->tokenString();
						
						if ( NULL != component ) {							
							clazz = component->getClass();
							if ( clazz->getID() != classID ) {
								m_parser->error( MAKE_ERROR_MSG_2("ClassID of passed in component doesn't match ClassID encountered in parsing stream") );
							}
							control = dynamic_cast<Control*>(component);
							if ( NULL != control ) {
								controlContainer = dynamic_cast<Container*>(control );
							}
						}
						else {
							throw InvalidPointerException( MAKE_ERROR_MSG_2("component is NULL, can't write properties of stream to NULL component") );
						}
						m_parser->nextToken();
					}
					break;

					case '=' : case '.' : case '[' : {
						processAsignmentTokens( token, currentSymbol, clazz );
					}
					break;
				}

				while ( true == m_parser->tokenSymbolIs( "object" ) ) {					
					readChildComponents( component );					
					m_parser->nextToken();
				}
			}
			catch ( BasicException& e ) {
				StringUtils::trace( e.getMessage() + "\n" );
				break;
			}
		}
	}

	if ( 0 == m_componentInputLevel ) {
		std::vector<DeferredPropertySetter*>::iterator it = m_deferredProperties.begin();
		while ( it != m_deferredProperties.end() ) {
			DeferredPropertySetter* dps = *it;
			Class* clazz = dps->m_source->getClass();
			Property* prop = clazz->getProperty( dps->m_propertyName );
			if ( NULL != prop ) {
				Component* foundComponent = NULL;				
				if ( true == component->bindVariable( &foundComponent, dps->m_propertyVal ) ) {
					VariantData data;
					data = foundComponent;
					prop->set( &data );
				}				
			}
			delete dps;
			dps = NULL;
			it ++;
		}
		m_deferredProperties.clear();
	}
	m_componentInputLevel = -1;
}

void VFFInputStream::readComponent( Component** component )
{	
	m_componentInputLevel ++;
	String s;
	Control* control = NULL;
	Class* clazz = NULL;
	Container* controlContainer = NULL;
	if ( true == m_parser->tokenSymbolIs( "object" ) ) {		
		String currentSymbol;
		while ( (TO_EOF != m_parser->getToken()) && (false == m_parser->tokenSymbolIs( "end" )) ) {
			currentSymbol = m_parser->tokenString();
			try {				
				// do object
				VCFChar token = m_parser->nextToken();
				switch ( token ) {
					case ':' : {
						m_parser->nextToken();
						String className = m_parser->tokenString();
						m_parser->nextToken();
						m_parser->checkToken( ',' );
						m_parser->nextToken();
						String classID = m_parser->tokenString();
						ClassRegistry::getClassRegistry()->createNewInstance( className,  (Object**)component );
						if ( NULL != *component ) {							
							clazz = (*component)->getClass();
							control = dynamic_cast<Control*>(*component);
							if ( NULL != control ) {
								controlContainer = dynamic_cast<Container*>( control );
							}
						}
						else {
							m_parser->error( "Unable to instantiate an object of type \"" + className + "\"" );
						}
						m_parser->nextToken();
					}
					break;

					case '=' : case '.' : case '[' : {
						processAsignmentTokens( token, currentSymbol, clazz );
					}
					break;
				}

				while ( true == m_parser->tokenSymbolIs( "object" ) ) {
					Component* newComponent = NULL;
					readComponent( &newComponent );
					if ( NULL != newComponent ) {
						//control
						Control* newControl = dynamic_cast<Control*>(newComponent);
						if ( (NULL != newControl) && (NULL != controlContainer) ) {
							controlContainer->add( newControl, newControl->getAlignment() );
						}
						else {
							(*component)->addComponent( newComponent );
						}
					}
					m_parser->nextToken();
				}
			}
			catch ( BasicException& e ) {
				StringUtils::trace( e.getMessage() + "\n" );
				break;
			}
		}
	}

	if ( 0 == m_componentInputLevel ) {
		std::vector<DeferredPropertySetter*>::iterator it = m_deferredProperties.begin();
		while ( it != m_deferredProperties.end() ) {
			DeferredPropertySetter* dps = *it;
			Class* clazz = dps->m_source->getClass();
			Property* prop = clazz->getProperty( dps->m_propertyName );
			if ( NULL != prop ) {
				Component* foundComponent = NULL;				
				if ( true == (*component)->bindVariable( &foundComponent, dps->m_propertyVal ) ) {
					VariantData data;
					data = foundComponent;
					prop->set( &data );
				}				
			}
			delete dps;
			dps = NULL;
			it ++;
		}
		m_deferredProperties.clear();
	}
	m_componentInputLevel --;
}

void VFFInputStream::hexToBin( const String& hexString, Persistable* persistableObject )
{
	long hexSize = hexString.size();
	long binSize = hexSize / 2; 
	VCFChar* binBuffer = new VCFChar[binSize];	
	
	const VCFChar* hexStringBuf = hexString.c_str();

	memset( binBuffer, 0, binSize );
	VCFChar tmpHexBuf[3];
	memset( tmpHexBuf, 0, 3 );
	VCFChar* tmpBinBuffer = binBuffer;
	
	//THis is Icky code !!!!
	while ( binSize > 0 ) {		
		
		tmpHexBuf[0] = hexStringBuf[0];	
		tmpHexBuf[1] = hexStringBuf[1];
		uchar hex = '\0';
		sscanf( tmpHexBuf, "%X", &hex );
		*tmpBinBuffer = hex;		
		hexStringBuf += 2;			
		binSize --;
		tmpBinBuffer ++;
	}
	BasicInputStream bis( binBuffer, hexSize / 2 );
	persistableObject->loadFromStream( &bis );
	delete [] binBuffer;
}