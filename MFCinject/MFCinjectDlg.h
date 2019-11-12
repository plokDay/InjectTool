
// MFCinjectDlg.h: 头文件
//

#pragma once


// CMFCinjectDlg 对话框
class CMFCinjectDlg : public CDialogEx
{
// 构造
public:
	CMFCinjectDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCINJECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CMenu m_menu;
	afx_msg void OnBnClickedButton1();
	CListCtrl m_listProcess;
	
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void On32771();
	afx_msg void OnCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void On32772();
	afx_msg void On32775();
	afx_msg void EnumMode();
};
