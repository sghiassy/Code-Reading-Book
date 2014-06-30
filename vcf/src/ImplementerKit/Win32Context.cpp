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

// Win32Context.cpp

#include "GraphicsKit.h"
#include "Win32Context.h"
#include "Win32Image.h"
#include "RuntimeException.h"

using namespace VCF;



Win32Context::Win32Context()
{
	init();
}

Win32Context::Win32Context( const unsigned long& width, const unsigned long& height )
{
	init();
	
	HDC desktopDC = ::GetDC( ::GetDesktopWindow() );

	m_dc = ::CreateCompatibleDC( desktopDC );
	if ( NULL == m_dc ) {
		throw RuntimeException( MAKE_ERROR_MSG_2("Unable to create compatible Device Context for win32 context") );
	}
	m_memBitmap = ::CreateCompatibleBitmap( desktopDC, width, height );
	::SelectObject( m_dc, m_memBitmap );
	ReleaseDC( ::GetDesktopWindow(), desktopDC );
	m_isMemoryCtx = true;
	if ( NULL == m_memBitmap ){
		//throw exception 
		throw RuntimeException( MAKE_ERROR_MSG_2("Unable to create memory bitmap for win32 context") );
	}
}

Win32Context::Win32Context( const long& contextID )
{
	init();
	m_dc = (HDC)contextID;
}

Win32Context::~Win32Context()
{

	if ( NULL != m_memBitmap ){
		::DeleteObject( m_memBitmap );		
	}
	if ( NULL != m_dc ) {
		::DeleteDC( m_dc );
	}
	m_dc = NULL;

	clearBuffer();
}

void Win32Context::init()
{
	m_dc = NULL;
	m_context = NULL;
	m_memBitmap = NULL;	
	m_pathOperations.clear();
	m_textOperations.clear();
	m_pathStarted = false;
	m_isMemoryCtx = false;
	m_oldOrigin.m_x = 0.0;
	m_oldOrigin.m_y = 0.0;
	m_origin.m_x = 0.0;
	m_origin.m_y = 0.0;
	m_isXORModeOn = false;
	m_alignToBaseline = false;
}

void Win32Context::releaseHandle()
{
	clearBuffer();
}

void Win32Context::setCurrentFontState(VCF::FontState * state)
{
	m_currentFontState = *state;
}


VCF::FontState* Win32Context::getCurrentFontState()
{
	return &m_currentFontState;
}

void Win32Context::drawPartialImage( const double& x, const double& y, Rect* imageBounds, Image* image )
{
	checkHandle();
	if ( (imageBounds->getWidth() > image->getWidth()) || (imageBounds->getHeight() > image->getHeight()) ) {
		throw BasicException( MAKE_ERROR_MSG("Invalid image bounds reqiested"), __LINE__);
	}
	Win32Image* win32image = dynamic_cast<Win32Image*>(image);
	if ( NULL != win32image ){
		//HDC bmpDC = win32image->getDC();
		ImageBits* bits = win32image->getImageBits();
		HPALETTE oldPalette = NULL;
		if ( NULL != win32image->m_palette ){
			oldPalette = ::SelectPalette( m_dc, win32image->m_palette, FALSE );
			::RealizePalette( m_dc );
		}
		
		BITMAPINFO bmpInfo;
		memset( &bmpInfo, 0, sizeof(BITMAPINFO) );
		//memset( &bmpInfo.bmiHeader, 0, sizeof (BITMAPINFOHEADER) );
		bmpInfo.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth = imageBounds->getWidth();
		bmpInfo.bmiHeader.biHeight = -imageBounds->getHeight(); // Win32 DIB are upside down - do this to filp it over
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 32; 
		bmpInfo.bmiHeader.biCompression = BI_RGB;
		bmpInfo.bmiHeader.biSizeImage = (bmpInfo.bmiHeader.biHeight) * bmpInfo.bmiHeader.biWidth * 4;
		RGBAVals* bmpBuf = NULL;
		RGBAVals* tmpBmpBuf = NULL;
		HBITMAP hbmp = CreateDIBSection ( NULL, &bmpInfo, DIB_RGB_COLORS, (void**)&bmpBuf, NULL, NULL ); 
		RGBAVals* imageBuf = image->getImageBits()->m_bits;
		if ( NULL != hbmp ) {
			
			int incr = ((imageBounds->m_top * image->getWidth()) + imageBounds->m_left);
			imageBuf += incr;
			tmpBmpBuf = bmpBuf;
			int imgWidth = image->getWidth();
			int wIncr = imageBounds->getWidth();
			int s = (int)imageBounds->m_top;
			int e = (int)imageBounds->m_bottom;
			for (int y1=s;y1<e;y1++) {
				memcpy( tmpBmpBuf, imageBuf, wIncr*4 );				
				tmpBmpBuf += wIncr;
				imageBuf += imgWidth;
			}
			
			if ( true == image->isTransparent() )  {
				Color* imageTransColor = image->getTransparencyColor();
				COLORREF transColor = RGB( imageTransColor->getRed()*255, 
					imageTransColor->getGreen()*255, 
					imageTransColor->getBlue()*255 );
				
				Win32Context::drawTransparentBitmap( m_dc, hbmp, x, y, transColor );
			}
			else {
				SetDIBitsToDevice( m_dc, 
									x, 
									y, 
									imageBounds->getWidth(), 
									imageBounds->getHeight(), 
									0, 
									0, 
									0, 
									imageBounds->getHeight(),
									bmpBuf,
									&bmpInfo, 
									DIB_RGB_COLORS );				
			}
			
			DeleteObject( hbmp );
		}
		
		if ( NULL != oldPalette ){
			::SelectPalette( m_dc, oldPalette, FALSE );			
		}

	}
	else {
		throw InvalidImage( "Image Peer is not usable under MS Windows" );
	}
	releaseHandle();
}

void Win32Context::drawImageWithinBounds( Rect* bounds, Image* image )
{
	checkHandle();
	Win32Image* win32image = dynamic_cast<Win32Image*>(image);
	if ( NULL != win32image ){
		//HDC bmpDC = win32image->getDC();
		if ( true == image->isTransparent() )  {
			Color* imageTransColor = image->getTransparencyColor();
			COLORREF transColor = RGB( imageTransColor->getRed()*255, 
										imageTransColor->getGreen()*255, 
										imageTransColor->getBlue()*255 );

			HBITMAP imgHBmp = win32image->getBitmap();
			HBITMAP tmpBMP = (HBITMAP)CopyImage( imgHBmp, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG );
			if ( NULL == tmpBMP ) {
				throw InvalidImage( "Image Peer's bitmap handle is NULL or bad - unable to copy image." );
			}
			Win32Context::drawTransparentBitmap( m_dc, tmpBMP, bounds->m_left, bounds->m_top, transColor );
			DeleteObject( tmpBMP );
		}
		else {
			ImageBits* bits = win32image->getImageBits();
			HPALETTE oldPalette = NULL;
			if ( NULL != win32image->m_palette ){
				oldPalette = ::SelectPalette( m_dc, win32image->m_palette, FALSE );
				::RealizePalette( m_dc );
			}
			SetDIBitsToDevice( m_dc, 
								bounds->m_left, 
								bounds->m_top, 
								bounds->getWidth(), 
								bounds->getHeight(), 
								0, 
								0, 
								0, 
								bounds->getHeight(),
								bits->m_bits,
								&win32image->m_bmpInfo, 
								DIB_RGB_COLORS );
			if ( NULL != oldPalette ){
				::SelectPalette( m_dc, oldPalette, FALSE );			
			}
		}
	}
	releaseHandle();
}

void Win32Context::drawImage( const double & x, const double & y, Image* image )
{
	checkHandle();
	Win32Image* win32image = dynamic_cast<Win32Image*>(image);
	if ( NULL != win32image ){
		//HDC bmpDC = win32image->getDC();
		ImageBits* bits = win32image->getImageBits();
		if ( true == image->isTransparent() )  {
			Color* imageTransColor = image->getTransparencyColor();
			COLORREF transColor = RGB( (unsigned char)(imageTransColor->getRed()*255), 
										(unsigned char)(imageTransColor->getGreen()*255), 
										(unsigned char)(imageTransColor->getBlue()*255) );

			HBITMAP imgHBmp = win32image->getBitmap();
			HBITMAP tmpBMP = (HBITMAP)CopyImage( imgHBmp, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG );
			if ( NULL == tmpBMP ) {
				throw InvalidImage( "Image Peer's bitmap handle is NULL or bad - unable to copy image." );
			}
			Win32Context::drawTransparentBitmap( m_dc, tmpBMP, x, y, transColor );
			DeleteObject( tmpBMP );
		}
		else {
			HPALETTE oldPalette = NULL;
			if ( NULL != win32image->m_palette ){
				oldPalette = ::SelectPalette( m_dc, win32image->m_palette, FALSE );
				::RealizePalette( m_dc );
			}
			SetDIBitsToDevice( m_dc, 
								x, 
								y, 
								win32image->getWidth(), 
								win32image->getHeight(), 
								0, 
								0, 
								0, 
								win32image->getHeight(),
								bits->m_bits,
								&win32image->m_bmpInfo, 
								DIB_RGB_COLORS );
			if ( NULL != oldPalette ){
				::SelectPalette( m_dc, oldPalette, FALSE );			
			}	
		}
	}
	releaseHandle();
}

void Win32Context::textAt(const double & x, const double & y, const String & text)
{	
	checkHandle();
	if ( NULL == m_context ){
		//throw exception !
	}
	int oldBkMode = ::SetBkMode( m_dc, TRANSPARENT );

	UINT textAlignment = (m_alignToBaseline) ? TA_BASELINE | TA_LEFT : TA_TOP | TA_LEFT ;
	UINT oldTextAlignment = ::SetTextAlign( m_dc, textAlignment );
	Font* ctxFont = m_context->getCurrentFont();
	if ( NULL == ctxFont ){
		//throw exception
	}
	FontPeer* fontImpl = ctxFont->getFontPeer();
	
	if ( NULL == fontImpl ){
		//throw exception
	}

	LOGFONT* logFont = (LOGFONT*)fontImpl->getFontHandleID();

	
	HFONT font = CreateFontIndirect( logFont );
	
	Color* fontColor = ctxFont->getColor();
	COLORREF oldFontColor = ::GetTextColor( m_dc );
	if ( NULL != fontColor ){
		oldFontColor = ::SetTextColor( m_dc, fontColor->getRGB() );
	}
	else {
		oldFontColor = ::SetTextColor( m_dc, RGB(0,0,0) );
	}

	HFONT oldFont = (HFONT)::SelectObject( m_dc, font );
	
	char* textToDraw = new char[text.size()+1];
	memset( textToDraw, 0, text.size()+1 );
	text.copy( textToDraw, text.size() );
	RECT r = {0};
	r.left = r.right = x;
	r.top = r.bottom = y;
	UINT formatOptions = 0;
	formatOptions = DT_LEFT | DT_TOP | DT_NOCLIP;
	/* Not using for now
	DRAWTEXTPARAMS extraParams = {0};
	extraParams.cbSize = sizeof(DRAWTEXTPARAMS);
	*/
	
	/*
	*determine the size of the text and adjust the rect accordingly.
	*can't use DrawText to figure out the rect because it doesn't 
	*calc the bounds correctly.
	*/
	SIZE textSize = {0};
	GetTextExtentPoint32( m_dc, text.c_str(), text.size(), &textSize );
	r.right = r.left + textSize.cx;
	r.bottom = r.top + textSize.cy;

	DrawTextEx( m_dc, textToDraw, text.size(), &r, formatOptions, NULL ); 
	
	//clean up after ourselves
	delete[] textToDraw;
	::SelectObject( m_dc, oldFont );
	::DeleteObject( font );
	::SetTextColor( m_dc, oldFontColor );
	::SetBkMode( m_dc, oldBkMode );
	::SetTextAlign( m_dc, oldTextAlignment );
	releaseHandle();
}

void Win32Context::rectangle(const double & x1, const double & y1, const double & x2, const double & y2)
{
	testBuffer();
	m_pathStarted = true;	
	PointOperation* newPointOp = new PointOperation();
	newPointOp->primitive = PRIMITIVE_RECT;
	newPointOp->x = x1;
	newPointOp->y = y1;
	m_pathOperations.push_back( newPointOp );
	
	newPointOp = new PointOperation();
	newPointOp->primitive = PRIMITIVE_RECT;
	newPointOp->x = x2;
	newPointOp->y = y2;
	m_pathOperations.push_back( newPointOp );
	
}

void Win32Context::circle(const double & x, const double & y, const double & radius)
{	
	ellipse( x - radius, y - radius, x + radius, y + radius );	
}

void Win32Context::ellipse(const double & x1, const double & y1, const double & x2, const double & y2, const double & angle)
{
	testBuffer();
	m_pathStarted = true;	
	//swap out the values to ensure they are normalized since windows is brain dead about this
	double ax1 = x1;
	double ay1 = y1;
	double ax2 = x2;
	double ay2 = y2;
	
	double tmp = x2;
	if ( ax1 > ax2 ) {
		ax2 = ax1;
		ax1 = tmp;
	}
	tmp = ay2;
	if ( ay1 > ay2 ) {
		ay2 = ay1;
		ay1 = tmp;
	}
	PointOperation* newPointOp = new PointOperation();
	newPointOp->primitive = PRIMITIVE_ELLIPSE;		
	newPointOp->x = ax1;
	newPointOp->y = ay1;
	m_pathOperations.push_back( newPointOp );
	
	newPointOp = new PointOperation();
	newPointOp->primitive = PRIMITIVE_ELLIPSE;
	newPointOp->x = ax2;
	newPointOp->y = ay2;
	m_pathOperations.push_back( newPointOp );
	
}

void Win32Context::setCurrentStrokeState(VCF::StrokeState * state)
{
	m_currentStroke = *state;
}

void Win32Context::setCurrentFillState(VCF::FillState * state)
{
	m_currentFill = *state;
}

void Win32Context::polyline( std::vector<Point*> & pts)
{
	testBuffer();
	m_pathStarted = true;
	std::vector<Point*>::iterator it = pts.begin();
	PointOperation* newPointOp = NULL;
	while ( it != pts.end() ){			
		newPointOp = new PointOperation();
		newPointOp->primitive = PRIMITIVE_POLYLINE;
		newPointOp->x = (*it)->m_x;
		newPointOp->y = (*it)->m_y;
		m_pathOperations.push_back( newPointOp );
		it++;
	}	
}

void Win32Context::curve(const double & x1, const double & y1, const double & x2, const double & y2,
                         const double & x3, const double & y3, const double & x4, const double & y4 )
{
	testBuffer();
	m_pathStarted = true;
	PointOperation* newPointOp = NULL;
	//start point 1
	newPointOp = new PointOperation();
	newPointOp->primitive = PRIMITIVE_BEZIER;
	newPointOp->x = x1;
	newPointOp->y = y1;
	m_pathOperations.push_back( newPointOp );

	//start point 1
	newPointOp = new PointOperation();
	newPointOp->primitive = PRIMITIVE_BEZIER;
	newPointOp->x = x2;
	newPointOp->y = y2;
	m_pathOperations.push_back( newPointOp );

	//start point 1
	newPointOp = new PointOperation();
	newPointOp->primitive = PRIMITIVE_BEZIER;
	newPointOp->x = x3;
	newPointOp->y = y3;
	m_pathOperations.push_back( newPointOp );

	//start point 1
	newPointOp = new PointOperation();
	newPointOp->primitive = PRIMITIVE_BEZIER;
	newPointOp->x = x4;
	newPointOp->y = y4;
	m_pathOperations.push_back( newPointOp );
}

void Win32Context::lineTo(const double & x, const double & y)
{
	testBuffer();
	m_pathStarted = true;
	PointOperation* newPointOp = new PointOperation();
	newPointOp->primitive = PRIMITIVE_LINE;
	newPointOp->x = x;
	newPointOp->y = y;
	m_pathOperations.push_back( newPointOp );	
}

void Win32Context::moveTo(const double & x, const double & y)
{
	testBuffer();
	m_pathStarted = true;
	PointOperation* newPointOp = new PointOperation();
	newPointOp->primitive = PRIMITIVE_MOVE;
	newPointOp->x = x;
	newPointOp->y = y;
	m_pathOperations.push_back( newPointOp );	
}

void Win32Context::testBuffer()
{
	/**
	*means the path has been finished by a call to fill or stroke path
	*/
	if ( false == m_pathStarted ){
		clearBuffer();
	}
}

void Win32Context::fillPath()
{
	checkHandle();

	m_inFillPath = true;
	m_pathStarted = false;
	
	if ( NULL != m_dc ){
		LOGBRUSH logBrush = {0};
		logBrush.lbColor = m_currentFill.m_Color.getRGB();
		logBrush.lbStyle = translateFillStyle( m_currentFill.m_style );
		logBrush.lbHatch = translateHatch( m_currentFill.m_style );
		
		HBRUSH fill = ::CreateBrushIndirect( &logBrush );
		HBRUSH oldBrush = (HBRUSH)SelectObject( m_dc, fill );
		
		HPEN nullPen = ::CreatePen( PS_NULL, 0, 0 );
		HPEN oldPen = (HPEN)SelectObject( m_dc, nullPen );			
		
		execPathOperations();
		
		SelectObject( m_dc, oldBrush );
		DeleteObject( fill );
		
		SelectObject( m_dc, oldPen );
		DeleteObject( nullPen );
	}	

	releaseHandle();
}

void Win32Context::strokePath()
{
	checkHandle();

	m_inFillPath = false;
	m_pathStarted = false;
	
	LOGPEN logPen = {0};
	logPen.lopnColor = m_currentStroke.m_Color.getRGB();
	logPen.lopnStyle = translateStrokeStyle( m_currentStroke.m_style );
	logPen.lopnWidth.x = m_currentStroke.m_width;
	
	HPEN stroke = ::CreatePenIndirect( &logPen );
	HPEN oldPen = (HPEN)::SelectObject( m_dc, stroke );
	
	LOGBRUSH logBrush = {0};
	logBrush.lbColor = 0;
	logBrush.lbHatch = 0;
	logBrush.lbStyle = BS_HOLLOW;
	HBRUSH nullBrush = ::CreateBrushIndirect( &logBrush );
	HBRUSH oldBrush = (HBRUSH)::SelectObject( m_dc, nullBrush );
	
	int oldXORMode = 0;
	if ( m_isXORModeOn ) {
		oldXORMode = SetROP2( m_dc, R2_NOTXORPEN );
	}
	execPathOperations();
	
	SelectObject( m_dc, oldBrush );
	DeleteObject( nullBrush );
	
	SelectObject( m_dc, oldPen );
	DeleteObject( stroke );		
	if ( m_isXORModeOn ) {
		SetROP2( m_dc, oldXORMode );
	}
	releaseHandle();
}

void Win32Context::clearBuffer()
{
	clearPathBuffer();
	clearTextBuffer();
}

void Win32Context::clearPathBuffer()
{
	std::vector<PointOperation*>::iterator pts = m_pathOperations.begin();
	while ( pts != m_pathOperations.end() ){
		delete (*pts);
		pts++;
	}
	m_pathOperations.clear();
}

void Win32Context::clearTextBuffer()
{

}

long Win32Context::translateFillStyle( const FillStyle& fillState )
{
	long result = BS_HOLLOW;
	switch ( fillState ){
		case FILL_NONE:{
			result = BS_HOLLOW;	
		}
		break;

		case FILL_SOLID:{
			result = BS_SOLID;	
		}
		break;

		case FILL_DIAGONAL:{
			result = BS_HATCHED;	
		}
		break;

		case FILL_CROSS:{
			result = BS_HATCHED;	
		}
		break;

		case FILL_DIAGCROSS:{
			result = BS_HATCHED;	
		}
		break;

		case FILL_HORZ:{
			result = BS_HATCHED;	
		}
		break;
		
		case FILL_VERT:{
			result = BS_HATCHED;	
		}
		break;
	}

	return result;
}

long Win32Context::translateHatch( const FillStyle& fillState )
{
	long result = 0;
	switch ( fillState ){
		case FILL_DIAGONAL:{
			result = HS_BDIAGONAL;	
		}
		break;

		case FILL_CROSS:{
			result = HS_DIAGCROSS;	
		}
		break;

		case FILL_DIAGCROSS:{
			result = BS_SOLID;	
		}
		break;

		case FILL_HORZ:{
			result = HS_HORIZONTAL;	
		}
		break;
		
		case FILL_VERT:{
			result = HS_VERTICAL;	
		}
		break;
	}

	return result;
}

void Win32Context::execPathOperations()
{
	std::vector<PointOperation*>::iterator pts = m_pathOperations.begin();
	long remaingOps = m_pathOperations.size();
	while ( (pts != m_pathOperations.end()) && (remaingOps > 0) ){	
		PointOperation* pointOp = *pts;
		if ( NULL != pointOp ){
			switch ( pointOp->primitive ){
				case PRIMITIVE_MOVE:{
					m_currentMoveTo.m_x = pointOp->x;
					m_currentMoveTo.m_y = pointOp->y;
					::MoveToEx( m_dc, pointOp->x, pointOp->y, NULL );
				}
				break;

				case PRIMITIVE_LINE:{
					::LineTo( m_dc, pointOp->x, pointOp->y );
				}
				break;
				
				case PRIMITIVE_POLYLINE:{
					std::vector<PointOperation*> tmpPts;
					PointOperation* tmpPtOp = NULL;
					while ( (pts != m_pathOperations.end()) && (pointOp->primitive == PRIMITIVE_POLYLINE) ) {
						tmpPtOp = new PointOperation();
						*tmpPtOp = *pointOp;
						tmpPts.push_back( tmpPtOp );
						pts++;
						pointOp = *pts;
						remaingOps --;
					}

					POINT* polyPts = new POINT[tmpPts.size()];
					std::vector<PointOperation*>::iterator it = tmpPts.begin();
					int i =0;
					while ( it != tmpPts.end() ){
						polyPts[i].x = (*it)->x;
						polyPts[i].y = (*it)->y;
						delete (*it);
						it++;
						i++;
					}
					if ( m_inFillPath ){
						::Polygon( m_dc, polyPts,  tmpPts.size() );
					}
					else{
						::Polyline( m_dc, polyPts,  tmpPts.size() );
					}					
					
					delete[] polyPts;
					tmpPts.clear();
				}
				break;

				case PRIMITIVE_BEZIER:{
					if ( (remaingOps - 4) < 0 ){
						//throw exception !!
					}
					POINT bezPts[4] = {0};
					bezPts[0].x = pointOp->x;	
					bezPts[0].y = pointOp->y;
					pts++;
					pointOp = *pts;
					bezPts[1].x = pointOp->x;	
					bezPts[1].y = pointOp->y;
					pts++;
					pointOp = *pts;
					bezPts[2].x = pointOp->x;	
					bezPts[2].y = pointOp->y;
					pts++;
					pointOp = *pts;
					bezPts[3].x = pointOp->x;	
					bezPts[3].y = pointOp->y;
					if ( m_inFillPath ){
						::BeginPath( m_dc );
						::PolyBezier( m_dc, bezPts, 4 );
						::EndPath( m_dc );
						::FillPath( m_dc );
					}
					else {
						::PolyBezier( m_dc, bezPts, 4 );
					}
					remaingOps -= 4;
				}
				break;

				case PRIMITIVE_RECT:{
					if ( (remaingOps - 1) < 0 ){
						//throw exception !!
					}
					POINT pt = {0};					
					pt.x = pointOp->x;	
					pt.y = pointOp->y;
					pts++;
					pointOp = *pts;
					int fixVal = 0;
					if ( true == m_inFillPath ){
						fixVal = 1;
					}
					::Rectangle( m_dc, pt.x, pt.y, pointOp->x + fixVal, pointOp->y + fixVal );
					remaingOps -= 1;
				}
				break;

				case PRIMITIVE_ELLIPSE:{
					if ( (remaingOps - 1) < 0 ){
						//throw exception !!
					}
					POINT pt = {0};					
					pt.x = pointOp->x;	
					pt.y = pointOp->y;
					pts++;
					pointOp = *pts;
					int fixVal = 0;
					if ( true == m_inFillPath ){
						fixVal = 1;
					}

					::Ellipse( m_dc, pt.x, pt.y, pointOp->x + fixVal, pointOp->y + fixVal );
					remaingOps -= 1;
				}
				break;

				case PRIMITIVE_ARC:{
					
				}
				break;

				case PRIMITIVE_CLOSE:{
					::LineTo( m_dc, m_currentMoveTo.m_x, m_currentMoveTo.m_y );					
				}
				break;
			}			
		}
		pts ++;
		remaingOps --;
	}
}

long Win32Context::translateStrokeStyle( const StrokeStyle& strokeStyle )
{
	return 0;
}

StrokeState* Win32Context::getCurrentStrokeState()
{
	return &m_currentStroke;
}

FillState* Win32Context::getCurrentFillState()
{
	return &m_currentFill;
}

long Win32Context::getContextID()
{
	return (long)m_dc;
}

void Win32Context::setOrigin( const double& x, const double& y )
{
	checkHandle();

	POINT pt = {0};
	::SetViewportOrgEx( m_dc, x, y, &pt );
	m_oldOrigin.m_x = pt.x;
	m_oldOrigin.m_y = pt.y;

	releaseHandle();
}

Point* Win32Context::getOrigin()
{
	checkHandle();
	POINT pt = {0};
	::GetViewportOrgEx( m_dc, &pt );
	m_origin.m_x = pt.x;
	m_origin.m_y = pt.y;

	releaseHandle();

	return &m_origin;
}
void Win32Context::copyContext( const double& x, const double& y, ContextPeer* context )
{
	if ( NULL != context ){
		checkHandle();

		HDC dc = (HDC)context->getContextID();

		
		HBITMAP currentBitmap = NULL;
		if ( true == context->isMemoryContext() ){
			currentBitmap = (HBITMAP)::GetCurrentObject( dc, OBJ_BITMAP );
		}
		else if ( true == isMemoryContext() ){
			currentBitmap = (HBITMAP)::GetCurrentObject( m_dc, OBJ_BITMAP );
		}

		char* bmpBuffer = NULL;
		long bufferSize = ::GetObject( currentBitmap, sizeof(BITMAP), NULL );
		bmpBuffer = new char[bufferSize];
		memset( bmpBuffer, 0, bufferSize );
		::GetObject( currentBitmap, bufferSize, bmpBuffer );
		BITMAP* bitmap = (BITMAP*)bmpBuffer;

		::BitBlt( m_dc, x, y, bitmap->bmWidth, abs(bitmap->bmHeight), dc, 0, 0, SRCCOPY );		

		delete [] bmpBuffer;
		bmpBuffer = NULL;

		releaseHandle();

	}
	//else throw exception ???
}

bool Win32Context::isMemoryContext()
{
	return m_isMemoryCtx;
}

void Win32Context::copyToImage( Win32Image* image )
{

}

HDC Win32Context::getDC()
{
	return m_dc;
}

void Win32Context::setContext( GraphicsContext* context )
{
	m_context = context;
}

GraphicsContext* Win32Context::getContext()
{
	return m_context;
}

void Win32Context::setContextID( const long& handle )
{
	m_dc = (HDC)handle;
}

void Win32Context::textBoundedBy( Rect* bounds, const String& text, const bool& wordWrap )
{
	checkHandle();
	if ( NULL == m_context ){
		//throw exception !
	}
	int oldBkMode = ::SetBkMode( m_dc, TRANSPARENT );

	UINT textAlignment = (m_alignToBaseline) ? TA_BASELINE | TA_LEFT : TA_TOP | TA_LEFT ;
	UINT oldTextAlignment = ::SetTextAlign( m_dc, textAlignment );

	Font* ctxFont = m_context->getCurrentFont();
	if ( NULL == ctxFont ){
		//throw exception
	}
	FontPeer* fontImpl = ctxFont->getFontPeer();
	
	if ( NULL == fontImpl ){
		//throw exception
	}

	LOGFONT* logFont = (LOGFONT*)fontImpl->getFontHandleID();

	
	HFONT font = CreateFontIndirect( logFont );
	
	Color* fontColor = ctxFont->getColor();
	COLORREF oldFontColor = ::GetTextColor( m_dc );
	if ( NULL != fontColor ){
		oldFontColor = ::SetTextColor( m_dc, fontColor->getRGB() );
	}
	else {
		oldFontColor = ::SetTextColor( m_dc, RGB(0,0,0) );
	}

	HFONT oldFont = (HFONT)::SelectObject( m_dc, font );
	
	VCFChar* textToDraw = new VCFChar[text.size()+1];
	memset( textToDraw, 0, text.size()+1 );
	text.copy( textToDraw, text.size() );
	RECT r = {0};
	r.left = bounds->m_left;
	r.right = bounds->m_right;
	r.top = bounds->m_top;
	r.bottom = bounds->m_bottom;
	UINT formatOptions = 0;
	formatOptions = DT_LEFT | DT_TOP;
	if ( true == wordWrap ) {
		formatOptions |= DT_WORDBREAK;
	}
	else {
		formatOptions |= DT_WORD_ELLIPSIS;
	}

	/* Not using for now
	DRAWTEXTPARAMS extraParams = {0};
	extraParams.cbSize = sizeof(DRAWTEXTPARAMS);
	*/
	
	/*
	*determine the size of the text and adjust the rect accordingly.
	*can't use DrawText to figure out the rect because it doesn't 
	*calc the bounds correctly.
	*/
	//SIZE textSize = {0};
	//GetTextExtentPoint32( m_dc, text.c_str(), text.size(), &textSize );
	//r.right = r.left + textSize.cx;
	//r.bottom = r.top + textSize.cy;

	DrawTextEx( m_dc, textToDraw, text.size(), &r, formatOptions, NULL ); 
	
	//clean up after ourselves
	delete[] textToDraw;
	::SelectObject( m_dc, oldFont );
	::DeleteObject( font );
	::SetTextColor( m_dc, oldFontColor );
	::SetBkMode( m_dc, oldBkMode );
	::SetTextAlign( m_dc, oldTextAlignment );

	releaseHandle();	
}

bool Win32Context::isXORModeOn()
{
	return m_isXORModeOn;
}

void Win32Context::setXORModeOn( const bool& XORModeOn )
{
	m_isXORModeOn = XORModeOn;
}

double Win32Context::getTextWidth( const String& text )
{
	checkHandle();
	if ( NULL == m_context ){
		//throw exception !
	}
	Font* ctxFont = m_context->getCurrentFont();
	if ( NULL == ctxFont ){
		//throw exception
	}
	FontPeer* fontImpl = ctxFont->getFontPeer();
	
	if ( NULL == fontImpl ){
		//throw exception
	}

	LOGFONT* logFont = (LOGFONT*)fontImpl->getFontHandleID();

	
	HFONT font = CreateFontIndirect( logFont );

	HFONT oldFont = (HFONT)::SelectObject( m_dc, font );

	SIZE textSize = {0};
	
	GetTextExtentPoint32( m_dc, text.c_str(), text.size(), &textSize );

	::SelectObject( m_dc, oldFont );
	::DeleteObject( font );
	releaseHandle();
	return textSize.cx;
}

double Win32Context::getTextHeight( const String& text )
{
	checkHandle();
	if ( NULL == m_context ){
		//throw exception !
	}
	Font* ctxFont = m_context->getCurrentFont();
	if ( NULL == ctxFont ){
		//throw exception
	}
	FontPeer* fontImpl = ctxFont->getFontPeer();
	
	if ( NULL == fontImpl ){
		//throw exception
	}

	LOGFONT* logFont = (LOGFONT*)fontImpl->getFontHandleID();

	
	HFONT font = CreateFontIndirect( logFont );

	HFONT oldFont = (HFONT)::SelectObject( m_dc, font );

	SIZE textSize = {0};
	
	GetTextExtentPoint32( m_dc, text.c_str(), text.size(), &textSize );

	::SelectObject( m_dc, oldFont );
	::DeleteObject( font );

	releaseHandle();
	return textSize.cy;
}

void Win32Context::setTextAlignment( const bool& alignTobaseline )
{
	m_alignToBaseline = alignTobaseline;
}

bool Win32Context::isTextAlignedToBaseline()
{
	return m_alignToBaseline;
}

/**
*Utility function to draw a transparent bitmap from MSDN 
*HOWTO: Drawing Transparent Bitmaps
*Article ID: Q79212  
*JC: This is an awfult lot of shit to just draw a transparent bmp.
*Maybe this will be revisited ???
*/
void Win32Context::drawTransparentBitmap(HDC hdc, HBITMAP hBitmap, short xStart,
                           short yStart, COLORREF cTransparentColor)
{
	BITMAP     bm;
	COLORREF   cColor;
	HBITMAP    bmAndBack, bmAndObject, bmAndMem, bmSave;
	HBITMAP    bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
	HDC        hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
	POINT      ptSize;
	
	hdcTemp = CreateCompatibleDC( NULL );//hdc);
	SelectObject(hdcTemp, hBitmap);   // Select the bitmap
	
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
	ptSize.x = bm.bmWidth;            // Get width of bitmap
	ptSize.y = bm.bmHeight;           // Get height of bitmap
	DPtoLP(hdcTemp, &ptSize, 1);      // Convert from device
	// to logical points
	
	// Create some DCs to hold temporary data.
	hdcBack   = CreateCompatibleDC(hdc);
	hdcObject = CreateCompatibleDC(hdc);
	hdcMem    = CreateCompatibleDC(hdc);
	hdcSave   = CreateCompatibleDC(hdc);
	
	// Create a bitmap for each DC. DCs are required for a number of
	// GDI functions.
	
	// Monochrome DC
	bmAndBack   = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
	
	// Monochrome DC
	bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
	
	bmAndMem    = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
	bmSave      = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
	
	// Each DC must select a bitmap object to store pixel data.
	bmBackOld   = (HBITMAP)SelectObject(hdcBack, bmAndBack);
	bmObjectOld = (HBITMAP)SelectObject(hdcObject, bmAndObject);
	bmMemOld    = (HBITMAP)SelectObject(hdcMem, bmAndMem);
	bmSaveOld   = (HBITMAP)SelectObject(hdcSave, bmSave);
	
	// Set proper mapping mode.
	SetMapMode(hdcTemp, GetMapMode(hdc));
	
	// Save the bitmap sent here, because it will be overwritten.
	BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);
	
	// Set the background color of the source DC to the color.
	// contained in the parts of the bitmap that should be transparent
	cColor = SetBkColor(hdcTemp, cTransparentColor);
	
	// Create the object mask for the bitmap by performing a BitBlt
	// from the source bitmap to a monochrome bitmap.
	BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0,
		SRCCOPY);
	
	// Set the background color of the source DC back to the original
	// color.
	SetBkColor(hdcTemp, cColor);
	
	// Create the inverse of the object mask.
	BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0,
		NOTSRCCOPY);
	
	// Copy the background of the main DC to the destination.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, xStart, yStart,
		SRCCOPY);
	
	// Mask out the places where the bitmap will be placed.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);
	
	// Mask out the transparent colored pixels on the bitmap.
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);
	
	// XOR the bitmap with the background on the destination DC.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);
	
	// Copy the destination to the screen.
	BitBlt(hdc, xStart, yStart, ptSize.x, ptSize.y, hdcMem, 0, 0,
		SRCCOPY);
	
	// Place the original bitmap back into the bitmap sent here.
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);
	
	// Delete the memory bitmaps.
	DeleteObject(SelectObject(hdcBack, bmBackOld));
	DeleteObject(SelectObject(hdcObject, bmObjectOld));
	DeleteObject(SelectObject(hdcMem, bmMemOld));
	DeleteObject(SelectObject(hdcSave, bmSaveOld));
	
	// Delete the memory DCs.
	DeleteDC(hdcMem);
	DeleteDC(hdcBack);
	DeleteDC(hdcObject);
	DeleteDC(hdcSave);
	DeleteDC(hdcTemp);
}


