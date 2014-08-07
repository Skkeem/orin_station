
// clientDlg.h : 헤더 파일
//

#pragma once

#include "orin.h"

// CclientDlg 대화 상자
class CclientDlg : public CDialogEx
{
// 생성입니다.
public:
	CclientDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	SOCKET s;
	char buf[MAXBUF];
	char key[1];
	bool conn_flag;
	
	HANDLE hTimer;

	void TimerHandler();
	afx_msg void OnBnClickedButtonOn();
	afx_msg void OnBnClickedButtonOff();
	void DDeleteTimerQueueTimer(HANDLE TimerQueue, HANDLE Timer, HANDLE CompletionEvent);
};

void CALLBACK TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired);