// EmotionSelDemoDlg.h : header file
//

#if !defined(AFX_EMOTIONSELDEMODLG_H__A344B0C7_8737_47FC_8FEA_2A81474AA021__INCLUDED_)
#define AFX_EMOTIONSELDEMODLG_H__A344B0C7_8737_47FC_8FEA_2A81474AA021__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EmotionWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CEmotionSelDemoDlg dialog

class CEmotionSelDemoDlg : public CDialog
{
// Construction
public:
	CEmotionSelDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEmotionSelDemoDlg)
	enum { IDD = IDD_EMOTIONSELDEMO_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEmotionSelDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CEmotionWnd* m_pEmotionWnd;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEmotionSelDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelEmotion();
	afx_msg void OnDestroy();
	afx_msg void OnTest();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMOTIONSELDEMODLG_H__A344B0C7_8737_47FC_8FEA_2A81474AA021__INCLUDED_)
