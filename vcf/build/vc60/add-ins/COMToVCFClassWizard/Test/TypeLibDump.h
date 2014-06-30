//TypelibDump.h

#ifndef _TYPELIBDUMP_H__
#define _TYPELIBDUMP_H__

#include <vector>
#include <map>

class MethodHolder;
class InterfaceHolder;

class CoClassHolder {
public:
	virtual ~CoClassHolder();

	_bstr_t m_className;
	_bstr_t m_classID;
	_bstr_t m_progID;
	std::vector<InterfaceHolder*> m_implementedInterfaces;
};

class InterfaceHolder {
public:
	virtual ~InterfaceHolder();
	_bstr_t m_superInterfaceName;
	_bstr_t m_superInterfaceID;
	_bstr_t m_interfaceName;
	_bstr_t m_interfaceID;
	std::vector<MethodHolder*> m_methods;
};

class MethodHolder {
public:
	MethodHolder() {
		m_isPropertyGetter = false;		
		m_isPropertySetter = false;
		m_returnType = "";
		m_declaration = "";
		m_methodID = "";
	}


	_bstr_t m_returnType;
	_bstr_t m_declaration;	
	_bstr_t m_methodID;
	bool m_isPropertyGetter;
	bool m_isPropertySetter;	
};



class TypeLibHolder {
public:
	virtual ~TypeLibHolder();

	_bstr_t m_typeLibName;
	_bstr_t m_typeLibID;
	std::vector<CoClassHolder*> m_coClasses;
	std::map<_bstr_t,InterfaceHolder*> m_interfaces;
	_bstr_t m_typeDefs;
};

BSTR GenerateClassHeaderFromCOMCLSID( const CLSID& clsid );

BSTR GenerateClassTypeDefs( ITypeLib* pTypeLib, long indent );

BSTR GenerateInterfaceDefinition( ITypeLib* pTypeLib, const CLSID& clsid );

BSTR GenerateClassMethod( ITypeInfo* pMethodTypeInfo, long methodID );

HRESULT GenerateMethod( ITypeInfo* pMethodTypeInfo, long methodID, MethodHolder* pMethod );

HRESULT GenerateInterface( ITypeInfo* pTypeInfo, TYPEATTR* pTypeAttr, InterfaceHolder* pInterface );

BSTR TYPEDESCtoString( ITypeInfo* pti, TYPEDESC* ptdesc );

HRESULT GenerateTypeLibHolder( ITypeLib* pTypeLib, TypeLibHolder& typeLibHolder );

#endif //_TYPELIBDUMP_H__