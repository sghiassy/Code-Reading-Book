// wxAddNameServerDlg.cpp,v 1.1 2000/08/26 23:13:05 schmidt Exp
// AddNameServerDlg.cpp

#include "pch.h"
#include "wxAddNameServerDlg.h"


WxAddNameServerDlg::WxAddNameServerDlg( wxWindow* parent):
  wxDialog(),
  ior(""),
  serverName("")
{
  LoadFromResource( parent, "addNameServer");

  wxButton* ctrl = static_cast<wxButton*>( wxFindWindowByName(
      "okButton",
      this));
  assert( ctrl);
  ctrl->SetDefault();
}


bool WxAddNameServerDlg::TransferDataFromWindow()
{
  wxTextCtrl* ctrl = static_cast<wxTextCtrl*>( wxFindWindowByName(
      "iorText",
      this));
  assert( ctrl);
  ior = ctrl->GetValue();

  ctrl = static_cast<wxTextCtrl*>( wxFindWindowByName(
      "nameText",
      this));
  assert( ctrl);
  serverName = ctrl->GetValue();
  return true;
}

