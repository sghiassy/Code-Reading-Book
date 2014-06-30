# Microsoft Developer Studio Project File - Name="VCFBuilderUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=VCFBuilderUI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VCFBuilderUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VCFBuilderUI.mak" CFG="VCFBuilderUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VCFBuilderUI - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "VCFBuilderUI - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VCFBuilderUI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "$(VCF_INCLUDE)\core" /I "$(VCF_INCLUDE)\exceptions" /I "$(VCF_INCLUDE)\dragdrop" /I "$(VCF_INCLUDE)\events" /I "$(VCF_INCLUDE)\graphics" /I "$(VCF_INCLUDE)\implementer" /I "$(VCF_INCLUDE)\implementerKit" /I "$(VCF_INCLUDE)\utils" /I "$(VCF_INCLUDE)\io" /I "..\..\\" /I "..\TextEditor" /I "$(VCF_INCLUDE)\com" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "NO_MFC" /D "_USRDLL" /D "VCFBUILDERUI_EXPORTS" /D "FRAMEWORK_DLL" /D "GRAPHICSKIT_DLL" /D "APPKIT_DLL" /FD /Zm150 /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FoundationKit.lib GraphicsKit.lib ApplicationKit.lib comctl32.lib rpcrt4.lib opengl32.lib glaux.lib glu32.lib TextEditor.lib CPPParser.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\bin\VCFBuilderUI.dll" /libpath:"$(VCF_LIB)" /libpath:"..\\"
# SUBTRACT LINK32 /incremental:yes /debug

!ELSEIF  "$(CFG)" == "VCFBuilderUI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "$(VCF_INCLUDE)\core" /I "$(VCF_INCLUDE)\exceptions" /I "$(VCF_INCLUDE)\dragdrop" /I "$(VCF_INCLUDE)\events" /I "$(VCF_INCLUDE)\graphics" /I "$(VCF_INCLUDE)\implementer" /I "$(VCF_INCLUDE)\implementerKit" /I "$(VCF_INCLUDE)\utils" /I "$(VCF_INCLUDE)\io" /I "..\..\\" /I "..\TextEditor" /I "$(VCF_INCLUDE)\com" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "NO_MFC" /D "_USRDLL" /D "VCFBUILDERUI_EXPORTS" /D "FRAMEWORK_DLL" /D "GRAPHICSKIT_DLL" /D "APPKIT_DLL" /FD /GZ /Zm150 /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FoundationKit_d.lib GraphicsKit_d.lib ApplicationKit_d.lib comctl32.lib rpcrt4.lib opengl32.lib glaux.lib glu32.lib TextEditor_d.lib CPPParser_d.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /out:"..\..\bin\VCFBuilderUI_d.dll" /libpath:"$(VCF_LIB)" /libpath:"..\\"

!ENDIF 

# Begin Target

# Name "VCFBuilderUI - Win32 Release"
# Name "VCFBuilderUI - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\ComponentInstance.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ComponentNameManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\res\components.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\CPPClass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CPPClassInstance.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CPPCodeGenerator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DefaultComponentEditor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DefaultUIProject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\EventCellItem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\EventTableModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Form.cpp
# End Source File
# Begin Source File

SOURCE=..\..\FormGrid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\FormView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\GenericClass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\GridRuler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\GridRulerBtn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\MainUIPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\MainWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\NewComponentCmd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\NewProjectCmd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\NewProjectDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ObjectExplorer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Project.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ProjectObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ProjectTreeModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\PropertyCellItem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\PropertyCellItemEditor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\PropertyChangedCmd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\PropertyTableModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\RemoveComponentCmd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SelectionGripper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SelectionGripperHandle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SelectionManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SingleSelectionGripper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StatusBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\TranslateControlCmd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\UIForm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\VCFBuilder2.rc
# ADD BASE RSC /l 0x409 /i "\code\vcf\VCFBuilder2"
# ADD RSC /l 0x409 /i "\code\vcf\VCFBuilder2" /i "..\..\res"
# End Source File
# Begin Source File

SOURCE=..\..\VCFBuilderUI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\WebForm.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\CodeGenerator.h
# End Source File
# Begin Source File

SOURCE=..\..\ComponentInstance.h
# End Source File
# Begin Source File

SOURCE=..\..\ComponentNameManager.h
# End Source File
# Begin Source File

SOURCE=..\..\CPPClass.h
# End Source File
# Begin Source File

SOURCE=..\..\CPPClassInstance.h
# End Source File
# Begin Source File

SOURCE=..\..\CPPCodeGenerator.h
# End Source File
# Begin Source File

SOURCE=..\..\DefaultComponentEditor.h
# End Source File
# Begin Source File

SOURCE=..\..\DefaultUIProject.h
# End Source File
# Begin Source File

SOURCE=..\..\EventCellItem.h
# End Source File
# Begin Source File

SOURCE=..\..\EventTableModel.h
# End Source File
# Begin Source File

SOURCE=..\..\Form.h
# End Source File
# Begin Source File

SOURCE=..\..\FormGrid.h
# End Source File
# Begin Source File

SOURCE=..\..\FormView.h
# End Source File
# Begin Source File

SOURCE=..\..\GenericClass.h
# End Source File
# Begin Source File

SOURCE=..\..\GridRuler.h
# End Source File
# Begin Source File

SOURCE=..\..\GridRulerBtn.h
# End Source File
# Begin Source File

SOURCE=..\..\MainUIPanel.h
# End Source File
# Begin Source File

SOURCE=..\..\MainWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\NewComponentCmd.h
# End Source File
# Begin Source File

SOURCE=..\..\NewProjectCmd.h
# End Source File
# Begin Source File

SOURCE=..\..\NewProjectDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\ObjectExplorer.h
# End Source File
# Begin Source File

SOURCE=..\..\Project.h
# End Source File
# Begin Source File

SOURCE=..\..\ProjectObject.h
# End Source File
# Begin Source File

SOURCE=..\..\ProjectObjectTraverser.h
# End Source File
# Begin Source File

SOURCE=..\..\ProjectTreeModel.h
# End Source File
# Begin Source File

SOURCE=..\..\PropertyCellItem.h
# End Source File
# Begin Source File

SOURCE=..\..\PropertyCellItemEditor.h
# End Source File
# Begin Source File

SOURCE=..\..\PropertyChangedCmd.h
# End Source File
# Begin Source File

SOURCE=..\..\PropertyTableModel.h
# End Source File
# Begin Source File

SOURCE=..\..\RemoveComponentCmd.h
# End Source File
# Begin Source File

SOURCE=..\..\SelectionGripper.h
# End Source File
# Begin Source File

SOURCE=..\..\SelectionGripperHandle.h
# End Source File
# Begin Source File

SOURCE=..\..\SelectionManager.h
# End Source File
# Begin Source File

SOURCE=..\..\SingleSelectionGripper.h
# End Source File
# Begin Source File

SOURCE=..\..\StatusBar.h
# End Source File
# Begin Source File

SOURCE=..\..\TranslateControlCmd.h
# End Source File
# Begin Source File

SOURCE=..\..\UIForm.h
# End Source File
# Begin Source File

SOURCE=..\..\VCFBuilderUI.h
# End Source File
# Begin Source File

SOURCE=..\..\VCFBuilderUIConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\WebForm.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
