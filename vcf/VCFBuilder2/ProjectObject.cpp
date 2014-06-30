//ProjectObject.h
#include "ApplicationKit.h"
#include "ProjectObject.h"

using namespace VCF;

using namespace VCFBuilder;


ProjectObject::ProjectObject()
{
	m_owningProject = NULL;
	m_parentObject = NULL;
}

ProjectObject::~ProjectObject()
{

}

void ProjectObject::acceptTraversal( ProjectObjectTraverser* traverser )
{

}