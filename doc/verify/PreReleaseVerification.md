# ThinBridge リリース前検証手順

## 検証環境の用意

検証には原則として `/doc/verify/windows-11-22H2` の環境を使用する。
他の環境を使用する場合は、以下の条件を事前に整えておく。

* Active Directoryドメイン参加状態である。
  （または、`/doc/verify/join-to-fake-domain.reg` の内容を管理者権限でインポート済みである。）
* Google Chrome、Microsoft Edgeをインストール済みである。
* Google Chrome、Microsoft EdgeのGPO用ポリシーテンプレートを導入済みである。
* `C:\Users\Public\webextensions` 配下に `/webextensions/` の内容を配置済みである。
* `C:\Users\Public\webextensions\manifest.xml` の位置に `/doc/verify/manifest.xml` と同等のファイルを配置済みである。

準備は以下の手順で行う。

1. https://github.com/ThinBridge/ThinBridge/releases/latest もしくは
   https://github.com/ThinBridge/ThinBridge/actions/workflows/build-release.yaml よりThinBridgeの最新のインストーラ `ThinBridgeSetup_x64.exe` をダウンロードし、実行、インストールする。
2. Chrome用アドオンの開発版パッケージを用意し、インストールするための設定を行う。
   1. Chromeを起動する。
   2. アドオンの管理画面（`chrome:extensions`）を開く。
   3. `デベロッパーモード` を有効化する。
   4. `拡張機能をパッケージ化` で `C:\Users\Public\webextensions¥chrome` をパックする。（1つ上のディレクトリーに `chrome.crx` と `chrome.pem` が作られる）
   5. `chrome.crx` をChromeのアドオン管理画面にドラッグ＆ドロップし、インストールして、IDを控える。
      例：`egoppdngdcoikeknmbgiakconmchahhf`
   6. アドオンを一旦アンインストールする。
   7. `C:\Program Files\ThinBridge\ThinBridgeHost\chrome.json` のアクセス権を変更し、ユーザー権限での書き込みを許可した上で、`"allowed_origins"` に先ほど控えたIDに基づくURLを追加する。
      例：`"chrome-extension://egoppdngdcoikeknmbgiakconmchahhf/"`
   8. `C:\Users\Public\webextensions¥manifest.xml` のChrome用アドオンのIDを、先程控えたIDで置き換える。
   9. `gpedit.msc` を起動する。
   10. `Computer Configuration\Administrative Templates\Google\Google Chrome\Extensions` （`コンピューターの構成¥管理用テンプレート\Google\Google Chrome\拡張機能`）を開いて、以下のポリシーを設定する。
       * `Configure the list of force-installed apps and extensions`（`自動インストールするアプリと拡張機能のリストの設定`）
         * `Enabled`（`有効`）に設定して、`Show...`（`表示...`）をクリックし、以下の項目を追加する。
           * `<先程控えたID>;file:///C:/Users/Public/webextensions/manifest.xml`
             例：`egoppdngdcoikeknmbgiakconmchahhf;file:///C:/Users/Public/webextensions/manifest.xml`
3. Edgeアドオンの開発版パッケージを用意し、インストールするための設定を行う。
   1. Edgeを起動する。
   2. アドオンの管理画面（`edge:extensions`）を開く。
   3. `開発者モード` を有効化する。
   4. `拡張機能のパック` で `C:\Users\Public\webextensions¥edge` をパックする。（1つ上のディレクトリーに `edge.crx` と `edge.pem` が作られる）
   5. `edge.crx` をEdgeのアドオン管理画面にドラッグ＆ドロップし、インストールして、IDを控える。
      例：`oapdkmbdgdcjpacbjpcdfhncifimimcj`
   6. アドオンを一旦アンインストールする。
   7. `C:\Program Files\ThinBridge\ThinBridgeHost\edge.json` のアクセス権を変更し、ユーザー権限での書き込みを許可した上で、`"allowed_origins"` に、先ほど控えたIDに基づくURLを追加する。
      例：`"chrome-extension://oapdkmbdgdcjpacbjpcdfhncifimimcj/"`
   8. `C:\Users\Public\webextensions¥manifest.xml` のEdge用アドオンのIDを、先程控えたIDで置き換える。
   9. `gpedit.msc` を起動する。
   10. `Computer Configuration\Administrative Templates\Microsoft Edge\Extensions`（`コンピューターの構成¥管理用テンプレート¥Microsoft Edge¥拡張機能`）を開いて、以下のポリシーを設定する。
       * `Configure the list of force-installed apps and extensions`（`サイレント インストールされる拡張機能を制御する`）
         * `Enabled`（`有効`）に設定して、`Show...`（`表示...`）をクリックし、以下の項目を追加する。
           * `<先程控えたID>;file:///C:/Users/Public/webextensions/manifest.xml`
             例：`oapdkmbdgdcjpacbjpcdfhncifimimcj;file:///C:/Users/Public/webextensions/manifest.xml`
4. BHOの管理のためのポリシー設定を行う。
   1. `gpedit.msc` を起動する。
   2. `Computer Configuration\Administrative Templates\Windows Components\Internet Explorer` （`コンピューターの構成¥管理用テンプレート¥Windows コンポーネント¥Internet Explorer`）を開いて、以下のポリシーを設定する。
      * `Turn off add-on performance notifications`（`アドオンのパフォーマンスの通知を無効にする`）：`Enabled`（`有効`）
      * `Automatically activate newly installed add-ons`（`新たにインストールされたアドオンを自動的にアクティブ化する`）：`Enabled`（`有効`）
   3. `Computer Configuration\Administrative Templates\Windows Components\Internet Explorer¥Internet Control Panel\Advance Page` （`コンピューターの構成¥管理用テンプレート¥Windows コンポーネント¥Internet Explorer¥インターネット コントロール パネル¥[詳細設定]ページ`）を開いて、以下のポリシーを設定する。
      * `Allow third-party browser extensions`（`サード パーティ製のブラウザー拡張を許可する`）：`Enabled`（`有効`）
   4. `Computer Configuration\Administrative Templates\Windows Components\Internet Explorer¥Security Features\Add-on Management` （`コンピューターの構成¥管理用テンプレート¥Windows コンポーネント¥Internet Explorer¥セキュリティの機能¥アドオン管理`）を開いて、以下のポリシーを設定する。
      * `Add-on List`（`アドオンの一覧`）
        * `Enabled`（`有効`）に設定して、`Add-on List`→`Show...`（`アドオンの一覧`→`表示...`）をクリックし、以下の項目を追加する。
          * `Value name`（`値の名前`）：`{3A56619B-37AC-40DA-833E-410F3BEDCBDC}`
          * `Value`（`値`）：`0`
5. EdgeのIEモードタブの設定を行う。
   1. Edgeを起動する。
   2. `edge://settings/defaultBrowser` を開く。
   3. `Allow sites to be reloaded in Internet Explorer mode (IE mode)` を `Allow` に設定する。
   4. 以下のURLを `Add` で追加する。
      * https://piro.sakura.ne.jp/apps/jspanel.html
   5. `edge://settings/system` を開く。
   6. `Startup boost` をオフにする。
   7. `Continue running background extensions and apps when Microsoft Edge is closed` をオフにする。
   8. Edgeを終了する。

## 検証

### BHO無効時のIEモードのタブの挙動を含む、Manifest V3での動作

#### 準備

以下の通り設定して検証を行う。

* `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` を以下の内容に設定する。
  ```
  [GLOBAL]
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:1
  @INTRANET_ZONE
  @TRUSTED_ZONE
  @UNTRUSTED_ZONE
  @RDP_APPMODE
  
  [Edge]
  @BROWSER_PATH:
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  http*://example.com
  http*://example.com/*
  http*://*.example.com
  http*://*.example.com/*
  http*://piro.sakura.ne.jp
  http*://piro.sakura.ne.jp/*
  http*://groonga.org/ja
  http*://groonga.org/ja/*
  
  [Chrome]
  @BROWSER_PATH:
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  @EXCLUDE_GROUP:Edge
  *
  
  [Default]
  @BROWSER_PATH:
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  ```
* `Computer Configuration\Administrative Templates\Windows Components\Internet Explorer¥Security Features\Add-on Management` （`コンピューターの構成¥管理用テンプレート¥Windows コンポーネント¥Internet Explorer¥セキュリティの機能¥アドオン管理`）を開いて、以下のポリシーを設定する。
  * `Add-on List`（`アドオンの一覧`）
    * `Add-on List`→`Show...`（`アドオンの一覧`→`表示...`）をクリックし、`{3A56619B-37AC-40DA-833E-410F3BEDCBDC}` の値を `0` に設定する。

#### 検証

1. Edgeを起動する。
2. リンクによるEdgeからChromeへのページ遷移の検証：
   1. Edge→Chromeの検証のため、Edgeで https://groonga.org/ja/ を開く。
      * 期待される結果：
        * Edge上で https://groonga.org/ja/ が読み込まれる。
   2. `English` のリンクをクリックする。
      * 期待される結果：
        * 空白のページが現在のタブに読み込まれ、すぐに元のページに戻る。
        * Chromeでタブが開かれ、https://groonga.org/ が読み込まれる。
   3. Edge（IEモード）→Chromeの検証のため、Edgeで https://piro.sakura.ne.jp/apps/jspanel.html を開く。
      * 期待される結果：
        * EdgeのタブがIEモードに切り替わる。
   4. 以下のスクリプトを貼り付けて実行する。
      ```
      location.href='https://groonga.org/ja/';
      ```
      * 期待される結果：
        * EdgeのIEモードのタブで https://groonga.org/ja/ が読み込まれる。
   5. `English` のリンクをクリックする。
      * 期待される結果：
        * EdgeのIEモードのタブで https://groonga.org/ が読み込まれる。（Manifest V2版と非互換の動作だが、仕様上不可避な挙動）
        * Chromeでタブが開かれ、https://groonga.org/ が読み込まれる。
3. JavaScriptによるEdgeからChromeへのページ遷移の検証：
   1. Edge→Chromeの検証のため、Edgeで https://piro.sakura.ne.jp/apps/jspanel を開く。
      * 期待される結果：
        * Edge上で https://piro.sakura.ne.jp/apps/jspanel が読み込まれる。
   2. 以下のスクリプトを貼り付けて実行する。
      ```
      window.open('https://example.net/', '_blank');
      ```
      * 期待される結果：
        * Chromeでタブが開かれ、https://example.net/ が読み込まれる。
        * 空白のタブがEdge上に残っていない。
   3. 以下のスクリプトを貼り付けて実行する。
      ```
      window.open('https://example.net/', '_blank', 'toolbar=no');
      ```
      * 期待される結果：
        * Chromeでタブが開かれ、https://example.net/ が読み込まれる。
        * 空白のポップアップウィンドウがEdge上に残っていない。
4. リンクによるChromeからEdgeへのページ遷移の検証：
   1. Chrome→Edgeの検証のため、Chromeで https://groonga.org/ を開く。
      * 期待される結果：
        * Chrome上で https://groonga.org/ が読み込まれる。
   2. `日本語` リンクをクリックする。
      * 期待される結果：
        * 空白のページが現在のタブに読み込まれ、すぐに元のページに戻る。
        * Edgeでタブが開かれ、https://groonga.org/ja/ が読み込まれる。
5. JavaScriptによるChromeからEdgeへのページ遷移の検証：
   1. Chrome→Edgeの検証のため、Chromeで https://www.piro.sakura.ne.jp/apps/jspanel.html を開く。
      * 期待される結果：
        * 証明書の警告が表示される。
        * 警告を無視して先に進むと、Chrome上で https://www.piro.sakura.ne.jp/apps/jspanel.html が読み込まれる。
   2. 以下のスクリプトを貼り付けて実行する。
      ```
      window.open('https://example.com/', '_blank');
      ```
      * 期待される結果：
        * Edgeでタブが開かれ、 https://example.com/ が読み込まれる。
        * 空白のタブがChrome上に残っていない。
   3. 以下のスクリプトを貼り付けて実行する。
      ```
      window.open('https://example.com/', '_blank', 'toolbar=no');
      ```
      * 期待される結果：
        * Edgeでタブが開かれ、https://example.com/ が読み込まれる。
        * 空白のポップアップウィンドウがChrome上に残っていない。
6. [!65](https://gitlab.com/clear-code/browserselector/-/merge_requests/65)の検証：
   1. Edgeで https://piro.sakura.ne.jp/apps/jspanel.html を開く。
      * 期待される結果：
        * タブがIEモードに切り替わる。
   2. 以下のスクリプトを貼り付けて実行する。
      ```
      window.open('https://example.net/', '_blank', 'toolbar=no');
      ```
      * 期待される結果：
        * IEモードのポップアップウィンドウが開かれ、 https://example.net/ が読み込まれる。
   3. そのまま1分から2分待つ。
      * 期待される結果：
        * IEモードのポップアップウィンドウが残っている。
        * Chromeでタブが開かれ、https://example.net/ が読み込まれる。
7. [!68](https://gitlab.com/clear-code/browserselector/-/merge_requests/68)の検証：
   1. Edgeで https://example.com/ を開く。
      * 期待される結果：
        * Edge上で https://example.com/ が読み込まれる。
   2. `More information...` のリンクをミドルクリックする。
      * 期待される結果：
        * Chrome上で https://www.iana.org/help/example-domains が開かれる。
        * 空白のタブがEdge上に残っていない。
   3. Chromeで https://www.piro.sakura.ne.jp/ を開く。
      * 期待される結果：
        * Chrome上で https://www.piro.sakura.ne.jp/ が読み込まれる。
   4. ページ最下部の `outsider reflex` のリンクをミドルクリックする。
      * 期待される結果：
        * Edge上で https://piro.sakura.ne.jp/ が開かれる。
        * 空白のタブがChrome上に残っていない。
8. Edge、Chromeを終了する。

### BHO有効時のIEモードのタブの挙動を含む、Manifest V3での動作

#### 準備

以下の通り設定して検証を行う。

* `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` を以下の内容に設定する。
  ```
  [GLOBAL]
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:1
  @INTRANET_ZONE
  @TRUSTED_ZONE
  @UNTRUSTED_ZONE
  @RDP_APPMODE
  
  [Edge]
  @BROWSER_PATH:
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  http*://example.com
  http*://example.com/*
  http*://*.example.com
  http*://*.example.com/*
  http*://piro.sakura.ne.jp
  http*://piro.sakura.ne.jp/*
  http*://groonga.org/ja
  http*://groonga.org/ja/*
  
  [Chrome]
  @BROWSER_PATH:
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  @EXCLUDE_GROUP:Edge
  *://*
  
  [Default]
  @BROWSER_PATH:
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  ```
* `Computer Configuration\Administrative Templates\Windows Components\Internet Explorer¥Security Features\Add-on Management` （`コンピューターの構成¥管理用テンプレート¥Windows コンポーネント¥Internet Explorer¥セキュリティの機能¥アドオン管理`）を開いて、以下のポリシーを設定する。
  * `Add-on List`（`アドオンの一覧`）
    * `Add-on List`→`Show...`（`アドオンの一覧`→`表示...`）をクリックし、`{3A56619B-37AC-40DA-833E-410F3BEDCBDC}` の値を `1` に設定する。

#### 検証

1. Edgeを起動する。
2. リンクによるEdgeからChromeへのページ遷移の検証：
   1. Edge（IEモード）→Chromeの検証のため、Edgeで https://piro.sakura.ne.jp/apps/jspanel.html を開く。
      * 期待される結果：
        * EdgeのタブがIEモードに切り替わる。
   2. 以下のスクリプトを貼り付けて実行する。
      ```
      location.href='https://groonga.org/ja/';
      ```
      * 期待される結果：
        * EdgeのIEモードのタブで https://groonga.org/ja/ が読み込まれる。
   3. `English` のリンクをクリックする。
      * 期待される結果：
        * EdgeのIEモードのタブがページ遷移しないでそのまま維持される。
        * Chromeでタブが開かれ、https://groonga.org/ が読み込まれる。
2. [!65](https://gitlab.com/clear-code/browserselector/-/merge_requests/65)の検証：
   1. Edgeで https://piro.sakura.ne.jp/apps/jspanel.html を開く。
      * 期待される結果：
        * タブがIEモードに切り替わる。
   2. 以下のスクリプトを貼り付けて実行する。
      ```
      window.open('https://example.net/', '_blank', 'toolbar=no');
      ```
      * 期待される結果：
        * Chromeでタブが開かれ、https://example.net/ が読み込まれる。
        * IEモードのポップアップウィンドウが残っていない。
3. Edge、Chromeを終了する。



