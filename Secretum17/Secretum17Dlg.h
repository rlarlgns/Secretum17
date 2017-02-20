
// Secretum17Dlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"


// CSecretum17Dlg 대화 상자
class CSecretum17Dlg : public CDialogEx
{
// 생성입니다.
public:
	CSecretum17Dlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SECRETUM17_DIALOG };
#endif

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
	afx_msg void OnBnClickedFileopen();
	afx_msg void OnBnClickedKeyFileopen();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedEncodeBtn();
	afx_msg void OnBnClickedDecodeBtn();

	// 파일 세팅 체크
	bool check_file_flag = 0, check_keyfile_flag = 0;

	int m_Radio;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	CButton keyfile_search_btn;
	CButton keyfile_create_btn;
	CEdit keyfile_name_edit;
	CStatic keyfile_message;
	CStatic keyfile_group;
	CButton file_select_btn;
	CEdit file_path_btn;

	void set_dialog(bool al_mode);
	CButton encryption_btn;
	CButton decryption_btn;
	void init_mode();
	afx_msg void OnEnChangeEdit2();
};
