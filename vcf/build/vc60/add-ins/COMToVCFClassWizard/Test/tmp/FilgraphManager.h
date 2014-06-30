//FilgraphManager.h

#ifndef QUARTZTYPELIB_FILGRAPHMANAGER_H__
#define QUARTZTYPELIB_FILGRAPHMANAGER_H__
#include "FoundationKit.h"

namespace QuartzTypeLib  {

#define QUARTZTYPELIB_FILGRAPHMANAGER_CLASSID 		"E436EBB3-524F-11CE-9F53-0020AF0BA770"
/**
*class FilgraphManager
*UUID: E436EBB3-524F-11CE-9F53-0020AF0BA770
*ProgID: 
*/
class FilgraphManager : public VCF::Object, public IMediaControl, public IMediaEvent, public IMediaPosition, public IBasicAudio, public IBasicVideo, public IVideoWindow {
public:
	BEGIN_CLASSINFO( FilgraphManager, "QuartzTypeLib::FilgraphManager", "VCF::Object", QUARTZTYPELIB_FILGRAPHMANAGER_CLASSID );
	END_CLASSINFO( FilgraphManager );

	FilgraphManager();

	virtual ~FilgraphManager();

	virtual void Run(  );//[id(1610743808)]

	virtual void Pause(  );//[id(1610743809)]

	virtual void Stop(  );//[id(1610743810)]

	virtual void GetState( long msTimeout, long* pfs );//[id(1610743811)]

	virtual void RenderFile( BSTR strFilename );//[id(1610743812)]

	virtual void AddSourceFilter( BSTR strFilename, IDispatch** ppUnk );//[id(1610743813)]

	virtual IDispatch* getFilterCollection(  );//[id(1610743814)]

	virtual IDispatch* getRegFilterCollection(  );//[id(1610743815)]

	virtual void StopWhenReady(  );//[id(1610743816)]

	virtual void GetEventHandle( LONG_PTR* hEvent );//[id(1610743808)]

	virtual void GetEvent( long* lEventCode, LONG_PTR* lParam1, LONG_PTR* lParam2, long msTimeout );//[id(1610743809)]

	virtual void WaitForCompletion( long msTimeout, long* pEvCode );//[id(1610743810)]

	virtual void CancelDefaultHandling( long lEvCode );//[id(1610743811)]

	virtual void RestoreDefaultHandling( long lEvCode );//[id(1610743812)]

	virtual void FreeEventParams( long lEvCode, LONG_PTR lParam1, LONG_PTR lParam2 );//[id(1610743813)]

	virtual double getDuration(  );//[id(1610743808)]

	virtual void setCurrentPosition( double Val );//[id(1610743809)]

	virtual double getCurrentPosition(  );//[id(1610743809)]

	virtual double getStopTime(  );//[id(1610743811)]

	virtual void setStopTime( double Val );//[id(1610743811)]

	virtual double getPrerollTime(  );//[id(1610743813)]

	virtual void setPrerollTime( double Val );//[id(1610743813)]

	virtual void setRate( double Val );//[id(1610743815)]

	virtual double getRate(  );//[id(1610743815)]

	virtual long CanSeekForward(  );//[id(1610743817)]

	virtual long CanSeekBackward(  );//[id(1610743818)]

	virtual void setVolume( long Val );//[id(1610743808)]

	virtual long getVolume(  );//[id(1610743808)]

	virtual void setBalance( long Val );//[id(1610743810)]

	virtual long getBalance(  );//[id(1610743810)]

	virtual double getAvgTimePerFrame(  );//[id(1610743808)]

	virtual long getBitRate(  );//[id(1610743809)]

	virtual long getBitErrorRate(  );//[id(1610743810)]

	virtual long getVideoWidth(  );//[id(1610743811)]

	virtual long getVideoHeight(  );//[id(1610743812)]

	virtual void setSourceLeft( long Val );//[id(1610743813)]

	virtual long getSourceLeft(  );//[id(1610743813)]

	virtual void setSourceWidth( long Val );//[id(1610743815)]

	virtual long getSourceWidth(  );//[id(1610743815)]

	virtual void setSourceTop( long Val );//[id(1610743817)]

	virtual long getSourceTop(  );//[id(1610743817)]

	virtual void setSourceHeight( long Val );//[id(1610743819)]

	virtual long getSourceHeight(  );//[id(1610743819)]

	virtual void setDestinationLeft( long Val );//[id(1610743821)]

	virtual long getDestinationLeft(  );//[id(1610743821)]

	virtual void setDestinationWidth( long Val );//[id(1610743823)]

	virtual long getDestinationWidth(  );//[id(1610743823)]

	virtual void setDestinationTop( long Val );//[id(1610743825)]

	virtual long getDestinationTop(  );//[id(1610743825)]

	virtual void setDestinationHeight( long Val );//[id(1610743827)]

	virtual long getDestinationHeight(  );//[id(1610743827)]

	virtual void SetSourcePosition( long Left, long Top, long Width, long Height );//[id(1610743829)]

	virtual void GetSourcePosition( long* pLeft, long* pTop, long* pWidth, long* pHeight );//[id(1610743830)]

	virtual void SetDefaultSourcePosition(  );//[id(1610743831)]

	virtual void SetDestinationPosition( long Left, long Top, long Width, long Height );//[id(1610743832)]

	virtual void GetDestinationPosition( long* pLeft, long* pTop, long* pWidth, long* pHeight );//[id(1610743833)]

	virtual void SetDefaultDestinationPosition(  );//[id(1610743834)]

	virtual void GetVideoSize( long* pWidth, long* pHeight );//[id(1610743835)]

	virtual void GetVideoPaletteEntries( long StartIndex, long Entries, long* pRetrieved, long* pPalette );//[id(1610743836)]

	virtual void GetCurrentImage( long* pBufferSize, long* pDIBImage );//[id(1610743837)]

	virtual void IsUsingDefaultSource(  );//[id(1610743838)]

	virtual void IsUsingDefaultDestination(  );//[id(1610743839)]

	virtual void setCaption( BSTR Val );//[id(1610743808)]

	virtual BSTR getCaption(  );//[id(1610743808)]

	virtual void setWindowStyle( long Val );//[id(1610743810)]

	virtual long getWindowStyle(  );//[id(1610743810)]

	virtual void setWindowStyleEx( long Val );//[id(1610743812)]

	virtual long getWindowStyleEx(  );//[id(1610743812)]

	virtual void setAutoShow( long Val );//[id(1610743814)]

	virtual long getAutoShow(  );//[id(1610743814)]

	virtual void setWindowState( long Val );//[id(1610743816)]

	virtual long getWindowState(  );//[id(1610743816)]

	virtual void setBackgroundPalette( long Val );//[id(1610743818)]

	virtual long getBackgroundPalette(  );//[id(1610743818)]

	virtual void setVisible( long Val );//[id(1610743820)]

	virtual long getVisible(  );//[id(1610743820)]

	virtual void setLeft( long Val );//[id(1610743822)]

	virtual long getLeft(  );//[id(1610743822)]

	virtual void setWidth( long Val );//[id(1610743824)]

	virtual long getWidth(  );//[id(1610743824)]

	virtual void setTop( long Val );//[id(1610743826)]

	virtual long getTop(  );//[id(1610743826)]

	virtual void setHeight( long Val );//[id(1610743828)]

	virtual long getHeight(  );//[id(1610743828)]

	virtual void setOwner( LONG_PTR Val );//[id(1610743830)]

	virtual LONG_PTR getOwner(  );//[id(1610743830)]

	virtual void setMessageDrain( LONG_PTR Val );//[id(1610743832)]

	virtual LONG_PTR getMessageDrain(  );//[id(1610743832)]

	virtual long getBorderColor(  );//[id(1610743834)]

	virtual void setBorderColor( long Val );//[id(1610743834)]

	virtual long getFullScreenMode(  );//[id(1610743836)]

	virtual void setFullScreenMode( long Val );//[id(1610743836)]

	virtual void SetWindowForeground( long Focus );//[id(1610743838)]

	virtual void NotifyOwnerMessage( LONG_PTR hwnd, long uMsg, LONG_PTR wParam, LONG_PTR lParam );//[id(1610743839)]

	virtual void SetWindowPosition( long Left, long Top, long Width, long Height );//[id(1610743840)]

	virtual void GetWindowPosition( long* pLeft, long* pTop, long* pWidth, long* pHeight );//[id(1610743841)]

	virtual void GetMinIdealImageSize( long* pWidth, long* pHeight );//[id(1610743842)]

	virtual void GetMaxIdealImageSize( long* pWidth, long* pHeight );//[id(1610743843)]

	virtual void GetRestorePosition( long* pLeft, long* pTop, long* pWidth, long* pHeight );//[id(1610743844)]

	virtual void HideCursor( long HideCursor );//[id(1610743845)]

	virtual void IsCursorHidden( long* CursorHidden );//[id(1610743846)]

};

}  //end of namespace QuartzTypeLib

#endif //Adding method 38 of 39 for interface IVideoWindow

