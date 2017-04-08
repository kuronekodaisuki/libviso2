
// CaptureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Capture.h"
#include "CaptureDlg.h"


#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\video.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_STITCH	1
#define TIMER_PREVIEW	2

const float WIDTH = 640.0;
const float HEIGHT = 480.0;
const float FPS = 30.0;
const char STITCHED[] = "Stitched";


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CaptureDlg dialog



CaptureDlg::CaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CaptureDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAM1, m_cam1);
	DDX_Control(pDX, IDC_CAM2, m_cam2);
	DDX_Control(pDX, IDC_CAM3, m_cam3);
	DDX_Control(pDX, IDC_CAM4, m_cam4);
	DDX_Control(pDX, IDC_ONESHOT, m_oneshot);
	DDX_Control(pDX, IDOK, m_start);
	DDX_Control(pDX, IDC_STATUS, m_status);
}

BEGIN_MESSAGE_MAP(CaptureDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PREVIEW, &CaptureDlg::OnBnClickedPreview)
	ON_BN_CLICKED(IDC_ONESHOT, &CaptureDlg::OnBnClickedOneshot)
	ON_BN_CLICKED(IDOK, &CaptureDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_STOP, &CaptureDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_CAM1, &CaptureDlg::OnBnClickedCam1)
	ON_BN_CLICKED(IDC_CAM2, &CaptureDlg::OnBnClickedCam2)
	ON_BN_CLICKED(IDC_CAM3, &CaptureDlg::OnBnClickedCam3)
	ON_BN_CLICKED(IDC_CAM4, &CaptureDlg::OnBnClickedCam4)
END_MESSAGE_MAP()

typedef struct {
	CWnd	*pDlg;
	int		camID;
} PARAM;

// enum camera callback
static void callback(void *item, char *pDeviceName)
{
	TCHAR buffer[80] = L"";
	size_t wlen;
	PARAM *param = (PARAM *)item;

	if (param->camID <= IDC_CAM4) {
		CButton *item = (CButton*)param->pDlg->GetDlgItem(param->camID);

		item->EnableWindow();
		item->SetCheck(BST_CHECKED);
		item->ShowWindow(SW_SHOWNORMAL);
		mbstowcs_s(&wlen, buffer, pDeviceName, 80);
		// Buffaloのカメラの行末にCR,LFが入っているので、これを除去
		for (wlen--; 0 < wlen; wlen--) {
			if (buffer[wlen] == 13 || buffer[wlen] == 10)
				buffer[wlen] = 0;
		}
		item->SetWindowText(buffer);
	}
	param->camID++;
}


// CaptureDlg message handlers

BOOL CaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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
	PARAM param = { this, IDC_CAM1 };
	int count = WebCam::EnumDevices((PENUMDEVICE)&callback, &param);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CaptureDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CaptureDlg::OnBnClickedCam1()
{
	m_start.EnableWindow(FALSE);
}


void CaptureDlg::OnBnClickedCam2()
{
	m_start.EnableWindow(FALSE);
}


void CaptureDlg::OnBnClickedCam3()
{
	m_start.EnableWindow(FALSE);
}


void CaptureDlg::OnBnClickedCam4()
{
	m_start.EnableWindow(FALSE);
}

// 平均値を算出して表示
void CaptureDlg::UpdateValue(double v)
{
	CString buffer;
	double sum = 0;
	value[iValue] = v;
	iValue = (iValue + 1) % AVERAGE_SIZE;

	for (int i = 0; i < AVERAGE_SIZE; i++)
	{
		sum += value[i];
	}
	buffer.Format(L"%.2fmsec", sum / AVERAGE_SIZE);
	m_status.SetWindowText(buffer);
}

void CaptureDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_STITCH:
		//DoStitch();
		break;

	case TIMER_PREVIEW:
		DoPreview();
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

// カメラ開始
void CaptureDlg::DoOpen()
{
	//if (fileOpened)
	//{
	//	return;
	//}
	camera.clear();
	for (int id = IDC_CAM1; id <= IDC_CAM4; id++)
	{
		CButton *item = (CButton*)GetDlgItem(id);
		if (item->GetCheck() == BST_CHECKED)
		{
			WCHAR	wname[80];
			char name[80];// , buffer[100];
			size_t len;
			bool res;
			item->GetWindowText(wname, 80);
			wcstombs_s(&len, name, 80, wname, 80);

			WebCam cam = WebCam();
			res = cam.open(id - IDC_CAM1);
			if (res) {
				CString buffer;
				res = cam.set(cv::CAP_PROP_FRAME_WIDTH, WIDTH);
				res = cam.set(cv::CAP_PROP_FRAME_HEIGHT, HEIGHT);
				buffer.Format(L"%d %s", id - IDC_CAM1 + 1, name);
				res = cam.set(cv::CAP_PROP_FPS, FPS);
				res = cam.set(cv::CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
				cam.SetName(name);
				camera.push_back(cam);
			}
		}
	}
}

void CaptureDlg::OnBnClickedOk()
{
	if (2 <= camera.size())
	{
		/*
		if (m_save.GetCheck() == BST_CHECKED)
		{
		static TCHAR BASED_CODE szFilter[] = _T("AVI (*.avi)|*.avi|All Files (*.*)|*.*||");
		CFileDialog dlg(FALSE, L"avi", L"save.avi", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
		if (dlg.DoModal() == IDOK)
		{
		char name[80];
		size_t len;
		int fourcc = CV_FOURCC('I', 'Y', 'U', 'V');	//CV_FOURCC('M','J','P','G');	// Motion JPEG
		CString filename = dlg.GetFileName();
		wcstombs_s(&len, name, 80, (LPCWSTR)filename, 80);
		//if (writer.open(name, fourcc, FPS / 2, imageSize) != true)
		//{
		//	MessageBox(L"Cant save");
		//}
		}
		else
		{
		return;
		}
		}
		*/
		pTimer = SetTimer(TIMER_STITCH, 30, NULL);	// 30msec
	}
	else
	{
		MessageBox(L"カメラが少ないためステッチできません");
	}
}

void CaptureDlg::OnBnClickedStop()
{
	KillTimer(pTimer);
	//destroyAllWindows();
	for (size_t i = 0; i < camera.size(); i++)
	{
		//	camera[i].release();
	}
	//if (writer.isOpened())
	//{
	//	writer.release();
	//}
}

void CaptureDlg::DoPreview()
{
	for (size_t i = 0; i < camera.size(); i++)
	{
		cv::Mat image;
		camera[i] >> image;
		if (image.empty() == false) {
			imshow(camera[i].GetName(), image);
		}
	}
}