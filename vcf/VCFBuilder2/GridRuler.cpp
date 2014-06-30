//GridRuler.h
#include "ApplicationKit.h"
#include "GridRuler.h"
#include "Light3DBorder.h"


using namespace VCF;

using namespace VCFBuilder;


GridRuler::GridRuler():
	CustomControl( false )
{
	m_horizontalRuler = true;
}

GridRuler::~GridRuler()
{

}

bool GridRuler::isHorizontalRuler()
{
	return m_horizontalRuler;
}

void GridRuler::setHorizontalRuler( const bool& horizontalRuler )
{
	m_horizontalRuler = horizontalRuler;
}

void GridRuler::paint( GraphicsContext* ctx )
{
	CustomControl::paint( ctx );

	Light3DBorder bdr;
	Rect r(0,0,getWidth(),getHeight());
	r.inflate( -2, -2 );
	bdr.setInverted( true );
	bdr.paint( &r, ctx );

	Color* rulerFill = GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor( SYSCOLOR_WINDOW );
	ctx->setColor( rulerFill );
	r.inflate( -2, -2 );
	ctx->rectangle( &r );
	ctx->fillPath();

	int pos = 0;
	ctx->setColor( GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor( SYSCOLOR_SHADOW ) );
	if ( true == this->m_horizontalRuler ) {
		int w = getWidth();
		
		for ( int i=0;i<w;i++) {
			if ( (r.m_left + i) < r.m_right ) {
				if ( (i % 5) == 0 ) {
					if ( (i % 50) == 0 ) {
						ctx->moveTo( r.m_left + i, r.m_bottom - 10 );
						ctx->lineTo( r.m_left + i, r.m_bottom );
					}
					else {
						ctx->moveTo( r.m_left + i, r.m_bottom - 5 );
						ctx->lineTo( r.m_left + i, r.m_bottom );
					}				
				}
			}
		}
		ctx->strokePath();
	}
	else {
		int h = getHeight();
		
		for ( int i=0;i<h;i++) {
			if ( (r.m_top + i) < r.m_bottom ) {
				if ( (i % 5) == 0 ) {
					if ( (i % 50) == 0 ) {
						ctx->moveTo( r.m_right - 10, r.m_top + i );
						ctx->lineTo( r.m_right, r.m_top + i );
					}
					else {
						ctx->moveTo( r.m_right - 5, r.m_top + i );
						ctx->lineTo( r.m_right, r.m_top + i );
					}				
				}
			}
		}
		ctx->strokePath();
	}
}