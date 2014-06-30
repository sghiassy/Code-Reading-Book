//ComponentInstance.h
#include "ApplicationKit.h"
#include "ComponentInstance.h"
#include "Project.h"

using namespace VCF;

using namespace VCFBuilder;


ComponentInstance::ComponentInstance(  ProjectObject* parent  )
{
	m_component = NULL;
	setParent( parent );
}

ComponentInstance::~ComponentInstance()
{

}

void ComponentInstance::setComponent( Component* component )
{
	m_component = component;
	getOwningProject()->linkComponentToComponentInstance( component, this );
}