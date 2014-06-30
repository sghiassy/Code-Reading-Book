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

//ImageList.cpp

#include "ApplicationKit.h"


#include "InvalidImage.h"

#include <algorithm>

using namespace VCF;

ImageList::ImageList()
{
	init();
}

ImageList::ImageList( Image* listOfImages, const unsigned long& imageWidth, const unsigned long& imageHeight )
{
	init();
}

ImageList::~ImageList()
{
	if ( NULL != m_masterImage ) {
		delete m_masterImage;
		m_masterImage = NULL;
	}
}

void ImageList::init()
{	
	INIT_EVENT_HANDLER_LIST(SizeChanged)
	INIT_EVENT_HANDLER_LIST(ImageAdded)
	INIT_EVENT_HANDLER_LIST(ImageDeleted)

	m_imageWidth = 32;
	m_imageHeight = 32;
	m_imageCount = 5;
	m_resizeIncrement = 5;
	m_totalImageCount = 0;
	m_masterImage = NULL;
	changed();
}

unsigned long ImageList::getImageWidth()
{
	return m_imageWidth;
}

unsigned long ImageList::getImageHeight()
{
	return m_imageHeight;
}

void ImageList::setImageWidth( const unsigned long& width )
{
	m_imageWidth = 	width;
	changed();
	ImageListEvent event( this, IMAGELIST_EVENT_WIDTH_CHANGED );
	fireOnSizeChanged( &event );
}

void ImageList::setImageHeight( const unsigned long& height )
{
	m_imageHeight = height;
	changed();
	ImageListEvent event( this, IMAGELIST_EVENT_HEIGHT_CHANGED );
	fireOnSizeChanged( &event );
}

void ImageList::setTransparentColor( Color* color )
{
	m_transparentColor.copy( color );
}

Color* ImageList::getTransparentColor()
{
	return &m_transparentColor;
}

void ImageList::addImage( Image* newImage )
{
	if ( newImage->getHeight() != m_imageHeight ) {
		//throw exception
		return;
	}

	if ( newImage->getWidth() != m_imageWidth ) {
		//throw exception
		return;
	}
	
	int incr  = m_imageWidth * m_totalImageCount;	

	m_totalImageCount++;
	if ( m_totalImageCount >= m_imageCount ) {
		//resize the master image
		m_imageCount += this->m_resizeIncrement;
		this->changed();
	}

	RGBAVals* bits = m_masterImage->getImageBits()->m_bits;
	bits += incr;
	RGBAVals* newImgBits = newImage->getImageBits()->m_bits;
	int scanlineWidthToCopy = newImage->getWidth();
	int scanlineWidthOfMasterImage = m_masterImage->getWidth();//scanlineToCopy * m_imageCount;
	for (int i=0;i<m_imageHeight;i++) {
		memcpy( bits, newImgBits, scanlineWidthToCopy*sizeof(RGBAVals) );
		
		bits += scanlineWidthOfMasterImage;
		newImgBits += scanlineWidthToCopy;
	}
	
	ImageListEvent event( this, IMAGELIST_EVENT_ITEM_ADDED, newImage );
	event.setIndexOfImage( m_totalImageCount-1 );
	fireOnImageAdded( &event );
}
	
void ImageList::insertImage( const unsigned long & index, Image* newImage )
{
	if ( newImage->getHeight() != m_imageHeight ) {
		//throw exception
		return;
	}

	if ( newImage->getWidth() != m_imageWidth ) {
		//throw exception
		return;
	}
	
	int incr  = (m_imageWidth * sizeof(RGBAVals)) * index;	
	int oldImgCount = m_totalImageCount;
	int tmpSize = (m_imageHeight * m_imageWidth * sizeof(RGBAVals)) * (m_totalImageCount-index);
	int tmpLineIncr = (m_imageWidth * sizeof(RGBAVals)) * (m_totalImageCount-index);
	int fullLineIncr = (m_imageWidth * sizeof(RGBAVals)) * m_totalImageCount;

	//save off the old section of the 
	//image that will have to be moved over
	char* tmpBits = new char[tmpSize];
	char* tmpBitsPtr = tmpBits;
	char* oldBits = (char*)this->m_masterImage->getImageBits()->m_bits;
	oldBits += incr;	
	for ( int y=0;y<m_imageHeight;y++) {
		memcpy( tmpBitsPtr, oldBits, tmpLineIncr );
		tmpBitsPtr += tmpLineIncr;
		oldBits += fullLineIncr;
	}	

	//resize the image if neccessary
	m_totalImageCount++;
	if ( m_totalImageCount >= m_imageCount ) {
		//resize the master image
		m_imageCount += this->m_resizeIncrement;
		this->changed();
	}
	
	int moveOverIncr = (m_imageWidth * sizeof(RGBAVals)) * index+1;
	int indexIncr = (m_imageWidth * sizeof(RGBAVals)) * index;
	char* buf = (char*)m_masterImage->getImageBits()->m_bits;

	char* newImgBits = (char*)newImage->getImageBits()->m_bits;
	memcpy( oldBits, newImgBits, m_imageHeight * m_imageWidth * sizeof(RGBAVals) );

	tmpBitsPtr = tmpBits;
	for ( y=0;y<m_imageHeight;y++) {
		//copy back the original bits, moving over by one spot
		oldBits = buf;
		oldBits += moveOverIncr + (y * fullLineIncr);
		memcpy( oldBits, tmpBits, tmpLineIncr );

		//copy over the inserted image bits
		oldBits = buf;
		oldBits += indexIncr + (y * fullLineIncr);
		memcpy( oldBits, tmpBitsPtr, tmpLineIncr );
		tmpBitsPtr += tmpLineIncr;
	}

	delete [] tmpBits;

	ImageListEvent event( this, IMAGELIST_EVENT_ITEM_ADDED, newImage );
	event.setIndexOfImage( index );
	fireOnImageAdded( &event );
}

void ImageList::deleteImage( const unsigned long & index )
{	
	int incr  = (m_imageHeight * m_imageWidth * sizeof(RGBAVals)) * index;	
	
	int oldImgCount = m_totalImageCount;	

	int tmpSize = (m_imageHeight * m_imageWidth * sizeof(RGBAVals)) * (oldImgCount-(index+1));
	//save off the old section of the 
	//image that will have to be moved over
	char* tmpBits = new char[tmpSize];
	char* oldBits = (char*)this->m_masterImage->getImageBits()->m_bits;
	oldBits += incr;	
	memcpy( tmpBits, oldBits, tmpSize );

	m_totalImageCount--;
	//resize the image if neccessary	
	if ( (m_totalImageCount % m_imageCount) == m_resizeIncrement ) {
		//resize the master image
		m_imageCount -= this->m_resizeIncrement;
		this->changed();
	}
	//copy back the original bits, moving over by one spot
	int moveOverIncr = (m_imageHeight * m_imageWidth * sizeof(RGBAVals)) * index;
	oldBits = (char*)this->m_masterImage->getImageBits()->m_bits;
	oldBits += moveOverIncr;
	memcpy( oldBits, tmpBits, tmpSize );
	delete [] tmpBits;
	
	ImageListEvent event( this, IMAGELIST_EVENT_ITEM_DELETED );
	event.setIndexOfImage( index );
	fireOnImageAdded( &event );
}

void ImageList::draw( GraphicsContext* context, const unsigned long& index, Point* pt )
{
	Rect bounds( index * m_imageWidth, 0, index * m_imageWidth + m_imageWidth, m_imageHeight ); 	
	m_masterImage->setIsTransparent( true );
	m_masterImage->setTransparencyColor( &m_transparentColor );
	context->drawPartialImage( pt->m_x, pt->m_y, &bounds, m_masterImage );
}

void ImageList::draw( GraphicsContext* context, const unsigned long& index, Rect* bounds )
{
	double w = __min( bounds->getWidth(), m_imageWidth );
	double h = __min( bounds->getHeight(), m_imageHeight );
	Rect tmpBounds( index * m_imageWidth, 0, index * m_imageWidth + w, h ); 
	m_masterImage->setIsTransparent( true );
	m_masterImage->setTransparencyColor( &m_transparentColor );
	//context->drawImage( bounds->m_left, bounds->m_top, m_masterImage );
	context->drawPartialImage( bounds->m_left, bounds->m_top, &tmpBounds, m_masterImage );
}

void ImageList::copyImage( Image* imageToCopyTo, const unsigned long& index )
{
	int incr  = (m_imageHeight * m_imageWidth * sizeof(RGBAVals)) * index;	
	char* buf = (char*)m_masterImage->getImageBits()->m_bits;
	buf += incr;
	char* copyBuf = (char*)imageToCopyTo->getImageBits()->m_bits;
	memcpy( copyBuf, buf, (m_imageHeight * m_imageWidth * sizeof(RGBAVals)) );
}

void ImageList::changed()
{
	Image* oldImage = m_masterImage;
	
	GraphicsToolkit* toolkit = GraphicsToolkit::getDefaultGraphicsToolkit();
	m_masterImage = toolkit->createImage( m_imageWidth * m_imageCount, m_imageHeight );
	if ( NULL == m_masterImage ) {
		throw InvalidImage( "Toolkit couldn't create Image" );
	}	

	if ( NULL != oldImage ) {
		RGBAVals* oldBits = oldImage->getImageBits()->m_bits;
		long oldWidth = oldImage->getWidth();
		long oldHeight = oldImage->getHeight();
		RGBAVals* newBits = m_masterImage->getImageBits()->m_bits;
		long newWidth = m_masterImage->getWidth();

		uint32 size = __min( oldWidth,newWidth ); 

		for ( int i=0;i<m_imageHeight;i++ ){	
			if ( i < oldHeight ) {
				memcpy( newBits, oldBits, size * sizeof(RGBAVals) ); 	
				oldBits +=  oldWidth;
				newBits += newWidth;
			}
			else {
				break;
			}			
		}

		delete oldImage;
		oldImage = NULL;
	}
}

void ImageList::afterCreate( ComponentEvent* event )
{

}
