//ComponentNameManager.h
#include "ApplicationKit.h"
#include "ComponentNameManager.h"
#include "StringUtils.h"

using namespace VCF;

using namespace VCFBuilder;

ComponentNameManager ComponentNameManager::namemanagerInstance;

ComponentNameManager::ComponentNameManager()
{

}

ComponentNameManager::~ComponentNameManager()
{

}

String ComponentNameManager::getUniqueNameFromClassName( const String& className )
{
	String result = stripNamespace( className );
	
	std::map<String,int>::iterator found = m_nameMap.find( result );
	int count = 1;
	if ( found != m_nameMap.end() ) {
		count = found->second;
		count ++;
		m_nameMap[result] = count;
	}
	else {
		m_nameMap[result] = count;
	}
	result += StringUtils::toString( count );

	return result;
}

String ComponentNameManager::stripNamespace( const String& className )
{
	String result = className;
	int pos = result.find( "::" );
	while ( pos != String::npos ) {
		result.erase( 0, pos + 2 );
		pos = result.find( "::" );	
	}

	return result;
}