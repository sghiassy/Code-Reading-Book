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

#include <algorithm>

using namespace VCF;

Polygon::Polygon()
{
	m_currentMoveToIndex = -1;
	m_pathEnumerator = NULL;
}

Polygon::~Polygon()
{
	clear();
	if ( NULL != m_pathEnumerator ){
		delete m_pathEnumerator;
	}
}

bool Polygon::contains(Rect * rect)
{
	return false;
}

bool Polygon::contains(Point * pt)
{
	return false;
}

bool Polygon::intersects(Point* pt)
{
	return false;
}

bool Polygon::intersects(Rect* rect)
{
	return false;
}

Rect* Polygon::getBounds()
{
	ulong32 count = m_points.size();
	
	if ( count < 2 ){ 
		m_bounds.setRect( m_points[0]->m_point.m_x,
			              m_points[0]->m_point.m_y,
						  m_points[0]->m_point.m_x,
						  m_points[0]->m_point.m_y );				
		
	}
	else {
		if ( m_points[1]->m_point.m_x > m_points[0]->m_point.m_x ) {
			m_bounds.m_left = m_points[0]->m_point.m_x;
		}
		else {			
			m_bounds.m_left = m_points[1]->m_point.m_x;
		}

		if ( m_points[1]->m_point.m_y > m_points[0]->m_point.m_y ) {
			m_bounds.m_top = m_points[0]->m_point.m_y;
		}
		else {			
			m_bounds.m_top = m_points[1]->m_point.m_y;
		}

		if ( m_points[0]->m_point.m_x < m_points[1]->m_point.m_x ) {
			m_bounds.m_right = m_points[1]->m_point.m_x;
		}
		else {			
			m_bounds.m_right = m_points[0]->m_point.m_x;
		}

		if ( m_points[0]->m_point.m_y < m_points[1]->m_point.m_y ) {
			m_bounds.m_bottom = m_points[1]->m_point.m_y;
		}
		else {			
			m_bounds.m_bottom = m_points[0]->m_point.m_y;
		}

		for (int i=2;i<count;i++){//we start on 2 since the first segment
                                  //is already assigned
			if ( m_bounds.m_left > m_points[i]->m_point.m_x ){
				m_bounds.m_left = m_points[i]->m_point.m_x;
			}

			if ( m_bounds.m_top > m_points[i]->m_point.m_y ){
				m_bounds.m_top = m_points[i]->m_point.m_y;
			}

			if ( m_bounds.m_right < m_points[i]->m_point.m_x ){
				m_bounds.m_right = m_points[i]->m_point.m_x;
			}

			if ( m_bounds.m_bottom < m_points[i]->m_point.m_y ){
				m_bounds.m_bottom = m_points[i]->m_point.m_y;
			}
		}
	}

	return &m_bounds;	
}

void Polygon::lineTo( const double& x, const double& y )
{
	PathPoint* pt = new PathPoint( x, y, PathEnumerator::SEGMENT_LINETO );
	m_points.push_back( pt );
}

void Polygon::moveTo( const double& x, const double& y )
{
	PathPoint* pt = new PathPoint( x, y, PathEnumerator::SEGMENT_MOVETO );
	m_points.push_back( pt );
	m_currentMoveToIndex = m_points.size() - 1;
}
	
void Polygon::close()
{
	if ( m_currentMoveToIndex >= 0 ) {
		PathPoint* moveToPt = m_points[m_currentMoveToIndex];
		PathPoint* pt = new PathPoint( moveToPt->m_point, PathEnumerator::SEGMENT_CLOSE );
		m_points.push_back( pt );
	}
}

void Polygon::clear()
{
	std::vector<PathPoint*>::iterator it = m_points.begin();
	while ( it != m_points.end() ){
		delete *it;
		it ++;
	}
	m_points.clear();
}


PathEnumerator* Polygon::getPathEnumerator( Matrix2D* transform )
{
	if ( NULL != m_pathEnumerator ){
		delete m_pathEnumerator;
	}
	m_pathEnumerator = new Polygon::PolyPathEnumerator( this, transform );
	return m_pathEnumerator;
}


Polygon::PolyPathEnumerator::PolyPathEnumerator( Polygon* source, Matrix2D* matrix )
{
	m_source = source;
	m_pathIterator = m_source->m_points.begin();
	while ( m_pathIterator != m_source->m_points.end() ){
		PathPoint* pt = *m_pathIterator;
		double x = pt->m_point.m_x;
		double y = pt->m_point.m_x;
		if ( NULL != matrix ){
			Matrix2D& m = *matrix;
			x = pt->m_point.m_x * (m[M_00]) + 
				pt->m_point.m_y * (m[M_10]) + 
				(m[M_20]);
			
			y = pt->m_point.m_x * (m[M_01]) + 
				pt->m_point.m_y * (m[M_11]) + 
				(m[M_21]);
		}
		m_transformedPoints.push_back( new PathPoint( x, y, pt->m_type ) );
		m_pathIterator++;
	}
	m_pathIterator = m_transformedPoints.begin();
}
		
Polygon::PolyPathEnumerator::~PolyPathEnumerator()
{
	m_pathIterator = m_transformedPoints.begin();
	while ( m_pathIterator != m_transformedPoints.end() ){
		PathPoint* pt = *m_pathIterator;
		delete pt;
		m_pathIterator++;
	}
	m_transformedPoints.clear();
}

PathEnumerator::WindingType Polygon::PolyPathEnumerator::getWindingRule()
{
	PathEnumerator::WindingType result = WINDING_EVENODD;
	return result;
}	

bool Polygon::PolyPathEnumerator::hasMoreSegments()
{
	return (m_pathIterator != m_transformedPoints.end());
}

Rect* Polygon::PolyPathEnumerator::getBounds()
{
	ulong32 count = m_transformedPoints.size();
	
	if ( count < 2 ){ 
		m_bounds.setRect( m_transformedPoints[0]->m_point.m_x,
			              m_transformedPoints[0]->m_point.m_y,
						  m_transformedPoints[0]->m_point.m_x,
						  m_transformedPoints[0]->m_point.m_y );				
		
	}
	else {
		if ( m_transformedPoints[1]->m_point.m_x > m_transformedPoints[0]->m_point.m_x ) {
			m_bounds.m_left = m_transformedPoints[0]->m_point.m_x;
		}
		else {			
			m_bounds.m_left = m_transformedPoints[1]->m_point.m_x;
		}

		if ( m_transformedPoints[1]->m_point.m_y > m_transformedPoints[0]->m_point.m_y ) {
			m_bounds.m_top = m_transformedPoints[0]->m_point.m_y;
		}
		else {			
			m_bounds.m_top = m_transformedPoints[1]->m_point.m_y;
		}

		if ( m_transformedPoints[0]->m_point.m_x < m_transformedPoints[1]->m_point.m_x ) {
			m_bounds.m_right = m_transformedPoints[1]->m_point.m_x;
		}
		else {			
			m_bounds.m_right = m_transformedPoints[0]->m_point.m_x;
		}

		if ( m_transformedPoints[0]->m_point.m_y < m_transformedPoints[1]->m_point.m_y ) {
			m_bounds.m_bottom = m_transformedPoints[1]->m_point.m_y;
		}
		else {			
			m_bounds.m_bottom = m_transformedPoints[0]->m_point.m_y;
		}

		for (int i=2;i<count;i++){//we start on 2 since the first segment
                                  //is already assigned
			if ( m_bounds.m_left > m_transformedPoints[i]->m_point.m_x ){
				m_bounds.m_left = m_transformedPoints[i]->m_point.m_x;
			}

			if ( m_bounds.m_top > m_transformedPoints[i]->m_point.m_y ){
				m_bounds.m_top = m_transformedPoints[i]->m_point.m_y;
			}

			if ( m_bounds.m_right < m_transformedPoints[i]->m_point.m_x ){
				m_bounds.m_right = m_transformedPoints[i]->m_point.m_x;
			}

			if ( m_bounds.m_bottom < m_transformedPoints[i]->m_point.m_y ){
				m_bounds.m_bottom = m_transformedPoints[i]->m_point.m_y;
			}
		}
	}
	
	return &m_bounds;
}

void Polygon::PolyPathEnumerator::next()
{
	m_pathIterator ++;
}
		
ulong32 Polygon::PolyPathEnumerator::getPointCount()
{
	ulong32 result = 0;
	PathPoint* pt = *m_pathIterator;
	if ( NULL != pt ){
		switch ( pt->m_type ){
			case PathEnumerator::SEGMENT_MOVETO : case PathEnumerator::SEGMENT_LINETO :{
				result = 1;
			}
			break;

			case PathEnumerator::SEGMENT_QUADTO : {
				result = 4;
			}
			break;

			case PathEnumerator::SEGMENT_CUBICTO : {
				result = 4;
			}
			break;

			case PathEnumerator::SEGMENT_CLOSE : {
				result = 1;
			}
			break;
		}
	}
	return result;
}
		
PathEnumerator::SegmentType Polygon::PolyPathEnumerator::getSegment( Point* points )
{
	PathEnumerator::SegmentType result = SEGMENT_UNKNOWN;
	PathPoint* pt = *m_pathIterator;
	if ( NULL != pt ){
		result = pt->m_type;
		switch ( pt->m_type ){
			case PathEnumerator::SEGMENT_MOVETO : case PathEnumerator::SEGMENT_LINETO :{
				points[0].m_x = pt->m_point.m_x;
				points[0].m_y = pt->m_point.m_y;
			}
			break;

			case PathEnumerator::SEGMENT_QUADTO : {
				//result = 4;
			}
			break;

			case PathEnumerator::SEGMENT_CUBICTO : {
				//result = 4;
			}
			break;

			case PathEnumerator::SEGMENT_CLOSE : {
				//result = 0;
				points[0].m_x = pt->m_point.m_x;
				points[0].m_y = pt->m_point.m_y;
			}
			break;
		}
	}
	return result;
}

Enumerator<Point*>* Polygon::PolyPathEnumerator::flattenPoints()
{
	return NULL;
}