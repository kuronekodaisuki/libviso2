
// CaptureDlg.h : header file
//

#pragma once
#include "afxdialogex.h"
#include <opencv2/videoio.hpp>
#include "../WebCam.h"

#define AVERAGE_SIZE	10

//using namespace std;

// CaptureDlg dialog
class CaptureDlg : public CDialogEx
{
// Construction
public:
	CaptureDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	CButton m_cam1;
	CButton m_cam2;
	CButton m_cam3;
	CButton m_cam4;
	CButton m_oneshot;
	CButton m_start;
	CStatic m_status;
	afx_msg void OnBnClickedPreview();
	afx_msg void OnBnClickedOneshot();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedCam1();
	afx_msg void OnBnClickedCam2();
	afx_msg void OnBnClickedCam3();
	afx_msg void OnBnClickedCam4();

// Implementation
protected:
	HICON m_hIcon;
	std::vector<WebCam> camera;
	UINT_PTR pTimer;
	double value[AVERAGE_SIZE];
	int iValue;

	void UpdateValue(double value);
	void DoOpen();
	void DoStitch();
	void DoPreview();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
};
