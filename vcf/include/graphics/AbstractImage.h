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



#ifndef ABSTRACTIMAGE_H
#define ABSTRACTIMAGE_H




namespace VCF{

class ImageBits;
class GraphicsContext;

/**
*AbstractImage represents a base implementation of the Image interface. It implements
*common functions, such as getWidth, etc, but still requires actually image class GRAPHICSKIT_API to be 
*derived from it. It also provides basic support for ImageSizeChangedHandlers, so derived classes 
*do not have to baother with it. See Image for more information on what the functions do.
*
*@version 1.0
*@author Jim Crafton
*/
class GRAPHICSKIT_API AbstractImage : public Image , public Object, public Persistable {
public:

	AbstractImage( const bool& needsMemAlloc=true );

	virtual ~AbstractImage();

	virtual ImageBitDepth getBitDepth();    

    virtual void setWidth(const unsigned long & width);

    virtual unsigned long getWidth();

    virtual void setHeight(const unsigned long & height);

    virtual unsigned long getHeight();

    EVENT_HANDLER_LIST(ImageSizeChanged);

	/**
	*This macro creates a method for adding a listener to the AbstractImage's ImageSizeChangedHandler events
	*/
	virtual void addImageSizeChangedHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(ImageSizeChanged)
	}

	/**
	*This macro creates a method for removing a listener to the AbstractImage's ImageSizeChangedHandler events
	*/
	virtual void removeImageSizeChangedHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(ImageSizeChanged);
	}

	FIRE_EVENT(ImageSizeChanged,ImageEvent);    

    virtual GraphicsContext* getImageContext();

	virtual ImageBits* getImageBits();

	/**
	*returns the color that is used to blend with the contents of
	*a GraphicsContext when the Image is drawn. Only used when the 
	*Image is set to Transparent
	*/
	virtual Color* getTransparencyColor() {
		return &m_transparencyColor;	
	}

	virtual void setTransparencyColor( Color* transparencyColor ) {
		m_transparencyColor = *transparencyColor;
	}

	/**
	*Indicates whether or not the Image is using a transparent
	*color.
	*@return bool if this is true then the Image is transparent
	*and the contents of the underlying GraphicsContext will show through
	*wherever a pixel in the image is found that is the transparency color
	*/
	virtual bool isTransparent() {
		return m_isTransparent;	
	}

	virtual void setIsTransparent( const bool& transparent ) {
		m_isTransparent = transparent;
	}

	/**
	*Write the object to the specified output stream
	*/
    virtual void saveToStream( OutputStream * stream );
	
	/**
	**Read the object from the specified input stream
	*/
    virtual void loadFromStream( InputStream * stream );
protected:
	ImageBits* m_imageBits;
	int m_height;
    int m_width;
	GraphicsContext * m_context;
	Color m_transparencyColor;
	bool m_isTransparent;
private:    	
	
	ImageBitDepth m_bitDepth;
};

};
#endif //ABSTRACTIMAGE_H