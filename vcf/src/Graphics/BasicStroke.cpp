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

#include "GraphicsKit.h"


using namespace VCF;


BasicStroke::BasicStroke()
{

}

BasicStroke::~BasicStroke()
{

}

void BasicStroke::init()
{
	m_context = NULL;
	m_width  = 0.0;
	m_color = *Color::getColor( BLACK );	
}

void BasicStroke::setContext( GraphicsContext* context )
{
	m_context = context;
}

void BasicStroke::render( Path * path )
{
	if ( (NULL != m_context) && (NULL != path) ){
		m_strokeState.m_Color = m_color;
		m_strokeState.m_width = m_width;
		
		m_context->setCurrentStrokeState( &m_strokeState );	
		Matrix2D* currentXFrm = m_context->getCurrentTransform();
		PathEnumerator* pathEnum = path->getPathEnumerator( currentXFrm );
		if ( NULL != pathEnum ){
			while ( true == pathEnum->hasMoreSegments() ){
				ulong32 count = pathEnum->getPointCount();
				Point* pts = NULL;
				if ( count > 0 ){
					pts = new Point[count];
				}
				PathEnumerator::SegmentType type = pathEnum->getSegment( pts );
				switch ( type ){
					case PathEnumerator::SEGMENT_MOVETO: {
						m_context->moveTo( pts[0].m_x, pts[0].m_y );
					}
					break;

					case PathEnumerator::SEGMENT_LINETO: {
						m_context->lineTo( pts[0].m_x, pts[0].m_y );
					}
					break;

					case PathEnumerator::SEGMENT_QUADTO: {
						
					}
					break;

					case PathEnumerator::SEGMENT_CUBICTO: {
						
					}
					break;

					case PathEnumerator::SEGMENT_CLOSE: {
						
					}
					break;
				}
				pathEnum->next();
				delete [] pts;
			}
		}
	}
	m_context->strokePath();
}

double BasicStroke::getWidth()
{
	return m_width;
}

void BasicStroke::setWidth( double width )
{
	m_width = width;
}

Color* BasicStroke::getColor()
{
	return &m_color;
}

void BasicStroke::setColor( Color* color )
{
	m_color = *color;
}

void BasicStroke::line( const double& x1, const double& y1,
		                const double& x2, const double& y2 )
{

}