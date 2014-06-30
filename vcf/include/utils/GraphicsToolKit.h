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



#ifndef GRAPHICSTOOLKIT_H
#define GRAPHICSTOOLKIT_H



namespace VCF{

class OpenGLPeer;

class OpenGLContext;

class GRAPHICSKIT_API GraphicsToolkit : public Object{
public:
	

	GraphicsToolkit();

	virtual ~GraphicsToolkit();

    static GraphicsToolkit* getDefaultGraphicsToolkit();

	virtual ContextPeer* createContextPeer( const unsigned long& width, const unsigned long& height ) {
		return NULL;
	}

	virtual ContextPeer* createContextPeer( const long& contextID ) {
		return NULL;
	}

	virtual FontPeer* createFontPeer( const String& fontName ) {
		return NULL;
	}

	virtual FontPeer* createFontPeer( const String& fontName, const double& pointSize ) {
		return NULL;
	}
	
	virtual OpenGLPeer* createOpenGLPeer( GraphicsContext* glContext ) {
		return NULL;
	}

	virtual Image* createImage( const unsigned long& width, const unsigned long& height ) {
		return NULL;
	}

	virtual Image* createImage( GraphicsContext* context, Rect* rect ) {
		return NULL;
	}
	/**
	*Create a image from a filename. The file is loaded into a Bitmap instance.
	*The toolkit looks up the file extension and matches the type to 
	*an ImageLoader instance. The ImageLoader does the work of actually decoding the image
	*/
	virtual Image* createImage( const String& fileName );

	void registerImageLoader( const String& name, ImageLoader* imageLoader );

	ImageLoader* getImageLoader( const String& contentType );

	virtual Color* getSystemColor( const unsigned long& systemColor );

	Color* getColorFromColormap( const String& color );

	static void initGraphicsToolkit();

	static void closeGraphicsToolkit();
protected:
	std::map<String,String> m_contentTypes;
	
	std::map<String,Color*> m_colorMap;

	std::map<unsigned long,Color*> m_systemColors;

	void initContentTypes();

	static GraphicsToolkit* graphicsToolkitInstance;
	/**
	*ImageLoaders are stored in map, keyed by the MIME content type
	*Thus JPEG loaders are stored under the key: 
	*<p><pre>
	*	"image/jpeg"
	*</pre></p>
	*/
	std::map<String, ImageLoader*> m_imageLoaders;
};

};
#endif //GRAPHICSTOOLKIT_HSTOOLKIT_H