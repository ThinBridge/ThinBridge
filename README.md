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
    $ make release
    ```
    または
    ```sh
    $ git tag -a v4.0.2.4 -m "ThinBridge v4.0.2.4"
    $ git push origin master --tags
    ```
 3. GitHubリリース上でリリースノートを作成する。
    * [リリースノートのテンプレート](https://github.com/ThinBridge/ThinBridge/wiki/%E3%83%AA%E3%83%AA%E3%83%BC%E3%82%B9%E6%89%8B%E9%A0%86#%E3%83%AA%E3%83%AA%E3%83%BC%E3%82%B9%E8%AA%AC%E6%98%8E%E6%96%87%E3%81%AE%E3%83%86%E3%83%B3%E3%83%97%E3%83%AC%E3%83%BC%E3%83%88)
    * 参考: [現在の最終リリースのリリースノート](https://github.com/ThinBridge/ThinBridge/releases/latest)
 4. GitHub Actionsで生成されたEXEインストーラを添付する。
    [Build ThinBridge](https://github.com/ThinBridge/ThinBridge/actions/workflows/build-release.yaml)のArtifactsから取得した以下のファイルを使用する。
    * `ThinBridgeSetup_x64.exe` （`Installers`から取り出す）
    * `ThinBridgeSetup_x86.exe` （`Installers`から取り出す）
    * `Templates.zip` （`Templates`をそのまま使用）
