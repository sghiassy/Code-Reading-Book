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
//DesktopPeer.h

#ifndef _DESKTOPPEER_H__
#define _DESKTOPPEER_H__




using namespace VCF;


namespace VCF  {

class Desktop;


/**
*Class DesktopPeer interface documentation
*/
class APPKIT_API DesktopPeer { 
public:
	virtual ~DesktopPeer(){};

	/**
	*Prepares the desktop for painting.
	*MUST be called before calling getContext(), or errors
	*may occur
	*@param Rect* specifies a rectangular region to clip 
	*the drawing to. The rectangles coords are screen coords.
	*/
	virtual void desktopBeginPainting( Rect* clippingRect ) = 0;

	/**
	*called after finished painting on the desktop 
	*allows for native Windowing system cleanup to happen
	*/
	virtual void desktopEndPainting() = 0;

	virtual bool desktopSupportsVirtualDirectories() = 0;

	virtual bool desktopHasFileSystemDirectory() = 0;

	virtual String desktopGetDirectory() = 0;
	

	virtual ulong32 desktopGetHandleID() = 0;

	virtual void desktopSetDesktop( Desktop* desktop ) = 0;

	virtual void desktopTranslateToScreenCoords( Control* control, Point* pt ) = 0;

	virtual void desktopTranslateFromScreenCoords( Control* control, Point* pt ) = 0;
};


}; //end of namespace VCF

#endif //_DESKTOPPEER_H__


