//Form.h

#ifndef _FORM_H__
#define _FORM_H__



#include "ProjectObject.h"
#include "CPPClass.h"

using namespace VCF;


#define FORM_CLASSID		"0c46f289-4133-4174-8069-094d849f4768"


namespace VCFBuilder  {



class ComponentInstance;

/**
*Class Form documentation
*A form represents a visual "drawing board" to work
*with the UI of a particular window. A form represents the
*outermost container for a given UI.
*Forms for C++ projects always have 3 files associated with them:
*a .VFF file
*a .H file
*and as 
*There are two main cases for creating a form.
*1). Creating a new form from scratch
*2). Creating a form from and existing .VFF/.h/.cpp file triplet
*/
class Form : public ProjectObject { 
public:	
	Form( ProjectObject* parent, Project* owningProject );

	virtual ~Form();	

	String getVFFFileName() {
		return m_vffFileName;
	}

	/**
	*assumes that vffFileName is a fully qualified name
	*to a file somewhere on the hard disk
	*/
	void setVFFFileName( const String& vffFileName ) {
		m_vffFileName = vffFileName;
	}

	virtual void setName( const String& name );

	void addComponentInstance( ComponentInstance* componentInstance );

	void removeComponentInstance( ComponentInstance* componentInstance );

	Enumerator<ComponentInstance*>* getComponents();

	void saveVFF();	
	
	Component* getRootComponent() {
		return m_rootComponent;
	}

	virtual void setRootComponent( Component* rootComponent );

	void addComponentEventHandler( Component* component, EventProperty* event, const String& eventHandlerMethodName );

	void addComponentEventHandler( const String& componentName, const String& eventMethod, const String& eventHandlerMethodName );

	void removeComponentEventHandler( Component* component, EventProperty* event, const String& eventHandlerMethodName );

	String findComponentEventHandler( Component* component, EventProperty* event );

	GenericClass* getFormClass() {
		return m_formClass;
	}		
	
protected:

	virtual void destroy();
	
	Component* m_rootComponent;
	String m_vffFileName;
	std::vector<ComponentInstance*> m_components;
	EnumeratorContainer<std::vector<ComponentInstance*>,ComponentInstance*> m_componentContainer;
	
	std::multimap<String,String> m_registeredEventHandlerMap;

	GenericClass* m_formClass;
private:
};


}; //end of namespace VCFBuilder

#endif //_FORM_H__


