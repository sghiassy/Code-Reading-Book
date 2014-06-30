# Microsoft Developer Studio Project File - Name="NetworkKit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=NetworkKit - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NetworkKit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NetworkKit.mak" CFG="NetworkKit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NetworkKit - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "NetworkKit - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NetworkKit - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\lib"
# PROP Intermediate_Dir "ReleaseS\obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\..\include\Core" /I "..\..\..\include\Events" /I "..\..\..\include\Exceptions" /I "..\..\..\include\io" /I "..\..\..\include\Utils" /I "..\..\..\include\ImplementerKit" /I "..\..\..\include\net" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\NetworkKit_s.lib"

!ELSEIF  "$(CFG)" == "NetworkKit - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\lib"
# PROP Intermediate_Dir "DebugS\obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\include\Core" /I "..\..\..\include\Events" /I "..\..\..\include\Exceptions" /I "..\..\..\include\io" /I "..\..\..\include\Utils" /I "..\..\..\include\ImplementerKit" /I "..\..\..\include\net" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "NO_MFC" /YX"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\NetworkKit_sd.lib"

!ENDIF 

# Begin Target

# Name "NetworkKit - Win32 Release"
# Name "NetworkKit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\net\NetToolkit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\net\ServerSocketEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\net\Socket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\net\SocketEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\net\SocketListeningLoop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\net\URL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ImplementerKit\Win32SocketPeer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\net\NetToolkit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\net\ServerSocketEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\net\ServerSocketListener.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\net\Socket.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\net\SocketEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\net\SocketException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\net\SocketListener.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\net\SocketListeningLoop.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\net\SocketPeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\net\URL.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementerKit\Win32SocketPeer.h
# End Source File
# End Group
# End Target
# End Project
