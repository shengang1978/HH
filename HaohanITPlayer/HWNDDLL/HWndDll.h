// HWndDll.h : main header file for the HWNDDLL DLL
//

#if !defined(AFX_HWNDDLL_H__7F6F2EAF_F61F_445F_8F41_71471C927143__INCLUDED_)
#define AFX_HWNDDLL_H__7F6F2EAF_F61F_445F_8F41_71471C927143__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHWndDllApp
// See HWndDll.cpp for the implementation of this class
//

class CHWndDllApp : public CWinApp
{
public:
	CHWndDllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHWndDllApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CHWndDllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HWNDDLL_H__7F6F2EAF_F61F_445F_8F41_71471C927143__INCLUDED_)
