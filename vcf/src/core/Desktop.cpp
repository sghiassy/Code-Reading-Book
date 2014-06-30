//Desktop.h
#include "ApplicationKit.h"



using namespace VCF;

Desktop* Desktop::create()
{
	if ( NULL == Desktop::desktopInstance ) {
		Desktop::desktopInstance = new Desktop();
		Desktop::desktopInstance->init();
	}

	return Desktop::desktopInstance;
}

Desktop::Desktop()
{
	m_context = NULL;
	m_peer = NULL;	
}

Desktop::~Desktop()
{
	/**may be NULL if the init function was never called,
	*which is theoretically possible if no one uses the Desktop object
	*Maybe the Toolkit should init the deskotp ?
	*/
	if ( NULL != m_context ) {
		m_context->getPeer()->setContextID( 0 );
		delete m_context;	
	}
	m_context = NULL;

	delete m_peer;
	m_peer = NULL;
}	

void Desktop::init()
{
	if ( m_context == NULL ) {
		this->m_peer = UIToolkit::getDefaultUIToolkit()->createDesktopPeer( this );
		m_context = new GraphicsContext( 0 );
	}
}

void Desktop::beginPainting( Rect* clippingRect )
{
	m_peer->desktopBeginPainting( clippingRect );
}

void Desktop::endPainting()
{
	m_peer->desktopEndPainting();
}

bool Desktop::supportsVirtualDirectories()
{
	return m_peer->desktopSupportsVirtualDirectories();
}


bool Desktop::hasFileSystemDirectory()
{
	return m_peer->desktopHasFileSystemDirectory();
}

String Desktop::getDirectory()
{
	return m_peer->desktopGetDirectory();
}

void Desktop::translateToScreenCoords( Control* control, Point* pt )
{
	m_peer->desktopTranslateToScreenCoords( control, pt );
}

void Desktop::translateToScreenCoords( Control* control, Rect* rect )
{
	Point pt1( rect->m_left, rect->m_top );
	m_peer->desktopTranslateToScreenCoords( control, &pt1 );
	Point pt2( rect->m_right, rect->m_bottom );
	m_peer->desktopTranslateToScreenCoords( control, &pt2 );
	rect->setRect( pt1.m_x, pt1.m_y, pt2.m_x, pt2.m_y );
}

void Desktop::translateFromScreenCoords( Control* control, Point* pt )
{
	m_peer->desktopTranslateFromScreenCoords( control, pt );
}

void Desktop::translateFromScreenCoords( Control* control, Rect* rect )
{
	Point pt1( rect->m_left, rect->m_top );
	m_peer->desktopTranslateFromScreenCoords( control, &pt1 );
	Point pt2( rect->m_right, rect->m_bottom );
	m_peer->desktopTranslateFromScreenCoords( control, &pt2 );
	rect->setRect( pt1.m_x, pt1.m_y, pt2.m_x, pt2.m_y );
}