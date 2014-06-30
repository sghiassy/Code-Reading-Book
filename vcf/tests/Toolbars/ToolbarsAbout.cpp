//ToolbarsAbout.h
#include "ApplicationKit.h"
#include "ToolbarsAbout.h"
#include "Label.h"

using namespace VCF;


ToolbarsAbout::ToolbarsAbout()
{
	setWidth( 300 );
	setHeight( 100 );

	CommandButton* okBtn = new CommandButton();
	okBtn->setWidth( okBtn->getPreferredWidth() );
	okBtn->setHeight( okBtn->getPreferredHeight() );

	okBtn->setTop( getHeight() - (okBtn->getHeight()+35) );
	okBtn->setLeft( getWidth() - (okBtn->getWidth()+25) );
	add( okBtn );
	okBtn->setFocus( true );
	okBtn->setCommandType( BC_OK );
	okBtn->setCaption( "&OK" );

	Label* label = new Label();
	label->setBounds( &Rect(25, 10, 200, 45) );
	add( label );
	label->setCaption( "About Toolbars." );

	setCaption( "About Toolbars" );
}

ToolbarsAbout::~ToolbarsAbout()
{

}

