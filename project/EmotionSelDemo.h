// EmotionSelDemo.h : main header file for the EMOTIONSELDEMO application
//

#if !defined(AFX_EMOTIONSELDEMO_H__2E61FAB2_7C8E_4276_9EB7_4F0686ACF264__INCLUDED_)
#define AFX_EMOTIONSELDEMO_H__2E61FAB2_7C8E_4276_9EB7_4F0686ACF264__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CEmotionSelDemoApp:
// See EmotionSelDemo.cpp for the implementation of this class
//

class CEmotionSelDemoApp : public CWinApp
{
public:
	CEmotionSelDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEmotionSelDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CEmotionSelDemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMOTIONSELDEMO_H__2E61FAB2_7C8E_4276_9EB7_4F0686ACF264__INCLUDED_)
