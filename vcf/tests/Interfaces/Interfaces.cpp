////Interfaces.cpp

#include "FoundationKit.h"
#include "Win32Peer.h"
#include <iostream>

using namespace std;
using namespace VCF;

#define TestInterface_ID		"949788F6-B429-4b04-9BC4-C5E678AADD08"




class TestInterface {
public:
	BEGIN_INTERFACEINFO(TestInterface,"TestInterface","Interface",TestInterface_ID)
	INTERFACE_METHOD1_VOID("printMe",TestInterface,printMe,const String&,TestInterface::printMe,"s");
	INTERFACE_METHOD1_VOID("doinkMyBigFatMamaBaby",TestInterface,doinkMyBigFatMamaBaby,int,TestInterface::doinkMyBigFatMamaBaby,"i");
	INTERFACE_METHOD1_VOID("doinkMyBigFatMamaBaby2",TestInterface,doinkMyBigFatMamaBaby2,bool,TestInterface::doinkMyBigFatMamaBaby2,"b");
	INTERFACE_METHOD1_VOID("doinkMyBigFatMamaBaby3",TestInterface,doinkMyBigFatMamaBaby3,unsigned long,TestInterface::doinkMyBigFatMamaBaby3,"+l");
	END_INTERFACEINFO(TestInterface)

	virtual ~TestInterface(){};

	virtual void printMe( const String& s ) = 0;

	virtual void doinkMyBigFatMamaBaby( int i ) = 0;
	
	virtual void doinkMyBigFatMamaBaby2( bool i ) = 0;

	virtual void doinkMyBigFatMamaBaby3( unsigned long dfg ) = 0;

};



#define IScrewYou_ID		"F6CB43FA-96C7-46d6-BC8C-63097E704FA2"
class IScrewYou {
public:
	BEGIN_INTERFACEINFO(IScrewYou,"IScrewYou","Interface",IScrewYou_ID)
	INTERFACE_METHOD1_VOID("TuffShit",IScrewYou,TuffShit,bool,IScrewYou::TuffShit,"b");
	INTERFACE_METHOD_RETURN("ReallyTuffShit",IScrewYou,ReallyTuffShit,long,IScrewYou::ReallyTuffShit);
	INTERFACE_METHOD3_RETURN("Query",IScrewYou,Query,bool, Object*,int,String,IScrewYou::Query,"ois");
	END_INTERFACEINFO(IScrewYou)

	virtual ~IScrewYou(){};

	virtual void TuffShit( bool truly ) = 0;

	virtual long ReallyTuffShit() = 0;

	virtual bool Query( Object* o, int param, String s ) = 0;
};

#define Test_ID		"131E4860-DD3B-4392-B5CD-3466953306C8"

class Test : public Object, public TestInterface, public IScrewYou {
public:
	BEGIN_CLASSINFO( Test, "Test", "VCF::Object", Test_ID )
	IMPLEMENTS_INTERFACE(Test,TestInterface,"Test","TestInterface",TestInterface_ID,"Interface")
	IMPLEMENTS_INTERFACE(Test,IScrewYou,"Test","IScrewYou",IScrewYou_ID,"Interface")
	END_CLASSINFO(Test)

	Test(){};

	virtual ~Test(){};

	virtual void printMe( const String& s ) {
		cout << "Test::printMe( " << s << " )" << endl;
	}

	virtual void doinkMyBigFatMamaBaby( int i ){
		cout << "Test::doinkMyBigFatMamaBaby("<<i<<")"<<endl;
	}
	
	virtual void doinkMyBigFatMamaBaby2( bool i ){
		cout << "Test::doinkMyBigFatMamaBaby2("<<i<<")"<<endl;
	}

	virtual void doinkMyBigFatMamaBaby3( unsigned long dfg ) {
		cout << "Test::doinkMyBigFatMamaBaby3("<<dfg<<")"<<endl;
	}

	virtual void TuffShit( bool truly )	{
		cout << "Test::TuffShit("<<truly<<")"<<endl;
	}

	virtual long ReallyTuffShit() {
		cout << "Test::ReallyTuffShit()"<<endl;
		return 34;
	}

	virtual bool Query( Object* o, int param, String s ) {
		printf( "Test::Query(%p,%d,%s)\n", o, param, s.c_str() );
		return o != NULL;
	}
};


class Foo : public Object {
public:
	Foo(){
		m_doofusVal = -1;
	};
	virtual ~Foo(){};

	int m_doofusVal;
};


int main( int argc, char** argv ){

	initFoundationKit();

	REGISTER_INTERFACEINFO( TestInterface );
	REGISTER_INTERFACEINFO( IScrewYou );

	REGISTER_CLASSINFO( Test );

	ClassRegistry* cr = ClassRegistry::getClassRegistry();
	Enumerator<InterfaceClass*>* interfaces = cr->getInterfaces();
	while ( true == interfaces->hasMoreElements() ) {
		InterfaceClass* intrFace = interfaces->nextElement();
		cout << "interface: \"" << intrFace->getInterfaceName() << "\", UUID: {" << intrFace->getID() << "}" << endl;
		Enumerator<Method*>* methods = intrFace->getMethods();
		while ( true == methods->hasMoreElements() ) {
			Method* method = methods->nextElement();
			cout << "\tmethod \"" << method->getName() << "\" has " << method->getArgCount() << " arguments" << endl;
		}
	}

	Enumerator<Class*>* classes = cr->getClasses();
	while ( true == classes->hasMoreElements() ) {
		Class* clazz = classes->nextElement();
		cout << "class: \"" << clazz->getClassName() << "\", UUID: {" << clazz->getID() << "}" << endl;
		Enumerator<Method*>* methods = clazz->getMethods();
		while ( true == methods->hasMoreElements() ) {
			Method* method = methods->nextElement();
			cout << "\tmethod \"" << method->getName() << "\" has " << method->getArgCount() << " arguments" << endl;
		}
		
		Enumerator<InterfaceClass*>* classInterfaces = clazz->getInterfaces();
		while ( true == classInterfaces->hasMoreElements() ) {
			InterfaceClass* intrFace = classInterfaces->nextElement();
			cout << "class " << clazz->getClassName() << " implements interface: \"" << intrFace->getInterfaceName() << "\", UUID: {" << intrFace->getID() << "}" << endl;
			Enumerator<Method*>* methods = intrFace->getMethods();
			while ( true == methods->hasMoreElements() ) {
				Method* method = methods->nextElement();
				cout << "\tmethod \"" << method->getName() << "\" has " << method->getArgCount() << " arguments" << endl;
			}
		}
	}

	Test t;

	Class* clazz = t.getClass();
	InterfaceClass* testIFace = clazz->getInterfaceByID( TestInterface_ID );

	LARGE_INTEGER   m_sStart, m_sStop, m_liFreq; 
	QueryPerformanceFrequency(&m_liFreq); 

	
	String inputStr;
	
	while ( inputStr != "n" ) {
		
		QueryPerformanceCounter(&m_sStart); 
		t.printMe( "Far out" );
		QueryPerformanceCounter(&m_sStop); 
		double printMeDirect = ((double)( m_sStop.LowPart - m_sStart.LowPart))/(double)m_liFreq.LowPart;
		
		QueryPerformanceCounter(&m_sStart); 
		String ss = "Wow, just invoked via Reflection !";
		QueryPerformanceCounter(&m_sStop); 
		double stringDirectAssignment = ((double)( m_sStop.LowPart - m_sStart.LowPart))/(double)m_liFreq.LowPart;
		
		
		QueryPerformanceCounter(&m_sStart); 
		VariantData* s = new VariantData();
		(*s) = "Wow, just invoked via Reflection !";
		QueryPerformanceCounter(&m_sStop); 
		double stringVariantDataAssignment = ((double)( m_sStop.LowPart - m_sStart.LowPart))/(double)m_liFreq.LowPart;
		
		QueryPerformanceCounter(&m_sStart); 
		testIFace->getMethod( "printMe" )->invoke( &s );
		QueryPerformanceCounter(&m_sStop); 
		double printMeInvoke1 = ((double)( m_sStop.LowPart - m_sStart.LowPart))/(double)m_liFreq.LowPart;
		
		
		Method* meth = testIFace->getMethod( "printMe" );
		QueryPerformanceCounter(&m_sStart); 
		meth->invoke( &s );
		QueryPerformanceCounter(&m_sStop); 
		double printMeInvoke2 = ((double)( m_sStop.LowPart - m_sStart.LowPart))/(double)m_liFreq.LowPart;
		
		delete s;

		cout << "/////////////////////////////" << endl;
		cout << "STATS:" << endl;
		printf( "Test::printMe() method call took \t\t\t\t%.8f ms\n", printMeDirect );
		printf( "Direct string ss assignment took \t\t\t\t%.8f ms\n", stringDirectAssignment );
		printf( "VariantData string assignment took \t\t\t\t%.8f ms\n", stringVariantDataAssignment );
		printf( "Test::printMe() method call via reflection + l.u. took \t\t%.8f ms\n", printMeInvoke1 );
		printf( "Test::printMe() method call via reflection took \t\t%.8f ms\n", printMeInvoke2 );
	
		cout << "////////////////////////////\nTest again ? (y/n): ";
		cin >> inputStr;
	}

	VariantData* val = new VariantData();
	
	(*val) = (unsigned long)123;

	testIFace->getMethod( "doinkMyBigFatMamaBaby3" )->invoke(&val);

	InterfaceClass* screwYou = clazz->getInterfaceByID(IScrewYou_ID);

	(*val) = false;

	screwYou->getMethod( "TuffShit" )->invoke(&val);;
	
	VariantData* retVal = screwYou->getMethod( "ReallyTuffShit" )->invoke(NULL); 

	cout << "screwYou->getMethod( \"ReallyTuffShit\" ) returned " << retVal->toString() << endl;

	ImplementedInterfaceClass* implIFace = cr->getImplementedInterface( "Test", IScrewYou_ID );
	Object* o;
	implIFace->createImplementingInstance( &o );

	//int  arr[] = {1,2,3};

	VariantData* args[] = {new VariantData(&t),new VariantData(34554),new VariantData(String("sdfdl;k"))};
	
	
	retVal = screwYou->getMethod( "Query" )->invoke(args); 

	delete val;


	CoInitialize( 0 );

	//
	CLSID StupidJunkCLSID;
	HRESULT hr = CLSIDFromProgID( L"Stupid.Junk", &StupidJunkCLSID );
	if ( SUCCEEDED(hr) ){
		IUnknown* pIUnknown = NULL;
		hr = CoCreateInstance( StupidJunkCLSID, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, 
		                          (void**) &pIUnknown );

		if ( SUCCEEDED(hr) ) {
			IDispatch* pDisp = NULL;
			hr = pIUnknown->QueryInterface( IID_IDispatch, (void**)&pDisp ); 
			if ( SUCCEEDED(hr) ) {

				VARIANTARG res;
				VariantInit( &res );

				DISPPARAMS dispatchParams = {0};
				dispatchParams.cArgs = 0;
				dispatchParams.cNamedArgs = 0;
				DISPID dispatchID = 0;

				LPOLESTR methName = L"Crap";
				
				QueryPerformanceCounter(&m_sStart); 

				hr = pDisp->GetIDsOfNames( IID_NULL, &methName, 1, LOCALE_USER_DEFAULT, &dispatchID );
				if ( SUCCEEDED(hr) ) {
					pDisp->Invoke( dispatchID, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD,
									&dispatchParams, &res, 0, 0);
				}
				QueryPerformanceCounter(&m_sStop); 
				double comMethInvoke = ((double)( m_sStop.LowPart - m_sStart.LowPart))/(double)m_liFreq.LowPart;

				printf( "COM IDispath::Invoke() took %.8f ms\n", comMethInvoke );

				pDisp->Release();
			}
			pIUnknown->Release();
		}
	}
	CoUninitialize();

	Rect* r = Rect::create( 12,34,56,67 );
	r->free();

	return 0;
}










