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
#include "VCFMath.h"
#include <algorithm>


#define CIRCLE_STEPS 128
#define PERTURBATION 2e-3

using namespace VCF;


void VectorPath::reversePoints( Point* points, const int32& ptCount )
{
	Point tmp_p;
	
	for (int i = 0; i < (ptCount >> 1); i++) {
		tmp_p = points[i];
		points[i] = points[ptCount - (i + 1)];
		points[ptCount - (i + 1)] = tmp_p;
    }
}

void VectorPath::reversePoints( std::vector<Point*>& points, const int32& ptCount )
{
	Point tmp_p;
	int i = 0;
	std::vector<Point*>::iterator it = points.begin();
	while ( (it != points.end()) && (i < (ptCount >> 1)) ){
		tmp_p = **it;
		**it = *(points[ptCount - (i + 1)]);
		*(points[ptCount - (i + 1)]) = tmp_p;
		it++;
		i++;
	}	
}

BezierPath::BezierPath( const PathCodeType& pathcode, const double& x1, const double& y1, 
		         const double& x2, const double& y2,
				 const double& x3, const double& y3 )
{

}


BezierPath::~BezierPath()
{

}

void BezierPath::affineTransform( const double matrix[6] )
{

}


int32 VectorPath::compareCoords( const double& x1, const double& y1, 
		 						 const double& x2, const double& y2 )
{
	if (y1 - EPSILON > y2) return 1;
	if (y1 + EPSILON < y2) return -1;
	if (x1 - EPSILON > x2) return 1;
	if (x1 + EPSILON < x2) return -1;
	return 0;
}

VectorPath::VectorPath( const uint32& emptyPtCount )
{	
	addEmptyPoints( emptyPtCount );
}

VectorPath::VectorPath( const double& x, const double& y, const double& radius )
{
	int32 i;
	
	double theta;	
	
	for (i = 0; i < CIRCLE_STEPS + 1; i++) {
		VectorPathPoint* vec = new VectorPathPoint(PCT_MOVETO);

		vec->m_code = i ? PCT_LINETO : PCT_MOVETO;
		theta = (i & (CIRCLE_STEPS - 1)) * (M_PI * 2.0 / CIRCLE_STEPS);
		vec->m_x = x + radius * cos (theta);
		vec->m_y = y - radius * sin (theta);
		
		if ( i == CIRCLE_STEPS ){
			vec->m_code = PCT_END;		
		}
		this->m_points.push_back( vec );
    }
	
	
}

VectorPath::VectorPath( Rect* rect )
{
	VectorPathPoint* vec = new VectorPathPoint(PCT_MOVETO, rect->m_left, rect->m_top );
	this->m_points.push_back( vec );

	vec = new VectorPathPoint(PCT_LINETO, rect->m_right, rect->m_top );
	this->m_points.push_back( vec );

	vec = new VectorPathPoint(PCT_LINETO, rect->m_right, rect->m_bottom );
	this->m_points.push_back( vec );

	vec = new VectorPathPoint(PCT_LINETO, rect->m_left, rect->m_bottom );
	this->m_points.push_back( vec );

	vec = new VectorPathPoint(PCT_END, rect->m_left, rect->m_top );
	this->m_points.push_back( vec );
}

VectorPath::VectorPath( SortedVectorPath* svp )
{
	
	int n_segs = svp->getSegCount();	
	int *visited = NULL;
	int n_new = 0;
	int n_new_max = 0;
	int i = 0;
	int j = 0;
	int k = 0;
	int seg_num = 0;
	int first = 0;
	double last_x = 0.0;
	double last_y = 0.0;
	int n_points = 0;
	int pt_num = 0;
	
	

	std::vector<VectorPath::SVPEnd*> ends;
	ends.resize( n_segs * 2 );
	
	VectorPath::SVPEnd* svpEnd = NULL;

	for (i = 0; i < n_segs; i++) {
		int lastpt;
		SVPSegment* seg = (*svp)[i];
		
		Point* svpSegPt = seg->m_points[0];		

		ends[i * 2] = new VectorPath::SVPEnd( i, 0, svpSegPt->m_x, svpSegPt->m_y );
		
		lastpt = seg->m_points.size() - 1;
		svpSegPt = seg->m_points[lastpt];
		ends[i * 2 + 1] = new VectorPath::SVPEnd( i, 1, svpSegPt->m_x, svpSegPt->m_y );
    }
	
	std::sort( ends.begin(), ends.end(), VectorPath::SVPEnd::CompareEnds() );

	//qsort ( ends, n_segs * 2, sizeof(VectorPath::SVPEnd), VectorPath::SVPEnd::compareEnds );
	
	n_new = 0;      
	n_new_max = 16; //Raph: I suppose we _could_ estimate this from traversing
					//the svp, so we don't have to reallocate 
	
	visited = new int[n_segs];//art_new (int, n_segs);
	memset( visited, 0, n_segs*sizeof(int) );
	//for (i = 0; i < n_segs; i++)
	//	visited[i] = 0;
	
	first = 1;
	for (i = 0; i < n_segs; i++) {
		if (!first) {
			// search for the continuation of the existing subpath 
			// This could be a binary search (which is why we sorted, above) 
			for (j = 0; j < n_segs * 2; j++) {
				if ( (!visited[ends[j]->m_segNum]) &&
					(0 == VectorPath::compareCoords( last_x, last_y,
													 ends[j]->m_x, ends[j]->m_y )) ){
					break;
				}//end of if
			}

			if ( n_segs * 2 == j ) {
				first = 1;
			}
		}//end of if !first

		if (first) {
			// start a new subpath 
			for (j = 0; j < n_segs * 2; j++){
				if (!visited[ends[j]->m_segNum]) {
					break;
				}
			}
		}

		if (j == n_segs * 2) {
			printf ("failure\n");
		}

		seg_num = ends[j]->m_segNum;
		SVPSegment* svpSeg = (*svp)[seg_num];
		n_points = svpSeg->m_points.size();		

		for (k = 0; k < n_points; k++) {
			pt_num = svpSeg->m_direction ? k : n_points - (1 + k);
			Point* svpSegPt = svpSeg->m_points[pt_num];
			if ( 0 == k ) {
				if (first){
					this->addPoint( PCT_MOVETO, svpSegPt->m_x, svpSegPt->m_y );
				}
			}
			else {
				this->addPoint( PCT_LINETO, svpSegPt->m_x, svpSegPt->m_y );				
				if ( (n_points - 1) == k ){
					last_x = svpSegPt->m_x;
					last_y = svpSegPt->m_y;
					// to make more robust, check for meeting first_[xy],
					//set first if so 
				}
			}
			first = 0;
		}
		visited[seg_num] = 1;
    }
	
	this->addPoint( PCT_END, 0, 0 );
	delete [] visited;
	
	std::vector<VectorPath::SVPEnd*>::iterator it = ends.begin();
	while ( it != ends.end() ){
		delete *it;
		it++;
	}
	ends.clear();
}


VectorPath::~VectorPath()
{
	std::vector<VectorPathPoint*>::iterator it = m_points.begin();
	while ( it != m_points.end() ){
		VectorPathPoint* pt = *it;
		delete pt;
		pt = NULL;
		it++;
	}
	m_points.clear();
}

VectorPathPoint* VectorPath::addPoint( const PathCodeType& code, const double& x1, const double& y1 )
{
	VectorPathPoint* result = new VectorPathPoint(PCT_MOVETO);
	result->m_code = code;
	result->m_x = x1;
	result->m_y = y1;

	this->m_points.push_back( result );
	return result;
}

void VectorPath::addPoint( VectorPathPoint* pt )
{
	this->m_points.push_back( pt );
}

void VectorPath::addEmptyPoints( const uint32& emptyPtCount )
{
	VectorPathPoint* vec = NULL;
	for (int i=0;i<emptyPtCount;i++){
		vec = new VectorPathPoint();
		this->m_points.push_back( vec );
	}
}

Rect* VectorPath::getBounds()
{
	if ( true == m_points.empty() ){
		m_bounds.setRect( 0.0, 0.0, 0.0, 0.0 );
	}
	else {
		int i = 0;
		double x0 = 0.0;
		double y0 = 0.0;
		double x1 = 0.0;
		double y1 = 0.0;
		
		if (m_points[0]->m_code == PCT_END)	{
			x0 = y0 = x1 = y1 = 0;
		}
		else {
			x0 = x1 = m_points[0]->m_x;
			y0 = y1 = m_points[0]->m_y;
			int size = m_points.size();
			for (i = 1; i<size; i++) {
				if ( m_points[i]->m_code == PCT_END ){
					break;
				}
				else {
					if (m_points[i]->m_x < x0) x0 = m_points[i]->m_x;
					if (m_points[i]->m_x > x1) x1 = m_points[i]->m_x;
					if (m_points[i]->m_y < y0) y0 = m_points[i]->m_y;
					if (m_points[i]->m_y > y1) y1 = m_points[i]->m_y;
				}
			}
		}
		m_bounds.m_left = x0;
		m_bounds.m_top = y0;
		m_bounds.m_right = x1;
		m_bounds.m_bottom = y1;
	}
	return &m_bounds;
}

VectorPathPoint* VectorPath::operator[](uint32 pos)
{
	return m_points[pos];
}

VectorPath* VectorPath::perturb()
{
	int32 i;
	int32 size;
	VectorPath* newPath = NULL;
	double x, y;
	double x_start, y_start;
	int open;
	
	for (i = 0; m_points[i]->m_code != PCT_END; i++) {
		size = i;
	}
	
	newPath = new VectorPath();
	
	x_start = 0;
	y_start = 0;
	open = 0;
	for (i = 0; i < size; i++) {
		VectorPathPoint* pt = new VectorPathPoint();
		pt->m_code = m_points[i]->m_code;
		x = m_points[i]->m_x + (PERTURBATION * rand ()) / RAND_MAX - PERTURBATION * 0.5;
		y = m_points[i]->m_y + (PERTURBATION * rand ()) / RAND_MAX - PERTURBATION * 0.5;
		if (m_points[i]->m_code == PCT_MOVETO){
			x_start = x;
			y_start = y;
			open = 0;
		}
		else if (m_points[i]->m_code == PCT_MOVETO_OPEN)
			open = 1;
		if (!open && (i + 1 == size || m_points[i + 1]->m_code != PCT_LINETO)) {
			x = x_start;
			y = y_start;
		}
		pt->m_x = x;
		pt->m_y = y;
		
		if ( i == (size-1) ){
			pt->m_code = PCT_END;
		}

		newPath->addPoint( pt );
    }
	
	
	return newPath;
}

VectorPath* VectorPath::affineTransform( const double matrix[6] )
{
	int32 i;
	int32 size;
	VectorPath* newPath = new VectorPath();
	double x, y;
	
	for (i = 0; m_points[i]->m_code != PCT_END; i++) {
		size = i;
	}
	
	newPath = new VectorPath();
	
	
	for (i = 0; i < size; i++) {
		VectorPathPoint* pt = new VectorPathPoint();
		pt->m_code = m_points[i]->m_code;
		x = m_points[i]->m_x;
		y = m_points[i]->m_y;
		pt->m_x = matrix[0] * x + matrix[2] * y + matrix[4];
		pt->m_y = matrix[1] * x + matrix[3] * y + matrix[5];

		if ( i == (size-1) ){
			pt->m_code = PCT_END;
		}

		newPath->addPoint( pt );
    }
	
	
	return newPath;
}



VectorPathDash::VectorPathDash( VectorPath* vPath, const double& offset, 
		            const double* dashArray, const int32& dashArrayCount )
{
	m_path = vPath;
	m_offset = offset;
	m_dashes.clear();
	if ( (NULL != dashArray) && (dashArrayCount > 0) ){
		for (int i=0;i<dashArrayCount;i++){
			this->m_dashes.push_back( dashArray[i] );
		}
	}
}
	
VectorPathDash::~VectorPathDash()
{

}

int32 VectorPathDash::getMaxSubPath()
{
	int32 maxSubPath = 0;
	int i = 0;
	int start = 0;
	int size = this->m_path->getPointCount();
	for (i = 0; i < size; i++) {
		VectorPathPoint* pt = (*m_path)[i];
		if ( pt->m_code == PCT_END ){
			break;
		}
		else if (pt->m_code == PCT_MOVETO || pt->m_code == PCT_MOVETO_OPEN){
			if ( i - start > maxSubPath ){
				maxSubPath = i - start;
			}
			start = i;
		}
    }
	
	if (i - start > maxSubPath){
		maxSubPath = i - start;
	}	
	
	return maxSubPath;
}



VectorPath* VectorPathDash::doDash()
{	
	if ( NULL == m_path ){
		return NULL;
	}

	int32 maxSubPath = 0;
	
	std::vector<double> dists;


	int32 n_result = 0;
	int32 n_result_max = 0;
	int32 start = 0;
	int32 end = 0;
	int i = 0;
	double total_dist = 0.0;
	
	/* state while traversing dasharray - offset is offset of current dash
	value, toggle is 0 for "off" and 1 for "on", and phase is the distance
	in, >= 0, < dash->dash[offset]. */
	int32 offset = 0;
	int32 toggle = 0;
	double phase = 0.0;
	
	/* initial values */
	int offset_init, toggle_init;
	double phase_init;
	
	maxSubPath = this->getMaxSubPath();
	
	for ( int ii=0;ii<maxSubPath;ii++ ){
		dists.push_back(0.0);
	}

	n_result = 0;
	n_result_max = 16;

	VectorPath* result = new VectorPath( n_result_max );//==> art_new (ArtVpath, n_result_max);
	
	/* determine initial values of dash state */
	int32 nDashes = this->getDashCount();

	toggle_init = 1;
	offset_init = 0;
	phase_init = this->m_offset;
	while (phase_init >= this->m_dashes[offset_init])
    {
		toggle_init = !toggle_init;
		phase_init -= this->m_dashes[offset_init];
		offset_init++;
		if (offset_init == nDashes)
			offset_init = 0;
    }
	
	VectorPath& vpath = *m_path;

	for (start = 0; vpath[start]->m_code != PCT_END; start = end)
    {
		for (end = start + 1; vpath[end]->m_code == PCT_LINETO; end++);
		/* subpath is [start..end) */
		total_dist = 0;
		for (i = start; i < end - 1; i++){
			double dx, dy;
			VectorPathPoint* pt1 = vpath[i];
			VectorPathPoint* pt2 = vpath[i + 1];

			dx = pt2->m_x - pt1->m_x;
			dy = pt2->m_y - pt1->m_y;
			dists[i - start] = sqrt (dx * dx + dy * dy);
			total_dist += dists[i - start];
		}
		if (total_dist <= this->m_dashes[offset_init] - phase_init) {
			/* subpath fits entirely within first dash */
			if (toggle_init){
				for (i = start; i < end; i++){
					VectorPathPoint* pt = vpath[i];
					result->addPoint( pt->m_code, pt->m_x, pt->m_y );
				}
			}
		}
		else {
		/* subpath is composed of at least one dash - thus all
			generated pieces are open */
			double dist;
			
			phase = phase_init;
			offset = offset_init;
			toggle = toggle_init;
			dist = 0;
			i = start;
			if (toggle){
				VectorPathPoint* pt = vpath[i];
				result->addPoint( PCT_MOVETO_OPEN, pt->m_x, pt->m_y );
			}
				
			while (i != end - 1) {
				if (dists[i - start] - dist > this->m_dashes[offset] - phase) {
					/* dash boundary is next */
					double a;					
					double x, y;

					VectorPathPoint* pt1 = vpath[i];
					VectorPathPoint* pt2 = vpath[i + 1];
					
					dist += this->m_dashes[offset] - phase;

					a = dist / dists[i - start];

					x = pt1->m_x + a * (pt2->m_x - pt1->m_x);
					y = pt1->m_y + a * (pt2->m_y - pt1->m_y);
					result->addPoint( toggle ? PCT_LINETO : PCT_MOVETO_OPEN, x, y );
						
					/* advance to next dash */
					toggle = !toggle;
					phase = 0;
					offset++;
					if (offset == nDashes)
						offset = 0;
				}
				else {
					/* end of line in vpath is next */
					phase += dists[i - start] - dist;
					i++;
					dist = 0;
					if (toggle) {
						VectorPathPoint* pt = vpath[i];
						result->addPoint( PCT_LINETO, pt->m_x, pt->m_y );
					}
				}
			}
		}
    }
	
	result->addPoint( PCT_END, 0, 0 );
	
	return result;
}


/**
*Sorted Vector Paths
*/

SVPSegment::SVPSegment( const int32& direction, const int32& numPoints, Point* points, Rect* bbox )
{	
	this->m_direction = direction;
	
	for ( int i=0;i<numPoints;i++){
		m_points.push_back( new Point( points[i] ) );		
	}	
	
	if ( NULL != bbox ) {
		this->m_bbox = *bbox;
	}
	else if ( (NULL != points) && (numPoints > 0) ) {
		double x_min = 0.0;
		double x_max = 0.0;
		
		x_min = x_max = points[0].m_x;
		for (int i = 1; i < numPoints; i++)
		{
			if (x_min > points[i].m_x)
				x_min = points[i].m_x;
			if (x_max < points[i].m_x)
				x_max = points[i].m_x;
		}
		m_bbox.m_left = x_min;
		m_bbox.m_top = points[0].m_y;
		
		m_bbox.m_right = x_max;
		m_bbox.m_bottom = points[numPoints - 1].m_y;
    }
}

SVPSegment::SVPSegment( const int32& direction, std::vector<Point*>& points, Rect* bbox )
{	
	this->m_direction = direction;
	int numPoints = 0;
	if ( false == points.empty() ){
		int numPoints = points.size();
		for ( int i=0;i<numPoints;i++){
			this->m_points.push_back( new Point( *points[i] ) );		
		}
	}
		
	
	if ( NULL != bbox ) {
		this->m_bbox = *bbox;
	}
	else if ( (numPoints > 0) ) {
		double x_min = 0.0;
		double x_max = 0.0;
		
		x_min = x_max = points[0]->m_x;
		for (int i = 1; i < numPoints; i++)
		{
			if (x_min > points[i]->m_x)
				x_min = points[i]->m_x;
			if (x_max < points[i]->m_x)
				x_max = points[i]->m_x;
		}
		m_bbox.m_left = x_min;
		m_bbox.m_top = points[0]->m_y;
		
		m_bbox.m_right = x_max;
		m_bbox.m_bottom = points[numPoints - 1]->m_y;
    }
}

SVPSegment::SVPSegment( SVPSegment* seg )
{
	this->m_bbox = seg->m_bbox;
	this->m_direction = seg->m_direction;
	std::vector<Point*>::iterator it = seg->m_points.begin();
	while ( it != seg->m_points.end() ){
		this->m_points.push_back( new Point(**it) );
		it++;
	}
}

SVPSegment::~SVPSegment()
{
	std::vector<Point*>::iterator it = m_points.begin();
	while ( it != m_points.end() ){
		Point* pt = *it;
		delete pt;
		pt = NULL;
		it++;
	}
	m_points.clear();	
}

int32 SVPSegment::compare( SVPSegment* segment )
{
	return SVPSegment::compareSegments( this, segment );
}

int32 SVPSegment::compareSegments(  SVPSegment* seg1, SVPSegment* seg2  )
{
	int32 result = 0;

	if ( (NULL != seg1) && (NULL != seg2) ){
		
		if (seg1->m_points[0]->m_y - EPSILON > seg2->m_points[0]->m_y) return 1;
		else if (seg1->m_points[0]->m_y + EPSILON < seg2->m_points[0]->m_y) return -1;
		else if (seg1->m_points[0]->m_x - EPSILON > seg2->m_points[0]->m_x) return 1;
		else if (seg1->m_points[0]->m_x + EPSILON < seg2->m_points[0]->m_x) return -1;
		else if ((seg1->m_points[1]->m_x - seg1->m_points[0]->m_x) *
			(seg2->m_points[1]->m_y - seg2->m_points[0]->m_y) -
			(seg1->m_points[1]->m_y - seg1->m_points[0]->m_y) *
			(seg2->m_points[1]->m_x - seg2->m_points[0]->m_x) > 0) return 1;
	}

	return result;
}


SortedVectorPath::SortedVectorPath( const SVPOperation& operation, 
		              SortedVectorPath* svp1, SortedVectorPath* svp2 )
{
	if ( (operation != SVPOP_NONE) && (NULL != svp1) && (NULL != svp2) ){
		switch ( operation ){			
			case SVPOP_MERGE:{
				this->mergeOf( svp1, svp2 );
			}
			break;
			case SVPOP_UNION:{
				this->unionOf( svp1, svp2 );
			}
			break;

			case SVPOP_INTERSECT:{
				this->interectionOf( svp1, svp2 );
			}
			break;

			case SVPOP_DIFF:{
				this->diffOf( svp1, svp2 );
			}
			break;

			case SVPOP_MINUS:{
				this->subtractionOf( svp1, svp2 );
			}
			break;
		}
	}
}

SortedVectorPath::SortedVectorPath( VectorPath* vPath )
{
	int n_segs = 0;
	int n_segs_max = 16;
	
	int dir = 0;
	int new_dir = 0;
	int i = 0;
	std::vector<Point*> points;
	int pointCount = 0;//this doesn't change
	int n_points = 0;
	int n_points_max  =0;
	double x = 0.0;
	double y = 0.0;
	double x_min = 0.0;
	double x_max = 0.0;		
	
	//svp = (ArtSVP *)art_alloc (sizeof(ArtSVP) +
	//		     (n_segs_max - 1) * sizeof(ArtSVPSeg));
	
	//this->m_segments.resize( n_segs_max - 1, NULL );

	VectorPath::Iterator it = vPath->m_points.begin();
	while ( (it != vPath->m_points.end()) && ((*it)->m_code != PCT_END) ) {
		VectorPathPoint* pathPoint = *it;
		if ( (pathPoint->m_code == PCT_MOVETO) || (pathPoint->m_code == PCT_MOVETO_OPEN) )	{
			if ( (!points.empty()) && (n_points >= 2) ) {
				if ( n_segs == n_segs_max ) {
					n_segs_max <<= 1;
					//svp = (ArtSVP *)art_realloc (svp, sizeof(ArtSVP) +
					//	(n_segs_max - 1) *
					//	sizeof(ArtSVPSeg));
				}
								
				if (dir < 0)
					VectorPath::reversePoints( points, n_points );

				Rect bbox(x_min, points[0]->m_y, x_max, points[n_points - 1]->m_y);
				this->m_segments.push_back( new SVPSegment( (dir > 0),
															points,
															&bbox ) );
				//this is only here because the points are copied
				//in the LibArt version the point is jus t transfered
				//over and then reset - this current implementation
				//will suffer because of the STL, adn not doing it
				//the LibArt way
				if (dir < 0)
					VectorPath::reversePoints( points, n_points );
				
				n_segs++;
				//points = NULL;
				for( std::vector<Point*>::iterator it = points.begin();it!=points.end();it++){
					delete *it;
				}
				points.clear();
				pointCount = 0;
			}
			
			
			n_points = 1;
			x = pathPoint->m_x;
			y = pathPoint->m_y;
			if ( true == points.empty() ) {
				n_points_max = 4;
				//points = new Point[n_points_max];//==>>points = art_new (ArtPoint, n_points_max);
				points.push_back( new Point(x,y) );
				pointCount++;
			}
			else{
				points[0]->m_x = x;
				points[0]->m_y = y;
			}			
			x_min = x;
			x_max = x;
			dir = 0;
		}
		else {// must be LINETO 	
			new_dir = ( (pathPoint->m_y > y) ||	((pathPoint->m_y == y) && (pathPoint->m_x > x)) ) ? 1 : -1;
			if ( (dir) && (dir != new_dir) ) {
				//new segment 
				x = points[n_points - 1]->m_x;
				y = points[n_points - 1]->m_y;
				if ( n_segs == n_segs_max ){
					n_segs_max <<= 1;
					//svp = (ArtSVP *)art_realloc (svp, sizeof(ArtSVP) +
					//	(n_segs_max - 1) *
					//	sizeof(ArtSVPSeg));
				}

				if (dir < 0)
					VectorPath::reversePoints( points, n_points );

				Rect bbox(x_min, points[0]->m_y, x_max, points[n_points - 1]->m_y);
				this->m_segments.push_back( new SVPSegment( (dir > 0),
															points,
															&bbox ) );
				//reset the points - reverse them again
				if (dir < 0)
					VectorPath::reversePoints( points, n_points );
				
				n_segs++;
				
				n_points = 1;
				n_points_max = 4;
				//points = art_new (ArtPoint, n_points_max);
				points[0]->m_x = x;
				points[0]->m_y = y;
				x_min = x;
				x_max = x;
			}
						
			x = pathPoint->m_x;
			y = pathPoint->m_y;
			if ( n_points >= pointCount ){
				points.push_back( new Point(x,y) );
				pointCount++;
			}
			else {
				points[n_points]->m_x = x;
				points[n_points]->m_y = y;
			}
				
			if (x < x_min) x_min = x;
			else if (x > x_max) x_max = x;
			n_points++;
			
			dir = new_dir;			
		}
		i++;
		it++; //increment iterator		
	}//end of while
	
	if ( !points.empty() )  {
		if ( n_points >= 2 ) {
			if ( n_segs == n_segs_max ) {
				n_segs_max <<= 1;
				//svp = (ArtSVP *)art_realloc (svp, sizeof(ArtSVP) +
				//	(n_segs_max - 1) *
				//	sizeof(ArtSVPSeg));
			}
			if ( dir < 0 )
				VectorPath::reversePoints( points, n_points );

			Rect bbox(x_min, points[0]->m_y, x_max, points[n_points - 1]->m_y);
			this->m_segments.push_back( new SVPSegment( (dir > 0),
														points,
														&bbox ) );
			n_segs++;
		}

		for( std::vector<Point*>::iterator it = points.begin();it!=points.end();it++){
			delete *it;
		}
		points.clear();
    }	

	//svp->n_segs = n_segs;
	SVPSegment** svpSegs = (SVPSegment**)m_segments.begin();
	
	std::sort( m_segments.begin(), m_segments.end(), SVPSegment::CompareSegments() );

	//qsort ( svpSegs, n_segs, sizeof(SVPSegment*), SVPSegment::compareSegments );	
}

SortedVectorPath::~SortedVectorPath()
{
	std::vector<SVPSegment*>::iterator it = m_segments.begin();
	while ( it != m_segments.end() ){
		SVPSegment* seg = *it;
		delete seg;
		seg = NULL;
		it++;
	}
	m_segments.clear();
}

int SortedVectorPath::addSegment( const int32& direction, const int32& numPoints, Point* points, Rect* bbox )
{
	
	SVPSegment* seg = new SVPSegment( direction, numPoints, points, bbox  );

	this->m_segments.push_back( seg );
	
	return m_segments.size();
}

int SortedVectorPath::addSegment( const int32& direction, std::vector<Point*>& points, Rect* bbox )
{
	SVPSegment* seg = new SVPSegment( direction, points, bbox  );

	this->m_segments.push_back( seg );
	
	return m_segments.size();
}

void SortedVectorPath::mergeOf( SortedVectorPath* svp1, SortedVectorPath* svp2 )
{
	int ix1 = 0; 
	int ix2 = 0;
	
	int svp1Count = svp1->m_segments.size();
	int svp2Count = svp2->m_segments.size();
	int count = svp1Count + svp2Count;

	for (int ix = 0; ix < count; ix++) {
		if ( (ix1 < svp1Count) &&
			((ix2 == svp2Count) || (svp1->m_segments[ix1]->compare( svp2->m_segments[ix2] ) < 1)) ) {
			
			this->m_segments.push_back( new SVPSegment( (svp1->m_segments[ix1++]) ) );
		}
		else {
			this->m_segments.push_back( new SVPSegment( (svp2->m_segments[ix2++]) ) );
		}
    }	
}

void SortedVectorPath::mergePerturbed( SortedVectorPath* svp1, SortedVectorPath* svp2 )
{
	VectorPath vpath1( svp1 ); //==>>vpath1 = art_vpath_from_svp (svp1);

	VectorPath* vpath1_p = vpath1.perturb();//==>>vpath1_p = art_vpath_perturb (vpath1);	
	
	SortedVectorPath svp1_p(vpath1_p);
	delete vpath1_p;
	vpath1_p = NULL;

	VectorPath vpath2( svp2 );
	VectorPath* vpath2_p = vpath2.perturb();

	SortedVectorPath svp2_p(vpath2_p);
	delete vpath2_p;
	vpath2_p = NULL;

	this->mergeOf( &svp1_p, &svp2_p );	
}

void SortedVectorPath::intersectNeighbors( int i, int *active_segs, int *n_ips, int *n_ips_max, 
											std::vector<Point*>& ips, int *cursor )
{
	/*
	Point z0;
	Point z1;
	Point z2;
	Point z3;
	int asi01 = 0;
	int asi23 = 0;
	Point ip;
	
	asi01 = active_segs[i - 1];
	
	z0 = ips[asi01][0];
	if (n_ips[asi01] == 1)
		z1 = vp->segs[asi01].points[cursor[asi01] + 1];
	else
		z1 = ips[asi01][1];
	
	asi23 = active_segs[i];
	
	z2 = ips[asi23][0];
	if (n_ips[asi23] == 1)
		z3 = vp->segs[asi23].points[cursor[asi23] + 1];
	else
		z3 = ips[asi23][1];
	
	if (intersect_lines (z0, z1, z2, z3, &ip))
    {
#ifdef VERBOSE
		printf ("new intersection point: (%g, %g)\n", ip.x, ip.y);
#endif
		insert_ip (asi01, n_ips, n_ips_max, ips, ip);
		insert_ip (asi23, n_ips, n_ips_max, ips, ip);
    }
	*/
}

int SortedVectorPath::XOrder2 ( const Point& z0, const Point& z1, const Point& z2, const Point& z3)
{
	double a23 = 0.0;
	double b23 = 0.0;
	double c23 = 0.0;
	double d0 = 0.0;
	double d1 = 0.0;
	
	a23 = z2.m_y - z3.m_y;
	b23 = z3.m_x - z2.m_x;
	c23 = -(z2.m_x * a23 + z2.m_y * b23);
	
	if (a23 > 0)
    {
		a23 = -a23;
		b23 = -b23;
		c23 = -c23;
    }
	
	d0 = a23 * z0.m_x + b23 * z0.m_y + c23;
	
	if (d0 > EPSILON)
		return -1;
	else if (d0 < -EPSILON)
		return 1;
	
	d1 = a23 * z1.m_x + b23 * z1.m_y + c23;
	if (d1 > EPSILON)
		return -1;
	else if (d1 < -EPSILON)
		return 1;
	
	if (z0.m_x <= z2.m_x && z1.m_x <= z2.m_x && z0.m_x <= z3.m_x && z1.m_x <= z3.m_x)
		return -1;
	if (z0.m_x >= z2.m_x && z1.m_x >= z2.m_x && z0.m_x >= z3.m_x && z1.m_x >= z3.m_x)
		return 1;
	
	
	return 0;
}

SortedVectorPath* SortedVectorPath::uncross()
{
	/*
	typedef std::vector<Point*> VPoints;

	int* active_segs = NULL;
	int n_active_segs = 0;
	int *cursor = NULL;
	int seg_idx = 0;
	double y = 0.0;
	int tmp1 = 0;
	int tmp2 = 0;
	int asi = 0;
	int i = 0;
	int j = 0;
	 //new data structures 
	// intersection points; invariant: *ips[i] is only allocated if
	//i is active 
	int* n_ips = NULL;
	int* n_ips_max = NULL;

	std::vector<Point*> ips;

	// new sorted vector path 
	int n_segs_max, seg_num;

	SortedVectorPath* new_vp = NULL;
	
	int *n_points_max;
	// mapping from argument to new segment numbers - again, only valid
	//if active 
	int *seg_map;
	double y_curs;
	ArtPoint p_curs;
	int first_share;
	double share_x;
	ArtPoint *pts;
	
	n_segs_max = 16;
	
	//new_vp = (ArtSVP *)art_alloc (sizeof(ArtSVP) +
	//	(n_segs_max - 1) * sizeof(ArtSVPSeg));
	//new_vp->n_segs = 0;
	
	new_vp = new SortedVectorPath();

	//if we are empty then just return a new empty SVP !
	if ( this->m_segments.size() == 0)
		return new_vp;

	//otherwise pepare for a descent into hell........
	int n_segs = m_segments.size();

	active_segs = new int[n_segs];

	cursor = new int[n_segs];
	
	seg_map = new int[n_segs];

	n_ips = new int[n_segs];

	n_ips_max = new int[n_segs];

	//ips = art_new (ArtPoint *, vp->n_segs);
	
	n_points_max = new int[n_segs_max];
	
	n_active_segs = 0;
	seg_idx = 0;
	y = this->m_segments[0]->m_points[0]->m_y;

	while ( (seg_idx < n_segs) || (n_active_segs > 0) ) {

		
		// maybe move deletions to end of loop (to avoid so much special
		//casing on the end of a segment)? 
		
		// delete segments ending at y from active list 

		for (i = 0; i < n_active_segs; i++)	{
			asi = active_segs[i];
			int segNumPoints = m_segments[asi]->m_points.size();

			if ( ((segNumPoints - 1) == cursor[asi]) &&
				(m_segments[asi]->m_points[cursor[asi]]->m_y == y) ) {
				do	{
#ifdef VERBOSE
					printf ("deleting %d\n", asi);
#endif
					Point* pt = ips[asi];
					delete pt;
					pt = NULL;
					ips[asi] = NULL;
					//art_free (ips[asi]);// ?????
					n_active_segs--;
					for (j = i; j < n_active_segs; j++)
						active_segs[j] = active_segs[j + 1];
					if (i < n_active_segs)
						asi = active_segs[i];
					else
						break;
				}
				while ( (segNumPoints - 1 == cursor[asi]) &&
					    (m_segments[asi]->m_points[cursor[asi]]->m_y == y) );
				
				// test intersection of neighbors 
				if (i > 0 && i < n_active_segs)
					intersectNeighbors (i, active_segs,
					n_ips, n_ips_max, ips,
					cursor );
				
				i--;
			}	      
		}
		
		// insert new segments into the active list 
		while ( (seg_idx < n_segs) && (y == m_segments[seg_idx]->m_points[0]->m_y)) {
#ifdef VERBOSE
			printf ("inserting %d\n", seg_idx);
#endif
			cursor[seg_idx] = 0;
			for (i = 0; i < n_active_segs; i++)	{
				asi = active_segs[i];
				if ( SortedVectorPath::XOrder2 ( *(m_segments[seg_idx]->m_points[0]),
					                             *(segs[seg_idx]->m_points[1]),
												 *(m_segments[asi]->m_points[cursor[asi]]),
												 *(m_segments[asi]->m_points[cursor[asi] + 1]) ) == -1 )
					break;
			}
			
			// Create and initialize the intersection points data structure 
			n_ips[seg_idx] = 1;
			n_ips_max[seg_idx] = 2;
			ips[seg_idx] = art_new (ArtPoint, n_ips_max[seg_idx]);
			ips[seg_idx][0] = vp->segs[seg_idx].points[0];
			
			// Start a new segment in the new vector path 
			pts = art_new (ArtPoint, 16);
			pts[0] = vp->segs[seg_idx].points[0];
			seg_num = art_svp_add_segment (&new_vp, &n_segs_max,
				&n_points_max,
				1, vp->segs[seg_idx].dir,
				pts,
				NULL);
			n_points_max[seg_num] = 16;
			seg_map[seg_idx] = seg_num;
			
			tmp1 = seg_idx;
			for (j = i; j < n_active_segs; j++)
			{
				tmp2 = active_segs[j];
				active_segs[j] = tmp1;
				tmp1 = tmp2;
			}
			active_segs[n_active_segs] = tmp1;
			n_active_segs++;
			
			if (i > 0)
				intersect_neighbors (i, active_segs,
				n_ips, n_ips_max, ips,
				cursor, vp);
			
			if (i + 1 < n_active_segs)
				intersect_neighbors (i + 1, active_segs,
				n_ips, n_ips_max, ips,
				cursor, vp);
			
			seg_idx++;
		}
		
		// all active segs cross the y scanline (considering segs to be
		//closed on top and open on bottom) 
#ifdef VERBOSE
		for (i = 0; i < n_active_segs; i++)
		{
			asi = active_segs[i];
			printf ("%d ", asi);
			for (j = 0; j < n_ips[asi]; j++)
				printf ("(%g, %g) - ",
				ips[asi][j].x,
				ips[asi][j].y);
			printf ("(%g, %g) %s\n",
				vp->segs[asi].points[cursor[asi] + 1].x,
				vp->segs[asi].points[cursor[asi] + 1].y,
				vp->segs[asi].dir ? "v" : "^");
		}
#endif
		
		// advance y to the next event 
		//Note: this is quadratic. We'd probably get decent constant
		//factor speed improvement by caching the y_curs values. 
		if (n_active_segs == 0)
		{
			if (seg_idx < vp->n_segs)
				y = vp->segs[seg_idx].points[0].y;
			// else we're done 
		}
		else
		{
			asi = active_segs[0];
			if (n_ips[asi] == 1)
				y = vp->segs[asi].points[cursor[asi] + 1].y;
			else
				y = ips[asi][1].y;
			for (i = 1; i < n_active_segs; i++)
			{
				asi = active_segs[i];
				if (n_ips[asi] == 1)
					y_curs = vp->segs[asi].points[cursor[asi] + 1].y;
				else
					y_curs = ips[asi][1].y;
				if (y > y_curs)
					y = y_curs;
			}
			if (seg_idx < vp->n_segs && y > vp->segs[seg_idx].points[0].y)
				y = vp->segs[seg_idx].points[0].y;
		}
		
		first_share = -1;
		share_x = 0; // to avoid gcc warning, although share_x is never
		      //used when first_share is -1 
		// advance cursors to reach new y 
		for (i = 0; i < n_active_segs; i++)
		{
			asi = active_segs[i];
			if (n_ips[asi] == 1)
				p_curs = vp->segs[asi].points[cursor[asi] + 1];
			else
				p_curs = ips[asi][1];
			if (p_curs.y == y)
			{
				svp_add_point (new_vp, n_points_max,
					p_curs, seg_map, active_segs, n_active_segs, i);
				
				n_ips[asi]--;
				for (j = 0; j < n_ips[asi]; j++)
					ips[asi][j] = ips[asi][j + 1];
				
				if (n_ips[asi] == 0)
				{
					ips[asi][0] = p_curs;
					n_ips[asi] = 1;
					cursor[asi]++;
				}
				
				if (first_share < 0 || p_curs.x != share_x)
				{
				// this is where crossings are detected, and if
				//	found, the active segments switched around. 
					
					fix_crossing (first_share, i,
						active_segs, n_active_segs,
						cursor, ips, n_ips, n_ips_max, vp, seg_map,
						&new_vp,
						&n_segs_max, &n_points_max);
					
					first_share = i;
					share_x = p_curs.x;
				}
				
				if (cursor[asi] < vp->segs[asi].n_points - 1)
				{
					
					if (i > 0)
						intersect_neighbors (i, active_segs,
						n_ips, n_ips_max, ips,
						cursor, vp);
					
					if (i + 1 < n_active_segs)
						intersect_neighbors (i + 1, active_segs,
						n_ips, n_ips_max, ips,
						cursor, vp);
				}
			}
			else
			{
				// not on a cursor point 
				fix_crossing (first_share, i,
					active_segs, n_active_segs,
					cursor, ips, n_ips, n_ips_max, vp, seg_map,
					&new_vp,
					&n_segs_max, &n_points_max);
				first_share = -1;
			}
		}
		
		// fix crossing on last shared group 
		fix_crossing (first_share, i,
			active_segs, n_active_segs,
			cursor, ips, n_ips, n_ips_max, vp, seg_map,
			&new_vp,
			&n_segs_max, &n_points_max);
		
#ifdef VERBOSE
		printf ("\n");
#endif
    }
	
	// not necessary to sort, new segments only get added at y, which
	//increases monotonically 
#if 0
	qsort (&new_vp->segs, new_vp->n_segs, sizeof (svp_seg), svp_seg_compare);
	{
		int k;
		for (k = 0; k < new_vp->n_segs - 1; k++)
		{
			printf ("(%g, %g) - (%g, %g) %s (%g, %g) - (%g, %g)\n",
				new_vp->segs[k].points[0].x,
				new_vp->segs[k].points[0].y,
				new_vp->segs[k].points[1].x,
				new_vp->segs[k].points[1].y,
				svp_seg_compare (&new_vp->segs[k], &new_vp->segs[k + 1]) > 1 ? ">": "<",
				new_vp->segs[k + 1].points[0].x,
				new_vp->segs[k + 1].points[0].y,
				new_vp->segs[k + 1].points[1].x,
				new_vp->segs[k + 1].points[1].y);
		}
	}
#endif
	
	art_free (n_points_max);
	art_free (seg_map);
	art_free (n_ips_max);
	art_free (n_ips);
	art_free (ips);
	art_free (cursor);
	art_free (active_segs);
	
	return new_vp;
	*/
	return NULL;
}

void SortedVectorPath::uncross( SortedVectorPath* svp )
{
	
}

void SortedVectorPath::unionOf( SortedVectorPath* svp1, SortedVectorPath* svp2 )
{
	SortedVectorPath tmpSvp1;
	tmpSvp1.mergePerturbed( svp1, svp2 );
	/*
	ArtSVP *svp3, *svp4, *svp_new;
	
	this->mergePerturbed( svp1, svp2 );
	//svp3 = art_svp_merge_perturbed (svp1, svp2);
	svp4 = art_svp_uncross (svp3);
	art_svp_free (svp3);
	
	svp_new = art_svp_rewind_uncrossed (svp4, ART_WIND_RULE_POSITIVE);

	art_svp_free (svp4);
	return svp_new;
	*/
}

void SortedVectorPath::interectionOf( SortedVectorPath* svp1, SortedVectorPath* svp2 )
{

}

void SortedVectorPath::diffOf( SortedVectorPath* svp1, SortedVectorPath* svp2 )
{

}

void SortedVectorPath::subtractionOf( SortedVectorPath* svp1, SortedVectorPath* svp2 )
{

}