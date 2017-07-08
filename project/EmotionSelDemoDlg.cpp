// EmotionSelDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EmotionSelDemo.h"
#include "EmotionSelDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_EMOTIONWND	9001
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEmotionSelDemoDlg dialog

CEmotionSelDemoDlg::CEmotionSelDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEmotionSelDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEmotionSelDemoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEmotionSelDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEmotionSelDemoDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEmotionSelDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CEmotionSelDemoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEL_EMOTION, OnSelEmotion)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_WM_NCLBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEmotionSelDemoDlg message handlers

BOOL CEmotionSelDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	TCHAR szModulePath[MAX_PATH], szDir[MAX_PATH];
	::GetModuleFileName(NULL, szModulePath, MAX_PATH);
	LPTSTR pCh = _tcsrchr(szModulePath, _T('\\'));
	if(pCh != NULL)
		pCh[1] = 0;

	this->m_pEmotionWnd = new CEmotionWnd();
	this->m_pEmotionWnd->CreateWnd(this, IDC_EMOTIONWND);

	LPCTSTR pDirNames[] = 
	{
		_T("Face"), _T("栗子猴表情包") 
	};
	int index;
	for(index = 0; index < sizeof(pDirNames)/sizeof(pDirNames[0]); ++index)
	{
		_stprintf(szDir, _T("%s%s"), szModulePath, pDirNames[index]);
		this->m_pEmotionWnd->LoadEmotions(szDir, TRUE);
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEmotionSelDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEmotionSelDemoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEmotionSelDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CEmotionSelDemoDlg::OnSelEmotion() 
{
	// TODO: Add your control notification handler code here
	RECT rc;
	CWnd *pBtn = this->GetDlgItem(IDC_SEL_EMOTION);
	pBtn->GetWindowRect(&rc);
	this->m_pEmotionWnd->ShowOrHide(rc.left, rc.top);
}

BOOL CEmotionSelDemoDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	LPNMHDR pHdr = (LPNMHDR)lParam;
	switch(pHdr->idFrom)
	{
	case IDC_EMOTIONWND:
		{
			LPNMHDR_EMOTION pHdr2 = (LPNMHDR_EMOTION)lParam;
			this->SetDlgItemText(IDC_SHORTNAME, pHdr2->szShortName);
			this->SetDlgItemText(IDC_FULLPATH, pHdr2->szFullPath);
		}
		break;
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CEmotionSelDemoDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	this->m_pEmotionWnd->DestroyWindow();
	delete this->m_pEmotionWnd;
	this->m_pEmotionWnd = NULL;
}

void CEmotionSelDemoDlg::OnTest() 
{
	// TODO: Add your control notification handler code here
	if(this->m_pEmotionWnd->IsWindowVisible())
		this->m_pEmotionWnd->ShowWindow(SW_HIDE);

	this->SetDlgItemText(IDC_FULLPATH, 
		_T("Test button is clicked, and ...\r\n")
		_T("MY BLOG:\r\n")
		_T("http://www.cnblogs.com/hoodlum1980"));
}

void CEmotionSelDemoDlg::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(this->m_pEmotionWnd->IsWindowVisible())
		this->m_pEmotionWnd->ShowWindow(SW_HIDE);

	CDialog::OnNcLButtonDown(nHitTest, point);
}

void CEmotionSelDemoDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(this->m_pEmotionWnd->IsWindowVisible())
		this->m_pEmotionWnd->ShowWindow(SW_HIDE);

	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CEmotionSelDemoDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);

	//The EN_SETFOCUS notification message is sent when an edit control
	//receives the keyboard focus. The parent window of the edit control
	//receives this notification message through a WM_COMMAND message. 
	switch(wmId)
	{
	case IDC_SHORTNAME:
	case IDC_FULLPATH:
		{
			if(wmEvent == EN_SETFOCUS)
			{
				if(this->m_pEmotionWnd->IsWindowVisible())
					this->m_pEmotionWnd->ShowWindow(SW_HIDE);
			}
		}
		break;
	}
	return CDialog::OnCommand(wParam, lParam);
}
