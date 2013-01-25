// LcGUIDlg.h : header file
//

#pragma once
#include "afxwin.h"



// CLcGUIDlg dialog
class CLcGUIDlg : public CDialog
{
// Construction
public:
	CLcGUIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LCGUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit EDIT_TRAINING_VAR;
	CEdit EDIT_DESCRIPTION_VAR;
	CEdit EDIT_OBJECTS_VAR;
	afx_msg void OnBnClickedOpenTraining();
	afx_msg void OnBnClickedOpenDescription();
	afx_msg void OnBnClickedOpenObjects();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedRadio6();
	afx_msg void OnBnClickedRadio7();
	afx_msg	void OnTimer (UINT TimerVal);

	CEdit EDIT_TIME_VAR;
	CButton BUTTON_RUN;
	CEdit EDIT_LOG;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeDescription();
};
