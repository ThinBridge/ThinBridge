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

以下、リリースするバージョンを4.2.1.0と仮定して説明する。

 1. PowerShellウインドウ等を開き、script/ThinBridgeVersionUp.ps1 を使ってインストーラおよび各モジュールのバージョンを更新する
    ```console
    > cd script
    > powershell.exe -ExecutionPolicy RemoteSigned -file .\ThinBridgeVersionUp.ps1 4.2.1.0
    ```
 2. 上記スクリプトによる変更を確認し、問題がなければコミットする。
    ```console
    > git diff
    > git commit -a
    ```
 3. 次のコマンドでタグを打ってプッシュする
    ```console
    > git tag -a v4.2.1.0 -m "ThinBridge v4.2.1.0"
    > git push origin master --tags
    ```
 4. GitHubリリース上でリリースノートを作成する。
     * 参考: [現在の最終リリースのリリースノート](https://github.com/ThinBridge/ThinBridge/releases/latest)
     * リリースノートのテンプレート：
       ```
       ## 前バージョンからの変更点
       
       * 変更点1
       * 変更点2
       * ...
       
       ## リリースについて
       
       ThinBridgeでは2種類のインストーラとGPOテンプレートを提供しています。
       
       * **ThinBridgeSetup_x64.exe**
         *  64ビット版のインストーラです。
         * 一般的な環境ではこちらのインストーラを利用ください。
       * **ThinBridgeSetup_x86.exe**
         * 32ビット版Windows環境を利用している場合はこちらを利用します。
       * **Templates.zip**
         * ThinBridge用のADMXテンプレートです。
       ```
 5. GitHub Actionsで生成されたEXEインストーラを添付する。
    [Build ThinBridge](https://github.com/ThinBridge/ThinBridge/actions/workflows/build-release.yaml)のArtifactsから取得した以下のファイルを使用する。
    * `ThinBridgeSetup_x64.exe` （`Installers`から取り出す）
    * `ThinBridgeSetup_x86.exe` （`Installers`から取り出す）
    * `Templates.zip` （`Templates`をそのまま使用）

ThinBridgeアドオンのリリース手順
--------------------------------

 1. リリース対象のアドオンについて、マニフェスト内のバージョン番号を繰り上げる。
    * webextensions/edge/manifest.json
    * webextensions/chrome/manifest.json
    * webextensions/firefox/manifest.json
 2. `webextensions` ディレクトリー内で `make` を実行し、アップロード用のファイルを作成する。
 3. 各ブラウザーのアドオンストアに、2で作成したファイルをアップロードする。
    * Edge用アドオンについては、使用手順の説明を以下の通り入力する。
      ```
      This extension requires its native messaging host.
      https://github.com/ThinBridge/ThinBridge/releases/tag/v4.0.2.4
      And this extension needs to be installed to Active Directory managed environments, via GPO. Steps:
      https://github.com/ThinBridge/ThinBridge/blob/master/DEVELOPMENT.md#how-to-try-extensions-for-development
      ```


CRXファイルを取得する
---------------------

### Chrome

```bash
$ curl -L -o ThinBridgeChrome.crx "https://clients2.google.com/service/update2/crx?response=redirect&prodversion=90.0.0&acceptformat=crx2,crx3&x=id%3Diiajmhibpjkpmfmbhegccdfmfnfeffmh%26uc"
```

### Edge

```bash
$ curl -L -o ThinBridgeEdge.crx "https://edge.microsoft.com/extensionwebstorebase/v1/crx?response=redirect&prod=chromiumcrx&prodchannel=&x=id%3Dfamoofbkcpjdkihdngnhgbdfkfenhcnf%26installsource%3Dondemand%26uc"
```
