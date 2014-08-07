
// clientDlg.h : ��� ����
//

#pragma once

#include "orin.h"

// CclientDlg ��ȭ ����
class CclientDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CclientDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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