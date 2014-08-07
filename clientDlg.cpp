
// clientDlg.cpp : 구현 파일
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

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CclientDlg 대화 상자




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


// CclientDlg 메시지 처리기

BOOL CclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	/*******************************************************************
	 * 변수 초기화
	 ******************************************************************/
	key[0] = 'k';
	conn_flag = false;
	memset(&orin, 0, sizeof(stStationPC_Data));
	GetDlgItem(IDC_BUTTON_OFF)->EnableWindow(false);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
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

