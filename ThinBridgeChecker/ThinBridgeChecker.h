
// CreateProcessChk.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CThinBridgeCheckerApp:
// このクラスの実装については、ThinBridgeChecker.cpp を参照してください。
//

class CThinBridgeCheckerApp : public CWinApp
{
public:
	CThinBridgeCheckerApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装
	DECLARE_MESSAGE_MAP()
};

extern CThinBridgeCheckerApp theApp;