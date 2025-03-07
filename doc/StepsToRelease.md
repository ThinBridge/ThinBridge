# リリース手順

## ThinBridge本体のリリース手順

以下、リリースするバージョンを4.2.1.0と仮定して説明する。

 1. masterブランチを使用して[リリース前検証](verify/PreReleaseVerification.md)を一通り実施し、不具合がないことを確認する。
    ```console
    > git switch master
    ```
 2. バージョン繰り上げ用のブランチを作成する。
    ```console
    > git switch -c release-v4.2.1.0
    ```
 3. PowerShellウインドウ等を開き、[script/ThinBridgeVersionUp.ps1](../script/ThinBridgeVersionUp.ps1) を使ってインストーラおよび各モジュールのバージョンを更新する
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
 5. バージョン繰り上げ用のブランチからGitHubのプルリクエストを作成し、 @ashie、@piroor、@HashidaTKS いずれかの（自分以外の）レビューを受ける。
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


## ThinBridgeアドオンのリリース手順

以下、リリースするバージョンを2.3.1と仮定して説明する。

* 手元にこのリポジトリーをclone済みである。
* 手元に組織内配布用のCRX作成用の秘密鍵（`edge.pem`）をダウンロード済みである。
* 作業は`master`ブランチで行う。
  * Manifest V2版の更新の場合は、`addon-v1.4-maintenance`
* 作業環境はWindows PCを使用する。
* 作業環境にWSLをセットアップ済みである。
  * 作業環境には`npm`がセットアップ済みである。
    ```console
    $ sudo apt install npm
    ```
* CRXの作成および動作検証はWindowsで行う。

1. [リリース前検証手順](verify/PreReleaseVerification.md)に従って、リリース前の動作確認を行う。
   * 必須の確認対象はEdgeである。
2. GitHubリリースの作成
   1. masterブランチを元に、バージョン繰り上げ用のブランチを作成する。
      ```console
      > git switch master
      > git switch master
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
   5. バージョン繰り上げ用のブランチからGitHubのプルリクエストを作成し、 @ashie、@piroor、@HashidaTKS いずれかの（自分以外の）レビューを受ける。
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
   8. GitHubリリース上でリリースノートを作成する。
      参考: [v2.2.1リリースのリリースノート](https://github.com/ThinBridge/ThinBridge/releases/tag/addon-v2.2.1)
      * `Set as a pre-release` のチェックはオンにする。
      * `Set as the latest release` のチェックはオフにする。
      * この時点では `Safe draft` で保存するに留める。
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
3. zipパッケージの作成
   1. `webextensions` ディレクトリー内で `make` を実行する。
   2. ユニットテストが実行されたあと、以下の4つのファイルが ThinBridge/webextensions 以下に作成される。Devサフィックスの物は使用しない。
      * ThinBridgeEdge.zip
      * ThinBridgeChrome.zip
      * ThinBridgeEdgeDev.zip
      * ThinBridgeChromeDev.zip
4. 組織内配布用crxパッケージの作成
   1. 前項で作成したファイルのうち、`ThinBridgeEdge.zip` を展開する。
      ```console
      > unzip ThinBridgeEdge.zip -d ThinBridgeEdge
      ```
   2. Edgeを起動し、`edge://extensions` を開く。
   3. 「開発者モード」を有効化する（左下トグルスイッチ）。
   4. 「拡張機能のパック」をクリックする（右上）。
   5. 表示されたポップアップで以下を指定し、「拡張機能のパック」をクリックする。
      * 拡張ルートディレクトリー: zipを展開したフォルダー（`ThinBridge/webextensions/ThinBridgeEdge` など）
      * 秘密キーファイル: 事前に用意した秘密鍵（`edge.pem`）
   6. 拡張ルートディレクトリーで指定したディレクトリーの親ディレクトリーに、`ThinBridgeEdge.crx` が作成される。
   7. `ThinBridgeEdge.crx` を上記Edge画面にドラッグ＆ドロップしてインストールする。
   8. インストールされたThinBridge拡張機能のIDが `jcamehnjflombcdhafhiogbojgghefec` であることを確認する。
   9. ThinBridge拡張機能をアンインストールする。
   10. 取り扱い時に他のバージョンと混同しないよう、バージョン番号を含むファイル名に変更する。
       例：`ThinBridgeEdge-v2.3.1.crx`
   11. 作成済みのGitHubリリースを更新する。
       * `Assets`に、作成したcrxをアップロードする。
       * `Set as a pre-release` のチェックはオンにする。
       * `Set as the latest release` のチェックはオフにする。
       * `Publish release` で公開する、もしくは `Safe draft` で保存する。
5. Microsoftストアへの登録申請
   * 公開されると、ストアからインストールして利用中の実際のクライアントPCに直接デプロイされ、問題があると顧客業務への影響が非常に大きいので、間違いがないようリリースは画面を共有して2名以上で確認しながら行う。
   * 申請前に、ストアからインストールして使用している顧客宛に、リリースの了承を得る。
     （安全のため、顧客から明確な不具合報告が来ていない限り、ストア版の更新は行わない。）
   * アカウントは `edge@clear-code.com` を使用し、パスキーでログインする
   1. 申請前に、[現行版のGitHubリリース](https://github.com/ThinBridge/ThinBridge/releases)にストア版crxが添付されているかを確認する。
      * 添付されていない場合は、[ストアで公開されているcrxファイルをダウンロードして](https://edge.microsoft.com/extensionwebstorebase/v1/crx?response=redirect&x=id%3Dfamoofbkcpjdkihdngnhgbdfkfenhcnf%26installsource%3Dondemand%26uc)、現行版のgithubリリースに添付する。
   2. [Microsoftストアのダッシュボード](https://partner.microsoft.com/en-us/dashboard/microsoftedge/overview)でThinBridgeを選択する。
   3. 「Packages」を選択する。
   4. 既存パッケージの「Replace or Drag your package here (.zip)」にThinBridgeEdge.zipファイルをドラッグ＆ドロップで登録する。
   5. 「Continue」をクリックする。
   6. 「Publish」をクリックする。（オプションはデフォルトのまま）
   7. テスト方法の入力欄に以下を入力して送信する。
      ```
      This extension requires its native messaging host.
      https://github.com/ThinBridge/ThinBridge/releases/tag/v4.2.1.0
      And this extension needs to be installed to Active Directory managed environments, via GPO. Steps:
      https://github.com/ThinBridge/ThinBridge/blob/master/DEVELOPMENT.md#how-to-try-extensions-for-development
      ```
6. Chromeストアへの登録申請
   * 公開されると、ストアからインストールして利用中の実際のクライアントPCに直接デプロイされ、問題があると顧客業務への影響が非常に大きいので、間違いがないようリリースは画面を共有して2名以上で確認しながら行う。
   * 申請前に、ストアからインストールして使用している顧客宛に、リリースの了承を得る。
     （安全のため、顧客から明確な不具合報告が来ていない限り、ストア版の更新は行わない。）
   * アカウントは `google@clear-code.com` を使用し、パスキーでログインする
   1. 申請前に、[現行版のGitHubリリース](https://github.com/ThinBridge/ThinBridge/releases)にストア版crxが添付されているかを確認する。
      * 添付されていない場合は、[ストアで公開されているcrxファイルをダウンロードして](https://clients2.google.com/service/update2/crx?response=redirect&prodversion=90.0.0&acceptformat=crx2,crx3&x=id%3Diiajmhibpjkpmfmbhegccdfmfnfeffmh%26uc)、現行版のgithubリリースに添付する。
   2. Chrome Webストアのダッシュボードを開き、ThinBridgeを選択する。
   3. 「パッケージ」を選択する。
   4. 「新しいパッケージをアップロード」をクリックする。
   5. ThinBridgeChrome.zipをドラッグ＆ドロップで登録する。
7. ストアでの公開後の対応
   1. 各ストアからcrxファイルをダウンロードする。（バージョン番号は適宜差し替える）
      ```console
      > curl -L -o ThinBridgeEdge-v2.3.1-store.crx "https://edge.microsoft.com/extensionwebstorebase/v1/crx?response=redirect&prod=chromiumcrx&prodchannel=&x=id%3Dfamoofbkcpjdkihdngnhgbdfkfenhcnf%26installsource%3Dondemand%26uc"
      > curl -L -o ThinBridgeChrome-v2.3.1-store.crx "https://clients2.google.com/service/update2/crx?response=redirect&prodversion=90.0.0&acceptformat=crx2,crx3&x=id%3Diiajmhibpjkpmfmbhegccdfmfnfeffmh%26uc"
      ```
   2. 作成済みのGitHubリリースを更新する。
      * `Assets`に、ダウンロードしたcrxをアップロードする。
      * `Set as a pre-release` のチェックはオンにする。
      * `Set as the latest release` のチェックはオフにする。
      * `Publish release` で公開する、もしくは `Safe draft` で保存する。
8. ThinBridgeアドオンを更新した旨を顧客に連絡する。
   * 組織内配布用crxを提供している顧客には、githubリリースから組織内配布用crxをダウンロードして使用する旨を案内する。

