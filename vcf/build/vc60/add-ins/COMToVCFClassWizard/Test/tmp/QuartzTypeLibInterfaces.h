//QuartzTypeLib.h

#ifndef _QUARTZTYPELIBINTERFACES_H__
#define _QUARTZTYPELIBINTERFACES_H__

#include "FoundationKit.h"

namespace QuartzTypeLib  {

/**
*Interface IAMCollection
*UUID: 56A868B9-0AD4-11CE-B03A-0020AF0BA770
*/
class IAMCollection : public Interface { 
public:
	virtual ~IAMCollection(){};

	virtual long getCount(  ) = 0;//[id(1610743808)]

	virtual void Item( long lItem, IUnknown** ppUnk ) = 0;//[id(1610743809)]

	virtual IUnknown* get_NewEnum(  ) = 0;//[id(1610743810)]

};


/**
*Interface IAMStats
*UUID: BC9BCF80-DCD2-11D2-ABF6-00A0C905F375
*/
class IAMStats : public Interface { 
public:
	virtual ~IAMStats(){};

	virtual void Reset(  ) = 0;//[id(1610743808)]

	virtual long getCount(  ) = 0;//[id(1610743809)]

	virtual void GetValueByIndex( long lIndex, BSTR* szName, long* lCount, double* dLast, double* dAverage, double* dStdDev, double* dMin, double* dMax ) = 0;//[id(1610743810)]

	virtual void GetValueByName( BSTR szName, long* lIndex, long* lCount, double* dLast, double* dAverage, double* dStdDev, double* dMin, double* dMax ) = 0;//[id(1610743811)]

	virtual void GetIndex( BSTR szName, long lCreate, long* plIndex ) = 0;//[id(1610743812)]

	virtual void AddValue( long lIndex, double dValue ) = 0;//[id(1610743813)]

};


/**
*Interface IBasicAudio
*UUID: 56A868B3-0AD4-11CE-B03A-0020AF0BA770
*/
class IBasicAudio : public Interface { 
public:
	virtual ~IBasicAudio(){};

	virtual void setVolume( long Val ) = 0;//[id(1610743808)]

	virtual long getVolume(  ) = 0;//[id(1610743808)]

	virtual void setBalance( long Val ) = 0;//[id(1610743810)]

	virtual long getBalance(  ) = 0;//[id(1610743810)]

};


/**
*Interface IBasicVideo
*UUID: 56A868B5-0AD4-11CE-B03A-0020AF0BA770
*/
class IBasicVideo : public Interface { 
public:
	virtual ~IBasicVideo(){};

	virtual double getAvgTimePerFrame(  ) = 0;//[id(1610743808)]

	virtual long getBitRate(  ) = 0;//[id(1610743809)]

	virtual long getBitErrorRate(  ) = 0;//[id(1610743810)]

	virtual long getVideoWidth(  ) = 0;//[id(1610743811)]

	virtual long getVideoHeight(  ) = 0;//[id(1610743812)]

	virtual void setSourceLeft( long Val ) = 0;//[id(1610743813)]

	virtual long getSourceLeft(  ) = 0;//[id(1610743813)]

	virtual void setSourceWidth( long Val ) = 0;//[id(1610743815)]

	virtual long getSourceWidth(  ) = 0;//[id(1610743815)]

	virtual void setSourceTop( long Val ) = 0;//[id(1610743817)]

	virtual long getSourceTop(  ) = 0;//[id(1610743817)]

	virtual void setSourceHeight( long Val ) = 0;//[id(1610743819)]

	virtual long getSourceHeight(  ) = 0;//[id(1610743819)]

	virtual void setDestinationLeft( long Val ) = 0;//[id(1610743821)]

	virtual long getDestinationLeft(  ) = 0;//[id(1610743821)]

	virtual void setDestinationWidth( long Val ) = 0;//[id(1610743823)]

	virtual long getDestinationWidth(  ) = 0;//[id(1610743823)]

	virtual void setDestinationTop( long Val ) = 0;//[id(1610743825)]

	virtual long getDestinationTop(  ) = 0;//[id(1610743825)]

	virtual void setDestinationHeight( long Val ) = 0;//[id(1610743827)]

	virtual long getDestinationHeight(  ) = 0;//[id(1610743827)]

	virtual void SetSourcePosition( long Left, long Top, long Width, long Height ) = 0;//[id(1610743829)]

	virtual void GetSourcePosition( long* pLeft, long* pTop, long* pWidth, long* pHeight ) = 0;//[id(1610743830)]

	virtual void SetDefaultSourcePosition(  ) = 0;//[id(1610743831)]

	virtual void SetDestinationPosition( long Left, long Top, long Width, long Height ) = 0;//[id(1610743832)]

	virtual void GetDestinationPosition( long* pLeft, long* pTop, long* pWidth, long* pHeight ) = 0;//[id(1610743833)]

	virtual void SetDefaultDestinationPosition(  ) = 0;//[id(1610743834)]

	virtual void GetVideoSize( long* pWidth, long* pHeight ) = 0;//[id(1610743835)]

	virtual void GetVideoPaletteEntries( long StartIndex, long Entries, long* pRetrieved, long* pPalette ) = 0;//[id(1610743836)]

	virtual void GetCurrentImage( long* pBufferSize, long* pDIBImage ) = 0;//[id(1610743837)]

	virtual void IsUsingDefaultSource(  ) = 0;//[id(1610743838)]

	virtual void IsUsingDefaultDestination(  ) = 0;//[id(1610743839)]

};


/**
*Interface IBasicVideo2
*UUID: 329BB360-F6EA-11D1-9038-00A0C9697298
*/
class IBasicVideo2 : public Interface { 
public:
	virtual ~IBasicVideo2(){};

	virtual HRESULT GetPreferredAspectRatio( long* plAspectX, long* plAspectY ) = 0;

};


/**
*Interface IDeferredCommand
*UUID: 56A868B8-0AD4-11CE-B03A-0020AF0BA770
*/
class IDeferredCommand : public Interface { 
public:
	virtual ~IDeferredCommand(){};

	virtual HRESULT Cancel(  ) = 0;

	virtual HRESULT Confidence( long* pConfidence ) = 0;

	virtual HRESULT Postpone( double newtime ) = 0;

	virtual HRESULT GetHResult( HRESULT* phrResult ) = 0;

};


/**
*Interface IFilterInfo
*UUID: 56A868BA-0AD4-11CE-B03A-0020AF0BA770
*/
class IFilterInfo : public Interface { 
public:
	virtual ~IFilterInfo(){};

	virtual void FindPin( BSTR strPinID, IDispatch** ppUnk ) = 0;//[id(1610743808)]

	virtual BSTR getName(  ) = 0;//[id(1610743809)]

	virtual BSTR getVendorInfo(  ) = 0;//[id(1610743810)]

	virtual IUnknown* getFilter(  ) = 0;//[id(1610743811)]

	virtual IDispatch* getPins(  ) = 0;//[id(1610743812)]

	virtual long getIsFileSource(  ) = 0;//[id(1610743813)]

	virtual BSTR getFilename(  ) = 0;//[id(1610743814)]

	virtual void setFilename( BSTR Val ) = 0;//[id(1610743814)]

};


/**
*Interface IMediaControl
*UUID: 56A868B1-0AD4-11CE-B03A-0020AF0BA770
*/
class IMediaControl : public Interface { 
public:
	virtual ~IMediaControl(){};

	virtual void Run(  ) = 0;//[id(1610743808)]

	virtual void Pause(  ) = 0;//[id(1610743809)]

	virtual void Stop(  ) = 0;//[id(1610743810)]

	virtual void GetState( long msTimeout, long* pfs ) = 0;//[id(1610743811)]

	virtual void RenderFile( BSTR strFilename ) = 0;//[id(1610743812)]

	virtual void AddSourceFilter( BSTR strFilename, IDispatch** ppUnk ) = 0;//[id(1610743813)]

	virtual IDispatch* getFilterCollection(  ) = 0;//[id(1610743814)]

	virtual IDispatch* getRegFilterCollection(  ) = 0;//[id(1610743815)]

	virtual void StopWhenReady(  ) = 0;//[id(1610743816)]

};


/**
*Interface IMediaEvent
*UUID: 56A868B6-0AD4-11CE-B03A-0020AF0BA770
*/
class IMediaEvent : public Interface { 
public:
	virtual ~IMediaEvent(){};

	virtual void GetEventHandle( LONG_PTR* hEvent ) = 0;//[id(1610743808)]

	virtual void GetEvent( long* lEventCode, LONG_PTR* lParam1, LONG_PTR* lParam2, long msTimeout ) = 0;//[id(1610743809)]

	virtual void WaitForCompletion( long msTimeout, long* pEvCode ) = 0;//[id(1610743810)]

	virtual void CancelDefaultHandling( long lEvCode ) = 0;//[id(1610743811)]

	virtual void RestoreDefaultHandling( long lEvCode ) = 0;//[id(1610743812)]

	virtual void FreeEventParams( long lEvCode, LONG_PTR lParam1, LONG_PTR lParam2 ) = 0;//[id(1610743813)]

};


/**
*Interface IMediaEventEx
*UUID: 56A868C0-0AD4-11CE-B03A-0020AF0BA770
*/
class IMediaEventEx : public Interface { 
public:
	virtual ~IMediaEventEx(){};

	virtual HRESULT SetNotifyWindow( LONG_PTR hwnd, long lMsg, LONG_PTR lInstanceData ) = 0;

	virtual HRESULT SetNotifyFlags( long lNoNotifyFlags ) = 0;

	virtual HRESULT GetNotifyFlags( long* lplNoNotifyFlags ) = 0;

};


/**
*Interface IMediaPosition
*UUID: 56A868B2-0AD4-11CE-B03A-0020AF0BA770
*/
class IMediaPosition : public Interface { 
public:
	virtual ~IMediaPosition(){};

	virtual double getDuration(  ) = 0;//[id(1610743808)]

	virtual void setCurrentPosition( double Val ) = 0;//[id(1610743809)]

	virtual double getCurrentPosition(  ) = 0;//[id(1610743809)]

	virtual double getStopTime(  ) = 0;//[id(1610743811)]

	virtual void setStopTime( double Val ) = 0;//[id(1610743811)]

	virtual double getPrerollTime(  ) = 0;//[id(1610743813)]

	virtual void setPrerollTime( double Val ) = 0;//[id(1610743813)]

	virtual void setRate( double Val ) = 0;//[id(1610743815)]

	virtual double getRate(  ) = 0;//[id(1610743815)]

	virtual long CanSeekForward(  ) = 0;//[id(1610743817)]

	virtual long CanSeekBackward(  ) = 0;//[id(1610743818)]

};


/**
*Interface IMediaTypeInfo
*UUID: 56A868BC-0AD4-11CE-B03A-0020AF0BA770
*/
class IMediaTypeInfo : public Interface { 
public:
	virtual ~IMediaTypeInfo(){};

	virtual BSTR getType(  ) = 0;//[id(1610743808)]

	virtual BSTR getSubtype(  ) = 0;//[id(1610743809)]

};


/**
*Interface IPinInfo
*UUID: 56A868BD-0AD4-11CE-B03A-0020AF0BA770
*/
class IPinInfo : public Interface { 
public:
	virtual ~IPinInfo(){};

	virtual IUnknown* getPin(  ) = 0;//[id(1610743808)]

	virtual IDispatch* getConnectedTo(  ) = 0;//[id(1610743809)]

	virtual IDispatch* getConnectionMediaType(  ) = 0;//[id(1610743810)]

	virtual IDispatch* getFilterInfo(  ) = 0;//[id(1610743811)]

	virtual BSTR getName(  ) = 0;//[id(1610743812)]

	virtual long getDirection(  ) = 0;//[id(1610743813)]

	virtual BSTR getPinID(  ) = 0;//[id(1610743814)]

	virtual IDispatch* getMediaTypes(  ) = 0;//[id(1610743815)]

	virtual void Connect( IUnknown* pPin ) = 0;//[id(1610743816)]

	virtual void ConnectDirect( IUnknown* pPin ) = 0;//[id(1610743817)]

	virtual void ConnectWithType( IUnknown* pPin, IDispatch* pMediaType ) = 0;//[id(1610743818)]

	virtual void Disconnect(  ) = 0;//[id(1610743819)]

	virtual void Render(  ) = 0;//[id(1610743820)]

};


/**
*Interface IQueueCommand
*UUID: 56A868B7-0AD4-11CE-B03A-0020AF0BA770
*/
class IQueueCommand : public Interface { 
public:
	virtual ~IQueueCommand(){};

	virtual HRESULT InvokeAtStreamTime( IDeferredCommand** pCmd, double time, GUID* iid, long dispidMethod, short wFlags, long cArgs, VARIANT* pDispParams, VARIANT* pvarResult, short* puArgErr ) = 0;

	virtual HRESULT InvokeAtPresentationTime( IDeferredCommand** pCmd, double time, GUID* iid, long dispidMethod, short wFlags, long cArgs, VARIANT* pDispParams, VARIANT* pvarResult, short* puArgErr ) = 0;

};


/**
*Interface IRegFilterInfo
*UUID: 56A868BB-0AD4-11CE-B03A-0020AF0BA770
*/
class IRegFilterInfo : public Interface { 
public:
	virtual ~IRegFilterInfo(){};

	virtual BSTR getName(  ) = 0;//[id(1610743808)]

	virtual void Filter( IDispatch** ppUnk ) = 0;//[id(1610743809)]

};


/**
*Interface IVideoWindow
*UUID: 56A868B4-0AD4-11CE-B03A-0020AF0BA770
*/
class IVideoWindow : public Interface { 
public:
	virtual ~IVideoWindow(){};

	virtual void setCaption( BSTR Val ) = 0;//[id(1610743808)]

	virtual BSTR getCaption(  ) = 0;//[id(1610743808)]

	virtual void setWindowStyle( long Val ) = 0;//[id(1610743810)]

	virtual long getWindowStyle(  ) = 0;//[id(1610743810)]

	virtual void setWindowStyleEx( long Val ) = 0;//[id(1610743812)]

	virtual long getWindowStyleEx(  ) = 0;//[id(1610743812)]

	virtual void setAutoShow( long Val ) = 0;//[id(1610743814)]

	virtual long getAutoShow(  ) = 0;//[id(1610743814)]

	virtual void setWindowState( long Val ) = 0;//[id(1610743816)]

	virtual long getWindowState(  ) = 0;//[id(1610743816)]

	virtual void setBackgroundPalette( long Val ) = 0;//[id(1610743818)]

	virtual long getBackgroundPalette(  ) = 0;//[id(1610743818)]

	virtual void setVisible( long Val ) = 0;//[id(1610743820)]

	virtual long getVisible(  ) = 0;//[id(1610743820)]

	virtual void setLeft( long Val ) = 0;//[id(1610743822)]

	virtual long getLeft(  ) = 0;//[id(1610743822)]

	virtual void setWidth( long Val ) = 0;//[id(1610743824)]

	virtual long getWidth(  ) = 0;//[id(1610743824)]

	virtual void setTop( long Val ) = 0;//[id(1610743826)]

	virtual long getTop(  ) = 0;//[id(1610743826)]

	virtual void setHeight( long Val ) = 0;//[id(1610743828)]

	virtual long getHeight(  ) = 0;//[id(1610743828)]

	virtual void setOwner( LONG_PTR Val ) = 0;//[id(1610743830)]

	virtual LONG_PTR getOwner(  ) = 0;//[id(1610743830)]

	virtual void setMessageDrain( LONG_PTR Val ) = 0;//[id(1610743832)]

	virtual LONG_PTR getMessageDrain(  ) = 0;//[id(1610743832)]

	virtual long getBorderColor(  ) = 0;//[id(1610743834)]

	virtual void setBorderColor( long Val ) = 0;//[id(1610743834)]

	virtual long getFullScreenMode(  ) = 0;//[id(1610743836)]

	virtual void setFullScreenMode( long Val ) = 0;//[id(1610743836)]

	virtual void SetWindowForeground( long Focus ) = 0;//[id(1610743838)]

	virtual void NotifyOwnerMessage( LONG_PTR hwnd, long uMsg, LONG_PTR wParam, LONG_PTR lParam ) = 0;//[id(1610743839)]

	virtual void SetWindowPosition( long Left, long Top, long Width, long Height ) = 0;//[id(1610743840)]

	virtual void GetWindowPosition( long* pLeft, long* pTop, long* pWidth, long* pHeight ) = 0;//[id(1610743841)]

	virtual void GetMinIdealImageSize( long* pWidth, long* pHeight ) = 0;//[id(1610743842)]

	virtual void GetMaxIdealImageSize( long* pWidth, long* pHeight ) = 0;//[id(1610743843)]

	virtual void GetRestorePosition( long* pLeft, long* pTop, long* pWidth, long* pHeight ) = 0;//[id(1610743844)]

	virtual void HideCursor( long HideCursor ) = 0;//[id(1610743845)]

	virtual void IsCursorHidden( long* CursorHidden ) = 0;//[id(1610743846)]

};


}  //end of namespace QuartzTypeLib

#endif //_QUARTZTYPELIBINTERFACES_H__

