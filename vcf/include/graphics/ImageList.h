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



#ifndef IMAGELIST_H
#define IMAGELIST_H



namespace VCF
{


#define IMAGELIST_CLASSID				"2EA88629-682F-4b01-BB6F-9990BCF67DD6"

class Image;

class GraphicsContext;

class APPKIT_API ImageList : public Component{
public:
	BEGIN_CLASSINFO(ImageList, "VCF::ImageList", "VCF::Component", IMAGELIST_CLASSID )
	OBJECT_PROPERTY( Color, "transparentColor", ImageList::getTransparentColor, ImageList::setTransparentColor );
	EVENT("VCF::ImageListEventHandler", "VCF::ImageListEvent", "SizeChanged" );
	EVENT("VCF::ImageListEventHandler", "VCF::ImageListEvent", "ImageAdded" );
	EVENT("VCF::ImageListEventHandler", "VCF::ImageListEvent", "ImageDeleted" );
	END_CLASSINFO(ImageList)

	ImageList();

	/**
	*Creates a new imagelist from a Image and the 
	*appropriate width and height. 
	*/
	ImageList( Image* listOfImages, const unsigned long& imageWidth, const unsigned long& imageHeight );

	virtual ~ImageList();

	void init();

	EVENT_HANDLER_LIST(SizeChanged)
	EVENT_HANDLER_LIST(ImageAdded)
	EVENT_HANDLER_LIST(ImageDeleted)

	ADD_EVENT(SizeChanged)
	ADD_EVENT(ImageAdded)
	ADD_EVENT(ImageDeleted)

	REMOVE_EVENT(SizeChanged)
	REMOVE_EVENT(ImageAdded)
	REMOVE_EVENT(ImageDeleted)

	FIRE_EVENT(SizeChanged,ImageListEvent)
	FIRE_EVENT(ImageAdded,ImageListEvent)
	FIRE_EVENT(ImageDeleted,ImageListEvent)

	/**
	*returns the width of each image in the list, <b><i>NOT</i></b>
	*the width of the list itself. All images in the list
	*will have the same width and height.
	*/
	unsigned long getImageWidth();

	/**
	*returns the height of an image in the list.
	*All images in the list will have the same width and height.
	*/
	unsigned long getImageHeight();

	void setImageWidth( const unsigned long& width );

	void setImageHeight( const unsigned long& height );

	void setTransparentColor( Color* color );

	Color* getTransparentColor();

	void addImage( Image* newImage );
	
	void insertImage( const unsigned long & index, Image* newImage );

	void deleteImage( const unsigned long & index );

	virtual void draw( GraphicsContext* context, const unsigned long& index, Point* pt );

	/**
	*Draws the image within the bounds specified in bounds. Where the bounds->m_left and m_top
	*represent the upper left and top coords. If the boudns width or height is less than the 
	*images, then clipping occurs
	*/
	virtual void draw( GraphicsContext* context, const unsigned long& index, Rect* bounds );

	void copyImage( Image* imageToCopyTo, const unsigned long& index );

	void setResizeIncrement( const ulong32& resizeIncrement ) {
		m_resizeIncrement = resizeIncrement;
	}	

	virtual void afterCreate( ComponentEvent* event );

	uint32 getImageCount() {
		return m_totalImageCount;
	}

	Image* getMasterImage() {
		return m_masterImage;
	}
private:
	unsigned long m_imageWidth;
	unsigned long m_imageHeight;
    Color m_transparentColor;
	/**
	*this represents the number of images the m_masterImage
	*can hold
	*/
	uint32 m_imageCount;

	/**
	*this represents the number of images actually
	*displayed within the m_masterImage
	*/
	uint32 m_totalImageCount;
	Image* m_masterImage;	
	ulong32 m_resizeIncrement;
	void changed();
};

};

#endif //IMAGELIST_H