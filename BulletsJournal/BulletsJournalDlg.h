
// BulletsJournalDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CBulletsJournalDlg �Ի���
class CBulletsJournalDlg : public CDialogEx
{
// ����
public:
	CBulletsJournalDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_BULLETSJOURNAL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
	CString ipToStr(DWORD dwIP);

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedLoginButton1();
	CStatic m_picplay1;
	CIPAddressCtrl m_deviceIp1;
	afx_msg void OnBnClickedOnekeyButton();
	afx_msg void OnBnClickedConfigButton1();
};

