# Microsoft Developer Studio Project File - Name="sdfsdf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=sdfsdf - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sdfsdf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sdfsdf.mak" CFG="sdfsdf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sdfsdf - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "sdfsdf - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sdfsdf - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "$(VCF_INCLUDE)\core" /I "$(VCF_INCLUDE)\exceptions" /I "$(VCF_INCLUDE)\dragdrop" /I "$(VCF_INCLUDE)\events" /I "$(VCF_INCLUDE)\implementer" /I "$(VCF_INCLUDE)\implementerKit" /I "$(VCF_INCLUDE)\utils" /I "$(VCF_INCLUDE)\io" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "NO_MFC" /D "_CONSOLE" /D "FRAMEWORK_DLL" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FoundationKitDLL.lib rpcrt4.lib  /nologo /subsystem:console /machine:I386 /libpath:"$(VCF_LIB)"

!ELSEIF  "$(CFG)" == "sdfsdf - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ  /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "$(VCF_INCLUDE)\core" /I "$(VCF_INCLUDE)\exceptions" /I "$(VCF_INCLUDE)\dragdrop" /I "$(VCF_INCLUDE)\events" /I "$(VCF_INCLUDE)\implementer" /I "$(VCF_INCLUDE)\implementerKit" /I "$(VCF_INCLUDE)\utils" /I "$(VCF_INCLUDE)\io" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "NO_MFC" /D "_CONSOLE" /D "FRAMEWORK_DLL" /FD /GZ   /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FoundationKitDLL_d.lib rpcrt4.lib  /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"$(VCF_LIB)"

!ENDIF 

# Begin Target

# Name "sdfsdf - Win32 Release"
# Name "sdfsdf - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AMTimeline.cpp
# End Source File
# Begin Source File

SOURCE=.\AMTimelineComp.cpp
# End Source File
# Begin Source File

SOURCE=.\AMTimelineEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\AMTimelineGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\AMTimelineObj.cpp
# End Source File
# Begin Source File

SOURCE=.\AMTimelineSrc.cpp
# End Source File
# Begin Source File

SOURCE=.\AMTimelineTrack.cpp
# End Source File
# Begin Source File

SOURCE=.\AMTimelineTrans.cpp
# End Source File
# Begin Source File

SOURCE=.\AudMixer.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorSource.cpp
# End Source File
# Begin Source File

SOURCE=.\DexterLib.cpp
# End Source File
# Begin Source File

SOURCE=.\DxtAlphaSetter.cpp
# End Source File
# Begin Source File

SOURCE=.\DxtCompositor.cpp
# End Source File
# Begin Source File

SOURCE=.\DxtJpeg.cpp
# End Source File
# Begin Source File

SOURCE=.\DxtKey.cpp
# End Source File
# Begin Source File

SOURCE=.\MediaDet.cpp
# End Source File
# Begin Source File

SOURCE=.\MediaLocator.cpp
# End Source File
# Begin Source File

SOURCE=.\NullRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertySetter.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\SampleGrabber.cpp
# End Source File
# Begin Source File

SOURCE=.\sdfsdf.cpp
# End Source File
# Begin Source File

SOURCE=.\SmartRenderEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\Xml2Dex.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AMTimeline.h
# End Source File
# Begin Source File

SOURCE=.\AMTimelineComp.h
# End Source File
# Begin Source File

SOURCE=.\AMTimelineEffect.h
# End Source File
# Begin Source File

SOURCE=.\AMTimelineGroup.h
# End Source File
# Begin Source File

SOURCE=.\AMTimelineObj.h
# End Source File
# Begin Source File

SOURCE=.\AMTimelineSrc.h
# End Source File
# Begin Source File

SOURCE=.\AMTimelineTrack.h
# End Source File
# Begin Source File

SOURCE=.\AMTimelineTrans.h
# End Source File
# Begin Source File

SOURCE=.\AudMixer.h
# End Source File
# Begin Source File

SOURCE=.\ColorSource.h
# End Source File
# Begin Source File

SOURCE=.\DexterLib.h
# End Source File
# Begin Source File

SOURCE=.\DexterLibInterfaces.h
# End Source File
# Begin Source File

SOURCE=.\DxtAlphaSetter.h
# End Source File
# Begin Source File

SOURCE=.\DxtCompositor.h
# End Source File
# Begin Source File

SOURCE=.\DxtJpeg.h
# End Source File
# Begin Source File

SOURCE=.\DxtKey.h
# End Source File
# Begin Source File

SOURCE=.\MediaDet.h
# End Source File
# Begin Source File

SOURCE=.\MediaLocator.h
# End Source File
# Begin Source File

SOURCE=.\NullRenderer.h
# End Source File
# Begin Source File

SOURCE=.\PropertySetter.h
# End Source File
# Begin Source File

SOURCE=.\RenderEngine.h
# End Source File
# Begin Source File

SOURCE=.\SampleGrabber.h
# End Source File
# Begin Source File

SOURCE=.\SmartRenderEngine.h
# End Source File
# Begin Source File

SOURCE=.\Xml2Dex.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
