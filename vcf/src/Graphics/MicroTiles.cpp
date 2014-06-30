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


#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif



MicroTileArray::MicroTileArray( const int32& x0, const int32& y0, const int32& x1, const int32& y1 )
{
	m_x0 = x0;
	m_y0 = y0;

	m_width = x1 - x0;
	m_height = y1 - y0;

	//this->m_UTiles.resize( m_width * m_height, NULL );
}

MicroTileArray::MicroTileArray( const int32& x0, const int32& y0, const int32& x1, const int32& y1, const int32 shiftBy )
{
	setNewCoords( x0, y0, x1, y1, shiftBy );
}

MicroTileArray::MicroTileArray( Rect* rect )
{
	if ( NULL != rect ){
		//ArtMicroTileBBox *utiles;
		MicroTileBBox bb;
		int32 width = 0;
		int32 height = 0;
		int32 x = 0;
		int32 y = 0;
		int32 xf0 = 0;
		int32 yf0 = 0;
		int32 xf1 = 0;
		int32 yf1 = 0;
		int32 ix = 0;
		
		//uta = art_new (ArtUta, 1);
		this->m_x0 = ((int32)rect->m_left) >> VCF_UTILE_SHIFT;
		this->m_y0 = ((int32)rect->m_top) >> VCF_UTILE_SHIFT;
		width = (((int32)(rect->m_right) + VCF_UTILE_SIZE - 1) >> VCF_UTILE_SHIFT) - this->m_x0;
		height = ((((int32)rect->m_bottom) + VCF_UTILE_SIZE - 1) >> VCF_UTILE_SHIFT) - this->m_y0;
		
		this->m_UTiles.resize( width * height, NULL );
		//utiles = art_new (ArtMicroTileBBox, width * height);
		
		this->m_width = width;
		this->m_height = height;
		//this->utiles = utiles;
		
		xf0 = ((int32)rect->m_left) & (VCF_UTILE_SIZE - 1);
		yf0 = ((int32)rect->m_top) & (VCF_UTILE_SIZE - 1);
		xf1 = ((((int32)rect->m_right) - 1) & (VCF_UTILE_SIZE - 1)) + 1;
		yf1 = ((((int32)rect->m_bottom) - 1) & (VCF_UTILE_SIZE - 1)) + 1;
		if (height == 1) {
			if (width == 1){				
				this->insertTileAt( 0, xf0, yf0, xf1, yf1 );//==>>utiles[0] = ART_UTA_BBOX_CONS (xf0, yf0, xf1, yf1);				
			}
			else {
				this->insertTileAt( 0, xf0, yf0, VCF_UTILE_SIZE, yf1 );//==>>utiles[0] = ART_UTA_BBOX_CONS (xf0, yf0, VCF_UTILE_SIZE, yf1);
				//bb.setBBox( 0, yf0, VCF_UTILE_SIZE, yf1 );
				for (x = 1; x < width - 1; x++){
					this->insertTileAt( x, 0, yf0, VCF_UTILE_SIZE, yf1 );//==>>utiles[x] = bb;
				}
				this->insertTileAt( x, 0, yf0, xf1, yf1 );//==>>utiles[x] = ART_UTA_BBOX_CONS (0, yf0, xf1, yf1);
			}
		}
		else
		{
			if (width == 1)
			{
				insertTileAt( 0, xf0, yf0, xf1, VCF_UTILE_SIZE );//==>>utiles[0] = ART_UTA_BBOX_CONS (xf0, yf0, xf1, VCF_UTILE_SIZE);
				//bb.setBBox( xf0, 0, xf1, VCF_UTILE_SIZE );
				for (y = 1; y < height - 1; y++){
					insertTileAt( y, xf0, 0, xf1, VCF_UTILE_SIZE );//==>>utiles[y] = bb;
				}
				insertTileAt( y, xf0, 0, xf1, yf1 );
			}
			else
			{
				insertTileAt( 0, xf0, yf0, VCF_UTILE_SIZE, VCF_UTILE_SIZE );
				//bb.setBBox( 0, yf0, VCF_UTILE_SIZE, VCF_UTILE_SIZE );
				for (x = 1; x < width - 1; x++){
					insertTileAt( x, 0, yf0, VCF_UTILE_SIZE, VCF_UTILE_SIZE );
				}
				insertTileAt( x, 0, yf0, xf1, VCF_UTILE_SIZE );
				ix = width;
				for (y = 1; y < height - 1; y++) { 
					insertTileAt( ix++, xf0, 0, VCF_UTILE_SIZE, VCF_UTILE_SIZE );
					//bb.setBBox( 0, 0, VCF_UTILE_SIZE, VCF_UTILE_SIZE );
					for (x = 1; x < width - 1; x++){
						insertTileAt( ix++, 0, 0, VCF_UTILE_SIZE, VCF_UTILE_SIZE );
					}
					insertTileAt( ix++, 0, 0, xf1, VCF_UTILE_SIZE );
				}
				insertTileAt( ix++, xf0, 0, VCF_UTILE_SIZE, yf1 );
				//bb.setBBox( 0, 0, VCF_UTILE_SIZE, yf1 );
				for (x = 1; x < width - 1; x++) {
					insertTileAt( ix++, 0, 0, VCF_UTILE_SIZE, yf1 );
				}
				insertTileAt( ix++, 0, 0, xf1, yf1 );
			}
		}		
	}
}

MicroTileArray::MicroTileArray( VectorPath* path )
{	
	Rect bbox;
	//int32* rbuf = NULL;
	int32 i = 0;
	double x = 0.0;
	double y = 0.0;
	int32 sum = 0;
	int32 xt = 0;
	int32 yt = 0;
	//ArtUtaBbox *utiles;
	MicroTileBBox bb;
	int width = 0;
	int height = 0;
	int ix = 0;
	
	bbox = *(path->getBounds());//==>>art_vpath_bbox_irect (vec, &bbox);
	
	this->setNewCoords( bbox.m_left, bbox.m_top, bbox.m_right, bbox.m_bottom );//=>>uta = art_uta_new_coords (bbox.x0, bbox.y0, bbox.x1, bbox.y1);
	
	width = this->m_width;
	height = this->m_height;	
	
	//resize the tile array
	this->m_UTiles.resize( width * height, NULL );

	std::vector<int32> intList;
	for (i = 0; i < width * height; i++)
		intList.push_back(0);
	
	x = 0;
	y = 0;
	int size = path->getPointCount();
	bool quitloop = false;
	for (i = 0; i<size; i++)  {
		VectorPathPoint* pt = (*path)[i];
		switch (pt->m_code)	{
		case PCT_END: {
				quitloop = true;
			};
			break;

			case PCT_MOVETO:{
				x = pt->m_x;
				y = pt->m_y;
			}
			break;

			case PCT_LINETO: { 
				this->addLine( pt->m_x, pt->m_y, x, y, intList, width );//==>>art_uta_add_line (uta, vec[i].x, vec[i].y, x, y, rbuf, width);
				x = pt->m_x;
				y = pt->m_y;
			}
			break;
		}
		if ( true == quitloop ){
			break;
		}
    }
	
	/* now add in the filling from rbuf */
	ix = 0;
	for (yt = 0; yt < height; yt++) {
		sum = 0;
		for (xt = 0; xt < width; xt++){
			
			sum += intList[ix];
			/* Nonzero winding rule - others are possible, but hardly
			worth it. */
			if (sum != 0) {
				MicroTileBBox newBB(0);
				
				newBB = ((int32)newBB) & 0xffff0000;
				newBB = ((int32)newBB) | ((VCF_UTILE_SIZE << 8) | VCF_UTILE_SIZE);
				//utiles[ix] = bb;
				this->insertTileAt( ix, newBB );
				if (xt != width - 1) {
					newBB = 0;
					//bb = *(this->m_UTiles[ix + 1]);

					newBB = ((int32)newBB) & 0xffff00;
					newBB = ((int32)newBB) | VCF_UTILE_SIZE;
					this->insertTileAt(ix + 1, newBB );
				}
				if (yt != height - 1){
					MicroTileBBox* bb = this->m_UTiles[ix + width];
					if ( NULL != bb ){
						*bb = ((int32)*bb) & 0xff0000ff;
						*bb = ((int32)*bb) | VCF_UTILE_SIZE << 8;
						//this->insertTileAt(ix + 1, bb );
						//utiles[ix + width] = bb;
					}
					if (xt != width - 1)
					{
						bb = this->m_UTiles[ix + width];
						if ( NULL != bb ){
							*bb = ((int32)*bb) & 0xffff;
						}
					}

				}
			}
			ix++;
		}
    }
	
	//art_free (rbuf);
	
	//return uta;
}

MicroTileArray::~MicroTileArray()
{
	std::vector<MicroTileBBox*>::iterator it = this->m_UTiles.begin();
	while ( it != m_UTiles.end() ){
		MicroTileBBox* bbox = *it;
		delete bbox;
		bbox = NULL;
		it++;
	}
	m_UTiles.clear();
}

void MicroTileArray::addLine( double x0, double y0, double x1, double y1, 
							 std::vector<int32>& rbuf, int rbuf_rowstride )
{
	
	int32 xmin = 0;
	int32 ymin = 0;
	double xmax = 0.0;
	double ymax = 0.0;
	int32 xmaxf = 0;
	int32 ymaxf = 0;
	int32 xmaxc = 0;
	int32 ymaxc = 0;
	int xt0 = 0;
	int yt0 = 0;
	int xt1 = 0;
	int yt1 = 0;
	int xf0 = 0;
	int yf0 = 0;
	int xf1 = 0;
	int yf1 = 0;
	int ix = 0;
	int ix1 = 0;
	MicroTileBBox bb(0);
	
	xmin = floor (MIN(x0, x1));
	xmax = MAX(x0, x1);
	xmaxf = floor (xmax);
	xmaxc = ceil (xmax);
	ymin = floor (MIN(y0, y1));
	ymax = MAX(y0, y1);
	ymaxf = floor (ymax);
	ymaxc = ceil (ymax);
	xt0 = (xmin >> VCF_UTILE_SHIFT) - this->m_x0;
	yt0 = (ymin >> VCF_UTILE_SHIFT) - this->m_y0;
	xt1 = (xmaxf >> VCF_UTILE_SHIFT) - this->m_x0;
	yt1 = (ymaxf >> VCF_UTILE_SHIFT) - this->m_y0;
	MicroTileBBox* tile = NULL;
	if (xt0 == xt1 && yt0 == yt1) {
		// entirely inside a microtile, this is easy! 
		xf0 = xmin & (VCF_UTILE_SHIFT - 1);
		yf0 = ymin & (VCF_UTILE_SHIFT - 1);
		xf1 = (xmaxf & (VCF_UTILE_SHIFT - 1)) + xmaxc - xmaxf;
		yf1 = (ymaxf & (VCF_UTILE_SHIFT - 1)) + ymaxc - ymaxf;
		
		ix = yt0 * this->m_width + xt0;
		
		tile = this->m_UTiles[ix];
		//insertTileAt( ix, 
		if ( NULL != tile ) {
			if (*tile == 0) {
				tile->setBBox(xf0, yf0, xf1, yf1);
			}
			else {
				tile->setBBox( MIN(tile->getX0(), xf0),
					MIN(tile->getY0(), yf0),
					MAX(tile->getX1(), xf1),
					MAX(tile->getY1(), yf1));
			}
		}
		else {
			insertTileAt( ix, xf0, yf0, xf1, yf1 );
		}
		
		//uta->utiles[ix] = bb;
	}
	else  {
		double dx, dy;
		int sx, sy;
		
		dx = x1 - x0;
		dy = y1 - y0;
		sx = dx > 0 ? 1 : dx < 0 ? -1 : 0;
		sy = dy > 0 ? 1 : dy < 0 ? -1 : 0;
		if (ymin == ymaxf)	{
			// special case horizontal (dx/dy slope would be infinite) 
			xf0 = xmin & (VCF_UTILE_SIZE - 1);
			yf0 = ymin & (VCF_UTILE_SIZE - 1);
			xf1 = (xmaxf & (VCF_UTILE_SIZE - 1)) + xmaxc - xmaxf;
			yf1 = (ymaxf & (VCF_UTILE_SIZE - 1)) + ymaxc - ymaxf;
			
			ix = yt0 * this->m_width + xt0;
			ix1 = yt0 * this->m_width + xt1;
			while (ix != ix1){
				tile = this->m_UTiles[ix];
				if ( NULL != tile ) {
					if (*tile == 0) {
						tile->setBBox(xf0, yf0, xf1, yf1);
					}
					else {
						tile->setBBox( MIN(tile->getX0(), xf0),
							MIN(tile->getY0(), yf0),
							MAX(tile->getX1(), xf1),
							MAX(tile->getY1(), yf1));
					}
				}
				else {
					insertTileAt( ix, xf0, yf0, xf1, yf1 );
				}
				xf0 = 0;
				ix++;
			}
			tile = this->m_UTiles[ix];
			//bb = uta->utiles[ix];
			if ( NULL != tile ){
				if ( 0 == *tile ){
					tile->setBBox( 0, yf0, xf1, yf1 );
				}
				else {
					tile->setBBox( 0,
									MIN(tile->getY0(), yf0),
									MAX(tile->getX1(), xf1),
									MAX(tile->getY1(), yf1) );
				}	
			}
			else {
				insertTileAt( ix, 0, yf0, xf1, yf1 );
			}
		}
		else {
			// Do a Bresenham-style traversal of the line 
			double dx_dy;
			double x, y;
			double xn, yn;
			
			// normalize coordinates to uta origin 
			x0 -= this->m_x0 << VCF_UTILE_SHIFT;
			y0 -= this->m_y0 << VCF_UTILE_SHIFT;
			x1 -= this->m_x0 << VCF_UTILE_SHIFT;
			y1 -= this->m_y0 << VCF_UTILE_SHIFT;
			if (dy < 0)	{
				double tmp;
				
				tmp = x0;
				x0 = x1;
				x1 = tmp;
				
				tmp = y0;
				y0 = y1;
				y1 = tmp;
				
				dx = -dx;
				sx = -sx;
				dy = -dy;
				// we leave sy alone, because it would always be 1,
				//and we need it for the rbuf stuff. 
			}
			xt0 = ((int)floor (x0) >> VCF_UTILE_SHIFT);
			xt1 = ((int)floor (x1) >> VCF_UTILE_SHIFT);
			// now [xy]0 is above [xy]1 
			
			ix = yt0 * this->m_width + xt0;
			ix1 = yt1 * this->m_width + xt1;
			
			
			dx_dy = dx / dy;
			x = x0;
			y = y0;
			while (ix != ix1) {
				int dix;
				
				// figure out whether next crossing is horizontal or vertical 
				
				yn = (yt0 + 1) << VCF_UTILE_SHIFT;
				xn = x0 + dx_dy * (yn - y0);
				if (xt0 != (int)floor (xn) >> VCF_UTILE_SHIFT) {
					// horizontal crossing 
					xt0 += sx;
					dix = sx;
					if (dx > 0) {
						xn = xt0 << VCF_UTILE_SHIFT;
						yn = y0 + (xn - x0) / dx_dy;
						
						xf0 = (int)floor (x) & (VCF_UTILE_SHIFT - 1);
						xf1 = VCF_UTILE_SIZE;
					}
					else {
						xn = (xt0 + 1) << VCF_UTILE_SHIFT;
						yn = y0 + (xn - x0) / dx_dy;
						
						xf0 = 0;
						xmaxc = (int)ceil (x);
						xf1 = xmaxc - ((xt0 + 1) << VCF_UTILE_SHIFT);
					}
					ymaxf = (int)floor (yn);
					ymaxc = (int)ceil (yn);
					yf1 = (ymaxf & (VCF_UTILE_SIZE - 1)) + ymaxc - ymaxf;
				}
				else {
					// vertical crossing 
					dix = this->m_width;
					xf0 = (int)floor (MIN(x, xn)) & (VCF_UTILE_SIZE - 1);
					xmax = MAX(x, xn);
					xmaxc = (int)ceil (xmax);
					xf1 = xmaxc - (xt0 << VCF_UTILE_SHIFT);
					yf1 = VCF_UTILE_SIZE;
					
					if ( false == rbuf.empty() )
						rbuf[yt0 * rbuf_rowstride + xt0] += sy;
					
					yt0++;
				}
				yf0 = (int)floor (y) & (VCF_UTILE_SIZE - 1);

				tile = this->m_UTiles[ix];

				if ( NULL != tile ){
					if ( 0 == *tile ){
						tile->setBBox( xf0, yf0, xf1, yf1 );
					}
					else {
						tile->setBBox( MIN(tile->getX0(), xf0),
										MIN(tile->getY0(), yf0),
										MAX(tile->getX1(), xf1),
										MAX(tile->getY1(), yf1) );
					}
				}
				else {
					insertTileAt( ix, xf0, yf0, xf1, yf1 );	
				}

				/*bb = uta->utiles[ix];
				if (bb == 0)
					bb = ART_UTA_BBOX_CONS(xf0, yf0, xf1, yf1);
				else
					bb = ART_UTA_BBOX_CONS(MIN(ART_UTA_BBOX_X0(bb), xf0),
					MIN(ART_UTA_BBOX_Y0(bb), yf0),
					MAX(ART_UTA_BBOX_X1(bb), xf1),
					MAX(ART_UTA_BBOX_Y1(bb), yf1));
				uta->utiles[ix] = bb;
				*/

				x = xn;
				y = yn;
				ix += dix;
			}
			xmax = MAX(x, x1);
			xmaxc = ceil (xmax);
			ymaxc = ceil (y1);
			xf0 = (int)floor (MIN(x1, x)) & (VCF_UTILE_SIZE - 1);
			yf0 = (int)floor (y) & (VCF_UTILE_SIZE - 1);
			xf1 = xmaxc - (xt0 << VCF_UTILE_SHIFT);
			yf1 = ymaxc - (yt0 << VCF_UTILE_SHIFT);
			//bb = uta->utiles[ix];
			tile = this->m_UTiles[ix];
			if ( NULL != tile ){
				if ( 0 == *tile ){
					tile->setBBox( xf0, yf0, xf1, yf1 ); 
				}
				else {
					tile->setBBox( MIN( tile->getX0(), xf0),
									MIN( tile->getY0(), yf0),
									MAX(tile->getX1(), xf1),
									MAX(tile->getY1(), yf1) );
				}
			}
			else {
				insertTileAt( ix, xf0, yf0, xf1, yf1 );
			}
		}
	}
}


void MicroTileArray::insertTileAt( const uint32& index, const int32& x0, const int32& y0, const int32& x1, const int32& y1 )
{
	int size = this->m_UTiles.size();
	if ( index >= size ) {
		//resize the array
		m_UTiles.resize( index, NULL );	
	}
	
	MicroTileBBox* bbox = m_UTiles[index];
	if ( NULL == bbox ){
		m_UTiles[index] = new MicroTileBBox( x0, y0, x1, y1 );
	}
	else {
		bbox->setBBox( x0, y0, x1, y1 );
	}

}

void MicroTileArray::insertTileAt( const uint32& index, const MicroTileBBox& bbox )
{
	int size = this->m_UTiles.size();
	if ( index >= size ) {
		//resize the array
		m_UTiles.resize( index, NULL );	
	}
	
	MicroTileBBox* tile = m_UTiles[index];
	if ( NULL == tile ){
		m_UTiles[index] = new MicroTileBBox( bbox );
	}
	else {
		*tile = bbox;
	}
}

void MicroTileArray::setNewCoords( const int32& x0, const int32& y0, const int32& x1, const int32& y1, const int32 shiftBy )
{
	m_x0 = x0 >> shiftBy;
	m_y0 = y0 >> shiftBy;
	int32 x = 1 + (x1 >> shiftBy);
	int32 y = 1 + (y1 >> shiftBy);

	m_width = x - x0;
	m_height = y - y0;
	this->m_UTiles.clear();
}

MicroTileBBox* MicroTileArray::addMicroTile( const int32& x0, const int32& y0, const int32& x1, const int32& y1 )
{
	MicroTileBBox* result = new MicroTileBBox( x0, y0, x1, y1 );
	this->m_UTiles.push_back( result );
	return result;
}

void MicroTileArray::addMicroTile( MicroTileBBox* bbox )
{
	this->m_UTiles.push_back( bbox );	
}


MicroTileArray* MicroTileArray::unionOf( MicroTileArray* tileArray )
{
	MicroTileArray* result = NULL;
	
	int32 x0, y0, x1, y1;
	int32 x, y;
	int32 ix, ix1, ix2;
	MicroTileBBox bb;
	MicroTileBBox bb1;
	MicroTileBBox bb2;
	
	x0 = MIN(this->m_x0, tileArray->getX0());
	y0 = MIN(this->m_y0, tileArray->getY0());
	x1 = MAX(this->m_x0 + this->m_width, tileArray->getX0() + tileArray->getWidth() );
	y1 = MAX(this->m_y0 + this->m_height, tileArray->getY0() + tileArray->getHeight() );
	
	result = new MicroTileArray( x0, y0, x1, y1 );
	
	/* could move the first two if/else statements out of the loop */
	ix = 0;
	for (y = y0; y < y1; y++) {
		ix1 = (y - this->m_y0) * this->m_width + x0 - this->m_x0;
		ix2 = (y - tileArray->getY0()) * tileArray->getWidth() + x0 - tileArray->getX0();
		for (x = x0; x < x1; x++) {
			if ( (x < this->m_x0) || (y < this->m_y0) ||
				 (x >= this->m_x0 + this->m_width) || (y >= this->m_y0 + this->m_height) ){
				bb1 = 0;
			}
			else {
				bb1 = *this->m_UTiles[ix1];
			}
			if ( (x < tileArray->getX0()) || (y < tileArray->getY0()) ||
				 (x >= tileArray->getX0() + tileArray->getWidth()) || (y >= tileArray->getY0() + tileArray->getHeight()) ) {
				bb2 = 0;
			}
			else {
				bb2 = *((*tileArray)[ix2]);
			}
			if (bb1 == 0) {
				bb = bb2;
			}
			else if (bb2 == 0) {
				bb = bb1;
			}
			else {
				bb = VCF_UTA_BBOX_CONS( MIN( bb1.getX0(), bb2.getX0() ),
										MIN( bb1.getY0(), bb2.getY1() ),
										MAX( bb1.getX1(), bb2.getX1() ),
										MAX( bb1.getY1(), bb2.getY1() ) );
			}
			result->addMicroTile( new MicroTileBBox(bb) );
			//uta->utiles[ix] = bb;
			ix++;
			ix1++;
			ix2++;
		}
    }	
	return result;
}