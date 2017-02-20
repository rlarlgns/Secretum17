#pragma once


// MD5_dlg 대화 상자입니다.

class MD5_dlg : public CDialogEx
{
	DECLARE_DYNAMIC(MD5_dlg)

public:
	MD5_dlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~MD5_dlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MD5 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
