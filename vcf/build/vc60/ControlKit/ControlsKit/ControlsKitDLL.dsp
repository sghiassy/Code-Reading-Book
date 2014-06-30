# Microsoft Developer Studio Project File - Name="ControlsKitDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ControlsKitDLL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ControlsKitDLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ControlsKitDLL.mak" CFG="ControlsKitDLL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ControlsKitDLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ControlsKitDLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ControlsKitDLL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CONTROLSKITDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CONTROLSKITDLL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "ControlsKitDLL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CONTROLSKITDLL_EXPORTS" /YX /FD /GZ  /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CONTROLSKITDLL_EXPORTS" /YX /FD /GZ  /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ControlsKitDLL - Win32 Release"
# Name "ControlsKitDLL - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\..\src\core\CheckBoxControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\CommandButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\DefaultListItem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\DefaultListModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\DefaultTableCellItem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\DefaultTableModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\DefaultTabModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\DefaultTabPage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\DefaultTextModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\DefaultTreeItem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\DefaultTreeModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\ImageControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\Label.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\ListBoxControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\ListViewControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\MultilineTextControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\OpenGLControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\Panel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\Splitter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\TabbedPages.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\TableControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\TextControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\Toolbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\ToolbarButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\ToolbarDock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\ToolbarSeparator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\TreeControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\core\TreeListControl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\..\include\core\CheckBoxControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\ComboBoxControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\events\ComboBoxListener.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\CommandButton.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\ControlsKit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\DefaultListModel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\DefaultTableCellItem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\DefaultTableModel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\DefaultTabModel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\DefaultTabPage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\DefaultTextModel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\DefaultTreeItem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\DefaultTreeModel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\HeaderControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\ImageControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\ListViewControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\MultilineTextControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\OpenGLControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\Panel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\RadioButtonControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\ScrollBarControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\Splitter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\TabbedPages.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\TableControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\TextControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\Toolbar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\ToolbarButton.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\ToolbarDock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\ToolbarSeparator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\TreeControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\core\TreeListControl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
