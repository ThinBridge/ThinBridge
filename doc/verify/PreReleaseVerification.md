# ThinBridge リリース前検証手順

## 検証環境の用意

検証には原則として `/doc/verify` 配下のいずれかの環境（推奨：`/doc/verify/windows-11-22H2`）を使用する。
`/doc/verify` 配下の物ではない他のWindows環境を使用する場合は、以下の条件を事前に整えておく。

* Active Directoryドメイン参加状態である。
  （または、`/doc/verify/join-to-fake-domain.reg` の内容を管理者権限でインポート済みである。）
* Google Chrome、Microsoft Edgeをインストール済みである。
* Google Chrome、Microsoft EdgeのGPO用ポリシーテンプレートを導入済みである。
* `C:\Users\Public\webextensions` 配下に `/webextensions/` の内容を配置済みである。
* `C:\Users\Public\webextensions\manifest.xml` の位置に `/doc/verify/manifest.xml` と同等のファイルを配置済みである。

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
   11. Chromeを再起動し、アドオンの管理画面（`chrome://extensions`）を開いて、BrowserSelectorの開発版が管理者によってインストールされた状態になっていることを確認する。
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
   11. Edgeを再起動し、アドオンの管理画面（`edge://extensions`）を開いて、BrowserSelectorの開発版が管理者によってインストールされた状態になっていることを確認する。
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
   3. `Allow sites to be reloaded in Internet Explorer mode (IE mode)` を `Allow` に設定する。
   4. `edge://settings/system` を開く。
   5. `Startup boost` をオフにする。
   6. `Continue running background extensions and apps when Microsoft Edge is closed` をオフにする。
   7. Edgeを終了する。

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
6. EdgeのIEモードのタブから開かれたポップアップウィンドウからのページ遷移の検証：
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
        * Chromeでタブが開かれない。
7. リダイレクト対象のURLを新規タブで開いた際の挙動の検証：
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
  *
  
  [Default]
  @BROWSER_PATH:
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  ```
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
2. EdgeのIEモードのタブから開かれたポップアップウィンドウからのページ遷移の検証：
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


### EdgeのIEモード境界をまたぐページ遷移における設定無効時の動作

#### 準備

以下の通り設定して検証を行う。

* `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` を以下の内容に設定する。
  ```
  [GLOBAL]
  @DISABLED
  @TOP_PAGE_ONLY
  @INTRANET_ZONE
  @TRUSTED_ZONE
  @UNTRUSTED_ZONE
  @CTX_APPNAME:
  @RDP_APPMODE
  
  [RDP]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [VMWARE]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CITRIX]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [Firefox]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [Chrome]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [Edge]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [IE]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [Default]
  @BROWSER_PATH:
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM01]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM02]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM03]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM04]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM05]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM18]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM19]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM20]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  ```
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
        * Edgeの通常タブで https://example.com/ が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
   2. `More information...` のリンクをクリックする。
      * 期待される結果：
        * Edgeの通常タブで https://www.iana.org/help/example-domains が開かれる。
        * ThinBridgeによるリダイレクトが発生しない。
   3. Edge（通常モード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://www.iana.org/help/example-domains
   4. Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * Edgeの通常タブで https://example.com/ が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
   5. `More information...` のリンクをクリックする。
      * 期待される結果：
        * タブがIEモードに切り替わり、 https://www.iana.org/help/example-domains が開かれる。
        * ThinBridgeによるリダイレクトが発生しない。
   6. Edge（IEモード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： https://www.iana.org/help/example-domains
      * 追加： https://example.com/
   7. Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * EdgeのIEモードのタブで https://example.com/ が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
   8. アドレスバーに https://www.iana.org/help/example-domains を入力し、Enterする。
      * 期待される結果：
        * タブが通常モードに切り替わり、 https://www.iana.org/help/example-domains が開かれる。
        * ThinBridgeによるリダイレクトが発生しない。
   9. Edge（IEモード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://www.iana.org/help/example-domains
   7. Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * EdgeのIEモードのタブで https://example.com/ が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
   8. アドレスバーに https://www.iana.org/help/example-domains を入力し、Enterする。
      * 期待される結果：
        * EdgeのIEモードのタブで https://www.iana.org/help/example-domains が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
3. Edgeを終了する。


### EdgeのIEモード境界をまたぐページ遷移におけるDefault設定の動作

#### 準備

以下の通り設定して検証を行う。

* `C:\Program Files\ThinBridge\ThinBridgeBHO.ini` を以下の内容に設定する。
  ```
  [GLOBAL]
  @DISABLED
  @TOP_PAGE_ONLY
  @INTRANET_ZONE
  @TRUSTED_ZONE
  @UNTRUSTED_ZONE
  @CTX_APPNAME:仮想ブラウザー
  @RDP_APPMODE
  
  [RDP]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [VMWARE]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CITRIX]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [Firefox]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [Chrome]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [Edge]
  @BROWSER_PATH:
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  *://example.com*
  *://www.iana.org*
  *://groonga.org*
  
  [IE]
  @BROWSER_PATH:
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  *://example.com*
  *://www.iana.org*
  *://groonga.org*
  *://mroonga.org*
  
  [Default]
  @BROWSER_PATH:CITRIX
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM01]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM02]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM03]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM04]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM05]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM18]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM19]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  
  [CUSTOM20]
  @BROWSER_PATH:
  @DISABLED
  @TOP_PAGE_ONLY
  @REDIRECT_PAGE_ACTION:0
  @CLOSE_TIMEOUT:3
  ```
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
        * Edgeの通常タブで https://example.com/ が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
   2. `More information...` のリンクをクリックする。
      * 期待される結果：
        * Edgeの通常タブで https://www.iana.org/help/example-domains が開かれる。
        * ThinBridgeによるリダイレクトが発生しない。
   3. Edge（通常モード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://www.iana.org/help/example-domains
   4. Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * Edgeの通常タブで https://example.com/ が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
   5. `More information...` のリンクをクリックする。
      * 期待される結果：
        * タブがIEモードに切り替わり、 https://www.iana.org/help/example-domains が開かれる。
        * ThinBridgeによるリダイレクトが発生しない。
   6. Edge（IEモード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： https://www.iana.org/help/example-domains
      * 追加： https://example.com/
   7. Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * EdgeのIEモードのタブで https://example.com/ が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
   8. アドレスバーに https://www.iana.org/help/example-domains を入力し、Enterする。
      * 期待される結果：
        * タブが通常モードに切り替わり、 https://www.iana.org/help/example-domains が開かれる。
        * ThinBridgeによるリダイレクトが発生しない。
   9. Edge（IEモード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://www.iana.org/help/example-domains
   7. Edgeで新しいタブで https://example.com/ を開く。
      * 期待される結果：
        * EdgeのIEモードのタブで https://example.com/ が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
   8. `More information...` のリンクをクリックする。
      * 期待される結果：
        * EdgeのIEモードのタブで https://www.iana.org/help/example-domains が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
3. IEのみに該当するURLの動作の検証：
   1. Edge（通常モード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： 全項目
   2. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
   3. `Mroonga` のリンクをクリックする。
      * 期待される結果：
        * Edgeの通常タブで https://mroonga.org/ が開かれる。
        * ThinBridgeによるリダイレクトが発生しない。
   4. Edge（通常モード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://mroonga.org/
   5. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
   6. `Mroonga` のリンクをクリックする。
      * 期待される結果：
        * タブがIEモードに切り替わらず、読み込みが中断される。
        * ThinBridgeによるリダイレクトが発生し、 https://mroonga.org/ がIEで開かれるか、IEの起動を試みている旨を示すダイアログが表示される。
   7. Edge（IEモード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： https://mroonga.org/
      * 追加： https://groonga.org/related-projects.html
   8. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
   9. アドレスバーに https://mroonga.org/ を入力し、Enterする。
      * 期待される結果：
        * EdgeのタブがThinBridgeにより閉じられる。
        * ThinBridgeによるリダイレクトが発生し、 https://mroonga.org/ がIEで開かれるか、IEの起動を試みている旨を示すダイアログが表示される。
   10. Edge（IEモード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
       * 追加： https://mroonga.org/
   11. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
       * 期待される結果：
         * EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
         * ThinBridgeによるリダイレクトが発生しない。
   12. `Mroonga` のリンクをクリックする。
       * 期待される結果：
          * Edgeのタブでの読み込みが中断される。
          * ThinBridgeによるリダイレクトが発生し、 https://mroonga.org/ がIEで開かれるか、IEの起動を試みている旨を示すダイアログが表示される。
4. いずれのブラウザーにも該当しないURLの動作の検証：
   1. Edge（通常モード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： 全項目
   2. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
   3. `PGroonga` のリンクをクリックする。
      * 期待される結果：
        * Edgeのタブでの読み込みが中断される。
        * ThinBridgeによるリダイレクトが発生し、「Citrixがインストールされていないか、設定されていないため起動できません。」というメッセージが表示される。
   4. Edge（通常モード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 追加： https://pgroonga.github.io/
   5. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * Edgeの通常タブで https://groonga.org/related-projects.html が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
   6. `PGroonga` のリンクをクリックする。
      * 期待される結果：
        * Edgeのタブでの読み込みが中断される。
        * ThinBridgeによるリダイレクトが発生し、「Citrixがインストールされていないか、設定されていないため起動できません。」というメッセージが表示される。
   7. Edge（IEモード）→Edge（通常モード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
      * 削除： https://pgroonga.github.io/
      * 追加： https://groonga.org/related-projects.html
   8. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
      * 期待される結果：
        * EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
        * ThinBridgeによるリダイレクトが発生しない。
   9. アドレスバーに https://pgroonga.github.io/ を入力し、Enterする。
      * 期待される結果：
        * Edgeのタブでの読み込みが中断される。
        * ThinBridgeによるリダイレクトが発生し、「Citrixがインストールされていないか、設定されていないため起動できません。」というメッセージが表示される。
   10. Edge（IEモード）→Edge（IEモード）の検証のため、`edge://settings/defaultBrowser` を開き、IEモードの対象URLを以下の通り設定する。
       * 追加： https://pgroonga.github.io/
   11. Edgeで新しいタブで https://groonga.org/related-projects.html を開く。
       * 期待される結果：
         * EdgeのIEモードのタブで https://groonga.org/related-projects.html が読み込まれる。
         * ThinBridgeによるリダイレクトが発生しない。
   12. `PGroonga` のリンクをクリックする。
       * 期待される結果：
         * Edgeのタブでの読み込みが中断される。
         * ThinBridgeによるリダイレクトが発生し、「Citrixがインストールされていないか、設定されていないため起動できません。」というメッセージが表示される。
5. Edgeを終了する。


