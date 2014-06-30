# Microsoft Developer Studio Project File - Name="CosNotification_Static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CosNotification_Static - Win32 Static Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CosNotification_Static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CosNotification_Static.mak" CFG="CosNotification_Static - Win32 Static Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CosNotification_Static - Win32 Static Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CosNotification_Static - Win32 Static Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CosNotification_Static - Win32 Static Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "LIB\Release\CosNotification"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../" /I "../../" /I "../../../" /D "_MBCS" /D "_LIB" /D "TAO_AS_STATIC_LIBS" /D "NDEBUG" /D "ACE_AS_STATIC_LIBS" /D "WIN32" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"TAO_CosNotifications.lib"

!ELSEIF  "$(CFG)" == "CosNotification_Static - Win32 Static Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "LIB\Debug\CosNotification"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../" /I "../../" /I "../../../" /D "_MBCS" /D "_LIB" /D "TAO_AS_STATIC_LIBS" /D "_DEBUG" /D "ACE_AS_STATIC_LIBS" /D "WIN32" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"TAO_CosNotificationsd.lib"

!ENDIF 

# Begin Target

# Name "CosNotification_Static - Win32 Static Release"
# Name "CosNotification_Static - Win32 Static Debug"
# Begin Group "Template Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CosNotificationS_T.cpp

!IF  "$(CFG)" == "CosNotification_Static - Win32 Static Release"

!ELSEIF  "$(CFG)" == "CosNotification_Static - Win32 Static Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CosNotifyChannelAdminS_T.cpp

!IF  "$(CFG)" == "CosNotification_Static - Win32 Static Release"

!ELSEIF  "$(CFG)" == "CosNotification_Static - Win32 Static Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CosNotifyCommS_T.cpp

!IF  "$(CFG)" == "CosNotification_Static - Win32 Static Release"

!ELSEIF  "$(CFG)" == "CosNotification_Static - Win32 Static Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CosNotifyFilterS_T.cpp

!IF  "$(CFG)" == "CosNotification_Static - Win32 Static Release"

!ELSEIF  "$(CFG)" == "CosNotification_Static - Win32 Static Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ID_Pool_T.cpp

!IF  "$(CFG)" == "CosNotification_Static - Win32 Static Release"

!ELSEIF  "$(CFG)" == "CosNotification_Static - Win32 Static Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Proxy_T.cpp

!IF  "$(CFG)" == "CosNotification_Static - Win32 Static Release"

!ELSEIF  "$(CFG)" == "CosNotification_Static - Win32 Static Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ProxyConsumer_T.cpp

!IF  "$(CFG)" == "CosNotification_Static - Win32 Static Release"

!ELSEIF  "$(CFG)" == "CosNotification_Static - Win32 Static Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ProxySupplier_T.cpp

!IF  "$(CFG)" == "CosNotification_Static - Win32 Static Release"

!ELSEIF  "$(CFG)" == "CosNotification_Static - Win32 Static Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "IDL Files"

# PROP Default_Filter "idl"
# Begin Source File

SOURCE=.\CosEventChannelAdmin.idl

!IF  "$(CFG)" == "CosNotification_Static - Win32 Static Release"

# Begin Custom Build - Invoking TAO_IDL Compiler on $(InputPath)
InputPath=.\CosEventChannelAdmin.idl
InputName=CosEventChannelAdmin

BuildCmds= \
	..\..\..\bin\tao_idl -Ge 1 -I../../ -I../../tao  -Wb,pre_include=ace/pre.h      -Wb,post_include=ace/post.h -Wb,export_macro=TAO_Notify_Export      -Wb,export_include=Notify/notify_export.h $(InputName).idl

"$(InputName)C.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "CosNotification_Static - Win32 Static Debug"

# Begin Custom Build - Invoking TAO_IDL Compiler on $(InputPath)
InputPath=.\CosEventChannelAdmin.idl
InputName=CosEventChannelAdmin

BuildCmds= \
	..\..\..\bin\tao_idl -Ge 1 -I../../ -I../../tao  -Wb,pre_include=ace/pre.h      -Wb,post_include=ace/post.h -Wb,export_macro=TAO_Notify_Export      -Wb,export_include=Notify/notify_export.h $(InputName).idl

"$(InputName)C.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CosEventComm.idl

!IF  "$(CFG)" == "CosNotification_Static - Win32 Static Release"

!ELSEIF  "$(CFG)" == "CosNotification_Static - Win32 Static Debug"

# Begin Custom Build - Invoking TAO_IDL Compiler on $(InputPath)
InputPath=.\CosEventComm.idl
InputName=CosEventComm

BuildCmds= \
	..\..\..\bin\tao_idl -Ge 1 -I../../ -I../../tao  -Wb,pre_include=ace/pre.h      -Wb,post_include=ace/post.h -Wb,export_macro=TAO_Notify_Export      -Wb,export_include=Notify/notify_export.h $(InputName).idl

"$(InputName)C.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CosNotification.idl

!IF  "$(CFG)" == "CosNotification_Static - Win32 Static Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Invoking TAO_IDL on $(InputPath)
InputPath=.\CosNotification.idl
InputName=CosNotification

BuildCmds= \
	..\..\..\bin\Release\tao_idl_static -Ge 1 -I../../ -I../../tao     -Wb,pre_include=ace/pre.h -Wb,post_include=ace/post.h    -Wb,export_macro=TAO_Notify_Export -Wb,export_include=Notify/notify_export.h    $(InputName).idl

"$(InputName)C.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "CosNotification_Static - Win32 Static Debug"

# PROP Ignore_Default_Tool 1
USERDEP__COSNO="..\..\..\bin\tao_idl_static.exe"	
# Begin Custom Build - Invoking TAO_IDL on $(InputPath)
InputPath=.\CosNotification.idl
InputName=CosNotification

BuildCmds= \
	..\..\..\bin\tao_idl_static -Ge 1 -I../../ -I../../tao    -Wb,pre_include=ace/pre.h  -Wb,post_include=ace/post.h   -Wb,export_macro=TAO_Notify_Export  -Wb,export_include=Notify/notify_export.h   $(InputName).idl

"$(InputName)C.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CosNotifyChannelAdmin.idl

!IF  "$(CFG)" == "CosNotification_Static - Win32 Static Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Invoking TAO_IDL on $(InputPath)
InputPath=.\CosNotifyChannelAdmin.idl
InputName=CosNotifyChannelAdmin

BuildCmds= \
	..\..\..\bin\Release\tao_idl_static -Ge 1 -I../../ -I../../tao     -Wb,pre_include=ace/pre.h -Wb,post_include=ace/post.h    -Wb,export_macro=TAO_Notify_Export -Wb,export_include=Notify/notify_export.h    $(InputName).idl

"$(InputName)C.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "CosNotification_Static - Win32 Static Debug"

# PROP Ignore_Default_Tool 1
USERDEP__COSNOT="..\..\..\bin\tao_idl_static.exe"	
# Begin Custom Build - Invoking TAO_IDL on $(InputPath)
InputPath=.\CosNotifyChannelAdmin.idl
InputName=CosNotifyChannelAdmin

BuildCmds= \
	..\..\..\bin\tao_idl_static -Ge 1 -I../../ -I../../tao    -Wb,pre_include=ace/pre.h  -Wb,post_include=ace/post.h   -Wb,export_macro=TAO_Notify_Export  -Wb,export_include=Notify/notify_export.h   $(InputName).idl

"$(InputName)C.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CosNotifyComm.idl

!IF  "$(CFG)" == "CosNotification_Static - Win32 Static Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Invoking TAO_IDL on $(InputPath)
InputPath=.\CosNotifyComm.idl
InputName=CosNotifyComm

BuildCmds= \
	..\..\..\bin\Release\tao_idl_static -Ge 1 -I../../ -I../../tao     -Wb,pre_include=ace/pre.h -Wb,post_include=ace/post.h    -Wb,export_macro=TAO_Notify_Export -Wb,export_include=Notify/notify_export.h    $(InputName).idl

"$(InputName)C.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "CosNotification_Static - Win32 Static Debug"

# PROP Ignore_Default_Tool 1
USERDEP__COSNOTI="..\..\..\bin\tao_idl_static.exe"	
# Begin Custom Build - Invoking TAO_IDL on $(InputPath)
InputPath=.\CosNotifyComm.idl
InputName=CosNotifyComm

BuildCmds= \
	..\..\..\bin\tao_idl_static -Ge 1 -I../../ -I../../tao    -Wb,pre_include=ace/pre.h  -Wb,post_include=ace/post.h   -Wb,export_macro=TAO_Notify_Export  -Wb,export_include=Notify/notify_export.h   $(InputName).idl

"$(InputName)C.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CosNotifyFilter.idl

!IF  "$(CFG)" == "CosNotification_Static - Win32 Static Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Invoking TAO_IDL on $(InputPath)
InputPath=.\CosNotifyFilter.idl
InputName=CosNotifyFilter

BuildCmds= \
	..\..\..\bin\Release\tao_idl_static -Ge 1 -I../../ -I../../tao     -Wb,pre_include=ace/pre.h -Wb,post_include=ace/post.h    -Wb,export_macro=TAO_Notify_Export -Wb,export_include=Notify/notify_export.h    $(InputName).idl

"$(InputName)C.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "CosNotification_Static - Win32 Static Debug"

# PROP Ignore_Default_Tool 1
USERDEP__COSNOTIF="..\..\..\bin\tao_idl_static.exe"	
# Begin Custom Build - Invoking TAO_IDL on $(InputPath)
InputPath=.\CosNotifyFilter.idl
InputName=CosNotifyFilter

BuildCmds= \
	..\..\..\bin\tao_idl_static -Ge 1 -I../../ -I../../tao    -Wb,pre_include=ace/pre.h  -Wb,post_include=ace/post.h   -Wb,export_macro=TAO_Notify_Export  -Wb,export_include=Notify/notify_export.h   $(InputName).idl

"$(InputName)C.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)C.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)S_T.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Inline Files"

# PROP Default_Filter "i"
# Begin Source File

SOURCE=.\CosEventChannelAdminC.i
# End Source File
# Begin Source File

SOURCE=.\CosEventChannelAdminS.i
# End Source File
# Begin Source File

SOURCE=.\CosEventChannelAdminS_T.i
# End Source File
# Begin Source File

SOURCE=.\CosEventCommC.i
# End Source File
# Begin Source File

SOURCE=.\CosEventCommS.i
# End Source File
# Begin Source File

SOURCE=.\CosEventCommS_T.i
# End Source File
# Begin Source File

SOURCE=.\CosNotificationC.i
# End Source File
# Begin Source File

SOURCE=.\CosNotificationS.i
# End Source File
# Begin Source File

SOURCE=.\CosNotificationS_T.i
# End Source File
# Begin Source File

SOURCE=.\CosNotifyChannelAdminC.i
# End Source File
# Begin Source File

SOURCE=.\CosNotifyChannelAdminS.i
# End Source File
# Begin Source File

SOURCE=.\CosNotifyCommC.i
# End Source File
# Begin Source File

SOURCE=.\CosNotifyCommS.i
# End Source File
# Begin Source File

SOURCE=.\CosNotifyFilterC.i
# End Source File
# Begin Source File

SOURCE=.\CosNotifyFilterS.i
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Event_Manager.i
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Event_Map.i
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ID_Pool_T.i
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\CosNotificationC.h
# End Source File
# Begin Source File

SOURCE=.\CosNotificationS.h
# End Source File
# Begin Source File

SOURCE=.\CosNotificationS_T.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_AdminProperties.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Buffering_Strategy.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Channel_Objects_Factory.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Collection.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Collection_Factory.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Command.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Constraint_Interpreter.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Constraint_Visitors.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ConsumerAdmin_i.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Default_CO_Factory.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Default_Collection_Factory.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Default_EMO_Factory.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Default_POA_Factory.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Event.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Event_Dispatch_Command.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Event_Manager.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Event_Manager_Objects_Factory.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Event_Map.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Event_Processor.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_EventChannel_i.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_EventChannelFactory_i.h
# End Source File
# Begin Source File

SOURCE=.\Notify\notify_export.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Factory.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Filter_i.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_FilterAdmin_i.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_FilterFactory_i.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ID_Pool_T.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Listener_Filter_Eval_Command.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Listeners.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Lookup_Command.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_MT_Worker_Task.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_POA_Factory.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Proxy_T.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ProxyConsumer_T.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ProxyPushConsumer_i.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ProxyPushSupplier_i.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ProxySupplier_T.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_PushConsumer.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_PushSupplier.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_QoSAdmin_i.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Resource_Manager.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_SequenceProxyPushConsumer_i.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_SequenceProxyPushSupplier_i.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Source_Filter_Eval_Command.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_StructuredProxyPushConsumer_i.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_StructuredProxyPushSupplier_i.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_StructuredPushConsumer.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_StructuredPushSupplier.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_SupplierAdmin_i.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Types.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Update_Dispatch_Command.h
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Worker_Task.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\CosNotificationC.cpp
# End Source File
# Begin Source File

SOURCE=.\CosNotificationS.cpp
# End Source File
# Begin Source File

SOURCE=.\CosNotifyChannelAdminC.cpp
# End Source File
# Begin Source File

SOURCE=.\CosNotifyChannelAdminS.cpp
# End Source File
# Begin Source File

SOURCE=.\CosNotifyCommC.cpp
# End Source File
# Begin Source File

SOURCE=.\CosNotifyCommS.cpp
# End Source File
# Begin Source File

SOURCE=.\CosNotifyFilterC.cpp
# End Source File
# Begin Source File

SOURCE=.\CosNotifyFilterS.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_AdminProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_AdminProperties.i
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Buffering_Strategy.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Collection.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Command.i
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Constraint_Interpreter.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Constraint_Visitors.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ConsumerAdmin_i.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Default_CO_Factory.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Default_Collection_Factory.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Default_EMO_Factory.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Default_POA_Factory.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Event.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Event.i
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Event_Dispatch_Command.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Event_Manager.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Event_Map.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Event_Processor.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_EventChannel_i.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_EventChannelFactory_i.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Factory.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Filter_i.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_FilterAdmin_i.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_FilterFactory_i.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Listener_Filter_Eval_Command.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Lookup_Command.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_MT_Worker_Task.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ProxyPushConsumer_i.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ProxyPushSupplier_i.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_QoSAdmin_i.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_SequenceProxyPushConsumer_i.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_SequenceProxyPushSupplier_i.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Source_Filter_Eval_Command.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_StructuredProxyPushConsumer_i.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_StructuredProxyPushSupplier_i.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_SupplierAdmin_i.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Update_Dispatch_Command.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Worker_Task.cpp
# End Source File
# End Group
# End Target
# End Project
