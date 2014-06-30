//Win32Desktop.h

#ifndef _WIN32DESKTOP_H__
#define _WIN32DESKTOP_H__



namespace VCF  {

/**
*Class Win32Desktop documentation
*/
class Win32Desktop : public Object, public DesktopPeer { 
public:
	Win32Desktop( Desktop* desktop );

	virtual ~Win32Desktop();

	/**
	*Prepares the desktop for painting.
	*MUST be called before calling getContext(), or errors
	*may occur
	*@param Rect* specifies a rectangular region to clip 
	*the drawing to. The rectangles coords are screen coords.
	*/
	virtual void desktopBeginPainting( Rect* clippingRect );

	/**
	*called after finished painting on the desktop 
	*allows for native Windowing system cleanup to happen
	*/
	virtual void desktopEndPainting();

	virtual bool desktopSupportsVirtualDirectories();

	virtual bool desktopHasFileSystemDirectory();

	virtual String desktopGetDirectory();
	

	virtual ulong32 desktopGetHandleID();

	virtual void desktopSetDesktop( Desktop* desktop );

	virtual void desktopTranslateToScreenCoords( Control* control, Point* pt );

	virtual void desktopTranslateFromScreenCoords( Control* control, Point* pt );
protected:
	Desktop* m_desktop;
	HRGN m_hClipRgn;
private:
};


}; //end of namespace VCF

#endif //_WIN32DESKTOP_H__


