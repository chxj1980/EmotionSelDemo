// EmotionWnd.cpp : implementation file
//

#include "stdafx.h"
#include "EmotionSelDemo.h"
#include "EmotionWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//动画定时器
#define TIMERID_SHOWNEXTFRAME	101
#define TIMERID_TIP_SHOW		102
#define TIMERID_TIP_HIDE		103

CEmotionInfo::CEmotionInfo()
{
	memset(this->m_FileName, 0, sizeof(this->m_FileName));
	memset(this->m_FileDesc, 0, sizeof(this->m_FileDesc));
	this->m_pImg = new CxImage();
	this->m_pImg->SetRetreiveAllFrames(TRUE);
}

CEmotionInfo::~CEmotionInfo()
{
	delete this->m_pImg;
	this->m_pImg = NULL;
}

//获取文件名（不含所在文件夹）
void CEmotionInfo::GetShortFileName(LPTSTR pBuf) const
{
	LPTSTR pCh = (LPTSTR)_tcsrchr(this->m_FileName, _T('\\'));
	if(pCh != NULL)
		_tcscpy(pBuf, pCh + 1);
}

//获取完整路径
void CEmotionInfo::GetFullPath(LPTSTR pBuf) const
{
	_tcscpy(pBuf, this->m_FileName);
}

void CEmotionInfo::SetFullPath(LPCTSTR pPath)
{
	//_tcscpy(this->m_FileName, pPath);

	// 本地文件URL参考:
	// http://en.wikipedia.org/wiki/File_URI_scheme

	_stprintf(this->m_FileName, _T("file:///%s"), pPath);
}

void CEmotionInfo::GetDescription(LPTSTR pBuf) const
{
	_tcscpy(pBuf, this->m_FileDesc);
}

void CEmotionInfo::SetDescription(LPCTSTR pDesc)
{
	if(pDesc == NULL)
		return;
	size_t len = _tcslen(pDesc);
	size_t nBufSize = sizeof(this->m_FileDesc)/sizeof(this->m_FileDesc[0]);

	if(len > nBufSize - 1)
	{
		_tcsncpy(this->m_FileDesc, pDesc, nBufSize - 4);
		this->m_FileDesc[nBufSize - 4] = _T('.');
		this->m_FileDesc[nBufSize - 3] = _T('.');
		this->m_FileDesc[nBufSize - 2] = _T('.');
		this->m_FileDesc[nBufSize - 1] = 0;
	}
	else
	{
		_tcscpy(this->m_FileDesc, pDesc);
	}
}

BOOL CEmotionInfo::LoadImg(LPCTSTR pFullPath)
{
	return this->m_pImg->Load(pFullPath, CXIMAGE_FORMAT_UNKNOWN);
}

void CEmotionInfo::CalcDrawInfo(int nCellWidth, int nCellHeight, LPINT pDestWidth, LPINT pDestHeight) const
{
	int nImgWidth = this->m_pImg->GetWidth();
	int nImgHeight = this->m_pImg->GetHeight();
	if(nImgWidth > nCellWidth - 4 || nImgWidth > nCellHeight - 4)
	{
		double k1 = (nCellWidth - 4) * 1.0 / nImgWidth;
		double k2 = (nCellHeight - 4) * 1.0 / nImgHeight;
		double k = min(k1, k2);
		*pDestWidth = (int)(nImgWidth * k) + 1;
		*pDestHeight = (int)(nImgHeight * k) + 1;
	}
	else
	{
		*pDestWidth = nImgWidth;
		*pDestHeight = nImgHeight;
	}
}

UINT CEmotionInfo::GetDelay(int iFrame)
{
	int nDelay = 160;
	CxImage* pFrame = this->m_pImg->GetFrame(iFrame);
	if(pFrame != NULL)
	{
		nDelay = max(160, pFrame->GetFrameDelay());
	}
	return nDelay;
}

/////////////////////////////////////////////////////////////////////////////
// CEmotionWnd

CEmotionWnd::CEmotionWnd()
{
}

CEmotionWnd::~CEmotionWnd()
{
}


BEGIN_MESSAGE_MAP(CEmotionWnd, CWnd)
	//{{AFX_MSG_MAP(CEmotionWnd)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEmotionWnd message handlers

void CEmotionWnd::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	TOOLINFO ti = { 0 };
	ti.cbSize = sizeof(TOOLINFO);
	ti.hwnd = this->GetSafeHwnd();
	ti.uId = 0;
	::SendMessage(this->m_hToolTip, TTM_DELTOOL, 0, (LPARAM)(&ti));
	::DestroyWindow(this->m_hToolTip);
	this->m_hToolTip = NULL;

	this->m_pFont->DeleteObject();
	delete this->m_pFont;
	this->m_pFont = NULL;

	this->m_pBrush->DeleteObject();
	delete this->m_pBrush;
	this->m_pBrush = NULL;

	int index;
	for(index = 0; index < 3; index++)
	{
		this->m_pMemDC[index]->SelectObject(this->m_pOldBm[index]);
		this->m_pMemDC[index]->DeleteDC();
		delete this->m_pMemDC[index];
		this->m_pMemDC[index] = NULL;

		this->m_pMemBm[index]->DeleteObject();
		delete this->m_pMemBm[index];
		this->m_pMemBm[index] = NULL;

		this->m_pOldBm[index] = NULL;
	}

	//
	// !! DONOT FORGET DELETE THE OBJECTS ON_THE_HEAP !!
	//

	this->FreeEmotions();
	delete this->m_pEmotions;
	this->m_pEmotions = NULL;
}

BOOL CEmotionWnd::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	//return CWnd::OnEraseBkgnd(pDC);
	return TRUE;
}

void CEmotionWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	this->SetCapture();
	int nImg, nCol;
	this->m_nClicked = this->HitTest(point.x, point.y, &nImg, &nCol);
	this->m_lastHover_idx = nImg;

	LPRECT pRc = this->GetLogicBtnPos(this->m_nClicked);
	if(pRc != NULL)
	{
		this->DrawLogicButton(this->m_nClicked, TRUE);
		this->InvalidateRect(pRc, FALSE);
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CEmotionWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	ReleaseCapture();

	int nImg, nCol;
	int nHit = this->HitTest(point.x, point.y, &nImg, &nCol);
	BOOL bUpdatePage = FALSE;
	BOOL bCustomClicked = FALSE;
	RECT rcUpdate, rc1;

	//选择了某个表情
	if(this->m_nClicked == nHit
		&& nHit == HIT_IMAGE
		&& nImg == this->m_lastHover_idx
		&& nImg >= 0)
	{
		NMHDR_EMOTION data;
		memset(&data, 0, sizeof(NMHDR_EMOTION));
		data.header.hwndFrom = this->GetSafeHwnd();
		data.header.idFrom = this->m_nID;
		CEmotionInfo *pEmotion = this->m_pEmotions->at(nImg);
		pEmotion->GetFullPath(data.szFullPath);
		pEmotion->GetShortFileName(data.szShortName);

		this->HideToolTip(TRUE);
		this->ShowWindow(SW_HIDE);
		HWND hWndOwner = this->GetOwner()->GetSafeHwnd();
		::SendMessage(hWndOwner, WM_NOTIFY, (WPARAM)(this->m_nID), (LPARAM)(&data));
		CWnd::OnLButtonUp(nFlags, point);
		return;
	}

	SetRectEmpty(&rc1);
	if(this->m_nClicked == HIT_PREV
		|| this->m_nClicked == HIT_NEXT
		|| this->m_nClicked == HIT_CUSTOM)
	{
		if(nHit == this->m_nClicked)
		{
			if(nHit == HIT_PREV && this->m_curPage > 0)
			{
				--this->m_curPage;
				bUpdatePage = TRUE;
			}
			else if(nHit == HIT_NEXT && this->m_curPage < (this->m_cPages - 1))
			{
				++this->m_curPage;
				bUpdatePage = TRUE;
			}
			else if(nHit == HIT_CUSTOM)
			{
				bCustomClicked = TRUE;
			}
		}
		this->DrawLogicButton(this->m_nClicked, FALSE);
		if(bUpdatePage)
		{
			this->OnPageChanged();
			this->DrawLogicButton(HIT_STATIC, FALSE);
			SetRect(&rc1, this->m_margin_left, this->m_margin_top,
				this->m_wnd_width - this->m_margin_right,
				this->m_wnd_height - this->m_margin_bottom);
		}
		LPRECT pRcBtn = this->GetLogicBtnPos(this->m_nClicked);
		UnionRect(&rcUpdate, pRcBtn, &rc1);
		this->InvalidateRect(&rcUpdate, FALSE);
	}
	//点击了自定义表情按钮，请自行决定如何处理
	if(bCustomClicked)
	{
		//this->OnCustomClicked();
	}
	this->m_nClicked = HIT_NULL;
	CWnd::OnLButtonUp(nFlags, point);
}

void CEmotionWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int nImgIndex, nCol, nPreviewPos;
	int nHit = this->HitTest(point.x, point.y, &nImgIndex, &nCol);

	if(nHit != this->m_nLastHit || nImgIndex != this->m_lastHover_idx)
	{
		CRgn rgn, rgn2;
		RECT rcSel, rcUpdate;
		LPRECT pRcPreview;

		//hide tooltip
		this->KillTimer(TIMERID_TIP_HIDE);
		this->HideToolTip(TRUE);

		this->GetCellRect(nImgIndex, &rcSel, 1, 1);
		UnionRect(&rcUpdate, &this->m_rcLastHover, &rcSel);
		this->UpdateView(&rcUpdate, &rcSel);
		rgn.CreateRectRgnIndirect(&rcUpdate);

		nPreviewPos = this->GetPreviewPos(nCol, this->m_lastHover_col);
		if(this->m_nPreviewPos != nPreviewPos)
		{
			//erase old preview rect;
			if(this->m_nPreviewPos != PV_HIDE)
			{
				pRcPreview = (this->m_nPreviewPos == PV_LEFT)? 
					&m_rcLeft : &m_rcRight;
				rgn2.CreateRectRgnIndirect(pRcPreview);
				rgn.CombineRgn(&rgn, &rgn2, RGN_OR);
				rgn2.DeleteObject();

				this->UpdateView(pRcPreview, NULL);
			}
		}

		//update preview
		this->KillTimer(TIMERID_SHOWNEXTFRAME);
		if(nPreviewPos != PV_HIDE)
		{
			pRcPreview = (nPreviewPos == PV_LEFT)? 
				&m_rcLeft : &m_rcRight;
			
			rgn2.CreateRectRgnIndirect(pRcPreview);
			rgn.CombineRgn(&rgn, &rgn2, RGN_OR);
			rgn2.DeleteObject();

			CEmotionInfo *pInfo = this->m_pEmotions->at(nImgIndex);
			this->m_cFrames = pInfo->GetImg()->GetNumFrames();
			this->m_curFrame = 0;
			this->DrawPreview(pRcPreview, pInfo, this->m_curFrame);
			if(this->m_cFrames > 1)
			{
				UINT nDelay = pInfo->GetDelay(this->m_curFrame);
				this->SetTimer(TIMERID_SHOWNEXTFRAME, nDelay, NULL); 
			}
		}
		this->InvalidateRgn(&rgn, FALSE);
		rgn.DeleteObject();

		CopyRect(&this->m_rcLastHover, &rcSel);
		this->m_nLastHit = nHit;
		this->m_lastHover_idx = nImgIndex;
		this->m_lastHover_col = nCol;
		this->m_nPreviewPos = nPreviewPos;

		//鼠标在此停留一会后显示tip
		if(nImgIndex >= 0)
		{
			this->SetTimer(TIMERID_TIP_SHOW, 800, NULL);
		}
	}

	//TrackMouseEvent
	if(this->m_pFuncTME != NULL)
		this->m_pFuncTME(&this->m_TME);

	CWnd::OnMouseMove(nFlags, point);
}

void CEmotionWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	dc.BitBlt(0, 0, this->m_wnd_width, this->m_wnd_height, 
		this->m_pMemDC[0], 0, 0, SRCCOPY);

	// Do not call CWnd::OnPaint() for painting messages
}

void CEmotionWnd::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default	
	switch(nIDEvent)
	{
	case TIMERID_SHOWNEXTFRAME:
		{
			this->KillTimer(nIDEvent);
			if(this->m_lastHover_idx < 0)
				break;

			this->m_curFrame++;
			if(this->m_curFrame >= this->m_cFrames)
				this->m_curFrame = 0;

			CEmotionInfo *pInfo = this->m_pEmotions->at(this->m_lastHover_idx);
			LPRECT pRcPreview = (this->m_nPreviewPos == PV_LEFT) ? &this->m_rcLeft : &this->m_rcRight;
			this->DrawPreview(pRcPreview, pInfo, this->m_curFrame);
			this->InvalidateRect(pRcPreview, FALSE);
			
			UINT nDelay = pInfo->GetDelay(this->m_curFrame);
			this->SetTimer(nIDEvent, nDelay, NULL);
		}
		break;

	case TIMERID_TIP_SHOW:
		{
			this->KillTimer(nIDEvent);
			this->SetTimer(TIMERID_TIP_HIDE, 4000, NULL);
			if(this->m_lastHover_idx >= 0)
			{
				TCHAR szFullPath[MAX_PATH + 32], szTip[384];
				RECT rcWnd;
				CEmotionInfo *pInfo = this->m_pEmotions->at(this->m_lastHover_idx);
				pInfo->GetFullPath(szFullPath);
				int nFrameCount = pInfo->GetImg()->GetNumFrames();
				_stprintf(szTip, _T("%s\r\nFrameCount = %ld"), szFullPath, nFrameCount);

				this->GetWindowRect(&rcWnd);
				this->ShowToolTip(szTip, 
					rcWnd.left + this->m_rcLastHover.left + 8,
					rcWnd.top + this->m_rcLastHover.bottom + 16);	
			}
		}
		break;

	case TIMERID_TIP_HIDE:
		{
			this->KillTimer(nIDEvent);
			this->HideToolTip(FALSE);
		}
		break;
	}

	CWnd::OnTimer(nIDEvent);
}

void CEmotionWnd::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	if(pNewWnd->GetSafeHwnd() == this->m_hToolTip)
		return;

	CWnd* pTopParent = NULL;
	if(pNewWnd != NULL)
	{
		pTopParent = pNewWnd->GetTopLevelParent();	
	}
	
	CWnd* pMyOwner = this->GetOwner();
	if(pTopParent != pMyOwner)
		this->ShowWindow(SW_HIDE);
}

void CEmotionWnd::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if(!bShow)
	{
		this->KillTimer(TIMERID_TIP_SHOW);
		this->KillTimer(TIMERID_TIP_HIDE);
		this->HideToolTip(TRUE);
	}
}

//
// Protected Functions (Helper Funcs)
//

void CEmotionWnd::ClearHoverData()
{
	this->m_lastHover_idx = -1;
	this->m_lastHover_col = -1;
	this->m_nLastHit = HIT_NULL;
	//this->m_nClicked = HIT_NULL;
	this->m_nPreviewPos = PV_HIDE;
	SetRectEmpty(&this->m_rcLastHover);
}

void CEmotionWnd::DrawLogicButton(int nHit, BOOL bPushed)
{
	LPRECT pRc;
	CString strText;
	switch(nHit)
	{
	case HIT_PREV:
		pRc = &this->m_rcPrev;
		strText.Format(_T("上一页"));
		break;
	
	case HIT_NEXT:
		pRc = &this->m_rcNext;
		strText.Format(_T("下一页"));
		break;
	
	case HIT_STATIC:
		pRc = &this->m_rcStatic;
		strText.Format(_T("[%ld/%ld]"), this->m_curPage + 1, this->m_cPages);
		break;

	case HIT_CUSTOM:
		pRc = &this->m_rcCustom;
		strText.Format(_T("自定义表情"));
		break;

	default:
		return;
	}
	RECT rc;
	CopyRect(&rc, pRc);
	if(bPushed)
	{
		rc.left++;
		rc.top++;
	}
	this->UpdateView(pRc, NULL);
	this->m_pMemDC[0]->SetTextColor(RGB(11, 99, 181));
	CGdiObject *pOldFont = this->m_pMemDC[0]->SelectObject(this->m_pFont);
	this->m_pMemDC[0]->DrawText(strText, &rc, DT_SINGLELINE | DT_NOCLIP);
	//this->m_pMemDC[0]->FrameRect(pRc, this->m_pBrush);
	this->m_pMemDC[0]->SelectObject(pOldFont);
}

void CEmotionWnd::DrawPreview(LPRECT pRc, CEmotionInfo *pEmotion, int iFrame)
{
	CBrush brush;
	brush.CreateStockObject(WHITE_BRUSH);
	this->m_pMemDC[0]->FillRect(pRc, &brush);
	this->m_pMemDC[0]->FrameRect(pRc, this->m_pBrush);

	int dest_cx, dest_cy, cellWidth, cellHeight;
	CxImage *pFrame = pEmotion->GetImg()->GetFrame(iFrame);
	if(pFrame != NULL)
	{
		cellWidth = pRc->right - pRc->left - 1;
		cellHeight = pRc->bottom - pRc->top - 1;
		pEmotion->CalcDrawInfo(cellWidth, cellHeight, &dest_cx, &dest_cy);

		HDC hdc = this->m_pMemDC[0]->GetSafeHdc();
		pFrame->Draw(hdc, 
			pRc->left + (cellWidth - dest_cx + 1)/2,
			pRc->top + (cellHeight - dest_cy + 1)/2,
			dest_cx, dest_cy);
	}
}

//给出表情的绝对索引，返回其所在的 Bounds
//dx/dy: 向内收缩的像素值
void CEmotionWnd::GetCellRect(int nImgIndex, LPRECT pRc, int dx, int dy)
{
	if(nImgIndex < 0)
	{
		SetRectEmpty(pRc);
		return;
	}

	//转换到相对值
	int iRelative = nImgIndex  - nImgIndex / EW_PageSize * EW_PageSize;
	int iRow = iRelative / EW_ColCount;
	int iCol = iRelative - iRow * EW_ColCount;

	pRc->left = this->m_margin_left + iCol * EW_CellSize + dx;
	pRc->top = this->m_margin_top + iRow * EW_CellSize + dy;
	pRc->right = pRc->left + EW_CellSize - dx;
	pRc->bottom = pRc->top + EW_CellSize - dy;
}

int CEmotionWnd::GetPreviewPos(int nCurCol, int nLastCol)
{
	if(nCurCol < 0)
		return PV_HIDE;

	if(nCurCol < 4 || (nCurCol == 4 && nCurCol == nLastCol))
		return PV_RIGHT;

	//col: 00 01 02 03 04 05 06 07 | 08 09 10 11 12 13 14
	if(nCurCol > (EW_ColCount - 5) || 
		(nCurCol == EW_ColCount - 5 && nCurCol == nLastCol))
		return PV_LEFT;

	//保持位置不变
	if(nCurCol >= 0 && nLastCol >= 0)
		return this->m_nPreviewPos;

	return (nCurCol < EW_ColCount/2) ? PV_RIGHT : PV_LEFT;
}

LPRECT CEmotionWnd::GetLogicBtnPos(int nBtn)
{
	LPRECT pRc = NULL;
	switch(nBtn)
	{
	case HIT_PREV: pRc = &this->m_rcPrev; break;
	case HIT_NEXT: pRc = &this->m_rcNext; break;
	case HIT_STATIC: pRc = &this->m_rcStatic; break;
	case HIT_CUSTOM: pRc = &this->m_rcCustom; break;
	}
	return pRc;
}

int CEmotionWnd::HitTest(int x, int y, LPINT pImgIndex, LPINT pCol)
{
	POINT pt;
	pt.x = x;
	pt.y = y;

	*pImgIndex = -1;
	*pCol = -1;
	if(PtInRect(&this->m_rcPrev, pt))
		return HIT_PREV;

	if(PtInRect(&this->m_rcNext, pt))
		return HIT_NEXT;

	if(PtInRect(&this->m_rcCustom, pt))
		return HIT_CUSTOM;

	if(x < this->m_margin_left
		|| x >= this->m_margin_left + EW_CellSize * EW_ColCount
		|| y < this->m_margin_top
		|| y >= this->m_margin_top + EW_CellSize * EW_RowCount)
	{
		return HIT_NULL;
	}

	int iRow = (y - this->m_margin_top) / EW_CellSize;
	int iCol = (x - this->m_margin_left) / EW_CellSize;
	int iImg = EW_PageSize * this->m_curPage + iRow * EW_ColCount + iCol;
	if(iImg < this->m_pEmotions->size())
	{
		*pImgIndex = iImg;
		*pCol = iCol;
		return HIT_IMAGE;
	}
	else
		return HIT_NULL;
}

//此函数仅仅在创建窗口前调用一次
void CEmotionWnd::Init() 
{
	int nMiddleWidth = EW_CellSize * EW_ColCount;
	int nMiddleHeight = EW_CellSize * EW_RowCount;

	this->m_margin_left = 8;
	this->m_margin_top = 24;
	this->m_margin_right = 9;
	this->m_margin_bottom = 33;

	this->m_wnd_width = this->m_margin_left
		+ nMiddleWidth
		+ this->m_margin_right;

	this->m_wnd_height = this->m_margin_top
		+ nMiddleHeight
		+ this->m_margin_bottom;

	//左右缩略图矩形( 3 x 3 cells )
	this->m_rcLeft.left = this->m_margin_left + 1;
	this->m_rcLeft.top = this->m_margin_top + 1;
	this->m_rcLeft.right = this->m_rcLeft.left + EW_CellSize * 3 - 1;
	this->m_rcLeft.bottom = this->m_rcLeft.top + EW_CellSize * 3 - 1;

	this->m_rcRight.left = this->m_margin_left + EW_CellSize * (EW_ColCount - 3) + 1;
	this->m_rcRight.top = this->m_margin_top + 1;
	this->m_rcRight.right = this->m_rcRight.left + EW_CellSize * 3 - 1;
	this->m_rcRight.bottom = this->m_rcRight.top + EW_CellSize * 3 - 1;

	//蓝色画刷
	this->m_pBrush = new CBrush();
	this->m_pBrush->CreateSolidBrush(RGB(0, 0, 255));

	int index;
	HDC hdc = ::GetDC(NULL);
	CDC cdc;
	cdc.Attach(hdc);
	for(index = 0; index < 3; ++index)
	{
		this->m_pMemDC[index] = new CDC();
		this->m_pMemDC[index]->CreateCompatibleDC(&cdc);
		this->m_pMemDC[index]->SetBkMode(TRANSPARENT);
		//this->m_pMemDC[index]->SetStretchBltMode(COLORONCOLOR);
		this->m_pMemBm[index] = new CBitmap();
		this->m_pMemBm[index]->CreateCompatibleBitmap(&cdc, this->m_wnd_width, this->m_wnd_height);
		this->m_pOldBm[index] = this->m_pMemDC[index]->SelectObject(this->m_pMemBm[index]);
	}
	cdc.Detach();
	::ReleaseDC(NULL, hdc);

	//Init BkGnd Bitmap;
	RECT rc = { 0, 0, this->m_wnd_width, this->m_wnd_height };
	CBrush brush;
	brush.CreateSolidBrush(RGB(233, 246, 254));
	
	this->m_pMemDC[2]->FillRect(&rc, &brush);
	brush.DeleteObject();

	//border
	brush.CreateSolidBrush(RGB(137, 184, 228));
	this->m_pMemDC[2]->FrameRect(&rc, &brush);
	brush.DeleteObject();

	//cells's bkgnd;
	SetRect(&rc, this->m_margin_left, this->m_margin_top,
		this->m_margin_left + nMiddleWidth,
		this->m_margin_top + nMiddleHeight);
	brush.CreateStockObject(WHITE_BRUSH);
	this->m_pMemDC[2]->FillRect(&rc, &brush);

	//grid lines
	int row, col, x, y;
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, RGB(223, 230, 246));
	CGdiObject *pOldPen = this->m_pMemDC[2]->SelectObject(&pen);
	x = this->m_margin_left;
	y = this->m_margin_top;
	for(row = 0; row <= EW_RowCount; ++row)
	{
		this->m_pMemDC[2]->MoveTo(x, y); 
		this->m_pMemDC[2]->LineTo(x + nMiddleWidth, y);
		y += EW_CellSize;
	}
	y = this->m_margin_top;
	for(col = 0; col <= EW_ColCount; ++col)
	{
		this->m_pMemDC[2]->MoveTo(x, y); 
		this->m_pMemDC[2]->LineTo(x, y + nMiddleHeight);
		x += EW_CellSize;
	}
	this->m_pMemDC[2]->SelectObject(pOldPen);
	pen.DeleteObject();

	//Draw title;
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	_tcscpy(lf.lfFaceName, _T("宋体"));
	lf.lfWeight = FW_NORMAL;
	lf.lfHeight = 12;
	
	this->m_pFont = new CFont();
	this->m_pFont->CreateFontIndirect(&lf);
	CGdiObject* pOldFont = this->m_pMemDC[2]->SelectObject(this->m_pFont);
	CString strTitle(_T("<<< 在这里选择表情 >>>"));
	SetRect(&rc, 0, 0, this->m_wnd_width, this->m_margin_top);
	this->m_pMemDC[2]->SetTextColor(RGB(11, 99, 181));
	this->m_pMemDC[2]->DrawText(strTitle, &rc, DT_CENTER | DT_VCENTER| DT_SINGLELINE | DT_NOCLIP);

	strTitle.Format(_T("阿"));
	CSize size = this->m_pMemDC[2]->GetTextExtent(strTitle);
	this->m_pMemDC[2]->SelectObject(pOldFont);
	
	//“自定义表情”
	this->m_rcCustom.right = this->m_wnd_width - this->m_margin_right;
	this->m_rcCustom.left = this->m_rcCustom.right - size.cx * 5 - 2;
	this->m_rcCustom.top = this->m_wnd_height - this->m_margin_bottom/2 - size.cy/2 - 1;
	this->m_rcCustom.bottom = this->m_rcCustom.top + size.cy + 2;

	this->m_rcNext.right = this->m_rcCustom.left - 4;
	this->m_rcNext.left = this->m_rcNext.right - size.cx * 3 - 2;
	this->m_rcNext.top = this->m_rcCustom.top;
	this->m_rcNext.bottom = this->m_rcCustom.bottom;

	CopyRect(&m_rcPrev, &m_rcNext);
	OffsetRect(&m_rcPrev, -(size.cx * 3 + 4), 0);

	CopyRect(&m_rcStatic, &m_rcNext);
	OffsetRect(&m_rcStatic, -(size.cx * 3 + 4) * 2, 0);

	//Copy BkGnd's content to MemBitmap;
	this->m_pMemDC[0]->BitBlt(0, 0, this->m_wnd_width, this->m_wnd_height,
		this->m_pMemDC[2], 0, 0, SRCCOPY);
	this->m_pMemDC[1]->BitBlt(0, 0, this->m_wnd_width, this->m_wnd_height,
		this->m_pMemDC[2], 0, 0, SRCCOPY);
	
	//绘制按钮
	this->DrawLogicButton(HIT_PREV, FALSE);
	this->DrawLogicButton(HIT_NEXT, FALSE);
	this->DrawLogicButton(HIT_CUSTOM, FALSE);

	this->m_pEmotions = new vector<CEmotionInfo*>();
}

//更新MemBm2
void CEmotionWnd::OnPageChanged()
{
	this->ClearHoverData();
	this->m_pMemDC[1]->BitBlt(0, 0, this->m_wnd_width, this->m_wnd_height,
		this->m_pMemDC[2], 0, 0, SRCCOPY);

	HDC hdc = this->m_pMemDC[1]->GetSafeHdc();
	int row = 0, col = 0;
	int dest_x, dest_y, dest_cx, dest_cy;
	int nTotalCount = 0;
	
	vector<CEmotionInfo*>::iterator pos;
	if(this->m_pEmotions->size() > 0)
	{
		pos = this->m_pEmotions->begin();
		pos += this->m_curPage * EW_PageSize;
		for(; pos != this->m_pEmotions->end(); ++pos)
		{
			CxImage *pImg = (*pos)->GetImg();
			(*pos)->CalcDrawInfo(EW_CellSize, EW_CellSize, &dest_cx, &dest_cy);

			dest_x = this->m_margin_left + col * EW_CellSize
				+ (EW_CellSize - dest_cx + 1)/2;

			dest_y = this->m_margin_top + row * EW_CellSize
				+ (EW_CellSize - dest_cy + 1)/2; 

			pImg->Draw(hdc, dest_x, dest_y, dest_cx, dest_cy);
			++nTotalCount;

			++col;
			if(col == EW_ColCount)
			{
				col = 0;
				++row;
				if(row == EW_RowCount) break;
			}
		}
	}
	int cx = this->m_wnd_width - this->m_margin_left - this->m_margin_right;
	int cy = this->m_wnd_height - this->m_margin_top - this->m_margin_bottom;
	this->m_pMemDC[0]->BitBlt(this->m_margin_left, this->m_margin_top, cx, cy,
		this->m_pMemDC[1], this->m_margin_left, this->m_margin_top, SRCCOPY);
}

//更新 MemDC[0] 的内容(主要是背景的更新)
void CEmotionWnd::UpdateView(LPRECT pRcBkGnd, LPRECT pRcHover)
{
	this->m_pMemDC[0]->BitBlt(pRcBkGnd->left, 
		pRcBkGnd->top,
		pRcBkGnd->right - pRcBkGnd->left,
		pRcBkGnd->bottom - pRcBkGnd->top,
		this->m_pMemDC[1],
		pRcBkGnd->left, 
		pRcBkGnd->top, 
		SRCCOPY);

	//绘制蓝色矩形
	if(pRcHover != NULL && !IsRectEmpty(pRcHover))
	{
		this->m_pMemDC[0]->FrameRect(pRcHover, this->m_pBrush);
	}
}

void CEmotionWnd::ShowToolTip(LPCTSTR pContent, int x, int y)
{
	TCHAR szContent[512];
	TOOLINFO ti;
	
	memset(&ti, 0, sizeof(TOOLINFO));
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_TRACK | TTF_TRANSPARENT; //TTF_IDISHWND | TTF_CENTERTIP;
	ti.hwnd = this->GetSafeHwnd();
	ti.uId = 0;
	
	//隐藏tooltip
	::SendMessage(this->m_hToolTip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)0);
	
	int nToolCount = (int)::SendMessage(this->m_hToolTip, TTM_GETTOOLCOUNT, 0, 0);
	if(nToolCount <= 0)
		::SendMessage(this->m_hToolTip, TTM_ADDTOOL, 0, (LPARAM)(&ti));

	//以下代码防止缓冲区溢出 (因为 VC6 没有 _tcscpy_s ... )
	int nBufSize = sizeof(szContent) / sizeof(szContent[0]);
	if(_tcslen(pContent) > nBufSize - 4)
	{
		_tcsncpy(szContent, pContent, nBufSize - 4);
		szContent[nBufSize - 1] = 0;
		szContent[nBufSize - 2] = _T('.');
		szContent[nBufSize - 3] = _T('.');
		szContent[nBufSize - 4] = _T('.');
	}
	else
	{
		_tcscpy(szContent, pContent);
	}

	ti.lpszText = szContent;
	::SendMessage(this->m_hToolTip, TTM_UPDATETIPTEXT, 0, (LPARAM)(&ti));

	//设置指向位置
	::SendMessage(this->m_hToolTip, TTM_TRACKPOSITION, 0, MAKELONG(x, y));

	//set z-order
	::SetWindowPos(this->m_hToolTip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	//显示tooltip
	::SendMessage(this->m_hToolTip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)(&ti));
}

void CEmotionWnd::HideToolTip(BOOL bImmediately)
{

#ifndef AW_HIDE
#define AW_HIDE		0x00010000
#endif

#ifndef AW_BLEND
#define AW_BLEND	0x00080000
#endif

	if(::IsWindowVisible(this->m_hToolTip))
	{
		if(bImmediately || this->m_pAnimateWindow == NULL)
			::SendMessage(this->m_hToolTip, TTM_TRACKACTIVATE, FALSE, 0);
		else
			this->m_pAnimateWindow(this->m_hToolTip, 200, AW_HIDE | AW_BLEND);
	}
}

//
// Public Interface
//

//请不要直接 Create/CreateEx，应该调用 CreateWnd 来创建
void CEmotionWnd::CreateWnd(CWnd *pOwnerWnd, int nID)
{
	this->m_nID = nID;
	this->Init();

	LPCTSTR pClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
		::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)::GetStockObject(WHITE_BRUSH),
		NULL);

	DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
	DWORD dwStyle = WS_POPUP;
	this->CreateEx(dwExStyle, pClassName, _T("EmotionWnd"), dwStyle,
		0, 0, this->m_wnd_width, this->m_wnd_height, NULL, NULL, NULL);
	
	this->SetOwner(pOwnerWnd);

	//Create ToolTip
	this->m_hToolTip = ::CreateWindow(TOOLTIPS_CLASS, 
		NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, // | TTS_BALLOON,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, NULL, NULL);

	// set the version so we can have non buggy mouse event forwarding
	//::SendMessage(this->m_hToolTip, CCM_SETVERSION, COMCTL32_VERSION, 0);
	::SendMessage(this->m_hToolTip, TTM_SETMAXTIPWIDTH, 0, (LPARAM)300);

	//_TrackMouseEvent(&this->m_TME); 
	this->m_TME.cbSize = sizeof(TRACKMOUSEEVENT);
	this->m_TME.dwFlags = TME_LEAVE;
	this->m_TME.hwndTrack = this->GetSafeHwnd();
	this->m_TME.dwHoverTime = 0;

	//因为 VC6 SDK 太旧，所以采用下面的方法获取 TrackMouseEvent 函数地址；
	this->m_pFuncTME = NULL;
	this->m_pAnimateWindow = NULL;
	HMODULE hModule_User32 = GetModuleHandle(_T("USER32"));
	if(hModule_User32 != NULL)
	{
		this->m_pFuncTME = (PF_TRACKMOUSEEVENT)GetProcAddress(hModule_User32, "TrackMouseEvent");
		this->m_pAnimateWindow = (PF_ANIMATEWINDOW)GetProcAddress(hModule_User32, "AnimateWindow");
	}

	if(this->m_pFuncTME != NULL)
		this->m_pFuncTME(&this->m_TME);
}

void CEmotionWnd::FreeEmotions()
{
	vector<CEmotionInfo*>::iterator pos;
	if(this->m_pEmotions->size() > 0)
	{
		for(pos = this->m_pEmotions->begin(); pos != this->m_pEmotions->end(); ++pos)
		{
			delete (*pos);
		}
		this->m_pEmotions->clear();
	}
}

int CEmotionWnd::LoadEmotions(LPCTSTR szDir, BOOL bAppend)
{
	if(!bAppend)
		this->FreeEmotions();

	CEmotionInfo *pCur = NULL;
	TCHAR szFilter[MAX_PATH], szFileName[MAX_PATH];
	WIN32_FIND_DATA wfd;
	
	_stprintf(szFilter, _T("%s\\*.gif"), szDir);
	HANDLE hFind = FindFirstFile(szFilter, &wfd);
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		_stprintf(szFileName, _T("%s\\%s"), szDir, wfd.cFileName);
		pCur = new CEmotionInfo();
		pCur->SetFullPath(szFileName);
		pCur->LoadImg(szFileName);
		this->m_pEmotions->push_back(pCur);

		while (FindNextFile(hFind, &wfd) != 0) 
		{
			_stprintf(szFileName, _T("%s\\%s"), szDir, wfd.cFileName);
			pCur = new CEmotionInfo();
			pCur->SetFullPath(szFileName);
			pCur->LoadImg(szFileName);
			this->m_pEmotions->push_back(pCur);
		}
		FindClose(hFind);
	}

	this->m_cPages = (this->m_pEmotions->size() + EW_PageSize - 1) / EW_PageSize;
	this->m_curPage = 0;
	this->OnPageChanged();
	this->DrawLogicButton(HIT_STATIC, FALSE);

	return this->m_pEmotions->size();
}

void CEmotionWnd::SetID(int nID)
{
	this->m_nID = nID;
}

void CEmotionWnd::ShowOrHide(int nBaseX, int nBaseY)
{
	if(this->IsWindowVisible())
	{
		this->ShowWindow(SW_HIDE);
		return;
	}

	int left = nBaseX;
	int top = nBaseY - this->m_wnd_height - 1;

	//判断是否右侧超出屏幕位置
	/*
	//VC6 SDK 太旧，不支持以下API。。。。
	MONITORINFO mi = { 0 };
	POINT pt = { nBaseX, nBaseY };
	
	HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
	if(hMonitor != NULL)
	{
		mi.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &mi);
		if(left + this->m_wnd_width > mi.rcMonitor.right)
		{
			left = mi.rcMonitor.right - this->m_wnd_width - 4;
		}
	}
	*/
	int nScrWidth = GetSystemMetrics(SM_CXSCREEN);
	if(left + this->m_wnd_width > nScrWidth)
	{
		left = nScrWidth - this->m_wnd_width - 4;
	}
	this->SetWindowPos(NULL, left, top, 0, 0, 
		SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
}

//
// Overrided virtual Functions
//

LRESULT CEmotionWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(message == WM_MOUSELEAVE)
	{
		if(this->m_lastHover_idx >= 0)
		{
			RECT rcUpdate, rcSel;
			LPRECT pRcPreview = (this->m_nPreviewPos == PV_LEFT)? 
				&this->m_rcLeft : &this->m_rcRight;

			this->KillTimer(TIMERID_TIP_SHOW);
			this->KillTimer(TIMERID_TIP_HIDE);
			this->HideToolTip(TRUE);
			SetRectEmpty(&rcUpdate);
			this->GetCellRect(this->m_lastHover_idx, &rcSel, 0, 0);
			UnionRect(&rcUpdate, &rcSel, pRcPreview);
			this->ClearHoverData();
			this->UpdateView(&rcUpdate, NULL);
			this->InvalidateRect(&rcUpdate);
		}
	}
	return CWnd::WindowProc(message, wParam, lParam);
}
