//DexterLib.h

#ifndef _DEXTERLIBINTERFACES_H__
#define _DEXTERLIBINTERFACES_H__

#include "FoundationKit.h"

namespace DexterLib  {

/**
*Interface IAMErrorLog
*UUID: E43E73A2-0EFA-11D3-9601-00A0C9441E20
*/
class IAMErrorLog : public Interface { 
public:
	virtual ~IAMErrorLog(){};

	virtual HRESULT LogError( VARIANT* pExtraInfo ) = 0;

};


/**
*Interface IAMSetErrorLog
*UUID: 963566DA-BE21-4EAF-88E9-35704F8F52A1
*/
class IAMSetErrorLog : public Interface { 
public:
	virtual ~IAMSetErrorLog(){};

	virtual HRESULT getErrorLog( IAMErrorLog** pVal ) = 0;

	virtual HRESULT setErrorLog( IAMErrorLog* pVal ) = 0;

};


/**
*Interface IAMTimeline
*UUID: 78530B74-61F9-11D2-8CAD-00A024580902
*/
class IAMTimeline : public Interface { 
public:
	virtual ~IAMTimeline(){};

	virtual HRESULT CreateEmptyNode( IAMTimelineObj** ppObj,  ) = 0;

	virtual HRESULT AddGroup(  ) = 0;

	virtual HRESULT RemGroupFromList(  ) = 0;

	virtual HRESULT GetGroup( IAMTimelineObj** ppGroup,  ) = 0;

	virtual HRESULT GetGroupCount(  ) = 0;

	virtual HRESULT ClearAllGroups(  ) = 0;

	virtual HRESULT GetInsertMode(  ) = 0;

	virtual HRESULT SetInsertMode(  ) = 0;

	virtual HRESULT EnableTransitions(  ) = 0;

	virtual HRESULT TransitionsEnabled(  ) = 0;

	virtual HRESULT EnableEffects(  ) = 0;

	virtual HRESULT EffectsEnabled(  ) = 0;

	virtual HRESULT SetInterestRange(  ) = 0;

	virtual HRESULT GetDuration(  ) = 0;

	virtual HRESULT GetDuration2(  ) = 0;

	virtual HRESULT SetDefaultFPS(  ) = 0;

	virtual HRESULT GetDefaultFPS(  ) = 0;

	virtual HRESULT IsDirty(  ) = 0;

	virtual HRESULT GetDirtyRange(  ) = 0;

	virtual HRESULT GetCountOfType(  ) = 0;

	virtual HRESULT ValidateSourceNames(  ) = 0;

	virtual HRESULT SetDefaultTransition(  ) = 0;

	virtual HRESULT GetDefaultTransition(  ) = 0;

	virtual HRESULT SetDefaultEffect(  ) = 0;

	virtual HRESULT GetDefaultEffect(  ) = 0;

	virtual HRESULT SetDefaultTransitionB(  ) = 0;

	virtual HRESULT GetDefaultTransitionB( BSTR* pGuid ) = 0;

	virtual HRESULT SetDefaultEffectB(  ) = 0;

	virtual HRESULT GetDefaultEffectB( BSTR* pGuid ) = 0;

};


/**
*Interface IAMTimelineComp
*UUID: EAE58536-622E-11D2-8CAD-00A024580902
*/
class IAMTimelineComp : public Interface { 
public:
	virtual ~IAMTimelineComp(){};

	virtual HRESULT VTrackInsBefore(  ) = 0;

	virtual HRESULT VTrackSwapPriorities(  ) = 0;

	virtual HRESULT VTrackGetCount(  ) = 0;

	virtual HRESULT GetVTrack( IAMTimelineObj** ppVirtualTrack,  ) = 0;

	virtual HRESULT GetCountOfType(  ) = 0;

	virtual HRESULT GetRecursiveLayerOfType( IAMTimelineObj** ppVirtualTrack,  ) = 0;

	virtual HRESULT GetRecursiveLayerOfTypeI( IAMTimelineObj** ppVirtualTrack, long* pWhichLayer,  ) = 0;

	virtual HRESULT GetNextVTrack( IAMTimelineObj** ppNextVirtualTrack ) = 0;

};


/**
*Interface IAMTimelineEffect
*UUID: BCE0C264-622D-11D2-8CAD-00A024580902
*/
class IAMTimelineEffect : public Interface { 
public:
	virtual ~IAMTimelineEffect(){};

	virtual HRESULT EffectGetPriority(  ) = 0;

};


/**
*Interface IAMTimelineEffectable
*UUID: EAE58537-622E-11D2-8CAD-00A024580902
*/
class IAMTimelineEffectable : public Interface { 
public:
	virtual ~IAMTimelineEffectable(){};

	virtual HRESULT EffectInsBefore(  ) = 0;

	virtual HRESULT EffectSwapPriorities(  ) = 0;

	virtual HRESULT EffectGetCount(  ) = 0;

	virtual HRESULT GetEffect( IAMTimelineObj** ppFx,  ) = 0;

};


/**
*Interface IAMTimelineGroup
*UUID: 9EED4F00-B8A6-11D2-8023-00C0DF10D434
*/
class IAMTimelineGroup : public Interface { 
public:
	virtual ~IAMTimelineGroup(){};

	virtual HRESULT SetTimeline(  ) = 0;

	virtual HRESULT GetTimeline( IAMTimeline** ppTimeline ) = 0;

	virtual HRESULT GetPriority(  ) = 0;

	virtual HRESULT GetMediaType( _AMMediaType* __MIDL_0040 ) = 0;

	virtual HRESULT SetMediaType( _AMMediaType* __MIDL_0041 ) = 0;

	virtual HRESULT SetOutputFPS(  ) = 0;

	virtual HRESULT GetOutputFPS(  ) = 0;

	virtual HRESULT SetGroupName(  ) = 0;

	virtual HRESULT GetGroupName( BSTR* pGroupName ) = 0;

	virtual HRESULT SetPreviewMode(  ) = 0;

	virtual HRESULT GetPreviewMode(  ) = 0;

	virtual HRESULT SetMediaTypeForVB( long Val ) = 0;

	virtual HRESULT GetOutputBuffering( int* pnBuffer ) = 0;

	virtual HRESULT SetOutputBuffering( int nBuffer ) = 0;

	virtual HRESULT SetSmartRecompressFormat(  ) = 0;

	virtual HRESULT GetSmartRecompressFormat(  ) = 0;

	virtual HRESULT IsSmartRecompressFormatSet(  ) = 0;

	virtual HRESULT IsRecompressFormatDirty(  ) = 0;

	virtual HRESULT ClearRecompressFormatDirty(  ) = 0;

	virtual HRESULT SetRecompFormatFromSource(  ) = 0;

};


/**
*Interface IAMTimelineObj
*UUID: 78530B77-61F9-11D2-8CAD-00A024580902
*/
class IAMTimelineObj : public Interface { 
public:
	virtual ~IAMTimelineObj(){};

	virtual HRESULT GetStartStop(  ) = 0;

	virtual HRESULT GetStartStop2(  ) = 0;

	virtual HRESULT FixTimes(  ) = 0;

	virtual HRESULT FixTimes2(  ) = 0;

	virtual HRESULT SetStartStop(  ) = 0;

	virtual HRESULT SetStartStop2(  ) = 0;

	virtual HRESULT GetPropertySetter( IPropertySetter** pVal ) = 0;

	virtual HRESULT SetPropertySetter(  ) = 0;

	virtual HRESULT GetSubObject( IUnknown** pVal ) = 0;

	virtual HRESULT SetSubObject(  ) = 0;

	virtual HRESULT SetSubObjectGUID(  ) = 0;

	virtual HRESULT SetSubObjectGUIDB(  ) = 0;

	virtual HRESULT GetSubObjectGUID(  ) = 0;

	virtual HRESULT GetSubObjectGUIDB( BSTR* pVal ) = 0;

	virtual HRESULT GetSubObjectLoaded(  ) = 0;

	virtual HRESULT GetTimelineType(  ) = 0;

	virtual HRESULT SetTimelineType(  ) = 0;

	virtual HRESULT GetUserID(  ) = 0;

	virtual HRESULT SetUserID(  ) = 0;

	virtual HRESULT GetGenID(  ) = 0;

	virtual HRESULT GetUserName( BSTR* pVal ) = 0;

	virtual HRESULT SetUserName(  ) = 0;

	virtual HRESULT GetUserData(  ) = 0;

	virtual HRESULT SetUserData(  ) = 0;

	virtual HRESULT GetMuted(  ) = 0;

	virtual HRESULT SetMuted(  ) = 0;

	virtual HRESULT GetLocked(  ) = 0;

	virtual HRESULT SetLocked(  ) = 0;

	virtual HRESULT GetDirtyRange(  ) = 0;

	virtual HRESULT GetDirtyRange2(  ) = 0;

	virtual HRESULT SetDirtyRange(  ) = 0;

	virtual HRESULT SetDirtyRange2(  ) = 0;

	virtual HRESULT ClearDirty(  ) = 0;

	virtual HRESULT Remove(  ) = 0;

	virtual HRESULT RemoveAll(  ) = 0;

	virtual HRESULT GetTimelineNoRef(  ) = 0;

	virtual HRESULT GetGroupIBelongTo( IAMTimelineGroup** ppGroup ) = 0;

	virtual HRESULT GetEmbedDepth(  ) = 0;

};


/**
*Interface IAMTimelineSplittable
*UUID: A0F840A0-D590-11D2-8D55-00A0C9441E20
*/
class IAMTimelineSplittable : public Interface { 
public:
	virtual ~IAMTimelineSplittable(){};

	virtual HRESULT SplitAt(  ) = 0;

	virtual HRESULT SplitAt2(  ) = 0;

};


/**
*Interface IAMTimelineSrc
*UUID: 78530B79-61F9-11D2-8CAD-00A024580902
*/
class IAMTimelineSrc : public Interface { 
public:
	virtual ~IAMTimelineSrc(){};

	virtual HRESULT GetMediaTimes(  ) = 0;

	virtual HRESULT GetMediaTimes2(  ) = 0;

	virtual HRESULT ModifyStopTime(  ) = 0;

	virtual HRESULT ModifyStopTime2(  ) = 0;

	virtual HRESULT FixMediaTimes(  ) = 0;

	virtual HRESULT FixMediaTimes2(  ) = 0;

	virtual HRESULT SetMediaTimes(  ) = 0;

	virtual HRESULT SetMediaTimes2(  ) = 0;

	virtual HRESULT SetMediaLength(  ) = 0;

	virtual HRESULT SetMediaLength2(  ) = 0;

	virtual HRESULT GetMediaLength(  ) = 0;

	virtual HRESULT GetMediaLength2(  ) = 0;

	virtual HRESULT GetMediaName( BSTR* pVal ) = 0;

	virtual HRESULT SetMediaName(  ) = 0;

	virtual HRESULT SpliceWithNext(  ) = 0;

	virtual HRESULT GetStreamNumber(  ) = 0;

	virtual HRESULT SetStreamNumber(  ) = 0;

	virtual HRESULT IsNormalRate(  ) = 0;

	virtual HRESULT GetDefaultFPS(  ) = 0;

	virtual HRESULT SetDefaultFPS(  ) = 0;

	virtual HRESULT GetStretchMode(  ) = 0;

	virtual HRESULT SetStretchMode(  ) = 0;

};


/**
*Interface IAMTimelineTrack
*UUID: EAE58538-622E-11D2-8CAD-00A024580902
*/
class IAMTimelineTrack : public Interface { 
public:
	virtual ~IAMTimelineTrack(){};

	virtual HRESULT SrcAdd(  ) = 0;

	virtual HRESULT GetNextSrc( IAMTimelineObj** ppSrc,  ) = 0;

	virtual HRESULT GetNextSrc2( IAMTimelineObj** ppSrc,  ) = 0;

	virtual HRESULT MoveEverythingBy(  ) = 0;

	virtual HRESULT MoveEverythingBy2(  ) = 0;

	virtual HRESULT GetSourcesCount(  ) = 0;

	virtual HRESULT AreYouBlank(  ) = 0;

	virtual HRESULT GetSrcAtTime( IAMTimelineObj** ppSrc,  ) = 0;

	virtual HRESULT GetSrcAtTime2( IAMTimelineObj** ppSrc,  ) = 0;

	virtual HRESULT InsertSpace(  ) = 0;

	virtual HRESULT InsertSpace2(  ) = 0;

	virtual HRESULT ZeroBetween(  ) = 0;

	virtual HRESULT ZeroBetween2(  ) = 0;

	virtual HRESULT GetNextSrcEx( IAMTimelineObj** ppNext ) = 0;

};


/**
*Interface IAMTimelineTrans
*UUID: BCE0C265-622D-11D2-8CAD-00A024580902
*/
class IAMTimelineTrans : public Interface { 
public:
	virtual ~IAMTimelineTrans(){};

	virtual HRESULT GetCutPoint(  ) = 0;

	virtual HRESULT GetCutPoint2(  ) = 0;

	virtual HRESULT SetCutPoint(  ) = 0;

	virtual HRESULT SetCutPoint2(  ) = 0;

	virtual HRESULT GetSwapInputs(  ) = 0;

	virtual HRESULT SetSwapInputs(  ) = 0;

	virtual HRESULT GetCutsOnly(  ) = 0;

	virtual HRESULT SetCutsOnly(  ) = 0;

};


/**
*Interface IAMTimelineTransable
*UUID: 378FA386-622E-11D2-8CAD-00A024580902
*/
class IAMTimelineTransable : public Interface { 
public:
	virtual ~IAMTimelineTransable(){};

	virtual HRESULT TransAdd(  ) = 0;

	virtual HRESULT TransGetCount(  ) = 0;

	virtual HRESULT GetNextTrans( IAMTimelineObj** ppTrans,  ) = 0;

	virtual HRESULT GetNextTrans2( IAMTimelineObj** ppTrans,  ) = 0;

	virtual HRESULT GetTransAtTime( IAMTimelineObj** ppObj,  ) = 0;

	virtual HRESULT GetTransAtTime2( IAMTimelineObj** ppObj,  ) = 0;

};


/**
*Interface IAMTimelineVirtualTrack
*UUID: A8ED5F80-C2C7-11D2-8D39-00A0C9441E20
*/
class IAMTimelineVirtualTrack : public Interface { 
public:
	virtual ~IAMTimelineVirtualTrack(){};

	virtual HRESULT TrackGetPriority(  ) = 0;

	virtual HRESULT SetTrackDirty(  ) = 0;

};


/**
*Interface IBaseFilter
*UUID: 56A86895-0AD4-11CE-B03A-0020AF0BA770
*/
class IBaseFilter : public Interface { 
public:
	virtual ~IBaseFilter(){};

	virtual HRESULT EnumPins( IEnumPins** ppEnum ) = 0;

	virtual HRESULT FindPin( String Id, IPin** ppPin ) = 0;

	virtual HRESULT QueryFilterInfo( _FilterInfo* pInfo ) = 0;

	virtual HRESULT JoinFilterGraph( IFilterGraph* pGraph, String pName ) = 0;

	virtual HRESULT QueryVendorInfo( String* pVendorInfo ) = 0;

};


/**
*Interface IDXEffect
*UUID: E31FB81B-1335-11D1-8189-0000F87557DB
*/
class IDXEffect : public Interface { 
public:
	virtual ~IDXEffect(){};

	virtual long getCapabilities(  ) = 0;//[id(10000)]

	virtual float getProgress(  ) = 0;//[id(10001)]

	virtual void setProgress( float Val ) = 0;//[id(10001)]

	virtual float getStepResolution(  ) = 0;//[id(10002)]

	virtual float getDuration(  ) = 0;//[id(10003)]

	virtual void setDuration( float Val ) = 0;//[id(10003)]

};


/**
*Interface IDxtAlphaSetter
*UUID: 4EE9EAD9-DA4D-43D0-9383-06B90C08B12B
*/
class IDxtAlphaSetter : public Interface { 
public:
	virtual ~IDxtAlphaSetter(){};

	virtual long getCapabilities(  ) = 0;//[id(10000)]

	virtual float getProgress(  ) = 0;//[id(10001)]

	virtual void setProgress( float Val ) = 0;//[id(10001)]

	virtual float getStepResolution(  ) = 0;//[id(10002)]

	virtual float getDuration(  ) = 0;//[id(10003)]

	virtual void setDuration( float Val ) = 0;//[id(10003)]

	virtual long getAlpha(  ) = 0;//[id(1)]

	virtual void setAlpha( long Val ) = 0;//[id(1)]

	virtual double getAlphaRamp(  ) = 0;//[id(2)]

	virtual void setAlphaRamp( double Val ) = 0;//[id(2)]

};


/**
*Interface IDxtCompositor
*UUID: BB44391E-6ABD-422F-9E2E-385C9DFF51FC
*/
class IDxtCompositor : public Interface { 
public:
	virtual ~IDxtCompositor(){};

	virtual long getCapabilities(  ) = 0;//[id(10000)]

	virtual float getProgress(  ) = 0;//[id(10001)]

	virtual void setProgress( float Val ) = 0;//[id(10001)]

	virtual float getStepResolution(  ) = 0;//[id(10002)]

	virtual float getDuration(  ) = 0;//[id(10003)]

	virtual void setDuration( float Val ) = 0;//[id(10003)]

	virtual long getOffsetX(  ) = 0;//[id(1)]

	virtual void setOffsetX( long Val ) = 0;//[id(1)]

	virtual long getOffsetY(  ) = 0;//[id(2)]

	virtual void setOffsetY( long Val ) = 0;//[id(2)]

	virtual long getWidth(  ) = 0;//[id(3)]

	virtual void setWidth( long Val ) = 0;//[id(3)]

	virtual long getHeight(  ) = 0;//[id(4)]

	virtual void setHeight( long Val ) = 0;//[id(4)]

	virtual long getSrcOffsetX(  ) = 0;//[id(5)]

	virtual void setSrcOffsetX( long Val ) = 0;//[id(5)]

	virtual long getSrcOffsetY(  ) = 0;//[id(6)]

	virtual void setSrcOffsetY( long Val ) = 0;//[id(6)]

	virtual long getSrcWidth(  ) = 0;//[id(7)]

	virtual void setSrcWidth( long Val ) = 0;//[id(7)]

	virtual long getSrcHeight(  ) = 0;//[id(8)]

	virtual void setSrcHeight( long Val ) = 0;//[id(8)]

};


/**
*Interface IDxtJpeg
*UUID: DE75D011-7A65-11D2-8CEA-00A0C9441E20
*/
class IDxtJpeg : public Interface { 
public:
	virtual ~IDxtJpeg(){};

	virtual long getCapabilities(  ) = 0;//[id(10000)]

	virtual float getProgress(  ) = 0;//[id(10001)]

	virtual void setProgress( float Val ) = 0;//[id(10001)]

	virtual float getStepResolution(  ) = 0;//[id(10002)]

	virtual float getDuration(  ) = 0;//[id(10003)]

	virtual void setDuration( float Val ) = 0;//[id(10003)]

	virtual long getMaskNum(  ) = 0;//[id(1)]

	virtual void setMaskNum( long Val ) = 0;//[id(1)]

	virtual BSTR getMaskName(  ) = 0;//[id(2)]

	virtual void setMaskName( BSTR Val ) = 0;//[id(2)]

	virtual double getScaleX(  ) = 0;//[id(3)]

	virtual void setScaleX( double Val ) = 0;//[id(3)]

	virtual double getScaleY(  ) = 0;//[id(4)]

	virtual void setScaleY( double Val ) = 0;//[id(4)]

	virtual long getOffsetX(  ) = 0;//[id(5)]

	virtual void setOffsetX( long Val ) = 0;//[id(5)]

	virtual long getOffsetY(  ) = 0;//[id(6)]

	virtual void setOffsetY( long Val ) = 0;//[id(6)]

	virtual long getReplicateX(  ) = 0;//[id(7)]

	virtual void setReplicateX( long Val ) = 0;//[id(7)]

	virtual long getReplicateY(  ) = 0;//[id(8)]

	virtual void setReplicateY( long Val ) = 0;//[id(8)]

	virtual long getBorderColor(  ) = 0;//[id(9)]

	virtual void setBorderColor( long Val ) = 0;//[id(9)]

	virtual long getBorderWidth(  ) = 0;//[id(10)]

	virtual void setBorderWidth( long Val ) = 0;//[id(10)]

	virtual long getBorderSoftness(  ) = 0;//[id(11)]

	virtual void setBorderSoftness( long Val ) = 0;//[id(11)]

	virtual void ApplyChanges(  ) = 0;//[id(1610809366)]

	virtual void LoadDefSettings(  ) = 0;//[id(1610809367)]

};


/**
*Interface IDxtKey
*UUID: 3255DE56-38FB-4901-B980-94B438010D7B
*/
class IDxtKey : public Interface { 
public:
	virtual ~IDxtKey(){};

	virtual long getCapabilities(  ) = 0;//[id(10000)]

	virtual float getProgress(  ) = 0;//[id(10001)]

	virtual void setProgress( float Val ) = 0;//[id(10001)]

	virtual float getStepResolution(  ) = 0;//[id(10002)]

	virtual float getDuration(  ) = 0;//[id(10003)]

	virtual void setDuration( float Val ) = 0;//[id(10003)]

	virtual int getKeyType(  ) = 0;//[id(1)]

	virtual void setKeyType( int Val ) = 0;//[id(1)]

	virtual int getHue(  ) = 0;//[id(2)]

	virtual void setHue( int Val ) = 0;//[id(2)]

	virtual int getLuminance(  ) = 0;//[id(3)]

	virtual void setLuminance( int Val ) = 0;//[id(3)]

	virtual unsigned long getRGB(  ) = 0;//[id(4)]

	virtual void setRGB( unsigned long Val ) = 0;//[id(4)]

	virtual int getSimilarity(  ) = 0;//[id(5)]

	virtual void setSimilarity( int Val ) = 0;//[id(5)]

	virtual long getInvert(  ) = 0;//[id(6)]

	virtual void setInvert( long Val ) = 0;//[id(6)]

};


/**
*Interface IEnumFilters
*UUID: 56A86893-0AD4-11CE-B03A-0020AF0BA770
*/
class IEnumFilters : public Interface { 
public:
	virtual ~IEnumFilters(){};

	virtual HRESULT Next( unsigned long cFilters, IBaseFilter** ppFilter, unsigned long* pcFetched ) = 0;

	virtual HRESULT Skip( unsigned long cFilters ) = 0;

	virtual HRESULT Reset(  ) = 0;

	virtual HRESULT Clone( IEnumFilters** ppEnum ) = 0;

};


/**
*Interface IEnumMediaTypes
*UUID: 89C31040-846B-11CE-97D3-00AA0055595A
*/
class IEnumMediaTypes : public Interface { 
public:
	virtual ~IEnumMediaTypes(){};

	virtual HRESULT Next( unsigned long cMediaTypes, _AMMediaType** ppMediaTypes, unsigned long* pcFetched ) = 0;

	virtual HRESULT Skip( unsigned long cMediaTypes ) = 0;

	virtual HRESULT Reset(  ) = 0;

	virtual HRESULT Clone( IEnumMediaTypes** ppEnum ) = 0;

};


/**
*Interface IEnumPins
*UUID: 56A86892-0AD4-11CE-B03A-0020AF0BA770
*/
class IEnumPins : public Interface { 
public:
	virtual ~IEnumPins(){};

	virtual HRESULT Next( unsigned long cPins, IPin** ppPins, unsigned long* pcFetched ) = 0;

	virtual HRESULT Skip( unsigned long cPins ) = 0;

	virtual HRESULT Reset(  ) = 0;

	virtual HRESULT Clone( IEnumPins** ppEnum ) = 0;

};


/**
*Interface IErrorLog
*UUID: 3127CA40-446E-11CE-8135-00AA004BB851
*/
class IErrorLog : public Interface { 
public:
	virtual ~IErrorLog(){};

	virtual HRESULT AddError( String pszPropName, EXCEPINFO* pExcepInfo ) = 0;

};


/**
*Interface IFilterGraph
*UUID: 56A8689F-0AD4-11CE-B03A-0020AF0BA770
*/
class IFilterGraph : public Interface { 
public:
	virtual ~IFilterGraph(){};

	virtual HRESULT AddFilter( IBaseFilter* pFilter, String pName ) = 0;

	virtual HRESULT RemoveFilter( IBaseFilter* pFilter ) = 0;

	virtual HRESULT EnumFilters( IEnumFilters** ppEnum ) = 0;

	virtual HRESULT FindFilterByName( String pName, IBaseFilter** ppFilter ) = 0;

	virtual HRESULT ConnectDirect( IPin* ppinOut, IPin* ppinIn, _AMMediaType* pmt ) = 0;

	virtual HRESULT Reconnect( IPin* pPin ) = 0;

	virtual HRESULT Disconnect( IPin* pPin ) = 0;

	virtual HRESULT SetDefaultSyncSource(  ) = 0;

};


/**
*Interface IFindCompressorCB
*UUID: F03FA8DE-879A-4D59-9B2C-26BB1CF83461
*/
class IFindCompressorCB : public Interface { 
public:
	virtual ~IFindCompressorCB(){};

	virtual HRESULT GetCompressor( IBaseFilter** ppFilter ) = 0;

};


/**
*Interface IGraphBuilder
*UUID: 56A868A9-0AD4-11CE-B03A-0020AF0BA770
*/
class IGraphBuilder : public Interface { 
public:
	virtual ~IGraphBuilder(){};

	virtual HRESULT Connect( IPin* ppinOut, IPin* ppinIn ) = 0;

	virtual HRESULT Render( IPin* ppinOut ) = 0;

	virtual HRESULT RenderFile( String lpcwstrFile, String lpcwstrPlayList ) = 0;

	virtual HRESULT AddSourceFilter( String lpcwstrFileName, String lpcwstrFilterName, IBaseFilter** ppFilter ) = 0;

	virtual HRESULT SetLogFile( ULONG_PTR hFile ) = 0;

	virtual HRESULT Abort(  ) = 0;

	virtual HRESULT ShouldOperationContinue(  ) = 0;

};


/**
*Interface IGrfCache
*UUID: AE9472BE-B0C3-11D2-8D24-00A0C9441E20
*/
class IGrfCache : public Interface { 
public:
	virtual ~IGrfCache(){};

	virtual HRESULT AddFilter(  ) = 0;

	virtual HRESULT ConnectPins(  ) = 0;

	virtual HRESULT SetGraph(  ) = 0;

	virtual HRESULT DoConnectionsNow(  ) = 0;

};


/**
*Interface IMediaDet
*UUID: 65BD0710-24D2-4FF7-9324-ED2E5D3ABAFA
*/
class IMediaDet : public Interface { 
public:
	virtual ~IMediaDet(){};

	virtual HRESULT getFilter( IUnknown** pVal ) = 0;

	virtual HRESULT setFilter( IUnknown* pVal ) = 0;

	virtual HRESULT getOutputStreams( long* pVal ) = 0;

	virtual HRESULT getCurrentStream( long* pVal ) = 0;

	virtual HRESULT setCurrentStream( long pVal ) = 0;

	virtual HRESULT getStreamType( GUID* pVal ) = 0;

	virtual HRESULT getStreamTypeB( BSTR* pVal ) = 0;

	virtual HRESULT getStreamLength( double* pVal ) = 0;

	virtual HRESULT getFilename( BSTR* pVal ) = 0;

	virtual HRESULT setFilename( BSTR pVal ) = 0;

	virtual HRESULT GetBitmapBits(  ) = 0;

	virtual HRESULT WriteBitmapBits(  ) = 0;

	virtual HRESULT getStreamMediaType( _AMMediaType* pVal ) = 0;

	virtual HRESULT GetSampleGrabber( ISampleGrabber** ppVal ) = 0;

	virtual HRESULT getFrameRate( double* pVal ) = 0;

	virtual HRESULT EnterBitmapGrabMode(  ) = 0;

};


/**
*Interface IMediaFilter
*UUID: 56A86899-0AD4-11CE-B03A-0020AF0BA770
*/
class IMediaFilter : public Interface { 
public:
	virtual ~IMediaFilter(){};

	virtual HRESULT Stop(  ) = 0;

	virtual HRESULT Pause(  ) = 0;

	virtual HRESULT Run(  ) = 0;

	virtual HRESULT GetState( unsigned long dwMilliSecsTimeout, _FilterState* State ) = 0;

	virtual HRESULT SetSyncSource( IReferenceClock* pClock ) = 0;

	virtual HRESULT GetSyncSource( IReferenceClock** pClock ) = 0;

};


/**
*Interface IMediaLocator
*UUID: 288581E0-66CE-11D2-918F-00C0DF10D434
*/
class IMediaLocator : public Interface { 
public:
	virtual ~IMediaLocator(){};

	virtual HRESULT FindMediaFile(  ) = 0;

	virtual HRESULT AddFoundLocation(  ) = 0;

};


/**
*Interface IMediaSample
*UUID: 56A8689A-0AD4-11CE-B03A-0020AF0BA770
*/
class IMediaSample : public Interface { 
public:
	virtual ~IMediaSample(){};

	virtual HRESULT GetPointer( unsigned char** ppBuffer ) = 0;

	virtual long GetSize(  ) = 0;

	virtual HRESULT GetTime( int64* pTimeStart, int64* pTimeEnd ) = 0;

	virtual HRESULT SetTime( int64* pTimeStart, int64* pTimeEnd ) = 0;

	virtual HRESULT IsSyncPoint(  ) = 0;

	virtual HRESULT SetSyncPoint(  ) = 0;

	virtual HRESULT IsPreroll(  ) = 0;

	virtual HRESULT SetPreroll(  ) = 0;

	virtual long GetActualDataLength(  ) = 0;

	virtual HRESULT SetActualDataLength(  ) = 0;

	virtual HRESULT GetMediaType(  ) = 0;

	virtual HRESULT SetMediaType(  ) = 0;

	virtual HRESULT IsDiscontinuity(  ) = 0;

	virtual HRESULT SetDiscontinuity(  ) = 0;

	virtual HRESULT GetMediaTime( int64* pTimeStart, int64* pTimeEnd ) = 0;

	virtual HRESULT SetMediaTime( int64* pTimeStart, int64* pTimeEnd ) = 0;

};


/**
*Interface IPersist
*UUID: 0000010C-0000-0000-C000-000000000046
*/
class IPersist : public Interface { 
public:
	virtual ~IPersist(){};

	virtual HRESULT GetClassID( GUID* pClassID ) = 0;

};


/**
*Interface IPersistStream
*UUID: 00000109-0000-0000-C000-000000000046
*/
class IPersistStream : public Interface { 
public:
	virtual ~IPersistStream(){};

	virtual HRESULT IsDirty(  ) = 0;

	virtual HRESULT Load( IStream* pstm ) = 0;

	virtual HRESULT Save( IStream* pstm, long fClearDirty ) = 0;

	virtual HRESULT GetSizeMax( _ULARGE_INTEGER* pcbSize ) = 0;

};


/**
*Interface IPin
*UUID: 56A86891-0AD4-11CE-B03A-0020AF0BA770
*/
class IPin : public Interface { 
public:
	virtual ~IPin(){};

	virtual HRESULT Connect( IPin* pReceivePin, _AMMediaType* pmt ) = 0;

	virtual HRESULT ReceiveConnection( IPin* pConnector, _AMMediaType* pmt ) = 0;

	virtual HRESULT Disconnect(  ) = 0;

	virtual HRESULT ConnectedTo( IPin** pPin ) = 0;

	virtual HRESULT ConnectionMediaType( _AMMediaType* pmt ) = 0;

	virtual HRESULT QueryPinInfo( _PinInfo* pInfo ) = 0;

	virtual HRESULT QueryDirection( _PinDirection* pPinDir ) = 0;

	virtual HRESULT QueryId( String* Id ) = 0;

	virtual HRESULT QueryAccept( _AMMediaType* pmt ) = 0;

	virtual HRESULT EnumMediaTypes( IEnumMediaTypes** ppEnum ) = 0;

	virtual HRESULT QueryInternalConnections( IPin** apPin, unsigned long* nPin ) = 0;

	virtual HRESULT EndOfStream(  ) = 0;

	virtual HRESULT BeginFlush(  ) = 0;

	virtual HRESULT EndFlush(  ) = 0;

	virtual HRESULT NewSegment( int64 tStart, int64 tStop, double dRate ) = 0;

};


/**
*Interface IPropertyBag
*UUID: 55272A00-42CB-11CE-8135-00AA004BB851
*/
class IPropertyBag : public Interface { 
public:
	virtual ~IPropertyBag(){};

	virtual HRESULT RemoteRead( String pszPropName, VARIANT* pVar, IErrorLog* pErrorLog, unsigned long varType, IUnknown* pUnkObj ) = 0;

	virtual HRESULT Write( String pszPropName, VARIANT* pVar ) = 0;

};


/**
*Interface IPropertySetter
*UUID: AE9472BD-B0C3-11D2-8D24-00A0C9441E20
*/
class IPropertySetter : public Interface { 
public:
	virtual ~IPropertySetter(){};

	virtual HRESULT LoadXML( IUnknown* pxml ) = 0;

	virtual HRESULT PrintXML( unsigned char* pszXML, int cbXML, int* pcbPrinted, int indent ) = 0;

	virtual HRESULT CloneProps( IPropertySetter** ppSetter, int64 rtStart, int64 rtStop ) = 0;

	virtual HRESULT AddProp( DEXTER_PARAM Param, DEXTER_VALUE* paValue ) = 0;

	virtual HRESULT GetProps( long* pcParams, DEXTER_PARAM** paParam, DEXTER_VALUE** paValue ) = 0;

	virtual HRESULT FreeProps( long cParams, DEXTER_PARAM* paParam, DEXTER_VALUE* paValue ) = 0;

	virtual HRESULT ClearProps(  ) = 0;

	virtual HRESULT SaveToBlob( long* pcSize, unsigned char** ppb ) = 0;

	virtual HRESULT LoadFromBlob( long cSize, unsigned char* pb ) = 0;

	virtual HRESULT SetProps( IUnknown* pTarget, int64 rtNow ) = 0;

};


/**
*Interface IReferenceClock
*UUID: 56A86897-0AD4-11CE-B03A-0020AF0BA770
*/
class IReferenceClock : public Interface { 
public:
	virtual ~IReferenceClock(){};

	virtual HRESULT GetTime( int64* pTime ) = 0;

	virtual HRESULT AdviseTime( int64 baseTime, int64 streamTime, ULONG_PTR hEvent, ULONG_PTR* pdwAdviseCookie ) = 0;

	virtual HRESULT AdvisePeriodic( int64 startTime, int64 periodTime, ULONG_PTR hSemaphore, ULONG_PTR* pdwAdviseCookie ) = 0;

	virtual HRESULT Unadvise( ULONG_PTR dwAdviseCookie ) = 0;

};


/**
*Interface IRenderEngine
*UUID: 6BEE3A81-66C9-11D2-918F-00C0DF10D434
*/
class IRenderEngine : public Interface { 
public:
	virtual ~IRenderEngine(){};

	virtual HRESULT SetTimelineObject(  ) = 0;

	virtual HRESULT GetTimelineObject( IAMTimeline** ppTimeline ) = 0;

	virtual HRESULT GetFilterGraph( IGraphBuilder** ppFG ) = 0;

	virtual HRESULT SetFilterGraph(  ) = 0;

	virtual HRESULT SetInterestRange(  ) = 0;

	virtual HRESULT SetInterestRange2(  ) = 0;

	virtual HRESULT SetRenderRange(  ) = 0;

	virtual HRESULT SetRenderRange2(  ) = 0;

	virtual HRESULT GetGroupOutputPin( IPin** ppRenderPin ) = 0;

	virtual HRESULT ScrapIt(  ) = 0;

	virtual HRESULT RenderOutputPins(  ) = 0;

	virtual HRESULT GetVendorString( BSTR* pVendorID ) = 0;

	virtual HRESULT ConnectFrontEnd(  ) = 0;

	virtual HRESULT SetSourceConnectCallback(  ) = 0;

	virtual HRESULT SetDynamicReconnectLevel(  ) = 0;

	virtual HRESULT DoSmartRecompression(  ) = 0;

	virtual HRESULT UseInSmartRecompressionGraph(  ) = 0;

	virtual HRESULT SetSourceNameValidation(  ) = 0;

	virtual HRESULT Commit(  ) = 0;

	virtual HRESULT Decommit(  ) = 0;

	virtual HRESULT GetCaps(  ) = 0;

};


/**
*Interface ISampleGrabber
*UUID: 6B652FFF-11FE-4FCE-92AD-0266B5D7C78F
*/
class ISampleGrabber : public Interface { 
public:
	virtual ~ISampleGrabber(){};

	virtual HRESULT SetOneShot(  ) = 0;

	virtual HRESULT SetMediaType(  ) = 0;

	virtual HRESULT GetConnectedMediaType(  ) = 0;

	virtual HRESULT SetBufferSamples(  ) = 0;

	virtual HRESULT GetCurrentBuffer( long* pBufferSize, long* pBuffer ) = 0;

	virtual HRESULT GetCurrentSample( IMediaSample** ppSample ) = 0;

	virtual HRESULT SetCallback(  ) = 0;

};


/**
*Interface ISampleGrabberCB
*UUID: 0579154A-2B53-4994-B0D0-E773148EFF85
*/
class ISampleGrabberCB : public Interface { 
public:
	virtual ~ISampleGrabberCB(){};

	virtual HRESULT SampleCB(  ) = 0;

	virtual HRESULT BufferCB(  ) = 0;

};


/**
*Interface ISequentialStream
*UUID: 0C733A30-2A1C-11CE-ADE5-00AA0044773D
*/
class ISequentialStream : public Interface { 
public:
	virtual ~ISequentialStream(){};

	virtual HRESULT RemoteRead( unsigned char* pv, unsigned long cb, unsigned long* pcbRead ) = 0;

	virtual HRESULT RemoteWrite( unsigned char* pv, unsigned long cb, unsigned long* pcbWritten ) = 0;

};


/**
*Interface ISmartRenderEngine
*UUID: F03FA8CE-879A-4D59-9B2C-26BB1CF83461
*/
class ISmartRenderEngine : public Interface { 
public:
	virtual ~ISmartRenderEngine(){};

	virtual HRESULT SetGroupCompressor(  ) = 0;

	virtual HRESULT GetGroupCompressor(  ) = 0;

	virtual HRESULT SetFindCompressorCB(  ) = 0;

};


/**
*Interface IStream
*UUID: 0000000C-0000-0000-C000-000000000046
*/
class IStream : public Interface { 
public:
	virtual ~IStream(){};

	virtual HRESULT RemoteSeek( _LARGE_INTEGER dlibMove, unsigned long dwOrigin, _ULARGE_INTEGER* plibNewPosition ) = 0;

	virtual HRESULT SetSize( _ULARGE_INTEGER libNewSize ) = 0;

	virtual HRESULT RemoteCopyTo( IStream* pstm, _ULARGE_INTEGER cb, _ULARGE_INTEGER* pcbRead, _ULARGE_INTEGER* pcbWritten ) = 0;

	virtual HRESULT Commit( unsigned long grfCommitFlags ) = 0;

	virtual HRESULT Revert(  ) = 0;

	virtual HRESULT LockRegion( _ULARGE_INTEGER libOffset, _ULARGE_INTEGER cb, unsigned long dwLockType ) = 0;

	virtual HRESULT UnlockRegion( _ULARGE_INTEGER libOffset, _ULARGE_INTEGER cb, unsigned long dwLockType ) = 0;

	virtual HRESULT Stat( tagSTATSTG* pstatstg, unsigned long grfStatFlag ) = 0;

	virtual HRESULT Clone( IStream** ppstm ) = 0;

};


/**
*Interface IXml2Dex
*UUID: 18C628ED-962A-11D2-8D08-00A0C9441E20
*/
class IXml2Dex : public Interface { 
public:
	virtual ~IXml2Dex(){};

	virtual HRESULT CreateGraphFromFile( IUnknown** ppGraph,  ) = 0;

	virtual HRESULT WriteGrfFile(  ) = 0;

	virtual HRESULT WriteXMLFile(  ) = 0;

	virtual HRESULT ReadXMLFile(  ) = 0;

	virtual HRESULT Delete(  ) = 0;

	virtual HRESULT WriteXMLPart(  ) = 0;

	virtual HRESULT PasteXMLFile(  ) = 0;

	virtual HRESULT CopyXML(  ) = 0;

	virtual HRESULT PasteXML(  ) = 0;

	virtual HRESULT Reset(  ) = 0;

	virtual HRESULT ReadXML(  ) = 0;

	virtual HRESULT WriteXML(  ) = 0;

};


}  //end of namespace DexterLib

#endif //_DEXTERLIBINTERFACES_H__

