// DlgMsg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "ThinBridgeChecker.h"
#include "DlgMsg.h"
#include "afxdialogex.h"


// DlgMsg �_�C�A���O

IMPLEMENT_DYNAMIC(DlgMsg, CDialogEx)

DlgMsg::DlgMsg(CWnd* pParent /*=NULL*/)
	: CDialogEx(DlgMsg::IDD, pParent)
{
m_iCnt=0;
}

DlgMsg::~DlgMsg()
{
}

void DlgMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ST_STEP, m_Step);
	DDX_Control(pDX, IDC_ST, m_Func);
}


BEGIN_MESSAGE_MAP(DlgMsg, CDialogEx)
END_MESSAGE_MAP()


// DlgMsg ���b�Z�[�W �n���h���[
