// wxBindNewContext.cpp,v 1.1 2000/08/26 23:13:05 schmidt Exp
// wxBindNewContext.cpp


#include "pch.h"
#include "wxBindNewContext.h"


WxBindNewContext::WxBindNewContext( wxWindow* parent):
  wxDialog()
{
  LoadFromResource( parent, "bindNewContext");
}


bool WxBindNewContext::TransferDataFromWindow()
{
  name.length(1);
  wxTextCtrl* ctrl = static_cast<wxTextCtrl*>( wxFindWindowByName(
      "idText",
      this));
  assert( ctrl);
  name[0].id = CORBA::string_dup( ctrl->GetValue());

  ctrl = static_cast<wxTextCtrl*>( wxFindWindowByName(
      "kindText",
      this));
  assert( ctrl);
  name[0].kind = CORBA::string_dup( ctrl->GetValue());

  return true;
}

