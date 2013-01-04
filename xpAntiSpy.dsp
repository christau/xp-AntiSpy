# Microsoft Developer Studio Project File - Name="xpAntiSpy" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=xpAntiSpy - Win32 Release_Ger
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "xpAntiSpy.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "xpAntiSpy.mak" CFG="xpAntiSpy - Win32 Release_Ger"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "xpAntiSpy - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "xpAntiSpy - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE "xpAntiSpy - Win32 Release_Eng" (basierend auf  "Win32 (x86) Application")
!MESSAGE "xpAntiSpy - Win32 Release_Ger" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xpAntiSpy - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "temp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "temp"
# PROP Intermediate_Dir "temp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"Release/XPAntiSpy3.pch" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /fo"Release/XPAntiSpy3.res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib imagehlp.lib /nologo /subsystem:windows /machine:I386 /out:"Release/XPAntiSpy3.exe"

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "temp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "temp"
# PROP Intermediate_Dir "temp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./language" /I "./res" /I "./src" /I "./lib" /I "./include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "__ENGLISH__" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /fo"Debug/XPAntiSpy3.res" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib imagehlp.lib Srclient.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"Debug/XPAntiSpy3.exe" /pdbtype:sept /libpath:"./lib"
# SUBTRACT LINK32 /incremental:no

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Release_Eng"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "temp"
# PROP BASE Intermediate_Dir "temp"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "temp"
# PROP Intermediate_Dir "temp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"Release/XPAntiSpy3.pch" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "./language" /I "./res" /I "./src" /I "./lib" /I "./include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "__ENGLISH__" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /fo"Release/XPAntiSpy3.res" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /fo"temp/XPAntiSpy3.res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386 /out:"Release/XPAntiSpy3.exe"
# ADD LINK32 version.lib imagehlp.lib Srclient.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"release/en/xp-AntiSpy.exe" /libpath:"./lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Release_Ger"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "temp"
# PROP BASE Intermediate_Dir "temp"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "temp"
# PROP Intermediate_Dir "temp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"Release/XPAntiSpy3.pch" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "./language" /I "./res" /I "./src" /I "./lib" /I "./include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "__GERMAN__" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /fo"Release/XPAntiSpy3.res" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /fo"temp/XPAntiSpy3.res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386 /out:"Release/XPAntiSpy3.exe"
# ADD LINK32 version.lib imagehlp.lib Srclient.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"release/de/xp-AntiSpy.exe" /libpath:"./lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "xpAntiSpy - Win32 Release"
# Name "xpAntiSpy - Win32 Debug"
# Name "xpAntiSpy - Win32 Release_Eng"
# Name "xpAntiSpy - Win32 Release_Ger"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\About.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AntiSpySettings.cpp
# End Source File
# Begin Source File

SOURCE=.\src\checkprofiledlg.cpp
# ADD CPP /I "..\language"
# End Source File
# Begin Source File

SOURCE=.\src\CMDLineDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\coloredit.cpp
# ADD CPP /I "..\language"
# End Source File
# Begin Source File

SOURCE=.\src\Configuration.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConnectionLimitDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConnectionLimitNG.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ExEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\src\HelpBox.cpp
# End Source File
# Begin Source File

SOURCE=.\src\HoverButton.cpp
# End Source File
# Begin Source File

SOURCE=.\src\initprofiledlg.cpp
# ADD CPP /I "..\language"
# End Source File
# Begin Source File

SOURCE=.\src\legenddlg.cpp
# ADD CPP /I "..\language"
# End Source File
# Begin Source File

SOURCE=.\src\MyHyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ProfileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\resource_helper.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ServicesControl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SimpleSplitter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\src\TimeServerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\verinfo.cpp
# End Source File
# Begin Source File

SOURCE=".\language\xpAntiSpy-eng.rc"

!IF  "$(CFG)" == "xpAntiSpy - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Release_Eng"

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Release_Ger"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\language\xpAntiSpy-ger.rc"

!IF  "$(CFG)" == "xpAntiSpy - Win32 Release"

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Debug"

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Release_Eng"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Release_Ger"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\xpAntiSpy.cpp
# End Source File
# Begin Source File

SOURCE=.\src\XPAntiSpyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\XPAntiSpySettingsCtrl.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\About.h
# End Source File
# Begin Source File

SOURCE=.\src\AntiSpySettings.h
# End Source File
# Begin Source File

SOURCE=.\src\checkprofiledlg.h
# End Source File
# Begin Source File

SOURCE=.\src\CMDLineDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\coloredit.h
# End Source File
# Begin Source File

SOURCE=.\src\Configuration.h
# End Source File
# Begin Source File

SOURCE=.\src\connectionlimitdlg.h
# End Source File
# Begin Source File

SOURCE=.\src\ConnectionLimitNG.h
# End Source File
# Begin Source File

SOURCE=.\src\ExEdit.h
# End Source File
# Begin Source File

SOURCE=.\src\GlobalConstants.h
# End Source File
# Begin Source File

SOURCE=.\src\HelpBox.h
# End Source File
# Begin Source File

SOURCE=.\src\HoverButton.h
# End Source File
# Begin Source File

SOURCE=.\src\initprofiledlg.h
# End Source File
# Begin Source File

SOURCE=.\src\iswow64.h
# End Source File
# Begin Source File

SOURCE=.\src\legenddlg.h
# End Source File
# Begin Source File

SOURCE=.\src\MyHyperLink.h
# End Source File
# Begin Source File

SOURCE=.\src\ProfileDlg.h
# End Source File
# Begin Source File

SOURCE=.\language\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\ServicesControl.h
# End Source File
# Begin Source File

SOURCE=.\src\settings_helper.h
# End Source File
# Begin Source File

SOURCE=.\src\SimpleSplitter.h
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\src\TimeServerDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\verinfo.h
# End Source File
# Begin Source File

SOURCE=.\src\xpAntiSpy.h
# End Source File
# Begin Source File

SOURCE=.\src\XPAntiSpyDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\XPAntiSpySettingsCtrl.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\about_logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\alert.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp

!IF  "$(CFG)" == "xpAntiSpy - Win32 Release"

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Debug"

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Release_Eng"

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Release_Ger"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00002.bmp

!IF  "$(CFG)" == "xpAntiSpy - Win32 Release"

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Debug"

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Release_Eng"

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Release_Ger"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn_search_hover.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checked.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checked.ico
# End Source File
# Begin Source File

SOURCE=.\res\checked_big.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checked_change.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checked_change_big.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checked_locked.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checked_locked_big.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checked_right.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checked_right_big.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checked_right_locked.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checked_wrong.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checked_wrong_big.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checked_wrong_locked.bmp
# End Source File
# Begin Source File

SOURCE=.\res\default1.bin
# End Source File
# Begin Source File

SOURCE=.\res\disabled.bmp
# End Source File
# Begin Source File

SOURCE=.\res\divider.bmp
# End Source File
# Begin Source File

SOURCE=.\res\divider_left.bmp
# End Source File
# Begin Source File

SOURCE=.\res\divider_mid.bmp
# End Source File
# Begin Source File

SOURCE=.\res\divider_right.bmp
# End Source File
# Begin Source File

SOURCE=.\res\edit_clear.bmp
# End Source File
# Begin Source File

SOURCE=.\res\edit_clear_hover.bmp
# End Source File
# Begin Source File

SOURCE=.\res\empty.ico
# End Source File
# Begin Source File

SOURCE=.\res\icn_search.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icn_search.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon4.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon5.ico
# End Source File
# Begin Source File

SOURCE=.\res\pinned.bmp
# End Source File
# Begin Source File

SOURCE=.\res\right.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sponsore.bmp

!IF  "$(CFG)" == "xpAntiSpy - Win32 Release"

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Debug"

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Release_Eng"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xpAntiSpy - Win32 Release_Ger"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\tree_ctrl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unchecke.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unchecke.ico
# End Source File
# Begin Source File

SOURCE=.\res\unchecked.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unchecked_big.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unchecked_change.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unchecked_change_big.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unchecked_locked.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unchecked_locked_big.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unchecked_right.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unchecked_right_big.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unchecked_right_locked.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unchecked_wrong.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unchecked_wrong_big.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unchecked_wrong_locked.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unpinned.bmp
# End Source File
# Begin Source File

SOURCE=.\res\white_btn_left.bmp
# End Source File
# Begin Source File

SOURCE=.\res\white_btn_left_sel.bmp
# End Source File
# Begin Source File

SOURCE=.\res\white_btn_middle.bmp
# End Source File
# Begin Source File

SOURCE=.\res\white_btn_middle_sel.bmp
# End Source File
# Begin Source File

SOURCE=.\res\white_btn_right.bmp
# End Source File
# Begin Source File

SOURCE=.\res\white_btn_right_sel.bmp
# End Source File
# Begin Source File

SOURCE=.\res\wrong.bmp
# End Source File
# Begin Source File

SOURCE=.\res\xpAntiSpy.ico
# End Source File
# Begin Source File

SOURCE=.\res\xpAntiSpy.rc2
# End Source File
# Begin Source File

SOURCE=.\res\XPANTISPY16_16.ico
# End Source File
# Begin Source File

SOURCE=.\res\XPANTISPY32_32.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\ctfmon.exe
# End Source File
# Begin Source File

SOURCE=.\res\empty_icon
# End Source File
# Begin Source File

SOURCE="language\..\res\xp-antispy.exe.manifest"
# End Source File
# End Target
# End Project
