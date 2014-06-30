//FilgraphManager.cpp

#include "QuartzTypeLib.h"
#include "QuartzTypeLibInterfaces.h"
#include "FilgraphManager.h"

using namespace VCF;
using namespace QuartzTypeLib;

FilgraphManager::FilgraphManager()
{

}

FilgraphManager::~FilgraphManager()
{

}

void FilgraphManager::Run(  )
{

}

void FilgraphManager::Pause(  )
{

}

void FilgraphManager::Stop(  )
{

}

void FilgraphManager::GetState( long msTimeout, long* pfs )
{

}

void FilgraphManager::RenderFile( BSTR strFilename )
{

}

void FilgraphManager::AddSourceFilter( BSTR strFilename, IDispatch** ppUnk )
{

}

IDispatch* FilgraphManager::getFilterCollection(  )
{
	IDispatch* result;

	return result;
}

IDispatch* FilgraphManager::getRegFilterCollection(  )
{
	IDispatch* result;

	return result;
}

void FilgraphManager::StopWhenReady(  )
{

}

void FilgraphManager::GetEventHandle( LONG_PTR* hEvent )
{

}

void FilgraphManager::GetEvent( long* lEventCode, LONG_PTR* lParam1, LONG_PTR* lParam2, long msTimeout )
{

}

void FilgraphManager::WaitForCompletion( long msTimeout, long* pEvCode )
{

}

void FilgraphManager::CancelDefaultHandling( long lEvCode )
{

}

void FilgraphManager::RestoreDefaultHandling( long lEvCode )
{

}

void FilgraphManager::FreeEventParams( long lEvCode, LONG_PTR lParam1, LONG_PTR lParam2 )
{

}

double FilgraphManager::getDuration(  )
{
	double result;

	return result;
}

void FilgraphManager::setCurrentPosition( double Val )
{

}

double FilgraphManager::getCurrentPosition(  )
{
	double result;

	return result;
}

double FilgraphManager::getStopTime(  )
{
	double result;

	return result;
}

void FilgraphManager::setStopTime( double Val )
{

}

double FilgraphManager::getPrerollTime(  )
{
	double result;

	return result;
}

void FilgraphManager::setPrerollTime( double Val )
{

}

void FilgraphManager::setRate( double Val )
{

}

double FilgraphManager::getRate(  )
{
	double result;

	return result;
}

long FilgraphManager::CanSeekForward(  )
{

}

long FilgraphManager::CanSeekBackward(  )
{

}

void FilgraphManager::setVolume( long Val )
{

}

long FilgraphManager::getVolume(  )
{
	long result;

	return result;
}

void FilgraphManager::setBalance( long Val )
{

}

long FilgraphManager::getBalance(  )
{
	long result;

	return result;
}

double FilgraphManager::getAvgTimePerFrame(  )
{
	double result;

	return result;
}

long FilgraphManager::getBitRate(  )
{
	long result;

	return result;
}

long FilgraphManager::getBitErrorRate(  )
{
	long result;

	return result;
}

long FilgraphManager::getVideoWidth(  )
{
	long result;

	return result;
}

long FilgraphManager::getVideoHeight(  )
{
	long result;

	return result;
}

void FilgraphManager::setSourceLeft( long Val )
{

}

long FilgraphManager::getSourceLeft(  )
{
	long result;

	return result;
}

void FilgraphManager::setSourceWidth( long Val )
{

}

long FilgraphManager::getSourceWidth(  )
{
	long result;

	return result;
}

void FilgraphManager::setSourceTop( long Val )
{

}

long FilgraphManager::getSourceTop(  )
{
	long result;

	return result;
}

void FilgraphManager::setSourceHeight( long Val )
{

}

long FilgraphManager::getSourceHeight(  )
{
	long result;

	return result;
}

void FilgraphManager::setDestinationLeft( long Val )
{

}

long FilgraphManager::getDestinationLeft(  )
{
	long result;

	return result;
}

void FilgraphManager::setDestinationWidth( long Val )
{

}

long FilgraphManager::getDestinationWidth(  )
{
	long result;

	return result;
}

void FilgraphManager::setDestinationTop( long Val )
{

}

long FilgraphManager::getDestinationTop(  )
{
	long result;

	return result;
}

void FilgraphManager::setDestinationHeight( long Val )
{

}

long FilgraphManager::getDestinationHeight(  )
{
	long result;

	return result;
}

void FilgraphManager::SetSourcePosition( long Left, long Top, long Width, long Height )
{

}

void FilgraphManager::GetSourcePosition( long* pLeft, long* pTop, long* pWidth, long* pHeight )
{

}

void FilgraphManager::SetDefaultSourcePosition(  )
{

}

void FilgraphManager::SetDestinationPosition( long Left, long Top, long Width, long Height )
{

}

void FilgraphManager::GetDestinationPosition( long* pLeft, long* pTop, long* pWidth, long* pHeight )
{

}

void FilgraphManager::SetDefaultDestinationPosition(  )
{

}

void FilgraphManager::GetVideoSize( long* pWidth, long* pHeight )
{

}

void FilgraphManager::GetVideoPaletteEntries( long StartIndex, long Entries, long* pRetrieved, long* pPalette )
{

}

void FilgraphManager::GetCurrentImage( long* pBufferSize, long* pDIBImage )
{

}

void FilgraphManager::IsUsingDefaultSource(  )
{

}

void FilgraphManager::IsUsingDefaultDestination(  )
{

}

void FilgraphManager::setCaption( BSTR Val )
{

}

BSTR FilgraphManager::getCaption(  )
{
	BSTR result;

	return result;
}

void FilgraphManager::setWindowStyle( long Val )
{

}

long FilgraphManager::getWindowStyle(  )
{
	long result;

	return result;
}

void FilgraphManager::setWindowStyleEx( long Val )
{

}

long FilgraphManager::getWindowStyleEx(  )
{
	long result;

	return result;
}

void FilgraphManager::setAutoShow( long Val )
{

}

long FilgraphManager::getAutoShow(  )
{
	long result;

	return result;
}

void FilgraphManager::setWindowState( long Val )
{

}

long FilgraphManager::getWindowState(  )
{
	long result;

	return result;
}

void FilgraphManager::setBackgroundPalette( long Val )
{

}

long FilgraphManager::getBackgroundPalette(  )
{
	long result;

	return result;
}

void FilgraphManager::setVisible( long Val )
{

}

long FilgraphManager::getVisible(  )
{
	long result;

	return result;
}

void FilgraphManager::setLeft( long Val )
{

}

long FilgraphManager::getLeft(  )
{
	long result;

	return result;
}

void FilgraphManager::setWidth( long Val )
{

}

long FilgraphManager::getWidth(  )
{
	long result;

	return result;
}

void FilgraphManager::setTop( long Val )
{

}

long FilgraphManager::getTop(  )
{
	long result;

	return result;
}

void FilgraphManager::setHeight( long Val )
{

}

long FilgraphManager::getHeight(  )
{
	long result;

	return result;
}

void FilgraphManager::setOwner( LONG_PTR Val )
{

}

LONG_PTR FilgraphManager::getOwner(  )
{
	LONG_PTR result;

	return result;
}

void FilgraphManager::setMessageDrain( LONG_PTR Val )
{

}

LONG_PTR FilgraphManager::getMessageDrain(  )
{
	LONG_PTR result;

	return result;
}

long FilgraphManager::getBorderColor(  )
{
	long result;

	return result;
}

void FilgraphManager::setBorderColor( long Val )
{

}

long FilgraphManager::getFullScreenMode(  )
{
	long result;

	return result;
}

void FilgraphManager::setFullScreenMode( long Val )
{

}

void FilgraphManager::SetWindowForeground( long Focus )
{

}

void FilgraphManager::NotifyOwnerMessage( LONG_PTR hwnd, long uMsg, LONG_PTR wParam, LONG_PTR lParam )
{

}

void FilgraphManager::SetWindowPosition( long Left, long Top, long Width, long Height )
{

}

void FilgraphManager::GetWindowPosition( long* pLeft, long* pTop, long* pWidth, long* pHeight )
{

}

void FilgraphManager::GetMinIdealImageSize( long* pWidth, long* pHeight )
{

}

void FilgraphManager::GetMaxIdealImageSize( long* pWidth, long* pHeight )
{

}

void FilgraphManager::GetRestorePosition( long* pLeft, long* pTop, long* pWidth, long* pHeight )
{

}

void FilgraphManager::HideCursor( long HideCursor )
{

}

void FilgraphManager::IsCursorHidden( long* CursorHidden )
{

}

