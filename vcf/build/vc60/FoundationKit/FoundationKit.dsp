# Microsoft Developer Studio Project File - Name="FoundationKit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=FoundationKit - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FoundationKit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FoundationKit.mak" CFG="FoundationKit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FoundationKit - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FoundationKit - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "FoundationKit"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FoundationKit - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /WX /GR /GX /O2 /I "..\..\..\include\Core" /I "..\..\..\include\Events" /I "..\..\..\include\Exceptions" /I "..\..\..\include\io" /I "..\..\..\include\Utils" /I "..\..\..\include\ImplementerKit" /I "..\..\..\include\Implementer" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "NO_MFC" /Yu"FoundationKit.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\FoundationKit_s.lib"

!ELSEIF  "$(CFG)" == "FoundationKit - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /WX /Gm /GR /GX /ZI /Od /I "..\..\..\include\Core" /I "..\..\..\include\Events" /I "..\..\..\include\Exceptions" /I "..\..\..\include\io" /I "..\..\..\include\Utils" /I "..\..\..\include\ImplementerKit" /I "..\..\..\include\Implementer" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "NO_MFC" /Yu"FoundationKit.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\FoundationKit_sd.lib"

!ENDIF 

# Begin Target

# Name "FoundationKit - Win32 Release"
# Name "FoundationKit - Win32 Debug"
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

SOURCE=..\..\..\src\Utils\ErrorLog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Events\Event.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Events\EventHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\IO\File.cpp
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

SOURCE=..\..\..\src\Utils\Library.cpp
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

SOURCE=..\..\..\src\Utils\SystemToolkit.cpp
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

SOURCE=..\..\..\src\ImplementerKit\Win32LibraryPeer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ImplementerKit\Win32Peer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ImplementerKit\Win32ProcessPeer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ImplementerKit\Win32Registry.cpp
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

SOURCE=..\..\..\include\events\Listener.h
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
# End Target
# End Project
