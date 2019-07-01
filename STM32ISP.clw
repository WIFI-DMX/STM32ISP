; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CIAP
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "STM32ISP.h"

ClassCount=5
Class1=CSTM32ISPApp
Class2=CSTM32ISPDlg
Class3=CAboutDlg

ResourceCount=7
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_STM32ISP_DIALOG
Resource4=IDD_ABOUTBOX (English (U.S.))
Resource5=IDD_DIALOG_IAP
Class4=CISP
Resource6=IDD_DIALOG_ISP
Class5=CIAP
Resource7=IDD_STM32ISP_DIALOG (English (U.S.))

[CLS:CSTM32ISPApp]
Type=0
HeaderFile=STM32ISP.h
ImplementationFile=STM32ISP.cpp
Filter=N

[CLS:CSTM32ISPDlg]
Type=0
HeaderFile=STM32ISPDlg.h
ImplementationFile=STM32ISPDlg.cpp
Filter=W
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CSTM32ISPDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=STM32ISPDlg.h
ImplementationFile=STM32ISPDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg


[DLG:IDD_STM32ISP_DIALOG]
Type=1
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Class=CSTM32ISPDlg

[DLG:IDD_STM32ISP_DIALOG (English (U.S.))]
Type=1
Class=CSTM32ISPDlg
ControlCount=13
Control1=IDC_STATIC,button,1342177287
Control2=IDC_EDIT_File,edit,1350631552
Control3=IDC_BUTTON_OpenFile,button,1342242816
Control4=IDC_EDIT_OutPut,edit,1352728580
Control5=IDC_COMBO_Port,combobox,1344340226
Control6=IDC_COMBO_Bps,combobox,1344340226
Control7=IDC_STATIC,button,1342177287
Control8=IDC_STATIC,button,1342177287
Control9=IDC_PROGRESS1,msctls_progress32,1350565889
Control10=IDC_CHECK_GO,button,1342242819
Control11=IDC_STATIC,static,1342308352
Control12=IDC_COMBO_DTR,combobox,1344340227
Control13=IDC_TAB1,SysTabControl32,1342177280

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_DIALOG_ISP]
Type=1
Class=CISP
ControlCount=19
Control1=IDC_BUTTON_Start,button,1342242816
Control2=IDC_BUTTON_ReadInfo,button,1342242816
Control3=IDC_BUTTON_EraseChip,button,1342242816
Control4=IDC_BUTTON_ReadFlash,button,1342242816
Control5=IDC_STATIC,button,1342177287
Control6=IDC_CHECK_OptionBytes,button,1342242819
Control7=IDC_STATIC,static,1342308352
Control8=IDC_EDIT_RPD,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT_User,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_EDIT_WRP0,edit,1350631552
Control13=IDC_EDIT_WRP1,edit,1350631552
Control14=IDC_EDIT_WRP2,edit,1350631552
Control15=IDC_EDIT_WRP3,edit,1350631552
Control16=IDC_STATIC,static,1342308352
Control17=IDC_EDIT_Data0,edit,1350631552
Control18=IDC_EDIT_Data1,edit,1350631552
Control19=IDC_EDIT_SAVE,edit,1350631552

[CLS:CISP]
Type=0
HeaderFile=ISP.h
ImplementationFile=ISP.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC

[DLG:IDD_DIALOG_IAP]
Type=1
Class=CIAP
ControlCount=1
Control1=IDC_BUTTON_IAPStart,button,1342242816

[CLS:CIAP]
Type=0
HeaderFile=IAP.h
ImplementationFile=IAP.cpp
BaseClass=CDialog
Filter=D
LastObject=CIAP

