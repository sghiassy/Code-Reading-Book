//DefaultComponentEditor.h
#include "ApplicationKit.h"
#include "ComponentEditor.h"
#include "AbstractComponentEditor.h"
#include "DefaultComponentEditor.h"
#include "VCFBuilderUI.h"
#include "Project.h"
#include "MainUIPanel.h"
#include "RemoveComponentCmd.h"



using namespace VCF;
using namespace VCFBuilder;



DefaultComponentEditor::DefaultComponentEditor()
{
	m_commandCount = 1;
}

DefaultComponentEditor::~DefaultComponentEditor()
{

}

Command* DefaultComponentEditor::getCommand( const ulong32& index )
{
	Command* result = NULL;
	//no commands for frame controls
	Frame* framedControl = dynamic_cast<Frame*>(m_component);
	
	switch( index ) {
		case 0 : {
			if ( NULL == framedControl ) {
				Project* project = VCFBuilderUIApplication::getVCFBuilderUI()->getCurrentOpenProject();
				
				result = new RemoveComponentCmd( m_component->getName(), 
					project,
					VCFBuilderUIApplication::getVCFBuilderUI()->getMainUIPanel()->getActiveForm() );
				
			}
		}
		break;

		case 1 : {
			
		}
		break;

		case 2 : {
			
		}
		break;
	}
	
	return result;
}