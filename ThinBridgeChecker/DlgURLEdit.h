#pragma once


// CDlgURLEdit �_�C�A���O

class CDlgURLEdit : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgURLEdit)

public:
	CDlgURLEdit(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CDlgURLEdit();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DIALOG1 };
	CString m_strURL;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
