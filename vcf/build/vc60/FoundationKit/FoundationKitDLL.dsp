# Microsoft Developer Studio Project File - Name="FoundationKitDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=FOUNDATIONKITDLL - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FoundationKitDLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FoundationKitDLL.mak" CFG="FOUNDATIONKITDLL - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FoundationKitDLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FoundationKitDLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "FoundationKitDLL"
# PROP Scc_LocalPath "..\..\.."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FoundationKitDLL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\lib"
# PROP Intermediate_Dir "ReleaseDLL\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FOUNDATIONKITDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\..\include\Core" /I "..\..\..\include\Events" /I "..\..\..\include\Exceptions" /I "..\..\..\include\io" /I "..\..\..\include\Utils" /I "..\..\..\include\ImplementerKit" /I "..\..\..\include\Implementer" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FRAMEWORK_DLL" /D "FRAMEWORK_EXPORTS" /Yu"FoundationKit.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 rpcrt4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\..\bin/FoundationKit.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "FoundationKitDLL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FoundationKitDLL___Win32_Debug"
# PROP BASE Intermediate_Dir "FoundationKitDLL___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\lib"
# PROP Intermediate_Dir "DebugDLL\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FOUNDATIONKITDLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\include\Core" /I "..\..\..\include\Events" /I "..\..\..\include\Exceptions" /I "..\..\..\include\io" /I "..\..\..\include\Utils" /I "..\..\..\include\ImplementerKit" /I "..\..\..\include\Implementer" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FRAMEWORK_DLL" /D "FRAMEWORK_EXPORTS" /Yu"FoundationKit.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Rpcrt4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /profile /debug /machine:I386 /out:"..\..\..\bin/FoundationKit_d.dll"

!ENDIF 

# Begin Target

# Name "FoundationKitDLL - Win32 Release"
# Name "FoundationKitDLL - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\exceptions\BasicException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\BasicFileError.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\BasicInputStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\BasicOutputStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\exceptions\CantCreateObjectException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\Class.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\ClassInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\exceptions\ClassNotFound.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\ClassRegistry.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\CommonDefines.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\Directory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\Enum.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\Enumerator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\ErrorLog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\exceptions\ErrorStrings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\events\Event.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\events\EventHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\File.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\FileIOError.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\FileNotFound.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\Implementer\FilePeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\FileStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\FileStreamPeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\FileUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\FoundationKit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\FrameworkConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\Interface.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\InterfaceClass.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\exceptions\InvalidPeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\exceptions\InvalidPointerException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\Library.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementer\LibraryPeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\Locales.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\MemoryStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\MemStreamUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\Method.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\exceptions\NoFreeMemException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\exceptions\NoPeerFoundException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\exceptions\NoSuchElementException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\events\NotifyListener.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\Object.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\ObjectWithEvents.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\exceptions\OutOfBoundsException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\Parser.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\Persistable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\Point.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\exceptions\ProcessException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementer\ProcessPeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\Property.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\events\PropertyChangeEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\exceptions\PropertyChangeException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\events\PropertyListener.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\Rect.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\Registry.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementer\RegistryPeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\Runnable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\exceptions\RuntimeException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\Size.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\Stream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\StringUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\System.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\SystemPeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\SystemToolkit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\TextInputStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\TextOutputStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\Thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\events\ThreadEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementer\ThreadPeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\exceptions\TypeCastException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\io\VariantDataStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\VCF.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\VCFChar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\core\VCFMath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\VCFProcess.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\VCFString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ImplementerKit\Win32FilePeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementerKit\Win32FileStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementerkit\Win32LibraryPeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementerkit\Win32ProcessPeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementerkit\Win32Registry.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementerKit\Win32SystemPeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementerKit\Win32Thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\XMLParser.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\Exceptions\BasicException.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\IO\BasicInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\IO\BasicOutputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\core\Class.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\core\ClassRegistry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\IO\Directory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Utils\ErrorLog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Events\Event.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Events\EventHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\io\File.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\IO\FileStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\IO\FileUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\core\FoundationKit.cpp
# ADD CPP /Yc"FoundationKit.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\core\InterfaceClass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\Library.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\core\Locales.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\IO\MemoryStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\core\Object.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\core\ObjectWithEvents.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Utils\Parser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\core\Point.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Events\PropertyChangeEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\core\Rect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Utils\Registry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\IO\Stream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Utils\StringUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Utils\System.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\SystemToolkit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\IO\TextInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\IO\TextOutputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Utils\Thread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Events\ThreadEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\core\VCFMath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Utils\VCFProcess.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ImplementerKit\Win32FilePeer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ImplementerKit\Win32FileStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\implementerkit\Win32LibraryPeer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ImplementerKit\Win32Peer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\implementerkit\Win32ProcessPeer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\implementerkit\Win32Registry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ImplementerKit\Win32SystemPeer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ImplementerKit\Win32Thread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Utils\XMLParser.cpp
# End Source File
# End Group
# End Target
# End Project
