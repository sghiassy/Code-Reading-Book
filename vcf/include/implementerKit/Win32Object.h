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

// Win32Object.h

#ifndef _WIN32OBJECT_H__
#define _WIN32OBJECT_H__




namespace VCF
{

#define SIMPLE_VIEW				WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
#define BORDERED_VIEW			WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
#define SIMPLE_DIALOG			WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
#define FRAME_WINDOW			WS_OVERLAPPEDWINDOW

class Control;

class APPKIT_API Win32Object : public ObjectWithEvents  
{
public:
	Win32Object();
	virtual ~Win32Object();

	void registerWin32Class( const String& className, WNDPROC wndProc );

	static void addRegisterWin32Class( const String& className );
	
	/**
	*Checks whether the class has been registered. Performs a
	*std::find() using the m_registeredWindowClasses static list
	*/
	bool isRegistered();

	/**
	*called before the create window is called. Overide this to change the 
	*m_styleMask and m_exStyleMask values for determing the initial look of the window
	*/
	virtual void createParams();

	/**
	*This is where the window is actually created. Called from constructors
	*/
	void init();

	virtual LRESULT handleEventMessages( UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultWndProc = NULL);

	static Win32Object* getWin32ObjectFromHWND( HWND hwnd );

	static void registerWin32Object( Win32Object* wndObj );

	HWND getHwnd();

	LRESULT defaultWndProcedure( UINT message, WPARAM wParam, LPARAM lParam );
	
	void setCreated( const bool& creationComplete );

	bool isCreated();

	void setPeerControl( Control* control );

	Control* getPeerControl();
protected:
	void destroyWindowHandle();

	static std::vector< std::string > m_registeredWindowClasses;
	static std::map<HWND, Win32Object*> m_windowMap;
	DWORD m_styleMask;
	DWORD m_exStyleMask;
	Rect m_bounds;
	/**
	*this actually contains the current window proc
	*/
	WNDPROC m_wndProc;
	/**
	*this contains the window proc used for window
	*subclassing or just performing the default
	*behaviour. By default it is set
	*to point to ::DefWindowProc()
	*/
	WNDPROC m_defaultWndProc;
	HWND m_hwnd;
	String m_windowCaption;
	bool m_created;
	Control* m_peerControl;
};


};

#endif 
