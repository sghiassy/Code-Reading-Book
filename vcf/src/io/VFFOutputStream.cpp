//VFFOutputStream.cpp

#include "ApplicationKit.h"
#include "VFFOutputStream.h"


using namespace VCF;

String VFFOutputStream::getTabString()
{
	String result = "";
	for ( int i=0;i<this->m_tabLevel;i++) {
		result += "\t";
	}
	return result;
}

String VFFOutputStream::binToHex( Persistable* persistableObject )
{
	String result = "";
	
	BasicOutputStream bos;
	bos << persistableObject;

	VCFChar* buffer = bos.getBuffer();	
	VCFChar* tmpBuffer = buffer;
	long size = 0;//bos.getSize();
	long bufSize = bos.getSize();
	long inc =  sizeof( VCFChar );
	VCFChar hexBytes[3];
	memset( hexBytes, 0, 3 );
	//this is slow, we might want to figure out a better way !
	while ( size < bufSize ) {
		uchar hexVal = *((uchar*)tmpBuffer);
		sprintf( hexBytes, "%02X", hexVal );
		result += hexBytes;
		tmpBuffer++;
		size +=sizeof( VCFChar );
	}
	size = result.size();
	return result;
}

void VFFOutputStream::writeObject( Object* object, const String& objectPropertyName )
{
	String s;
	String tabString;
	
	Persistable* persistable = dynamic_cast<Persistable*>(object);
	if ( NULL != persistable ) {
		String hexString = binToHex( persistable );
		tabString = getTabString();
		m_tabLevel ++;
		String tabString2 = getTabString();
		s = tabString + objectPropertyName + " = {\n" + tabString2 + hexString + "\n" + tabString + "}\n";
		m_tabLevel --;
		m_stream->write( s );
	}
	else { //write out the properties
		Class* clazz = object->getClass();
		if ( NULL != clazz ) {
			Enumerator<Property*>* props = clazz->getProperties();
			if ( NULL != props ) {
				tabString = getTabString();
				while ( true == props->hasMoreElements() ) {
					Property* prop = props->nextElement();
					if ( NULL != prop ) {		
						VariantData* value = prop->get();
						if ( PROP_OBJECT == value->type ) {
							Object* obj = (Object*)(*value);
							if ( NULL != obj ) {
								writeObject( obj, prop->getName() );							
							}
							else {
								s = tabString + prop->getName() + " = null\n";
								this->m_stream->write( s );
							}
						}
						else if ( PROP_STRING == value->type ) {
							s = tabString + objectPropertyName + "." + prop->getName() + " = '" + value->toString() + "'\n";
							this->m_stream->write( s );			
						}
						else {
							s = tabString + objectPropertyName + "." + prop->getName() + " = " + value->toString() + "\n";
							this->m_stream->write( s );			
						}
					}
				}
			}
		}
	}	
}

void VFFOutputStream::writeComponent( Component* component )
{
	Class* clazz = component->getClass();
	String tabString = getTabString();
	if ( NULL != clazz ) {
		String className = clazz->getClassName();
		String s = tabString + "object " + component->getName() + " : " + className + ", \'" + clazz->getID() + "\'\n";
		this->m_stream->write( s );
		Enumerator<Property*>* props = clazz->getProperties();
		if ( NULL != props ) {
			m_tabLevel ++;
			tabString = getTabString();
			while ( true == props->hasMoreElements() ) {
				Property* prop = props->nextElement();
				if ( NULL != prop ) {
					VariantData* value = prop->get();
					if ( PROP_OBJECT == prop->getType() ) {						
						Object* obj = (Object*)(*value);
						if ( NULL != obj ) {
							Component* subComponent = dynamic_cast<Component*>(obj);
							if ( NULL != subComponent ) {
								//this is a reference to a component - 
								//we don't want to write it out here other wise 
								//it will be written out twice
								s  = tabString + prop->getName() + " = @" + subComponent->getName() + "\n";
								this->m_stream->write( s );
							}
							else {
								writeObject( obj, prop->getName() );							
							}
						}
						else {
							s = tabString + prop->getName() + " = null\n";
							this->m_stream->write( s );
						}
					}
					else if ( PROP_STRING == value->type ) {
						s = tabString + prop->getName() + " = '" + value->toString() + "'\n";
						this->m_stream->write( s );			
					}
					else {
						s = tabString + prop->getName() + " = " + value->toString() + "\n";
						this->m_stream->write( s );
					}
				}
			}
			m_tabLevel --;
		}
		
		Container* container = dynamic_cast<Container*>( component );
		if ( NULL != container ) {
			Enumerator<Control*>* children = container->getChildren();
			if ( NULL != children ) {
				m_tabLevel ++;
				while ( true == children->hasMoreElements() ) {
					Control* child = children->nextElement();
					writeComponent( child );
				}
				m_tabLevel --;
			}
		}

		Enumerator<Component*>* components = component->getComponents();
		if ( NULL != components ) {
			m_tabLevel ++;
			while ( true == components->hasMoreElements() ) {
				Component* childComponent = components->nextElement();				
				writeComponent( childComponent );
			}
			m_tabLevel --;
		}

		tabString = getTabString();
		s = tabString + "end\n";
		this->m_stream->write( s );
	}
}

