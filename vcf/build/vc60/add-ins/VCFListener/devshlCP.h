#ifndef _DEVSHLCP_H_
#define _DEVSHLCP_H_

#import "D:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin\devshl.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids	//"Import typelib"
template <class T>
class CProxyIApplicationEvents : public IConnectionPointImpl<T, &IID_IApplicationEvents, CComDynamicUnkArray>
{
	//Warning this class may be recreated by the wizard.
public:
	HRESULT Fire_BeforeBuildStart()
	{
		HRESULT ret;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IApplicationEvents* pIApplicationEvents = reinterpret_cast<IApplicationEvents*>(sp.p);
			if (pIApplicationEvents != NULL)
				ret = pIApplicationEvents->BeforeBuildStart();
		}	return ret;
	
	}
	HRESULT Fire_BuildFinish(LONG nNumErrors, LONG nNumWarnings)
	{
		HRESULT ret;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IApplicationEvents* pIApplicationEvents = reinterpret_cast<IApplicationEvents*>(sp.p);
			if (pIApplicationEvents != NULL)
				ret = pIApplicationEvents->BuildFinish(nNumErrors, nNumWarnings);
		}	return ret;
	
	}
	HRESULT Fire_BeforeApplicationShutDown()
	{
		HRESULT ret;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IApplicationEvents* pIApplicationEvents = reinterpret_cast<IApplicationEvents*>(sp.p);
			if (pIApplicationEvents != NULL)
				ret = pIApplicationEvents->BeforeApplicationShutDown();
		}	return ret;
	
	}
	HRESULT Fire_DocumentOpen(IDispatch * theDocument)
	{
		HRESULT ret;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IApplicationEvents* pIApplicationEvents = reinterpret_cast<IApplicationEvents*>(sp.p);
			if (pIApplicationEvents != NULL)
				ret = pIApplicationEvents->DocumentOpen(theDocument);
		}	return ret;
	
	}
	HRESULT Fire_BeforeDocumentClose(IDispatch * theDocument)
	{
		HRESULT ret;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IApplicationEvents* pIApplicationEvents = reinterpret_cast<IApplicationEvents*>(sp.p);
			if (pIApplicationEvents != NULL)
				ret = pIApplicationEvents->BeforeDocumentClose(theDocument);
		}	return ret;
	
	}
	HRESULT Fire_DocumentSave(IDispatch * theDocument)
	{
		HRESULT ret;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IApplicationEvents* pIApplicationEvents = reinterpret_cast<IApplicationEvents*>(sp.p);
			if (pIApplicationEvents != NULL)
				ret = pIApplicationEvents->DocumentSave(theDocument);
		}	return ret;
	
	}
	HRESULT Fire_NewDocument(IDispatch * theDocument)
	{
		HRESULT ret;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IApplicationEvents* pIApplicationEvents = reinterpret_cast<IApplicationEvents*>(sp.p);
			if (pIApplicationEvents != NULL)
				ret = pIApplicationEvents->NewDocument(theDocument);
		}	return ret;
	
	}
	HRESULT Fire_WindowActivate(IDispatch * theWindow)
	{
		HRESULT ret;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IApplicationEvents* pIApplicationEvents = reinterpret_cast<IApplicationEvents*>(sp.p);
			if (pIApplicationEvents != NULL)
				ret = pIApplicationEvents->WindowActivate(theWindow);
		}	return ret;
	
	}
	HRESULT Fire_WindowDeactivate(IDispatch * theWindow)
	{
		HRESULT ret;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IApplicationEvents* pIApplicationEvents = reinterpret_cast<IApplicationEvents*>(sp.p);
			if (pIApplicationEvents != NULL)
				ret = pIApplicationEvents->WindowDeactivate(theWindow);
		}	return ret;
	
	}
	HRESULT Fire_WorkspaceOpen()
	{
		HRESULT ret;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IApplicationEvents* pIApplicationEvents = reinterpret_cast<IApplicationEvents*>(sp.p);
			if (pIApplicationEvents != NULL)
				ret = pIApplicationEvents->WorkspaceOpen();
		}	return ret;
	
	}
	HRESULT Fire_WorkspaceClose()
	{
		HRESULT ret;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IApplicationEvents* pIApplicationEvents = reinterpret_cast<IApplicationEvents*>(sp.p);
			if (pIApplicationEvents != NULL)
				ret = pIApplicationEvents->WorkspaceClose();
		}	return ret;
	
	}
	HRESULT Fire_NewWorkspace()
	{
		HRESULT ret;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IApplicationEvents* pIApplicationEvents = reinterpret_cast<IApplicationEvents*>(sp.p);
			if (pIApplicationEvents != NULL)
				ret = pIApplicationEvents->NewWorkspace();
		}	return ret;
	
	}
};
#endif