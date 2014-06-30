//FormGrid.h
#include "ApplicationKit.h"
#include "FormGrid.h"


using namespace VCF;

using namespace VCFBuilder;


FormGrid::FormGrid()
{
	setColor( GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor( SYSCOLOR_WINDOW ) );	
	m_gridVisible = true;	
}

FormGrid::~FormGrid()
{

}

void FormGrid::paint( GraphicsContext* ctx )
{
	Panel::paint( ctx );
	return;
	if ( true == m_gridVisible ) {
		int y = 0;
		int x = 0;
		
		while ( y < getHeight() ) {		
			while ( x < getWidth() ) {
				ctx->moveTo( x, y );
				ctx->lineTo( x+1, y+1 );
				
				x += 10;
			}		
			x = 0;
			y += 10;
		}
		ctx->strokePath();
	}
}

