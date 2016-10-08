// StudioMaxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StudioMax.h"
#include "StudioMaxDlg.h"
#include "DataMapper.h"

// #define DEVELOPER_TRACE
#ifdef DEVELOPER_TRACE
#	include <sstream>
	using namespace std ;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CStudioMaxDlg dialog

CStudioMaxDlg::CStudioMaxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStudioMaxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStudioMaxDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStudioMaxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStudioMaxDlg)
	DDX_Control(pDX, IDC_MIDI_SOFTWARE_INPUT_DEVICE, m_selectMIDISoftwareInDevice);
	DDX_Control(pDX, IDC_MIDI_SOFTWARE_OUTPUT_DEVICE, m_selectMIDISoftwareOutDevice);
	DDX_Control(pDX, IDC_MIDI_HARDWARE_INPUT_DEVICE, m_selectMIDIHardwareInDevice);
	DDX_Control(pDX, IDC_MIDI_HARDWARE_OUTPUT_DEVICE, m_selectMIDIHardwareOutDevice);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStudioMaxDlg, CDialog)
	//{{AFX_MSG_MAP(CStudioMaxDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_MIDI_SOFTWARE_INPUT_DEVICE, OnChangeMIDIDevice)
	ON_CBN_SELCHANGE(IDC_MIDI_SOFTWARE_OUTPUT_DEVICE, OnChangeMIDIDevice)
	ON_CBN_SELCHANGE(IDC_MIDI_HARDWARE_INPUT_DEVICE, OnChangeMIDIDevice)
	ON_CBN_SELCHANGE(IDC_MIDI_HARDWARE_OUTPUT_DEVICE, OnChangeMIDIDevice)
	ON_BN_CLICKED(IDOK, AppFinished)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStudioMaxDlg message handlers

BOOL CStudioMaxDlg::OnInitDialog()
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
	
	// Get all of the MIDI devices...
	UINT numDev = ::midiInGetNumDevs() ;
	for( UINT idx = 0; idx < numDev; ++idx )
	{
		MIDIINCAPS caps ;
		::midiInGetDevCaps(idx, &caps, sizeof(MIDIINCAPS) ) ;
		m_selectMIDIHardwareInDevice.AddString(caps.szPname) ;
		m_selectMIDISoftwareInDevice.AddString(caps.szPname) ;
	}
	numDev = ::midiOutGetNumDevs() ;
	for( idx = 0; idx < numDev; ++idx )
	{
		MIDIOUTCAPS caps ;
		::midiOutGetDevCaps(idx, &caps, sizeof(MIDIOUTCAPS) ) ;
		m_selectMIDISoftwareOutDevice.AddString(caps.szPname) ;
		m_selectMIDIHardwareOutDevice.AddString(caps.szPname) ;
	}

	m_selectMIDIHardwareInDevice.SetCurSel(AfxGetApp()->GetProfileInt(_T("Ports"), _T("InFromStudioMix"), -1)) ;
	m_selectMIDIHardwareOutDevice.SetCurSel(AfxGetApp()->GetProfileInt(_T("Ports"), _T("OutToStudioMix"), -1)) ;
	m_selectMIDISoftwareInDevice.SetCurSel(::AfxGetApp()->GetProfileInt(_T("Ports"), _T("InFromProgram"), -1)) ;
	m_selectMIDISoftwareOutDevice.SetCurSel(::AfxGetApp()->GetProfileInt(_T("Ports"), _T("OutToProgram"), -1)) ;
	
	OnChangeMIDIDevice() ;
		
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CStudioMaxDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CStudioMaxDlg::OnPaint() 
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
HCURSOR CStudioMaxDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CStudioMaxDlg::OnChangeMIDIDevice() 
{
	int selHWInIdx = m_selectMIDIHardwareInDevice.GetCurSel() ;
	int selSWOutIdx = m_selectMIDISoftwareOutDevice.GetCurSel() ;
	if( selHWInIdx >= 0 && selSWOutIdx >= 0  )
	{
		if(! m_mapper.MapStudioMixToProgram(selHWInIdx, selSWOutIdx) )
		{
			m_selectMIDIHardwareInDevice.SetCurSel(-1) ;
			m_selectMIDISoftwareOutDevice.SetCurSel(-1) ;
		}
		else
		{
			CString tempText ;
			m_selectMIDIHardwareInDevice.GetWindowText(tempText) ;
			TRACE("Opened %s for Hardware Input.\n", static_cast<const char*>(tempText) ) ;
			m_selectMIDISoftwareOutDevice.GetWindowText(tempText) ;
			TRACE("Opened %s for Software Output.\n", static_cast<const char*>(tempText) ) ;
		}
	}

	int selSWInIdx = m_selectMIDISoftwareInDevice.GetCurSel() ;
	int selHWOutIdx = m_selectMIDIHardwareOutDevice.GetCurSel() ;
	if( selSWInIdx >= 0 && selHWOutIdx >= 0  )
	{
		if(! m_mapper.MapProgramToStudioMix(selSWInIdx, selHWOutIdx) )
		{
			m_selectMIDISoftwareInDevice.SetCurSel(-1) ;
			m_selectMIDIHardwareOutDevice.SetCurSel(-1) ;
		}
		else
		{
			CString tempText ;
			m_selectMIDISoftwareInDevice.GetWindowText(tempText) ;
			TRACE("Opened %s for Software Input.\n", static_cast<const char*>(tempText) ) ;
			m_selectMIDIHardwareOutDevice.GetWindowText(tempText) ;
			TRACE("Opened %s for Hardware Output.\n", static_cast<const char*>(tempText) ) ;
		}
	}
	
	
}

void CStudioMaxDlg::AppFinished() 
{
	::AfxGetApp()->WriteProfileInt(_T("Ports"), _T("InFromStudioMix"), m_selectMIDIHardwareInDevice.GetCurSel() ) ;
	::AfxGetApp()->WriteProfileInt(_T("Ports"), _T("OutToStudioMix"), m_selectMIDIHardwareOutDevice.GetCurSel() ) ;

	::AfxGetApp()->WriteProfileInt(_T("Ports"), _T("InFromProgram"), m_selectMIDISoftwareInDevice.GetCurSel() ) ;
	::AfxGetApp()->WriteProfileInt(_T("Ports"), _T("OutToProgram"), m_selectMIDISoftwareOutDevice.GetCurSel() ) ;

	CDialog::OnOK() ;
}
void CStudioMaxDlg::OnCancel()
{
	AppFinished() ;
}
