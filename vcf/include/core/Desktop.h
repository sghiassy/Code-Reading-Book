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
//Desktop.h

#ifndef _DESKTOP_H__
#define _DESKTOP_H__




using namespace VCF;


namespace VCF  {

class GraphicsContext;

class DesktopPeer;

/**
*Class Desktop 
*the desktop represents the Windowing systems bottom most
*and is usuall the first window the User sees when the system starts
*up. The Desktop is also sometimes refered to as the Shell, as in
*MS Windows. A desktop may or may not allow files, i.e. it in fact is 
*an actual location on the filesystem 
*/
class APPKIT_API Desktop : public VCF::Object { 
public:
	static Desktop* create();

	Desktop();

	virtual ~Desktop();

	GraphicsContext* getContext() {
		return m_context;
	}

	/**
	*Prepares the desktop for painting.
	*MUST be called before calling getContext(), or errors
	*may occur
	*@param Rect* specifies an optional rectangular region to clip 
	*the drawing to. The rectangles coords are screen coords.
	*/
	void beginPainting( Rect* clippingRect = NULL);

	/**
	*called after finished painting on the desktop 
	*allows for native Windowing system cleanup to happen
	*/
	void endPainting();

	bool supportsVirtualDirectories();


	bool hasFileSystemDirectory();

	String getDirectory();		

	DesktopPeer* getDesktopPeer() {
		return m_peer;
	}

	static Desktop* getDesktop() {
		return Desktop::desktopInstance;
	}

	void translateToScreenCoords( Control* control, Point* pt );
	
	void translateToScreenCoords( Control* control, Rect* rect );

	void translateFromScreenCoords( Control* control, Point* pt );
	
	void translateFromScreenCoords( Control* control, Rect* rect );

	/**
	*called by the UIToolkit. repeated calls after the first initialization
	*do nothing
	*/
	void init();
protected:
	

	static Desktop* desktopInstance;
	GraphicsContext* m_context;
	bool m_paintingInitialized;
	DesktopPeer* m_peer;
private:
};


}; //end of namespace VCF

#endif //_DESKTOP_H__


