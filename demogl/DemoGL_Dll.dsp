# Microsoft Developer Studio Project File - Name="DemoGL_Dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DemoGL_Dll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DemoGL_Dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DemoGL_Dll.mak" CFG="DemoGL_Dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DemoGL_Dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DemoGL_Dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DemoGL_Dll - Win32 VC50" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DemoGL_Dll - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEMOGL_DLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GR /GX /O2 /Ob2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEMOGL_DLL_EXPORTS" /D "ZLIB_DLL" /Fr /FD /c
# SUBTRACT CPP /WX /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 OpenGL32.lib zlibstat.lib jpeglib.lib glu32.lib kernel32.lib user32.lib gdi32.lib shell32.lib advapi32.lib /nologo /version:1.101 /dll /machine:I386 /nodefaultlib:"LIBC" /libpath:"libs" /OPT:NOWIN98
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEMOGL_DLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GR /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEMOGL_DLL_EXPORTS" /D "ZLIB_DLL" /Fr /YX"DemoGL\dgl_dllstdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 OpenGL32.lib zlibstat.lib jpeglib.lib glu32.lib winmm.lib kernel32.lib user32.lib gdi32.lib shell32.lib advapi32.lib /nologo /version:1.3 /dll /debug /machine:I386 /nodefaultlib:"LIBC" /pdbtype:sept /libpath:"libs"
# SUBTRACT LINK32 /profile /pdb:none /incremental:no /map /nodefaultlib

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 VC50"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DemoGL_Dll___Win32_VC50"
# PROP BASE Intermediate_Dir "DemoGL_Dll___Win32_VC50"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseVC50"
# PROP Intermediate_Dir "ReleaseVC50"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEMOGL_DLL_EXPORTS" /D "ZLIB_DLL" /FD /c
# SUBTRACT BASE CPP /WX /Fr /YX /Yc /Yu
# ADD CPP /nologo /G5 /MT /W3 /GR /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEMOGL_DLL_EXPORTS" /D "ZLIB_DLL" /FD /c
# SUBTRACT CPP /WX /Fr /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 zlibstat.lib OpenGL32.lib winmm.lib XAudio.lib glu32.lib jpeglib.lib kernel32.lib user32.lib gdi32.lib shell32.lib /nologo /dll /machine:I386 /nodefaultlib:"LIBC" /libpath:"lib" /LINK50COMPAT
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 zlibstat.lib OpenGL32.lib winmm.lib glu32.lib jpeglib.lib kernel32.lib user32.lib gdi32.lib shell32.lib advapi32.lib /nologo /dll /machine:I386 /nodefaultlib:"LIBC" /out:"DemoGL_Dll.dll" /libpath:"libs" /LINK50COMPAT
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "DemoGL_Dll - Win32 Release"
# Name "DemoGL_Dll - Win32 Debug"
# Name "DemoGL_Dll - Win32 VC50"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "3rdParty_CPP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CPP\Misc\CFrameHeader.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\Misc\CId3Tag.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\Misc\CMP3Info.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\Misc\CVBitRate.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\Misc\Jpegfile.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\Misc\StdString.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dllbootutilfuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dlldemodat.cpp

!IF  "$(CFG)" == "DemoGL_Dll - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 Debug"

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 VC50"

# ADD BASE CPP /W3
# ADD CPP /W3

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dlleffectbase.cpp

!IF  "$(CFG)" == "DemoGL_Dll - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 Debug"

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 VC50"

# ADD BASE CPP /W3
# ADD CPP /W3

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dlleffectstore.cpp

!IF  "$(CFG)" == "DemoGL_Dll - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 Debug"

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 VC50"

# ADD BASE CPP /W3
# ADD CPP /W3

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dllendsystem.cpp

!IF  "$(CFG)" == "DemoGL_Dll - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 Debug"

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 VC50"

# ADD BASE CPP /W3
# ADD CPP /W3

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dllextensions.cpp

!IF  "$(CFG)" == "DemoGL_Dll - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 Debug"

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 VC50"

# ADD BASE CPP /W3
# ADD CPP /W3

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dllfifoqueue.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dllguicontrol.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dllkernel.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dlllayer.cpp

!IF  "$(CFG)" == "DemoGL_Dll - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 Debug"

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 VC50"

# ADD BASE CPP /W3
# ADD CPP /W3

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dlllayerutilfuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dlllowleveldebugger.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dllmain.cpp

!IF  "$(CFG)" == "DemoGL_Dll - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 Debug"

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 VC50"

# ADD BASE CPP /W3
# ADD CPP /W3

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dllprogressbar.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dllscriptfuncs.cpp

!IF  "$(CFG)" == "DemoGL_Dll - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 Debug"

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 VC50"

# ADD BASE CPP /W3
# ADD CPP /W3

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dllsoundsystem.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dllstartsystem.cpp

!IF  "$(CFG)" == "DemoGL_Dll - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 Debug"

# ADD CPP /ZI

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 VC50"

# ADD BASE CPP /W3
# ADD CPP /W3

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dllstartupdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dllsysconsole.cpp

!IF  "$(CFG)" == "DemoGL_Dll - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 Debug"

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 VC50"

# ADD BASE CPP /W3
# ADD CPP /W3

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dlltexture.cpp

!IF  "$(CFG)" == "DemoGL_Dll - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 Debug"

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 VC50"

# ADD BASE CPP /W3
# ADD CPP /W3

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dlltextureutilfuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dlltimelineevent.cpp

!IF  "$(CFG)" == "DemoGL_Dll - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 Debug"

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 VC50"

# ADD BASE CPP /W3
# ADD CPP /W3

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dlltimelineutilfuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dlltoken.cpp

!IF  "$(CFG)" == "DemoGL_Dll - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 Debug"

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 VC50"

# ADD BASE CPP /W3
# ADD CPP /W3

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CPP\DemoGL\dgl_dllutilfuncs.cpp

!IF  "$(CFG)" == "DemoGL_Dll - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 Debug"

!ELSEIF  "$(CFG)" == "DemoGL_Dll - Win32 VC50"

# ADD BASE CPP /W3
# ADD CPP /W3

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Resources\resources.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "3rdParty_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Include\Misc\bass.h
# End Source File
# Begin Source File

SOURCE=.\Include\Misc\CFrameHeader.h
# End Source File
# Begin Source File

SOURCE=.\Include\Misc\CId3Tag.h
# End Source File
# Begin Source File

SOURCE=.\Include\Misc\CMP3Info.h
# End Source File
# Begin Source File

SOURCE=.\Include\Misc\CVBitRate.h
# End Source File
# Begin Source File

SOURCE=.\Include\JpegLib\JCONFIG.H
# End Source File
# Begin Source File

SOURCE=.\Include\JpegLib\JMORECFG.H
# End Source File
# Begin Source File

SOURCE=.\Include\JpegLib\Jpegfile.h
# End Source File
# Begin Source File

SOURCE=.\Include\JpegLib\JPEGLIB.H
# End Source File
# Begin Source File

SOURCE=.\Include\Misc\StdString.h
# End Source File
# Begin Source File

SOURCE=.\Include\Misc\unzip.h
# End Source File
# Begin Source File

SOURCE=.\Include\Misc\zconf.h
# End Source File
# Begin Source File

SOURCE=.\Include\Misc\zlib.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Include\Distribution\DemoGL_Bass.h
# End Source File
# Begin Source File

SOURCE=.\Include\Distribution\DemoGL_DLL.h
# End Source File
# Begin Source File

SOURCE=.\Include\Distribution\DemoGL_Effect.h
# End Source File
# Begin Source File

SOURCE=.\Include\Distribution\DemoGL_Extensions.h
# End Source File
# Begin Source File

SOURCE=.\Include\Distribution\DemoGL_glext.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dllbootutilfuncs.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dlldemodat.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dlleffect.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dlleffectstore.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dllendsystem.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dllextensions.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dllfifoqueue.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dllguicontrol.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dllkernel.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dlllayer.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dlllayerutilfuncs.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dlllowleveldebugger.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dllmain.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dllprogressbar.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dllscriptfuncs.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dllsoundsystem.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dllstartsystem.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dllstartupdialog.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dllstdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dllsysconsole.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dlltexture.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dlltextureutilfuncs.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dlltimelineevent.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dlltimelineutilfuncs.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dlltoken.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\dgl_dllutilfuncs.h
# End Source File
# Begin Source File

SOURCE=.\Include\DemoGL\resource.h
# End Source File
# Begin Source File

SOURCE=.\Include\Misc\src_beautifiers.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Resources\applogo.bmp
# End Source File
# Begin Source File

SOURCE=.\Resources\DemoGL.ico
# End Source File
# Begin Source File

SOURCE=.\Resources\dgllogo.z
# End Source File
# Begin Source File

SOURCE=.\Resources\envmap2.jpg
# End Source File
# End Group
# Begin Group "Libs"

# PROP Default_Filter ".lib"
# Begin Source File

SOURCE=.\Libs\JpegLib.lib
# End Source File
# Begin Source File

SOURCE=.\Libs\zlibstat.lib
# End Source File
# Begin Source File

SOURCE=.\Libs\bass.lib
# End Source File
# End Group
# Begin Group "Source docs"

# PROP Default_Filter ".txt"
# Begin Source File

SOURCE=".\Source docs\Changelog.txt"
# End Source File
# Begin Source File

SOURCE=".\DLL Version.txt"
# End Source File
# Begin Source File

SOURCE=".\Source docs\License.htm"
# End Source File
# Begin Source File

SOURCE=".\Source docs\srcdescr.htm"
# End Source File
# Begin Source File

SOURCE=..\Misc\ToDo.txt
# End Source File
# End Group
# Begin Source File

SOURCE=.\readme1st.htm
# End Source File
# End Target
# End Project
