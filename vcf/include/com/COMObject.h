

#ifndef COMOBJECT_H
#define COMOBJECT_H




#ifndef WIN32

class IUnknown{

};

typedef unsigned long REFIID;

typedef unsigned long HRESULT ;

typedef unsigned long ULONG;

#endif

#ifndef PPVOID
typedef LPVOID * PPVOID;
#endif  //PPVOID



namespace VCFCOM {
/**
 * base class for IUnknown implementation.
 *  Follows the implementation standards described in "Inside OLE", 2nd edition, by Craig Brockschmidt. 
 */
class COMObject;

/**
*helper macro used to properly typecast a interface object into IUnknown for passing
*as a parameter
*/
#define AS_IUNKNOWN(Object) (IUnknown*)(COMObject*)Object

/**
*Helper macro for implementing QueryInterface. Unfortunately, due to the rules of COM, 
*you can't just let inheritance handle the calls for you in your COMObject derived class.
*These macros are here to simplify the typing involved, since the code will always be the same. 
*In the future this could be replaced by templates, possibly.
*/
#define COMOBJ_QUERY_INTERFACE_IMPL \
	STDMETHODIMP QueryInterface(REFIID riid, PPVOID ppv){ return COMObject::QueryInterface(riid, ppv);}; \

/**
*helper macro for implementing AddRef
*/
#define COMOBJ_ADD_REF_IMPL \
	STDMETHODIMP_(ULONG) AddRef(void){ return COMObject::AddRef();}; \


/**
*helper macro for implementing Release
*/
#define COMOBJ_RELEASE_IMPL \
	STDMETHODIMP_(ULONG) Release(void){ return COMObject::Release();}; \



class APPKIT_API COMObject : public IUnknown, public VCF::Object {
public:    
	COMObject();
	virtual ~COMObject();
	/**
	*This method is used to determine whether or not the class supports 
	*the interface requested by the riid parameter. If the class does support the 
	*interface then the class must observe the following rules in casting itself into the 
	*object parameter:
	*	The requested interface type first, followed by the base class for COM objects, followed
	*	by the "this" pointer. For example:
	*		<p><pre>			
	*			*object = (IDataObject*)(COMObject*)this;
	*		</pre></p>
	*
	*Further the class must declared as follows:
	*	Class name first, folowed by the public COM interfaces the class is supporting, followed by the 
	*	base COM Object class, and then any other framework interfaces that are being implemented.
	*	For example:
	*		<p><pre>
	*			class MyConnectionPoint : public IConnectionPoint, public COMObject {};
	*		</pre></p>
	*
	*	This ensures that the vtables C++ constructs, due to multiple inheritance, are done so correctly.
	*/
    virtual bool supportsInterface( REFIID riid, void** object )=0;

	//COM CALLS
    STDMETHODIMP QueryInterface(REFIID riid, PPVOID ppv);
	STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);
protected:
    ULONG m_ref;
};

}
#endif //COMOBJECT_H