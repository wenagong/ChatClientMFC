// CLoginDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "CLoginDlg.h"
#include "afxdialogex.h"
#include"CClientSocket.h"

// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN_DLG, pParent)
	, m_strUsername(_T("awen")) //默认初始化
	, m_uPort(6080)
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strUsername);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPAddressCtrl);
	DDX_Text(pDX, IDC_EDIT2, m_uPort);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序

//登录模块
void CLoginDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	if (m_strUsername.IsEmpty()) {
		MessageBox(_T("用户名不能为空."));
		return;
	}
	//获取IP地址
	CString strIPAddress;
	m_IPAddressCtrl.GetWindowText(strIPAddress);

	if (strIPAddress.IsEmpty() || strIPAddress == _T("0.0.0.0")) {
		MessageBox(_T("请输入IP地址."));
		return;
	}

	if (m_uPort == 0) {
		MessageBox(_T("请输入端口号."));
		return;
	}

	//分配内存
	theApp.m_pClientSocket = new CClientSocket;
	//创建套接字
	if (FALSE == theApp.m_pClientSocket->Create()) {
		CString str;
		str.Format(_T("创建套接字失败.错误代码：%d"), GetLastError());
		MessageBox(str);
		delete theApp.m_pClientSocket;
		theApp.m_pClientSocket = NULL;
		return;
	}

	//连接服务器
	if (theApp.m_pClientSocket->Connect(strIPAddress, m_uPort) == FALSE) {
		CString str;
		str.Format(_T("连接服务器失败.错误代码：%d"), GetLastError());
		MessageBox(str);
		delete theApp.m_pClientSocket;
		theApp.m_pClientSocket = NULL;
		return;
	}

	//登录
	Command cmd;
	cmd.cmd = LOGIN;
	cmd.nDataSize = m_strUsername.GetLength() * 2;
	
	//发送命令
	theApp.m_pClientSocket->Send(&cmd, sizeof(cmd));

	//发送用户名
	theApp.m_pClientSocket->Send(m_strUsername, m_strUsername.GetLength() * 2);

	CDialogEx::OnOK(); //关闭对话框
}


BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//设置默认IP地址
	m_IPAddressCtrl.SetWindowTextW(_T("192.168.1.101"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
