#include "stdafx.h"
#include "CameraStream.h"
#include <iostream>

// https://blog.csdn.net/xiaoyao_SDU/article/details/80412803


//���캯��
CameraStream::CameraStream(void)
{
	
}
//��������
CameraStream::~CameraStream(void)
{
}
//��ʼ��������������ʼ��״̬���
bool CameraStream::Init()
{
	if (NET_DVR_Init())
	{
		this->isInit = true;
		return true;
	}
	else
	{
		this->isInit = false;
		return false;
	}
}

//��¼��������������ͷid�Լ����������¼
bool CameraStream::login(char* sDeviceAddress)
//bool HK_camera::Login(const char* sDeviceAddress,const char* sUserName,const char* sPassword, WORD wPort);        //��½��VS2017�汾��
{
	// ��ʼ��sdk
	if (!this->isInit){
		if (this->Init()){
			cout << "------------ ��ʼ���ɹ�" << endl;
		} else {
			cout << "------------ ��ʼ��ʧ��" << endl;
			return false;
		}
	}

	if (lUserID < 0)
	{
		NET_DVR_DEVICEINFO_V30 struDeviceInfo;
		lUserID = NET_DVR_Login_V30(sDeviceAddress, 8000, "admin", "qazwsx12!@", &struDeviceInfo);
		if (lUserID >= 0){
			return true;
		} else{
			return false;
		}
	} else {
		return true;
	}
}

//��Ƶ����ʾ����
bool CameraStream::start(void(CALLBACK *g_RealDataCallBack_V30)(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser), void(CALLBACK *decCBFun)(long nPort, char* pBuf, long nSize, FRAME_INFO* pFrameInfo, long nUser, long nReserved2), LONG &nPort, HWND hPlayWnd, char* sDeviceAddress)
{
	// ��¼
	if (!login(sDeviceAddress)){
		return false;
	}

	
	//����Ԥ�������ûص�������
	NET_DVR_PREVIEWINFO struPlayInfo = { 0 };
	struPlayInfo.hPlayWnd = hPlayWnd; //����Ϊ�գ��豸SDK������ֻȡ��
	struPlayInfo.lChannel = 1; //Channel number �豸ͨ��
	struPlayInfo.dwStreamType = 0;// �������ͣ�0-��������1-��������2-����3��3-����4, 4-����5,5-����6,7-����7,8-����8,9-����9,10-����10
	struPlayInfo.dwLinkMode = 0;// 0��TCP��ʽ,1��UDP��ʽ,2���ಥ��ʽ,3 - RTP��ʽ��4-RTP/RTSP,5-RSTP/HTTP 
	struPlayInfo.bBlocked = 1; //0-������ȡ��, 1-����ȡ��, �������SDK�ڲ�connectʧ�ܽ�����5s�ĳ�ʱ���ܹ�����,���ʺ�����ѯȡ������.

	realPlayHandler = NET_DVR_RealPlay_V40(lUserID, &struPlayInfo, g_RealDataCallBack_V30, NULL);
	return true;
}
bool CameraStream::stop(LONG &nPort){
	// TODU
	
	//ֹͣԤ��
	if ( !NET_DVR_StopRealPlay(realPlayHandler) )
	{
		NET_DVR_GetLastError();
		return false;
	}
	realPlayHandler = -1;
	//ֹͣ����
	//if (Port > -1)
	//{
	//	/*if (!PlayM4_StopSound())
	//	{
	//		PlayM4_GetLastError(Port);
	//	}*/
	//	if (!PlayM4_Stop(Port))
	//	{
	//		PlayM4_GetLastError(Port);
	//	}
	//	if (!PlayM4_CloseStream(Port))
	//	{
	//		PlayM4_GetLastError(Port);
	//	}
	//	PlayM4_FreePort(Port);
	//	Port = -1;
	//	nPort = -1;
	//}
	//if ( !NET_DVR_Logout(lUserID) ){
	//	NET_DVR_GetLastError();
	//	return false;
	//}
	//lUserID = -1;

	//if ( !NET_DVR_Cleanup() ){
	//	NET_DVR_GetLastError();
	//	return false;
	//}
	//nPort = -1;
	return true;
}