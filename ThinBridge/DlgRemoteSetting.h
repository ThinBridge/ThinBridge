#pragma once


class CDlgRemoteSetting : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgRemoteSetting)

public:
	CDlgRemoteSetting();
	virtual ~CDlgRemoteSetting();

	enum { IDD = IDD_DLG_REMOTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
class CDlgRemoteSettingRD : public CDlgRemoteSetting
{
	DECLARE_DYNCREATE(CDlgRemoteSettingRD)

public:
	CDlgRemoteSettingRD(){};
	virtual ~CDlgRemoteSettingRD(){};
	virtual BOOL OnInitDialog();
};
class CDlgRemoteSettingGEN : public CDlgRemoteSetting
{
	DECLARE_DYNCREATE(CDlgRemoteSettingGEN)

public:
	CDlgRemoteSettingGEN(){};
	virtual ~CDlgRemoteSettingGEN(){};
	virtual BOOL OnInitDialog();
};
