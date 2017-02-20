// MD5_dlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Secretum17.h"
#include "MD5_dlg.h"
#include "afxdialogex.h"


// MD5_dlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(MD5_dlg, CDialogEx)

MD5_dlg::MD5_dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MD5, pParent)
{

}

MD5_dlg::~MD5_dlg()
{
}

void MD5_dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MD5_dlg, CDialogEx)
END_MESSAGE_MAP()


// MD5_dlg 메시지 처리기입니다.
