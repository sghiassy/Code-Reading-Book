//AppKitPeerConfig.h
#ifndef _APPKITPERCONFIG_H__
#define _APPKITPERCONFIG_H__

#ifdef WIN32
#include "Win32Peer.h"
//COM stuff
#include "VCFCOM.h"
#include "COMObject.h"
#include "EnumObject.h"
#include "COMDataObject.h"
#include "COMDropTarget.h"

//controls & GDI stuff
#include <Commdlg.h>

#include "Win32ToolKit.h"
#include "Win32Object.h"
#include "AbstractWin32Component.h"
#include "Win32ResourceBundle.h"
#include "Win32ResourceStream.h"
#include "Win32Component.h"
#include "Win32Window.h"
#include "Win32Context.h"
#include "Win32Image.h"
#include "Win32Application.h"
#include "Win32Edit.h"
#include "Win32Tree.h"
#include "Win32Listview.h"
#include "Win32Dialog.h"
#include "Win32Button.h"
#include "Win32MenuItem.h"
#include "Win32MenuBar.h"
#include "Win32FileOpenDialog.h"
#include "Win32FileSaveDialog.h"
#include "Win32ColorDialog.h"
#include "Win32DropTargetPeer.h"
#include "Win32ControlContext.h"
#include "Win32PopupMenu.h"
#include "Win32FolderBrowseDialog.h"
#include "Win32FontDialog.h"
#include "Win32Desktop.h"
#include "Win32ScrollPeer.h"
#include "Win32CursorPeer.h"
#include "Win32ToolKit.h"
#endif

#endif //_APPKITPERCONFIG_H__