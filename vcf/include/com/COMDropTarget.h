// COMDropTarget.h: interface for the _DropTarget class.


#ifndef _COMDROPTARGET_H__
#define _COMDROPTARGET_H__



namespace VCFCOM
{

class APPKIT_API DropTarget : public IDropTarget, public COMObject  
{
public:
	DropTarget();

	virtual ~DropTarget();	


	COMOBJ_QUERY_INTERFACE_IMPL;
	COMOBJ_ADD_REF_IMPL;
	COMOBJ_RELEASE_IMPL;

	virtual bool supportsInterface( REFIID riid, void** object ){
		bool result = 0 != (IID_IDropTarget == riid);
		if ( true == result ){	
			*object = (IDropTarget*)(COMObject*)this;
		}
		return 	result;
	};
/**
	STDMETHODIMP QueryInterface(REFIID riid, PPVOID ppv);
	STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);
*/
	STDMETHODIMP DragEnter( LPDATAOBJECT dataObject, DWORD keyState, POINTL point, LPDWORD effect );
    STDMETHODIMP DragOver( DWORD keyState, POINTL point, LPDWORD effect );
    STDMETHODIMP DragLeave(void);
    STDMETHODIMP Drop( LPDATAOBJECT dataObject, DWORD keyState, POINTL point, LPDWORD effect );
	
	void setPeer( VCF::DropTargetPeer* Peer );

private:
	IDataObject* m_dataObject;
	VCF::DropTargetPeer* m_Peer;
};


};

#endif 
