# Microsoft Developer Studio Project File - Name="VCFBuilder" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=VCFBuilder - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VCFBuilder.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VCFBuilder.mak" CFG="VCFBuilder - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VCFBuilder - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VCFBuilder - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VCFBuilder - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "$(VCF_INCLUDE)\core" /I "$(VCF_INCLUDE)\exceptions" /I "$(VCF_INCLUDE)\dragdrop" /I "$(VCF_INCLUDE)\events" /I "$(VCF_INCLUDE)\graphics" /I "$(VCF_INCLUDE)\implementer" /I "$(VCF_INCLUDE)\implementerKit" /I "$(VCF_INCLUDE)\utils" /I "$(VCF_INCLUDE)\io" /I "..\..\..\..\VCFBuilder2" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "FRAMEWORK_DLL" /D "GRAPHICSKIT_DLL" /D "APPKIT_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /libpath:"..\..\..\..\VCFBuilder2\Packages" /libpath:"$(VCF_LIB)"

!ELSEIF  "$(CFG)" == "VCFBuilder - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GR /GX /ZI /Od /I "$(VCF_INCLUDE)\core" /I "$(VCF_INCLUDE)\exceptions" /I "$(VCF_INCLUDE)\dragdrop" /I "$(VCF_INCLUDE)\events" /I "$(VCF_INCLUDE)\graphics" /I "$(VCF_INCLUDE)\implementer" /I "$(VCF_INCLUDE)\implementerKit" /I "$(VCF_INCLUDE)\utils" /I "$(VCF_INCLUDE)\io" /I "..\..\..\..\VCFBuilder2" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "FRAMEWORK_DLL" /D "GRAPHICSKIT_DLL" /D "APPKIT_DLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ApplicationKit.lib GraphicsKit.lib FoundationKit.lib rpcrt4.lib VCFBuilderUI.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\..\VCFBuilder2\Bin\VCFBuilder.dll" /pdbtype:sept /libpath:"..\..\..\..\VCFBuilder2\Packages" /libpath:"$(VCF_LIB)"

!ENDIF 

# Begin Target

# Name "VCFBuilder - Win32 Release"
# Name "VCFBuilder - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Commands.cpp
# End Source File
# Begin Source File

SOURCE=.\DevStudioMainWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\DevStudioMDIClientWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\DSAddIn.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SubClassWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\VCFBuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\VCFBuilder.def
# End Source File
# Begin Source File

SOURCE=.\VCFBuilder.odl
# ADD MTL /h "VCFBuilderTypes.h"
# End Source File
# Begin Source File

SOURCE=.\VCFBuilder.rc
# End Source File
# Begin Source File

SOURCE=.\VCFBuilderHostView.cpp
# End Source File
# Begin Source File

SOURCE=.\VCFBuilderMDIChild.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Commands.h
# End Source File
# Begin Source File

SOURCE=.\DevStudioMainWnd.h
# End Source File
# Begin Source File

SOURCE=.\DevStudioMDIClientWnd.h
# End Source File
# Begin Source File

SOURCE=.\DSAddIn.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SubClassWnd.h
# End Source File
# Begin Source File

SOURCE=.\VCFBuilder.h
# End Source File
# Begin Source File

SOURCE=.\VCFBuilderHostView.h
# End Source File
# Begin Source File

SOURCE=.\VCFBuilderMDIChild.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\TBarLrge.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TBarMedm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\VCFBuilder.rc2
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
