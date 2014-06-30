//UIForm.h
#include "ApplicationKit.h"
#include "Form.h"
#include "UIForm.h"
#include "Project.h"

using namespace VCF;

using namespace VCFBuilder;


UIForm::UIForm( ProjectObject* parent, Project* owningProject ):
	Form( parent, owningProject )
{
	m_formCPPClass = new CPPClass( this );
	m_formClass = m_formCPPClass;
	m_frameControl = NULL;
}

UIForm::~UIForm()
{
	delete m_formClass;
}

void UIForm::setName( const String& name )
{
	Form::setName( name );	
}