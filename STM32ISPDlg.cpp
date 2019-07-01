// STM32ISPDlg.cpp : implementation file
//
//wifi-dmx.com
#include "stdafx.h"
#include "STM32ISP.h"
#include "STM32ISPDlg.h"
#include "Class\ClassXP.h"
#include "Class\fifo.h"
#include "STMISP.h"
#include "Pcomm.h"
#include "Ini.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BIN_st Update;
int
ADDR_i=0,
Port,
Bps,
Type,
Time,
My_BpsChoose=0;
///////////////////////////////////////////////////////////////////////////////////////////////
// Options Bytes Areas

DWORD ADDR_F1_OPB     = 0x1FFFF800;
DWORD ADDR_F2_OPB     = 0x1FFFC000;
DWORD ADDR_L1_OPB     = 0x1FF80000;
 
// STM32F1 Series
DWORD ADDR_USER_OPB    = 0x1FFFF800;
DWORD ADDR_DATA_OPB    = 0x1FFFF804;
DWORD ADDR_RDP_OPB     = 0x1FFFF800;
DWORD ADDR_WRP0_OPB    = 0x1FFFF808;
DWORD ADDR_WRP2_OPB    = 0x1FFFF80C;

DWORD ADDR_FLASH_SIZE  = 0x1FFFF7E0;

#define WRITE_PAGENUM  256
#define WAIT_Timeout   10
#define Print_Enable   (1)
#define DownLoad_Time  (10)

const DWORD  MAX_DATA_SIZE   = 0xFF;  // Packet size(in byte)
BYTE   ACK                   = 0x79;
BYTE   NACK                  = 0x1F;

ACKS ACK_VALUE          = ST79;
LPTARGET_DESCRIPTOR Target;
ACKS   McuTarget;
DWORD  Progress;
DWORD  ActivityTime;

IAP_Type myiap={
0,0,0,
};
unsigned int ISP_Address = 0x00,StartAddr = 0x00,EndAddr = 0x00,FlashSize=0,Go_Run=1;
//////////////////////////////////////////////////////////////////////////////////////////

VOID CALLBACK TxEmptyIrq(int port);
VOID CALLBACK CntIrq(int port) ;
BOOL HexFileSave(const char *filename, void* lpBuffer, DWORD dwCount);
BOOL HexFileRead(const char *filename, void* lpBuffer,int*Updatelen);
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSTM32ISPDlg dialog

CSTM32ISPDlg::CSTM32ISPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSTM32ISPDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSTM32ISPDlg)
	m_valuefile = _T("E:\\项目\\XY-012A(PRO) 程序\\备份\\XY-012A-V1_13-S.bin");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSTM32ISPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSTM32ISPDlg)
	DDX_Control(pDX, IDC_TAB1, m_tab1);
	DDX_Control(pDX, IDC_COMBO_DTR, m_ctrldtr);
	DDX_Control(pDX, IDC_EDIT_File, m_ctrlfile);
	DDX_Control(pDX, IDC_COMBO_Port, m_ctrlport);
	DDX_Control(pDX, IDC_COMBO_Bps, m_ctrlbps);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlprogress);
	DDX_Control(pDX, IDC_EDIT_OutPut, m_output);
	DDX_Text(pDX, IDC_EDIT_File, m_valuefile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSTM32ISPDlg, CDialog)
	//{{AFX_MSG_MAP(CSTM32ISPDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_OpenFile, OnBUTTONOpenFile)
	ON_BN_CLICKED(IDC_CHECK_GO, OnCheckGo)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INFO, OnMyInfo)
	ON_MESSAGE(WM_PCOMM, OnPcomm) //这里是消息影射
	ON_MESSAGE(WM_PCOMMSEND, SendPcomm) //这里是消息影射
	ON_MESSAGE(WM_START_KEYDOWM, OnStartKeydowm)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSTM32ISPDlg message handlers

BOOL CSTM32ISPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	ClassXP();

	State = 1314;

    CRect Rect ;
    GetClientRect(&Rect);
	old.x=Rect.right-Rect.left;
	old.y=Rect.bottom-Rect.top;

	ScanPort();
	m_ctrlbps.InsertString(0, "115200");
	m_ctrlbps.InsertString(1, "250000");
	m_ctrlbps.SetCurSel(0);

	FifoInit(&MyFifo);
/*
	CFont m_font;
	m_font.CreateFont(16,0,0,0,FW_BOLD, 0,0,0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");
	m_ctrlstart.SetFont(&m_font);
*/

	m_output.GetWindowRect(&Rect);
	ScreenToClient(&Rect);

	OldTLPoint[0] = Rect.TopLeft();
	OldBRPoint[0] = Rect.BottomRight();

	Rect.SetRect(OldTLPoint[0],OldBRPoint[0]);
	m_output.MoveWindow(Rect,TRUE);

	
	m_ctrlprogress.GetWindowRect(&Rect);
	ScreenToClient(&Rect);

	OldTLPoint[1] = Rect.TopLeft();
	OldBRPoint[1] = Rect.BottomRight();

	Rect.SetRect(OldTLPoint[1],OldBRPoint[1]);
	m_ctrlprogress.MoveWindow(Rect,TRUE);

	GetFile();

	m_ctrldtr.SetCurSel(0);


	m_tab1.InsertItem(0,_T("ISP"));
	m_tab1.InsertItem(1,_T("IAP"));
	m_myisp.Create(IDD_DIALOG_ISP,GetDlgItem(IDC_TAB1));
	m_myiap.Create(IDD_DIALOG_IAP,GetDlgItem(IDC_TAB1));

	CRect rs;
	m_tab1.GetClientRect(&rs);
    rs.top+=20;
	rs.bottom-=0;
	rs.left+=0;
	rs.right-=0;

    m_myisp.MoveWindow(&rs);
	m_myiap.MoveWindow(&rs);
	m_myisp.ShowWindow(true);
	m_myiap.ShowWindow(false);
	m_tab1.SetCurSel(0);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSTM32ISPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSTM32ISPDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSTM32ISPDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSTM32ISPDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if(State!=1314)return;
	// TODO: Add your message handler code here
    if((nType == SIZE_RESTORED)||(nType == SIZE_MAXIMIZED)){
		
		CRect Recta,Rect;
		float fsp[2];
		POINT Newp;
		GetClientRect(&Recta);
		Newp.x=Recta.right-Recta.left;
	    Newp.y=Recta.bottom-Recta.top;

		fsp[0]=(float)Newp.x/old.x;
		fsp[1]=(float)Newp.y/old.y;
		
		CString a;
		a.Format("fsp[0]: %f fsp[1]: %f\r\n", fsp[0],fsp[1]);
		//m_output.ReplaceSel(a);

		if((fsp[0]<1)||(fsp[1]<1))return;

		m_output.GetWindowRect(&Rect);
		ScreenToClient(&Rect);

		CPoint TLPoint[3];//左上
		CPoint BRPoint[3];//右下

		TLPoint[0].x= long(OldTLPoint[0].x*1);
		TLPoint[0].y= long(OldTLPoint[0].y*1);
		BRPoint[0].x= long(OldBRPoint[0].x*fsp[0]);
		BRPoint[0].y= long(OldBRPoint[0].y*fsp[1]);
		Rect.SetRect(TLPoint[0],BRPoint[0]);

		m_output.MoveWindow(Rect,TRUE);

		TLPoint[1].x= long(OldTLPoint[1].x*1);
		TLPoint[1].y= long(OldTLPoint[1].y*fsp[1]);
		BRPoint[1].x= long(OldBRPoint[1].x*fsp[0]);
		BRPoint[1].y= long(OldBRPoint[1].y*fsp[1]);
		Rect.SetRect(TLPoint[1],BRPoint[1]);

		m_ctrlprogress.MoveWindow(Rect,TRUE);        //old=Newp;
		
		//m_output.MoveWindow(203+102,460+22,Rect.Width()-(203+102)-8,Rect.Height()-(460+22)-7-54,TRUE);
	}	
}

void CSTM32ISPDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	Time++;
/*
    CString a;
	a.Format("\r\nConnectting ... %d ",Time);
	m_output.SetSel(28,-1);
	m_output.ReplaceSel(a);

	 
    volatile unsigned char UDR;
    if(MyFifo.count){
			if(FifoOut(&MyFifo,&UDR)==FifoNormal){
                CString a;
			    a.Format("%02X ",(unsigned char)UDR);
	   	        m_output.ReplaceSel(a);
			}
	}
*/
	STBL_Init_BL();
	CDialog::OnTimer(nIDEvent);
}


void CSTM32ISPDlg::ScanPort()
{
	CStringArray arry_str_COM;
	for(int i=0;i<GetSerialPort(arry_str_COM);i++)
	{
		((CComboBox *)GetDlgItem(IDC_COMBO_Port))->InsertString(i,arry_str_COM.GetAt(i));
	}
	((CComboBox *)GetDlgItem(IDC_COMBO_Port))->SetCurSel(0);
}

int CSTM32ISPDlg::GetSerialPort(CStringArray &arrCom)
{
		//CStringArray ary;
		arrCom.RemoveAll(); 
		HKEY   hkey;     
		LONG   lRes   =   RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T( "HARDWARE\\DEVICEMAP\\SERIALCOMM"), NULL, KEY_QUERY_VALUE   |   KEY_ENUMERATE_SUB_KEYS   |   KEY_READ,   &hkey);     
		if   (lRes   ==   ERROR_SUCCESS)     
		{ 
			TCHAR   tchKey[MAX_PATH];     
			TCHAR   tchValue[20]; 
			DWORD   dwIndex   =   0; 
			DWORD   dwType   =   REG_SZ; 
			while(lRes   ==   ERROR_SUCCESS) 
			{ 
				DWORD   dwCount   =   MAX_PATH; 
				DWORD   dwVCount   =   20; 
				lRes   =   RegEnumValue(hkey,   dwIndex++,   tchKey,   &dwCount,   NULL,   &dwType,   (LPBYTE)tchValue,   &dwVCount); 
				if(lRes   ==   ERROR_SUCCESS) 
				{ 
					if((dwVCount   >   0)   &&   (dwCount   >   0)) 
					{ 
						arrCom.Add(tchValue);
						//ary.Add(tchValue);
					} 
				} 
			} 
		}
		RegCloseKey(hkey); 
		
		
	return   arrCom.GetSize();
}

/*******************************************************************************************/
/* Function    : FileExist                                                                 */
/* IN          : file name                                                                 */
/* OUT         : boolean                                                                   */
/* Description : verify if the given file exists                                           */
/*******************************************************************************************/
BOOL CSTM32ISPDlg::FileExist(LPCTSTR filename)
{
	// Data structure for FindFirstFile
	WIN32_FIND_DATA findData;

	// Clear find structure
	ZeroMemory(&findData, sizeof(findData));

	// Search the file
	HANDLE hFind = FindFirstFile( filename, &findData );
	if ( hFind == INVALID_HANDLE_VALUE )
	{
	// File not found
	return false;
	}

	// File found

	// Release find handle
	FindClose( hFind );
	hFind = NULL;

	// The file exists
	return true;
}

void CSTM32ISPDlg::ChangeFile(void) 
{
	
	CString Path(AfxGetApp()->m_pszHelpFilePath);
	char Drive[3], Dir[256], Fname[256], Ext[256];
    _splitpath(Path,Drive,Dir,Fname,Ext);
	
	CString ConfFile;
	ConfFile.Format("%s%s%s", Drive, Dir , "Default.conf");

	CIni Ini((LPCSTR)ConfFile);

	UpdateData(TRUE);
	Ini.WriteString((LPCTSTR)"File",(LPCTSTR)"Name", (LPCTSTR)m_valuefile);
	UpdateData(FALSE);

	// TODO: Add your control notification handler code here	
}

void CSTM32ISPDlg::GetFile(void) 
{

	CString Path(AfxGetApp()->m_pszHelpFilePath);
	char Drive[3], Dir[256], Fname[256], Ext[256];
    _splitpath(Path,Drive,Dir,Fname,Ext);
	
	CString ConfFile;
	ConfFile.Format("%s%s%s", Drive, Dir , "Default.conf");

    //m_output.ReplaceSel(ConfFile);

	if(!FileExist((LPCTSTR)ConfFile))
	{
		FILE* fp = fopen((LPCTSTR)ConfFile, "a+");//file %s does not exist .. Creating file
		if(fp)fclose(fp);
		m_valuefile="";
		UpdateData(TRUE);
		return;
	}

	CIni Ini((LPCSTR)ConfFile);

	UpdateData(TRUE);
	m_valuefile=(LPCTSTR) Ini.GetString((LPCTSTR)"File",(LPCTSTR)"Name", "0");
	Go_Run=Ini.GetBool((LPCTSTR)"CMD",(LPCTSTR)"Go_Run", TRUE);
	UpdateData(FALSE);

	if(Go_Run){
		((CButton*)GetDlgItem(IDC_CHECK_GO))->SetCheck(TRUE);
	}
	else{
		((CButton*)GetDlgItem(IDC_CHECK_GO))->SetCheck(FALSE);
	}
	// TODO: Add your control notification handler code here	
}

void CSTM32ISPDlg::OnBUTTONOpenFile() 
{
	// TODO: Add your control notification handler code here
	_TCHAR Drive[3], Dir[256], Fname[256], Ext[256];
	_TCHAR *ptr;
	BOOL bRet=FALSE;
	
	OPENFILENAME ofn;
	char szFile[MAX_PATH];
	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = TEXT('\0');
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("(*.bin;*.hex)\0*.bin;*.hex\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_EXPLORER |OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_READONLY;
	ofn.hwndOwner = ::AfxGetMainWnd()->m_hWnd;

    m_ctrlfile.SetSel(0,0);
    m_ctrlfile.SetSel(-1);
    m_ctrlfile.SetSel(0,-1);
    m_ctrlfile.ReplaceSel("");

	if (GetOpenFileName(&ofn))
	{
		//::MessageBox(NULL,szFile,"",MB_OK);
		m_ctrlfile.SetReadOnly(TRUE);//将光标移到开头
		m_ctrlfile.SetSel(0,0);//将光标移到末尾
		m_ctrlfile.SetSel(-1);//选中所有内容
		m_ctrlfile.SetSel(0,-1);//写入值

		//m_ctrlfile.ReplaceSel("Position: ");
		m_ctrlfile.ReplaceSel(szFile);
		//m_ctrlfile.ReplaceSel("\r\n");

		UpdateData(TRUE);
		_splitpath(szFile, Drive, Dir, Fname, Ext);
		ptr=strupr(Ext);
		strcpy(Ext, ptr);

		if (strcmp(Ext, ".BIN")==0)
			bRet=LoadBIN(szFile);
		else
		if (strcmp(Ext, ".HEX")==0) 
			bRet=HexFileRead(szFile,Update.Update_Buffer,&Update.UpdateLen);

		if(bRet)
		{
			CString a;
			a.Format("Size: %d byte\r\n", Update.UpdateLen);
			m_output.SetSel(-1);
			m_output.ReplaceSel(a);
			m_output.SetSel(0, 0);
			ChangeFile();
		}
		else{
			::MessageBox(NULL,"Fail to Open File","",MB_OK);  
		}
/*
	    int i;
		FILE *fp = fopen(szFile, "rb");
		if(NULL == fp)     
		{         
			::MessageBox(NULL,"Fail to Open File","",MB_OK);    
		} 
		else
		{
			fseek(fp, 0, SEEK_END);											//把指针移动到文件的结尾 ，获取文件长度					
			Update.UpdateLen = ftell(fp);									//获取文件长度
			rewind(fp);														//把指针移动到文件开头 因为我们一开始把指针移动到结尾，如果不移动回来 会出错	
		
			//int i = fread(Update.Update_Buffer, 1, Update.UpdateLen, fp);	//读文件
			i = fread(Update.Update_Buffer, sizeof(char), Update.UpdateLen, fp);

			fclose(fp);
			fp = NULL;

			CString a;
			a.Format("Size: %d byte\r\n", Update.UpdateLen);
			m_output.SetSel(-1);
			m_output.ReplaceSel(a);
			m_output.SetSel(0, 0);

			//m_Update_Progress.SetRange32(0, (BANK3_WRITE_END_ADDR-BANK1_WRITE_START_ADDR)); 
	       //m_Update_Progress.SetPos(0); 
		}
*/
	}
}


void CSTM32ISPDlg::OnStartKeydowm(WPARAM wParam, LPARAM lParam) 
{

	// TODO: Add your control notification handler code here
	_TCHAR Drive[3], Dir[256], Fname[256], Ext[256];
	_TCHAR *ptr;
	BOOL bRet=FALSE;
	CString str_tmp,sz_val;
	int Length=0;
	UpdateData(TRUE);

	//AfxMessageBox(_T("1....."));

	_splitpath(m_valuefile, Drive, Dir, Fname, Ext);
	ptr=strupr(Ext);
	strcpy(Ext, ptr);

	if (strcmp(Ext, ".BIN")==0)
		bRet=LoadBIN(m_valuefile);
	else
	if (strcmp(Ext, ".HEX")==0) 
		bRet=HexFileRead(m_valuefile,Update.Update_Buffer,&Update.UpdateLen);

	if(bRet)
	{
		CString a;
		a.Format("Size: %d byte\r\n", Update.UpdateLen);
		m_output.SetSel(-1);
		m_output.ReplaceSel(a);
		m_output.SetSel(0, 0);
	}
	else{
		Update.UpdateLen=0;
		m_output.SetSel(-1);
		m_output.ReplaceSel("Fail to Open File! \r\n");
		m_output.SetSel(0, 0);
		return;
	}
	
	//取端口号
	GetDlgItemText(IDC_COMBO_Port,str_tmp);
	Length=str_tmp.GetLength();
	if(Length<5)
	sz_val=str_tmp.Right(str_tmp.Find("COM")+1);
	else if(Length<6)
	sz_val=str_tmp.Right(str_tmp.Find("COM")+2);
	else if(Length<7)
	sz_val=str_tmp.Right(str_tmp.Find("COM")+3);

	Port = atoi(sz_val.GetBuffer(MAX_PATH));

	
	if(m_ctrlbps.GetCurSel())
	{
		My_BpsChoose = 250000L;
	}
	else{
		My_BpsChoose = 115200L;
	}
	//sio_baud ( Port, My_BpsChoose );

	CString a;
	m_myisp.GetDlgItemText(IDC_BUTTON_Start,a);
	
	if(!strcmp(a,"Start"))
	{
		if(SIO_OK!=sio_open(Port))
		{
			m_output.SetSel(-1);
			m_output.ReplaceSel("Error! No This comport Or In using! \r\n");
			m_output.SetSel(0, 0);
		}
		else
		{
			sio_ioctl(Port,B115200,BIT_8 | STOP_1 | P_EVEN);
			if(My_BpsChoose == 250000L)sio_baud ( Port, My_BpsChoose );

			sio_cnt_irq(Port,CntIrq,1);
			m_myisp.SetDlgItemText(IDC_BUTTON_Start,"Stop");

			m_ctrlport.EnableWindow(FALSE); 
			m_ctrlbps.EnableWindow(FALSE);

			Time=0;
			//m_Update_Progress.SetRange32(0, (BANK3_WRITE_END_ADDR-BANK1_WRITE_START_ADDR)); 
			//m_Update_Progress.SetPos(0);

			CString a;
		    a.Format("Status: COM%d Opened %d \r\n",Port,My_BpsChoose);
			m_output.SetSel(0,-1);
			m_output.ReplaceSel(a);
			myiap.Times=0;
			myiap.Writelength=0;
			myiap.ReceiveNum=0;

			SetTimer(1,DownLoad_Time,NULL);//时间为100毫秒

			myiap.SYSstate = SYSIAP_CONNECT;

			m_ctrlprogress.SetRange32(0, Update.UpdateLen); 
			m_ctrlprogress.SetPos(0);

		}
	}
	else 
	{
		KillTimer(1);
		sio_close(Port);

		m_myisp.SetDlgItemText(IDC_BUTTON_Start,"Start");
		m_ctrlport.EnableWindow(TRUE); 
		m_ctrlbps.EnableWindow(TRUE);
		CString a;
		a.Format("\r\nStatus: COM%d Closed \r\n",Port);
	    m_output.SetSel(-1);
		m_output.ReplaceSel(a);

		Port = -1;
	}	

}

///////////////////////////串口中断回调函数//////////////////////////////////
VOID CALLBACK TxEmptyIrq(int port)
{
	if(::AfxGetMainWnd()) 
	{
		if(::AfxGetMainWnd()->m_hWnd)
		{
			::PostMessage(::AfxGetMainWnd()->m_hWnd,WM_PCOMMSEND,0,0); 
		}
	}
}

VOID CALLBACK CntIrq(int port) 
{
	if(::AfxGetMainWnd()) 
	{
		if(::AfxGetMainWnd()->m_hWnd)
		{
			::PostMessage(::AfxGetMainWnd()->m_hWnd,WM_PCOMM,0,0); 
		}
	}
}

void CSTM32ISPDlg::OnMyInfo(WPARAM pParam,LPARAM IParam)
{
	if(0==pParam)
	{

	}
	else
	{

	}
}

void CSTM32ISPDlg::OnPcomm()
{

	char buf[1024];
	int end=sio_read(Port,buf,1024);
	if(end)
	{
		for(int i=0;i<end;i++)
		{
			FifoIn(&MyFifo,buf[i]);
		}			
	}
	
}
void CSTM32ISPDlg::SendPcomm()
{
	/*
static char i=0;
	sio_putch(Port,i++);
	if(i==0)
*/

}


BYTE CSTM32ISPDlg::STBL_Init_BL()
{	
	volatile unsigned short RX_Buffer[255];
	volatile unsigned char TX_Buffer[255];
	int i;
	static int j=0;
#if Stm32_Enable
	static unsigned char state=0,k=15;
	if(state!=myiap.SYSstate){
		state=myiap.SYSstate;
		//k=k+1;if(k>15)k=0;
		LCM_PrintsTemp(k,0,(unsigned char*)&Number_Isp[(state)% 100 / 10]);
		LCM_PrintsTemp(k,1,(unsigned char*)&Number_Isp[(state)%10]);
	}
#endif	
#if(Print_Enable)
    //CString c;
	//c.Format("State:%02d \r\n",(unsigned char)myiap.SYSstate);
	//m_output.ReplaceSel(c);
#endif
	switch(myiap.SYSstate){
	    case SYSIAP_CONNECT:     
		myiap.Num=0;
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
#if(Print_Enable)
                CString a;
			    a.Format("%02X \r\n",(unsigned char)RX_Buffer[myiap.Num]);
	   	        m_output.ReplaceSel(a);
#endif
				if((RX_Buffer[myiap.Num]==0x79)||(RX_Buffer[myiap.Num]==0x1F)){
					myiap.ReceiveNum++;
					if(myiap.ReceiveNum>3){
						myiap.SYSstate = SYSIAP_Getversion;	
						while(MyFifo.count){FifoOut(&MyFifo,&RX_Buffer[myiap.Num]);}
						myiap.ReceiveNum=0;
					}
					return SUCCESS_IAP;
				}
				else{
					myiap.ReceiveNum=0;
				}
			}
		}
		{
			TX_Buffer[0] = INIT_CON; 
			sio_write(Port,(char*)&TX_Buffer,1);
			myiap.Times++;
#if(Print_Enable)
			CString a;
			m_output.SetSel(20,-1);
			a.Format("\r\nConnectting...%d ",myiap.Times);
	   	    m_output.ReplaceSel(a);
#endif
#if LCD_Enable
	if(myiap.Times%10==0)
	LCM_PrintsTemp(12,3,(unsigned char*)&Number_Isp[((myiap.Times%100)*10)/(100)]);
#endif
		}
		myiap.Timeout=0;
		ISP_Address = StartAddr ;
		break;
		case SYSIAP_Getversion://15
		{
			TX_Buffer[0] = GET_CMD;
			TX_Buffer[1] = ~TX_Buffer[0];
			sio_write(Port,(char*)&TX_Buffer,2);
			myiap.SYSstate = SYSIAP_Getversion_ACK;
			myiap.Num=0;
		}
		case SYSIAP_Getversion_ACK:
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
#if(Print_Enable)
                CString a;
			    a.Format("%02X ",(unsigned char)RX_Buffer[myiap.Num]);
	   	        m_output.ReplaceSel(a);
#endif
				if((RX_Buffer[0]==0x79)&&(RX_Buffer[14]==0x79)){			
					myiap.SYSstate = SYSIAP_Getprotect;
					myiap.Num=0;
					myiap.Timeout=0;
					while(MyFifo.count){FifoOut(&MyFifo,&RX_Buffer[myiap.Num]);}
#if(Print_Enable)
	   	        m_output.ReplaceSel("\r\n");
#endif
					return SUCCESS_IAP;
				}
				else{
				
				}
				myiap.Num++;
			}
		}
		if(myiap.Timeout++>WAIT_Timeout)myiap.SYSstate = SYSIAP_CONNECT;
		break;
		case SYSIAP_Getprotect://5
		{
			TX_Buffer[0] = GET_VER_ROPS_CMD;
			TX_Buffer[1] = ~TX_Buffer[0];
			sio_write(Port,(char*)&TX_Buffer,2);
			myiap.SYSstate = SYSIAP_Getprotect_ACK;
			myiap.Num=0;
		}
		case SYSIAP_Getprotect_ACK:
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
#if(Print_Enable) 
				CString a;
			    a.Format("%02X ",(unsigned char)RX_Buffer[myiap.Num]);
	   	        m_output.ReplaceSel(a);
#endif
				if((RX_Buffer[0]==0x79)&&(RX_Buffer[4]==0x79)){
					myiap.SYSstate = SYSIAP_GetchipID;
					myiap.Num=0;
					myiap.Timeout=0;
					while(MyFifo.count){FifoOut(&MyFifo,&RX_Buffer[myiap.Num]);}
#if(Print_Enable)
	   				CString b;
					b.Format("\r\nBootLoader Version:%d.%d\r\n",((unsigned char)RX_Buffer[1])/16,((unsigned char)RX_Buffer[1])%16);
	   				m_output.ReplaceSel(b);
#endif
					return SUCCESS_IAP;
				}
				else{
				
				}
				myiap.Num++;
			}
		}
		if(myiap.Timeout++>WAIT_Timeout)myiap.SYSstate = SYSIAP_CONNECT;
		break;
		case SYSIAP_GetchipID://5
		{
			TX_Buffer[0] = GET_ID_CMD;
			TX_Buffer[1] = ~TX_Buffer[0];
			sio_write(Port,(char*)&TX_Buffer,2);
			myiap.SYSstate = SYSIAP_GetchipID_ACK;
			myiap.Num=0;
		}
		case SYSIAP_GetchipID_ACK:
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
#if(Print_Enable) 
				CString a;
			    a.Format("%02X ",(unsigned char)RX_Buffer[myiap.Num]);
	   	        m_output.ReplaceSel(a);
#endif
				if((RX_Buffer[0]==0x79)&&(RX_Buffer[4]==0x79)){
					myiap.SYSstate = SYSIAP_READSize_CMD;
					myiap.Num=0;
					myiap.Timeout=0;
					while(MyFifo.count){FifoOut(&MyFifo,&RX_Buffer[myiap.Num]);}
#if(Print_Enable)
	   	            DWORD PID = 0x00000000;
					PID=(((unsigned char)RX_Buffer[2])<<8)|((unsigned char)RX_Buffer[3]);
					CString b;
					b.Format("\r\nChip PID:0x%08x \r\n",PID);
	   				m_output.ReplaceSel(b);
#endif
					return SUCCESS_IAP;
				}
				else{
				
				}
				myiap.Num++;
			}
		}
		if(myiap.Timeout++>WAIT_Timeout)myiap.SYSstate = SYSIAP_CONNECT;
		break;
		case SYSIAP_READSize_CMD://1
		myiap.Num=0;
		{
			TX_Buffer[0] = READ_CMD;
			TX_Buffer[1] = ~TX_Buffer[0];
			sio_write(Port,(char*)&TX_Buffer,2);
			myiap.SYSstate = SYSIAP_READSize_CMD_ACK;
			myiap.Num=0;
		}
		case SYSIAP_READSize_CMD_ACK:
		myiap.Num=0;
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
#if(Print_Enable)
                CString a;
			    a.Format("%02X ",(unsigned char)RX_Buffer[myiap.Num]);
	   	        m_output.ReplaceSel(a);
#endif
				if(RX_Buffer[0]==0x79){
					myiap.SYSstate = SYSIAP_READSize_ADDR;
					myiap.Num=0;
					myiap.Timeout=0;
					while(MyFifo.count){FifoOut(&MyFifo,&RX_Buffer[myiap.Num]);}
#if(Print_Enable)
	   	        m_output.ReplaceSel("\r\n");
#endif					
				    return SUCCESS_IAP;
				}
				else{

#if(Print_Enable)
					m_output.ReplaceSel("Read OptionBytes Fail\r\n");
#endif
					myiap.SYSstate = SYSIAP_SETunprotectREADOUT;
				}				
			}
		}
		if(myiap.Timeout++>WAIT_Timeout)myiap.SYSstate = SYSIAP_CONNECT;
		break;
		case SYSIAP_READSize_ADDR:
		{
			BYTE Checksum = 0x00;
			DWORD Read_address = ADDR_FLASH_SIZE;

            TX_Buffer[0] = (BYTE)((Read_address >> 24) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[0];
			TX_Buffer[1] = (BYTE)((Read_address >> 16) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[1];
			TX_Buffer[2] = (BYTE)((Read_address >> 8 ) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[2];
			TX_Buffer[3] = (BYTE)((Read_address      ) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[3];
			TX_Buffer[4] = Checksum;

			sio_write(Port,(char*)&TX_Buffer,5);
			myiap.SYSstate = SYSIAP_READSize_ADDR_ACK;
			myiap.Num=0;
		}
		case SYSIAP_READSize_ADDR_ACK:
		myiap.Num=0;
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
#if(Print_Enable)
                CString a;
			    a.Format("%02X ",(unsigned char)RX_Buffer[myiap.Num]);
	   	        m_output.ReplaceSel(a);
#endif
				if(RX_Buffer[0]==0x79){
					myiap.SYSstate = SYSIAP_READSize_Num;
					myiap.Num=0;
					myiap.Timeout=0;
					while(MyFifo.count){FifoOut(&MyFifo,&RX_Buffer[myiap.Num]);}
#if(Print_Enable)
					m_output.ReplaceSel("\r\n");
#endif
					return SUCCESS_IAP;
				}
				else{
#if(Print_Enable)
					m_output.ReplaceSel("Read OptionBytes Fail\r\n");
#endif
					myiap.SYSstate = SYSIAP_CONNECT;
				}				
			}
		}
		if(myiap.Timeout++>WAIT_Timeout)myiap.SYSstate = SYSIAP_CONNECT;
		break;
		case SYSIAP_READSize_Num:	
		{
			BYTE Read_length = 20;

			// send data size to be read
			TX_Buffer[0] = Read_length-1;
			TX_Buffer[1] = ~(Read_length-1);

			sio_write(Port,(char*)&TX_Buffer,2);
			myiap.SYSstate = SYSIAP_READSize_Num_ACK;
			myiap.Num=0;
		}
		case SYSIAP_READSize_Num_ACK:
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
#if(Print_Enable)
                CString a;
			    a.Format("%02X ",(unsigned char)RX_Buffer[myiap.Num]);
	   	        m_output.ReplaceSel(a);
#endif
				if((RX_Buffer[0]==0x79)&&(myiap.Num>=20)){
					myiap.SYSstate = SYSIAP_READ_CMD;
					myiap.Num=0;
					myiap.Timeout=0;				
					FlashSize=((unsigned char)RX_Buffer[1])|(((unsigned char)RX_Buffer[2])<<8);
					if((unsigned int)Update.UpdateLen>FlashSize*1024)myiap.SYSstate = SYSIAP_WRITE_STOP;
#if(Print_Enable)
					CString b;
					b.Format("\r\nFlash Size:%dKB \r\n",FlashSize);
	   				m_output.ReplaceSel(b);
#endif
					while(MyFifo.count){FifoOut(&MyFifo,&RX_Buffer[myiap.Num]);}
					return SUCCESS_IAP;
				}
				else{
					//myiap.SYSstate = SYSIAP_CONNECT;
				}				
			}
		    myiap.Num++;
		}
		if(myiap.Timeout++>WAIT_Timeout)myiap.SYSstate = SYSIAP_CONNECT;
		break;
		case SYSIAP_READ_CMD://1
		myiap.Num=0;
		{
			TX_Buffer[0] = READ_CMD;
			TX_Buffer[1] = ~TX_Buffer[0];
			sio_write(Port,(char*)&TX_Buffer,2);
			myiap.SYSstate = SYSIAP_READ_CMD_ACK;
			myiap.Num=0;
		}
		case SYSIAP_READ_CMD_ACK:
		myiap.Num=0;
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
#if(Print_Enable)
                CString a;
			    a.Format("%02X ",(unsigned char)RX_Buffer[myiap.Num]);
	   	        m_output.ReplaceSel(a);
#endif
				if(RX_Buffer[0]==0x79){
					myiap.SYSstate = SYSIAP_READ_ADDR;
					myiap.Num=0;
					myiap.Timeout=0;
					while(MyFifo.count){FifoOut(&MyFifo,&RX_Buffer[myiap.Num]);}
#if(Print_Enable)
	   	        m_output.ReplaceSel("\r\n");
#endif					
				    return SUCCESS_IAP;
				}
				else{

#if(Print_Enable)
					m_output.ReplaceSel("Read OptionBytes Fail\r\n");
#endif
					myiap.SYSstate = SYSIAP_SETunprotectREADOUT;
				}				
			}
		}
		if(myiap.Timeout++>WAIT_Timeout)myiap.SYSstate = SYSIAP_CONNECT;
		break;
		case SYSIAP_READ_ADDR:
		{
			BYTE Checksum = 0x00;
			DWORD Read_address = 0x1FFFF800;

            TX_Buffer[0] = (BYTE)((Read_address >> 24) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[0];
			TX_Buffer[1] = (BYTE)((Read_address >> 16) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[1];
			TX_Buffer[2] = (BYTE)((Read_address >> 8 ) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[2];
			TX_Buffer[3] = (BYTE)((Read_address      ) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[3];
			TX_Buffer[4] = Checksum;

			sio_write(Port,(char*)&TX_Buffer,5);
			myiap.SYSstate = SYSIAP_READ_ADDR_ACK;
			myiap.Num=0;
		}
		case SYSIAP_READ_ADDR_ACK:
		myiap.Num=0;
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
#if(Print_Enable)
                CString a;
			    a.Format("%02X ",(unsigned char)RX_Buffer[myiap.Num]);
	   	        m_output.ReplaceSel(a);
#endif
				if(RX_Buffer[0]==0x79){
					myiap.SYSstate = SYSIAP_READ_Num;
					myiap.Num=0;
					myiap.Timeout=0;
					while(MyFifo.count){FifoOut(&MyFifo,&RX_Buffer[myiap.Num]);}
#if(Print_Enable)
					m_output.ReplaceSel("\r\n");
#endif
					return SUCCESS_IAP;
				}
				else{
#if(Print_Enable)
					m_output.ReplaceSel("Read OptionBytes Fail\r\n");
#endif
					myiap.SYSstate = SYSIAP_CONNECT;
				}				
			}
		}
		if(myiap.Timeout++>WAIT_Timeout)myiap.SYSstate = SYSIAP_CONNECT;
		break;
		case SYSIAP_READ_Num:	
		{
			BYTE Read_length = 16;

			// send data size to be read
			TX_Buffer[0] = Read_length-1;
			TX_Buffer[1] = ~(Read_length-1);

			sio_write(Port,(char*)&TX_Buffer,2);
			myiap.SYSstate = SYSIAP_READ_Num_ACK;
			myiap.Num=0;
		}
		case SYSIAP_READ_Num_ACK:
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
#if(Print_Enable)
                CString a;
			    a.Format("%02X ",(unsigned char)RX_Buffer[myiap.Num]);
	   	        m_output.ReplaceSel(a);
#endif
				if((RX_Buffer[0]==0x79)&&(myiap.Num>=16)){
					myiap.SYSstate = SYSIAP_ERASE_CMD;
					myiap.Num=0;
					myiap.Timeout=0;
					while(MyFifo.count){FifoOut(&MyFifo,&RX_Buffer[myiap.Num]);}
#if(Print_Enable)
					m_output.ReplaceSel("\r\n");
#endif
					return SUCCESS_IAP;
				}
				else{
				
				}				
			}
		    myiap.Num++;
		}
		if(myiap.Timeout++>WAIT_Timeout)myiap.SYSstate = SYSIAP_CONNECT;
		break;
		case SYSIAP_SETunprotectWrite:
		{
			TX_Buffer[0] = WRITE_TEMP_UNPROTECT_CMD;
			TX_Buffer[1] = ~TX_Buffer[0];
			sio_write(Port,(char*)&TX_Buffer,2);
			myiap.SYSstate = SYSIAP_SETunprotectWrite_ACK;
			myiap.Num=0;
		}
		case SYSIAP_SETunprotectWrite_ACK:
		myiap.Num=0;
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
#if(Print_Enable)
                CString a;
			    a.Format("%02X ",(unsigned char)RX_Buffer[myiap.Num]);
	   	        m_output.ReplaceSel(a);
#endif
				if((RX_Buffer[0]==0x79)&&(RX_Buffer[1]==0x79)){
					myiap.SYSstate = SYSIAP_CONNECT;
					myiap.Num=0;
					myiap.Timeout=0;
					while(MyFifo.count){FifoOut(&MyFifo,&RX_Buffer[myiap.Num]);}
#if(Print_Enable)
					m_output.ReplaceSel("\r\n");
#endif
					return SUCCESS_IAP;
				}
				else{
				}				
			}
			myiap.Num++;
		}
		if(myiap.Timeout++>WAIT_Timeout)myiap.SYSstate = SYSIAP_CONNECT;
		break;
		case SYSIAP_SETunprotectREADOUT:	
		{
			TX_Buffer[0] = READOUT_PERM_UNPROTECT_CMD;
			TX_Buffer[1] = ~TX_Buffer[0];
			sio_write(Port,(char*)&TX_Buffer,2);
			myiap.Timeout=0;
			myiap.SYSstate=SYSIAP_SETunprotectREADOUT_ACK;
			myiap.Num=0;
		}
		case SYSIAP_SETunprotectREADOUT_ACK:
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
#if(Print_Enable) 
				CString a;
			    a.Format("%02X ",(unsigned char)RX_Buffer[myiap.Num]);
	   	        m_output.ReplaceSel(a);
#endif
				if((RX_Buffer[0]==0x79)&&(RX_Buffer[1]==0x79)){
					myiap.SYSstate = SYSIAP_CONNECT;
					myiap.Num=0;
					myiap.Timeout=0;
					while(MyFifo.count){FifoOut(&MyFifo,&RX_Buffer[myiap.Num]);}
#if(Print_Enable)
					m_output.ReplaceSel("Read unprotect Succeed\r\n");
#endif
					return SUCCESS_IAP;
				}
				else{
				}				
			}
			myiap.Num++;
		}
		if(myiap.Timeout++>WAIT_Timeout)myiap.SYSstate = SYSIAP_CONNECT;
		break;
		case SYSIAP_ERASE_CMD:		
		{
			TX_Buffer[0] = ERASE_CMD;
			TX_Buffer[1] = ~TX_Buffer[0];
			sio_write(Port,(char*)&TX_Buffer,2);
			myiap.SYSstate = SYSIAP_ERASE_CMD_ACK;
			myiap.Num=0;
		}
		case SYSIAP_ERASE_CMD_ACK:
		myiap.Num=0;
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
#if(Print_Enable)
                CString a;
			    a.Format("%02X ",(unsigned char)RX_Buffer[myiap.Num]);
	   	        m_output.ReplaceSel(a);
#endif
				if(RX_Buffer[0]==0x79){
					myiap.SYSstate = SYSIAP_ERASE_ALL;
					myiap.Num=0;
					myiap.Timeout=0;
					while(MyFifo.count){FifoOut(&MyFifo,&RX_Buffer[myiap.Num]);}
#if(Print_Enable)
					m_output.ReplaceSel("\r\n");
#endif
					return SUCCESS_IAP;
				}
				else{
#if(Print_Enable)
					m_output.ReplaceSel("Erase Fail\r\n");
#endif
					myiap.SYSstate = SYSIAP_CONNECT;
				}				
			}
		}
		if(myiap.Timeout++>WAIT_Timeout)myiap.SYSstate = SYSIAP_CONNECT;
		break;
		case SYSIAP_ERASE_ALL:		
		{
			TX_Buffer[0] = 0xFF;
			TX_Buffer[1] = ~TX_Buffer[0];
			sio_write(Port,(char*)&TX_Buffer,2);		
			myiap.SYSstate=SYSIAP_ERASE_ALL_ACK;
			myiap.Num=0;
		}
		case SYSIAP_ERASE_ALL_ACK:
		myiap.Num=0;
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
#if(Print_Enable) 
				CString a;
			    a.Format("%02X ",(unsigned char)RX_Buffer[myiap.Num]);
	   	        m_output.ReplaceSel(a);
#endif
				if(RX_Buffer[0]==0x79){
					myiap.SYSstate = SYSIAP_WRITE_CMD;
					myiap.Num=0;
					myiap.Timeout=0;
					while(MyFifo.count){FifoOut(&MyFifo,&RX_Buffer[myiap.Num]);}
#if(Print_Enable) 
					m_output.ReplaceSel("\r\n");
#endif
					SetTimer(1,1,NULL);
					return SUCCESS_IAP;
				}
				else{
#if(Print_Enable) 
					m_output.ReplaceSel("Erase all Fail\r\n");
#endif
					myiap.SYSstate = SYSIAP_CONNECT;
				}				
			}
		}
		if(myiap.Timeout++>WAIT_Timeout)myiap.SYSstate = SYSIAP_CONNECT;
		break;
////////////////////////////////////////////////////////////////////////////////////////////////
		case SYSIAP_WRITE_CMD:
		myiap.Num=0;
		{
			TX_Buffer[0] = WRITE_CMD;
			TX_Buffer[1] = ~TX_Buffer[0];
			sio_write(Port,(char*)&TX_Buffer,2);
			myiap.SYSstate = SYSIAP_WRITE_CMD_ACK;
			myiap.Num=0;
		}
		case SYSIAP_WRITE_CMD_ACK:
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
				if(RX_Buffer[0]==0x79){
					myiap.SYSstate = SYSIAP_WRITE_ADDR;
					myiap.Num=0;
					myiap.Timeout=0;
					//SetTimer(1,DownLoad_Time,NULL);
					return SUCCESS_IAP;
				}
				else{
					myiap.SYSstate = SYSIAP_CONNECT;
					SetTimer(1,DownLoad_Time,NULL);
					return ERROR_OFFSET;
				}				
			}
		}
		if(myiap.Timeout++>WAIT_Timeout){myiap.SYSstate = SYSIAP_CONNECT;SetTimer(1,DownLoad_Time,NULL);return ERROR_OFFSET;}
		break;
		case SYSIAP_WRITE_ADDR:
		myiap.Num=0;
		{
			int Checksum = 0x00;
			DWORD Write_address = 0x08000000+(WRITE_PAGENUM*myiap.Writelength);

            TX_Buffer[0] = (BYTE)((Write_address >> 24) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[0];
			TX_Buffer[1] = (BYTE)((Write_address >> 16) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[1];
			TX_Buffer[2] = (BYTE)((Write_address >> 8 ) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[2];
			TX_Buffer[3] = (BYTE)((Write_address      ) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[3];
			TX_Buffer[4] = Checksum;

			sio_write(Port,(char*)&TX_Buffer,5);
			myiap.SYSstate = SYSIAP_WRITE_ADDR_ACK;
			myiap.Num=0;
		}
		case SYSIAP_WRITE_ADDR_ACK:
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
#if(Print_Enable) 
				if(myiap.Writelength%8==0)
				{
					CString a;
					a.Format("%02X %X \r\n",(unsigned char)RX_Buffer[myiap.Num],0x08000000+(WRITE_PAGENUM*myiap.Writelength));
	   				m_output.ReplaceSel(a);
				}
#endif
				if(RX_Buffer[0]==0x79){
					myiap.SYSstate = SYSIAP_WRITE_DATA;
					myiap.Num=0;
					myiap.Timeout=0;
					//SetTimer(1,DownLoad_Time,NULL);
					return SUCCESS_IAP;
				}
				else{
					SetTimer(1,DownLoad_Time,NULL);
					myiap.SYSstate = SYSIAP_CONNECT;
				}				
			}
		}
		if(myiap.Timeout++>WAIT_Timeout){myiap.SYSstate = SYSIAP_CONNECT;SetTimer(1,DownLoad_Time,NULL);return ERROR_OFFSET;}
		break;
		case SYSIAP_WRITE_DATA:
		{
			int Checksum = 0x00;
			int Write_length = WRITE_PAGENUM;

			if((myiap.Writelength+1)*WRITE_PAGENUM>(unsigned int)Update.UpdateLen){
				Write_length = (((myiap.Writelength+1)*WRITE_PAGENUM-Update.UpdateLen*4/4));
				if(WRITE_PAGENUM > Write_length)Write_length = WRITE_PAGENUM - Write_length;
#if(Print_Enable) 
				CString a;
			    a.Format("length:%d ",Write_length);
	   	        m_output.ReplaceSel(a);
#endif
			}

            TX_Buffer[0] = Write_length-1; Checksum = TX_Buffer[0];
#if(Stm32_Enable)
			for (i = 0; i< Write_length; i=i+4){
				
                TX_Buffer[i+1]=(uint8_t)((*(__IO uint32_t*) ISP_Address)>>0);
				Checksum = Checksum ^TX_Buffer[i+1]; 
				TX_Buffer[i+2]=(uint8_t)((*(__IO uint32_t*) ISP_Address)>>8);
				Checksum = Checksum ^TX_Buffer[i+2]; 
				TX_Buffer[i+3]=(uint8_t)((*(__IO uint32_t*) ISP_Address)>>16);
				Checksum = Checksum ^TX_Buffer[i+3]; 
				TX_Buffer[i+4]=(uint8_t)((*(__IO uint32_t*) ISP_Address)>>24);
				Checksum = Checksum ^TX_Buffer[i+4]; 
				ISP_Address	=ISP_Address+4;
			}
	#if LCD_Enable
			if(ISP_Address%FLASH_PAGE_SIZE==0)LCM_Frame(((ISP_Address-StartAddr)*100)/(EndAddr-StartAddr));
	#endif
#else
			for (i = 0; i< Write_length; i++){
				Checksum = Checksum ^ Update.Update_Buffer[(myiap.Writelength*WRITE_PAGENUM)+i];//Update.UpdateLen
                TX_Buffer[i+1]=Update.Update_Buffer[(myiap.Writelength*WRITE_PAGENUM)+i];
			
			}
#endif
			TX_Buffer[Write_length+1] = Checksum;

			sio_write(Port,(char*)&TX_Buffer,Write_length+2);
#if(Print_Enable) 
			m_ctrlprogress.SetPos((myiap.Writelength+1)*WRITE_PAGENUM);
#endif
			myiap.SYSstate = SYSIAP_WRITE_DATA_ACK;
			myiap.Num=0;
		}
		case SYSIAP_WRITE_DATA_ACK:
		myiap.Num=0;
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
				if(RX_Buffer[0]==0x79){
					myiap.Writelength++;
					if(myiap.Writelength*WRITE_PAGENUM > (unsigned int)Update.UpdateLen){
						if(Go_Run){
							myiap.SYSstate = SYSIAP_GO_CMD;
						}
						else{
							myiap.SYSstate = SYSIAP_WRITE_FINISH;
						}
									
						SetTimer(1,DownLoad_Time,NULL);
					}
					else{
					    myiap.SYSstate = SYSIAP_WRITE_CMD;
					}
					myiap.Timeout=0;
					myiap.Num=0;				
					return SUCCESS_IAP;
				}
				else{
					SetTimer(1,DownLoad_Time,NULL);
					myiap.SYSstate = SYSIAP_CONNECT;
				}				
			}
		}
		if(myiap.Timeout++>WAIT_Timeout)myiap.SYSstate = SYSIAP_CONNECT;
		break;
		case SYSIAP_GO_CMD:
		myiap.Num=0;
		{
			TX_Buffer[0] = GO_CMD;
			TX_Buffer[1] = ~TX_Buffer[0];
			sio_write(Port,(char*)&TX_Buffer,2);
			myiap.SYSstate = SYSIAP_GO_CMD_ACK;
			myiap.Num=0;
		}
		case SYSIAP_GO_CMD_ACK:
		while(MyFifo.count){
			if(FifoOut(&MyFifo,&RX_Buffer[myiap.Num])==FifoNormal){
				if(RX_Buffer[0]==0x79){
					myiap.SYSstate = SYSIAP_GO_ADDR;
					myiap.Num=0;
					myiap.Timeout=0;
					//SetTimer(1,DownLoad_Time,NULL);
					return SUCCESS_IAP;
				}
				else{
					myiap.SYSstate = SYSIAP_GO_ADDR;
					SetTimer(1,DownLoad_Time,NULL);
					return ERROR_OFFSET;
				}				
			}
		}
		if(myiap.Timeout++>WAIT_Timeout){myiap.SYSstate = SYSIAP_GO_ADDR;SetTimer(1,DownLoad_Time,NULL);return ERROR_OFFSET;}
		break;
		case SYSIAP_GO_ADDR:
		myiap.Num=0;
		{
			int Checksum = 0x00;
			DWORD Go_address = 0x08000000;

            TX_Buffer[0] = (BYTE)((Go_address >> 24) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[0];
			TX_Buffer[1] = (BYTE)((Go_address >> 16) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[1];
			TX_Buffer[2] = (BYTE)((Go_address >> 8 ) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[2];
			TX_Buffer[3] = (BYTE)((Go_address      ) & 0x000000FF); Checksum= Checksum ^ TX_Buffer[3];
			TX_Buffer[4] = Checksum;

			sio_write(Port,(char*)&TX_Buffer,5);
			myiap.SYSstate = SYSIAP_GO_ADDR_ACK;
			myiap.Num=0;
		}
		case SYSIAP_GO_ADDR_ACK:
			myiap.SYSstate = SYSIAP_WRITE_FINISH;
		break;
		case SYSIAP_WRITE_FINISH:
#if(Print_Enable) 
			m_output.ReplaceSel("Write data Finish \r\n");
#endif
#if LCD_Enable
			LCM_Frame(100);	
			LCM_Prints(10,1,"OK");
#endif	
            myiap.SYSstate = SYSIAP_WRITE_STOP;
			myiap.Timeout=0;
		break;
		case SYSIAP_WRITE_STOP:
		OnStartKeydowm(0,0);
		break;
		default:break;
	}
#if Stm32_Enable
    if(j++%10==0){LED4_Port->ODR ^= LED4_Pin;}
#endif	 
	return UNREOGNIZED_DEVICE;
}



BOOL CSTM32ISPDlg::LoadBIN(CString pFilePath)
{
	CFile File;
    CFileException ex;
	if (File.Open(/*(LPCSTR)*/pFilePath, CFile::modeRead, &ex))
	{
		IMAGEELEMENT Element;
		Element.dwDataLength=File.SeekToEnd();
		File.SeekToBegin();

		Element.dwAddress = 0x08000000;
		Element.Data=new BYTE[Element.dwDataLength];
		//File.Read(Element.Data, Element.dwDataLength);
		Update.UpdateLen= Element.dwDataLength;
		File.Read(Update.Update_Buffer, Update.UpdateLen);
		File.Close();
		/*
		if (!SetImageElement(0, TRUE, Element))
		{
			AfxMessageBox("Unable to insert element in the image...");
			return FALSE;
		}
		*/
		free(Element.Data);
	}
	else return FALSE;

	return TRUE;
}

BOOL CSTM32ISPDlg::LoadHEX(CString pFilePath)
{
	IMAGEELEMENT Element;//, *pPrevElement;
	BOOL bRet= TRUE ;
	FILE *fp;
//	BOOL bConcatenate;
	Update.UpdateLen=0;

	fp = _tfopen(pFilePath, _T("r"));
	if (!fp)
		bRet=FALSE;
	else
	{
		unsigned long target_address=0,                 /* address of array index */
					  base_address=0,
					  extended_address=0,
					  address=0,                        /* offset/address from hexfile */
					  i=0,                              /* counterindex */
					  checksum=0,                       /* checksum from hexfile */
					  byte_count=0,                     /* bytes per line in hexfile */
					  sum_var=0;                        /* to calculate checksum */
		              
		unsigned long separator='0',                    /* separator string in hexfile */
					  character='0',                    /* date from hexfile */
					  colon ='0';                       /* begin of line in hexfile */
		unsigned short lineno=0;
		BOOL last_byte = FALSE;						    /* conversion  end/begin */

		//UNICODE char message[255];		// error message
		CString message;


		do {
			sum_var = 0;                          /* checksum calculation begin */
			fscanf(fp,"%1c",&colon);
			//fscanf_s(fp, "%1c", &colon, sizeof(colon));
			if (feof(fp))  { // Detect End Of File & Exit if S9 or S8 or S7 record not found
				last_byte = TRUE;
			}
			else 
			if (colon == ':') {                   /* do only if intel hexfile */			
				fscanf(fp,"%2x",&byte_count);
				//fscanf_s(fp, "%2x", &byte_count, sizeof(byte_count));
				sum_var += byte_count;
				fscanf(fp,"%4x",&address);
				//fscanf_s(fp, "%4x", &address, sizeof(address));
				fscanf(fp,"%2x",&separator);
				//fscanf_s(fp, "%2x", &separator, sizeof(separator));
				if (separator== 0x00)
				{		
					//if(byte_count!=12)Update.UpdateLen=byte_count;
					target_address = (extended_address<<16) + (base_address<<4) + address;
					sum_var = sum_var + (address >> 8) + (address % 256);
					Element.dwAddress=target_address;
					Element.dwDataLength=byte_count;
					Element.Data=(PBYTE)malloc(Element.dwDataLength);
					for (i=0; i < byte_count; i++) {
						fscanf(fp,"%2x",&character);
						//fscanf_s(fp,"%2x",&character, sizeof(character));
						sum_var += character;
						Element.Data[i] = (unsigned char)character;
						Update.Update_Buffer[Update.UpdateLen++]= (unsigned char)character;
					}
					sum_var += separator;
					fscanf(fp,"%2x",&checksum);
					//fscanf_s(fp,"%2x",&checksum, sizeof(checksum));
					sum_var = (sum_var % 256);
					if (((checksum + sum_var) % 256) != 0) {
						free(Element.Data);
						//UNICODE wsprintf(message, "FILE : line %i: Bad hexadecimal checksum!", lineno);
						message.Format(_T("FILE : line %i: Bad hexadecimal checksum!"), lineno);
						LDisplayError(message);
						bRet=FALSE;
						//break;
					}
					// The Element is correct. Check if this element is contiguous with this one. In this case we'll not
					// create a new element but concatenate data
					/*
					bConcatenate=FALSE;
					if (GetNbElements()!=0)
					{
						pPrevElement=(PIMAGEELEMENT)m_pElements->GetAt(GetNbElements()-1);
						if (pPrevElement->dwAddress+pPrevElement->dwDataLength==Element.dwAddress)
							bConcatenate=TRUE;
						else
							bConcatenate=FALSE;
					}
					
					if (!bConcatenate)
						SetImageElement(GetNbElements(), TRUE, Element);
					else
					{
						pPrevElement->Data=(PBYTE)realloc(pPrevElement->Data, pPrevElement->dwDataLength+Element.dwDataLength);
						memcpy(pPrevElement->Data+pPrevElement->dwDataLength, Element.Data, Element.dwDataLength);
						pPrevElement->dwDataLength=pPrevElement->dwDataLength+Element.dwDataLength;
					}
					*/
					free(Element.Data);
				}
				else
				if (separator== 0x01)
				{
					sum_var = sum_var + (address >> 8) + (address % 256) + separator;
					sum_var = (sum_var % 256);
					fscanf(fp,"%2x",&checksum);
					//fscanf_s(fp,"%2x",&checksum, sizeof(checksum));
					if (((checksum + sum_var) % 256) != 0) {
						//UNICODE wsprintf(message, "FILE : line %i: Bad hexadecimal checksum!", lineno);
						message.Format(_T("FILE : line %i: Bad hexadecimal checksum!"), lineno);
						LDisplayError(message);
						bRet=FALSE;
						break;
					}
					else {
						last_byte = TRUE;         /* eof */
					}
				}
				else
				if (separator==0x02)
				{
					fscanf(fp,"%4x",&base_address);
					//fscanf_s(fp,"%4x",&base_address,sizeof(base_address));
					fscanf(fp,"%2x",&checksum);
					//fscanf_s(fp, "%2x", &checksum, sizeof(checksum));
					sum_var = sum_var + (address >> 8) + (address % 256) + separator \
									  + (base_address >> 8) + (base_address % 256);
					sum_var = (sum_var % 256);
					if (((checksum + sum_var) % 256) != 0) {
						//wsprintf(message, "FILE : line %i: Bad hexadecimal checksum!", lineno);
						message.Format(_T("FILE : line %i: Bad hexadecimal checksum!"), lineno);
						LDisplayError(message);
						bRet=FALSE;
						break;
					}
				}
				else
				if (separator==0x03)
				{
					sum_var = sum_var + (address >> 8) + (address % 256) + separator;
					for (i=0; i < byte_count; i++) {
						fscanf(fp,"%2x",&character);
						//fscanf_s(fp, "%2x", &character, sizeof(character));
						sum_var += character;
					}
					fscanf(fp, "%2x", &checksum);
					//fscanf_s(fp, "%2x", &checksum, sizeof(checksum));
					sum_var = (sum_var % 256);
					if (((checksum + sum_var) % 256) != 0) {
						//wsprintf(message, "FILE : line %i: Bad hexadecimal checksum!", lineno);
						message.Format(_T("FILE : line %i: Bad hexadecimal checksum!"), lineno);
						LDisplayError(message);
						bRet=FALSE;
						break;
					}
				}
				else
				if (separator==0x04)
				{
					fscanf(fp, "%4x", &extended_address);
					//fscanf_s(fp, "%4x", &extended_address, sizeof (extended_address));
					fscanf(fp,"%2x",&checksum);
					//fscanf_s(fp,"%2x",&checksum,sizeof (checksum));
					sum_var = sum_var + (address >> 8) + (address % 256) + separator \
							 + (extended_address >> 8) + (extended_address % 256);
					sum_var = (sum_var % 256);
					if (((checksum + sum_var) % 256) != 0) {
						//wsprintf(message, "FILE : line %i: Bad hexadecimal checksum!", lineno);
						message.Format(_T("FILE : line %i: Bad hexadecimal checksum!"), lineno);
						LDisplayError(message);
						bRet=FALSE;
						break;
					}
				}
				else
				if (separator==0x05)
				{
					sum_var = sum_var + (address >> 8) + (address % 256) + separator;
					for (i=0; i < byte_count; i++) {
						fscanf(fp,"%2x",&character);
						//fscanf_s(fp, "%2x", &character, sizeof(character));
						sum_var += character;
					}
					fscanf(fp, "%2x", &checksum);
					//fscanf_s(fp, "%2x", &checksum, sizeof(checksum));
					sum_var = (sum_var % 256);
					if (((checksum + sum_var) % 256) != 0) {
						//wsprintf(message, "FILE : line %i: Bad hexadecimal checksum!", lineno);
						message.Format(_T("FILE : line %i: Bad hexadecimal checksum!"), lineno);
						LDisplayError(message);
						bRet=FALSE;
						break;
					}
				}
				else
				{
					//wsprintf(message, "FILE : line %i: Not in Intel Hex format!", lineno);
					message.Format(_T("FILE : line %i: Not in Intel Hex format!"), lineno);
				    LDisplayError(message);
					bRet=FALSE;
					break;
				}
			}
			else if ((colon == '\r') || (colon == '\n')) { // Skip CR:0x0D & LF:0x0A Characters
				lineno++; // increment number of line
				if (feof(fp))  { // Detect End Of File & Exit if last record not found
					last_byte = TRUE;         /* eof */
				}
			}
			else if (colon == ' ') { // Skip Space ' ' Characters
				if (feof(fp))  { // Detect End Of File & Exit if last record not found
					last_byte = TRUE;         /* eof */
				}
			}
			else {
				//wsprintf(message, "FILE : line %i: Not in Intel Hex format!", lineno);
				message.Format(_T("FILE : line %i: Not in Intel Hex format!"), lineno);
				LDisplayError(message);
				bRet=FALSE;
				break;
			}
		}
		while (!last_byte);

		fclose(fp);
	}

	//if (bRet)
	//	CompactElements();

	return bRet;
}

//------------------------------------------------------------------------------
//     * 00 - Data record
//     * 01 - End of file record
//     * 02 - Extended segment address record
//     * 03 - Start segment address record
//     * 04 - Extended linear address record
//     * 05 - Start linear address record
//------------------------------------------------------------------------------

BOOL HexFileRead(const char *filename, void* lpBuffer,int*UpdateLen)
{
	char FileLine[1024];
	char Codedata;
	char* CodeAddr;
	char FirstBassState=0,FirstStartState=0;
	int  FirstBassAddress=0,FirstStartAddr=0;
	unsigned char sun=0,check=0;
	int  iByteCount,iStartAddr,iRecordType,i,BassAddress=0;
	
	FILE *file;
	file = fopen(filename,"r");
	CodeAddr=(char*)lpBuffer;

	while(!feof(file))//检测文件结束符号
	{
		//: + 长度 + 地址 + 类型 + N个数据(N >= 0) + 校验
		//1 + 2    + 4    + 2    + N * 2           + 2
		fscanf(file,"%1c%2x%4x%2x",&Codedata,&iByteCount,&iStartAddr,&iRecordType);
		if(Codedata!=':')
		{
			fscanf(file,"%s\r\n",FileLine);
			continue;
		}

		if(iRecordType==0)
		{
			sun=iByteCount+(unsigned char)iStartAddr+(unsigned char)(iStartAddr>>8)+iRecordType;
			if(FirstStartState==0){FirstStartAddr=iStartAddr;FirstStartState+=1;}//首次记录
			if(iByteCount==0)
			{
				fscanf(file,"%s\r\n",FileLine);
				break;
			}
			for(i=0;i<iByteCount;i++)
			{
				fscanf(file,"%2x",&Codedata);
				*(CodeAddr + iStartAddr + i + BassAddress-FirstStartAddr) = Codedata;
				sun+=Codedata;
			}
			*UpdateLen=(iStartAddr+iByteCount+BassAddress-FirstStartAddr);
			fscanf(file,"%2x\r\n",&check);
			if((unsigned char)check!=(unsigned char)(0x100-sun))
			{
				fclose(file);
				return FALSE;
			}
			sun=0;
		}
		else if(iRecordType==1)  //End of File
		{
			fclose(file);
			return TRUE;
		}
		else if(iRecordType==2) // base address
		{
			if(FirstBassState==0)
			{
				fscanf(file,"%4x",&FirstBassAddress);
				FirstBassAddress <<= 16;
				FirstBassState=1;
			}
			else{
				FirstBassState=2;
				fscanf(file,"%4x",&BassAddress);
				BassAddress <<= 16;
				BassAddress=BassAddress-FirstBassAddress;
			}
			
			fscanf(file,"%s\r\n",FileLine);
			continue;
		}
		else if(iRecordType==4) // upper address
		{
			if(FirstBassState==0)
			{
				fscanf(file,"%4x",&FirstBassAddress);
				FirstBassAddress <<= 16;
				FirstBassState=1;
			}
			else
			{
				fscanf(file,"%4x",&BassAddress);
				BassAddress <<= 16;
				FirstBassState=2;
				BassAddress=BassAddress-FirstBassAddress;
			}
			
			fscanf(file,"%s\r\n",FileLine);
			continue;
		}
		else if(iRecordType==3) // nothing to do
		{
			fscanf(file,"%s\r\n",FileLine);
			continue;
		}
		else
		{
			fclose(file);
			return FALSE;
		}
	}
	fclose(file);
	return FALSE;
}

BOOL HexFileSave(const char *filename, void* lpBuffer, DWORD dwCount)
{
	CFile OutFile(filename,CFile::modeWrite|CFile::modeCreate);
	
	char* DataRead;
	DataRead = (char*)lpBuffer;
	BYTE VerifyCode=0;
	BYTE Code;
	CString str;
	CString strData = "";
	
	if(dwCount > 0x10000) //for max address over 0xFFFF
	{
		strData += ":020000040000FA\r\n";
	}
	for(DWORD i=0;i<dwCount;i+=0x20)
	{
		strData += ":20";       //line start
		str.Format("%.*X",4,i); //2 bytes start address
		strData += str;
		strData += "00";        //record type
		VerifyCode = 0x20 + (BYTE)(i>>8) + (BYTE)(i);
		for(BYTE j=0;j<0x20;j++)
		{
			Code=(BYTE)*(DataRead+i+j);
			str.Format("%.*X",2,Code);
			strData += str;
			VerifyCode += Code;
		}
		str.Format("%.*X",2,(BYTE)((~VerifyCode)+1));
		strData += str;
		strData += "\r\n";
		if(i==0xFFE0)  //for max address over 0xFFFF
		{
			strData += ":020000040001F9\r\n";
		}
	}
	strData += ":00000001FF";
	OutFile.Write(strData,strData.GetLength());
	OutFile.Close();
	return TRUE;
}


void CSTM32ISPDlg::OnCheckGo() 
{
	// TODO: Add your control notification handler code here
	if(BST_CHECKED==((CButton*)GetDlgItem(IDC_CHECK_GO))->GetCheck()){
		Go_Run=1;
	}
	else{
		Go_Run=0;
	}

	CString Path(AfxGetApp()->m_pszHelpFilePath);
	char Drive[3], Dir[256], Fname[256], Ext[256];
    _splitpath(Path,Drive,Dir,Fname,Ext);
	
	CString ConfFile;
	ConfFile.Format("%s%s%s", Drive, Dir , "Default.conf");

	CIni Ini((LPCSTR)ConfFile);

	UpdateData(TRUE);
	Ini.WriteBool((LPCTSTR)"CMD",(LPCTSTR)"Go_Run",Go_Run);
	UpdateData(FALSE);
}


void CSTM32ISPDlg::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int	CurSel =m_tab1.GetCurSel();
	switch(CurSel)
	{
	case 0:
		m_myisp.ShowWindow(true);
		m_myiap.ShowWindow(false);
		break;
	case 1:
		m_myisp.ShowWindow(false);
		m_myiap.ShowWindow(true);
		break;
	default:break;
	}
	
	*pResult = 0;
}
//wifi-dmx.com