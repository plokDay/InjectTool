
// MFCinjectDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MFCinject.h"
#include "MFCinjectDlg.h"
#include "afxdialogex.h"
#include <windows.h>
#include <psapi.h>
#include <winternl.h>
#include <TlHelp32.h>
#include "resource.h"
#include "CModule.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//#define NM_CUSTOMDRAW (WM_USER+100)

// CMFCinjectDlg 对话框


TCHAR	g_szDllPath[MAX_PATH] = { 0 };//Dll路径
HMODULE g_hModeue;//模块句柄
bool CMFCinjectDlg::method = true;  // 类外定义
CMFCinjectDlg::CMFCinjectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCINJECT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCinjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listProcess);
}

BEGIN_MESSAGE_MAP(CMFCinjectDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCinjectDlg::OnBnClickedButton1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CMFCinjectDlg::OnNMRClickList1)
	ON_COMMAND(ID_32771, &CMFCinjectDlg::On32771)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &CMFCinjectDlg::OnCustomdrawList1)
	ON_COMMAND(ID_32772, &CMFCinjectDlg::On32772)
	ON_COMMAND(ID_32775, &CMFCinjectDlg::EnumMode)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &CMFCinjectDlg::OnColumnclickList1)
END_MESSAGE_MAP()


// CMFCinjectDlg 消息处理程序

BOOL CMFCinjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetWindowText(L"进程注入工具");
	m_menu.LoadMenu(IDR_MENU1);

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_listProcess.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listProcess.InsertColumn(0, L"PID", 0, 80);
	m_listProcess.InsertColumn(1, L"进程名", 0, 150);
	m_listProcess.InsertColumn(2, L"路径", 0, 550);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCinjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCinjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//刷新进程
void CMFCinjectDlg::OnBnClickedButton1()
{
	m_listProcess.DeleteAllItems();
	int nIdx = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{

		PROCESSENTRY32 proc = { sizeof(PROCESSENTRY32) };
		Process32First(hSnap, &proc);

		do
		{
			HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, proc.th32ProcessID);
			if (hProc != NULL) {
				CString buffer;
				buffer.Format(L"%d", proc.th32ProcessID);
				m_listProcess.InsertItem(nIdx, buffer);
				m_listProcess.SetItemText(nIdx, 1, proc.szExeFile);
				TCHAR processPath[MAX_PATH] = L"--";
				GetModuleFileNameEx(hProc, 0, processPath, MAX_PATH);
				m_listProcess.SetItemText(nIdx, 2, processPath);
				m_listProcess.SetItemData(nIdx, -1);

				nIdx++;
			}
		} while (Process32Next(hSnap, &proc));
	}
	m_listProcess.Scroll(CSize(0, 100000));

}
struct DATA

{

	int subitem;// 点击表头的列数

	CListCtrl* plist; //listctrl的指针

};
int CALLBACK CMFCinjectDlg::listCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	DATA* pListCtrl = (DATA*)lParamSort;
	int sort_column = pListCtrl->subitem;
	// 从参数中提取所需比较lc的两行数据
	int row1 = (int)lParam1;
	int row2 = (int)lParam2;
	CListCtrl* lc = (CListCtrl*)lParamSort;

	CString lp1 = (pListCtrl->plist)->GetItemText(lParam1, sort_column);
	CString lp2 = (pListCtrl->plist)->GetItemText(lParam2, sort_column);

	// 比较，对不同的列，不同比较，注意记录前一次排序方向，下一次要相反排序
	if (sort_column == 0 )  //根据列的数据类型选择比较的类型
	{
		// int型比较
		if (method)
			return _ttoi(lp1) - _ttoi(lp2);
		else
			return _ttoi(lp2) - _ttoi(lp1);
	}
	    else
	{
		// 文字型比较
		if (method)
			return lp1.CompareNoCase(lp2);
		else
			return lp2.CompareNoCase(lp1);
	}
	    return 0;
}


//右键菜单
void CMFCinjectDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CMenu* pSubMenu = m_menu.GetSubMenu(0);
	CPoint pos;
	GetCursorPos(&pos);
	pSubMenu->TrackPopupMenu(0, pos.x, pos.y, this);
	*pResult = 0;
}

//遍历模块
bool CheckModule(DWORD dwPid, std::vector<MODULEENTRY32>* modList)
{
	bool bRes = false;
	//1.创建模块快照
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
	if (hSnap == INVALID_HANDLE_VALUE) {
		return bRes;
	}

	//2.第一次遍历模块
	MODULEENTRY32 stcMd = { sizeof(stcMd) };
	if (Module32First(hSnap, &stcMd))
	{
		//3.循环遍历模块Next
		do
		{
			modList->push_back(stcMd);
			if (!StrCmpW(stcMd.szExePath, g_szDllPath))
			{
				bRes = true;
				g_hModeue = stcMd.hModule;
				return bRes;
			}
		} while (Module32Next(hSnap, &stcMd));
	}
	CloseHandle(hSnap);
	return bRes;

}
//卸载
void unInject(DWORD dwPid)
{
	// 1.打开目标进程
	HANDLE hProcess = OpenProcess(
		PROCESS_ALL_ACCESS,		// 打开权限
		FALSE,					// 是否继承
		dwPid);						// 进程PID

	// 2.在目标进程中创建线程
	HANDLE hThread = CreateRemoteThread(
		hProcess,					// 目标进程句柄
		NULL,						// 安全属性
		NULL,						// 栈大小
		(PTHREAD_START_ROUTINE)FreeLibrary,	// 回调函数
		(LPVOID)g_hModeue,					// 回调函数参数
		NULL,						// 标志
		NULL						// 线程ID
	);

	// 5.等待线程结束
	WaitForSingleObject(hThread, -1);
	CloseHandle(hThread);
	CloseHandle(hProcess);
}

//远线程注入
void inject(DWORD dwPid, LPWSTR dllPath)
{
	// 1.打开目标进程
	HANDLE hProcess = OpenProcess(
		PROCESS_ALL_ACCESS,		// 打开权限
		FALSE,					// 是否继承
		dwPid);						// 进程PID

// 2.在目标进程中申请空间
	LPVOID lpPathAddr = VirtualAllocEx(
		hProcess,					// 目标进程句柄
		0,							// 指定申请地址
		wcslen(dllPath) * 2 + 2,	// 申请空间大小
		MEM_RESERVE | MEM_COMMIT,	// 内存的状态
		PAGE_READWRITE);			// 内存属性


	// 3.在目标进程中写入Dll路径
	SIZE_T dwWriteSize = 0;
	WriteProcessMemory(
		hProcess,					// 目标进程句柄
		lpPathAddr,					// 目标进程地址
		dllPath,					// 写入的缓冲区
		wcslen(dllPath) * 2 + 2,	// 缓冲区大小
		&dwWriteSize);				// 实际写入大小

	// 4.在目标进程中创建线程
	HANDLE hThread = CreateRemoteThread(
		hProcess,					// 目标进程句柄
		NULL,						// 安全属性
		NULL,						// 栈大小
		(PTHREAD_START_ROUTINE)LoadLibrary,	// 回调函数
		lpPathAddr,					// 回调函数参数
		NULL,						// 标志
		NULL						// 线程ID
	);

	// 5.等待线程结束
	WaitForSingleObject(hThread, -1);

	// 6.清理环境
	VirtualFreeEx(hProcess, lpPathAddr, 0, MEM_RELEASE);
	CloseHandle(hThread);
	CloseHandle(hProcess);
}

enum COLOR
{
	COLOR_RED=-2,
	COLOR_GREEN=-3,
	COLOR_BLUE=-4,
};
//注入
void CMFCinjectDlg::On32771()
{
	std::vector<MODULEENTRY32> mmodule;
	int index = (int)m_listProcess.GetFirstSelectedItemPosition();
	if (index == 0) {
		return;
	}
	index -= 1;
	CString CSPid = m_listProcess.GetItemText(index, 0);//PID
	DWORD dwpid = _ttoi(CSPid);
	BROWSEINFO		sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("请选择一个文件：");
	sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_USENEWUI | BIF_BROWSEINCLUDEFILES;
	sInfo.lpfn = NULL;

	// 显示文件选择对话框
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件名
		if (::SHGetPathFromIDList(lpidlBrowse, g_szDllPath))
		{
			inject(dwpid, g_szDllPath);
		}
	}
	if (lpidlBrowse != NULL)
	{
		::CoTaskMemFree(lpidlBrowse);
	}
	else
	{
		return;
	}
	//判断是否注入成功
	if (CheckModule(dwpid, &mmodule))
	{
		MessageBox(L"注入成功", 0, 0);
		m_listProcess.SetItemData(index, COLOR_RED);
		SetDlgItemText(IDC_DLLPATH, g_szDllPath);

	}
	else
		MessageBox(L"注入失败", 0, 0);

}

//变颜色
void CMFCinjectDlg::OnCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);
	NMCUSTOMDRAW nmCustomDraw = pNMCD->nmcd;
	switch (nmCustomDraw.dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
	{
		if (COLOR_RED == nmCustomDraw.lItemlParam)
		{
			pNMCD->clrTextBk = RGB(255, 0, 0);
			pNMCD->clrText = RGB(255, 255, 255);
		}
		else if (COLOR_GREEN == nmCustomDraw.lItemlParam)
		{
			pNMCD->clrTextBk = RGB(0, 255, 0);
			pNMCD->clrText = RGB(255, 255, 255);
		}
		else if (COLOR_BLUE == nmCustomDraw.lItemlParam)
		{
			pNMCD->clrTextBk = RGB(0, 0, 255);
			pNMCD->clrText = RGB(255, 255, 255);
		}
		else
		{
		}
		break;
	}
	default:
	{
		break;
	}
	}
	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;
	*pResult |= CDRF_NOTIFYITEMDRAW;
	return;

}

//卸载
void CMFCinjectDlg::On32772()
{
	if (g_szDllPath == 0)
	{
		MessageBox(L"Dll路径为空", 0, 0);
		return;
	}
	int index = (int)m_listProcess.GetFirstSelectedItemPosition();
	if (index == 0) {
		return;
	}
	index -= 1;
	CString CSPid = m_listProcess.GetItemText(index, 0);//PID
	DWORD dwpid = _ttoi(CSPid);
	std::vector<MODULEENTRY32> mmodule;
	if (!CheckModule(dwpid, &mmodule))
	{
		CString buffer;
		buffer.Format(L"该进程未注入%s", g_szDllPath);
		MessageBox(buffer, 0, 0);
		return;
	}
	unInject(dwpid);
	if (!CheckModule(dwpid, &mmodule))
	{
		MessageBox(L"卸载成功", 0, 0);
		m_listProcess.SetItemData(index, -1);
	}
	else
		MessageBox(L"未成功卸载", 0, 0);
}


//遍历模块
void CMFCinjectDlg::EnumMode()
{
	int index = (int)m_listProcess.GetFirstSelectedItemPosition();
	if (index == 0) {
		return;
	}
	index -= 1;

	CString strPid = m_listProcess.GetItemText(index, 0);// 第1列是pid
	DWORD dwPid = _wtoi(strPid);
	CModule mModule;
	CheckModule(dwPid, &mModule.mmodule);
	mModule.DoModal();
}

//点击列表头
void CMFCinjectDlg::OnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	sort_column = pNMLV->iSubItem;//点击的列

	int count = m_listProcess.GetItemCount();
	for (int i = 0; i < count; i++)
		m_listProcess.SetItemData(i, i);

	DATA data;
	data.subitem = sort_column;
	data.plist = &m_listProcess;

	method = !method;
	m_listProcess.SortItems(listCompare, (LPARAM)&data);
	*pResult = 0;
}
