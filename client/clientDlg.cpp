
// clientDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "client.h"
#include "clientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IP "127.0.0.1"
#define PORT 3800

typedef struct
{
	BOOL boolStationPC;

	double dNav_Lat, dNav_Lon;
	double dNav_X, dNav_Y;
	double dNav_Roll, dNav_Pitch, dNav_Yaw;
	double dNav_U, dNav_V;

	double dThr_Port, dThr_SB;
	double dWP_Global[2], dWP_Local[2];
	int nObs;
	int dObs_info[30][3];
	double dTime;
	int nMission;

} stStationPC_Data;

stStationPC_Data orin;
int i;

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CclientDlg ��ȭ ����




CclientDlg::CclientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CclientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CclientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ON, &CclientDlg::OnBnClickedButtonOn)
	ON_BN_CLICKED(IDC_BUTTON_OFF, &CclientDlg::OnBnClickedButtonOff)
END_MESSAGE_MAP()


// CclientDlg �޽��� ó����

BOOL CclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	/*******************************************************************
	 * ���� �ʱ�ȭ
	 ******************************************************************/
	key[0] = 'k';
	conn_flag = false;
	memset(&orin, 0, sizeof(stStationPC_Data));
	GetDlgItem(IDC_BUTTON_OFF)->EnableWindow(false);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/**************************************************************************************
 **************************************************************************************
 **************************************************************************************
 *   @author : Sung Kook Kim (ingenuus@kaist.ac.kr)
 *   @date : 2014/07/29
 *   @description : Client program.
 **************************************************************************************
 **************************************************************************************
 **************************************************************************************/



void CclientDlg::OnBnClickedButtonOn()
{
	if (!conn_flag) {
		GetDlgItem(IDC_BUTTON_ON)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_OFF)->EnableWindow(true);

		Startup_winsock();
		s = Open_clientsock(IP, PORT);
		conn_flag = true;

		if (CreateTimerQueueTimer(&hTimer, NULL, TimerProc, this, 0, 100, 0) == 0) {//WT_EXECUTEINTIMERTHREAD
			//GetLastError();
			exit(0);
		}
		i = 0;
	}
}


void CclientDlg::OnBnClickedButtonOff()
{
	if (conn_flag) {
		GetDlgItem(IDC_BUTTON_OFF)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_ON)->EnableWindow(true);

		Closesocket(s);
		Cleanup_winsock();
		conn_flag = false;

		DDeleteTimerQueueTimer(NULL, hTimer, NULL);
	}
}


void CALLBACK TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	CclientDlg *pThis = (CclientDlg *)lpParameter;
	pThis->TimerHandler();
}


void CclientDlg::TimerHandler()
{
	if (conn_flag) {
		CString str;

		Rio_sendn(s, key, 1, 0);
		Rio_recvn(s, buf, sizeof(stStationPC_Data), 0);
		memcpy(&orin, buf, sizeof(stStationPC_Data));

		str.Format(_T("%f, %f"), orin.dNav_X, orin.dNav_Y);
		SetDlgItemText(IDC_STATIC_XY, str);

		str.Format(_T("%f, %f, %f"), orin.dNav_Roll, orin.dNav_Pitch, orin.dNav_Yaw);
		SetDlgItemText(IDC_STATIC_RPY, str);

		str.Format(_T("%f, %f"), orin.dNav_U, orin.dNav_V);
		SetDlgItemText(IDC_STATIC_UV, str);

		str.Format(_T("%f, %f"), orin.dThr_Port, orin.dThr_SB);
		SetDlgItemText(IDC_STATIC_PSB, str);

		str.Format(_T("%d"), orin.nObs);
		SetDlgItemText(IDC_STATIC_NOB, str);

		str.Format(_T("%d"), orin.nMission);
		SetDlgItemText(IDC_STATIC_NM, str);

		str.Format(_T("%d"), i++);
		SetDlgItemText(IDC_STATIC_CNT, str);
	}
}


void CclientDlg::DDeleteTimerQueueTimer(HANDLE TimerQueue, HANDLE Timer, HANDLE CompletionEvent) {
	if (DeleteTimerQueueTimer(TimerQueue, Timer, CompletionEvent) == 0) {
		if (GetLastError() != ERROR_IO_PENDING)
			DDeleteTimerQueueTimer(TimerQueue, Timer, CompletionEvent);
	}
}

