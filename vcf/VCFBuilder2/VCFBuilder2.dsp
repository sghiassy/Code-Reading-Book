# Microsoft Developer Studio Project File - Name="VCFBuilder2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=VCFBuilder2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VCFBuilder2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VCFBuilder2.mak" CFG="VCFBuilder2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VCFBuilder2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "VCFBuilder2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VCFBuilder2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "$(VCF_INCLUDE)\core" /I "$(VCF_INCLUDE)\exceptions" /I "$(VCF_INCLUDE)\dragdrop" /I "$(VCF_INCLUDE)\events" /I "$(VCF_INCLUDE)\graphics" /I "$(VCF_INCLUDE)\implementer" /I "$(VCF_INCLUDE)\implementerKit" /I "$(VCF_INCLUDE)\utils" /I "$(VCF_INCLUDE)\io" /I "$(VCF_INCLUDE)\com" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "NO_MFC" /D "FRAMEWORK_DLL" /D "GRAPHICSKIT_DLL" /D "APPKIT_DLL" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ApplicationKit.lib GraphicsKit.lib FoundationKit.lib rpcrt4.lib VCFBuilderUI.lib /nologo /entry:"mainCRTStartup" /subsystem:windows /machine:I386 /out:".\Bin/VCFBuilder2.exe" /libpath:"$(VCF_LIB)" /libpath:".\Packages"

!ELSEIF  "$(CFG)" == "VCFBuilder2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Bin"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "$(VCF_INCLUDE)\core" /I "$(VCF_INCLUDE)\exceptions" /I "$(VCF_INCLUDE)\dragdrop" /I "$(VCF_INCLUDE)\events" /I "$(VCF_INCLUDE)\graphics" /I "$(VCF_INCLUDE)\implementer" /I "$(VCF_INCLUDE)\implementerKit" /I "$(VCF_INCLUDE)\utils" /I "$(VCF_INCLUDE)\io" /I "$(VCF_INCLUDE)\com" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "NO_MFC" /D "FRAMEWORK_DLL" /D "GRAPHICSKIT_DLL" /D "APPKIT_DLL" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ApplicationKit_d.lib GraphicsKit_d.lib FoundationKit_d.lib rpcrt4.lib VCFBuilderUI_d.lib /nologo /entry:"mainCRTStartup" /subsystem:windows /profile /debug /machine:I386 /libpath:"$(VCF_LIB)" /libpath:".\Packages"

!ENDIF 

# Begin Target

# Name "VCFBuilder2 - Win32 Release"
# Name "VCFBuilder2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\VCFBuilder2.cpp
# End Source File
# Begin Source File

SOURCE=.\VCFBuilderApp.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\VCFBuilderApp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\defaultComponent.bmp
# End Source File
# Begin Source File

SOURCE=.\res\pointer.bmp
# End Source File
# Begin Source File

SOURCE=.\res\popup.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Bin\ObjectRepository\project1.vcp
# End Source File
# Begin Source File

SOURCE=.\workspace.project.format.xml
# End Source File
# End Target
# End Project
