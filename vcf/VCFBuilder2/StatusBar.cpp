//StatusBar.h
#include "ApplicationKit.h"
#include "StatusBar.h"

using namespace VCF;
StatusBar::StatusBar():
	CustomControl( true )
{

}

StatusBar::~StatusBar()
{

}

void StatusBar::paint( GraphicsContext* ctx )
{
	CustomControl::paint( ctx );
	ctx->textAt( 5, 5, this->m_statusCaption );
}