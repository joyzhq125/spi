# Microsoft Developer Studio Project File - Name="sec" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=sec - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sec.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sec.mak" CFG="sec - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sec - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sec - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "sec"
# PROP Scc_LocalPath "..\..\.."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sec - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /O2 /I "$(BREWDIR)\inc" /I "..\..\..\impl\inc\we" /I "..\..\..\impl\inc\sec" /I "..\..\..\inc" /I ".\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AEE_SIMULATOR" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "AEE_SIMULATOR" /MTd
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"sec.dll"

!ELSEIF  "$(CFG)" == "sec - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "sec___Win32_Debug"
# PROP BASE Intermediate_Dir "sec___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(BREWDIR)\inc" /I "..\..\..\impl\inc\sec" /I "..\..\..\inc" /I ".\\" /I "..\..\..\impl\src\oem" /I "..\..\..\..\..\wep\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AEE_SIMULATOR" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "AEE_SIMULATOR" /MTd
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"sec.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "sec - Win32 Release"
# Name "sec - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\impl\src\aee\isecb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\aee\isecw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\aee\isigntext.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_seccsc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_secfile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_seclog.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_secmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_secpkc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_secx509parser.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\aee\sec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\cert\sec_cd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\cert\sec_cm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\main\sec_dp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\main\sec_evtdcvt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\main\sec_ibrs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\ue\sec_iue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\main\sec_iwap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\main\sec_iwapim.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\wim\sec_iwim.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\alg\sec_lib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\main\sec_main.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\aee\sec_mod.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\main\sec_msg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\main\sec_resp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\main\sec_str.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\main\sec_time.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\main\sec_tl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\ue\sec_ue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\ue\sec_ueresp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\wim\sec_wim.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\wim\sec_wimpkcresp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\wim\sec_wimresp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\wim\sec_wimsi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\wim\sec_wimsp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\core\cert\sec_wtlsparser.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\wep\comm\we_libalg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\wep\comm\we_scl.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\inc\isecb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\inc\isecw.h
# End Source File
# Begin Source File

SOURCE=..\..\..\inc\isigntext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_seccb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_seccsc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_secerr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_secevent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_secfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_sechandle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_seclog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_secmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_secpkc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\src\oem\oem_secx509parser.h
# End Source File
# Begin Source File

SOURCE=..\..\..\inc\sec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_asn1.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_ast.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_cd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_cm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_comm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_dp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_ibrs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_iue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_iwap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_iwapim.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_iwim.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_lib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_main.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_mod.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_pubdata.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_resp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_tl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_ue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_uecst.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_ueh.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_ueresp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_wim.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_wimpkcresp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_wimresp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_wimsi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_wimsp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\impl\inc\sec\sec_wtlsparser.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\wep\inc\we_csc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\wep\inc\we_def.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\wep\inc\we_libalg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\wep\inc\we_mem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\wep\inc\we_scl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
