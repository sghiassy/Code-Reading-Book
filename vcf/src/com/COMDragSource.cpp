//COMDragSource.cpp

#include "COMDragSource.h"

using namespace ImprovCOM;

/*
bool DragSource::supportsInterface( REFIID riid )
{
	return COMObject::supportsInterface( riid ) || ( IID_IDropSource == riid );
}
*/

STDMETHODIMP DragSource::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
{
	HRESULT result = NO_ERROR;
	if ( true == fEscapePressed ){
		result = ResultFromScode(DRAGDROP_S_CANCEL);
	}

	if ( true != (grfKeyState & MK_LBUTTON) ){
		result = ResultFromScode( DRAGDROP_S_DROP );
	}

	return result;
}

STDMETHODIMP DragSource::GiveFeedback( DWORD dwEffect )
{
	return ResultFromScode( DRAGDROP_S_USEDEFAULTCURSORS );
}