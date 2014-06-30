//ImageControl.h

/**
Copyright (c) 2000-2001, Jim Crafton
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
	Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in 
	the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

NB: This software will not save the world. 
*/

#ifndef _IMAGECONTROL_H__
#define _IMAGECONTROL_H__



using namespace VCF;


#define IMAGEFILENAMESTRING_CLASSID		"166982EF-60DB-4476-ABF1-AD4EB82B2B08"


#define IMAGECONTROL_CLASSID			"16b4cd34-382c-4b3c-9fe9-f04b5983cd6c"


namespace VCF  {

class APPKIT_API ImageFilenameString : public Object{
public:
	BEGIN_CLASSINFO(ImageFilenameString, "VCF::ImageFilenameString", "VCF::Object", IMAGEFILENAMESTRING_CLASSID);
	END_CLASSINFO( ImageFilenameString );
	ImageFilenameString(){};
	
	ImageFilenameString( const ImageFilenameString& filename ) {
		m_string = filename.m_string;
	}

	virtual ~ImageFilenameString(){};
	
	operator String () {
		return m_string;
	}

	ImageFilenameString& operator= ( const ImageFilenameString& filename ) {
		m_string = filename.m_string;
		return *this;
	}
	
	ImageFilenameString& operator= ( const String& filename ) {
		m_string = filename;
		return *this;
	}

	virtual String toString() {
		return m_string;
	}

protected:
	String m_string;
};

/**
*Class ImageControl documentation
*/
class APPKIT_API ImageControl : public VCF::CustomControl { 
public:
	BEGIN_CLASSINFO(ImageControl, "VCF::ImageControl", "VCF::CustomControl", IMAGECONTROL_CLASSID)
	OBJECT_PROPERTY( Image, "image", ImageControl::getImage, ImageControl::setImage );
	PROPERTY(bool, "transparent", ImageControl::getTransparent, ImageControl::setTransparent, PROP_BOOL);
	OBJECT_PROPERTY_REF(ImageFilenameString, "filename", ImageControl::getFilename, ImageControl::setFilename);
	END_CLASSINFO(ImageControl)

	ImageControl();

	virtual ~ImageControl();

	Image* getImage();

	void setImage( Image* image );

	bool getTransparent();

	void setTransparent( const bool& transparent );

	ImageFilenameString& getFilename();

	void setFilename( const ImageFilenameString& filename );

	virtual void paint( GraphicsContext* context );
protected:

private:
	Image* m_image;
	bool m_transparent;
	ImageFilenameString m_filename;
};


class APPKIT_API ImageFilenamePropertyEditor : public AbstractPropertyEditor {
public:
	ImageFilenamePropertyEditor();

	virtual ~ImageFilenamePropertyEditor();
	
	virtual bool hasCustomEditor(){
		return true;	
	};	

	virtual Control* getCustomEditor();

	void showFilenameEditor( VariantData* data );
private:
	
};

class APPKIT_API ImagePropertyEditor : public AbstractPropertyEditor {
public:
	ImagePropertyEditor();

	virtual ~ImagePropertyEditor();
	
	virtual bool hasCustomEditor(){
		return true;	
	};	

	virtual Control* getCustomEditor();

	void showImageEditor( VariantData* data );
private:
	
};

}; //end of namespace VCF

#endif //_IMAGECONTROL_H__


