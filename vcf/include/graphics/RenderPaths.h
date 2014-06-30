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

#ifndef _RENDERPATHS_H__
#define _RENDERPATHS_H__


namespace VCF {

#define EPSILON 1e-6

class Rect;

class Point;

class SortedVectorPath;

enum PathCodeType{
  PCT_MOVETO=0,
  PCT_MOVETO_OPEN,
  PCT_CURVETO,
  PCT_LINETO,
  PCT_END
};


class GRAPHICSKIT_API BezierPath {
public:
	BezierPath( const PathCodeType& pathcode, const double& x1=0.0, const double& y1=0.0, 
		         const double& x2=0.0, const double& y2=0.0,
				 const double& x3=0.0, const double& y3=0.0 );

	virtual ~BezierPath();

	virtual void affineTransform( const double matrix[6] );
	
	PathCodeType getPathCode(){
		return m_code;
	}

	void setPathCode( const PathCodeType& pathCode ){
		m_code = pathCode;	
	}

	double getX1(){
		return m_x1;
	}

	double getY1(){
		return m_y1;
	}

	double getX2(){
		return m_x2;
	}

	double getY2(){
		return m_y2;
	}

	double getX3(){
		return m_x3;
	}

	double getY3(){
		return m_y3;
	}

	void setX1( const double& val ){
		m_x1 = val;
	}

	void setY1( const double& val ){
		m_y1 = val;
	}

	void setX2( const double& val ){
		m_x2 = val;
	}

	void setY2( const double& val ){
		m_y2= val;
	}

	void setX3( const double& val ){
		m_x3 = val;
	}

	void setY3( const double& val ){
		m_y3 = val;
	}

	void setAll( const double& x1, const double& y1, 
		         const double& x2, const double& y2,
				 const double& x3, const double& y3 ){
		m_x1 = x1;
		m_y1 = y1;
		m_x2 = x2;
		m_y2 = y2;
		m_x3 = x3;
		m_y3 = y3;

	}

private:
	PathCodeType m_code;
	double m_x1;
	double m_y1;
	double m_x2;
	double m_y2;
	double m_x3;
	double m_y3;
};


class GRAPHICSKIT_API VectorPathPoint {
public:
	VectorPathPoint( const PathCodeType& code=PCT_MOVETO, const double& x1=0.0, const double& y1=0.0 ){
		m_code = code;
		m_x = x1;
		m_y = y1;
	}
	
	virtual ~VectorPathPoint(){}

	PathCodeType m_code;
	double m_x;
	double m_y;
};

class GRAPHICSKIT_API VectorPath {
public:
	class SVPEnd{	
	public:

		class CompareEnds {
		public:
			int operator()( SVPEnd *svpEnd1, SVPEnd *svpEnd2 ){
				return VectorPath::compareCoords( svpEnd1->m_x, svpEnd1->m_y, svpEnd2->m_x, svpEnd2->m_y );
			}
		};

		SVPEnd( const int32& segNum=0, const int32& which=0, const double& x=0.0, const double& y=0.0 ){
			m_segNum = segNum;
			m_which = which;
			m_x = x;
			m_y = y;
		}

		virtual ~SVPEnd(){};

		int32 m_segNum;
		int32 m_which; /* 0 = top, 1 = bottom */
		double m_x;
		double m_y;		
	};


	VectorPath( const uint32& emptyPtCount=0 );

	/**
	*as circle
	*/
	VectorPath( const double& x, const double& y, const double& radius );

	/**
	*as a rect
	*/
	VectorPath( Rect* rect );

	VectorPath( SortedVectorPath* svp );

	virtual ~VectorPath();

	VectorPathPoint* addPoint( const PathCodeType& code, const double& x1, const double& y1 );

	void addPoint( VectorPathPoint* pt );

	VectorPath* perturb();

	virtual VectorPath* affineTransform( const double matrix[6] );

	VectorPathPoint* operator[](uint32 pos);

	int32 getPointCount(){
		return m_points.size();
	}

	void addEmptyPoints( const uint32& emptyPtCount );

	Rect* getBounds();
	
	static int32 compareCoords( const double& x1, const double& y1, 
		                      const double& x2, const double& y2 );

	static void reversePoints( Point* points, const int32& ptCount );

	static void reversePoints( std::vector<Point*>& points, const int32& ptCount );

	/**
	*why is this a friend ? 
	*Because SortedVectorPath needs non sequential access to the 
	*m_points vector so exposing it throught an Enumerator is
	*not so useful. Don't want to have a function that returns a 
	*reference to the vector 'cause that exposes the collection
	*type to the outside world and that's not so hot, ditto for 
	*a ref to an iterator of m_points. So this seems the best solution, 
	*especially given the fact that the two classes are tightly 
	*coupled anyways and by and large they will not be used by the 
	*outside to often or referenced directly.
	*/
	friend class SortedVectorPath;

	/**
	*typedef for the collection iterator
	*/
	typedef std::vector<VectorPathPoint*>::iterator Iterator;
private:
	std::vector<VectorPathPoint*> m_points;
	Rect m_bounds;
};


class GRAPHICSKIT_API VectorPathDash {
public:
	VectorPathDash( VectorPath* vPath=NULL, const double& offset=0.0, 
		            const double* dashArray=NULL, const int32& dashArrayCount=0 );
	
	
	virtual ~VectorPathDash();

	int32 getDashCount(){
		return m_dashes.size();
	}

	double getOffset(){
		return m_offset;
	}

	void setOffset( const double& offset ){
		m_offset = offset;
	}

	void addDash( const double dash ){
		m_dashes.push_back( dash );
	}
	
	VectorPath* getPath(){
		return m_path;
	}

	void setPath( VectorPath* path ){
		m_path = path;
	}
	/**
	*Returns the length of the largest subpath within vpath 
	*/
	int32 getMaxSubPath();

	VectorPath* doDash();
private:
	double m_offset;
	std::vector<double> m_dashes;
	VectorPath* m_path;
};


/**
*Sorted Vector Path stuff
*/
class GRAPHICSKIT_API SVPSegment {
public:
	/**
	*the SVPSegment compare functor class for STL algorithms
	*/
	class CompareSegments {
	public:
		int operator() ( SVPSegment* seg1, SVPSegment* seg2 ){
			return SVPSegment::compareSegments( seg1, seg2 );
		}
	};


	SVPSegment( const int32& direction, const int32& numPoints, Point* points, Rect* bbox );

	SVPSegment( const int32& direction, std::vector<Point*>& points, Rect* bbox );

	SVPSegment( SVPSegment* seg );

	virtual ~SVPSegment();

	Point* operator[] ( int pos ){
		return m_points[pos];
	}

	int32 compare( SVPSegment* segment );	

	static int32 compareSegments( SVPSegment* seg1, SVPSegment* seg2 );

	virtual String toString(){
		String s = "Number of Points = " + StringUtils::toString(m_points.size()) + "\n";

		return s;
	}
	/**
	*specifies "up" or "down"
	*/
	int m_direction;
	std::vector<Point*> m_points;
	Rect m_bbox;
};

class GRAPHICSKIT_API SortedVectorPath {
public:

	enum SVPOperation {
		SVPOP_NONE=0,
		SVPOP_MERGE,
		SVPOP_UNION,
		SVPOP_INTERSECT,
		SVPOP_DIFF,
		SVPOP_MINUS
	};

	SortedVectorPath( const SVPOperation& operation=SVPOP_NONE, 
		              SortedVectorPath* svp1=NULL, SortedVectorPath* svp2=NULL );	

	/**
	*paraphrased from art_svp_from_vpath - Raph
	* Construct a sorted vector path from a vpath.
	* @param VectorPath vpath to convert.
	*
	* Converts a vector path into sorted vector path form. The svp form is
	* more efficient for rendering and other vector operations.
	*
	* Basically, the implementation is to traverse the vector path,
	* generating a new segment for each "run" of points in the vector
	* path with monotonically increasing Y values. All the resulting
	* values are then sorted.
	*
	* Note: I'm not sure that the sorting rule is correct with respect
	* to numerical stability issues.
	*
	*JC Note
	*The original code uses dynamic arrays all over the place - this 
	*has been replaced using STL and STL sort algorithms
	**/
	SortedVectorPath( VectorPath* vPath );

	virtual ~SortedVectorPath();

	int32 getSegCount() {
		return m_segments.size();
	}

	int addSegment( const int32& direction, const int32& numPoints, Point* points, Rect* bbox );

	int addSegment( const int32& direction, std::vector<Point*>& points, Rect* bbox );
	
	void mergePerturbed( SortedVectorPath* svp1, SortedVectorPath* svp2 );

	void mergeOf( SortedVectorPath* svp1, SortedVectorPath* svp2 );
	/**
	*clears the segments and resets them to a union of the two paths
	*/
	void unionOf( SortedVectorPath* svp1, SortedVectorPath* svp2 );

	void interectionOf( SortedVectorPath* svp1, SortedVectorPath* svp2 );

	void diffOf( SortedVectorPath* svp1, SortedVectorPath* svp2 );

	void subtractionOf( SortedVectorPath* svp1, SortedVectorPath* svp2 );
	
	SortedVectorPath* uncross();

	void uncross( SortedVectorPath* svp );

	SVPSegment* operator[] ( int pos ){
		return m_segments[pos];
	}

	void intersectNeighbors( int i, int *active_segs, int *n_ips, int *n_ips_max, 
								std::vector<Point*>& ips, int *cursor );

	virtual String toString() {
		String s = "number of Segments = " +  StringUtils::toString( m_segments.size() ) + "\n";
		for (int i=0;i<m_segments.size();i++){
			s += m_segments[i]->toString();
		}
		return s;
	}
	
	static int XOrder2 (const Point& z0, const Point& z1, const Point& z2, const Point& z3);
private:

	std::vector<SVPSegment*> m_segments;	
};

}; //end of namespace VCF

#endif //_RENDERPATHS_H__
