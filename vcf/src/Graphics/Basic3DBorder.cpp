/**
*Copyright (c) 2000-2001, Jim Crafton
*All rights reserved.
*Redistribution and use in source and binary forms, with or without
*modification, are permitted provided that the following conditions
*are met:
*	Redistributions of source code must retain the above copyright
*	notice, this list of conditions and the following disclaimer.
*
*	Redistributions in binary form must reproduce the above copyright
*	notice, this list of conditions and the following disclaimer in 
*	the documentation and/or other materials provided with the distribution.
*
*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
*AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
*OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
*SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*NB: This software will not save the world.
*/

// Basic3DBorder.cpp


#include "ApplicationKit.h"


using namespace VCF;


Basic3DBorder::Basic3DBorder()
{
	
}

Basic3DBorder::~Basic3DBorder()
{
	
}

Rect* Basic3DBorder::getClientRect( Control* component )
{
	Rect* result = NULL;
	if ( NULL != component ){
		result = component->getClientBounds();
		result->inflate( -2.0, -2.0 );
	}
	return result;
}

void Basic3DBorder::paint( Rect* bounds, GraphicsContext* context )
{
	if ( NULL != bounds ){ 
		GraphicsToolkit* toolkit = GraphicsToolkit::getDefaultGraphicsToolkit();
		Color* hilight = toolkit->getSystemColor( SYSCOLOR_HIGHLIGHT );
		
		Color* shadow = toolkit->getSystemColor( SYSCOLOR_SHADOW );
		
		Color* face = toolkit->getSystemColor( SYSCOLOR_FACE );
		
		StrokeState stroke;
		
		FillState fill;
		fill.m_Color = *face;
		context->rectangle( bounds->m_left, bounds->m_top, bounds->m_right, bounds->m_bottom );
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
		
		context->moveTo( bounds->m_right-1, bounds->m_top );	
		context->lineTo( bounds->m_right-1, bounds->m_bottom-1 );	
		context->lineTo( bounds->m_left, bounds->m_bottom-1 );
		
		stroke.m_Color.setRGB( 0.0, 0.0, 0.0 );
		context->setCurrentStrokeState( &stroke );
		
		context->strokePath();
	}
}

void Basic3DBorder::paint( Control* component, GraphicsContext* context )
{
	if ( NULL == component ){
		//throw exception
	}
	Rect* bounds = component->getClientBounds();
	if ( NULL != bounds ){
		paint( bounds, context );
	}
}