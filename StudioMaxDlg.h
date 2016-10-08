// StudioMaxDlg.h : header file
//

#if !defined(AFX_STUDIOMAXDLG_H__8E791917_DF7D_11D3_8544_00A0CC3EABDD__INCLUDED_)
#define AFX_STUDIOMAXDLG_H__8E791917_DF7D_11D3_8544_00A0CC3EABDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DataMapper.h"

/////////////////////////////////////////////////////////////////////////////
// CStudioMaxDlg dialog

class CStudioMaxDlg : public CDialog
{
// Construction
public:
	CStudioMaxDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CStudioMaxDlg)
	enum { IDD = IDD_STUDIOMAX_DIALOG };
	CComboBox	m_selectMIDISoftwareInDevice;
	CComboBox	m_selectMIDIHardwareOutDevice;
	CComboBox	m_selectMIDISoftwareOutDevice ;
	CComboBox	m_selectMIDIHardwareInDevice ;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStudioMaxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CStudioMaxDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChangeMIDIDevice();
	afx_msg void AppFinished();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	virtual void OnCancel() ;

private:
	DataMapper m_mapper ;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STUDIOMAXDLG_H__8E791917_DF7D_11D3_8544_00A0CC3EABDD__INCLUDED_)
