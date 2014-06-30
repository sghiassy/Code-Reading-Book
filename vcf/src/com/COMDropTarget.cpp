// COMDropTarget.cpp: implementation of the _DropTarget class.

#include "ApplicationKit.h"
#include "COMObject.h"
#include "COMDropTarget.h"

#include "COMUtils.h"

using namespace VCFCOM;


VCFCOM::DropTarget::DropTarget()
{
	m_Peer = NULL;
}

VCFCOM::DropTarget::~DropTarget()
{
	m_Peer = NULL;
}


STDMETHODIMP VCFCOM::DropTarget::DragEnter( LPDATAOBJECT dataObject, DWORD keyState, POINTL point, LPDWORD effect )
{
	HRESULT result = E_FAIL;

	m_dataObject=NULL;

    if(keyState & MK_CONTROL){
        *effect = DROPEFFECT_COPY;
	}

    m_dataObject = dataObject;

	if ( NULL != m_dataObject ){ 
		m_dataObject->AddRef();
	}

	if ( NULL != m_Peer ){
		VCF::DropTarget* target = m_Peer->getDropTarget();
		VCF::DragEvent event( target, NULL );
		event.setType( DROPTARGET_EVENT_ENTERED );
		target->processEvents( &event );		
	}
    
	result =  NOERROR;

	return result;
}

STDMETHODIMP VCFCOM::DropTarget::DragOver( DWORD keyState, POINTL point, LPDWORD effect )
{
	HRESULT result = NOERROR;

	if ( NULL == m_dataObject )
	{
        *effect=DROPEFFECT_NONE;
        result =  NOERROR;
	}
	
    //We can always drop; return effect flags based on keys.
	/*
    effect=DROPEFFECT_MOVE;
	*/	

    if ( keyState & MK_CONTROL ) {
	   *effect = DROPEFFECT_COPY;
	}    

	return result;
}	

STDMETHODIMP VCFCOM::DropTarget::DragLeave(void)
{
	HRESULT result = E_FAIL;
	
	if ( NULL != m_dataObject ){
		m_dataObject->Release();
		m_dataObject = NULL;
	}
	
    result = NO_ERROR;

	return result;
}

STDMETHODIMP VCFCOM::DropTarget::Drop( LPDATAOBJECT dataObject, DWORD keyState, POINTL point, LPDWORD effect )
{
	HRESULT result = E_FAIL;
	
	/*
    *pdwEffect=DROPEFFECT_NONE;
	*/
    if ( NULL != m_dataObject ){
		
		DragLeave();
		
		this->m_dataObject = dataObject;
		this->m_dataObject->AddRef();
		
		if ( NULL != m_Peer ){
			VCF::DropTarget* target = m_Peer->getDropTarget();
			unsigned long size = 0;
			
			
			IEnumFORMATETC* enumFMT = NULL;

			HRESULT result = m_dataObject->EnumFormatEtc( DATADIR_GET, &enumFMT );
			if ( (SUCCEEDED(result)) && (NULL != enumFMT) ){
				VCF::ClipboardDataObject data( NULL, size, 0 );
				
				FORMATETC fmtETC = {0};
				ULONG fetched = 0;

				enumFMT->Next( 1, &fmtETC, &fetched );

				while ( 1 == fetched ){
					data.addSupportedDataType( COMUtils::translateWin32ClipboardFormat(fmtETC.cfFormat) );	

					enumFMT->Next( 1, &fmtETC, &fetched );
				}

				VCF::DropEvent event( target, &data );
				event.setType( DROPTARGET_EVENT_DROPPED );
				target->processEvents( &event );
			}			
		}
		
		//*effect = DROPEFFECT_MOVE;
		
		if ( keyState & MK_CONTROL ){
			*effect = DROPEFFECT_COPY;
		}
		
		m_dataObject->Release();
		result = NOERROR;
	}
	else {
		result = ResultFromScode(E_FAIL);
	}
	return result;
}


void VCFCOM::DropTarget::setPeer( VCF::DropTargetPeer* Peer )
{
	this->m_Peer = Peer;
}
