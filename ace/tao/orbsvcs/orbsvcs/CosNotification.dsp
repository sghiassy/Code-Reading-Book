# Microsoft Developer Studio Project File - Name="CosNotification" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CosNotification - Win32 MFC Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CosNotification.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CosNotification.mak" CFG="CosNotification - Win32 MFC Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CosNotification - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CosNotification - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CosNotification - Win32 MFC Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CosNotification - Win32 MFC Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CosNotification - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "DLL\Release\CosNotification"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Notification_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../" /I "../../" /I "../../../" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "TAO_NOTIFY_BUILD_DLL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 TAO_CosEvent.lib TAO_CosTrading.lib ace.lib TAO.lib TAO_PortableServer.lib /nologo /dll /machine:I386 /out:"..\..\..\bin\TAO_CosNotification.dll" /libpath:"..\..\tao\PortableServer" /libpath:"..\..\tao" /libpath:"..\..\..\ace"

!ELSEIF  "$(CFG)" == "CosNotification - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "DLL\Debug\CosNotification"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Notification_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../" /I "../../" /I "../../../" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "TAO_NOTIFY_BUILD_DLL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 TAO_CosTradingd.lib TAOd.lib aced.lib TAO_PortableServerd.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\bin\TAO_CosNotificationd.dll" /pdbtype:sept /libpath:"..\..\tao\PortableServer" /libpath:"..\..\tao" /libpath:"..\..\..\ace"

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CosNotification___Win32_MFC_Debug"
# PROP BASE Intermediate_Dir "CosNotification___Win32_MFC_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "DLL\Debug\CosNotificationMFC"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../" /I "../../" /I "../../../" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "TAO_NOTIFY_BUILD_DLL" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../" /I "../../" /I "../../../" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "TAO_NOTIFY_BUILD_DLL" /D ACE_HAS_MFC=1 /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 TAOd.lib aced.lib TAO_CosEventd.lib TAO_CosTradingd.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\bin\TAO_CosNotificationd.dll" /pdbtype:sept /libpath:"..\..\tao" /libpath:"..\..\..\ace"
# ADD LINK32 TAOmfcd.lib aced.lib TAO_CosEventmfcd.lib TAO_CosTradingmfcd.lib TAO_PortableServermfcd.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\bin\TAO_CosNotificationmfcd.dll" /pdbtype:sept /libpath:"..\..\tao\PortableServer" /libpath:"..\..\tao" /libpath:"..\..\..\ace"

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CosNotification___Win32_MFC_Release"
# PROP BASE Intermediate_Dir "CosNotification___Win32_MFC_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "DLL\Release\CosNotificationMFC"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../" /I "../../" /I "../../../" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "TAO_NOTIFY_BUILD_DLL" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../" /I "../../" /I "../../../" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "TAO_NOTIFY_BUILD_DLL" /D ACE_HAS_MFC=1 /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ace.lib TAO.lib TAO_CosEvent.lib TAO_CosTrading.lib /nologo /dll /machine:I386 /out:"..\..\..\bin\TAO_CosNotification.dll" /libpath:"..\..\tao" /libpath:"..\..\..\ace"
# ADD LINK32 acemfc.lib TAOmfc.lib TAO_CosEventmfc.lib TAO_CosTradingmfc.lib TAO_PortableServermfc.lib /nologo /dll /machine:I386 /out:"..\..\..\bin\TAO_CosNotificationmfc.dll" /libpath:"..\..\tao\PortableServer" /libpath:"..\..\tao" /libpath:"..\..\..\ace"

!ENDIF 

# Begin Target

# Name "CosNotification - Win32 Release"
# Name "CosNotification - Win32 Debug"
# Name "CosNotification - Win32 MFC Debug"
# Name "CosNotification - Win32 MFC Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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

SOURCE=.\Notify\Notify_Buffering_Strategy.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Collection.cpp
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Command.cpp
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
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
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

SOURCE=.\Notify\Notify_Default_Resource_Factory.h
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

SOURCE=.\Notify\Notify_Listener_Filter_Eval.h
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
# Begin Group "IDL Files"

# PROP Default_Filter ".idl"
# Begin Source File

SOURCE=.\CosNotification.idl

!IF  "$(CFG)" == "CosNotification - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__COSNO="..\..\..\bin\release\tao_idl.exe"	
# Begin Custom Build - Invoking TAO_IDL Compiler on $(InputPath)
InputPath=.\CosNotification.idl
InputName=CosNotification

BuildCmds= \
	..\..\..\bin\release\tao_idl -Ge 1 -I../../ -I../../tao      -Wb,pre_include=ace/pre.h -Wb,post_include=ace/post.h     -Wb,export_macro=TAO_Notify_Export -Wb,export_include=Notify/notify_export.h     $(InputName).idl

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

!ELSEIF  "$(CFG)" == "CosNotification - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__COSNO="..\..\..\bin\tao_idl.exe"	
# Begin Custom Build - Invoking TAO_IDL Compiler on $(InputPath)
InputPath=.\CosNotification.idl
InputName=CosNotification

BuildCmds= \
	..\..\..\bin\tao_idl -Ge 1 -I../../ -I../../tao  -Wb,pre_include=ace/pre.h  -Wb,post_include=ace/post.h -Wb,export_macro=TAO_Notify_Export  -Wb,export_include=Notify\notify_export.h $(InputName).idl

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

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Debug"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CosNotifyChannelAdmin.idl

!IF  "$(CFG)" == "CosNotification - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__COSNOT="..\..\..\bin\release\tao_idl.exe"	
# Begin Custom Build - Invoking TAO_IDL Compiler on $(InputPath)
InputPath=.\CosNotifyChannelAdmin.idl
InputName=CosNotifyChannelAdmin

BuildCmds= \
	..\..\..\bin\release\tao_idl -Ge 1 -I../../ -I../../tao      -Wb,pre_include=ace/pre.h -Wb,post_include=ace/post.h     -Wb,export_macro=TAO_Notify_Export -Wb,export_include=Notify/notify_export.h     $(InputName).idl

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

!ELSEIF  "$(CFG)" == "CosNotification - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__COSNOT="..\..\..\bin\tao_idl.exe"	
# Begin Custom Build - Invoking TAO_IDL Compiler on $(InputPath)
InputPath=.\CosNotifyChannelAdmin.idl
InputName=CosNotifyChannelAdmin

BuildCmds= \
	..\..\..\bin\tao_idl -Ge 1 -I../../ -I../../tao  -Wb,pre_include=ace/pre.h  -Wb,post_include=ace/post.h -Wb,export_macro=TAO_Notify_Export  -Wb,export_include=Notify\notify_export.h $(InputName).idl

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

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Debug"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CosNotifyComm.idl

!IF  "$(CFG)" == "CosNotification - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__COSNOTI="..\..\..\bin\release\tao_idl.exe"	
# Begin Custom Build - Invoking TAO_IDL Compiler on $(InputPath)
InputPath=.\CosNotifyComm.idl
InputName=CosNotifyComm

BuildCmds= \
	..\..\..\bin\release\tao_idl -Ge 1 -I../../ -I../../tao      -Wb,pre_include=ace/pre.h -Wb,post_include=ace/post.h     -Wb,export_macro=TAO_Notify_Export -Wb,export_include=Notify/notify_export.h     $(InputName).idl

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

!ELSEIF  "$(CFG)" == "CosNotification - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__COSNOTI="..\..\..\bin\tao_idl.exe"	
# Begin Custom Build - Invoking TAO_IDL Compiler on $(InputPath)
InputPath=.\CosNotifyComm.idl
InputName=CosNotifyComm

BuildCmds= \
	..\..\..\bin\tao_idl -Ge 1 -I../../ -I../../tao  -Wb,pre_include=ace/pre.h  -Wb,post_include=ace/post.h -Wb,export_macro=TAO_Notify_Export  -Wb,export_include=Notify\notify_export.h $(InputName).idl

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

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Debug"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CosNotifyFilter.idl

!IF  "$(CFG)" == "CosNotification - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__COSNOTIF="..\..\..\bin\release\tao_idl.exe"	
# Begin Custom Build - Invoking TAO_IDL Compiler on $(InputPath)
InputPath=.\CosNotifyFilter.idl
InputName=CosNotifyFilter

BuildCmds= \
	..\..\..\bin\release\tao_idl -Ge 1 -I../../ -I../../tao      -Wb,pre_include=ace/pre.h -Wb,post_include=ace/post.h     -Wb,export_macro=TAO_Notify_Export -Wb,export_include=Notify/notify_export.h     $(InputName).idl

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

!ELSEIF  "$(CFG)" == "CosNotification - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__COSNOTIF="..\..\..\bin\tao_idl.exe"	
# Begin Custom Build - Invoking TAO_IDL Compiler on $(InputPath)
InputPath=.\CosNotifyFilter.idl
InputName=CosNotifyFilter

BuildCmds= \
	..\..\..\bin\tao_idl -Ge 1 -I../../ -I../../tao  -Wb,pre_include=ace/pre.h  -Wb,post_include=ace/post.h -Wb,export_macro=TAO_Notify_Export  -Wb,export_include=Notify\notify_export.h $(InputName).idl

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

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Debug"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Inline Files"

# PROP Default_Filter ".i"
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

SOURCE=.\Notify\Notify_AdminProperties.i
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Command.i
# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Event.i
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
# Begin Group "Template Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CosNotificationS_T.cpp

!IF  "$(CFG)" == "CosNotification - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CosNotifyChannelAdminS_T.cpp

!IF  "$(CFG)" == "CosNotification - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CosNotifyCommS_T.cpp

!IF  "$(CFG)" == "CosNotification - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CosNotifyFilterS_T.cpp

!IF  "$(CFG)" == "CosNotification - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ID_Pool_T.cpp

!IF  "$(CFG)" == "CosNotification - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_Proxy_T.cpp

!IF  "$(CFG)" == "CosNotification - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ProxyConsumer_T.cpp

!IF  "$(CFG)" == "CosNotification - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Notify\Notify_ProxySupplier_T.cpp

!IF  "$(CFG)" == "CosNotification - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CosNotification - Win32 MFC Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
