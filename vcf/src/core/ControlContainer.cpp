//ControlContainer.h
#include "ApplicationKit.h"


using namespace VCF;
ControlContainer::ControlContainer( const bool& heavyWeight ):
	CustomControl( heavyWeight )
{
	init();
}

ControlContainer::~ControlContainer()
{

}

void ControlContainer::init()
{
	m_bottomBorderHeight = 0.0;
	m_rightBorderWidth = 0.0;
	m_leftBorderWidth = 0.0;
	m_topBorderHeight = 0.0;
}

double ControlContainer::getBottomBorderHeight()
{
	return m_bottomBorderHeight;
}

void ControlContainer::setBottomBorderHeight( const double& bottomBorderHeight )
{
	m_bottomBorderHeight = __max( 0, bottomBorderHeight );
	resizeChildren();
}

double ControlContainer::getTopBorderHeight()
{
	return m_topBorderHeight;
}

void ControlContainer::setTopBorderHeight( const double& topBorderHeight )
{
	m_topBorderHeight = __max( 0, topBorderHeight );
	resizeChildren();
}

double ControlContainer::getRightBorderWidth()
{
	return m_rightBorderWidth;
}

void ControlContainer::setRightBorderWidth( const double& rightBorderWidth )
{
	m_rightBorderWidth = __max( 0, rightBorderWidth );
	resizeChildren();
}

double ControlContainer::getLeftBorderWidth()
{
	return m_leftBorderWidth;
}

void ControlContainer::setLeftBorderWidth( const double& leftBorderWidth )
{
	m_leftBorderWidth = __max( 0, leftBorderWidth );
	resizeChildren();
}

double ControlContainer::getBorderSize()
{
	double result = (m_bottomBorderHeight + m_rightBorderWidth + m_leftBorderWidth + m_topBorderHeight ) / 4.0;
	if ( (result != m_bottomBorderHeight) || (result != m_rightBorderWidth) || (result != m_leftBorderWidth) || (result != m_topBorderHeight) ) {
		result = -1;
	}
	return result;
}

void ControlContainer::setBorderSize( const double& borderSize )
{
	m_bottomBorderHeight = __max( 0.0, borderSize );
	m_rightBorderWidth = __max( 0.0, borderSize );
	m_leftBorderWidth = __max( 0.0, borderSize );
	m_topBorderHeight = __max( 0.0, borderSize );
	resizeChildren();
}

void ControlContainer::paint( GraphicsContext* context )
{
	CustomControl::paint( context );
	paintChildren( context );
}

void ControlContainer::resizeChildren()
{
	Rect* bounds = this->getBounds();		
	
	Rect tmpRect ( m_leftBorderWidth, m_topBorderHeight, 
		bounds->getWidth() - m_rightBorderWidth, bounds->getHeight() - m_bottomBorderHeight );
	
	Border* border = this->getBorder();
	if ( NULL != border ){
		Rect tmpBorderBounds = *border->getClientRect( this );
		tmpRect.m_left += tmpBorderBounds.m_left;
		tmpRect.m_top += tmpBorderBounds.m_top;
		tmpRect.m_right -= (bounds->getWidth() - tmpBorderBounds.m_right);
		tmpRect.m_bottom -= (bounds->getHeight() - tmpBorderBounds.m_bottom);
	}
	
	this->resizeChildrenUsingBounds( &tmpRect );
}