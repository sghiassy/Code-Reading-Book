//ComponentNameManager.h

#ifndef _COMPONENTNAMEMANAGER_H__
#define _COMPONENTNAMEMANAGER_H__



using namespace VCF;


namespace VCFBuilder  {

/**
*Class ComponentNameManager documentation
*/
class ComponentNameManager : public VCF::Object { 
public:
	ComponentNameManager();

	virtual ~ComponentNameManager();
	
	static ComponentNameManager* getComponentNameMgr() {
		return &ComponentNameManager::namemanagerInstance;
	}

	String getUniqueNameFromClassName( const String& className );


protected:
	static ComponentNameManager namemanagerInstance;
	std::map<String,int> m_nameMap;

	String stripNamespace( const String& className );
private:
};


}; //end of namespace VCFBuilder

#endif //_COMPONENTNAMEMANAGER_H__


