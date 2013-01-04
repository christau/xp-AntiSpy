; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CCheckProfileDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "xpantispy2-eng.h"
LastPage=0

ClassCount=21
Class1=CAbout
Class2=CCheckControl
Class3=CCheckList
Class4=CHelpBox
Class5=CHotStatic
Class6=CMediPlayerDlg
Class7=CPerfDlg
Class8=CPropPage
Class9=CProtocol
Class10=CSpecial
Class11=CTimeServerDlg
Class12=CDemoEdit
Class13=CTreeOptionsCtrl
Class14=CTreeOptionsCombo
Class15=CTreeOptionsEdit
Class16=CUserMgmt
Class17=CXPAntiSpyApp
Class18=CXPAntiSpyDlg

ResourceCount=19
Resource1=IDR_MENU1
Resource2=IDD_INIT_PROFILE
Resource3=IDD_TEST_DLG (Neutral (Standard))
Resource4=IDD_XPANTISPY2_DIALOG
Resource5=IDD_XPANTISPY2_DIALOG (Neutral (Standard))
Resource6=IDR_POPUP
Resource7=IDD_USERDEFINED_PROFILES (Neutral (Standard))
Resource8=IDD_SPECIAL_DIALOG (Neutral (Standard))
Resource9=IDR_MENU1 (Neutral (Standard))
Resource10=IDR_POPUP (Neutral (Standard))
Resource11=IDD_TIMESERVERDLG_DIALOG (Neutral (Standard))
Resource12=IDD_xpAntiSpy_DIALOG (Neutral (Standard))
Resource13=IDD_CMDLINE_DLG (Neutral (Standard))
Class19=TestDlg
Resource14=IDD_TCPIP_SYS_DLG (Neutral (Standard))
Class20=LegendDlg
Resource15="IDD_DLG_CHECK_PROFILE1" (Englisch (USA))
Resource16=IDD_LEGEND
Class21=CCheckProfileDlg
Resource17=IDD_ABOUT_DIALOG (Neutral (Standard))
Resource18=IDD_DLG_CHECK_PROFILE1 (Englisch (USA))
Resource19=IDD_DLG_CHECK_PROFILE (Neutral (Standard))

[CLS:CAbout]
Type=0
HeaderFile=\projekte\xp-antispy\src\about.h
ImplementationFile=\projekte\xp-antispy\src\about.cpp
BaseClass=CDialog
LastObject=65535

[CLS:CCheckControl]
Type=0
BaseClass=CWnd
HeaderFile=ChkListBox.h
ImplementationFile=ChkListBox.cpp

[CLS:CCheckList]
Type=0
BaseClass=CWnd
HeaderFile=ChkListBox.h
ImplementationFile=ChkListBox.cpp

[CLS:CHelpBox]
Type=0
BaseClass=CRichEditCtrl
HeaderFile=HelpBox.h
ImplementationFile=HelpBox.cpp
LastObject=CHelpBox
Filter=W
VirtualFilter=WC

[CLS:CHotStatic]
Type=0
BaseClass=CStatic
HeaderFile=HotStatic.h
ImplementationFile=HotStatic.cpp

[CLS:CMediPlayerDlg]
Type=0
BaseClass=CDialog
HeaderFile=MediPlayerDlg.h
ImplementationFile=MediPlayerDlg.cpp

[CLS:CPerfDlg]
Type=0
BaseClass=CDialog
HeaderFile=PerfDlg.h
ImplementationFile=PerfDlg.cpp

[CLS:CPropPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=PropPage.h
ImplementationFile=PropPage.cpp

[CLS:CProtocol]
Type=0
BaseClass=CDialog
HeaderFile=Protocol.h
ImplementationFile=Protocol.cpp

[CLS:CSpecial]
Type=0
BaseClass=CDialog
HeaderFile=Special.h
ImplementationFile=Special.cpp

[CLS:CTimeServerDlg]
Type=0
BaseClass=CDialog
HeaderFile=TimeServerDlg.h
ImplementationFile=TimeServerDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=CTimeServerDlg

[CLS:CDemoEdit]
Type=0
BaseClass=CTreeOptionsEdit
HeaderFile=TreeEditBox.h
ImplementationFile=TreeEditBox.cpp

[CLS:CTreeOptionsCtrl]
Type=0
BaseClass=CTreeCtrl
HeaderFile=TreeOptionsCtrl.h
ImplementationFile=TreeOptionsCtrl.cpp
Filter=T
VirtualFilter=GWC
LastObject=CTreeOptionsCtrl

[CLS:CTreeOptionsCombo]
Type=0
BaseClass=CComboBox
HeaderFile=TreeOptionsCtrl.h
ImplementationFile=TreeOptionsCtrl.cpp
LastObject=CTreeOptionsCombo
Filter=W
VirtualFilter=cWC

[CLS:CTreeOptionsEdit]
Type=0
BaseClass=CEdit
HeaderFile=TreeOptionsCtrl.h
ImplementationFile=TreeOptionsCtrl.cpp

[CLS:CUserMgmt]
Type=0
BaseClass=CDialog
HeaderFile=UserMgmt.h
ImplementationFile=UserMgmt.cpp

[CLS:CXPAntiSpyApp]
Type=0
BaseClass=CWinApp
HeaderFile=XPAntiSpy2.h
ImplementationFile=XPAntiSpy2.cpp

[DLG:IDD_PROTOCOL_DIALOG]
Type=1
Class=CProtocol

[DLG:IDD_USERMGMT_DIALOG]
Type=1
Class=CUserMgmt

[DLG:IDD_XPANTISPY2_DIALOG]
Type=1
Class=CXPAntiSpyDlg
ControlCount=0

[MNU:IDR_MENU1]
Type=1
Class=?
Command1=ID_SPECIAL_CHECKSYSTEMSTATUS
Command2=ID_SPECIAL_RESTOREALLSETTINGS
Command3=ID_SPECIAL_SETRECOMMENDEDSETTINGS
Command4=ID_SPECIAL_ENABLEHIDDENSETTINGS
Command5=ID_SPEZIAL_TIMESERVERFESTLEGEN
Command6=ID_MENU_ABOUT
Command7=ID__VISITTHEXPANTISPYHOMEPAGE
CommandCount=7

[MNU:IDR_POPUP]
Type=1
Class=?
Command1=ID_RESET
Command2=ID_SET
CommandCount=2

[DLG:IDD_XPANTISPY2_DIALOG (Neutral (Standard))]
Type=1
Class=CXPAntiSpyDlg
ControlCount=0

[DLG:IDD_ABOUT_DIALOG (Neutral (Standard))]
Type=1
Class=CAbout
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDC_ABOUT,static,1342308352
Control3=65535,static,1342179342
Control4=IDC_TRANSLATOR,static,1342308353

[DLG:IDD_SPECIAL_DIALOG (Neutral (Standard))]
Type=1
Class=?
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDC_SPECIAL_INFO,static,1342308352
Control3=IDC_STATIC,static,1342177283
Control4=IDC_STATIC,static,1342177283
Control5=IDC_SPECIAL_INFO_GOOD,static,1342308352
Control6=IDC_SPECIAL_INFO_BAD,static,1342308352

[DLG:IDD_TIMESERVERDLG_DIALOG (Neutral (Standard))]
Type=1
Class=?
ControlCount=7
Control1=IDC_OK,button,1342242827
Control2=IDC_COMBO1,combobox,1344339971
Control3=IDC_STATIC,button,1342178055
Control4=IDC_EDIT1,edit,1350631552
Control5=IDC_STATIC,button,1342178055
Control6=IDC_APPLY,button,1342242827
Control7=IDC_ADD,button,1342242827

[MNU:IDR_MENU1 (Neutral (Standard))]
Type=1
Class=?
Command1=ID_PROFILE_NORMAL
Command2=ID_PROFILE_SUGGESTED
Command3=ID_PROFILE_WINDOWS_UPDATE
Command4=ID_PROFILE_SYSTEM_DEFAULT
Command5=ID_PROFILE_AUTOSTART
Command6=ID_USERDEFINED_PROFILES
Command7=ID_SPECIAL_CHECKSYSTEMSTATUS
Command8=ID_SPEZIAL_TIMESERVERFESTLEGEN
Command9=ID_SPEZIAL_VERBINDUNGSLIMIT
Command10=ID_MNU_RUNAS_ADMIN
Command11=ID_MNU_SHOW_SEARCH
Command12=ID_MODE_STANDARD
Command13=ID_MODE_ACCESSIBLE
Command14=ID_MENU_ABOUT
Command15=ID__VISITTHEXPANTISPYHOMEPAGE
Command16=ID_HELPINDEX
Command17=ID__VIEWQUICKLEGEND
CommandCount=17

[MNU:IDR_POPUP (Neutral (Standard))]
Type=1
Class=?
Command1=ID_RESET
Command2=ID_SET
CommandCount=2

[DLG:IDD_TCPIP_SYS_DLG (Neutral (Standard))]
Type=1
Class=?
ControlCount=15
Control1=IDOK,button,1073807371
Control2=IDCANCEL,button,1073807371
Control3=IDC_COMBO,combobox,1075904770
Control4=IDC_BUILD,static,1073872896
Control5=IDC_PATCHED,static,1073872896
Control6=IDC_BTN_RESET,button,1073807371
Control7=IDC_ON,static,1073872896
Control8=IDC_CONNECTIONS,static,1073872896
Control9=IDC_INFO_BOX,button,1073741831
Control10=IDC_MAX_CONNS,static,1073872896
Control11=IDC_STATIC,button,1342177287
Control12=IDC_BACKUP_LIST,listbox,1352728835
Control13=ID_BACKUP_CLEAR,button,1073807371
Control14=ID_BTN_CREATE_BACKUP,button,1073807371
Control15=ID_BACKUP_RESTORE,button,1073807371

[DLG:IDD_USERDEFINED_PROFILES (Neutral (Standard))]
Type=1
Class=?
ControlCount=10
Control1=IDC_COMBO_PROFILES,combobox,1344340227
Control2=IDC_STATIC,static,1342308352
Control3=IDC_DEL_PROFILE,button,1342242827
Control4=IDC_STATIC,button,1342177287
Control5=IDC_DEL_ALL_PROFILES,button,1342242827
Control6=IDC_CREATE_PROFILE,button,1342242827
Control7=IDC_STATIC,button,1342177287
Control8=IDC_EDIT_NAME,edit,1350631552
Control9=IDC_EXPORT_PROFILE,button,1342242827
Control10=IDC_IMPORT_PROFILE,button,1342242827

[DLG:IDD_xpAntiSpy_DIALOG (Neutral (Standard))]
Type=1
Class=?
ControlCount=0

[CLS:TestDlg]
Type=0
HeaderFile=..\src\testdlg.h
ImplementationFile=..\src\testDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=ID__VISITTHEXPANTISPYHOMEPAGE

[DLG:IDD_CMDLINE_DLG (Neutral (Standard))]
Type=1
Class=?
ControlCount=1
Control1=IDC_LIST1,listbox,1345405185

[DLG:IDD_TEST_DLG (Neutral (Standard))]
Type=1
Class=TestDlg
ControlCount=1
Control1=IDOK,button,1342242817

[DLG:IDD_LEGEND]
Type=1
Class=LegendDlg
ControlCount=15
Control1=IDC_STATIC,static,1342177294
Control2=IDC_STATIC,static,1342177294
Control3=IDC_STATIC,static,1342177294
Control4=IDC_STATIC,static,1342177294
Control5=IDC_STATIC,static,1342177294
Control6=IDC_STATIC,static,1342177294
Control7=IDC_STATIC,static,1342177294
Control8=IDC_STATIC,static,1342177294
Control9=IDC_INFO_U,edit,1342183428
Control10=IDC_INFO_UW,edit,1342183428
Control11=IDC_INFO_UC,edit,1342183428
Control12=IDC_INFO_UR,edit,1342183428
Control13=IDC_INFO_LCK,edit,1342183428
Control14=IDC_STATIC,static,1342177294
Control15=IDC_STATIC,static,1342177294

[CLS:LegendDlg]
Type=0
HeaderFile=..\src\legenddlg.h
ImplementationFile=..\src\legenddlg.cpp
BaseClass=CDialog
Filter=D
LastObject=ID__VIEWQUICKLEGEND
VirtualFilter=dWC

[DLG:IDD_INIT_PROFILE]
Type=1
Class=?
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDC_BTN_LATER,button,1342242816
Control3=IDC_BTN_NEVER,button,1342242816
Control4=IDC_TEXT,edit,1342179333

[CLS:CCheckProfileDlg]
Type=0
HeaderFile=..\src\checkprofiledlg.h
ImplementationFile=..\src\checkprofiledlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_LIST
VirtualFilter=dWC

[DLG:IDD_DLG_CHECK_PROFILE (Neutral (Standard))]
Type=1
Class=CCheckProfileDlg
ControlCount=4
Control1=IDOK,button,1342242816
Control2=IDREAPPLY,button,1342242817
Control3=IDC_LIST,listbox,1352728835
Control4=IDC_INFO,static,1342308865

[DLG:IDD_DLG_CHECK_PROFILE1 (Englisch (USA))]
Type=1
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LIST,listbox,1352728835

[DLG:"IDD_DLG_CHECK_PROFILE1" (Englisch (USA))]
Type=1
Class=?
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LIST,listbox,1352728835

