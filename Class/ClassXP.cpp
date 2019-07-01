


////////////////////////////////////////////////////////////////////////////////////////////////////
// 说明: ClassXP.c 文件
// 更新: 2003-3-10
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 编译预处理
#if _WIN32_WINNT < 0x0400
#define _WIN32_WINNT 0x0400
#endif
#include <Windows.h>
#include "ClassXP.h"
#pragma warning(disable: 4311)
#pragma warning(disable: 4312)
#pragma comment(lib, "Msimg32.lib")

// 导出函数
#ifdef CXP_DLLMODE
#pragma comment(linker, "/EXPORT:ClassXP=_ClassXP@8")
#endif // CXP_DLLMODE

// 强制使用 C 语言方式编译
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 宏定义
#define Bk_Color  0x00FFFFFF 


// 窗口类型
#define CXPT_UNKNOWN		-1	// 不能处理的类型
#define CXPT_PUSHBUTTON		0	// 按钮
#define CXPT_CHECKBOX		1	// 复选框
#define CXPT_RADIOBOX		2	// 单选框
#define CXPT_EDITBOX		3	// 编辑框
#define CXPT_COMBOBOX		4	// 组合框


// 窗口状态
#define CXPS_DISABLED		0x00000001L		// 禁用状态
#define CXPS_PRESSED		0x00000002L		// 按下状态
#define CXPS_HOTLIGHT		0x00000004L		// 高亮状态 (鼠标在该窗口上)
#define CXPS_FOCUS			0x00000008L		// 具有键盘输入焦点
#define CXPS_DEFAULT		0x00000010L		// 默认状态 (用于按钮)
#define CXPS_CHECKED		0x00000020L		// 选中状态 (用于复选框)
#define CXPS_INDETERMINATE	0x00000040L		// 未确定状态 (用于复选框)
#define CXPS_READONLY		0x00000080L		// 只读状态 (用于编辑框)


// 设置窗口状态
#define CXPM_SETSTATE(Data, Mask, IsSet)	((IsSet) ? (Data |= Mask) : (Data &= ~Mask))
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// CLASSXP 结构，所有的代码都是围绕这个结构而编写的
typedef struct tagCLASSXP
{
	HWND hWnd;					// 窗口句柄
	DWORD dwType;				// 窗口的类型
	DWORD dwState;				// 窗口的状态
	WNDPROC wpPrev;				// 子类化之前的窗口回调函数地址
	struct tagCLASSXP * pNext;	// 指向下一个 CLASSXP 结构，这里采用单向链表结构
}
CLASSXP, * PCLASSXP;
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMDCXP 结构，为了方便使用内存兼容设备场景而设计
typedef struct tagMEMDCXP
{
	HWND hWnd;					// 窗口句柄，输入参数
	HDC hDC;					// 窗口设备场景，输出参数
	HDC hMemDC;					// 窗口内存兼容设备场景，输出参数
	BOOL bTransfer;				// 是否要用在 hDC 和 hMemDC 间传送数据，输入参数
	HBITMAP hBitmap;			// 位图句柄，输入和输出参数
}
MEMDCXP, * LPMEMDCXP;
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数声明
PCLASSXP WINAPI CreateClassXP(HWND hWnd);
PCLASSXP WINAPI DeleteClassXP(HWND hWnd);
PCLASSXP WINAPI GetClassXP(HWND hWnd);

DWORD WINAPI GetWindowTypeXP(HWND hWnd);
HDC WINAPI GetMemDCXP(LPMEMDCXP pMdcxp);
VOID WINAPI ReleaseMemDCXP(LPMEMDCXP pMdcxp);
VOID WINAPI GradientRectXP(HDC hDC, LPRECT pRect,COLORREF crColor[4]);
VOID WINAPI DrawDropGripXP(HDC hDC, LPRECT pRect);

BOOL CALLBACK EnumWndProcXP(HWND hWnd, LPARAM lParam);
LRESULT CALLBACK HookProcXP(int iCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProcXP(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

VOID WINAPI DrawPushButtonXP(PCLASSXP pCxp);
VOID WINAPI DrawCheckBoxXP(PCLASSXP pCxp);
VOID WINAPI DrawRadioBoxXP(PCLASSXP pCxp);
VOID WINAPI DrawEditBoxXP(PCLASSXP pCxp);
VOID WINAPI DrawComboBoxXP(PCLASSXP pCxp);
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 全局变量
HHOOK g_hPrevHookXP = NULL;		// 窗口消息 HOOK 句柄
PCLASSXP g_pClassXP = NULL;		// 窗口的 CLASSXP 结构指针

#ifdef CXP_DLLMODE
HINSTANCE g_hModuleXP = NULL;	// 动态连接库模块句柄
#endif // CXP_DLLMODE
////////////////////////////////////////////////////////////////////////////////////////////////////



#ifdef CXP_DLLMODE
////////////////////////////////////////////////////////////////////////////////////////////////////
// 动态连接库主函数
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID pvReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
	{
		g_hModuleXP = hModule;
		DisableThreadLibraryCalls(hModule);
	}
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
#endif // CXP_DLLMODE



////////////////////////////////////////////////////////////////////////////////////////////////////
// 设置或取消窗口的 ClassXP 风格
BOOL WINAPI ClassXP(HWND hWnd, BOOL bEnable)
{
	BOOL bReturn;

	bReturn = FALSE;

	// 如果是影响当前进程中的所有窗口
	if (hWnd == NULL)
	{
		// 如果是取消当前进程中的所有窗口
		if ((bEnable == FALSE) && (g_hPrevHookXP != NULL))
		{
			// 枚举当前线程的窗口并取消 ClassXP 风格
			EnumThreadWindows(GetCurrentThreadId(), EnumWndProcXP, FALSE);

			// 取消窗口消息 HOOK
			bReturn = UnhookWindowsHookEx(g_hPrevHookXP);
			g_hPrevHookXP = NULL;
		}
		// 如果是设置当前进程中的所有窗口
		else if ((bEnable == TRUE) && (g_hPrevHookXP == NULL))
		{
			// 枚举当前线程中已存在的窗口并设置为 ClassXP 风格
			EnumThreadWindows(GetCurrentThreadId(), EnumWndProcXP, TRUE);

			// 安装窗口消息 HOOK
			g_hPrevHookXP = SetWindowsHookEx(WH_CALLWNDPROC, HookProcXP, 0, GetCurrentThreadId());
			bReturn = (BOOL) g_hPrevHookXP;
		}
	}
	else
	{
		// 如果是取消指定窗口的 ClassXP 风格
		if (bEnable == FALSE)
			bReturn = (BOOL) DeleteClassXP(hWnd);
		// 如果是设置指定窗口的 ClassXP 风格
		else
			bReturn = (BOOL) CreateClassXP(hWnd);			
	}
	return bReturn;
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 创建并初始化 CLASSXP 数据结构；子类化窗口
// 如果返回 NULL，表示没有创建；否则返回新创建节点的指针，同时 g_pClassXP 指向新创建的节点
PCLASSXP WINAPI CreateClassXP(HWND hWnd)
{
	LONG lStyle;
	DWORD dwType;
	PCLASSXP pCxp;

	// 是否已经是 ClassXP 风格
	if (GetClassXP(hWnd) == NULL)
	{
		// 获取窗口类型，如果并判断是否能设置为 ClassXP 风格
		dwType = GetWindowTypeXP(hWnd);
		if ((dwType >= CXPT_PUSHBUTTON) && (dwType <= CXPT_COMBOBOX))
		{
			lStyle = GetWindowLong(hWnd, GWL_STYLE);

			// 分配存储空间，增加一个节点
			pCxp = (PCLASSXP) HeapAlloc(GetProcessHeap(), 0, sizeof(CLASSXP));
			pCxp->pNext = g_pClassXP;
			g_pClassXP = pCxp;

			// 子类化窗口并初始化 CLASSXP 数据结构
			pCxp->hWnd = hWnd;
			pCxp->dwType = dwType;
			pCxp->dwState = (lStyle & WS_DISABLED) ? CXPS_DISABLED : 0;
			if (hWnd == GetFocus())
				pCxp->dwState |= CXPS_FOCUS;
			pCxp->wpPrev = (WNDPROC) SetWindowLong(hWnd, GWL_WNDPROC, (LONG) WindowProcXP);

			// 按窗口类型分别 CLASSXP 数据结构
			switch (dwType)
			{
			case CXPT_PUSHBUTTON:
			case CXPT_CHECKBOX:
			case CXPT_RADIOBOX:
				if ((lStyle & SS_TYPEMASK) == BS_DEFPUSHBUTTON)
					pCxp->dwState |= CXPS_DEFAULT;

				lStyle = (LONG) SendMessage(hWnd, BM_GETCHECK, 0, 0);
				if (lStyle == BST_CHECKED)
					pCxp->dwState |= CXPS_CHECKED;
				else if (lStyle == BST_INDETERMINATE)
					pCxp->dwState |= CXPS_INDETERMINATE;
				break;

			case CXPT_EDITBOX:
				if (lStyle & ES_READONLY)
					pCxp->dwState |= CXPS_READONLY;
				break;
			}

			// 重画窗口
			RedrawWindow(hWnd, NULL, NULL,
				RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);
			return pCxp;
		}
	}
	return NULL;
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 取消窗口子类化；销毁窗口的 CLASSXP 数据结构
// 如果返回值不为 NULL 表示成功删除，返回值为指向上一个节点指针；
// 如果返回 NULL 且 g_pClassXP 为 NULL，表全部节点被删除；
// 否则表示没有找到该节点。
// 致谢: 感谢 dREAMtHEATER 改进此函数!
PCLASSXP WINAPI DeleteClassXP(HWND hWnd)
{
	PCLASSXP pDel;
	PCLASSXP pCxp;

	// 获取待删除的节点指针
	pDel = GetClassXP(hWnd);
	if (pDel != NULL)
	{
		// 如果待删除的节点就是 g_pClassXP 节点
		if (pDel == g_pClassXP)
			pCxp = g_pClassXP = pDel->pNext;
		else
		{
			// 循环查找待删除节点的上一个节点
			for (pCxp = g_pClassXP; pCxp != NULL; pCxp = pCxp->pNext)
			{
				// 如果找到
				if (pCxp->pNext == pDel)
				{
					// 使链表跳过待删除的节点
					pCxp->pNext = pDel->pNext;
					break;
				}
			}
		}

		// 取消窗口子类化并重画窗口
		SetWindowLong(hWnd, GWL_WNDPROC, (LONG) pDel->wpPrev);

		// 删除堆内存
		HeapFree(GetProcessHeap(), 0, pDel);

		// 重画窗口
		RedrawWindow(hWnd, NULL, NULL,
			RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);
		return pCxp;
	}
	return NULL;
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 获取窗口的 CLASSXP 数据结构
// 如果返回 NULL，表示没有找到；否则返回节点的指针
PCLASSXP WINAPI GetClassXP(HWND hWnd)
{
	PCLASSXP pCxp;

	for (pCxp = g_pClassXP; pCxp != NULL; pCxp = pCxp->pNext)
	{
		if (pCxp->hWnd == hWnd)
			return pCxp;
	}
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 获取窗口类型
DWORD WINAPI GetWindowTypeXP(HWND hWnd)
{
	DWORD lReturn;
	char szTemp[MAX_PATH];
	static char s_szClass[][32] = 
	{
		"Button",					// 按钮类
		"Edit",						// 编辑框类
		"ComboBox",					// 组合框类

#ifdef CXP_DLLMODE
		"TButton",					// VCL TButton 类
		"ThunderCommandButton",		// Visual Basic Command Button 类
		"ThunderRT6CommandButton",	// Visual Basic Command Button 类
		"TCheckBox",
		"ThunderCheckBox",
		"ThunderRT6CheckBox",
		"TEdit",
		"TNumberEdit",
		"ThunderTextBox",
		"ThunderRT6TextBox",
		"TComboBox",
		"ThunderComboBox",
		"ThunderRT6ComboBox"
#endif // CXP_DLLMODE
	};

	// 查找判断匹配的类名称
	GetClassName(hWnd, szTemp, sizeof(szTemp));
	for (lReturn = 0; lReturn < (sizeof(s_szClass) / sizeof(s_szClass[0])); lReturn++)
		if (lstrcmpi(szTemp, s_szClass[lReturn]) == 0)
			break;

	switch (lReturn)
	{
	case 0:
		lReturn = GetWindowLong(hWnd, GWL_STYLE);
		switch (lReturn & SS_TYPEMASK)
		{
		case BS_DEFPUSHBUTTON:		// 默认按钮
		case BS_PUSHBUTTON:			// 普通按钮
			lReturn = CXPT_PUSHBUTTON;
			break;

		case BS_CHECKBOX:			// 复选框
		case BS_AUTOCHECKBOX:		// 自动复选框
		case BS_3STATE:				// 三状态复选框
		case BS_AUTO3STATE:			// 自动三状态复选框
			if (lReturn & BS_PUSHLIKE)
				lReturn = CXPT_PUSHBUTTON;
			else
				lReturn = CXPT_CHECKBOX;
			break;

		case BS_RADIOBUTTON:		// 单选框
		case BS_AUTORADIOBUTTON:	// 自动单选框
			if (lReturn & BS_PUSHLIKE)
				lReturn = CXPT_PUSHBUTTON;
			else
				lReturn = CXPT_RADIOBOX;
			break;

		default:	// 未知类型
			lReturn = CXPT_UNKNOWN;
		}
		break;

	case 1:			// 编辑框
		lReturn = CXPT_EDITBOX;
		break;
	
	case 2:			// 组合框
		if ((GetWindowLong(hWnd, GWL_STYLE) & 0x00000003) == CBS_SIMPLE)
			lReturn = CXPT_UNKNOWN;
		else
			lReturn = CXPT_COMBOBOX;
		break;

#ifdef CXP_DLLMODE
	// VB 和 VCL 的控件，只有在动态连接库方式下才有可能出现这种情况
	case 3:
	case 4:
	case 5:
		lReturn = CXPT_PUSHBUTTON;
		break;

	case 6:
	case 7:
	case 8:
		lReturn = CXPT_CHECKBOX;
		break;

	case 9:
	case 10:
	case 11:
	case 12:
		lReturn = CXPT_EDITBOX;
		break;

	case 13:
	case 14:
	case 15:
		lReturn = CXPT_COMBOBOX;
		break;

#endif // CXP_DLLMODE

	default:		// 未知类型
		lReturn = CXPT_UNKNOWN;
	}

	return lReturn;
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 获取窗口的内存兼容设备场景
HDC WINAPI GetMemDCXP(LPMEMDCXP pMdcxp)
{
	RECT Rect;

	GetWindowRect(pMdcxp->hWnd, &Rect);

	// 创建兼容内存兼容设备场景并设置为透明模式
	pMdcxp->hDC = GetWindowDC(pMdcxp->hWnd);
	pMdcxp->hMemDC = CreateCompatibleDC(pMdcxp->hDC);;
	SetBkMode(pMdcxp->hMemDC, TRANSPARENT);

	// 是否已经指定了位图句柄
	if (pMdcxp->hBitmap)
	{
		// 选择位图对象
		SelectObject(pMdcxp->hMemDC, pMdcxp->hBitmap);
	}
	else
	{
		// 创建并选择位图对象
		pMdcxp->hBitmap = (HBITMAP) SelectObject(pMdcxp->hMemDC,
			CreateCompatibleBitmap(pMdcxp->hDC, Rect.right - Rect.left, Rect.bottom - Rect.top));
	}

	// 如果要传送数据
	if (pMdcxp->bTransfer == TRUE)
	{
		BitBlt(pMdcxp->hMemDC, 0, 0,
			Rect.right - Rect.left, Rect.bottom - Rect.top, pMdcxp->hDC, 0, 0, SRCCOPY);
	}

	return pMdcxp->hMemDC;
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 获取窗口的内存兼容设备场景
VOID WINAPI ReleaseMemDCXP(LPMEMDCXP pMdcxp)
{
	RECT Rect;

	// 如果要传送数据
	if (pMdcxp->bTransfer == TRUE)
	{
		GetWindowRect(pMdcxp->hWnd, &Rect);
		BitBlt(pMdcxp->hDC, 0, 0,
			Rect.right - Rect.left, Rect.bottom - Rect.top, pMdcxp->hMemDC, 0, 0, SRCCOPY);
	}

	if (pMdcxp->hBitmap)
		DeleteObject(SelectObject(pMdcxp->hMemDC, pMdcxp->hBitmap));
	DeleteDC(pMdcxp->hMemDC);
	ReleaseDC(pMdcxp->hWnd, pMdcxp->hDC);
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 绘制渐变矩形
VOID WINAPI GradientRectXP(HDC hDC, LPRECT pRect,COLORREF crColor[4])
{
	int i;
	TRIVERTEX Tve[4]; 
	GRADIENT_RECT GRect;
	GRADIENT_TRIANGLE GTrg;

	for (i = 0; i < 4; i++)
	{
		Tve[i].Red = ((COLOR16) GetRValue(crColor[i])) << 8;
		Tve[i].Green = ((COLOR16) GetGValue(crColor[i])) << 8;
		Tve[i].Blue = ((COLOR16) GetBValue(crColor[i])) << 8;
		Tve[i].Alpha = ((COLOR16) (crColor[i] >> 24)) << 8;
	}

	Tve[0].x = pRect->left;
	Tve[0].y = pRect->top;
	Tve[1].x = pRect->right;
	Tve[1].y = pRect->top;
	Tve[2].x = pRect->left;
	Tve[2].y = pRect->bottom;
	Tve[3].x = pRect->right;
	Tve[3].y = pRect->bottom;

	if ((crColor[0] == crColor[2]) &&
		(crColor[1] == crColor[3]))
		i = GRADIENT_FILL_RECT_H;
	if ((crColor[0] == crColor[1]) &&
		(crColor[2] == crColor[3]))
		i = GRADIENT_FILL_RECT_V;
	else
		i = GRADIENT_FILL_TRIANGLE;

	if (i == GRADIENT_FILL_TRIANGLE)
	{
		GTrg.Vertex1 = 0;
		GTrg.Vertex2 = 1;
		GTrg.Vertex3 = 2;
	}
	else
	{
		GRect.UpperLeft = 0;
		GRect.LowerRight = 3;
	}
	GradientFill(hDC, Tve, 4,
		((i == GRADIENT_FILL_TRIANGLE) ? ((PVOID) &GTrg) : ((PVOID) &GRect)), 1, i);

	if (i == GRADIENT_FILL_TRIANGLE)
	{
		GTrg.Vertex1 = 3;
		GradientFill(hDC,Tve, 4, &GTrg, 1, GRADIENT_FILL_TRIANGLE);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 绘制渐变矩形
VOID WINAPI DrawDropGripXP(HDC hDC, LPRECT pRect)
{
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 枚举窗口回调函数
BOOL CALLBACK EnumWndProcXP(HWND hWnd, LPARAM lParam)
{
	// 如果是取消指定窗口的 ClassXP 风格
	if (lParam == FALSE)
		DeleteClassXP(hWnd);
	// 如果是设置指定窗口的 ClassXP 风格
	else
		CreateClassXP(hWnd);	

	// 枚举子窗体
	EnumChildWindows(hWnd, EnumWndProcXP, lParam);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 窗口消息 HOOK 回调函数
LRESULT CALLBACK HookProcXP(int iCode, WPARAM wParam, LPARAM lParam)
{
	// 设置新建的窗口为 ClassXP 风格
	if ((((CWPSTRUCT *) lParam)->message == WM_CREATE) && (iCode >= 0))
		CreateClassXP(((CWPSTRUCT *) lParam)->hwnd);

	return CallNextHookEx(g_hPrevHookXP, iCode, wParam, lParam);
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 窗口子类化回调函数
LRESULT CALLBACK WindowProcXP(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LONG lReturn;
	PCLASSXP pCxp;
	TRACKMOUSEEVENT Tme;
	typedef VOID (WINAPI *DRAWXP)(PCLASSXP);
	static DRAWXP s_DrawXP[] = 
	{
		DrawPushButtonXP,
		DrawCheckBoxXP,
		DrawRadioBoxXP,
		DrawEditBoxXP,
		DrawComboBoxXP
	};

	pCxp = GetClassXP(hWnd);

	switch (pCxp->dwType)
	{
	case CXPT_PUSHBUTTON:	// 按钮
	case CXPT_CHECKBOX:		// 复选框
	case CXPT_RADIOBOX:		// 单选框
		switch (uMsg)
		{
		case BM_SETSTYLE:	// 按钮风格改变
			CXPM_SETSTATE(pCxp->dwState, CXPS_DEFAULT, wParam & BS_DEFPUSHBUTTON);
			s_DrawXP[pCxp->dwType](pCxp);
			break;

		case BM_SETSTATE:	// 设置按钮状态
			lReturn = (LONG) CallWindowProc(pCxp->wpPrev, hWnd, uMsg, wParam, lParam);
			CXPM_SETSTATE(pCxp->dwState, CXPS_PRESSED, wParam);
			s_DrawXP[pCxp->dwType](pCxp);
			return lReturn;

		case BM_SETCHECK:	// 设置选中状态
			lReturn = (LONG) CallWindowProc(pCxp->wpPrev, hWnd, uMsg, wParam, lParam);
			CXPM_SETSTATE(pCxp->dwState, CXPS_CHECKED, (wParam == BST_CHECKED));
			CXPM_SETSTATE(pCxp->dwState, CXPS_INDETERMINATE, (wParam == BST_INDETERMINATE));
			s_DrawXP[pCxp->dwType](pCxp);
			return lReturn;

		case WM_SETTEXT:	// 设置窗口文本
			lReturn = (LONG) DefWindowProc(hWnd, uMsg, wParam, lParam);
			s_DrawXP[pCxp->dwType](pCxp);
			return lReturn;

		case WM_PAINT:		// 窗口重画
			lReturn = (LONG) DefWindowProc(hWnd, uMsg, wParam, lParam);
			s_DrawXP[pCxp->dwType](pCxp);
			return lReturn;
		}
		break;

	case CXPT_EDITBOX:		// 编辑框
		if (uMsg == WM_NCPAINT)
		{
			lReturn = (LONG) DefWindowProc(hWnd, uMsg, wParam, lParam);
			DrawEditBoxXP(pCxp);
			return lReturn;
		}
		else if (uMsg == EM_SETREADONLY)
		{
			CXPM_SETSTATE(pCxp->dwState, CXPS_READONLY, wParam);
			DrawEditBoxXP(pCxp);
		}
		break;

	case CXPT_COMBOBOX:		// 组合框
		switch (uMsg)
		{
		case WM_PAINT:
		case WM_NCPAINT:
			lReturn = (LONG) CallWindowProc(pCxp->wpPrev, hWnd, uMsg, wParam, lParam);
			DrawComboBoxXP(pCxp);
			return lReturn;

		case WM_LBUTTONDOWN:
			pCxp->dwState |= CXPS_PRESSED;
			break;

		case WM_LBUTTONUP:
			pCxp->dwState &= ~CXPS_PRESSED;
			break;
		}
		break;
	}

	// 调用原来的回调函数
	lReturn = (LONG) CallWindowProc(pCxp->wpPrev, hWnd, uMsg, wParam, lParam);

	// 对所有窗口相同的处理
	switch (uMsg)
	{
	case WM_MOUSEMOVE:		// 窗口移动
		if (((pCxp->dwState & CXPS_HOTLIGHT) == 0) && ((wParam & MK_LBUTTON) == 0))
		{			
			pCxp->dwState |= CXPS_HOTLIGHT;
			s_DrawXP[pCxp->dwType](pCxp);

			// 追踪鼠标移出消息一次
			Tme.cbSize = sizeof(TRACKMOUSEEVENT);
			Tme.dwFlags = TME_LEAVE;
			Tme.hwndTrack = hWnd;
			TrackMouseEvent(&Tme);
		}
		break;

	case WM_MOUSELEAVE:		// 鼠标移出
		if (pCxp->dwState & CXPS_HOTLIGHT)
		{
			pCxp->dwState &= ~CXPS_HOTLIGHT;
			s_DrawXP[pCxp->dwType](pCxp);
		}
		break;

	case WM_ENABLE:			// 窗口被设置为禁用或可用
		CXPM_SETSTATE(pCxp->dwState, CXPS_DISABLED, !wParam);
		s_DrawXP[pCxp->dwType](pCxp);
		break;

	case WM_SETFOCUS:		// 获得焦点
		pCxp->dwState |= CXPS_FOCUS;
		s_DrawXP[pCxp->dwType](pCxp);
		break;

	case WM_KILLFOCUS:		// 丢失焦点
		pCxp->dwState &= ~CXPS_FOCUS;
		s_DrawXP[pCxp->dwType](pCxp);
		break;

	case WM_DESTROY:		// 窗口销毁
		DeleteClassXP(hWnd);
	}
	return lReturn;
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 绘制按钮
VOID WINAPI DrawPushButtonXP(PCLASSXP pCxp)
{
	int i;
	RECT Rect;
	MEMDCXP Mdcxp;
	HANDLE hHandle;
	COLORREF crColor;
	char szTemp[256];

	static COLORREF s_crGradientXP[][4] =
	{
		{0x00C1CCD1, 0x00C1CCD1, 0x00EEF1F2, 0xFFEEF1F2},
		{0x00CFF0FF, 0x00CFF0FF, 0x000097E5, 0xFF0097E5},
		{0x00BDCBD6, 0x00C6CFD6, 0x00EFF3F7, 0xFFEFF3F7},
		{0x00FFE7CE, 0x00FFE7CE, 0x00EE8269, 0xFFEE8269},
		{0x00FFFFFF, 0x00FFFFFF, 0x00D6DFE2, 0xFFD6DFE2},

		{0x00DEE3E7, 0x00E7E7E7, 0x00DEE3E7, 0xFFDEE3E7},
		{0x00FBFCFC, 0x00FBFCFC, 0x00E6EBEC, 0xFFE6EBEC},
	};

	// 获取内存兼容设备场景
	Mdcxp.hWnd = pCxp->hWnd;
	Mdcxp.bTransfer = FALSE;
	Mdcxp.hBitmap = NULL;
	GetMemDCXP(&Mdcxp);

	// 获取窗口大小
	GetWindowRect(pCxp->hWnd, &Rect);
	Rect.right -= Rect.left;
	Rect.bottom -= Rect.top;
	Rect.left = Rect.top = 0;

	// 画最外面的框，颜色与系统按钮颜色一直
	FrameRect(Mdcxp.hMemDC, &Rect, GetSysColorBrush(COLOR_BTNFACE));

	// 画最第二层边框
	InflateRect(&Rect, -1, -1);
	hHandle = (HANDLE) CreateSolidBrush(
		(pCxp->dwState & CXPS_DISABLED) ? (GetSysColor(COLOR_BTNFACE) - 0x00202020) : 0x00733C00);
	FrameRect(Mdcxp.hMemDC, &Rect, (HBRUSH) hHandle);
	DeleteObject((HGDIOBJ) hHandle);

	// 画热点框渐变背景
	InflateRect(&Rect, -1, -1);
	if (pCxp->dwState & CXPS_DISABLED)
	{
		i = -1;
		hHandle = (HANDLE) CreateSolidBrush(0x00EAF4F5);
		FillRect(Mdcxp.hMemDC, &Rect, (HBRUSH) hHandle);
		DeleteObject((HGDIOBJ) hHandle);
	}
	else
	{
		if (pCxp->dwState & CXPS_PRESSED)
			i = 0;
		else if (pCxp->dwState & CXPS_HOTLIGHT)
			i = 1;
		else if ((pCxp->dwState & CXPS_CHECKED) || (pCxp->dwState & CXPS_INDETERMINATE))
			i = 2;
		else if ((pCxp->dwState & CXPS_FOCUS) || (pCxp->dwState & CXPS_DEFAULT))
			i = 3;
		else
			i = 4;
		GradientRectXP(Mdcxp.hMemDC, &Rect, s_crGradientXP[i]);
	}

	// 画文字区域渐变背景
	InflateRect(&Rect, -2, -2);
	if ((pCxp->dwState & CXPS_PRESSED) ||
		(pCxp->dwState & CXPS_CHECKED) ||
		(pCxp->dwState & CXPS_INDETERMINATE))
		i = 5;
	else if (!(pCxp->dwState & CXPS_DISABLED))
		i = 6;
	if ((i == 5) || (i == 6))
		GradientRectXP(Mdcxp.hMemDC, &Rect, s_crGradientXP[i]);


	// 用与系统按钮一直的颜色第二层边框的四角像素
	crColor = GetSysColor(COLOR_BTNFACE);
	SetPixel(Mdcxp.hMemDC, 1, 1, crColor);
	SetPixel(Mdcxp.hMemDC, 1, Rect.bottom + 2, crColor);
	SetPixel(Mdcxp.hMemDC, Rect.right + 2, Rect.bottom + 2, crColor);
	SetPixel(Mdcxp.hMemDC, Rect.right + 2, 1, crColor);

	// 画第二层边框的拐角像素，太罗嗦了，千万别仔细看：）
	crColor = (pCxp->dwState & CXPS_DISABLED) ?
		(GetSysColor(COLOR_BTNFACE) - 0x00151515) : 0x00A57D52;
	SetPixel(Mdcxp.hMemDC, 2, 2, crColor);
	SetPixel(Mdcxp.hMemDC, 2, Rect.bottom + 1, crColor);
	SetPixel(Mdcxp.hMemDC, Rect.right + 1, Rect.bottom + 1, crColor);
	SetPixel(Mdcxp.hMemDC, Rect.right + 1, 2, crColor);
	crColor = (pCxp->dwState & CXPS_DISABLED) ?
		(GetSysColor(COLOR_BTNFACE) - 0x00111111) : 0x00AD967B;
	SetPixel(Mdcxp.hMemDC, 1, 2, crColor);
	SetPixel(Mdcxp.hMemDC, 2, 1, crColor);
	SetPixel(Mdcxp.hMemDC, Rect.right + 1, 1, crColor);
	SetPixel(Mdcxp.hMemDC, Rect.right + 2, 2, crColor);
	SetPixel(Mdcxp.hMemDC, Rect.right + 1, Rect.bottom + 2, crColor);
	SetPixel(Mdcxp.hMemDC, Rect.right + 2, Rect.bottom + 1, crColor);
	SetPixel(Mdcxp.hMemDC, 2, Rect.bottom + 2, crColor);
	SetPixel(Mdcxp.hMemDC, 1, Rect.bottom + 1, crColor);

	// 如果有焦点，画出焦点框
	if (pCxp->dwState & CXPS_FOCUS)
	{
		InflateRect(&Rect, 1, 1);
		DrawFocusRect(Mdcxp.hMemDC, &Rect);
	}

	// 画文字
	if (GetWindowText(pCxp->hWnd, szTemp, sizeof(szTemp)))
	{
		if (pCxp->dwState & CXPS_PRESSED)
			OffsetRect(&Rect, 1, 1);
		SetTextColor(Mdcxp.hMemDC,
			((pCxp->dwState & CXPS_INDETERMINATE) || (pCxp->dwState & CXPS_DISABLED)) ?
			0x0094A2A5: 0x00000000);
		hHandle = (HANDLE) SelectObject(Mdcxp.hMemDC,
			(HGDIOBJ) SendMessage(pCxp->hWnd, WM_GETFONT, 0, 0));
		DrawText(Mdcxp.hMemDC, szTemp, -1, &Rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		SelectObject(Mdcxp.hMemDC, (HGDIOBJ) hHandle);
	}

	Mdcxp.bTransfer = TRUE;
	ReleaseMemDCXP(&Mdcxp);
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 绘制复选框
VOID WINAPI DrawCheckBoxXP(PCLASSXP pCxp)
{
	int i;
	RECT Rect;
	MEMDCXP Mdcxp;
	HANDLE hHandle;
	char szTemp[256];
	COLORREF crColor;
	static COLORREF s_crGradientXP[][4] =
	{
		{0x00A5B2B5, 0x00CED7D6, 0x00CED7D6, 0x00DEEFF7},
		{0x00CEF3FF, 0x0063CBFF, 0x0063CBFF, 0x0031B2FF},
		{0x00D6DFDE, 0x00EFF3F7, 0x00EFF3F7, 0x00FFFFFF}
	};

	// 获取内存兼容设备场景
	Mdcxp.hWnd = pCxp->hWnd;
	Mdcxp.bTransfer = FALSE;
	Mdcxp.hBitmap = NULL;
	GetMemDCXP(&Mdcxp);

	// 获取窗口大小
	GetWindowRect(pCxp->hWnd, &Rect);
	Rect.right -= Rect.left;
	Rect.bottom -= Rect.top;
	Rect.left = Rect.top = 0;

	hHandle = (HANDLE) CreateSolidBrush(COLOR_BTNFACE);
	// 填充背景
	FillRect(Mdcxp.hMemDC, &Rect, (HBRUSH)hHandle/*GetSysColorBrush(COLOR_BTNFACE)*/);

	// 画最外面的框
	Rect.left = 0;
	Rect.right = 13;
	Rect.top = (Rect.bottom - 13) / 2;
	Rect.bottom = Rect.top + 13;
	hHandle = (HANDLE) CreateSolidBrush(
		(pCxp->dwState & CXPS_DISABLED) ? (GetSysColor(COLOR_BTNFACE) - 0x00202020) : 0x00845118);
	FrameRect(Mdcxp.hMemDC, &Rect, (HBRUSH) hHandle);
	DeleteObject((HGDIOBJ) hHandle);

	// 画热点框渐变背景
	InflateRect(&Rect, -1, -1);
	if (pCxp->dwState & CXPS_DISABLED)
		FillRect(Mdcxp.hMemDC, &Rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
	else
	{
		if (pCxp->dwState & CXPS_PRESSED)
			i = 0;
		else if (pCxp->dwState & CXPS_HOTLIGHT)
			i = 1;
		else
			i = 2;
		GradientRectXP(Mdcxp.hMemDC, &Rect, s_crGradientXP[i]);
	}

	// 画内框
	InflateRect(&Rect, -2, -2);
	if ((pCxp->dwState & CXPS_INDETERMINATE) ||
		((pCxp->dwState & CXPS_HOTLIGHT) && (!(pCxp->dwState & CXPS_PRESSED))))
	{
		if (pCxp->dwState & CXPS_INDETERMINATE)
		{
			if (pCxp->dwState & CXPS_DISABLED)
				crColor = 0x00BDCBCE;
			else if (pCxp->dwState & CXPS_PRESSED)
				crColor = 0x00188A18;
			else if (pCxp->dwState & CXPS_HOTLIGHT)
				crColor = 0x0021A221;
			else
				crColor = 0x0073C373;
		}
		else if (pCxp->dwState & CXPS_CHECKED)
			crColor = 0x00F7F7F7;
		else
			crColor = 0x00E7E7E7;

		hHandle = (HANDLE) CreateSolidBrush(crColor);
		FillRect(Mdcxp.hMemDC, &Rect, (HBRUSH) hHandle);
		DeleteObject((HGDIOBJ) hHandle);
	}


	// 画框内选中标志
	if (pCxp->dwState & CXPS_CHECKED)
	{
		hHandle = (HANDLE) SelectObject(Mdcxp.hMemDC,
			CreatePen(PS_SOLID, 1, (pCxp->dwState & CXPS_DISABLED) ? 0x000BDCBCE : 0x0021A221));
		for (i = 3; i < 10; i++)
		{
			MoveToEx(Mdcxp.hMemDC, i, Rect.top + ((i < 6) ? i - 1 : (9 - i)), NULL);
			LineTo(Mdcxp.hMemDC, i, Rect.top + ((i < 6) ? i + 2 : (12 - i)));
		}
		DeleteObject(SelectObject(Mdcxp.hMemDC, (HGDIOBJ) hHandle));
	}

	// 画文字
	if (GetWindowText(pCxp->hWnd, szTemp, sizeof(szTemp)))
	{
		SetTextColor(Mdcxp.hMemDC, GetSysColor((pCxp->dwState & CXPS_DISABLED) ?  COLOR_GRAYTEXT: COLOR_BTNTEXT));
		hHandle = (HANDLE) SelectObject(Mdcxp.hMemDC,
			(HGDIOBJ) SendMessage(pCxp->hWnd, WM_GETFONT, 0, 0));

		Rect.left = 18;
		Rect.top -= 2;
		Rect.bottom = Rect.top + 1 + DrawText(Mdcxp.hMemDC, szTemp, -1, &Rect,
			DT_CALCRECT | DT_SINGLELINE | DT_VCENTER);

		DrawText(Mdcxp.hMemDC, szTemp, -1, &Rect, DT_SINGLELINE | DT_VCENTER);
		SelectObject(Mdcxp.hMemDC, (HGDIOBJ) hHandle);

		// 如果有焦点，画出焦点框
		if (pCxp->dwState & CXPS_FOCUS)
		{
			InflateRect(&Rect, 1, 1);
			DrawFocusRect(Mdcxp.hMemDC, &Rect);
		}
	}
	Mdcxp.bTransfer = TRUE;
	ReleaseMemDCXP(&Mdcxp);
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 绘制单选框
VOID WINAPI DrawRadioBoxXP(PCLASSXP pCxp)
{
	int i;
	RECT Rect;
	MEMDCXP Mdcxp;
	HANDLE hHandle;
	char szTemp[256];
	COLORREF crColor;
	static COLORREF s_crGradientXP[][4] =
	{
		{0x00A5B2B5, 0x00CED7D6, 0x00CED7D6, 0x00DEEFF7},
		{0x00CEF3FF, 0x0063CBFF, 0x0063CBFF, 0x0031B2FF},
		{0x00D6DFDE, 0x00EFF3F7, 0x00EFF3F7, 0x00FFFFFF}
	};

	// 获取内存兼容设备场景
	Mdcxp.hWnd = pCxp->hWnd;
	Mdcxp.bTransfer = FALSE;
	Mdcxp.hBitmap = NULL;
	GetMemDCXP(&Mdcxp);

	// 获取窗口大小
	GetWindowRect(pCxp->hWnd, &Rect);
	Rect.right -= Rect.left;
	Rect.bottom -= Rect.top;
	Rect.left = Rect.top = 0;

	hHandle = (HANDLE) CreateSolidBrush(Bk_Color);
	// 填充背景
	FillRect(Mdcxp.hMemDC, &Rect,(HBRUSH) hHandle);

	// 画最外面的框
	Rect.left = 0;
	Rect.right = 13;
	Rect.top = (Rect.bottom - 13) / 2;
	Rect.bottom = Rect.top + 13;
	hHandle = (HANDLE) CreateSolidBrush(
		(pCxp->dwState & CXPS_DISABLED) ? (GetSysColor(COLOR_BTNFACE) - 0x00202020) : 0x00845118);
	FrameRect(Mdcxp.hMemDC, &Rect, (HBRUSH) hHandle);
	DeleteObject((HGDIOBJ) hHandle);

	// 画热点框渐变背景
	InflateRect(&Rect, -1, -1);
	if (pCxp->dwState & CXPS_DISABLED)
		FillRect(Mdcxp.hMemDC, &Rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
	else
	{
		if (pCxp->dwState & CXPS_PRESSED)
			i = 0;
		else if (pCxp->dwState & CXPS_HOTLIGHT)
			i = 1;
		else
			i = 2;
		GradientRectXP(Mdcxp.hMemDC, &Rect, s_crGradientXP[i]);
	}

	// 画内框
	InflateRect(&Rect, -2, -2);
	if ((pCxp->dwState & CXPS_INDETERMINATE) ||
		((pCxp->dwState & CXPS_HOTLIGHT) && (!(pCxp->dwState & CXPS_PRESSED))))
	{
		if (pCxp->dwState & CXPS_INDETERMINATE)
		{
			if (pCxp->dwState & CXPS_DISABLED)
				crColor = 0x00BDCBCE;
			else if (pCxp->dwState & CXPS_PRESSED)
				crColor = 0x00188A18;
			else if (pCxp->dwState & CXPS_HOTLIGHT)
				crColor = 0x0021A221;
			else
				crColor = 0x0073C373;
		}
		else if (pCxp->dwState & CXPS_CHECKED)
			crColor = 0x00F7F7F7;
		else
			crColor = 0x00E7E7E7;

		hHandle = (HANDLE) CreateSolidBrush(crColor);
		FillRect(Mdcxp.hMemDC, &Rect, (HBRUSH) hHandle);
		DeleteObject((HGDIOBJ) hHandle);
	}


	// 画框内选中标志
	if (pCxp->dwState & CXPS_CHECKED)
	{
		hHandle = (HANDLE) SelectObject(Mdcxp.hMemDC,
			CreatePen(PS_SOLID, 1, (pCxp->dwState & CXPS_DISABLED) ? 0x000BDCBCE : 0x0021A221));
		for (i = 3; i < 10; i++)
		{
			MoveToEx(Mdcxp.hMemDC, i, Rect.top + ((i < 6) ? i - 1 : (9 - i)), NULL);
			LineTo(Mdcxp.hMemDC, i, Rect.top + ((i < 6) ? i + 2 : (12 - i)));
		}
		DeleteObject(SelectObject(Mdcxp.hMemDC, (HGDIOBJ) hHandle));
	}

	// 画文字
	if (GetWindowText(pCxp->hWnd, szTemp, sizeof(szTemp)))
	{
		SetTextColor(Mdcxp.hMemDC, GetSysColor((pCxp->dwState & CXPS_DISABLED) ?  COLOR_GRAYTEXT: COLOR_BTNTEXT));
		hHandle = (HANDLE) SelectObject(Mdcxp.hMemDC,
			(HGDIOBJ) SendMessage(pCxp->hWnd, WM_GETFONT, 0, 0));

		Rect.left = 18;
		Rect.top -= 2;
		Rect.bottom = Rect.top + 1 + DrawText(Mdcxp.hMemDC, szTemp, -1, &Rect,
			DT_CALCRECT | DT_SINGLELINE | DT_VCENTER);

		DrawText(Mdcxp.hMemDC, szTemp, -1, &Rect, DT_SINGLELINE | DT_VCENTER);
		SelectObject(Mdcxp.hMemDC, (HGDIOBJ) hHandle);

		// 如果有焦点，画出焦点框
		if (pCxp->dwState & CXPS_FOCUS)
		{
			InflateRect(&Rect, 1, 1);
			DrawFocusRect(Mdcxp.hMemDC, &Rect);
		}
	}
	Mdcxp.bTransfer = TRUE;
	ReleaseMemDCXP(&Mdcxp);
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 绘制编辑框
VOID WINAPI DrawEditBoxXP(PCLASSXP pCxp)
{
	HDC hDC;
	RECT Rect;
	LONG lExStyle;
	HANDLE hHandle;

	lExStyle = GetWindowLong(pCxp->hWnd, GWL_EXSTYLE);
	if ((GetWindowLong(pCxp->hWnd, GWL_STYLE) & WS_BORDER) ||
		(lExStyle & WS_EX_CLIENTEDGE) || (lExStyle & WS_EX_STATICEDGE))
	{
		// 由于绘制的东西很少，所以直接绘制而不使用 MEMDCXP 方式
		hDC = GetWindowDC(pCxp->hWnd);

		// 获取窗口大小
		GetWindowRect(pCxp->hWnd, &Rect);
		Rect.right -= Rect.left;
		Rect.bottom -= Rect.top;
		Rect.top = Rect.left = 0;

		// 绘制外框
		hHandle = (HANDLE) CreateSolidBrush(
			(pCxp->dwState & CXPS_DISABLED) ? (GetSysColor(COLOR_BTNFACE) - 0x00202020) : 0x00BD9E7B);
		FrameRect(hDC, &Rect, (HBRUSH) hHandle);
		DeleteObject((HGDIOBJ) hHandle);

		// 绘制内框
		if ((lExStyle & WS_EX_CLIENTEDGE) || (lExStyle & WS_EX_STATICEDGE))
		{
			InflateRect(&Rect, -1, -1);
			hHandle = (HANDLE) GetSysColorBrush(
				(pCxp->dwState & CXPS_DISABLED) || (pCxp->dwState & CXPS_READONLY) ? COLOR_BTNFACE : COLOR_WINDOW);
			FrameRect(hDC, &Rect, (HBRUSH) hHandle);
			if ((lExStyle & WS_EX_CLIENTEDGE) && (lExStyle & WS_EX_STATICEDGE))
			{
				InflateRect(&Rect, -1, -1);
				FrameRect(hDC, &Rect, (HBRUSH)hHandle);
			}
		}

		// 释放设备场景
		ReleaseDC(pCxp->hWnd, hDC);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// 绘制组合框
VOID WINAPI DrawComboBoxXP(PCLASSXP pCxp)
{
	int i;
	RECT Rect;
	MEMDCXP Mdcxp;
	HANDLE hHandle;
	static COLORREF s_crGradientXP[][4] =
	{
		{0x00EFF3F7, 0x00DEE7E7, 0x00DEE3E7, 0x00DEE3E7},
		{0x00DEAEA5, 0x00F7CBBD, 0x00DE8273, 0x00F7C7B5},
		{0x00EFC7B5, 0x00E7AE94, 0x00DEA284, 0x00DEA68C},
		{0x00FFE3D6, 0x00F7CBBD, 0x00F7C3AD, 0x00F7C7B5},

		{0x00F7F7F7, 0x00EFF3F7, 0x00EFF3F7, 0x00EFF3F7},
		{0x00DEC3BD, 0x00DEB6AD, 0x00FFE3DE, 0x00F7E3DE},
		{0x00EFDBCE, 0x00EFCFC6, 0x00E7CFC6, 0x00E7CBBD},
		{0x00FFEFE7, 0x00FFE7DE, 0x00FFE3DE, 0x00F7E3DE},

		{0x00F7F7F7, 0x00E7EFEF, 0x00E7EBEF, 0x00DEE7E7},
		{0x00F78E6B, 0x00F79684, 0x00EF9E8C, 0x00EFDFD6},
		{0x00FFFFFF, 0x00FFE3CE, 0x00FFDFC6, 0x00FFDBBD},
		{0x00FFEBE7, 0x00FFCFBD, 0x00FFCBB5, 0x00F7CBAD}
	};

	// 获取内存兼容设备场景
	Mdcxp.hWnd = pCxp->hWnd;
	Mdcxp.bTransfer = TRUE;
	Mdcxp.hBitmap = NULL;
	GetMemDCXP(&Mdcxp);

	// 获取窗口大小
	GetWindowRect(pCxp->hWnd, &Rect);
	Rect.right -= Rect.left;
	Rect.bottom -= Rect.top;
	Rect.top = Rect.left = 0;

	/*if ((GetWindowLong(hWnd, GWL_STYLE) & 0x00000003) == CBS_SIMPLE)
	{
		GetWindow(hWnd, GW_CHILD);
		// 还原并释放内存设备场景
		Mdcxp.bTransfer = TRUE;
		ReleaseMemDCXP(&Mdcxp);		
	}*/

	// 绘制外框
	hHandle = (HANDLE) CreateSolidBrush(
		(pCxp->dwState & CXPS_DISABLED) ? (GetSysColor(COLOR_BTNFACE) - 0x00202020) : 0x00BD9E7B);
	FrameRect(Mdcxp.hMemDC, &Rect, (HBRUSH) hHandle);
	DeleteObject((HGDIOBJ) hHandle);

	// 绘制内框
	InflateRect(&Rect, -1, -1);
	hHandle = (HANDLE) GetSysColorBrush((pCxp->dwState & CXPS_DISABLED) ? COLOR_BTNFACE : COLOR_WINDOW);
	FrameRect(Mdcxp.hMemDC, &Rect, (HBRUSH) hHandle);

	InflateRect(&Rect, -1, -1);
	Rect.left = Rect.right - GetSystemMetrics(SM_CYVTHUMB);
	FrameRect(Mdcxp.hMemDC, &Rect, (HBRUSH) hHandle);

	Rect.left++;
	if (pCxp->dwState & CXPS_DISABLED)
		i = 0;
	else if (pCxp->dwState & CXPS_PRESSED)
		i = 1;
	else if (pCxp->dwState & CXPS_HOTLIGHT)
		i = 2;
	else
		i = 3;

	// 绘制下拉外框
	GradientRectXP(Mdcxp.hMemDC, &Rect, s_crGradientXP[i]);

	// 绘制下拉外框拐角像素
	SetPixel(Mdcxp.hMemDC, Rect.left, Rect.top, s_crGradientXP[i + 4][0]);
	SetPixel(Mdcxp.hMemDC, Rect.right - 1, Rect.top, s_crGradientXP[i + 4][1]);
	SetPixel(Mdcxp.hMemDC, Rect.left, Rect.bottom - 1, s_crGradientXP[i + 4][2]);
	SetPixel(Mdcxp.hMemDC, Rect.right - 1, Rect.bottom - 1, s_crGradientXP[i + 4][3]);		

	// 绘制下拉内框
	InflateRect(&Rect, -1, -1);
	GradientRectXP(Mdcxp.hMemDC, &Rect, s_crGradientXP[i + 8]);

	// 绘制下拉标志
	Rect.left += (Rect.right - Rect.left) / 2;
	Rect.top += (Rect.bottom - Rect.top) / 2;
	hHandle = (HANDLE) SelectObject(Mdcxp.hMemDC,
		CreatePen(PS_SOLID, 1, (pCxp->dwState & CXPS_DISABLED) ? 0x00C6CBCE : 0x0084614A));
	MoveToEx(Mdcxp.hMemDC, Rect.left - 4, Rect.top - 2, NULL);
	LineTo(Mdcxp.hMemDC, Rect.left, Rect.top + 2);
	LineTo(Mdcxp.hMemDC, Rect.left + 5, Rect.top - 3);
	MoveToEx(Mdcxp.hMemDC, Rect.left - 3, Rect.top - 2, NULL);
	LineTo(Mdcxp.hMemDC, Rect.left, Rect.top + 1);
	LineTo(Mdcxp.hMemDC, Rect.left + 4, Rect.top - 3);
	MoveToEx(Mdcxp.hMemDC, Rect.left - 3, Rect.top - 3, NULL);
	LineTo(Mdcxp.hMemDC, Rect.left, Rect.top);
	LineTo(Mdcxp.hMemDC, Rect.left + 4, Rect.top - 4);

	DeleteObject(SelectObject(Mdcxp.hMemDC, (HGDIOBJ) hHandle));

	// 还原并释放内存设备场景
	Mdcxp.bTransfer = TRUE;
	ReleaseMemDCXP(&Mdcxp);
}
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif // __cplusplus
////////////////////////////////////////////////////////////////////////////////////////////////////


