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

#ifndef _MICROTILES_H__
#define _MICROTILES_H__


namespace VCF {

#define VCF_UTA_BBOX_CONS(x0, y0, x1, y1) (((x0) << 24) | ((y0) << 16) | \
				       ((x1) << 8) | (y1))

#define VCF_UTA_BBOX_X0(ub) ((ub) >> 24)
#define VCF_UTA_BBOX_Y0(ub) (((ub) >> 16) & 0xff)
#define VCF_UTA_BBOX_X1(ub) (((ub) >> 8) & 0xff)
#define VCF_UTA_BBOX_Y1(ub) ((ub) & 0xff)
	
#define VCF_UTILE_SHIFT 5
#define VCF_UTILE_SIZE (1 << VCF_UTILE_SHIFT)

class GRAPHICSKIT_API MicroTileBBox {
public:
	MicroTileBBox( const int32& x0=0, const int32& y0=0, const int32& x1=0, const int32& y1=0 ){
		m_bbox = VCF_UTA_BBOX_CONS(x0,y0,x1,y1);
	}

	MicroTileBBox( const MicroTileBBox& bbox ){
		m_bbox = bbox.m_bbox;
	}

	virtual ~MicroTileBBox(){};

	int32 getX0() const {
		return VCF_UTA_BBOX_X0(m_bbox);
	}

	int32 getY0() const {
		return VCF_UTA_BBOX_Y0(m_bbox);
	}

	int32 getX1() const {
		return VCF_UTA_BBOX_X1(m_bbox);
	}

	int32 getY1() const {
		return VCF_UTA_BBOX_Y1(m_bbox);
	}

	operator uint32 () const{
		return m_bbox;
	}

	operator Rect () const{
		Rect result( VCF_UTA_BBOX_X0(m_bbox),
			         VCF_UTA_BBOX_Y0(m_bbox),
					 VCF_UTA_BBOX_X1(m_bbox),
					 VCF_UTA_BBOX_Y1(m_bbox) );
		return result;
	}

	operator= ( const int32& bbox ){
		m_bbox = (uint32)bbox;
	}

	operator= ( const uint32& bbox ){
		m_bbox = bbox;
	}

	operator= ( const Rect& rect ){
		m_bbox = VCF_UTA_BBOX_CONS( (int32)rect.m_left,
			                        (int32)rect.m_top,
									(int32)rect.m_right,
									(int32)rect.m_bottom );
	}

	operator= ( Rect* rect ){
		if ( NULL != rect ){
			m_bbox = VCF_UTA_BBOX_CONS( (int32)rect->m_left,
				                        (int32)rect->m_top,
										(int32)rect->m_right,
										(int32)rect->m_bottom );
		}
	}

	void setBBox( const int32& x0, const int32& y0, const int32& x1, const int32& y1 ){
		m_bbox = VCF_UTA_BBOX_CONS(x0,y0,x1,y1);
	}
private:
	uint32 m_bbox;
};


class VectorPath;

/**
*micro tile arrays
*!!!!!!WARNING !!!!!
*currently implemented using a vector of MicroTileBBox. This is extremely slow
*and should be replaced with an array of MicroTileBBox[] allocated a construction or 
*if resizing takes place.
*/
class GRAPHICSKIT_API MicroTileArray {
public:
	MicroTileArray( const int32& x0, const int32& y0, const int32& x1, const int32& y1 );

	MicroTileArray( const int32& x0, const int32& y0, const int32& x1, const int32& y1, const int32 shiftBy );

	MicroTileArray( Rect* rect );

	MicroTileArray( VectorPath* path );

	virtual ~MicroTileArray();

	int32 getX0(){
		return m_x0;
	}

	int32 getY0(){
		return m_y0;
	}

	int32 getWidth(){
		return m_width;
	}

	int32 getHeight(){
		return m_height;
	}

	void setWidth( const int32& width ){
		m_width = width;
	}	

	void setHeight( const int32& height ){
		m_height = height;
	}

	void setX0( const int32& x0 ){
		m_x0 = x0;
	}

	void setY0( const int32& y0 ){
		m_y0 = y0;
	}

	void setNewCoords( const int32& x0, const int32& y0, const int32& x1, const int32& y1, const int32 shiftBy=VCF_UTILE_SHIFT );

	MicroTileBBox* addMicroTile( const int32& x0, const int32& y0, const int32& x1, const int32& y1 );

	void addMicroTile( MicroTileBBox* bbox );
	
	MicroTileArray* unionOf( MicroTileArray* tileArray );

	void addLine( double x0, double y0, double x1, double y1, 
		          std::vector<int32>& rbuf, int rbuf_rowstride );

	MicroTileBBox* operator[] ( const int32& pos ){
		return m_UTiles[pos];
	}

	uint32 getTileCount(){
		return m_UTiles.size();
	}

	/**
	*this sets the tile a the requested index. If the index is out of bounds, then the array is resized
	*by the delta, and a new MicroTileBBox is created. If the index is in bounds, then
	*if there is alread data at that index, the data is freed, and then a new MicroTileBBox is created
	*otherwise the existing MicroTileBBox's setBBox() is called
	*/
	void insertTileAt( const uint32& index, const int32& x0, const int32& y0, const int32& x1, const int32& y1 );

	void insertTileAt( const uint32& index, const MicroTileBBox& bbox );
private:
	int32 m_x0;
	int32 m_y0;
	int32 m_width;
	int32 m_height;
	std::vector<MicroTileBBox*> m_UTiles;
};

}; //end of namespace VCF

#endif //_MICROTILES_H__