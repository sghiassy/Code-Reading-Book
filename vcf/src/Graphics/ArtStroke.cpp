//ArtStroke.cpp
#include "GraphicsKit.h"
#include "ArtStroke.h"
#include "libart.h"

using namespace VCF;

ArtStroke::ArtStroke() 
{
	init();
}

ArtStroke::~ArtStroke()
{

}

void ArtStroke::init()
{
	m_context = NULL;
	m_width  = 21.0;
	m_color = *Color::getColor( RED );
}

void ArtStroke::render( Path * path ){
	if ( (NULL != m_context) && (NULL != path) ){		
		
		Matrix2D* currentXFrm = m_context->getCurrentTransform();
		PathEnumerator* pathEnum = path->getPathEnumerator( currentXFrm );
		Rect* pathBounds = pathEnum->getBounds();
		ArtVpath *vec = art_new(ArtVpath,1);			
		Point lastPt;
		int totPts = 1;
		int i = 0;
		if ( NULL != pathEnum ){
			while ( true == pathEnum->hasMoreSegments() ){
				ulong32 count = pathEnum->getPointCount();
				Point* pts = NULL;
				if ( count > 0 ){
					pts = new Point[count];
					//totPts+= count;
				}
				PathEnumerator::SegmentType type = pathEnum->getSegment( pts );
				switch ( type ){
				case PathEnumerator::SEGMENT_MOVETO: {
					if ( i == 0 ) {
						vec[0].code = ART_MOVETO;
						vec[0].x = pts[0].m_x;
						vec[0].y = pts[0].m_y;
						i++;
					}
					else {
						art_vpath_add_point( &vec, &i, &totPts, ART_MOVETO, pts[0].m_x, pts[0].m_y );
						totPts = i;
					}
					lastPt = pts[0];
					
													 }
					break;
					
				case PathEnumerator::SEGMENT_LINETO: {
					if ( i == 0 ) {
						vec[0].code = ART_LINETO;
						vec[0].x = pts[0].m_x;
						vec[0].y = pts[0].m_y;
						i++;
					}
					else {
						art_vpath_add_point( &vec, &i, &totPts, ART_LINETO, pts[0].m_x, pts[0].m_y );
						totPts = i;
					}
					lastPt = pts[0];
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
			art_vpath_add_point( &vec, &i, &totPts, ART_END, lastPt.m_x, lastPt.m_y );
			
			
			ArtDRect r = {0};
			art_vpath_bbox_drect( vec, &r );
			
			//
			double mat[6];
			art_affine_identity( mat );
			art_affine_translate( mat, -r.x0, -r.y0 );
			
			ArtVpath* xFrmedVec = art_vpath_affine_transform( vec, mat );
			
			ArtSVP *svp = art_svp_vpath_stroke (xFrmedVec,
				ART_PATH_STROKE_JOIN_ROUND,
				ART_PATH_STROKE_CAP_ROUND, m_width, 4, 1.0);
			
			art_drect_svp( &r, svp );
			//art_affine_translate
			Rect rr( r.x0, r.y0, r.x1, r.y1 );		
			
			//UIGraphicsToolkit::getDefaultGraphicsToolkit()->createImage( rr.getWidth(), rr.getHeight() );
			
			Image* image = GraphicsToolkit::getDefaultGraphicsToolkit()->createImage( m_context, pathBounds ); 
			
			if ( NULL != image ) {
				int w = image->getWidth();
				int h = image->getHeight();
				unsigned char* buf = (unsigned char*)image->getImageBits()->m_bits;// new unsigned char[(h * w) * 3];				
				RGBAVals * tmp = image->getImageBits()->m_bits;
				//bgra
				unsigned long c = h * w;
				unsigned char tmpByte = 0;
				for ( int i=0;i<c;i++){
					tmp[i].a = 255;
					tmpByte = tmp[i].b;
					tmp[i].b = tmp[i].r;
					tmp[i].r = tmpByte;
				}

				//art_rgb_svp_aa( svp, 0, 0, w, h, 0xFF0000, 0x00FFff, buf, w * 3, NULL ); 
				
				art_rgb_svp_alpha( svp, 0, 0, w, h, 0xFF00FF20, buf, w*4, NULL );
				
				
				//unsigned long c = h * w;
				for ( i=0;i<c;i++){
					/*
					tmp[i].b = buf[i * 3];
					tmp[i].g = buf[i * 3 + 1];
					tmp[i].r = buf[i * 3 + 2];
					*/
					tmpByte = tmp[i].r;
					tmp[i].r = tmp[i].b;
					tmp[i].b = tmpByte;
				}
				
				m_context->drawImage( pathBounds->m_left, pathBounds->m_top, image );
				delete image;
				//delete []buf;
			}
			m_context->setColor( &m_color );
			m_context->rectangle( pathBounds );
			m_context->strokePath();
			
			art_free(vec);
			art_free(xFrmedVec);
			art_free(svp);				
			}
		}
		//m_context->strokePath();
	}