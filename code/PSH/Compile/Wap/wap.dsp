# Microsoft Developer Studio Project File - Name="wap" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=wap - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wap.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wap.mak" CFG="wap - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wap - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wap - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "wap"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wap - Win32 Release"

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
# ADD CPP /nologo /MTd /W3 /GX /O2 /I "$(BREWDIR)\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AEE_SIMULATOR" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "AEE_SIMULATOR" /MTd
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"hellowap.dll"

!ELSEIF  "$(CFG)" == "wap - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\obj"
# PROP Intermediate_Dir "..\..\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "$(BREWDIR)\inc" /I "..\..\Wba\inc" /I "..\..\Sp\inc" /I "..\..\Wbs\inc" /I "..\..\Msa\inc" /I "..\..\Mms\inc" /I "..\..\We\inc" /I "..\..\Pus\inc" /I "..\..\Prs\inc" /I "..\..\Sec\inc" /I "..\..\Sis\inc" /I "..\..\Sia\inc" /I "..\..\Stk\inc" /I "..\..\Uba\inc" /I "..\..\Ubs\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AEE_SIMULATOR" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "AEE_SIMULATOR" /MTd
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"wap.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "wap - Win32 Release"
# Name "wap - Win32 Debug"
# Begin Group "Wba"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Act.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Afi.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Bkm.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Cch.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Cks.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Cmn.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Dlg.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\intgr\Wba_Env.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_File.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\export\Wba_If.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Inse.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Main.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Ofln.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Opt.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Prof.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Prs.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Psl.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\refsrc\Wba_Rc.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Req.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Rpl.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Sec.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Set.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Sif.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Sig.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Slct.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_View.c
# End Source File
# Begin Source File

SOURCE=..\..\Wba\source\Wba_Win.c
# End Source File
# End Group
# Begin Group "Wbs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Wbs\refsrc\Wbs_Char.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Cssm.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Cssp.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Cssv.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Datr.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Ddt.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Dfcf.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Dhtm.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Dimf.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Dleh.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Dlg.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Dmsh.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Doc.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Dwi.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Dwml.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\intgr\Wbs_Env.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Hdef.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Hist.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Html.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Http.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Hvld.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\export\Wbs_If.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Job.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_L2wn.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_L4wn.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lbxt.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lccn.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lcli.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lcpr.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lcss.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lctl.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Ldrw.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Levt.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lfrm.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Limg.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_List.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lmn.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lnav.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lobj.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lpnn.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lrct.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lrec.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lrfl.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lrnd.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lrsc.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lsbr.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Ltbl.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Ltxt.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lutl.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Lvli.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Main.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Memp.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Ofli.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Oflm.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Oflp.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\refsrc\Wbs_Plab.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\refsrc\Wbs_Plg.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Plh.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\refsrc\Wbs_Plti.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\refsrc\Wbs_Pltx.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Prh.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\refsrc\Wbs_Rc.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Sibp.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Sicr.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Sifl.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Sifr.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Sifs.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Sig.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Siin.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Siit.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Sili.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Silt.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Sima.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Simi.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Siop.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Sire.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Sisi.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Sisq.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Siva.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Sivp.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\refsrc\Wbs_Spel.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Str.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Task.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Ua.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Var.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Vist.c
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\source\Wbs_Wml.c
# End Source File
# End Group
# Begin Group "Msa"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Msa\source\Msa_Addr.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_async.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_cmn.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_comm.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_conf.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_core.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_crh.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_cth.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_ctrl.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_del.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\intgr\Msa_Env.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\refsrc\Msa_Int.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_intsig.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_mcr.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_me.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_mem.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_mme.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_mmv.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_mob.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_moh.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_mr.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_mv.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_notif.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_opts.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_par.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_pbh.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_pckg.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_ph.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\refsrc\Msa_Rc.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_rend.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_sch.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_se.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_sig.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_slh.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_srh.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uicmn.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uicols.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uicomm.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uiconf.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uidia.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uiform.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uime.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uimenu.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uimme.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uimmv.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uimob.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uimv.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uipm.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uise.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uisemm.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_uisig.c
# End Source File
# Begin Source File

SOURCE=..\..\Msa\source\Msa_utils.c
# End Source File
# End Group
# Begin Group "Mms"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Mms\source\Fldmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Masync.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mcget.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mcnotif.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mconfig.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mcpdu.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mcpost.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mcsend.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mcwap.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mcwsp.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mhandler.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mlcreate.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mlfetch.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mlfieldc.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mlfieldp.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mlpduc.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mlpdup.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mmain.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mmem.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\refsrc\Mms_A.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mms_Cont.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\intgr\Mms_Env.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\export\Mms_If.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\refsrc\Mms_Rc.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\export\Mmsl_If.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mmsrec.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mmsrpl.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mmsui.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mnotify.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mreceive.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mreport.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Msend.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Msgread.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Msig.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mtimer.c
# End Source File
# Begin Source File

SOURCE=..\..\Mms\source\Mutils.c
# End Source File
# End Group
# Begin Group "Pus"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Pus\source\Pus_Cmmn.c
# End Source File
# Begin Source File

SOURCE=..\..\Pus\source\Pus_Conn.c
# End Source File
# Begin Source File

SOURCE=..\..\Pus\source\Pus_Def.c
# End Source File
# Begin Source File

SOURCE=..\..\Pus\refsrc\Pus_Dlg.c
# End Source File
# Begin Source File

SOURCE=..\..\Pus\intgr\Pus_Env.c
# End Source File
# Begin Source File

SOURCE=..\..\Pus\source\Pus_Hdr.c
# End Source File
# Begin Source File

SOURCE=..\..\Pus\source\Pus_Http.c
# End Source File
# Begin Source File

SOURCE=..\..\Pus\export\Pus_If.c
# End Source File
# Begin Source File

SOURCE=..\..\Pus\source\Pus_Main.c
# End Source File
# Begin Source File

SOURCE=..\..\Pus\source\Pus_Mprt.c
# End Source File
# Begin Source File

SOURCE=..\..\Pus\source\Pus_Msg.c
# End Source File
# Begin Source File

SOURCE=..\..\Pus\source\Pus_Prh.c
# End Source File
# Begin Source File

SOURCE=..\..\Pus\refsrc\Pus_Rc.c
# End Source File
# Begin Source File

SOURCE=..\..\Pus\source\Pus_Sia.c
# End Source File
# Begin Source File

SOURCE=..\..\Pus\source\Pus_Sig.c
# End Source File
# Begin Source File

SOURCE=..\..\Pus\source\Pus_Vld.c
# End Source File
# End Group
# Begin Group "Prs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Prs\refsrc\Prs_A.c
# End Source File
# Begin Source File

SOURCE=..\..\Prs\source\Prs_Auth.c
# End Source File
# Begin Source File

SOURCE=..\..\Prs\source\Prs_Bld.c
# End Source File
# Begin Source File

SOURCE=..\..\Prs\refsrc\Prs_Def.c
# End Source File
# Begin Source File

SOURCE=..\..\Prs\refsrc\Prs_Dlg.c
# End Source File
# Begin Source File

SOURCE=..\..\Prs\intgr\Prs_Env.c
# End Source File
# Begin Source File

SOURCE=..\..\Prs\export\Prs_If.c
# End Source File
# Begin Source File

SOURCE=..\..\Prs\source\Prs_Main.c
# End Source File
# Begin Source File

SOURCE=..\..\Prs\source\Prs_Otab.c
# End Source File
# Begin Source File

SOURCE=..\..\Prs\source\Prs_Otas.c
# End Source File
# Begin Source File

SOURCE=..\..\Prs\source\Prs_Prcs.c
# End Source File
# Begin Source File

SOURCE=..\..\Prs\refsrc\Prs_Rc.c
# End Source File
# Begin Source File

SOURCE=..\..\Prs\source\Prs_Val.c
# End Source File
# Begin Source File

SOURCE=..\..\Prs\source\Prs_Wap.c
# End Source File
# Begin Source File

SOURCE=..\..\Prs\refsrc\Prs_Xtrn.c
# End Source File
# End Group
# Begin Group "We"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\We\we_mgr\source\Frw.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_mgr\source\Frw_Cmmn.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_mgr\refsrc\Frw_Dlg.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_mgr\intgr\Frw_Env.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_mgr\refsrc\Frw_Int.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_mgr\source\Frw_Reg.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_mgr\source\Frw_Sig.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_mgr\source\Frw_Time.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\refsrc\Int_Pipe.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Act.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Afi.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\refsrc\We_Afir.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Asn1.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Chrs.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Chrt.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Cmmn.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Core.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Dcvt.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\refsrc\We_Dlg.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\refsrc\We_Dprc.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\refsrc\We_Drc.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\intgr\We_Env.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\refsrc\We_Ess.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Hdr.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\refsrc\We_Int.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Log.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Mem.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Mime.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Nap.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Pck.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Pdec.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Ptok.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Ptxt.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\refsrc\We_Rc.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\refsrc\We_Sas.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Time.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\export\We_Url.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\refsrc\We_Xchr.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\refsrc\We_Xmim.c
# End Source File
# Begin Source File

SOURCE=..\..\We\we_lib\refsrc\Wml_Inpf.c
# End Source File
# End Group
# Begin Group "Uba"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Uba\source\Uba_Actn.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\source\Uba_Cust.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\source\Uba_Data.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\refsrc\Uba_Del.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\source\Uba_Ditm.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\intgr\Uba_Env.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\source\Uba_Fitm.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\source\Uba_Fold.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\refsrc\Uba_Gui.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\refsrc\Uba_Img.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\refsrc\Uba_Kvc.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\source\Uba_Main.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\source\Uba_Menu.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\refsrc\Uba_Mmsg.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\source\Uba_Page.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\source\Uba_Pitm.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\refsrc\Uba_Push.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\refsrc\Uba_Rc.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\source\Uba_Rce.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\refsrc\Uba_Rfld.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\source\Uba_Sig.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\source\Uba_Text.c
# End Source File
# Begin Source File

SOURCE=..\..\Uba\refsrc\Uba_Vmai.c
# End Source File
# End Group
# Begin Group "Ubs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Ubs\refsrc\Ubs_A.c
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\intgr\Ubs_Env.c
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\source\Ubs_Hash.c
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\export\Ubs_If.c
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\source\Ubs_Job.c
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\source\Ubs_Keyv.c
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\source\Ubs_List.c
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\source\Ubs_Main.c
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\source\Ubs_Msg.c
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\source\Ubs_Msin.c
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\source\Ubs_Mtpy.c
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\source\Ubs_Ndx.c
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\source\Ubs_Sig.c
# End Source File
# End Group
# Begin Group "Sec"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Sec\source\Sec.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Cert.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Ch.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\intgr\Sec_Env.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\export\Sec_If.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Ifim.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Info.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\export\Sec_Lib.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\refsrc\Sec_Rc.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Resp.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Sig.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Sw.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Sw2.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Swa.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Swa2.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Swmn.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Swsh.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\refsrc\Sec_Ti.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Tih.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Utls.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Wim.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_Wtls.c
# End Source File
# Begin Source File

SOURCE=..\..\Sec\source\Sec_X509.c
# End Source File
# End Group
# Begin Group "Stk"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Auth.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Cach.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Cmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Cook.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\refsrc\Stk_Dlg.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\intgr\Stk_Env.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\refsrc\Stk_Fltr.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Hdr.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Hdrc.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Hdrt.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Http.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\export\Stk_If.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Main.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\refsrc\Stk_Rc.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Sig.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Ssl.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Sslh.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Tls.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Tlsc.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Tlsh.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Tlsp.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Tlsv.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Trh.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Wdp.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Wsp.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Wtlc.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Wtle.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Wtlh.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Wtlp.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Wtlr.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Wtls.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Wtlv.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Wtp.c
# End Source File
# Begin Source File

SOURCE=..\..\Stk\source\Stk_Wtps.c
# End Source File
# End Group
# Begin Group "Sis"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Sis\source\Sis_Asy.c
# End Source File
# Begin Source File

SOURCE=..\..\Sis\source\Sis_Cvt.c
# End Source File
# Begin Source File

SOURCE=..\..\Sis\intgr\Sis_Env.c
# End Source File
# Begin Source File

SOURCE=..\..\Sis\export\Sis_If.c
# End Source File
# Begin Source File

SOURCE=..\..\Sis\source\Sis_Isig.c
# End Source File
# Begin Source File

SOURCE=..\..\Sis\source\Sis_Main.c
# End Source File
# Begin Source File

SOURCE=..\..\Sis\source\Sis_Mem.c
# End Source File
# Begin Source File

SOURCE=..\..\Sis\source\Sis_Open.c
# End Source File
# Begin Source File

SOURCE=..\..\Sis\source\Sis_Prse.c
# End Source File
# End Group
# Begin Group "Sia"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Sia\source\Saasync.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Saattach.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Sacore.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Sacrh.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Sadsp.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Saintsig.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Saload.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Samain.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Samem.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Samenu.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Sapckg.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Saph.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Sasig.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Saslide.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Sasls.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Sasmnu.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Satimer.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Saui.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Sauidia.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Sauiform.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Sauisig.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\intgr\Sia_Env.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\export\Sia_If.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\refsrc\Sia_Rc.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Smtrcmn.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Smtrctrl.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Smtrpar.c
# End Source File
# Begin Source File

SOURCE=..\..\Sia\source\Smtrrend.c
# End Source File
# End Group
# Begin Group "Inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\We\inc\AEE.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\AEEStdLib.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Fldmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\Frw.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\Frw_Cmmn.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\Frw_Dlg.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\Frw_Env.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\Frw_Int.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\Frw_Reg.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\Frw_Sig.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\Frw_Time.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\Frw_Vrsn.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\Int_Pipe.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Masync.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mcget.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mcnotif.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mcpdu.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mcpost.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mcsend.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mcwap.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mcwsp.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mhandler.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mlcreate.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mlfetch.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mlfieldc.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mlfieldp.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mlpduc.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mlpdup.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mltypes.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mmain.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mmem.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mms_Cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mms_Cont.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mms_Def.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mms_Env.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mms_If.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mms_Int.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mms_Rc.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mms_Vrsn.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mmsl_If.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mmsrec.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mmsrpl.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mmsui.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mnotify.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mreceive.h
# End Source File
# Begin Source File

SOURCE=..\..\Mms\inc\Mreport.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Addr.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Async.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_cmn.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Comm.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Conf.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Core.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Crh.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Cth.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Ctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Def.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Del.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Env.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Int.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Intsig.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Mcr.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Me.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Mem.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Mme.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Mmv.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Mob.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Moh.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Mr.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Mv.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Notif.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Opts.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Par.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Pbh.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Pckg.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Ph.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Rc.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Rend.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Sch.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Se.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Sig.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Slh.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Srh.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Types.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uicmn.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uicols.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uicomm.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uiconf.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uidia.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uiform.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uime.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uimenu.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uimme.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uimmv.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uimob.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uimv.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uipm.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uise.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uisemm.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Uisig.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Utils.h
# End Source File
# Begin Source File

SOURCE=..\..\Msa\inc\Msa_Vrsn.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Auth.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Bld.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Cnst.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Def.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Dlg.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Env.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Fldt.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_If.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Int.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Main.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Otab.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Otas.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Prcs.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Rc.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Type.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Val.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Vrsn.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Wap.h
# End Source File
# Begin Source File

SOURCE=..\..\Prs\inc\Prs_Xtrn.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Cmmn.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Conn.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Def.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Dlg.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Env.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Hdr.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Http.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_If.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Main.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Mprt.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Msg.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Prh.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Rc.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Sia.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Sig.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Vld.h
# End Source File
# Begin Source File

SOURCE=..\..\Pus\inc\Pus_Vrsn.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Saasync.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Saattach.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sacore.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sacrh.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sadsp.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Saintsig.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Saload.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Samain.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Samem.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Samenu.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sapckg.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Saph.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sasig.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Saslide.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sasls.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sasmnu.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Satimer.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Satypes.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Saui.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sauidia.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sauiform.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sauisig.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Ati.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Awim.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Cert.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Ch.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Cti.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Cwim.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Env.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_If.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Ifim.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Info.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Lib.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Pdf.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Rc.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Resp.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Sig.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Str.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Sw.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Sw2.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Swmn.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Swsh.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Ti.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Utls.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Vrsn.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_Wtls.h
# End Source File
# Begin Source File

SOURCE=..\..\Sec\inc\Sec_X509.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sia_Cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sia_Def.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sia_Env.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sia_If.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sia_Rc.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Sia_Vrsn.h
# End Source File
# Begin Source File

SOURCE=..\..\Sis\inc\Sis_Asy.h
# End Source File
# Begin Source File

SOURCE=..\..\Sis\inc\Sis_Cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\Sis\inc\Sis_Cvt.h
# End Source File
# Begin Source File

SOURCE=..\..\Sis\inc\Sis_Def.h
# End Source File
# Begin Source File

SOURCE=..\..\Sis\inc\Sis_Env.h
# End Source File
# Begin Source File

SOURCE=..\..\Sis\inc\Sis_If.h
# End Source File
# Begin Source File

SOURCE=..\..\Sis\inc\Sis_Isig.h
# End Source File
# Begin Source File

SOURCE=..\..\Sis\inc\Sis_Main.h
# End Source File
# Begin Source File

SOURCE=..\..\Sis\inc\Sis_Mem.h
# End Source File
# Begin Source File

SOURCE=..\..\Sis\inc\Sis_Open.h
# End Source File
# Begin Source File

SOURCE=..\..\Sis\inc\Sis_Prse.h
# End Source File
# Begin Source File

SOURCE=..\..\Sis\inc\Sis_Vrsn.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Smtr.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Smtrcmn.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Smtrctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Smtrpar.h
# End Source File
# Begin Source File

SOURCE=..\..\Sia\inc\Smtrrend.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Auth.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Cach.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Cmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Cook.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Dlg.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Env.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Fltr.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Hdr.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Hdrc.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Http.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_If.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Main.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Rc.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Sig.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Ssld.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Tls.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Tlsd.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Tlsh.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Trdf.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Trh.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Vrsn.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Wdp.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Wsp.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Wtld.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Wtlh.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Wtls.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Wtp.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Wtpd.h
# End Source File
# Begin Source File

SOURCE=..\..\Stk\inc\Stk_Wtps.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Actn.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Cust.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Data.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Del.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Ditm.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Env.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Fitm.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Fold.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Gui.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Img.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Int.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Kvc.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Main.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Menu.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Mmsg.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Page.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Pitm.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Push.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Rc.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Rce.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Rfld.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Sig.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Text.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Vmai.h
# End Source File
# Begin Source File

SOURCE=..\..\Uba\inc\Uba_Vrsn.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_A.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_Cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_Ecfg.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_Env.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_Hash.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_If.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_Job.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_Keyv.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_List.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_Main.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_Msg.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_Msin.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_Mtpy.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_Ndx.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_Sig.h
# End Source File
# Begin Source File

SOURCE=..\..\Ubs\inc\Ubs_Vrsn.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Act.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Afi.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Bkm.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Cch.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Cks.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Cmn.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Dlg.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Env.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_File.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_If.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Inse.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Int.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Main.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Ofln.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Opt.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Prof.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Prs.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Psl.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Rc.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Req.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Rpl.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Sec.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Set.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Sif.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Sig.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Slct.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Str.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_View.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Vrsn.h
# End Source File
# Begin Source File

SOURCE=..\..\Wba\inc\Wba_Win.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Char.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Css.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Cssm.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Datr.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Dcmn.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Ddt.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Dfcf.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Dhtm.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Dimf.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Dleh.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Dlg.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Dmsh.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Doc.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Dwi.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Dwml.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Env.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Hdef.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Hist.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Html.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Http.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Hvld.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_If.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Job.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lbxt.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lccn.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lcli.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lcpr.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lcss.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lctl.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Ldef.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Ldrw.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Levt.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lfrm.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Limg.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_List.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lmn.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lnav.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lobj.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lrct.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lrdf.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lrec.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lrfl.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lrnd.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lrsc.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lsbr.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lsig.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Ltbl.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Ltxt.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lutl.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Lvli.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Main.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Memp.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Ofli.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Oflm.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Oflp.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Plg.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Plh.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Prh.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Rc.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Sibp.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Sicr.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Sifl.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Sifr.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Sifs.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Sig.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Siin.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Siit.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Sili.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Silt.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Sima.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Simi.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Siop.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Sire.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Sisi.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Sisq.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Sity.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Siva.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Spel.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Str.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Task.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Ua.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Var.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Vist.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Vrsn.h
# End Source File
# Begin Source File

SOURCE=..\..\Wbs\inc\Wbs_Wml.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Act.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Afi.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Afir.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Asn1.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Brk.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Chrs.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Chrt.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Cmmn.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Core.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Crpt.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Dcvt.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Def.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Dlg.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Dprc.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Drc.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Env.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Errc.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Ess.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_File.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Hdr.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Int.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Lib.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Log.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Mem.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Mime.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Nap.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Neta.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Pck.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Pdec.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Pipe.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Prsr.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Ptok.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Ptxt.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Rc.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Sas.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Sock.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Tel.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Time.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Url.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Wid.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Xchr.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\We_Xmim.h
# End Source File
# Begin Source File

SOURCE=..\..\We\inc\Wml_Inpf.h
# End Source File
# End Group
# Begin Group "Brew Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\BrewSrc\AEEAppGen.c
# End Source File
# Begin Source File

SOURCE=..\..\BrewSrc\AEEModGen.c
# End Source File
# Begin Source File

SOURCE=.\Wap.c
# End Source File
# End Group
# End Target
# End Project
