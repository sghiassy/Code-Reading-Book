# Microsoft Developer Studio Project File - Name="libart" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libart - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libart.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libart.mak" CFG="libart - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libart - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libart - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libart - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\lib"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\include\Graphics" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libart - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\lib"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include\Graphics" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\libart_d.lib"

!ENDIF 

# Begin Target

# Name "libart - Win32 Release"
# Name "libart - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_affine.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_alphagamma.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_bpath.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_gray_svp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_misc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_pixbuf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_rect.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_rect_svp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_rect_uta.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_render.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_render_gradient.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_render_svp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_rgb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_rgb_affine.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_rgb_affine_private.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_rgb_bitmap_affine.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_rgb_pixbuf_affine.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_rgb_rgba_affine.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_rgb_svp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_rgba.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_svp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_svp_ops.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_svp_point.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_svp_render_aa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_svp_vpath.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_svp_vpath_stroke.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_svp_wind.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_uta.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_uta_ops.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_uta_rect.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_uta_svp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_uta_vpath.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_vpath.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_vpath_bpath.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_vpath_dash.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Graphics\art_vpath_svp.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\graphics\art_affine.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_alphagamma.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_bpath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_filterlevel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_gray_svp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_misc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_pathcode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_pixbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_point.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_rect.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_rect_svp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_rect_uta.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_render.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_render_gradient.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_render_svp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_rgb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_rgb_affine.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_rgb_affine_private.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_rgb_bitmap_affine.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_rgb_pixbuf_affine.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_rgb_rgba_affine.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_rgb_svp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_rgba.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_svp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_svp_ops.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_svp_point.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_svp_render_aa.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_svp_vpath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_svp_vpath_stroke.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_svp_wind.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_uta.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_uta_ops.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_uta_rect.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_uta_svp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_uta_vpath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_vpath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_vpath_bpath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_vpath_dash.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\art_vpath_svp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\graphics\libart.h
# End Source File
# End Group
# End Target
# End Project
