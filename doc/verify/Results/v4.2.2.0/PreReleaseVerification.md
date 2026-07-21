# ThinBridge リリース前検証手順

## 検証環境の用意

検証には原則として `/doc/verify` 配下のいずれかの環境（推奨：`/doc/verify/windows-11-22H2`）を使用する。
`/doc/verify` 配下の物ではない他のWindows環境を使用する場合は、以下の条件を事前に整えておく。

* Active DirectoryドメインあるいはMDM（Mobile Device Management）サービスに参加状態である。
  （または、[/doc/verify/join-to-fake-mdm.reg](join-to-fake-mdm.reg) の内容を管理者権限でインポート済みである。）
* Google Chrome、Microsoft Edgeをインストール済みである。
* Google Chrome、Microsoft EdgeのGPO用ポリシーテンプレートを導入済みである。
* `C:\Users\Public\webextensions` 配下に `/webextensions/` の内容を配置済みである。
  * `C:\Users\Public\webextensions\manifest.xml` の位置に [/doc/verify/manifest.xml](manifest.xml) と同等のファイルを配置済みである。
* IEのセキュリティ設定で「インターネットゾーン」の保護モードを無効化してある。
  （[/doc/verify/disable-protect-mode.reg](disable-protect-mode.reg) の内容をインポート済みである。）

Windows Enterprise マルチセッションのテストをする場合は、以下の条件を事前に整えておく。

* Windows Enterprise マルチセッションの構築方法は以下。
  * https://learn.microsoft.com/ja-jp/azure/virtual-desktop/windows-multisession-faq
    * 以下の手順に従いながら、[ホスト プールの種類]にプール方式を指定してマルチセッション環境を作成する。
    * https://learn.microsoft.com/ja-jp/azure/virtual-desktop/tutorial-try-deploy-windows-11-desktop?tabs=windows-client
    * https://learn.microsoft.com/ja-jp/azure/virtual-desktop/add-session-hosts-host-pool?tabs=portal%2Cgui
* 予め2ユーザーを作成しておく
  * ユーザーA（セッションA）: テスト手順を実施するユーザー（セッション）。
  * ユーザーB（セッションB）: テスト中に常にログオンしておくユーザー（セッション）。別ユーザーでリダイレクトが発生しないかの確認用。

準備は以下の手順で行う。

1. https://github.com/ThinBridge/ThinBridge/releases/latest もしくは
   https://github.com/ThinBridge/ThinBridge/actions/workflows/build-release.yaml （各ビルドの `Artifacts` の `Installer`）よりThinBridgeの最新のインストーラ `ThinBridgeSetup_x64.exe` をダウンロードし、実行、インストールする。
2. Chrome用アドオンの開発版パッケージを用意し、インストールするための設定を行う。
   1. Chromeを起動する。
   2. アドオンの管理画面（`chrome://extensions`）を開く。
   3. `デベロッパーモード` を有効化する。
   4. `拡張機能をパッケージ化` で `C:\Users\Public\webextensions\chrome` をパックする。（1つ上のディレクトリーに `chrome.crx` と `chrome.pem` が作られる）
   5. `chrome.crx` をChromeのアドオン管理画面にドラッグ＆ドロップし、インストールして、IDを控える。
      例：`egoppdngdcoikeknmbgiakconmchahhf`
   6. アドオンを一旦アンインストールする。
   7. `C:\Program Files\ThinBridge\ThinBridgeHost\chrome.json` のアクセス権を変更し、ユーザー権限での書き込みを許可した上で、`"allowed_origins"` に先ほど控えたIDに基づくURLを追加する。
      例：`"chrome-extension://egoppdngdcoikeknmbgiakconmchahhf/"`
   8. `C:\Users\Public\webextensions\manifest.xml` のChrome用アドオンのIDを、先程控えたIDで置き換える。
   9. `gpedit.msc` を起動する。
   10. `Computer Configuration\Administrative Templates\Google\Google Chrome\Extensions` （`コンピューターの構成\管理用テンプレート\Google\Google Chrome\拡張機能`）を開いて、以下のポリシーを設定する。
       * `Configure the list of force-installed apps and extensions`（`自動インストールするアプリと拡張機能のリストの設定`）
         * `Enabled`（`有効`）に設定して、`Show...`（`表示...`）をクリックし、以下の項目を追加する。
           * `<先程控えたID>;file:///C:/Users/Public/webextensions/manifest.xml`
             例：`egoppdngdcoikeknmbgiakconmchahhf;file:///C:/Users/Public/webextensions/manifest.xml`
   11. Chromeを再起動し、アドオンの管理画面（`chrome://extensions`）を開いて、ThinBridgeの開発版が管理者によってインストールされた状態になっていることを確認する。
3. Edgeアドオンの開発版パッケージを用意し、インストールするための設定を行う。
   1. Edgeを起動する。
   2. アドオンの管理画面（`edge://extensions`）を開く。
   3. `開発者モード` を有効化する。
   4. `拡張機能のパック` で `C:\Users\Public\webextensions\edge` をパックする。（1つ上のディレクトリーに `edge.crx` と `edge.pem` が作られる）
   5. `edge.crx` をEdgeのアドオン管理画面にドラッグ＆ドロップし、インストールして、IDを控える。
      例：`oapdkmbdgdcjpacbjpcdfhncifimimcj`
   6. アドオンを一旦アンインストールする。
   7. `C:\Program Files\ThinBridge\ThinBridgeHost\edge.json` のアクセス権を変更し、ユーザー権限での書き込みを許可した上で、`"allowed_origins"` に、先ほど控えたIDに基づくURLを追加する。
      例：`"chrome-extension://oapdkmbdgdcjpacbjpcdfhncifimimcj/"`
   8. `C:\Users\Public\webextensions\manifest.xml` のEdge用アドオンのIDを、先程控えたIDで置き換える。
   9. `gpedit.msc` を起動する。
   10. `Computer Configuration\Administrative Templates\Microsoft Edge\Extensions`（`コンピューターの構成\管理用テンプレート\Microsoft Edge\拡張機能`）を開いて、以下のポリシーを設定する。
       * `Control which extensions are installed silently`（`サイレント インストールされる拡張機能を制御する`）
         * `Enabled`（`有効`）に設定して、`Show...`（`表示...`）をクリックし、以下の項目を追加する。
           * `<先程控えたID>;file:///C:/Users/Public/webextensions/manifest.xml`
             例：`oapdkmbdgdcjpacbjpcdfhncifimimcj;file:///C:/Users/Public/webextensions/manifest.xml`
   11. Edgeを再起動し、アドオンの管理画面（`edge://extensions`）を開いて、ThinBridgeの開発版が管理者によってインストールされた状態になっていることを確認する。
4. BHOの管理のためのポリシー設定を行う。
   1. `gpedit.msc` を起動する。
   2. `Computer Configuration\Administrative Templates\Windows Components\Internet Explorer` （`コンピューターの構成\管理用テンプレート\Windows コンポーネント\Internet Explorer`）を開いて、以下のポリシーを設定する。
      * `Turn off add-on performance notifications`（`アドオンのパフォーマンスの通知を無効にする`）：`Enabled`（`有効`）
      * `Automatically activate newly installed add-ons`（`新たにインストールされたアドオンを自動的にアクティブ化する`）：`Enabled`（`有効`）
   3. `Computer Configuration\Administrative Templates\Windows Components\Internet Explorer\Internet Control Panel\Advance Page` （`コンピューターの構成\管理用テンプレート\Windows コンポーネント\Internet Explorer\インターネット コントロール パネル\[詳細設定]ページ`）を開いて、以下のポリシーを設定する。
      * `Allow third-party browser extensions`（`サード パーティ製のブラウザー拡張を許可する`）：`Enabled`（`有効`）
5. EdgeのIEモードタブを有効化する。
   1. Edgeを起動する。
   2. `edge://settings/defaultBrowser` を開く。
   3. `Allow sites to be reloaded in Internet Explorer mode (IE mode)`（`Internet Explorer モード (IE モード) でサイトの再読み込みを許可`） を `Allow` （`許可`）に設定する。
   4. `edge://settings/system` を開く。
   5. `Startup boost`（`スタートアップ ブースト`） をオフにする。
   6. `Continue running background extensions and apps when Microsoft Edge is closed`（`Microsoft Edge が終了してもバック グラウンドの拡張機能およびアプリの実行を続行する`） をオフにする。
   7. Edgeを終了する。
6. (Windows Enterprise マルチセッションのみ) ユーザーBにログオンする

## 検証

### インストーラー検証

#### タスクスケジューラー設定

* ThinBridgeをインストール済みの場合、一旦アンインストールする
* マシンのタスク スケジューラを起動する
* 「タスク スケジューラ」->「タスク スケジューラ ライブラリ」を開く
* もし、ThinBridgeRuleUpdateTaskというタスクが存在していた場合、削除する
* ThinBridgeのインストーラーを実行する
* 端末のタスク スケジューラを起動する
* 「タスク スケジューラ」->「タスク スケジューラ ライブラリ」を開く
  * [x] ThinBridgeRuleUpdateTaskというタスクが存在すること
  * [x] ThinBridgeRuleUpdateTaskの設定内容が、以下の通りであること
    * 全般
      * 名前: ThinBridgeRuleUpdateTask
      * セキュリティオプション
        * タスクの実行時に使うユーザー アカウント: SYSTEM
        * ユーザーがログオンしているかどうかに関わらず実行する: チェック
        * 最上位の特権で実行する: チェック
        * 構成: Windows Vista, Windows Server 2008
    * トリガー
      * タスクの開始: スケジュールに従う
      * 設定
        * 一回: チェック
        * 開始: 2020/01/01 12:00:00
      * 詳細設定
        * 繰り返し間隔: 1時間
        * 継続時間: 無制限
        * 有効: チェック
    * 操作
      * 操作: プログラムの開始
      * プログラム/スクリプト: `C:\Program Files\ThinBridge\ThinBridgeRuleUpdater.exe`
      * 引数の追加: /ForceDelay
    * 条件
      * なし
    * 設定
      * タスクを要求時に実行する: チェック
      * スケジュールされた時刻にタスクを開始できなかった場合、すぐにタスクを実行する: チェック
      * タスクを停止するまでの時間: 3日間
      * 要求時に実行中のタスクが終了しない場合、タスクを強制的に停止する: チェック
      * 新しいインスタンスを開始しない

### BHO無効時のIEモードのタブの挙動を含む、Manifest V3での動作

#### 準備

以下の通り設定して検証を行う。

* [PreReleaseVerification/senario1.ini](PreReleaseVerification/senario1.ini) を `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` に配置する。
* `Computer Configuration\Administrative Templates\Windows Components\Internet Explorer\Security Features\Add-on Management` （`コンピューターの構成\管理用テンプレート\Windows コンポーネント\Internet Explorer\セキュリティの機能\アドオン管理`）を開いて、以下のポリシーを設定する。
  * `Add-on List`（`アドオンの一覧`）
    * `Enabled`（`有効`）に設定して、`Add-on List`→`Show...`（`アドオンの一覧`→`表示...`）をクリックし、以下の名前の項目を設定（項目がなければ追加）して、`OK` を押してダイアログを閉じ、`OK` を押して変更を保存する。
      * `Value name`（`値の名前`）：`{3A56619B-37AC-40DA-833E-410F3BEDCBDC}`
      * `Value`（`値`）：`0`
* EdgeのIEモードタブの設定を以下の通り行う。
   1. Edgeを起動し、`edge://settings/defaultBrowser` を開く。
   2. 以下のURLのみが登録された状態にする（必要な項目を `Add` で追加し、それ以外を削除する）。
      * https://piro.sakura.ne.jp/apps/jspanel.html

#### 検証

1. Edgeを起動する。
2. リンクによるEdgeからChromeへのページ遷移の検証：
   1. Edge→Chromeの検証のため、Edgeで https://groonga.org/ja/ を開く。
      * 期待される結果：
        * [x] Edge上で https://groonga.org/ja/ が読み込まれる。
   2. `English` のリンクをクリックする。
      * 期待される結果：
        * [x] 空白のページが現在のタブに読み込まれ、すぐに元のページに戻る。
        * [x] Chromeでタブが開かれ、https://groonga.org/ が読み込まれる。
        * [-] (Windows Enterprise マルチセッションのみ) ユーザーBでEdgeやChromeが開いていない。
   3. Edge（IEモード）→Chromeの検証のため、Edgeで https://piro.sakura.ne.jp/apps/jspanel.html を開く。
      * 期待される結果：
        * [x] EdgeのタブがIEモードに切り替わる。
   4. 以下のスクリプトを貼り付けて実行する。
      ```
      location.href='https://groonga.org/ja/';
      ```
      * 期待される結果：
        * [x] EdgeのIEモードのタブで https://groonga.org/ja/ が読み込まれる。
   5. `English` のリンクをクリックする。
      * 期待される結果：
        * [x] EdgeのIEモードのタブで https://groonga.org/ が読み込まれる。（Manifest V2版と非互換の動作だが、仕様上不可避な挙動）
        * [x] Chromeでタブが開かれ、https://groonga.org/ が読み込まれる。
        * [-] (Windows Enterprise マルチセッションのみ) ユーザーBでEdgeやChromeが開いていない。
3. JavaScriptによるEdgeからChromeへのページ遷移の検証：
   1. Edge→Chromeの検証のため、Edgeで https://piro.sakura.ne.jp/apps/jspanel を開く。
      * 期待される結果：
        * [x] Edge上で https://piro.sakura.ne.jp/apps/jspanel が読み込まれる。
   2. 以下のスクリプトを貼り付けて実行する。
      ```
      window.open('https://example.net/', '_blank');
      ```
      * 期待される結果：
        * [x] Chromeでタブが開かれ、https://example.net/ が読み込まれる。
        * [x] 空白のタブがEdge上に残っていない。
        * [-] (Windows Enterprise マルチセッションのみ) ユーザーBでEdgeやChromeが開いていない。
   3. 以下のスクリプトを貼り付けて実行する。
      ```
      window.open('https://example.net/', '_blank', 'toolbar=no');
      ```
      * 期待される結果：
        * [x] Chromeでタブが開かれ、https://example.net/ が読み込まれる。
        * [x] 空白のポップアップウィンドウがEdge上に残っていない。
        * [-] (Windows Enterprise マルチセッションのみ) ユーザーBでEdgeやChromeが開いていない。
4. リンクによるChromeからEdgeへのページ遷移の検証 (`@EXCLUDE_GROUP`を使用するためThinBridge v4.2.0.0以降が必要)：
   1. Chrome→Edgeの検証のため、Chromeで https://groonga.org/ を開く。
      * 期待される結果：
        * [-] Chrome上で https://groonga.org/ が読み込まれる。
   2. `日本語` リンクをクリックする。
      * 期待される結果：
        * [-] 空白のページが現在のタブに読み込まれ、すぐに元のページに戻る。
        * [-] Edgeでタブが開かれ、https://groonga.org/ja/ が読み込まれる。
        * [-] (Windows Enterprise マルチセッションのみ) ユーザーBでEdgeやChromeが開いていない。
5. JavaScriptによるChromeからEdgeへのページ遷移の検証：
   1. Chrome→Edgeの検証のため、Chromeで https://www.piro.sakura.ne.jp/apps/jspanel.html を開く。
      * 期待される結果：
        * [-] 証明書の警告が表示される。
        * [-] 警告を無視して先に進むと、Chrome上で https://www.piro.sakura.ne.jp/apps/jspanel.html が読み込まれる。
   2. 以下のスクリプトを貼り付けて実行する。
      ```
      window.open('https://example.com/', '_blank');
      ```
      * 期待される結果：
        * [-] Edgeでタブが開かれ、 https://example.com/ が読み込まれる。
        * [-] 空白のタブがChrome上に残っていない。
        * [-] (Windows Enterprise マルチセッションのみ) ユーザーBでEdgeやChromeが開いていない。
   3. 以下のスクリプトを貼り付けて実行する。
      ```
      window.open('https://example.com/', '_blank', 'toolbar=no');
      ```
      * 期待される結果：
        * [-] Edgeでタブが開かれ、https://example.com/ が読み込まれる。
        * [-] 空白のポップアップウィンドウがChrome上に残っていない。
        * [-] (Windows Enterprise マルチセッションのみ) ユーザーBでEdgeやChromeが開いていない。
6. EdgeのIEモードのタブから開かれたポップアップウィンドウからのページ遷移の検証：
   1. Edgeで https://piro.sakura.ne.jp/apps/jspanel.html を開く。
      * 期待される結果：
        * [x] タブがIEモードに切り替わる。
   2. 以下のスクリプトを貼り付けて実行する。
      ```
      window.open('https://example.net/', '_blank', 'toolbar=no');
      ```
      * 期待される結果：
        * [x] IEモードのポップアップウィンドウが開かれるがすぐ閉じる。
        * [x] Chrome上でhttps://example.net/ が読み込まれる。
        * [-] (Windows Enterprise マルチセッションのみ) ユーザーBでEdgeやChromeが開いていない。
7. リダイレクト対象のURLを新規タブで開いた際の挙動の検証：
   1. Edgeで https://example.com/ を開く。
      * 期待される結果：
        * [x] Edge上で https://example.com/ が読み込まれる。
   2. `More information...` のリンクをミドルクリックする。
      * 期待される結果：
        * [x] Chrome上で https://www.iana.org/help/example-domains が開かれる。
        * [x] 空白のタブがEdge上に残っていない。
        * [-] (Windows Enterprise マルチセッションのみ) ユーザーBでEdgeやChromeが開いていない。
   3. Chromeで https://www.piro.sakura.ne.jp/ を開く。
      * 期待される結果：
        * [-] Chrome上で https://www.piro.sakura.ne.jp/ が読み込まれる。
   4. ページ最下部の `outsider reflex` のリンクをミドルクリックする。
      * 期待される結果：
        * [-] Edge上で https://piro.sakura.ne.jp/ が開かれる。
        * [-] 空白のタブがChrome上に残っていない。
        * [-] (Windows Enterprise マルチセッションのみ) ユーザーBでEdgeやChromeが開いていない。
8. Edge、Chromeを終了する。

### BHO有効時のIEモードのタブの挙動を含む、Manifest V3での動作

#### 準備

以下の通り設定して検証を行う。

* 設定は`BHO無効時のIEモードのタブの挙動を含む、Manifest V3での動作`と同一とする。（[PreReleaseVerification/senario1.ini](PreReleaseVerification/senario1.ini) を `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` に配置する。）
* `Computer Configuration\Administrative Templates\Windows Components\Internet Explorer\Security Features\Add-on Management` （`コンピューターの構成\管理用テンプレート\Windows コンポーネント\Internet Explorer\セキュリティの機能\アドオン管理`）を開いて、以下のポリシーを設定する。
  * `Add-on List`（`アドオンの一覧`）
    * `Enabled`（`有効`）に設定して、`Add-on List`→`Show...`（`アドオンの一覧`→`表示...`）をクリックし、以下の名前の項目を設定（項目がなければ追加）して、`OK` を押してダイアログを閉じ、`OK` を押して変更を保存する。
      * `Value name`（`値の名前`）：`{3A56619B-37AC-40DA-833E-410F3BEDCBDC}`
      * `Value`（`値`）：`1`
* EdgeのIEモードタブの設定を以下の通り行う。
   1. Edgeを起動し、`edge://settings/defaultBrowser` を開く。
   2. 以下のURLのみが登録された状態にする（必要な項目を `Add` で追加し、それ以外を削除する）。
      * https://piro.sakura.ne.jp/apps/jspanel.html

#### 検証

1. Edgeを起動する。
2. リンクによるEdgeからChromeへのページ遷移の検証：
   1. Edge（IEモード）→Chromeの検証のため、Edgeで https://piro.sakura.ne.jp/apps/jspanel.html を開く。
      * 期待される結果：
        * [x] EdgeのタブがIEモードに切り替わる。
   2. 以下のスクリプトを貼り付けて実行する。
      ```
      location.href='https://groonga.org/ja/';
      ```
      * 期待される結果：
        * [x] EdgeのIEモードのタブで https://groonga.org/ja/ が読み込まれる。
   3. `English` のリンクをクリックする。
      * 期待される結果：
        * [x] EdgeのIEモードのタブがページ遷移しないでそのまま維持される。
        * [x] Chromeでタブが開かれ、https://groonga.org/ が読み込まれる。
        * [-] (Windows Enterprise マルチセッションのみ) ユーザーBでEdgeやChromeが開いていない。
2. EdgeのIEモードのタブから開かれたポップアップウィンドウからのページ遷移の検証：
   1. Edgeで https://piro.sakura.ne.jp/apps/jspanel.html を開く。
      * 期待される結果：
        * [x] タブがIEモードに切り替わる。
   2. 以下のスクリプトを貼り付けて実行する。
      ```
      window.open('https://example.net/', '_blank', 'toolbar=no');
      ```
      * 期待される結果：
        * [x] Chromeでタブが開かれ、https://example.net/ が読み込まれる。
        * [x] IEモードのポップアップウィンドウが残っていない。
        * [-] (Windows Enterprise マルチセッションのみ) ユーザーBでEdgeやChromeが開いていない。
3. クエリーパラメーターを含むURLをリダイレクトする際の挙動の検証：
   1. Edgeの通常タブ→Chromeの検証のため、Edgeで https://piro.sakura.ne.jp/apps/jspanel を開く。
      * 期待される結果：
        * [x] Edge上で https://piro.sakura.ne.jp/apps/jspanel が読み込まれる。
   2. 以下のスクリプトを貼り付けて実行する。
      ```
      window.open('https://example.net/?query-parameter=value', '_blank');
      ```
      * 期待される結果：
        * [x] Chromeでタブが開かれ、https://example.net/?query-parameter=value が読み込まれる。
        * [x] 空白のタブがEdge上に残っていない。
        * [-] (Windows Enterprise マルチセッションのみ) ユーザーBでEdgeやChromeが開いていない。
   3. EdgeのIEモードのタブ→Chromeの検証のため、Edgeで https://piro.sakura.ne.jp/apps/jspanel.html を開く。
      * 期待される結果：
        * [x] タブがIEモードに切り替わる。
   4. 以下のスクリプトを貼り付けて実行する。
      ```
      location.href = 'https://example.net/?query-parameter=value';
      ```
      * 期待される結果：
        * [x] Chromeでタブが開かれ、https://example.net/?query-parameter=value が読み込まれる。
        * [x] 空白のタブがEdge上に残っていない。
        * [-] (Windows Enterprise マルチセッションのみ) ユーザーBでEdgeやChromeが開いていない。
4. Edge、Chromeを終了する。


### EdgeのIEモード境界をまたぐページ遷移における設定無効時の動作

#### 準備

以下の通り設定して検証を行う。

* [PreReleaseVerification/senario2.ini](PreReleaseVerification/senario2.ini) を `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` に配置する。
* `Computer Configuration\Administrative Templates\Windows Components\Internet Explorer\Security Features\Add-on Management` （`コンピューターの構成\管理用テンプレート\Windows コンポーネント\Internet Explorer\セキュリティの機能\アドオン管理`）を開いて、以下のポリシーを設定する。
  * `Add-on List`（`アドオンの一覧`）
    * `Enabled`（`有効`）に設定して、`Add-on List`→`Show...`（`アドオンの一覧`→`表示...`）をクリックし、以下の名前の項目を設定（項目がなければ追加）して、`OK` を押してダイアログを閉じ、`OK` を押して変更を保存する。
      * `Value name`（`値の名前`）：`{3A56619B-37AC-40DA-833E-410F3BEDCBDC}`
      * `Value`（`値`）：`1`
* EdgeのIEモードタブの設定を以下の通り行う。
   1. Edgeを起動し、`edge://settings/defaultBrowser` を開く。
   2. すべての項目を削除する。

#### 検証

1. Edgeを起動する。
2. リンクによるページ遷移でのEdge上での閲覧継続の検証：
   1. Edge（通常モード）→Edge（通常モード）の検証のため、Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://example.com/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   2. `More information...` のリンクをクリックする。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://www.iana.org/help/example-domains が開かれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   3. Edge（通常モード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://www.iana.org/help/example-domains
   4. Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://example.com/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   5. `More information...` のリンクをクリックする。
      * 期待される結果：
        * [ ] タブがIEモードに切り替わり、 https://www.iana.org/help/example-domains が開かれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   6. Edge（IEモード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： https://www.iana.org/help/example-domains
      * 追加： https://example.com/
   7. Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://example.com/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   8. アドレスバーに https://www.iana.org/help/example-domains を入力し、Enterする。
      * 期待される結果：
        * [ ] タブが通常モードに切り替わり、 https://www.iana.org/help/example-domains が開かれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   9. Edge（IEモード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://www.iana.org/help/example-domains
   7. Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://example.com/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   8. アドレスバーに https://www.iana.org/help/example-domains を入力し、Enterする。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://www.iana.org/help/example-domains が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
3. Edgeを終了する。


### EdgeのIEモード境界をまたぐページ遷移におけるDefault設定の動作

#### 準備

以下の通り設定して検証を行う。

* [PreReleaseVerification/senario3.ini](PreReleaseVerification/senario3.ini) を `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` に配置する。
* `Computer Configuration\Administrative Templates\Windows Components\Internet Explorer\Security Features\Add-on Management` （`コンピューターの構成\管理用テンプレート\Windows コンポーネント\Internet Explorer\セキュリティの機能\アドオン管理`）を開いて、以下のポリシーを設定する。
  * `Add-on List`（`アドオンの一覧`）
    * `Enabled`（`有効`）に設定して、`Add-on List`→`Show...`（`アドオンの一覧`→`表示...`）をクリックし、以下の名前の項目を設定（項目がなければ追加）して、`OK` を押してダイアログを閉じ、`OK` を押して変更を保存する。
      * `Value name`（`値の名前`）：`{3A56619B-37AC-40DA-833E-410F3BEDCBDC}`
      * `Value`（`値`）：`1`
* EdgeのIEモードタブの設定を以下の通り行う。
   1. Edgeを起動し、`edge://settings/defaultBrowser` を開く。
   2. すべての項目を削除する。

#### 検証

1. Edgeを起動する。
2. EdgeとIEの両方に該当するURLの動作の検証：
   1. Edge（通常モード）→Edge（通常モード）の検証のため、Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://example.com/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   2. `More information...` のリンクをクリックする。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://www.iana.org/help/example-domains が開かれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   3. Edge（通常モード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://www.iana.org/help/example-domains
   4. Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://example.com/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   5. `More information...` のリンクをクリックする。
      * 期待される結果：
        * [ ] タブがIEモードに切り替わり、 https://www.iana.org/help/example-domains が開かれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   6. Edge（IEモード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： https://www.iana.org/help/example-domains
      * 追加： https://example.com/
   7. Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://example.com/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   8. アドレスバーに https://www.iana.org/help/example-domains を入力し、Enterする。
      * 期待される結果：
        * [ ] タブが通常モードに切り替わり、 https://www.iana.org/help/example-domains が開かれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   9. Edge（IEモード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://www.iana.org/help/example-domains
   7. Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://example.com/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   8. `More information...` のリンクをクリックする。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://www.iana.org/help/example-domains が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
3. IEのみに該当するURLの動作の検証：
   1. Edge（通常モード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： 全項目
   2. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   3. `Mroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] タブがページ遷移しない。
        * [ ] ThinBridgeによるリダイレクトが発生し、 https://mroonga.org/ がIEで開かれるか、IEの起動を試みている旨を示すダイアログが表示される。
        * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでIEへのリダイレクトが発生しない。
   4. Edge（通常モード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://mroonga.org/
   5. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   6. `Mroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] タブがIEモードに切り替わらず、ページ遷移しない。
        * [ ] ThinBridgeによるリダイレクトが発生し、 https://mroonga.org/ がIEで開かれるか、IEの起動を試みている旨を示すダイアログが表示される。
        * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでIEへのリダイレクトが発生しない。
   7. Edge（IEモード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： https://mroonga.org/
      * 追加： https://groonga.org/related-projects.html
   8. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   9. アドレスバーに https://mroonga.org/ を入力し、Enterする。
      * 期待される結果：
        * [ ] Edgeのタブがページ遷移しない。また、試行回によってはタブがThinBridgeによって閉じられる。
        * [ ] ThinBridgeによるリダイレクトが発生し、 https://mroonga.org/ がIEで開かれるか、IEの起動を試みている旨を示すダイアログが表示される。
        * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでIEへのリダイレクトが発生しない。
   10. Edge（IEモード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
       * 追加： https://mroonga.org/
   11. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
       * 期待される結果：
         * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
         * [ ] ThinBridgeによるリダイレクトが発生しない。
   12. `Mroonga` のリンクをクリックする。
       * 期待される結果：
          * [ ] Edgeのタブがページ遷移しない。
          * [ ] ThinBridgeによるリダイレクトが発生し、 https://mroonga.org/ がIEで開かれるか、IEの起動を試みている旨を示すダイアログが表示される。
          * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでIEへのリダイレクトが発生しない。
4. 共用URL（CUSTOM18）に該当するURLの動作の検証：
   1. Edge（通常モード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： 全項目
   2. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   3. `Rroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] Edgeの通常タブで http://ranguba.org/#about-rroonga が開かれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   4. Edge（通常モード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： http://ranguba.org/#about-rroonga
   5. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   6. `Rroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] タブがIEモードに切り替わり、 http://ranguba.org/#about-rroonga が開かれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   7. Edge（IEモード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： http://ranguba.org/#about-rroonga
      * 追加： https://groonga.org/related-projects.html
   8. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   9. アドレスバーに http://ranguba.org/#about-rroonga を入力し、Enterする。
      * 期待される結果：
        * [ ] タブが通常モードに切り替わり、 http://ranguba.org/#about-rroonga が開かれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   10. Edge（IEモード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
       * 追加： http://ranguba.org/#about-rroonga
   11. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
       * 期待される結果：
         * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
         * [ ] ThinBridgeによるリダイレクトが発生しない。
   12. `Rroonga` のリンクをクリックする。
       * 期待される結果：
         * [ ] EdgeのIEモードのタブで http://ranguba.org/#about-rroonga が読み込まれる。
         * [ ] ThinBridgeによるリダイレクトが発生しない。
5. いずれのブラウザーにも該当しないURLの動作の検証：
   1. Edge（通常モード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： 全項目
   2. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   3. `PGroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] Edgeのタブがページ遷移しない。
        * [ ] ThinBridgeによるリダイレクトが発生し、「Citrixがインストールされていないか、設定されていないため起動できません。」というメッセージが表示される。
        * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでCitrixへのリダイレクトが発生しない。
   4. Edge（通常モード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://pgroonga.github.io/
   5. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   6. `PGroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] Edgeのタブがページ遷移しない。
        * [ ] ThinBridgeによるリダイレクトが発生し、「Citrixがインストールされていないか、設定されていないため起動できません。」というメッセージが表示される。
        * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでCitrixへのリダイレクトが発生しない。
   7. Edge（IEモード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： https://pgroonga.github.io/
      * 追加： https://groonga.org/related-projects.html
   8. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   9. アドレスバーに https://pgroonga.github.io/ を入力し、Enterする。
      * 期待される結果：
        * [ ] Edgeのタブがページ遷移しない。また、試行回によってはタブがThinBridgeによって閉じられる。
        * [ ] ThinBridgeによるリダイレクトが発生し、「Citrixがインストールされていないか、設定されていないため起動できません。」というメッセージが表示される。
        * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでCitrixへのリダイレクトが発生しない。
   10. Edge（IEモード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
       * 追加： https://pgroonga.github.io/
   11. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
       * 期待される結果：
         * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
         * [ ] ThinBridgeによるリダイレクトが発生しない。
   12. `PGroonga` のリンクをクリックする。
       * 期待される結果：
         * [ ] Edgeのタブがページ遷移しない。
         * [ ] ThinBridgeによるリダイレクトが発生し、「Citrixがインストールされていないか、設定されていないため起動できません。」というメッセージが表示される。
         * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでCitrixへのリダイレクトが発生しない。
6. Edgeを終了する。

### EdgeのIEモード境界をまたぐページ遷移におけるDefault設定なしの動作

#### 準備

以下の通り設定して検証を行う。

* [PreReleaseVerification/senario4.ini](PreReleaseVerification/senario4.ini) を `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` に配置する。
* `Computer Configuration\Administrative Templates\Windows Components\Internet Explorer\Security Features\Add-on Management` （`コンピューターの構成\管理用テンプレート\Windows コンポーネント\Internet Explorer\セキュリティの機能\アドオン管理`）を開いて、以下のポリシーを設定する。
  * `Add-on List`（`アドオンの一覧`）
    * `Enabled`（`有効`）に設定して、`Add-on List`→`Show...`（`アドオンの一覧`→`表示...`）をクリックし、以下の名前の項目を設定（項目がなければ追加）して、`OK` を押してダイアログを閉じ、`OK` を押して変更を保存する。
      * `Value name`（`値の名前`）：`{3A56619B-37AC-40DA-833E-410F3BEDCBDC}`
      * `Value`（`値`）：`1`
* EdgeのIEモードタブの設定を以下の通り行う。
   1. Edgeを起動し、`edge://settings/defaultBrowser` を開く。
   2. すべての項目を削除する。

#### 検証

1. Edgeを起動する。
2. EdgeとIEの両方に該当するURLの動作の検証：
   1. Edge（通常モード）→Edge（通常モード）の検証のため、Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://example.com/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   2. `More information...` のリンクをクリックする。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://www.iana.org/help/example-domains が開かれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   3. Edge（通常モード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://www.iana.org/help/example-domains
   4. Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://example.com/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   5. `More information...` のリンクをクリックする。
      * 期待される結果：
        * [ ] タブがIEモードに切り替わり、 https://www.iana.org/help/example-domains が開かれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   6. Edge（IEモード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： https://www.iana.org/help/example-domains
      * 追加： https://example.com/
   7. Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://example.com/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   8. アドレスバーに https://www.iana.org/help/example-domains を入力し、Enterする。
      * 期待される結果：
        * [ ] タブが通常モードに切り替わり、 https://www.iana.org/help/example-domains が開かれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   9. Edge（IEモード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://www.iana.org/help/example-domains
   7. Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://example.com/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   8. `More information...` のリンクをクリックする。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://www.iana.org/help/example-domains が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
3. IEのみに該当するURLの動作の検証：
   1. Edge（通常モード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： 全項目
   2. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   3. `Mroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] Edgeのタブがページ遷移しない。
        * [ ] ThinBridgeによるリダイレクトが発生し、 https://mroonga.org/ がIEで開かれるか、IEの起動を試みている旨を示すダイアログが表示される。
        * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでIEへのリダイレクトが発生しない。
   4. Edge（通常モード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://mroonga.org/
   5. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   6. `Mroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] タブがIEモードに切り替わらず、ページ遷移しない。
        * [ ] ThinBridgeによるリダイレクトが発生し、 https://mroonga.org/ がIEで開かれるか、IEの起動を試みている旨を示すダイアログが表示される。
        * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでIEへのリダイレクトが発生しない。
   7. Edge（IEモード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： https://mroonga.org/
      * 追加： https://groonga.org/related-projects.html
   8. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   9. アドレスバーに https://mroonga.org/ を入力し、Enterする。
      * 期待される結果：
        * [ ] Edgeのタブがページ遷移しない。また、試行回によってはタブがThinBridgeによって閉じられる。
        * [ ] ThinBridgeによるリダイレクトが発生し、 https://mroonga.org/ がIEで開かれるか、IEの起動を試みている旨を示すダイアログが表示される。
        * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでIEへのリダイレクトが発生しない。
   10. Edge（IEモード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
       * 追加： https://mroonga.org/
   11. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
       * 期待される結果：
         * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
         * [ ] ThinBridgeによるリダイレクトが発生しない。
   12. `Mroonga` のリンクをクリックする。
       * 期待される結果：
          * [ ] Edgeのタブがページ遷移しない。
          * [ ] ThinBridgeによるリダイレクトが発生し、 https://mroonga.org/ がIEで開かれるか、IEの起動を試みている旨を示すダイアログが表示される。
          * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでIEへのリダイレクトが発生しない。
4. 共用URL（CUSTOM18）に該当するURLの動作の検証：
   1. Edge（通常モード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： 全項目
   2. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   3. `Rroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] Edgeの通常タブで http://ranguba.org/#about-rroonga が開かれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   4. Edge（通常モード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： http://ranguba.org/#about-rroonga
   5. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   6. `Rroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] タブがIEモードに切り替わり、 http://ranguba.org/#about-rroonga が開かれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   7. Edge（IEモード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： http://ranguba.org/#about-rroonga
      * 追加： https://groonga.org/related-projects.html
   8. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   9. アドレスバーに http://ranguba.org/#about-rroonga を入力し、Enterする。
      * 期待される結果：
        * [ ] タブが通常モードに切り替わり、 http://ranguba.org/#about-rroonga が開かれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   10. Edge（IEモード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
       * 追加： http://ranguba.org/#about-rroonga
   11. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
       * 期待される結果：
         * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
         * [ ] ThinBridgeによるリダイレクトが発生しない。
   12. `Rroonga` のリンクをクリックする。
       * 期待される結果：
         * [ ] EdgeのIEモードのタブで http://ranguba.org/#about-rroonga が読み込まれる。
         * [ ] ThinBridgeによるリダイレクトが発生しない。
5. いずれのブラウザーにも該当しないURLの動作の検証：
   1. Edge（通常モード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： 全項目
   2. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   3. `PGroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://pgroonga.github.io/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   4. Edge（通常モード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://pgroonga.github.io/
   5. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   6. `PGroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://pgroonga.github.io/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   7. Edge（IEモード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： https://pgroonga.github.io/
      * 追加： https://groonga.org/related-projects.html
   8. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   9. アドレスバーに https://pgroonga.github.io/ を入力し、Enterする。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://pgroonga.github.io/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   10. Edge（IEモード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
       * 追加： https://pgroonga.github.io/
   11. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
       * 期待される結果：
         * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
         * [ ] ThinBridgeによるリダイレクトが発生しない。
   12. `PGroonga` のリンクをクリックする。
       * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://pgroonga.github.io/ が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
6. Edgeを終了する。



### `onTabUpdated()`によるリダイレクトの動作検証

#### 背景

crxパッケージ化されたアドオンをGPOでインストールした状態では、通常は`onBeforeRequest()`でリダイレクトが行われる。しかし、`onBeforeRequest()`実行時点で設定の読み込みが完了していない状況においては、`onBeforeRequest()`ではリダイレクトが行われず、`onTabUpdated()`でリダイレクトが行われる。両者では、WebExtensionsの仕様上どうしても挙動が異なってくるが、本アドオンではいずれの状況においても安定的にリダイレクト機能を提供する必要がある。

このため、本テスト項目は、常に`onTabUpdated()`でリダイレクトが行われる状況でテストを実施できるよう、開発者モードで実施する。
（GPOインストールではない状況では仕様上`onBeforeRequest()`が動作しない。）

#### 準備

設定は`EdgeのIEモード境界をまたぐページ遷移におけるDefault設定の動作`と同一とする。
（[PreReleaseVerification/senario3.ini](PreReleaseVerification/senario3.ini) を `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` に配置する。）

1. Edgeを起動する。
2. 拡張機能の管理画面（`edge://extensions`）を開く。
3. `開発者モード` を有効化する。
4. GPOでインストールした拡張機能を一時的にアンインストールする。
   1. `gpedit.msc` を起動する。
   2. `Computer Configuration\Administrative Templates\Microsoft Edge\Extensions`（`コンピューターの構成\管理用テンプレート\Microsoft Edge\拡張機能`）の `Control which extensions are installed silently`（`サイレント インストールされる拡張機能を制御する`）を開き、検証環境の準備段階で追加した項目について、先頭に `_` を挿入して保存する。
   3. Edgeの拡張機能管理画面上からThinBridgeが消えたことを確認する。
5. `展開して読み込み` で `C:\Users\Public\webextensions\edge` を読み込んで、IDを控える。
   例：`bplpacmhanoonjdmoelfokbphgopcbma`
6. `C:\Program Files\ThinBridge\ThinBridgeHost\edge.json` のアクセス権を変更し、ユーザー権限での書き込みを許可した上で、`"allowed_origins"` に、先ほど控えたIDに基づくURLを追加する。
   例：`"chrome-extension://bplpacmhanoonjdmoelfokbphgopcbma/"`
7. BHOを有効化する
   1. `gpedit.msc` を起動する。
   2. `Computer Configuration\Administrative Templates\Windows Components\Internet Explorer\Security Features\Add-on Management` （`コンピューターの構成\管理用テンプレート\Windows コンポーネント\Internet Explorer\セキュリティの機能\アドオン管理`）を開いて、以下のポリシーを設定する。
      * `Add-on List`（`アドオンの一覧`）
      * `Enabled`（`有効`）に設定して、`Add-on List`→`Show...`（`アドオンの一覧`→`表示...`）をクリックし、以下の名前の項目を設定（項目がなければ追加）して、`OK` を押してダイアログを閉じ、`OK` を押して変更を保存する。
        * `Value name`（`値の名前`）：`{3A56619B-37AC-40DA-833E-410F3BEDCBDC}`
        * `Value`（`値`）：`1`
8. もし、Edgeで「この拡張機能は既知のソースからのものではなく、ユーザーの知らないうちに追加された可能性があります。」の警告が出る場合、GPOでThinBridge拡張機能のインストールを許可する。
   1. `gpedit.msc` を起動する。
   2. `Computer Configuration\Administrative Templates\Microsoft Edge\Extensions`（`コンピューターの構成\管理用テンプレート\Microsoft Edge\拡張機能`）の `Allow specific extensions to be installed`（`特定の拡張機能のインストールを許可する`）を開く。
   3. `Enabled`（`有効`）に設定して、`Extension IDs to exampt from the block list`→`Show...`（`禁止リストから除外する拡張子 ID`→`表示...`）をクリックする
   4. 先程控えたIDを追加して、`OK` を押してダイアログを閉じ、`OK` を押して変更を保存する。
      例：`bplpacmhanoonjdmoelfokbphgopcbma`
9. （必要に応じて: 「サービスワーカー」をクリックし、DevToolsを起動する。当画面で逐次状況を観察しながらテストする。）

#### 検証

本検証においてはリダイレクトが発生する場合の挙動に大きな違いがあるため、読み込みが継続する場合の挙動は検証せず、リダイレクトする場合の検証を重点的に行う。

1. Edgeを起動する。
2. IEへのリダイレクト時の動作の検証：
   1. Edge（通常モード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： 全項目
   2. Edgeで新しいタブで https://groonga.org/ を開き、`Related projects` のリンクを辿って https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   3. `Mroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] タブが一瞬ページ遷移するが、 https://groonga.org/related-projects.html に戻っている。
        * [ ] ThinBridgeによるリダイレクトが発生し、 https://mroonga.org/ がIEで開かれるか、IEの起動を試みている旨を示すダイアログが表示される。
        * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでIEへのリダイレクトが発生しない。
   4. Edge（通常モード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://mroonga.org/
   5. Edgeで新しいタブで https://groonga.org/ を開き、`Related projects` のリンクを辿って https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   6. `Mroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] タブが一瞬ページ遷移するが、通常モードのタブで https://groonga.org/related-projects.html に戻っている。
        * [ ] ThinBridgeによるリダイレクトが発生し、 https://mroonga.org/ がIEで開かれるか、IEの起動を試みている旨を示すダイアログが表示される。
        * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでIEへのリダイレクトが発生しない。
   7. Edge（IEモード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： https://mroonga.org/
      * 追加： https://groonga.org/related-projects.html
   8. Edgeで新しいタブで https://groonga.org/ を開き、`Related projects` のリンクを辿って https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   9. アドレスバーに https://mroonga.org/ を入力し、Enterする。
      * 期待される結果：
        * [ ] タブが一瞬ページ遷移するが、 https://groonga.org/related-projects.html に戻っている。また、試行回によってはタブがThinBridgeによって閉じられる。
        * [ ] ThinBridgeによるリダイレクトが発生し、 https://mroonga.org/ がIEで開かれるか、IEの起動を試みている旨を示すダイアログが表示される。
        * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでIEへのリダイレクトが発生しない。
   10. Edge（IEモード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
       * 追加： https://mroonga.org/
   11. Edgeで新しいタブで https://groonga.org/ を開き、`Related projects` のリンクを辿って https://groonga.org/related-projects.html を開く。
       * 期待される結果：
         * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
         * [ ] ThinBridgeによるリダイレクトが発生しない。
   12. `Mroonga` のリンクをクリックする。
       * 期待される結果：
         * [ ] タブが一瞬ページ遷移するが、 https://groonga.org/related-projects.html に戻っている。
         * [ ] ThinBridgeによるリダイレクトが発生し、 https://mroonga.org/ がIEで開かれるか、IEの起動を試みている旨を示すダイアログが表示される。
         * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでIEへのリダイレクトが発生しない。
3. いずれのブラウザーにも該当しないURLの動作の検証：
   1. Edge（通常モード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： 全項目
   2. Edgeで新しいタブで https://groonga.org/ を開き、`Related projects` のリンクを辿って https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   3. `PGroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] タブが一瞬ページ遷移するが、 https://groonga.org/related-projects.html に戻っている。
        * [ ] ThinBridgeによるリダイレクトが発生し、「Citrixがインストールされていないか、設定されていないため起動できません。」というメッセージが表示される。
        * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでCitrixへのリダイレクトが発生しない。
   4. Edge（通常モード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://pgroonga.github.io/
   5. Edgeで新しいタブで https://groonga.org/ を開き、`Related projects` のリンクを辿って https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   6. `PGroonga` のリンクをクリックする。
      * 期待される結果：
        * [ ] タブが一瞬ページ遷移するが、 https://groonga.org/related-projects.html に戻っている。
        * [ ] ThinBridgeによるリダイレクトが発生し、「Citrixがインストールされていないか、設定されていないため起動できません。」というメッセージが表示される。
        * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでCitrixへのリダイレクトが発生しない。
   7. Edge（IEモード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： https://pgroonga.github.io/
      * 追加： https://groonga.org/related-projects.html
   8. Edgeで新しいタブで https://groonga.org/ を開き、`Related projects` のリンクを辿って https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
        * [ ] ThinBridgeによるリダイレクトが発生しない。
   9. アドレスバーに https://pgroonga.github.io/ を入力し、Enterする。
      * 期待される結果：
        * [ ] タブが一瞬ページ遷移するが、 https://groonga.org/related-projects.html に戻っている。また、試行回によってはタブがThinBridgeによって閉じられる。
        * [ ] ThinBridgeによるリダイレクトが発生し、「Citrixがインストールされていないか、設定されていないため起動できません。」というメッセージが表示される。
        * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでCitrixへのリダイレクトが発生しない。
   10. Edge（IEモード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
       * 追加： https://pgroonga.github.io/
   11. Edgeで新しいタブで https://groonga.org/ を開き、`Related projects` のリンクを辿って https://groonga.org/related-projects.html を開く。
       * 期待される結果：
         * [ ] EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
         * [ ] ThinBridgeによるリダイレクトが発生しない。
   12. `PGroonga` のリンクをクリックする。
       * 期待される結果：
         * [ ] タブが一瞬ページ遷移するが、 https://groonga.org/related-projects.html に戻っている。
         * [ ] ThinBridgeによるリダイレクトが発生し、「Citrixがインストールされていないか、設定されていないため起動できません。」というメッセージが表示される。
         * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでCitrixへのリダイレクトが発生しない。

#### 後始末

1. 拡張機能の管理画面（`edge://extensions`）を開く。
2. 「ThinBridge」拡張機能を削除する。
3. GPOで拡張機能をインストールする状態に戻す。
   1. `gpedit.msc` を起動する。
   2. `Computer Configuration\Administrative Templates\Microsoft Edge\Extensions`（`コンピューターの構成\管理用テンプレート\Microsoft Edge\拡張機能`）の `Control which extensions are installed silently`（`サイレント インストールされる拡張機能を制御する`）を開き、検証環境の準備段階で追加した項目について、先頭の `_` を削除して保存する。
   3. Edgeの拡張機能管理画面上にThinBridgeが表示されたことを確認する。
4. `開発者モード` を無効化する。
5. Edgeを終了する。


### サブフレームでのリダイレクトの動作検証

#### 準備

[PreReleaseVerification/senario5.ini](PreReleaseVerification/senario5.ini) を `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` に配置する。

#### 検証

1. Edgeを起動する。
2. 新規タブを開く。
3. URLエントリに `https://www.fluentd.org` を入力してEnterキーを押下する。
   * 期待される結果
     * [ ] トップフレームは https://www.fluentd.org に遷移する。
     * [ ] 「Featured Video」のサブフレームは空白ページに遷移する。
     * [ ] 「Featured Video」サブフレーム内のコンテンツ https://www.youtube.com/embed/sIVGsQgMHIo がChromeにリダイレクトされる。
     * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでChromeへのリダイレクトが発生しない。


### アドオンインストール前から存在する既存タブでの挙動

#### 準備

設定は`サブフレームでのリダイレクトの動作検証`と同一とする。
（[PreReleaseVerification/senario5.ini](PreReleaseVerification/senario5.ini) を `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` に配置する。）

#### 検証

1. Edgeを起動する。
2. 新規タブを開く。
3. アドレスバーに https://www.google.com を入力し、Enterする。
   * 期待される結果：
     * [ ] Chromeでタブが開かれ、https://www.google.com/ が読み込まれる。
     * [ ] Edgeのタブは閉じたりページ遷移したりせず新規タブがそのまま表示される。
     * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでChromeへのリダイレクトが発生しない。
4. GPOでインストールした拡張機能を一時的にアンインストールする。
   1. `gpedit.msc` を起動する。
   2. `Computer Configuration\Administrative Templates\Microsoft Edge\Extensions`（`コンピューターの構成\管理用テンプレート\Microsoft Edge\拡張機能`）の `Control which extensions are installed silently`（`サイレント インストールされる拡張機能を制御する`）を開き、検証環境の準備段階で追加した項目について、先頭に `_` を挿入して保存する。
   3. Edgeの拡張機能管理画面上（`edge://extensions/`）からThinBridgeが消えたことを確認する。
5. GPOでアンインストールした拡張機能を再度インストールする。
   1. `gpedit.msc` を起動する。
   2. `Computer Configuration\Administrative Templates\Microsoft Edge\Extensions`（`コンピューターの構成\管理用テンプレート\Microsoft Edge\拡張機能`）の `Control which extensions are installed silently`（`サイレント インストールされる拡張機能を制御する`）を開き、検証環境の準備段階で追加した項目について、先頭の `_` を削除して保存する。
   3. Edgeの拡張機能管理画面（`edge://extensions/`）にThinBridgeが再度表示されたことを確認する。
   4. アドオン再インストール後の既存タブの挙動を確認する。
     * 期待される結果：
      * [ ] 2.で開いたタブが閉じられない。
	   * [ ] 2で開いたタブのURLがChromeで新たに開かれていない。
6. 2.で開いたタブのアドレスバーに https://www.google.com を入力し、Enterする。
   * 期待される結果：
     * [ ] Chromeでタブが開かれ、https://www.google.com/ が読み込まれる。
     * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでChromeへのリダイレクトが発生しない。
     * [ ] Edgeのタブは閉じたりページ遷移したりせず新規タブがそのまま表示される。


### 既定のブラウザによるリダイレクト対象URLの挙動

#### 準備

1. 設定は`サブフレームでのリダイレクトの動作検証`と同一とする。
   （[PreReleaseVerification/senario5.ini](PreReleaseVerification/senario5.ini) を `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` に配置する。）
2. Windowsの既定のブラウザをEdgeに設定する。
3. 全てのEdgeウインドウを閉じる

#### 検証

1. `Win + R`キーで「ファイル名を指定して実行」ウインドウを開く。
2. https://www.google.com を入力し、Enterする。
   * 期待される結果：
     * [ ] 一瞬Edgeウインドウが開いたあと、すぐに閉じる
     * [ ] Chromeでタブが開かれ、https://www.google.com/ が読み込まれる。
     * [ ] (Windows Enterprise マルチセッションのみ) ユーザーBでChromeのタブが開かない。

## リダイレクト定義自動更新（ThinBridgeRuleUpdater）の動作確認

### ファイルから取得

#### 準備

1. [PreReleaseVerification/senario5.ini](PreReleaseVerification/senario5.ini) を `C:\Temp\senario5.ini` に配置する。）
2. リダイレクト定義自動更新設定（`C:\Program Files\ThinBridge\ThinBridgeRuleUpdaterSetting.exe`）を起動する
3. 「ThinBridgeリダイレクト定義ファイル取得先」に`C:\Temp\senario5.ini`を指定する
4. 「+」ボタンを押し、「ピークアウト間隔」に5分を指定する。
5. OKを押してダイアログを閉じる
6. 「接続＆取得データ テスト」ボタンを押し、接続テストを実行する
7. 取得が成功することを確認する
8. 「設定のみ保存」ボタンで設定を保存する

#### スタートアッププログラムによるログオン時の実行の動作確認

1. [PreReleaseVerification/senario1.ini](PreReleaseVerification/senario1.ini) を `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` に配置する。）
2. 現在のユーザーをログアウトする
3. 再度ログオンする
4. 一定時間待機する
5. リダイレクト定義自動更新のログを開く（`C:\Program Files\ThinBridge\TBUpdateLog\ThinBridgeRuleUpdaterYYYY-MM-DD.log`、YYYY-MM-DDは日付。）
  * [x] 以下のようなログが出力されていること
    ```
    自動更新//////////////////////////////////////////////////////////////////////////////////////////////////
    2025-10-14 13:41:33
    処理結果：成功：全て処理に成功しました 差分あり SUCCESS_ALL
    ExecUser:[XXX]LogonUser:[xXX]LogonUserSID:[XXX]CurrentSID:[XXX]
    SettingType:File
    【成功】
    設定ファイルの保存に成功しました。
    ThinBridgeRuleUpdater	2025-10-14 13:41:33	設定データ取得開始
    ThinBridgeRuleUpdater	2025-10-14 13:41:33	1:OK 200 OK_SERVER C:\temp\senario5.ini
    ThinBridgeRuleUpdater	2025-10-14 13:41:33	>>>ExecMain
    ThinBridgeRuleUpdater	2025-10-14 13:41:33	>>>GetThinBridgeRedirectRuleCIFS C:\temp\senario5.ini
    ...
    ThinBridgeRuleUpdater	2025-10-14 13:41:33	<<<GetThinBridgeRedirectRuleCIFS OK_SERVER
    ThinBridgeRuleUpdater	2025-10-14 13:41:33	>>>ExecMain
    ThinBridgeRuleUpdater	2025-10-14 13:41:33	設定データ取得完了
    ThinBridgeRuleUpdater	2025-10-14 13:41:33	設定データ差分確認開始
    ThinBridgeRuleUpdater	2025-10-14 13:41:33	設定データ差分確認完了
    ...
    ```
  * [x] 待機時間が出力されていないこと（待機していないこと）
  * [x] 「設定データ取得開始」が出力されていること
  * [x] 「設定データ取得完了」が出力されていること
  * [x] 「設定データ差分確認開始」が出力されていること
  * [x] 「設定データ差分確認完了」が出力されていること
  * [x] 「GetThinBridgeRedirectRuleCIFS」が出力されていること
6. `C:\Program Files\ThinBridge\ThinBridgeBHO.ini`の内容を確認する
  * [x] `C:\Temp\senario5.ini`と同じ内容になっていること

#### タスクスケジューラーによる実行の動作確認

1. [PreReleaseVerification/senario1.ini](PreReleaseVerification/senario1.ini) を `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` に配置する。）
2. 端末のタスク スケジューラを起動する
3. 「タスク スケジューラ」->「タスク スケジューラ ライブラリ」を開く
5. タスク スケジューラによるリダイレクト定義更新タスクの実行（12:00といった時刻の分の部分が0のときに発火する）を待つ
6. ThinBridgeRuleUpdateTaskタスクの完了を待つ（適宜リスト表示の余白を右クリック->最新の状態に更新を実行する）
7. リダイレクト定義自動更新のログを開く（`C:\Program Files\ThinBridge\TBUpdateLog\ThinBridgeRuleUpdaterYYYY-MM-DD.log`、YYYY-MM-DDは日付。）
  * [x] 以下のようなログが出力されていること
    ```
    自動更新//////////////////////////////////////////////////////////////////////////////////////////////////
    2025-10-14 14:00:01 待機(1分)
    ExecUser:[XXX]LogonUser:[XXX]LogonUserSID:[XXX]CurrentSID:[S-1-5-18]
    アクティブなログオンユーザーで再実行します。
    自動更新//////////////////////////////////////////////////////////////////////////////////////////////////
    2025-10-14 14:01:21
    処理結果：成功：全て処理に成功しました 差分あり SUCCESS_ALL
    ExecUser:[YYY]LogonUser:[YYY]LogonUserSID:[YYY]CurrentSID:[YYY]
    SettingType:File
    【成功】
    設定ファイルの保存に成功しました。
    ThinBridgeRuleUpdater	2025-10-14 14:01:21	設定データ取得開始
    ThinBridgeRuleUpdater	2025-10-14 14:01:21	1:OK 200 OK_SERVER C:\temp\senario5.ini
    ThinBridgeRuleUpdater	2025-10-14 14:01:21	>>>ExecMain
    ThinBridgeRuleUpdater	2025-10-14 14:01:21	>>>GetThinBridgeRedirectRuleCIFS C:\temp\senario5.ini
    ...
    ThinBridgeRuleUpdater	2025-10-14 14:01:21	<<<GetThinBridgeRedirectRuleCIFS OK_SERVER
    ThinBridgeRuleUpdater	2025-10-14 14:01:21	>>>ExecMain
    ThinBridgeRuleUpdater	2025-10-14 14:01:21	設定データ取得完了
    ThinBridgeRuleUpdater	2025-10-14 14:01:21	設定データ差分確認開始
    ThinBridgeRuleUpdater	2025-10-14 14:01:21	設定データ差分確認完了
    ...
    ```
  * [x] 待機時間が出力されていること
  * [x] 待機時間がピークアウト間隔以下の値であること
  * [x] 「アクティブなログオンユーザーで再実行します」が出力されていること
  * [x] 「設定データ取得開始」が出力されていること
  * [x] 「設定データ取得完了」が出力されていること
  * [x] 「設定データ差分確認開始」が出力されていること
  * [x] 「設定データ差分確認完了」が出力されていること
  * [x] 「GetThinBridgeRedirectRuleCIFS」が出力されていること
8. `C:\Program Files\ThinBridge\ThinBridgeBHO.ini`の内容を確認する
  * [x] `C:\Temp\senario5.ini`と同じ内容になっていること

### httpサーバーから取得

#### 準備

1. [PreReleaseVerification/senario5.ini](PreReleaseVerification/senario5.ini) を `C:\Temp\senario5.ini` に配置する。）
2. コマンドプロンプトを起動する
3. コマンドプロンプトで`.\tools\http-server`に移動する
4. `http_server.exe --root ..\..\PreReleaseVerification`を実行する
5. Webブラウザで`http://127.0.0.1:8080/`を開き、スーパーリロード（Ctrl+F5）をする。
6. PreReleaseVerificationフォルダの内容が表示されていることを確認する
7. リダイレクト定義自動更新設定（`C:\Program Files\ThinBridge\ThinBridgeRuleUpdaterSetting.exe`）を起動する
8. 「ThinBridgeリダイレクト定義ファイル取得先」に`http://127.0.0.1:8080/senario5.ini`を指定する
9. 「+」ボタンを押し、「ピークアウト間隔」に5分を指定する。
10. OKを押してダイアログを閉じる
11. 「接続＆取得データ テスト」ボタンを押し、接続テストを実行する
12. 取得が成功することを確認する
13. 「設定のみ保存」ボタンで設定を保存する

#### タスクスケジューラーによる実行の動作確認

※本テストの目的は確認は、データの取得にGetThinBridgeRedirectRuleが使われていることの確認なので、本1パターンのみで良い。

1. [PreReleaseVerification/senario1.ini](PreReleaseVerification/senario1.ini) を `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` に配置する。）
2. 端末のタスク スケジューラを起動する
3. 「タスク スケジューラ」->「タスク スケジューラ ライブラリ」を開く
5. タスク スケジューラによるリダイレクト定義更新タスクの実行（12:00といった時刻の分の部分が0のときに発火する）を待つか、手動実行する
6. ThinBridgeRuleUpdateTaskタスクの完了を待つ（適宜リスト表示の余白を右クリック->最新の状態に更新を実行する）
7. リダイレクト定義自動更新のログを開く（`C:\Program Files\ThinBridge\TBUpdateLog\ThinBridgeRuleUpdaterYYYY-MM-DD.log`、YYYY-MM-DDは日付。）
  * [x] 以下のようなログが出力されていること
    ```
    自動更新//////////////////////////////////////////////////////////////////////////////////////////////////
    2025-10-14 14:48:36 待機(0分)
    ExecUser:[XXX]LogonUser:[XXX]LogonUserSID:[XXX]CurrentSID:[S-1-5-18]
    アクティブなログオンユーザーで再実行します。
    自動更新//////////////////////////////////////////////////////////////////////////////////////////////////
    2025-10-14 14:49:11
    処理結果：成功：全て処理に成功しました 差分あり SUCCESS_ALL
    ExecUser:[XXX]LogonUser:[XXX]LogonUserSID:[XXX]CurrentSID:[XXX]
    SettingType:File
    【成功】
    設定ファイルの保存に成功しました。
    ThinBridgeRuleUpdater	2025-10-14 14:49:11	設定データ取得開始
    ThinBridgeRuleUpdater	2025-10-14 14:49:11	1:OK 200 OK_SERVER http://127.0.0.1:8080/senario5.ini
    ThinBridgeRuleUpdater	2025-10-14 14:49:11	>>>ExecMain
    ThinBridgeRuleUpdater	2025-10-14 14:49:11	>>>GetThinBridgeRedirectRule http://127.0.0.1:8080/senario5.ini
    ThinBridgeRuleUpdater	2025-10-14 14:49:11	>>>UserAgent ThinBridge PC:XXX ID:XXX
    ThinBridgeRuleUpdater	2025-10-14 14:49:11	GetThinBridgeRedirectRule dwStatusCode:200
    ...
    ThinBridgeRuleUpdater	2025-10-14 14:49:11	<<<GetThinBridgeRedirectRule OK_SERVER
    ThinBridgeRuleUpdater	2025-10-14 14:49:11	>>>ExecMain
    ThinBridgeRuleUpdater	2025-10-14 14:49:11	設定データ取得完了
    ThinBridgeRuleUpdater	2025-10-14 14:49:11	設定データ差分確認開始
    ThinBridgeRuleUpdater	2025-10-14 14:49:11	設定データ差分確認完了
    ...
    ```
  * [x] 待機時間が出力されていること
  * [x] 待機時間がピークアウト間隔以下の値であること
  * [x] 「アクティブなログオンユーザーで再実行します」が出力されていること
  * [x] 「設定データ取得開始」が出力されていること
  * [x] 「設定データ取得完了」が出力されていること
  * [x] 「設定データ差分確認開始」が出力されていること
  * [x] 「設定データ差分確認完了」が出力されていること
  * [x] 「GetThinBridgeRedirectRule」が出力されていること
8. `C:\Program Files\ThinBridge\ThinBridgeBHO.ini`の内容を確認する
  * [x] `C:\Temp\senario5.ini`と同じ内容になっていること