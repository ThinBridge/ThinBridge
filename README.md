ThinBridge
==========

ThinBridgeはエンタープライズ環境向けのブラウザ切替えソリューションです。

 * IE・Edge・Firefox・Chromeをサポート。
 * 仮想環境としてRDP・VMWare Horizon・Citrixをサポート。
 * リダイレクト対象リストをGUIの専用ツールで管理可能。
 * GPOを通じたリソース制限に対応。

インストール方法
----------------

 1. GitHubのリリースページからインストーラを取得する。
    * https://github.com/thinbridge/thinbridge/releases
    * `ThinBridgeSetup_x64.exe` (64ビットWindows向け)
    * `ThinBridgeSetup_x86.exe` (32ビットWindows向け)
 2. インストーラをWindows上で実行する。
 3. ブラウザ向けのアドオンをインストールする（任意）
    * [Microsfot Edge](https://microsoftedge.microsoft.com/addons/detail/famoofbkcpjdkihdngnhgbdfkfenhcnf)
      * ユーザー権限インストールでの使用には非対応。
      * Active Directoryドメイン参加状態の端末に対し、GPOの `コンピューターの構成` → `管理用テンプレート` → `Microsoft Edge` → `拡張機能` → `サイレント インストールされる拡張機能を制御する` で強制インストールする必要あり。
    * [Google Chrome](https://chrome.google.com/webstore/detail/iiajmhibpjkpmfmbhegccdfmfnfeffmh)
      * ユーザー権限インストールでの使用には非対応。
      * Active Directoryドメイン参加状態の端末に対し、GPOの `コンピューターの構成` → `管理用テンプレート` → `Google` → `Google Chrome` → `拡張機能` → `自動インストールするアプリと拡張機能のリストの設定` で強制インストールする必要あり。
    * [Mozilla Firefox](https://addons.mozilla.org/ja/firefox/addon/thinbridge/)
      * ユーザー権限インストール、GPOでのインストールのどちらでも使用可能。

ThinBridgeのリリース手順
------------------------

[リリース手順](doc/StepsToRelease.md)を参照。
