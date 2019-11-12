// CModule.cpp: 实现文件
//

#include "stdafx.h"
#include "MFCinject.h"
#include "CModule.h"
#include "afxdialogex.h"


// CModule 对话框

IMPLEMENT_DYNAMIC(CModule, CDialogEx)

CModule::CModule(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CModule::~CModule()
{
}

void CModule::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lsCtlModule);
}


BEGIN_MESSAGE_MAP(CModule, CDialogEx)
END_MESSAGE_MAP()


// CModule 消息处理程序


BOOL CModule::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetWindowText(L"遍历模块");
	m_lsCtlModule.SetExtendedStyle(LVS_EX_FULLROWSELECT );
	m_lsCtlModule.InsertColumn(0, L"索引", 0, 50);
	m_lsCtlModule.InsertColumn(1, L"模块名", 0, 120);
	m_lsCtlModule.InsertColumn(2, L"模块路径", 0, 300);
	int nIdx = 0;
	CString buffer;
	for (auto&i : mmodule)
	{
		buffer.Format(L"%d", nIdx);
		m_lsCtlModule.InsertItem(nIdx, buffer);
		m_lsCtlModule.SetItemText(nIdx, 1, i.szModule);
		m_lsCtlModule.SetItemText(nIdx, 2, i.szExePath);
		nIdx++;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

