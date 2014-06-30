//CodeEditor.h
#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"
#include "CodeEditor.h"
#include "ScintillaPeer.h"
#include "DefaultTextModel.h"

using namespace VCF;

using namespace VCFBuilder;


CodeEditor::CodeEditor() : 
	Control()
{
	m_model = new DefaultTextModel();

#ifdef WIN32
	
	m_Peer = (TextPeer*) new Win32ScintillaPeer( this);
#endif

	m_Peer->setControl( this );
}

CodeEditor::~CodeEditor()
{
	delete m_model;
	m_model = NULL;

}

void CodeEditor::afterCreate( ComponentEvent* event )
{

}

bool CodeEditor::isLightWeight()
{
	return false;
}

void CodeEditor::paint( GraphicsContext * context )
{

}