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

#ifndef _PATHENUMERATOR_H__
#define _PATHENUMERATOR_H__


namespace VCF {


class GRAPHICSKIT_API PathEnumerator : public Interface {
public:
	enum SegmentType{
		SEGMENT_UNKNOWN = 0,
		SEGMENT_MOVETO,
		SEGMENT_LINETO,
		SEGMENT_QUADTO,
		SEGMENT_CUBICTO,
		SEGMENT_CLOSE
	};

	enum WindingType {
		WINDING_EVENODD=0,
		WINDING_NONZERO
	};	

	virtual ~PathEnumerator(){};

	/**
	*returns the winding rule for this path
	*/
	virtual WindingType getWindingRule() = 0;

	/**
	*does this PathEnumerator have any more segments ?
	*@return bool true if the path has more segments
	*false otherwise.
	*/
	virtual bool hasMoreSegments() = 0;

	/**
	*iterates to the next segment in the path, or does nothing 
	*if the path has no more segments
	*/
	virtual void next() = 0;

	/**
	*returns the number of points in this segment.
	*The rules are:
	*1 point for SEGMENT_MOVETO and SEGMENT_LINETO
	*3 points for SEGMENT_CUBICTO
	*and 0 points for SEGMENT_CLOSE
	*/
	virtual ulong32 getPointCount() = 0;

	/**
	*retrieves the points for this segment
	*@param Point* an array of Point's. The array 
	*must be allocated by the caller and be of the size 
	*specified in getPointCount() for the segment. The values
	*will be copied into the array by the implementer of a 
	*PathEnumerator.
	*@return SegmentType the type of the segment returned
	*/
	virtual SegmentType getSegment( Point* points ) = 0;

	/**
	*returns an enumerator of all the points for this PathEnumerator
	*If any of the point segments are curves (either Cubic or Quadric),
	*then this will cause the curves to be interpolated and returned as 
	*a series of points as well.
	*/
	virtual Enumerator<Point*>* flattenPoints() = 0;

	virtual Rect* getBounds() = 0;

};

}; //end of namespace VCF

#endif //_PATHENUMERATOR_H__