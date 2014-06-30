# Microsoft Developer Studio Project File - Name="GraphicsKit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=GraphicsKit - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GraphicsKit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GraphicsKit.mak" CFG="GraphicsKit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GraphicsKit - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "GraphicsKit - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "GraphicsKit"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GraphicsKit - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\..\include\Graphics" /I "..\..\..\include\Core" /I "..\..\..\include\DragDrop" /I "..\..\..\include\Implementer" /I "..\..\..\include\Events" /I "..\..\..\include\Exceptions" /I "..\..\..\include\io" /I "..\..\..\include\Utils" /I "..\..\..\include\ImplementerKit" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "NO_MFC" /Yu"GraphicsKit.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\GraphicsKit_s.lib"

!ELSEIF  "$(CFG)" == "GraphicsKit - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\include\Graphics" /I "..\..\..\include\Core" /I "..\..\..\include\DragDrop" /I "..\..\..\include\Implementer" /I "..\..\..\include\Events" /I "..\..\..\include\Exceptions" /I "..\..\..\include\io" /I "..\..\..\include\Utils" /I "..\..\..\include\ImplementerKit" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "NO_MFC" /Yu"GraphicsKit.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\GraphicsKit_sd.lib"

!ENDIF 

# Begin Target

# Name "GraphicsKit - Win32 Release"
# Name "GraphicsKit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\Graphics\AbstractImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\AlphaGamma.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\ArtStroke.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\BasicFill.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\BasicStroke.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\Color.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\DrawToolkit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\Font.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\FontState.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\GlyphCollection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\GraphicsContext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\GraphicsKit.cpp

!IF  "$(CFG)" == "GraphicsKit - Win32 Release"

# ADD CPP /Yc"GraphicsKit.h"

!ELSEIF  "$(CFG)" == "GraphicsKit - Win32 Debug"

# ADD CPP /Yc"GraphicsKit.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\GraphicsObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Utils\GraphicsToolKit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Events\ImageEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\Matrix2D.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\MicroTiles.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\OpenGLContext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\PixelBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\Polygon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\RenderPaths.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\Vector2D.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ImplementerKit\Win32Context.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ImplementerKit\Win32Font.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ImplementerKit\Win32FontManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ImplementerKit\Win32GraphicsToolkit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ImplementerKit\Win32Image.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ImplementerKit\Win32OpenGLPeer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\graphics\AbstractImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\AlphaGamma.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\ArtStroke.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Basic3DBorder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\BasicFill.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\BasicRectangle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\BasicStroke.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Bitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Border.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Circle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\ClippingRegion.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Color.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Composition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementer\Contextpeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\ControlGraphicsContext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Curve.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\DrawToolkit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Ellipse.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Fill.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Filter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Font.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\FontPeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Glyph.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\GlyphCollection.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\GraphicsContext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\GraphicsKit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\GraphicsObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\GraphicsResourceMgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\utils\GraphicsToolKit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Image.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\ImageBits.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\ImageList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\ImageLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\ImageTile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Kernel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Layer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Matrix.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Matrix2D.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\MatrixFunction.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\MicroTiles.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\OpenGLContext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementer\OpenGLPeer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Path.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\PathEnumerator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\PixelBuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Polygon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Printable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\PrintContext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\RenderableArea.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\RenderPaths.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Stroke.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\TileManager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\UVMap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\VCFOpenGL.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\Vector2D.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementerKit\Win32Context.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementerKit\Win32Font.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementerKit\Win32GraphicsToolkit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementerKit\Win32Image.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\implementerkit\Win32OpenGLPeer.h
# End Source File
# End Group
# End Target
# End Project
