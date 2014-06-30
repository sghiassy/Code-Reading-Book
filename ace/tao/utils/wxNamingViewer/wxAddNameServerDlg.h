// wxAddNameServerDlg.h,v 1.1 2000/08/26 23:13:05 schmidt Exp
// wxAddNameServerDlg.h

#ifndef i_wxAddNameServerDlg_h
#define i_wxAddNameServerDlg_h


class WxAddNameServerDlg:
  public wxDialog
{
public:
  WxAddNameServerDlg( wxWindow* parent);

  const wxString& getIor() const
      { return ior; }
  const wxString& getServerName() const
      { return serverName; }

private:
  wxString ior;
  wxString serverName;

  virtual bool TransferDataFromWindow();
};

#endif

