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

 1. [リリース前検証](doc/verify/PreReleaseVerification.md)を一通り実施し、不具合がないことを確認する。
 2. バージョン繰り上げ用のブランチを作成する。
    ```console
    > git switch -c release-v4.2.1.0
    ```
 3. PowerShellウインドウ等を開き、script/ThinBridgeVersionUp.ps1 を使ってインストーラおよび各モジュールのバージョンを更新する
    ```console
    > cd script
    > powershell.exe -ExecutionPolicy RemoteSigned -file .\ThinBridgeVersionUp.ps1 4.2.1.0
    ```
 4. 上記スクリプトによる変更を確認し、問題がなければコミットする。
    ```console
    > git diff
    > git commit -a
    > git push --set-upstream origin release-v4.2.1.0
    ```
 5. バージョン繰り上げ用のブランチからGitHubのプルリクエストを作成し、レビューを受ける。
 6. GitHubでプルリクエストがマージされたら、ローカルリポジトリーのmasterに変更を反映する。
    ```console
    > git switch master
    > git pull
    > git branch -D release-v4.2.1.0 # マージ済みのブランチを削除
    ```
 7. 次のコマンドでタグを打ってプッシュする
    ```console
    > git tag -a v4.2.1.0 -m "ThinBridge v4.2.1.0"
    > git push origin master --tags
    ```
 8. GitHubリリース上でリリースノートを作成する。
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
 9. GitHub Actionsで生成されたEXEインストーラを添付する。
    [Build ThinBridge](https://github.com/ThinBridge/ThinBridge/actions/workflows/build-release.yaml)のArtifactsから取得した以下のファイルを使用する。
    * `ThinBridgeSetup_x64.exe` （`Installers`から取り出す）
    * `ThinBridgeSetup_x86.exe` （`Installers`から取り出す）
    * `Templates.zip` （`Templates`をそのまま使用）

ThinBridgeアドオンのリリース手順
--------------------------------

以下、リリースするバージョンを2.3.1と仮定して説明する。

 1. [リリース前検証](doc/verify/PreReleaseVerification.md)を一通り実施し、不具合がないことを確認する。
 2. バージョン繰り上げ用のブランチを作成する。
    ```console
    > git switch -c release-addon-v2.3.1
    ```
 3. リリース対象のアドオンについて、マニフェスト内のバージョン番号を繰り上げる。
    * webextensions/edge/manifest.json
    * webextensions/chrome/manifest.json
    * webextensions/firefox/manifest.json
 4. 変更を確認し、問題がなければコミットする。
    ```console
    > git diff
    > git commit -a
    > git push --set-upstream origin release-addon-v2.3.1
    ```
 5. バージョン繰り上げ用のブランチからGitHubのプルリクエストを作成し、レビューを受ける。
 6. GitHubでプルリクエストがマージされたら、ローカルリポジトリーのmasterに変更を反映する。
    ```console
    > git switch master
    > git pull
    > git branch -D release-addon-v2.3.1 # マージ済みのブランチを削除
    ```
 7. 次のコマンドでタグを打ってプッシュする
    ```console
    > git tag -a addon-v2.3.1 -m "Edge/Chrome add-on v2.3.1"
    > git push origin master --tags
    ```
 8. `webextensions` ディレクトリー内で `make` を実行し、アップロード用のファイルを作成する。
 9. 各ブラウザーのアドオンストアに、8で作成したファイルをアップロードする。
    * Edge用アドオンについては、使用手順の説明を以下の通り入力する。
      ```
      This extension requires its native messaging host.
      https://github.com/ThinBridge/ThinBridge/releases/tag/v4.2.1.0
      And this extension needs to be installed to Active Directory managed environments, via GPO. Steps:
      https://github.com/ThinBridge/ThinBridge/blob/master/DEVELOPMENT.md#how-to-try-extensions-for-development
      ```
 10. 事前に共有された秘密鍵を使い、組織内配布用のCRXを作成する。
 11. GitHubリリース上でリリースノートを作成する。
     このとき、10で作成した組織内配布用のCRXをリリースに添付する。
     * 参考: [v2.2.1リリースのリリースノート](https://github.com/ThinBridge/ThinBridge/releases/tag/addon-v2.2.1)
     * リリースノートのテンプレート：
       ```
       ## 前バージョンからの変更点
       
       * 変更点1
       * 変更点2
       * ...
       
       ## リリースパッケージについて
       
       ### 組織内配布版
       
       本リリースの`Assets`に以下パッケージを添付しています。
       
       * Microsoft Edge用: ThinBridgeEdge-v2.3.1.crx
       * Google Chrome用: 未リリース
       
       **注意事項**
       バージョン末尾に **`-store`** がついているファイルは使用しないでください。
       
       <details><summary>その他のリリースパッケージ</summary>
       
       ### ストア公開版
       
       それぞれMicrosoft EdgeストアおよびGoogle Chromeストアで公開されている（いた）バージョンです。
       ストアでは過去バージョンを取得することができないため、後日参照用に本リリースの`Assets`に添付しています。
       
       * Microsoft Edge用: ThinBridgeEdge-v2.3.1-store.crx
       * Google Chrome用: ThinBridgeChrome-v2.3.1-store.crx
       
       **注意事項**
       ストア公開版を使用する場合は、特段の理由がある場合を除いて、通常は各ブラウザの公式ストアから直接入手してください。
       本ファイルを組織内配布で使用しても、最新版がストアから自動的にダウンロードされるためバージョン固定目的で使用することはできません。バージョン固定が必要な場合は上記組織内配布版を使用してください。
       </details>
       ```
12. 当該バージョンがMicrosoft EdgeストアおよびGoogle Chromeストアで公開されたら、CRXをダウンロードしてリリースに添付する。


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
