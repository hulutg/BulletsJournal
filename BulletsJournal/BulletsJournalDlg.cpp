
// BulletsJournalDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BulletsJournal.h"
#include "BulletsJournalDlg.h"
#include "afxdialogex.h"
#include "CameraStream.h"
#include <atomic>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct ChannelInfo{
	// ���ھ�������ã�����imshow��
	HWND hPlayWnd;
	// α����
	list<Mat> matQueque; // ȫ��
	// �߳���
	std::mutex mutexLock;
	// ������ſ�ͨ����
	LONG nPort = -1;
	// ���IP
	char deviceIp[16];
	// ����ɹ����ӱ�־λ
	bool isReady = false;
	// ���SDK�쳣����
	char des[20]; 
	// �Ƿ����ڽ���Ԥ��
	bool isRealPlaying = false;
	// �Ƿ��Ѿ���¼
	bool isLogin = false;
	// �Ƿ�ɽ���ͼ����
	bool isPlayingCV = true;

	int samplingInterval = 0;

} ChannelInfo1, ChannelInfo2, ChannelInfo3��ChannelInfo4��ChannelInfo5;

CameraStream camera1;

std::atomic<bool> forcedReturn(false);

//Mat g_BGRImage;
//int g_count = 0;
//clock_t g_start=0, g_ends=0;


//���ݽ���ص�������
//���ܣ���YV_12��ʽ����Ƶ������ת��Ϊ�ɹ�opencv�����BGR���͵�ͼƬ���ݣ���ʵʱ��ʾ��
void CALLBACK decCBFun(long nPort, char* pBuf, long nSize, FRAME_INFO* pFrameInfo, long nUser, long nReserved2)
{
	// 10 ��
	if (ChannelInfo1.samplingInterval > 25){
		ChannelInfo1.samplingInterval = 0;

	} else if (ChannelInfo1.samplingInterval % 5 == 0){  
		ChannelInfo1.samplingInterval++;
		if (pFrameInfo->nType == T_YV12)
		{
			/*if (g_count == 0){
			g_start = clock();
			cout << "g_start = clock()" << endl;
			}*/
			//std::cout << "the frame infomation is T_YV12" << std::endl;
			/*if (g_BGRImage.empty())
			{
			g_BGRImage.create(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC3);
			}*/

			Mat YUVImage(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC1, (unsigned char*)pBuf);

			////cvtColor(YUVImage, g_BGRImage, COLOR_YUV2BGR_YV12);
			Rect rect(1000, 200, 800, 600);
			Mat ROI = YUVImage(rect);

			ChannelInfo1.mutexLock.lock();
			ChannelInfo1.matQueque.push_back(ROI.clone());
			ChannelInfo1.mutexLock.unlock();
			//srand((unsigned)time(NULL));//Ϊrand()�������ɲ�ͬ���������
			//int RandomNumber = rand() % 10;//����100���ڵ������
			//Sleep(50 + RandomNumber);
			///*Mat dst;
			//cvtColor(ROI, dst, CV_BGR2GRAY);
			//namedWindow("FHUT", WINDOW_AUTOSIZE);
			//imshow("FHUT", ROI);
			//waitKey(15);
			TRACE("\n ----------------------------------------------��ӣ�%d \n", ChannelInfo1.matQueque.size());
			//char image_name[25];
			//sprintf(image_name, "%s%d%s", "", start, ".jpg");//�����ͼƬ�� 
			//imwrite(image_name, ROI); //����һ֡ͼƬ 
			//Sleep(1 * 1000);
			//delete[] image_name;

			YUVImage.~Mat();
			ROI.~Mat();
		}

		/*if (g_count > 24){
		g_count = 1;
		g_start = clock();

		}else if (g_count==24){
		g_ends = clock();
		cout << "---------------- every 25 frames time escape: " << (double)(g_ends - g_start) / CLOCKS_PER_SEC << endl;

		}
		g_count++;*/
	} else{
		ChannelInfo1.samplingInterval++;
	}
};


//ʵʱ��Ƶ�������ݻ�ȡ �ص�����
void CALLBACK realDataCallBack_V30(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser)
{
	if (dwDataType == NET_DVR_STREAMDATA)//��������
	{
		if (dwBufSize > 0 && ChannelInfo1.nPort != -1)
		{
			if (!PlayM4_InputData(ChannelInfo1.nPort, pBuffer, dwBufSize))
			{
				//std::cout << "fail input data" << std::endl;
			}
			else
			{
				//std::cout << "success input data" << std::endl;
			}

		}
	}
};


void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* dwUser)
{
	switch (dwDataType)
	{
	case NET_DVR_SYSHEAD: //ϵͳͷ
		if (!PlayM4_GetPort(&ChannelInfo1.nPort)) //��ȡ���ſ�δʹ�õ�ͨ����
		{
			break;
		}
		//m_iPort = lPort; 
		//��һ�λص�����ϵͳͷ������ȡ�Ĳ��ſ� port �Ÿ�ֵ��ȫ�� port���´λص�����ʱ��ʹ�ô� port �Ų���
		if (dwBufSize > 0)
		{
			if (!PlayM4_SetStreamOpenMode(ChannelInfo1.nPort, STREAME_REALTIME)) //����ʵʱ������ģʽ
			{
				break;
			}
			if (!PlayM4_OpenStream(ChannelInfo1.nPort, pBuffer, dwBufSize, 1024 * 1024)) //�����ӿ�
			{
				break;
			}
			if (!PlayM4_SetDecCallBackExMend(ChannelInfo1.nPort, decCBFun, NULL, 0, NULL)){
				break;
			}
			if (!PlayM4_Play(ChannelInfo1.nPort, NULL)) //���ſ�ʼ
			{
				break;
			}
		} 
		break; 
	case NET_DVR_STREAMDATA: //��������
		if (dwBufSize > 0 && ChannelInfo1.nPort != -1){
			if (!PlayM4_InputData(ChannelInfo1.nPort, pBuffer, dwBufSize)){
				break;
			} 
		} 
		break; 
	default: //��������
		if (dwBufSize > 0 && ChannelInfo1.nPort != -1){
			if (!PlayM4_InputData(ChannelInfo1.nPort, pBuffer, dwBufSize)){
				break;
			} 
		} 
		break; 
	}
}

void popList() {
	while (ChannelInfo1.isPlayingCV){
		ChannelInfo1.mutexLock.lock();
		if (ChannelInfo1.matQueque.size() > 0){

			TRACE("\n---------------------------------------------- Ԫ���ײ����� BY OpenCV: %d \n", ChannelInfo1.matQueque.size());
			// TODO ͼ�������������
			Mat pop = ChannelInfo1.matQueque.front();
			/*imshow("IPCamera1", pop);
			waitKey(1);*/
			ChannelInfo1.matQueque.pop_front();
				
			pop.~Mat();
		}

		ChannelInfo1.mutexLock.unlock();
		srand((unsigned)time(NULL));//Ϊrand()�������ɲ�ͬ���������
		int RandomNumber = rand() % 10;//����100���ڵ������
		Sleep(50 + RandomNumber);
	}
	return;
}

void pushList(char *deviceIp, CameraStream camera) {
	bool status = camera.start(g_RealDataCallBack_V30, decCBFun, std::ref(ChannelInfo1.nPort), ChannelInfo1.hPlayWnd, deviceIp);
	
	if (!status){
		AfxMessageBox("����ʧ�ܣ�����ϵ����Ա��");
		return;
	}
	ChannelInfo1.isReady = status;
	while (true){
		if (forcedReturn){
			return;
		}
		Sleep(3000);
	}
	TRACE(" ---------------------------------------------- Login successful \n");
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CBulletsJournalDlg �Ի���



CBulletsJournalDlg::CBulletsJournalDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBulletsJournalDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBulletsJournalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_deviceIp1);
}

BEGIN_MESSAGE_MAP(CBulletsJournalDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOGIN_BUTTON1, &CBulletsJournalDlg::OnBnClickedLoginButton1)
	ON_BN_CLICKED(IDC_ONEKEY_BUTTON, &CBulletsJournalDlg::OnBnClickedOnekeyButton)
	ON_BN_CLICKED(IDC_CONFIG_BUTTON1, &CBulletsJournalDlg::OnBnClickedConfigButton1)
END_MESSAGE_MAP()


// CBulletsJournalDlg ��Ϣ�������

BOOL CBulletsJournalDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ShowWindow(SW_MINIMIZE);
	//hPlayWnd = GetDlgItem(IDC_STATIC_SCREEN1)->m_hWnd; 

	// ��ʼ��SDK
	NET_DVR_Init();

	// ����Ĭ��IP
	m_deviceIp1.SetAddress(192, 168, 1, 64);

	// ��opencv imshow�󶨵�MFC pictrue control�ؼ�
	namedWindow("IPCamera", 0);
	CRect screen1;
	CWnd *pWnd = GetDlgItem(IDC_STATIC_SCREEN1);//IDC_PICTUREΪ�ؼ�ID��
	pWnd->GetClientRect(&screen1);
	resizeWindow("IPCamera", screen1.Width(), screen1.Height());
	HWND hWnd_CAM1 = (HWND)cvGetWindowHandle("IPCamera");
	HWND hParent_CAM1 = ::GetParent(hWnd_CAM1);
	::SetParent(hWnd_CAM1, GetDlgItem(IDC_STATIC_SCREEN1)->m_hWnd);
	::ShowWindow(hParent_CAM1, SW_HIDE); //�������г����
	//GetDlgItem(IDC_STATIC_SCREEN1)->ShowWindow(0);//����ʱ����ʾ���ſؼ�

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CBulletsJournalDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CBulletsJournalDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CBulletsJournalDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CBulletsJournalDlg::OnBnClickedLoginButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	CString capital;
	GetDlgItem(IDC_LOGIN_BUTTON1)->GetWindowText(capital);
	if (capital == TEXT("ֹͣ")){
		bool status = camera1.stop(std::ref(ChannelInfo1.nPort));
		if (!status){
			AfxMessageBox("����ʧ�ܣ�����ϵ����Ա��");
			return;
		}
		ChannelInfo1.isReady = false;
		ChannelInfo1.isRealPlaying = false;
		Sleep(1500);
		GetDlgItem(IDC_LOGIN_BUTTON1)->SetWindowText("Ԥ��");
		GetDlgItem(IDC_ONEKEY_BUTTON)->EnableWindow(TRUE);
		return;
	}

	GetDlgItem(IDC_ONEKEY_BUTTON)->EnableWindow(FALSE);

	// ÿ������Ԥ��������һ��
	if (ChannelInfo1.matQueque.size() > 0){
		ChannelInfo1.matQueque.clear();
	}

	// �״�Ԥ�������� UpdateData ���ip
	if (!ChannelInfo1.isLogin){
		UpdateData(TRUE);
		DWORD dwDeviceIP;
		CString csTemp;
		m_deviceIp1.GetAddress(dwDeviceIP);
		csTemp = ipToStr(dwDeviceIP);
		sprintf_s(ChannelInfo1.deviceIp, 16, "%s", csTemp.GetBuffer(0));
		UpdateData(false);
	}

	bool status = camera1.start(g_RealDataCallBack_V30, decCBFun, std::ref(ChannelInfo1.nPort), ChannelInfo1.hPlayWnd, ChannelInfo1.deviceIp);
	if (!status){
		AfxMessageBox("����ʧ�ܣ�����ϵ����Ա��");
		return;
	}
	ChannelInfo1.isReady = true;
	ChannelInfo1.isLogin = true;
	ChannelInfo1.isRealPlaying = true;

	GetDlgItem(IDC_LOGIN_BUTTON1)->SetWindowText("ֹͣ");
	

	while (ChannelInfo1.isReady){
		ChannelInfo1.mutexLock.lock();
		if (ChannelInfo1.matQueque.size() > 0){

			TRACE("############################################################ Ԫ���ײ���ջ: %d \n", ChannelInfo1.matQueque.size());

			Mat pop = ChannelInfo1.matQueque.front();
			ChannelInfo1.matQueque.pop_front();

			imshow("IPCamera", pop);
			GetDlgItem(IDC_STATIC_SCREEN1)->ShowWindow(1); //��ʾ���ſؼ�
			pop.~Mat();
			waitKey(1);
		}

		ChannelInfo1.mutexLock.unlock();
		srand((unsigned)time(NULL));//Ϊrand()�������ɲ�ͬ���������
		int RandomNumber = rand() % 10;//����100���ڵ������
		Sleep(50 + RandomNumber);
	}
	return;
}

void CBulletsJournalDlg::OnBnClickedOnekeyButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString capital;
	GetDlgItem(IDC_ONEKEY_BUTTON)->GetWindowText(capital);
	if (capital == TEXT("��������")){
		ChannelInfo1.isPlayingCV = false;
		bool status = camera1.stop(std::ref(ChannelInfo1.nPort));
		Sleep(1500);
		GetDlgItem(IDC_ONEKEY_BUTTON)->SetWindowText("��ʼ����");
		GetDlgItem(IDC_LOGIN_BUTTON1)->EnableWindow(TRUE);
		return;
	}
	if (ChannelInfo1.isRealPlaying){
		AfxMessageBox("��ֹͣ����Ԥ����");
		return;
	} else {
		// �״�Ԥ�������� UpdateData ���ip
		GetDlgItem(IDC_LOGIN_BUTTON1)->EnableWindow(FALSE);

		if (!ChannelInfo1.isLogin){
			UpdateData(TRUE);
			DWORD dwDeviceIP;
			CString csTemp;
			m_deviceIp1.GetAddress(dwDeviceIP);
			csTemp = ipToStr(dwDeviceIP);
			sprintf_s(ChannelInfo1.deviceIp, 16, "%s", csTemp.GetBuffer(0));
			UpdateData(false);
		}
		ChannelInfo1.isPlayingCV = true;
		bool status = camera1.start(g_RealDataCallBack_V30, decCBFun, std::ref(ChannelInfo1.nPort), ChannelInfo1.hPlayWnd, ChannelInfo1.deviceIp);
		Sleep(500);
		std::thread popListThread(popList);
		ChannelInfo1.isRealPlaying = false;
		GetDlgItem(IDC_ONEKEY_BUTTON)->SetWindowText("��������");
		GetDlgItem(IDC_LOGIN_BUTTON1)->EnableWindow(FALSE);
		popListThread.detach();
	}
}


CString CBulletsJournalDlg::ipToStr(DWORD dwIP)
{
	CString strIP = _T("");
	WORD add1, add2, add3, add4;

	add1 = (WORD)(dwIP & 255);
	add2 = (WORD)((dwIP >> 8) & 255);
	add3 = (WORD)((dwIP >> 16) & 255);
	add4 = (WORD)((dwIP >> 24) & 255);
	strIP.Format("%d.%d.%d.%d", add4, add3, add2, add1);
	return strIP;
}


void CBulletsJournalDlg::OnBnClickedConfigButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	ShellExecute(NULL, _T("open"), _T("iexplore.exe"), _T("http://192.168.1.64/"), NULL, SW_SHOW);
}
