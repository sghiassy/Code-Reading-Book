//PropertySetter.cpp

#include "DexterLib.h"
#include "DexterLibInterfaces.h"
#include "PropertySetter.h"

using namespace VCF;
using namespace DexterLib;

PropertySetter::PropertySetter()
{

}

PropertySetter::~PropertySetter()
{

}

HRESULT PropertySetter::LoadXML( IUnknown* pxml )
{

}

HRESULT PropertySetter::PrintXML( unsigned char* pszXML, int cbXML, int* pcbPrinted, int indent )
{

}

HRESULT PropertySetter::CloneProps( IPropertySetter** ppSetter, int64 rtStart, int64 rtStop )
{

}

HRESULT PropertySetter::AddProp( DEXTER_PARAM Param, DEXTER_VALUE* paValue )
{

}

HRESULT PropertySetter::GetProps( long* pcParams, DEXTER_PARAM** paParam, DEXTER_VALUE** paValue )
{

}

HRESULT PropertySetter::FreeProps( long cParams, DEXTER_PARAM* paParam, DEXTER_VALUE* paValue )
{

}

HRESULT PropertySetter::ClearProps(  )
{

}

HRESULT PropertySetter::SaveToBlob( long* pcSize, unsigned char** ppb )
{

}

HRESULT PropertySetter::LoadFromBlob( long cSize, unsigned char* pb )
{

}

HRESULT PropertySetter::SetProps( IUnknown* pTarget, int64 rtNow )
{

}

HRESULT PropertySetter::getErrorLog( IAMErrorLog** pVal )
{
	HRESULT result;

	return result;
}

HRESULT PropertySetter::setErrorLog( IAMErrorLog* pVal )
{

}

