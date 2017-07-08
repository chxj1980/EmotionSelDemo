#if !defined(AFX_EMOTIONWND_H__1F421B64_D102_41EF_87D1_7C8DF4363EC7__INCLUDED_)
#define AFX_EMOTIONWND_H__1F421B64_D102_41EF_87D1_7C8DF4363EC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CxImage\ximage.h"
#include <vector>

using namespace std;

//常量定义
enum EmotionWndConst
{
	EW_RowCount = 8,
	EW_ColCount = 15,
	EW_CellSize = 30, //ImageSize = 24 x 24
	//EmotionWnd_PageSize = 120, //RowCount x ColCount
#define EW_PageSize	(EW_RowCount * EW_ColCount)
};

enum HitTest_Result
{
	HIT_NULL	= 0,
	HIT_IMAGE	= 1,
	HIT_PREV	= 2,
	HIT_NEXT	= 3,
	HIT_STATIC	= 4,
	HIT_CUSTOM	= 5,
};

enum PreviewPos
{
	PV_HIDE		= 0,
	PV_LEFT		= 1,
	PV_RIGHT	= 2,
};

//=========================================================================
//
// WM_NOTIFY
//

typedef struct tagNMHDR_EMOTION
{
	NMHDR header;
	TCHAR szFullPath[MAX_PATH + 32];
	TCHAR szShortName[MAX_PATH];
	TCHAR szFileDesc[128];
} NMHDR_EMOTION, *LPNMHDR_EMOTION;

//
// 通知事件码
//
#define EWN_EMOTIONSELECT	101
#define EWN_CUSTOMCLICKED	102

//=========================================================================

//定义函数指针
typedef BOOL (WINAPI *PF_TRACKMOUSEEVENT)(LPTRACKMOUSEEVENT lpEventTrack);
typedef BOOL (WINAPI *PF_ANIMATEWINDOW)(HWND hwnd, DWORD dwTime, DWORD dwFlags);

//存储表情文件信息
class CEmotionInfo
{
public:
	CEmotionInfo();
	virtual ~CEmotionInfo();

public:
	void GetShortFileName(LPTSTR pBuf) const;
	void GetFullPath(LPTSTR pBuf) const;
	void SetFullPath(LPCTSTR pPath);
	void GetDescription(LPTSTR pBuf) const;
	void SetDescription(LPCTSTR pDesc);
	CxImage* GetImg() const { return m_pImg; };
	BOOL LoadImg(LPCTSTR pFullPath);
	UINT GetDelay(int iFrame);
	void CalcDrawInfo(int nCellWidth, int nCellHeight, LPINT pDestWidth, LPINT pDestHeight) const;

protected:
	CxImage *m_pImg;
	TCHAR m_FileName[MAX_PATH + 32];
	TCHAR m_FileDesc[128];
};

// EmotionWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEmotionWnd window

class CEmotionWnd : public CWnd
{
// Construction
public:
	CEmotionWnd();

// Attributes
public:
protected:
	int m_nID;
	int m_margin_left;
	int m_margin_top;
	int m_margin_right;
	int m_margin_bottom;
	int m_wnd_width;
	int m_wnd_height;
	int m_nPreviewPos;
	int m_nLastHit;
	int m_nClicked;
	int m_lastHover_idx;
	int m_lastHover_col;
	int m_cPages;
	int m_curPage;
	int m_cFrames;
	int m_curFrame;
	RECT m_rcCustom; //"自定义表情" 按钮
	RECT m_rcPrev;
	RECT m_rcNext;
	RECT m_rcStatic;
	RECT m_rcLeft;
	RECT m_rcRight;
	RECT m_rcLastHover;

	//Gdi objects
	CDC *m_pMemDC[3];
	CBitmap* m_pMemBm[3];
	CGdiObject* m_pOldBm[3];
	CBrush* m_pBrush;
	CFont* m_pFont;
	vector<CEmotionInfo*> *m_pEmotions;

	//track mouse event;
	TRACKMOUSEEVENT m_TME;
	PF_TRACKMOUSEEVENT m_pFuncTME;
	PF_ANIMATEWINDOW m_pAnimateWindow;

	//CToolTipCtrl* m_pToolTip;
	HWND m_hToolTip;

// Operations
protected:
	void ClearHoverData();
	void DrawLogicButton(int nHit, BOOL bPushed);
	void DrawPreview(LPRECT pRc, CEmotionInfo *pEmotion, int iFrame);
	int GetPreviewPos(int nCurIndex, int nLastIndex);
	void GetCellRect(int nImgIndex, LPRECT pRc, int dx, int dy);
	LPRECT GetLogicBtnPos(int nBtn);
	int HitTest(int x, int y, LPINT pImgIndex, LPINT pCol);
	void Init();
	void OnPageChanged();
	void UpdateView(LPRECT pRcBkGnd, LPRECT pRcHover);
	void ShowToolTip(LPCTSTR pContent, int x, int y);
	void HideToolTip(BOOL bImmediately);

public:
	void CreateWnd(CWnd *pOwnerWnd, int nID);
	void FreeEmotions();
	int LoadEmotions(LPCTSTR szDir, BOOL bAppend);
	void SetID(int nID);
	void ShowOrHide(int nBaseX, int nBaseY);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEmotionWnd)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEmotionWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEmotionWnd)
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMOTIONWND_H__1F421B64_D102_41EF_87D1_7C8DF4363EC7__INCLUDED_)
