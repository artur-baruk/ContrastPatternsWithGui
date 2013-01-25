// LcGUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LcGUI.h"
#include "LcGUIDlg.h"
#include "LcDriver.h"



// Timer ID constants.
const UINT ID_TIMER_SECONDS = 0x1000;

// CLcGUIDlg dialog

LcDriver lcDriver;

CTime start_time;

bool worker_active = false;
bool finished = false;



UINT MyThreadProc( LPVOID pParam )
{
    LcDriver* pObject = (LcDriver*)pParam;

   // if (pObject == NULL ||
   //     !pObject->IsKindOf(RUNTIME_CLASS(LcDriver)))
  //  return 1;   // if pObject is not valid

	pObject->run();

	worker_active = false;
	finished = true;

    return 0;   // thread completed successfully
}

string slurp(ifstream& in) {
    stringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}


// Timer Handler.
void CLcGUIDlg::OnTimer( UINT nIDEvent )
{
    // Per minute timer ticked.
    if( nIDEvent == ID_TIMER_SECONDS && worker_active)
    {
		CTime t = CTime::GetCurrentTime();
		CTimeSpan ts = t - start_time;
		long sec = ts.GetTotalSeconds();
		CString str;
		str.Format("%d",sec);
        EDIT_TIME_VAR.SetWindowTextA(str);

    }
	if(!worker_active && finished) {
		BUTTON_RUN.EnableWindow(TRUE);
		finished = false;
		ifstream filestr;
		filestr.open ("report.txt");
		string s = slurp(filestr);
		CString str(s.c_str());
		EDIT_LOG.SetWindowTextA(str);
		filestr.close();
	}
}


CLcGUIDlg::CLcGUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLcGUIDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLcGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, EDIT_TRAINING, EDIT_TRAINING_VAR);
	DDX_Control(pDX, EDIT_DESCRIPTION, EDIT_DESCRIPTION_VAR);
	DDX_Control(pDX, EDIT_OBJECTS, EDIT_OBJECTS_VAR);
	DDX_Control(pDX, EDIT_TIME, EDIT_TIME_VAR);
	DDX_Control(pDX, IDOK, BUTTON_RUN);
	DDX_Control(pDX, IDC_EDIT1, EDIT_LOG);
}

BEGIN_MESSAGE_MAP(CLcGUIDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(BTN_OPEN_TRAINING, &CLcGUIDlg::OnBnClickedOpenTraining)
	ON_BN_CLICKED(BTN_OPEN_DESCRIPTION, &CLcGUIDlg::OnBnClickedOpenDescription)
	ON_BN_CLICKED(BTN_OPEN_OBJECTS, &CLcGUIDlg::OnBnClickedOpenObjects)
	ON_BN_CLICKED(IDOK, &CLcGUIDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO1, &CLcGUIDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CLcGUIDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CLcGUIDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CLcGUIDlg::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO5, &CLcGUIDlg::OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_RADIO6, &CLcGUIDlg::OnBnClickedRadio6)
	ON_BN_CLICKED(IDC_RADIO7, &CLcGUIDlg::OnBnClickedRadio7)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CLcGUIDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CLcGUIDlg message handlers


BOOL CLcGUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CheckRadioButton(IDC_RADIO1,IDC_RADIO2, IDC_RADIO1);

	CheckRadioButton(IDC_RADIO3,IDC_RADIO4, IDC_RADIO3);

	CheckRadioButton(IDC_RADIO5,IDC_RADIO7, IDC_RADIO5);

	GetDlgItem(IDC_RADIO7)->EnableWindow(FALSE);

	SetTimer(ID_TIMER_SECONDS, 1000,NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLcGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLcGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLcGUIDlg::OnBnClickedOpenTraining()
{
	FileOpenDialog fod;
	fod.create();
	EDIT_TRAINING_VAR.SetWindowTextA(fod.ofn.lpstrFile);
	lcDriver.configuration.setTrainingDataPath(fod.ofn.lpstrFile);
}

void CLcGUIDlg::OnBnClickedOpenDescription()
{
	FileOpenDialog fod;
	fod.create();
	EDIT_DESCRIPTION_VAR.SetWindowTextA(fod.ofn.lpstrFile);
	lcDriver.configuration.setDataDescriptionPath(fod.ofn.lpstrFile);
}

void CLcGUIDlg::OnBnClickedOpenObjects()
{
	FileOpenDialog fod;
	fod.create();
	EDIT_OBJECTS_VAR.SetWindowTextA(fod.ofn.lpstrFile);
	lcDriver.configuration.setObjectsToClsfPath(fod.ofn.lpstrFile);
}

void CLcGUIDlg::OnBnClickedOk()
{
	BUTTON_RUN.EnableWindow(FALSE);
	EDIT_LOG.SetWindowTextA("");
	start_time = CTime::GetCurrentTime();
	if (!worker_active) worker_active = true;
	AfxBeginThread(MyThreadProc, &lcDriver);
}
//agrawal
void CLcGUIDlg::OnBnClickedRadio1()
{
	lcDriver.configuration.setMethod(Agrawal);
}
//orlando
void CLcGUIDlg::OnBnClickedRadio2()
{
	lcDriver.configuration.setMethod(Orlando);
}
//for all classes
void CLcGUIDlg::OnBnClickedRadio3()
{
	lcDriver.configuration.setCounterType(AllClasses);
	GetDlgItem(IDC_RADIO7)->EnableWindow(FALSE);
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO7);
	pButton->SetCheck(false);
	pButton = (CButton*)GetDlgItem(IDC_RADIO5);
	pButton->SetCheck(true);
}

//binary
void CLcGUIDlg::OnBnClickedRadio4()
{
	lcDriver.configuration.setCounterType(Binary);
	GetDlgItem(IDC_RADIO7)->EnableWindow(TRUE);
}
//off
void CLcGUIDlg::OnBnClickedRadio5()
{
	lcDriver.configuration.setOptimizationType(No);
}
//generators
void CLcGUIDlg::OnBnClickedRadio6()
{
	lcDriver.configuration.setOptimizationType(Generators);
}
//closures
void CLcGUIDlg::OnBnClickedRadio7()
{
	lcDriver.configuration.setOptimizationType(Closures);
}




void CLcGUIDlg::OnBnClickedButton1()
{
	CString src;
	EDIT_LOG.GetWindowTextA(src);
	CFile f;
	char strFilter[] = { "TXT Files (*.txt)|*.txt|All Files (*.*)|*.*||" };
	CFileDialog FileDlg(FALSE, ".TXT", NULL, 0, strFilter);
	if( FileDlg.DoModal() == IDOK )
	{
	f.Open( FileDlg.GetPathName() /*+ FileDlg.GetFileName()*/, CFile::modeCreate | CFile::modeWrite);
		CArchive ar(&f, CArchive::store);
		ar << src;
		ar.Close();
	}
	else
		return;
	f.Close();
}


