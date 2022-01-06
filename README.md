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
    * [Google Chrome](https://chrome.google.com/webstore/detail/iiajmhibpjkpmfmbhegccdfmfnfeffmh)
    * [Mozilla Firefox](https://addons.mozilla.org/ja/firefox/addon/thinbridge/)

ThinBridgeのリリース手順
------------------------

 1. 次のファイルのバージョンをインクリメントする
    - ThinBridgeSetupX64.iss
    - ThinBridgeSetupX86.iss
 2. 次のコマンドでタグを打ってプッシュする
    ```sh
    $ git tag -a v4.0.2.4 -m "ThinBridge v4.0.2.4"
    $ git push origin master --tags
    ```
 3. GitHubリリース上でリリースノートを作成する。
    * 参考: [v4.0.2.4のリリースノート](https://github.com/ThinBridge/ThinBridge/releases/tag/v4.0.2.4)
 4. GitHub Actionsで生成されたEXEインストーラを添付する。
    * `ThinBridgeSetup_x64.exe`
    * `ThinBridgeSetup_x86.exe`
    * `Template.zip`
