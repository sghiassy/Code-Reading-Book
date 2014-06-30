//Light3DBorder.h
#include "ApplicationKit.h"



using namespace VCF;
Light3DBorder::Light3DBorder()
{
	m_inverted = false;
}

Light3DBorder::~Light3DBorder()
{
	
}

void Light3DBorder::paint( Control* control, GraphicsContext* context )
{
	if ( NULL == control ){
		//throw exception
	}
	Rect* bounds = control->getClientBounds();
	if ( NULL != bounds ){
		paint( bounds, context );
	}	
}

void Light3DBorder::paint( Rect* bounds, GraphicsContext* context )
{
	if ( NULL != bounds ){ 
		GraphicsToolkit* toolkit = GraphicsToolkit::getDefaultGraphicsToolkit();		
		
		Color* shadow = NULL;
		if ( true == m_inverted ){
			shadow = toolkit->getSystemColor( SYSCOLOR_HIGHLIGHT );
		}
		else {
			shadow = toolkit->getSystemColor( SYSCOLOR_SHADOW );
		}
		
		Color* hilight = NULL;
		if ( true == m_inverted ){
			hilight = toolkit->getSystemColor( SYSCOLOR_SHADOW );
		}
		else {
			hilight = toolkit->getSystemColor( SYSCOLOR_HIGHLIGHT );
		}

		Color* face = NULL;
		if ( true == m_inverted ){
			face = toolkit->getSystemColor( SYSCOLOR_FACE );
		}
		else {
			face = toolkit->getSystemColor( SYSCOLOR_FACE );
		}

		StrokeState stroke;
		
		FillState fill;
		fill.m_Color = *face;
		if ( true == m_inverted ) {
			context->rectangle( bounds->m_left+1, bounds->m_top+1, bounds->m_right-2, bounds->m_bottom-2 );	
		}
		else {
			context->rectangle( bounds );
		}
		
		context->setCurrentFillState( &fill );
		context->fillPath();
		
		stroke.m_Color = *hilight;
		
		context->moveTo( bounds->m_left+1, bounds->m_bottom-2 );	
		context->lineTo( bounds->m_left+1, bounds->m_top+1 );	
		context->lineTo( bounds->m_right-2, bounds->m_top+1 );
		
		context->setCurrentStrokeState( &stroke );
		
		context->strokePath();
		
		context->moveTo( bounds->m_right-2, bounds->m_top+1 );	
		context->lineTo( bounds->m_right-2, bounds->m_bottom-2 );	
		context->lineTo( bounds->m_left+1, bounds->m_bottom-2 );
		
		stroke.m_Color = *shadow;
		context->setCurrentStrokeState( &stroke );
		context->strokePath();		
	}
}

Rect* Light3DBorder::getClientRect( Control* component )
{
	Rect* result = NULL;
	if ( NULL != component ){
		result = component->getClientBounds();
		result->inflate( -1.0, -1.0 );
	}
	return result;
}