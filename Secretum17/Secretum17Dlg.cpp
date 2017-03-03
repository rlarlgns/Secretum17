
// Secretum17Dlg.cpp : 구현 파일
//

#include "stdafx.h"

#include "des.h"
#include "md5.h"

#include "Secretum17.h"
#include "Secretum17Dlg.h"
#include "afxdialogex.h"

#include <locale>

#define BLOCK 1
#define HASH 0
#define DES_MODE 0
#define MD5_MODE 1
#define AES_MODE 2

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/* 부모 클래스 포인터 선언 */
Block_AL* block_al;
Hash_AL* hash_al;

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSecretum17Dlg 대화 상자



CSecretum17Dlg::CSecretum17Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SECRETUM17_DIALOG, pParent)
	, m_Radio(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSecretum17Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_Radio);
	DDX_Control(pDX, IDC_KEY_FILEOPEN, keyfile_search_btn);
	DDX_Control(pDX, IDC_BUTTON5, keyfile_create_btn);
	DDX_Control(pDX, IDC_EDIT2, keyfile_name_edit);
	DDX_Control(pDX, IDC_KEYFILE_MESSAGE, keyfile_message);
	DDX_Control(pDX, IDC_KEYFILE_GROUP, keyfile_group);
	DDX_Control(pDX, IDC_FILEOPEN, file_select_btn);
	DDX_Control(pDX, IDC_EDIT1, file_path_btn);
	DDX_Control(pDX, IDC_ENCODE_BTN, encryption_btn);
	DDX_Control(pDX, IDC_DECODE_BTN, decryption_btn);
}

BEGIN_MESSAGE_MAP(CSecretum17Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_FILEOPEN, &CSecretum17Dlg::OnBnClickedFileopen)
	ON_BN_CLICKED(IDC_KEY_FILEOPEN, &CSecretum17Dlg::OnBnClickedKeyFileopen)
	
	ON_BN_CLICKED(IDC_BUTTON5, &CSecretum17Dlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_ENCODE_BTN, &CSecretum17Dlg::OnBnClickedEncodeBtn)
	ON_BN_CLICKED(IDC_DECODE_BTN, &CSecretum17Dlg::OnBnClickedDecodeBtn)
	ON_BN_CLICKED(IDC_RADIO1, &CSecretum17Dlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CSecretum17Dlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CSecretum17Dlg::OnBnClickedRadio3)
END_MESSAGE_MAP()


// CSecretum17Dlg 메시지 처리기

BOOL CSecretum17Dlg::OnInitDialog()
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	/* 한글 경로 에러 체크 */
	setlocale(LC_ALL, "Korean");

	/* radio btn 1 func click */
	OnBnClickedRadio1();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CSecretum17Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CSecretum17Dlg::OnPaint()
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
HCURSOR CSecretum17Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSecretum17Dlg::OnBnClickedFileopen()
{
	// TODO: Add your control notification handler code here
	static TCHAR BASED_CODE szFilter[] = _T("모든파일(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.txt"), _T("txt"), OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal())
	{
		/* 파일 절대 경로 출력 */
		CString pathName = dlg.GetPathName();
		CEdit *p = (CEdit*)GetDlgItem(IDC_EDIT1);
		p->SetWindowTextW(pathName);

		/* 파일 이름 값 추출 */
		CString m_filename = dlg.GetFileName();
		
		/* CString -> char* 형변환 */
		file_name = (char*)malloc(m_filename.GetLength());
		size_t CharactersConverted = 0;
		wcstombs_s(&CharactersConverted, file_name, m_filename.GetLength() + 1, m_filename, _TRUNCATE);

		/* 파일 체크 */
		check_file_flag = 1;
	}
}

void CSecretum17Dlg::OnBnClickedKeyFileopen()
{
	// TODO: Add your control notification handler code here
	static TCHAR BASED_CODE szFilter[] = _T("KEY FILE(*.key)|*.key||");
	CFileDialog dlg(TRUE, _T("*.key"), _T("tmpkey"), OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal())
	{
		/* 파일 절대 경로 출력 */
		CString pathName = dlg.GetPathName();
		CEdit *p = (CEdit*)GetDlgItem(IDC_EDIT2);
		p->SetWindowTextW(pathName);

		/* 파일 이름 값 추출 */
		CString m_filename = dlg.GetFileName();

		/* CString -> char* 형변환 */
		char* keyfile_name = (char*)malloc(m_filename.GetLength());
		size_t CharactersConverted = 0;
		wcstombs_s(&CharactersConverted, keyfile_name, m_filename.GetLength() + 1, m_filename, _TRUNCATE);
		
		/* 파일 체크 */
		check_keyfile_flag = 1;
	}
}

void CSecretum17Dlg::OnBnClickedButton5()
{
	if (block_al->Make_keyfile()) {
		MessageBox(_T("Fail!! Not make keyfile"));
	}
	else {
		MessageBox(_T("Make keyfile"));
	}
	
}


void CSecretum17Dlg::OnBnClickedEncodeBtn()
{
	if (check_file_flag == 0) {
		MessageBox(_T("Please select file..."));
		return;
	}

	if (algorithm == BLOCK) {

		if (check_keyfile_flag == 0) {
			MessageBox(_T("Please select key file..."));
			return;
		}

		/* 파일을 넘김 */
		if (block_al->Get_file(file_name)) {
			MessageBox(_T("get_file error"));
		}

		/* key 파일을 넘겨줌 */
		if (block_al->Get_keyfile(keyfile_name)) {
			MessageBox(_T("get_keyfile error"));
		}

		/* 암호화 */
		block_al->Encryption();
		MessageBox(_T("Success file encryption!"));
	}
	else if (algorithm == HASH) {

		if (hash_al->Get_file(file_name)) {
			MessageBox(_T("get_file error"));
		}

		/* 암호화 */
		hash_al->Encryption();
		CString str = CString::CStringT(CA2CT(hash_al->MD5_result.c_str()));
		MessageBox(_T("MD5 File : ") + str);
	}

}


void CSecretum17Dlg::OnBnClickedDecodeBtn()
{
	if (check_file_flag == 0 || check_keyfile_flag == 0) {
		MessageBox(_T("Please select file..."));
		return;
	}

	/* 복호화 */
	if (algorithm == BLOCK) {
		block_al->Decryption();
		MessageBox(_T("Success file decryption!"));
	}
}


void CSecretum17Dlg::set_dialog(bool al) {

	/* 텍스트 area 초기화 */
	file_path_btn.SetWindowTextW(_T(""));
	keyfile_name_edit.SetWindowTextW(_T(""));

	/* mode에 따른 dialog 변환 */
	keyfile_create_btn.EnableWindow(al);
	keyfile_name_edit.EnableWindow(al);
	keyfile_search_btn.EnableWindow(al);
	keyfile_message.EnableWindow(al);
	decryption_btn.EnableWindow(al);
}


void CSecretum17Dlg::OnBnClickedRadio1()
{
	set_dialog(BLOCK);
	algorithm = BLOCK;
	m_Radio = DES_MODE;	
}


void CSecretum17Dlg::OnBnClickedRadio2()
{
	set_dialog(HASH);
	algorithm = HASH;
	m_Radio = MD5_MODE;
}


void CSecretum17Dlg::OnBnClickedRadio3()
{
	set_dialog(BLOCK);
	algorithm = BLOCK;
	m_Radio = AES_MODE;
}
