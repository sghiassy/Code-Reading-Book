//PropertyChangedCmd.h

#ifndef _PROPERTYCHANGEDCMD_H__
#define _PROPERTYCHANGEDCMD_H__



#include "AbstractCommand.h"


using namespace VCF;


namespace VCFBuilder  {

/**
*Class PropertyChangedCmd documentation
*/
class PropertyChangedCmd : public VCF::AbstractCommand { 
public:
	PropertyChangedCmd( const String& propertyName, const String& componentName, VariantData* newValue );

	virtual ~PropertyChangedCmd();

	virtual void undo();

	virtual void redo();

	virtual void execute();
protected:
	VariantData m_newValue;
	String m_newStringVal;
	String m_originalStringVal;
	VariantData m_originalValue;
	String m_propertyName;
	String m_componentName;
	Component* findComponent();
	Component* findTheComponent( const String& componentName, Container* container, Control* containerControl );
	void setProperty( VariantData* value );
private:
};


}; //end of namespace VCFBuilder

#endif //_PROPERTYCHANGEDCMD_H__


