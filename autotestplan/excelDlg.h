// excelDlg.h : header file
//

#pragma once
#include "excel9.h"
#include "afxcmn.h"
#include "afxwin.h"


// CexcelDlg dialog
class CexcelDlg : public CDialog
{
// Construction
public:
	CexcelDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EXCEL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLoadds();
	CString mStrEditInfo;
	afx_msg void OnBnClickedBtnGentestplan();
public:
	afx_msg void OnBnClickedBtnSaveclose();
	afx_msg void OnBnClickedBtnQuit();
//	afx_msg void OnBnHotItemChangeCheckSettop(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMThemeChangedCheckSettop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckSettop();
	CProgressCtrl m_ctrlProgressGlobal;
	short m_Progress;
	afx_msg void OnBnClickedVerbose();
	CButton mctrl_Verbose;
	bool blEnableVerboseMode;
	CMenu *pMenu;
	afx_msg void OnFileLoadds();
	afx_msg void OnFileLoadimage32782();
	afx_msg void OnFileExit32783();
	afx_msg void OnHelpQuickStart();
	afx_msg void OnAboutVersion32784();
	afx_msg void OnFileAddToolTo();
	afx_msg void OnFileSaveas();
};


void UpdateProgress(CexcelDlg* pDlg,short pos);
//save logs
void SaveLogs(CString lpBuf,int forceSave=0);
void SaveLogs(LPTSTR lpBuf,int forceSave=0);

