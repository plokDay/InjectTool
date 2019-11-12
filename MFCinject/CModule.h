#pragma once
#include <TlHelp32.h>
#include <vector>

// CModule 对话框

class CModule : public CDialogEx
{
	DECLARE_DYNAMIC(CModule)

public:
	CModule(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CModule();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lsCtlModule;
	std::vector<MODULEENTRY32> mmodule;

	virtual BOOL OnInitDialog();
	afx_msg void On32775();
};
