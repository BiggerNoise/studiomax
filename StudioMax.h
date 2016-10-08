// StudioMax.h : main header file for the STUDIOMAX application
//

#if !defined(AFX_STUDIOMAX_H__8E791915_DF7D_11D3_8544_00A0CC3EABDD__INCLUDED_)
#define AFX_STUDIOMAX_H__8E791915_DF7D_11D3_8544_00A0CC3EABDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CStudioMaxApp:
// See StudioMax.cpp for the implementation of this class
//

class CStudioMaxApp : public CWinApp
{
public:
	CStudioMaxApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStudioMaxApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CStudioMaxApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STUDIOMAX_H__8E791915_DF7D_11D3_8544_00A0CC3EABDD__INCLUDED_)
