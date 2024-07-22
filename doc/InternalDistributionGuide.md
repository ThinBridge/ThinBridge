---
CJKmainfont: Noto Sans CJK JP
CJKoptions:
  - BoldFont=Noto Sans CJK JP Bold
title: "ThinBridge拡張機能 組織内サーバーを用いた配布・更新手順"
author: "株式会社クリアコード"
date: "2024-07-22"
titlepage: true
colorlinks: true
toc-title: "目次"
toc-own-page: true
listings-disable-line-numbers: true
code-block-font-size: \footnotesize
footnotes-pretty: true
---

# 目的

本手順書は、Active Directoryドメイン参加端末を対象として、組織内サーバーを使用してMicrosoft Edge用およびGoogle Chrome用ThinBridge拡張機能（以下、ThinBridge拡張機能）を配布する手順をまとめたものです。

# 概要

ThinBridge拡張機能は、ADドメイン参加端末に対してグループポリシー（GPO）で導入のための設定を行い、設定を認識した各端末のEdgeおよびChrome（以下、ブラウザー）が、ThinBridge拡張機能のファイルをダウンロードしてくることによりインストールされます。

通常、GPOを用いた拡張機能のインストール時には、拡張機能のファイルはMicrosoftのWebストアからインストールされます。この場合、ブラウザーおよびWebストアの仕様により、インストールされる拡張機能は常にWebストア上の最新バージョンとなり、それ以外の任意のバージョンを使用することはできません。

本手順書では、Webストアからではなく組織内サーバーからThinBridge拡張機能の更新情報を取得するようにGPOを設定することにより、当該サーバーに設置した任意のバージョンのThinBridge拡張機能を、ADドメイン参加端末へ新規または更新の形で一斉インストールさせる手順を説明します。


# 前提

本項目の作業手順は以下の作業環境で実施することを想定します。

* 作業者：システム管理者 1名
* 作業環境：Active Directoryドメインコントローラーを操作可能なWindows端末 1台
* 適用時に必要なサーバー：全端末からアクセス可能なファイル配布用サーバー 1台
  * Windowsファイル共有サーバー、もしくはWWWサーバーが必要です。
  * 本サーバーはドメインコントローラーとの兼用可です。



# ThinBridge拡張機能の組織内サーバーからのインストール

## 一部の検証対象端末を対象としたインストール（事前検証）

全体展開前の検証のために、ADドメイン参加端末のうち、情報システム部門のみの端末など、一部の端末のみを対象として、ThinBridge拡張機能を組織内サーバーで配布する形で新規に導入する場合、もしくは、組織内サーバーでの配布を行っていなかった状態から組織内サーバーでの配布へ切り替える場合について、手順の大まかな流れは以下の通りです。

1. GPOによるThinBridge拡張機能の新規インストール
2. GPOによる`edge.json`および`chrome.json`の書き換え
3. 端末でThinBridge拡張機能が新規インストールされたことの確認

以下に具体的な作業手順を記します。

1. **GPOによるThinBridge拡張機能の新規インストール**  
   組織内サーバーに設置したThinBridge拡張機能をインストールするため、グループポリシーの設定を変更します。  
   以下の作業はすべて、作業環境にて、システム管理者が管理者ユーザーアカウントで実施します。
   1. 全端末からアクセス可能なファイル配布用サーバー上に、一般ユーザー権限で読み取り可能な、ファイル配布用フォルダーを作成します。  
      以下、コンピューター名/ホスト名が「fileserver」であるWindowsファイル共有サーバーを使用し、ファイル配布用フォルダー名は「thinbridge-test」を使用するものと仮定します。  
      この仮定に従い、ファイル共有サーバー上に作成された共有フォルダーのUNCパスが  
      `\\fileserver\thinbridge-test\`  
      となると仮定します。
   2. 組織内配布用の各ブラウザー用crxファイル（`ThinBridgeEdge-vX.X.X.crx`、`ThinBridgeChrome-vX.X.X.crx`）、組織内配布用の各ブラウザー用Native Manifestファイル（`edge.json`、`chrome.json`）、組織内配布用の更新情報ファイル（`manifest.xml`）をダウンロードします。  
      バージョン2.2.1をインストールすると仮定すると、使用するcrxファイルは
      
      * `ThinBridgeEdge-v2.2.1.crx`
      * `ThinBridgeChrome-v2.2.1.crx`
      
      となります。
   3. 1.2で取得した5つのファイルを、1.1で作成したファイル配布用フォルダーに設置します。  
      前述の仮定に従い、各ファイルのUNCパスは
      
      * `\\fileserver\thinbridge-test\ThinBridgeEdge-v2.2.1.crx`
      * `\\fileserver\thinbridge-test\ThinBridgeChrome-v2.2.1.crx`
      * `\\fileserver\thinbridge-test\edge.json`
      * `\\fileserver\thinbridge-test\chrome.json`
      * `\\fileserver\thinbridge-test\manifest.xml`
      
      となると仮定します。
   4. 1.3で設置した`manifest.xml`を「メモ帳」もしくは何らかのテキスト編集ツールで開き、ファイルが以下のような内容であることを確認します。
      
      ```xml
      <?xml version='1.0' encoding='UTF-8'?>
      <gupdate xmlns='http://www.google.com/update2/response' protocol='2.0'>
        <app appid='jcamehnjflombcdhafhiogbojgghefec'>
          <updatecheck
            codebase='（ThinBridgeEdge-vX.X.X.crxのURL）'
            version='（バージョン番号）' />
        </app>
        <app appid='XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'>
          <updatecheck
            codebase='（ThinBridgeChrome-vX.X.X.crxのURL）'
            version='（バージョン番号）' />
        </app>
      </gupdate>
      ```
   5. 1.4で開いたファイルの`codebase='`から`'`までの間の箇所を、1.3で設置した実際のcrxファイルのURLで置き換えます。  
      また、`version ='`から`'`までの間の箇所を、インストールするThinBridge拡張機能のバージョンで置き換えます。  
      前述の仮定に従うと、ファイルのURLはUNCパスに基づいて
      
      * `file://fileserver/thinbridge-test/ThinBridgeEdge-v2.2.1.crx`
      * `file://fileserver/thinbridge-test/ThinBridgeChrome-v2.2.1.crx`
      
      となり、ThinBridge拡張機能バージョン2.2.1をインストールする場合の編集後の`manifest.xml`の内容は以下の要領となります。
      
      ```xml
      <?xml version='1.0' encoding='UTF-8'?>
      <gupdate xmlns='http://www.google.com/update2/response' protocol='2.0'>
        <app appid='jcamehnjflombcdhafhiogbojgghefec'>
          <updatecheck
            codebase='file://fileserver/thinbridge-test/ThinBridgeEdge-v2.2.1.crx'
            version='2.2.1' />
        </app>
        <app appid='XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'>
          <updatecheck
            codebase='file://fileserver/thinbridge-test/ThinBridgeChrome-v2.2.1.crx'
            version='2.2.1' />
        </app>
      </gupdate>
      ```
   6. 編集後の`manifest.xml`を上書き保存します。
   7. 配置した各ファイルの「プロパティ」を開き、「セキュリティ」タブを選択して、当該ファイルが「Everyone」で読み取り可能な状態になっていることを確認します。  
      もしそのようになっていない場合は、以下の手順でファイルを「Everyone」で読み取り可能な状態に設定します。
      
      1. 「編集」ボタンをクリックします。
      2. 開かれたダイアログ内で「追加」ボタンをクリックします。
      3. 開かれたダイアログ内で「選択するオブジェクト名を入力してください」欄に「Everyone」と入力します。
      4. 「OK」ボタンを押してダイアログを閉じる操作を3回繰り返します。
   8. 検証対象の端末に一般ユーザーでログインし、1.3で配置した5つのファイルを読み取れることを確認します。
      
      * `manifest.xml`のURL（前述の仮定に従うと`file://fileserver/thinbridge-test/manifest.xml`）をEdgeで開き、1.6で設定した通りの内容を読み取れることを確認します。
      * Edge用crxのURL（前述の仮定に従うと`file://fileserver/thinbridge-test/ThinBridgeEdge-v2.2.1.crx`）をEdgeで開き、ファイルがダウンロード可能であることを確認します。
      * Chrome用crxのURL（前述の仮定に従うと`file://fileserver/thinbridge-test/ThinBridgeChrome-v2.2.1.crx`）をChromeで開き、ファイルがダウンロード可能であることを確認します。
      * `edge.json`、`chrome.json`をメモ帳などで開き、内容を閲覧できることを確認します。
   9. グループポリシー管理コンソールを起動します。
   10. 検証対象の端末の一般ユーザーに適用するためのGPOを作成、もしくは検証対象の端末の一般ユーザーに適用されるGPOを用意します。  
       ここでは例として、「ThinBridge拡張機能（事前検証）」という名称のGPOを使用するものとします。
   11. 1.10で用意したGPOについて、  
       「管理用テンプレート」  
       →「Microsoft Edge」  
       →「拡張機能」  
       →「サイレント インストールされる拡張機能を制御する」  
       をダブルクリックして、当該ポリシーの設定画面を開きます。
   12. 設定の状態を「有効」に設定します。
   13. 「表示...」をクリックして、値の設定画面を開きます。
   14. 以下の項目が存在する場合、項目を削除します。
       
       * `jcamehnjflombcdhafhiogbojgghefec`
       * `famoofbkcpjdkihdngnhgbdfkfenhcnf`
   15. 以下の項目を追加します。
       
       * `jcamehnjflombcdhafhiogbojgghefec;（manifest.xmlのURL）`
       
       前述の仮定に従うと、追加する項目は以下の要領となります。
       
       * `jcamehnjflombcdhafhiogbojgghefec;file://fileserver/thinbridge-test/manifest.xml`
   16. 「OK」ボタンを押して、値の設定画面を閉じます。
   17. 「OK」ボタンを押して、ポリシーの設定画面を閉じます。
   18. 1.10で用意したGPOについて、  
       「管理用テンプレート」  
       →「Google」  
       →「Coogle Chrome」  
       →「拡張機能」  
       →「自動インストールするアプリと拡張機能のリストの設定」  
       をダブルクリックして、当該ポリシーの設定画面を開きます。
   19. 設定の状態を「有効」に設定します。
   20. 「表示...」をクリックして、値の設定画面を開きます。
   21. 以下の項目が存在する場合、項目を削除します。
       
       * `XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX`
       * `iiajmhibpjkpmfmbhegccdfmfnfeffmh`
   22. 以下の項目を追加します。
       
       * `XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX;（manifest.xmlのURL）`
       
       前述の仮定に従うと、追加する項目は以下の要領となります。
       
       * `XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX;file://fileserver/thinbridge-test/manifest.xml`
   23. 「OK」ボタンを押して、値の設定画面を閉じます。
   24. 「OK」ボタンを押して、ポリシーの設定画面を閉じます。
2. **GPOによる`edge.json`および`chrome.json`の書き換え**  
   組織内サーバーから配布されたThinBridge拡張機能によるThinBridge本体の呼び出し要求を許可するため、`edge.json`および`chrome.json`の配布のための設定を行います。  
   以下の作業はすべて、作業環境にて、システム管理者が管理者ユーザーアカウントで実施します。
   1. グループポリシー管理コンソールで、検証対象の端末の一般ユーザーに適用されるGPO（「ThinBridge拡張機能（事前検証）」と仮定）について、  
      「ユーザーの構成」  
      →「基本設定」  
      →「Windowsの設定」  
      →「ファイル」  
      を右クリックし「新規作成」から「ファイル」を選択します。
   2. 以下の通り設定します。
      
      * アクション：「置換」を選択。
      * ソースファイル：1.3で配置した`edge.json`を、全端末から参照可能なUNCパスで指定。（例：`\\fileserver\thinbridge-test\edge.json`）
      * ターゲットファイル：`C:\Program Files\ThinBridge\ThinBridgeHost\edge.json`を指定。
      * 属性：「読み取り専用」のみにチェック。
   3. 続いて、  
      「ユーザーの構成」  
      →「基本設定」  
      →「Windowsの設定」  
      →「ファイル」  
      を右クリックし「新規作成」から「ファイル」を選択します。
   2. 以下の通り設定します。
      
      * アクション：「置換」を選択。
      * ソースファイル：1.3で配置した`chrome.json`を、全端末から参照可能なUNCパスで指定。（例：`\\fileserver\thinbridge-test\chrome.json`）
      * ターゲットファイル：`C:\Program Files\ThinBridge\ThinBridgeHost\chrome.json`を指定。
      * 属性：「読み取り専用」のみにチェック。
3. **端末でThinBridge拡張機能がインストールされたことの確認**  
   端末の強制再起動などを行い、グループポリシーが検証対象の端末に反映された状態として下さい。
   
   以下にグループポリシーの反映を確認する手順を示します。  
   以下の作業はすべて、ADドメイン参加状態の検証対象の端末にて、システム管理者または一般ユーザーが一般ユーザーアカウントで実施します。
   
   1. `C:\Program Files\ThinBridge\ThinBridgeHost\`配下の`edge.json`をメモ帳で開き、前項で配布するよう設定したファイルの通りの内容になっている（ファイルが配信されている）ことを確認します。
   2. Edgeを起動します。
   3. 数秒待ち、Edgeのツールバー上にパズルピース型のボタンが表示されることを確認します。
   4. Edgeのツールバー上にパズルピース型のボタンをクリックし、「拡張機能の管理」をクリックします。
   5. 拡張機能の管理画面が開かれますので、「インストール済の拡張機能」の一覧に以下の項目が存在する事を確認します。
      
      * ThinBridge
   6. 項目の詳細情報を表示し、バージョンが組織内サーバーで配布しているバージョンと一致していることを確認します。
   7. `C:\Program Files\ThinBridge\ThinBridgeHost\`配下の`chrome.json`をメモ帳で開き、前項で配布するよう設定したファイルの通りの内容になっている（ファイルが配信されている）ことを確認します。
   8. Chromeを起動します。
   9. 数秒待ち、Chromeのツールバー上にパズルピース型のボタンが表示されることを確認します。
   10. Chromeのツールバー上にパズルピース型のボタンをクリックし、「拡張機能を管理」をクリックします。
   11. 拡張機能の管理画面が開かれますので、「すべての拡張機能」の一覧に以下の項目が存在する事を確認します。
      
       * ThinBridge
   12. 項目の詳細情報を表示し、バージョンが組織内サーバーで配布しているバージョンと一致していることを確認します。

以上の手順により、組織内サーバーに設置したバージョンのThinBridge拡張機能が、検証対象の端末にインストールされます。

## 全端末を対象としたインストール（本番展開）

ADドメイン参加端末全体を対象として、ThinBridge拡張機能を組織内サーバーで配布する形で新規に導入する場合、もしくは、組織内サーバーでの配布を行っていなかった状態から組織内サーバーでの配布へ切り替える場合について、手順の大まかな流れは以下の通りです。

1. GPOによるThinBridge拡張機能の新規インストール
2. GPOによる`edge.json`および`chrome.json`の書き換え
3. 端末でThinBridge拡張機能が新規インストールされたことの確認

以下に、具体的な作業手順を記します。

1. **GPOによるThinBridge拡張機能の新規インストール**  
   組織内サーバーに設置したThinBridge拡張機能をインストールするため、グループポリシーの設定を変更します。  
   以下の作業はすべて、作業環境にて、システム管理者が管理者ユーザーアカウントで実施します。
   1. 全端末からアクセス可能なファイル配布用サーバー上に、一般ユーザー権限で読み取り可能な、事前検証用とは別のファイル配布用フォルダーを作成します。  
      以下、コンピューター名/ホスト名が「fileserver」であるWindowsファイル共有サーバーを使用し、ファイル配布用フォルダー名は「thinbridge」を使用するものと仮定します。  
      この仮定に従い、ファイル共有サーバー上に作成された共有フォルダーのUNCパスが  
      `\\fileserver\thinbridge\`  
      となると仮定します。
   2. 組織内配布用の各ブラウザー用crxファイル（`ThinBridgeEdge-vX.X.X.crx`、`ThinBridgeChrome-vX.X.X.crx`）、組織内配布用の各ブラウザー用Native Manifestファイル（`edge.json`、`chrome.json`）、組織内配布用の更新情報ファイル（`manifest.xml`）をダウンロードします。  
      バージョン2.2.1をインストールすると仮定すると、使用するcrxファイルは
      
      * `ThinBridgeEdge-v2.2.1.crx`
      * `ThinBridgeChrome-v2.2.1.crx`
      
      となります。
   3. 1.2で取得した5つのファイルを、1.1で作成したファイル配布用フォルダーに設置します。  
      前述の仮定に従い、各ファイルのUNCパスは
      
      * `\\fileserver\thinbridge\ThinBridgeEdge-v2.2.1.crx`
      * `\\fileserver\thinbridge\ThinBridgeChrome-v2.2.1.crx`
      * `\\fileserver\thinbridge\edge.json`
      * `\\fileserver\thinbridge\chrome.json`
      * `\\fileserver\thinbridge\manifest.xml`
      
      となると仮定します。
   4. 1.3で設置した`manifest.xml`を「メモ帳」もしくは何らかのテキスト編集ツールで開き、ファイルが以下のような内容であることを確認します。
      
      ```xml
      <?xml version='1.0' encoding='UTF-8'?>
      <gupdate xmlns='http://www.google.com/update2/response' protocol='2.0'>
        <app appid='jcamehnjflombcdhafhiogbojgghefec'>
          <updatecheck
            codebase='（ThinBridgeEdge-vX.X.X.crxのURL）'
            version='（バージョン番号）' />
        </app>
        <app appid='XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'>
          <updatecheck
            codebase='（ThinBridgeChrome-vX.X.X.crxのURL）'
            version='（バージョン番号）' />
        </app>
      </gupdate>
      ```
   5. 1.4で開いたファイルの`codebase='`から`'`までの間の箇所を、1.3で設置した実際のcrxファイルのURLで置き換えます。  
      また、`version ='`から`'`までの間の箇所を、インストールするThinBridge拡張機能のバージョンで置き換えます。  
      前述の仮定に従うと、ファイルのURLはUNCパスに基づいて
      
      * `file://fileserver/thinbridge/ThinBridgeEdge-v2.2.1.crx`
      * `file://fileserver/thinbridge/ThinBridgeChrome-v2.2.1.crx`
      
      となり、ThinBridge拡張機能バージョン2.2.1をインストールする場合の編集後の`manifest.xml`の内容は以下の要領となります。
      
      ```xml
      <?xml version='1.0' encoding='UTF-8'?>
      <gupdate xmlns='http://www.google.com/update2/response' protocol='2.0'>
        <app appid='jcamehnjflombcdhafhiogbojgghefec'>
          <updatecheck
            codebase='file://fileserver/thinbridge/ThinBridgeEdge-v2.2.1.crx'
            version='2.2.1' />
        </app>
        <app appid='XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'>
          <updatecheck
            codebase='file://fileserver/thinbridge/ThinBridgeChrome-v2.2.1.crx'
            version='2.2.1' />
        </app>
      </gupdate>
      ```
   6. 編集後の`manifest.xml`を上書き保存します。
   7. 配置した各ファイルの「プロパティ」を開き、「セキュリティ」タブを選択して、当該ファイルが「Everyone」で読み取り可能な状態になっていることを確認します。  
      もしそのようになっていない場合は、以下の手順でファイルを「Everyone」で読み取り可能な状態に設定します。
      
      1. 「編集」ボタンをクリックします。
      2. 開かれたダイアログ内で「追加」ボタンをクリックします。
      3. 開かれたダイアログ内で「選択するオブジェクト名を入力してください」欄に「Everyone」と入力します。
      4. 「OK」ボタンを押してダイアログを閉じる操作を3回繰り返します。
   8. 検証対象の端末に一般ユーザーでログインし、1.3で配置した5つのファイルを読み取れることを確認します。
      
      * `manifest.xml`のURL（前述の仮定に従うと`file://fileserver/thinbridge/manifest.xml`）をEdgeで開き、1.6で設定した通りの内容を読み取れることを確認します。
      * Edge用crxのURL（前述の仮定に従うと`file://fileserver/thinbridge/ThinBridgeEdge-v2.2.1.crx`）をEdgeで開き、ファイルがダウンロード可能であることを確認します。
      * Chrome用crxのURL（前述の仮定に従うと`file://fileserver/thinbridge/ThinBridgeChrome-v2.2.1.crx`）をChromeで開き、ファイルがダウンロード可能であることを確認します。
      * `edge.json`、`chrome.json`をメモ帳などで開き、内容を閲覧できることを確認します。
   9. グループポリシー管理コンソールを起動します。
   10. 全端末の一般ユーザーに適用するためのGPOを作成、もしくは全端末の一般ユーザーに適用されるGPOを用意します。  
       ここでは例として、「ThinBridge拡張機能」という名称のGPOを使用するものとします。
   11. 1.10で用意したGPOについて、  
       「管理用テンプレート」  
       →「Microsoft Edge」  
       →「拡張機能」  
       →「サイレント インストールされる拡張機能を制御する」  
       をダブルクリックして、当該ポリシーの設定画面を開きます。
   12. 設定の状態を「有効」に設定します。
   13. 「表示...」をクリックして、値の設定画面を開きます。
   14. 以下の項目が存在する場合、項目を削除します。
       
       * `jcamehnjflombcdhafhiogbojgghefec`
       * `famoofbkcpjdkihdngnhgbdfkfenhcnf`
   15. 以下の項目を追加します。
       
       * `jcamehnjflombcdhafhiogbojgghefec;（manifest.xmlのURL）`
       
       前述の仮定に従うと、追加する項目は以下の要領となります。
       
       * `jcamehnjflombcdhafhiogbojgghefec;file://fileserver/thinbridge/manifest.xml`
   16. 「OK」ボタンを押して、値の設定画面を閉じます。
   17. 「OK」ボタンを押して、ポリシーの設定画面を閉じます。
   18. 1.10で用意したGPOについて、  
       「管理用テンプレート」  
       →「Google」  
       →「Coogle Chrome」  
       →「拡張機能」  
       →「自動インストールするアプリと拡張機能のリストの設定」  
       をダブルクリックして、当該ポリシーの設定画面を開きます。
   19. 設定の状態を「有効」に設定します。
   20. 「表示...」をクリックして、値の設定画面を開きます。
   21. 以下の項目が存在する場合、項目を削除します。
       
       * `XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX`
       * `iiajmhibpjkpmfmbhegccdfmfnfeffmh`
   22. 以下の項目を追加します。
       
       * `XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX;（manifest.xmlのURL）`
       
       前述の仮定に従うと、追加する項目は以下の要領となります。
       
       * `XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX;file://fileserver/thinbridge/manifest.xml`
   23. 「OK」ボタンを押して、値の設定画面を閉じます。
   24. 「OK」ボタンを押して、ポリシーの設定画面を閉じます。
2. **GPOによる`edge.json`および`chrome.json`の書き換え**  
   組織内サーバーから配布されたThinBridge拡張機能によるThinBridge本体の呼び出し要求を許可するため、`edge.json`および`chrome.json`の配布のための設定を行います。  
   以下の作業はすべて、作業環境にて、システム管理者が管理者ユーザーアカウントで実施します。
   1. グループポリシー管理コンソールで、全端末の一般ユーザーに適用されるGPO（「ThinBridge拡張機能」と仮定）について、  
      「ユーザーの構成」  
      →「基本設定」  
      →「Windowsの設定」  
      →「ファイル」  
      を右クリックし「新規作成」から「ファイル」を選択します。
   2. 以下の通り設定します。
      
      * アクション：「置換」を選択。
      * ソースファイル：1.3で配置した`edge.json`を、全端末から参照可能なUNCパスで指定。（例：`\\fileserver\thinbridge\edge.json`）
      * ターゲットファイル：`C:\Program Files\ThinBridge\ThinBridgeHost\edge.json`を指定。
      * 属性：「読み取り専用」のみにチェック。
   3. 続いて、  
      「ユーザーの構成」  
      →「基本設定」  
      →「Windowsの設定」  
      →「ファイル」  
      を右クリックし「新規作成」から「ファイル」を選択します。
   2. 以下の通り設定します。
      
      * アクション：「置換」を選択。
      * ソースファイル：1.3で配置した`chrome.json`を、全端末から参照可能なUNCパスで指定。（例：`\\fileserver\thinbridge\chrome.json`）
      * ターゲットファイル：`C:\Program Files\ThinBridge\ThinBridgeHost\chrome.json`を指定。
      * 属性：「読み取り専用」のみにチェック。
3. **端末でThinBridge拡張機能がインストールされたことの確認**  
   端末の強制再起動などを行い、グループポリシーが検証対象の端末に反映された状態として下さい。
   
   以下にグループポリシーの反映を確認する手順を示します。  
   以下の作業はすべて、任意のADドメイン参加端末にて、システム管理者または一般ユーザーが一般ユーザーアカウントで実施します。  
   全台で確認することは不可能ですので、何台か抽出して確認されることを推奨します。
   
   1. `C:\Program Files\ThinBridge\ThinBridgeHost\`配下の`edge.json`をメモ帳で開き、前項で配布するよう設定したファイルの通りの内容になっている（ファイルが配信されている）ことを確認します。
   2. Edgeを起動します。
   3. 数秒待ち、Edgeのツールバー上にパズルピース型のボタンが表示されることを確認します。
   4. Edgeのツールバー上にパズルピース型のボタンをクリックし、「拡張機能の管理」をクリックします。
   5. 拡張機能の管理画面が開かれますので、「インストール済の拡張機能」の一覧に以下の項目が存在する事を確認します。
      
      * ThinBridge
   6. 項目の詳細情報を表示し、バージョンが組織内サーバーで配布しているバージョンと一致していることを確認します。
   7. `C:\Program Files\ThinBridge\ThinBridgeHost\`配下の`chrome.json`をメモ帳で開き、前項で配布するよう設定したファイルの通りの内容になっている（ファイルが配信されている）ことを確認します。
   8. Chromeを起動します。
   9. 数秒待ち、Chromeのツールバー上にパズルピース型のボタンが表示されることを確認します。
   10. Chromeのツールバー上にパズルピース型のボタンをクリックし、「拡張機能を管理」をクリックします。
   11. 拡張機能の管理画面が開かれますので、「すべての拡張機能」の一覧に以下の項目が存在する事を確認します。
      
       * ThinBridge
   12. 項目の詳細情報を表示し、バージョンが組織内サーバーで配布しているバージョンと一致していることを確認します。

以上の手順により、組織内サーバーに設置したバージョンのThinBridge拡張機能が、各端末に導入されます。


# ThinBridge拡張機能の組織内サーバーからの更新

## 一部の検証対象端末を対象とした更新（事前検証）

全体展開前の検証のために、ADドメイン参加端末のうち、情報システム部門のみの端末など、一部の端末のみを対象として、ThinBridge拡張機能を組織内サーバーで配布する形で新バージョンへ更新する場合について、手順の大まかな流れは以下の通りです。

1. GPOによるThinBridge拡張機能の更新
2. 端末でThinBridge拡張機能が更新されたことの確認

**ThinBridge拡張機能の組織内サーバーからのインストール時に、事前検証と本番展開で同一のファイル配布用フォルダーを使用していた場合には、以下に記す手順では、検証対象の一部の端末のみを対象として更新を実施することができません。その場合、「ThinBridge拡張機能の組織内サーバーからのインストール」の「一部の検証対象端末を対象としたインストール（事前検証）」に記載の手順に則り、検証対象の端末について、事前検証用のファイル配布用フォルダーに設置した`manifest.xml`およびcrxファイルが使用される状態にあらかじめ変更しておいて下さい。**

以下に、検証対象の端末用に事前検証用のファイル配布用フォルダーが使われている状態を前提として、具体的な作業手順を記します。

1. **GPOによるThinBridge拡張機能の更新**  
   組織内サーバーに更新先バージョンのThinBridge拡張機能のファイルを設置します。  
   以下の作業はすべて、作業環境にて、システム管理者が管理者ユーザーアカウントで実施します。
   1. ファイル配布サーバー上の検証対象の端末用のファイル配布用フォルダーを開きます。  
      「ThinBridge拡張機能の組織内サーバーからのインストール」での仮定に従い、コンピューター名/ホスト名が「fileserver」であるWindowsファイル共有サーバーを使用し、ファイル配布用フォルダー名は「thinbridge-test」を使用するものと仮定します。
   2. 新バージョンの、組織内配布用の各ブラウザー用crxファイル（`ThinBridgeEdge-vX.X.X.crx`、`ThinBridgeChrome-vX.X.X.crx`）をダウンロードします。  
      バージョン2.3.0へ更新すると仮定すると、使用するcrxファイルは
      
      * `ThinBridgeEdge-v2.3.0.crx`
      * `ThinBridgeChrome-v2.3.0.crx`
      
      となります。
   3. 新バージョンのcrxファイルを、1.1で開いたファイル配布用フォルダーに設置します。  
      前述の仮定に従い、各ファイルのUNCパスは
      
      * `\\fileserver\thinbridge-test\ThinBridgeEdge-v2.3.0.crx`
      * `\\fileserver\thinbridge-test\ThinBridgeChrome-v2.3.0.crx`
      
      となると仮定します。
   4. ファイル配布用フォルダー内の`manifest.xml`を「メモ帳」もしくは何らかのテキスト編集ツールで開き、ファイルが以下のような内容であることを確認します。
      
      ```xml
      <?xml version='1.0' encoding='UTF-8'?>
      <gupdate xmlns='http://www.google.com/update2/response' protocol='2.0'>
        <app appid='jcamehnjflombcdhafhiogbojgghefec'>
          <updatecheck
            codebase='（ThinBridgeEdge-vX.X.X.crxのURL）'
            version='（バージョン番号）' />
        </app>
        <app appid='XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'>
          <updatecheck
            codebase='（ThinBridgeChrome-vX.X.X.crxのURL）'
            version='（バージョン番号）' />
        </app>
      </gupdate>
      ```
   5. 1.4で開いたファイルの`codebase='`から`'`までの間の箇所を、1.3で設置した実際のcrxファイルのURLで置き換えます。  
      また、`version ='`から`'`までの間の箇所を、更新先のThinBridge拡張機能のバージョンで置き換えます。  
      前述の仮定に従うと、ファイルのURLはUNCパスに基づいて
      
      * `file://fileserver/thinbridge-test/ThinBridgeEdge-v2.3.0.crx`
      * `file://fileserver/thinbridge-test/ThinBridgeChrome-v2.3.0.crx`
      
      となり、ThinBridge拡張機能バージョン2.3.0へ更新する場合の編集後の`manifest.xml`の内容は以下の要領となります。
      
      ```xml
      <?xml version='1.0' encoding='UTF-8'?>
      <gupdate xmlns='http://www.google.com/update2/response' protocol='2.0'>
        <app appid='jcamehnjflombcdhafhiogbojgghefec'>
          <updatecheck
            codebase='file://fileserver/thinbridge-test/ThinBridgeEdge-v2.3.0.crx'
            version='2.3.0' />
        </app>
        <app appid='XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'>
          <updatecheck
            codebase='file://fileserver/thinbridge-test/ThinBridgeChrome-v2.3.0.crx'
            version='2.3.0' />
        </app>
      </gupdate>
      ```
   6. 編集後の`manifest.xml`を上書き保存します。
   7. 1.3で配置した各ファイルおよび`manifest.xml`の「プロパティ」を開き、「セキュリティ」タブを選択して、当該ファイルが「Everyone」で読み取り可能な状態になっていることを確認します。  
      もしそのようになっていない場合は、以下の手順でファイルを「Everyone」で読み取り可能な状態に設定します。
      
      1. 「編集」ボタンをクリックします。
      2. 開かれたダイアログ内で「追加」ボタンをクリックします。
      3. 開かれたダイアログ内で「選択するオブジェクト名を入力してください」欄に「Everyone」と入力します。
      4. 「OK」ボタンを押してダイアログを閉じる操作を3回繰り返します。
   8. 検証対象の端末に一般ユーザーでログインし、ファイル配布用フォルダー内のファイルを読み取れることを確認します。
      
      * `manifest.xml`のURL（前述の仮定に従うと`file://fileserver/thinbridge-test/manifest.xml`）をEdgeで開き、1.5で設定した通りの内容を読み取れることを確認します。
      * Edge用crxのURL（前述の仮定に従うと`file://fileserver/thinbridge-test/ThinBridgeEdge-v2.3.0.crx`）をEdgeで開き、ファイルがダウンロード可能であることを確認します。
      * Chrome用crxのURL（前述の仮定に従うと`file://fileserver/thinbridge-test/ThinBridgeChrome-v2.3.0.crx`）をChromeで開き、ファイルがダウンロード可能であることを確認します。
2. **端末でThinBridge拡張機能が更新されることの確認**  
   以下の作業はすべて、ADドメイン参加状態の検証対象の端末にて、システム管理者または一般ユーザーが一般ユーザーアカウントで実施します。
   1. Edgeを起動します。
   2. Edgeのツールバー上にパズルピース型のボタンをクリックし、「拡張機能の管理」をクリックします。
   3. 拡張機能の管理画面が開かれますので、「開発者モード」を有効化します。
   4. 「インストール済の拡張機能」の見出し横に表示される「更新」ボタンをクリックします。
   5. 読み込み中を示すアニメーションが表示されますので、アニメーションが消えて「拡張機能が更新されました」というメッセージが表示されるまで待ちます。
   6. 「インストール済の拡張機能」の一覧にある「ThinBridge」の詳細情報を表示し、バージョンが更新後のバージョンと一致していることを確認します。
   7. 「開発者モード」を無効化します。
   8. Chromeを起動します。
   9. Chromeのツールバー上にパズルピース型のボタンをクリックし、「拡張機能の管理」をクリックします。
   10. 拡張機能の管理画面が開かれますので、「デベロッパー モード」を有効化します。
   11. ページ上部に表示される「更新」ボタンをクリックします。
   12. ページ下部に「更新しています...」というメッセージが表示されますので、メッセージが消えるまで待ちます。
   13. 「すべての拡張機能」の一覧にある「ThinBridge」の詳細情報を表示し、バージョンが更新後のバージョンと一致していることを確認します。
   14. 「デベロッパー モード」を無効化します。

以上の手順により、組織内サーバーに設置したバージョンのThinBridge拡張機能が、検証対象の端末に反映されます。


## 全端末を対象とした更新（本番展開）

ADドメイン参加端末全体を対象として、ThinBridge拡張機能を組織内サーバーで配布する形で新バージョンへ更新する場合について、手順の大まかな流れは以下の通りです。

1. GPOによるThinBridge拡張機能の更新
2. 端末でThinBridge拡張機能が更新されたことの確認

以下に、具体的な作業手順を記します。

1. **GPOによるThinBridge拡張機能の更新**  
   組織内サーバーに更新先バージョンのThinBridge拡張機能のファイルを設置します。  
   以下の作業はすべて、作業環境にて、システム管理者が管理者ユーザーアカウントで実施します。
   1. ファイル配布サーバー上の全体展開用のファイル配布用フォルダーを開きます。  
      「ThinBridge拡張機能の組織内サーバーからのインストール」での仮定に従い、コンピューター名/ホスト名が「fileserver」であるWindowsファイル共有サーバーを使用し、ファイル配布用フォルダー名は「thinbridge」を使用するものと仮定します。
   2. 新バージョンの、組織内配布用の各ブラウザー用crxファイル（`ThinBridgeEdge-vX.X.X.crx`、`ThinBridgeChrome-vX.X.X.crx`）をダウンロードします。  
      バージョン2.3.0へ更新すると仮定すると、使用するcrxファイルは
      
      * `ThinBridgeEdge-v2.3.0.crx`
      * `ThinBridgeChrome-v2.3.0.crx`
      
      となります。
   3. 新バージョンのcrxファイルを、1.1で開いたファイル配布用フォルダーに設置します。  
      前述の仮定に従い、各ファイルのUNCパスは
      
      * `\\fileserver\thinbridge\ThinBridgeEdge-v2.3.0.crx`
      * `\\fileserver\thinbridge\ThinBridgeChrome-v2.3.0.crx`
      
      となると仮定します。
   4. ファイル配布用フォルダー内の`manifest.xml`を「メモ帳」もしくは何らかのテキスト編集ツールで開き、ファイルが以下のような内容であることを確認します。
      
      ```xml
      <?xml version='1.0' encoding='UTF-8'?>
      <gupdate xmlns='http://www.google.com/update2/response' protocol='2.0'>
        <app appid='jcamehnjflombcdhafhiogbojgghefec'>
          <updatecheck
            codebase='（ThinBridgeEdge-vX.X.X.crxのURL）'
            version='（バージョン番号）' />
        </app>
        <app appid='XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'>
          <updatecheck
            codebase='（ThinBridgeChrome-vX.X.X.crxのURL）'
            version='（バージョン番号）' />
        </app>
      </gupdate>
      ```
   5. 1.4で開いたファイルの`codebase='`から`'`までの間の箇所を、1.3で設置した実際のcrxファイルのURLで置き換えます。  
      また、`version ='`から`'`までの間の箇所を、更新先のThinBridge拡張機能のバージョンで置き換えます。  
      前述の仮定に従うと、ファイルのURLはUNCパスに基づいて
      
      * `file://fileserver/thinbridge/ThinBridgeEdge-v2.3.0.crx`
      * `file://fileserver/thinbridge/ThinBridgeChrome-v2.3.0.crx`
      
      となり、ThinBridge拡張機能バージョン2.3.0へ更新する場合の編集後の`manifest.xml`の内容は以下の要領となります。
      
      ```xml
      <?xml version='1.0' encoding='UTF-8'?>
      <gupdate xmlns='http://www.google.com/update2/response' protocol='2.0'>
        <app appid='jcamehnjflombcdhafhiogbojgghefec'>
          <updatecheck
            codebase='file://fileserver/thinbridge/ThinBridgeEdge-v2.3.0.crx'
            version='2.3.0' />
        </app>
        <app appid='XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'>
          <updatecheck
            codebase='file://fileserver/thinbridge/ThinBridgeChrome-v2.3.0.crx'
            version='2.3.0' />
        </app>
      </gupdate>
      ```
   6. 編集後の`manifest.xml`を上書き保存します。
   7. 1.3で配置した各ファイルおよび`manifest.xml`の「プロパティ」を開き、「セキュリティ」タブを選択して、当該ファイルが「Everyone」で読み取り可能な状態になっていることを確認します。  
      もしそのようになっていない場合は、以下の手順でファイルを「Everyone」で読み取り可能な状態に設定します。
      
      1. 「編集」ボタンをクリックします。
      2. 開かれたダイアログ内で「追加」ボタンをクリックします。
      3. 開かれたダイアログ内で「選択するオブジェクト名を入力してください」欄に「Everyone」と入力します。
      4. 「OK」ボタンを押してダイアログを閉じる操作を3回繰り返します。
   8. ADドメイン参加状態の任意の端末に一般ユーザーでログインし、ファイル配布用フォルダー内のファイルを読み取れることを確認します。
      
      * `manifest.xml`のURL（前述の仮定に従うと`file://fileserver/thinbridge/manifest.xml`）をEdgeで開き、1.5で設定した通りの内容を読み取れることを確認します。
      * Edge用crxのURL（前述の仮定に従うと`file://fileserver/thinbridge/ThinBridgeEdge-v2.3.0.crx`）をEdgeで開き、ファイルがダウンロード可能であることを確認します。
      * Chrome用crxのURL（前述の仮定に従うと`file://fileserver/thinbridge/ThinBridgeChrome-v2.3.0.crx`）をChromeで開き、ファイルがダウンロード可能であることを確認します。

2. **端末でThinBridge拡張機能が更新されることの確認**  
   以下の作業はすべて、ADドメイン参加状態の検証対象の端末にて、システム管理者または一般ユーザーが一般ユーザーアカウントで実施します。
   1. Edgeを起動します。
   2. Edgeのツールバー上にパズルピース型のボタンをクリックし、「拡張機能の管理」をクリックします。
   3. 拡張機能の管理画面が開かれますので、「開発者モード」を有効化します。
   4. 「インストール済の拡張機能」の見出し横に表示される「更新」ボタンをクリックします。
   5. 読み込み中を示すアニメーションが表示されますので、アニメーションが消えて「拡張機能が更新されました」というメッセージが表示されるまで待ちます。
   6. 「インストール済の拡張機能」の一覧にある「ThinBridge」の詳細情報を表示し、バージョンが更新後のバージョンと一致していることを確認します。
   7. 「開発者モード」を無効化します。
   8. Chromeを起動します。
   9. Chromeのツールバー上にパズルピース型のボタンをクリックし、「拡張機能の管理」をクリックします。
   10. 拡張機能の管理画面が開かれますので、「デベロッパー モード」を有効化します。
   11. ページ上部に表示される「更新」ボタンをクリックします。
   12. ページ下部に「更新しています...」というメッセージが表示されますので、メッセージが消えるまで待ちます。
   13. 「すべての拡張機能」の一覧にある「ThinBridge」の詳細情報を表示し、バージョンが更新後のバージョンと一致していることを確認します。
   14. 「デベロッパー モード」を無効化します。

以上の手順により、組織内サーバーに設置したバージョンのThinBridge拡張機能が、各端末に反映されます。



# ThinBridge拡張機能の組織内サーバーからの切り戻し

## 一部の検証対象端末を対象とした切り戻し（事前検証）

全体展開前の検証のために、ADドメイン参加端末のうち、情報システム部門のみの端末など、一部の端末のみを対象として、ThinBridge拡張機能を組織内サーバーで配布する形で旧バージョンへ切り戻す場合について、手順の大まかな流れは以下の通りです。

1. GPOによるThinBridge拡張機能の切り戻し
2. 端末でThinBridge拡張機能が切り戻されたことの確認

**ThinBridge拡張機能の組織内サーバーからのインストール時に、事前検証と本番展開で同一のファイル配布用フォルダーを使用していた場合には、以下に記す手順では、検証対象の一部の端末のみを対象として切り戻しを実施することができません。その場合、「ThinBridge拡張機能の組織内サーバーからのインストール」の「一部の検証対象端末を対象としたインストール（事前検証）」に記載の手順に則り、検証対象の端末について、事前検証用のファイル配布用フォルダーに設置した`manifest.xml`およびcrxファイルが使用される状態にあらかじめ変更しておいて下さい。**

以下に、検証対象の端末用に事前検証用のファイル配布用フォルダーが使われている状態を前提として、具体的な作業手順を記します。

1. **ThinBridge拡張機能のファイルの切り戻し**  
   組織内サーバーに設置したThinBridge拡張機能のファイルを、切り戻し対象のバージョンに差し替えます。  
   以下の作業はすべて、作業環境にて、システム管理者が管理者ユーザーアカウントで実施します。
   1. ファイル配布サーバー上の検証対象の端末用のファイル配布用フォルダーを開きます。  
      「ThinBridge拡張機能の組織内サーバーからのインストール」での仮定に従い、コンピューター名/ホスト名が「fileserver」であるWindowsファイル共有サーバーを使用し、ファイル配布用フォルダー名は「thinbridge-test」を使用するものと仮定します。
   2. 切り戻し先となる旧バージョンの、組織内配布用の各ブラウザー用crxファイル（`ThinBridgeEdge-vX.X.X.crx`、`ThinBridgeChrome-vX.X.X.crx`）をダウンロードします。  
      バージョン2.2.1へ切り戻すと仮定すると、使用するcrxファイルは
      
      * `ThinBridgeEdge-v2.2.1.crx`
      * `ThinBridgeChrome-v2.2.1.crx`
      
      となります。
   3. 旧バージョンのcrxファイルを、1.1で開いたファイル配布用フォルダーに設置します。  
      前述の仮定に従い、各ファイルのUNCパスは
      
      * `\\fileserver\thinbridge-test\ThinBridgeEdge-v2.2.1.crx`
      * `\\fileserver\thinbridge-test\ThinBridgeChrome-v2.2.1.crx`
      
      となると仮定します。
   4. ファイル配布用フォルダー内の`manifest.xml`を「メモ帳」もしくは何らかのテキスト編集ツールで開き、ファイルが以下のような内容であることを確認します。
      
      ```xml
      <?xml version='1.0' encoding='UTF-8'?>
      <gupdate xmlns='http://www.google.com/update2/response' protocol='2.0'>
        <app appid='jcamehnjflombcdhafhiogbojgghefec'>
          <updatecheck
            codebase='（ThinBridgeEdge-vX.X.X.crxのURL）'
            version='（バージョン番号）' />
        </app>
        <app appid='XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'>
          <updatecheck
            codebase='（ThinBridgeChrome-vX.X.X.crxのURL）'
            version='（バージョン番号）' />
        </app>
      </gupdate>
      ```
   5. 1.4で開いたファイルの`codebase='`から`'`までの間の箇所を、1.3で設置した実際のcrxファイルのURLで置き換えます。  
      また、`version ='`から`'`までの間の箇所を、切り戻し先のThinBridge拡張機能のバージョンで置き換えます。  
      前述の仮定に従うと、ファイルのURLはUNCパスに基づいて
      
      * `file://fileserver/thinbridge-test/ThinBridgeEdge-v2.2.1.crx`
      * `file://fileserver/thinbridge-test/ThinBridgeChrome-v2.2.1.crx`
      
      となり、ThinBridge拡張機能バージョン2.2.1へ切り戻す場合の編集後の`manifest.xml`の内容は以下の要領となります。
      
      ```xml
      <?xml version='1.0' encoding='UTF-8'?>
      <gupdate xmlns='http://www.google.com/update2/response' protocol='2.0'>
        <app appid='jcamehnjflombcdhafhiogbojgghefec'>
          <updatecheck
            codebase='file://fileserver/thinbridge-test/ThinBridgeEdge-v2.2.1.crx'
            version='2.2.1' />
        </app>
        <app appid='XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'>
          <updatecheck
            codebase='file://fileserver/thinbridge-test/ThinBridgeChrome-v2.2.1.crx'
            version='2.2.1' />
        </app>
      </gupdate>
      ```
   6. 編集後の`manifest.xml`を上書き保存します。
   7. 1.3で配置した各ファイルおよび`manifest.xml`の「プロパティ」を開き、「セキュリティ」タブを選択して、当該ファイルが「Everyone」で読み取り可能な状態になっていることを確認します。  
      もしそのようになっていない場合は、以下の手順でファイルを「Everyone」で読み取り可能な状態に設定します。
      
      1. 「編集」ボタンをクリックします。
      2. 開かれたダイアログ内で「追加」ボタンをクリックします。
      3. 開かれたダイアログ内で「選択するオブジェクト名を入力してください」欄に「Everyone」と入力します。
      4. 「OK」ボタンを押してダイアログを閉じる操作を3回繰り返します。
   8. 検証対象の端末に一般ユーザーでログインし、ファイル配布用フォルダー内のファイルを読み取れることを確認します。
      
      * `manifest.xml`のURL（前述の仮定に従うと`file://fileserver/thinbridge-test/manifest.xml`）をEdgeで開き、1.5で設定した通りの内容を読み取れることを確認します。
      * Edge用crxのURL（前述の仮定に従うと`file://fileserver/thinbridge-test/ThinBridgeEdge-v2.2.1.crx`）をEdgeで開き、ファイルがダウンロード可能であることを確認します。
      * Chrome用crxのURL（前述の仮定に従うと`file://fileserver/thinbridge-test/ThinBridgeChrome-v2.2.1.crx`）をChromeで開き、ファイルがダウンロード可能であることを確認します。
2. **端末でThinBridge拡張機能が切り戻されることの確認**  
   以下の作業はすべて、ADドメイン参加状態の検証対象の端末にて、システム管理者または一般ユーザーが一般ユーザーアカウントで実施します。
   1. Edgeを起動します。
   2. Edgeのツールバー上にパズルピース型のボタンをクリックし、「拡張機能の管理」をクリックします。
   3. 拡張機能の管理画面が開かれますので、「開発者モード」を有効化します。
   4. 「インストール済の拡張機能」の見出し横に表示される「更新」ボタンをクリックします。
   5. 読み込み中を示すアニメーションが表示されますので、アニメーションが消えて「拡張機能が更新されました」というメッセージが表示されるまで待ちます。
   6. 「インストール済の拡張機能」の一覧にある「ThinBridge」の詳細情報を表示し、バージョンが切り戻し後のバージョンと一致していることを確認します。
   7. 「開発者モード」を無効化します。
   8. Chromeを起動します。
   9. Chromeのツールバー上にパズルピース型のボタンをクリックし、「拡張機能の管理」をクリックします。
   10. 拡張機能の管理画面が開かれますので、「デベロッパー モード」を有効化します。
   11. ページ上部に表示される「更新」ボタンをクリックします。
   12. ページ下部に「更新しています...」というメッセージが表示されますので、メッセージが消えるまで待ちます。
   13. 「すべての拡張機能」の一覧にある「ThinBridge」の詳細情報を表示し、バージョンが切り戻し後のバージョンと一致していることを確認します。
   14. 「デベロッパー モード」を無効化します。

以上の手順により、組織内サーバーに設置したバージョンのThinBridge拡張機能が、検証対象の端末に反映されます。


## 全端末を対象とした切り戻し（本番展開）

ADドメイン参加端末全体を対象として、ThinBridge拡張機能を組織内サーバーで配布する形で旧バージョンへ切り戻す場合について、手順の大まかな流れは以下の通りです。

1. GPOによるThinBridge拡張機能の切り戻し
2. 端末でThinBridge拡張機能が切り戻されたことの確認

以下に、具体的な作業手順を記します。

1. **ThinBridge拡張機能のファイルの切り戻し**  
   組織内サーバーに設置したThinBridge拡張機能のファイルを切り戻し対象のバージョンへ差し替えます。
   以下の作業はすべて、作業環境にて、システム管理者が管理者ユーザーアカウントで実施します。
   1. ファイル配布サーバー上の全体展開用のファイル配布用フォルダーを開きます。  
      「ThinBridge拡張機能の組織内サーバーからのインストール」での仮定に従い、コンピューター名/ホスト名が「fileserver」であるWindowsファイル共有サーバーを使用し、ファイル配布用フォルダー名は「thinbridge」を使用するものと仮定します。
   2. 切り戻し先となる旧バージョンの、組織内配布用の各ブラウザー用crxファイル（`ThinBridgeEdge-vX.X.X.crx`、`ThinBridgeChrome-vX.X.X.crx`）をダウンロードします。  
      バージョン2.2.1へ切り戻すと仮定すると、使用するcrxファイルは
      
      * `ThinBridgeEdge-v2.2.1.crx`
      * `ThinBridgeChrome-v2.2.1.crx`
      
      となります。
   3. 旧バージョンのcrxファイルを、1.1で開いたファイル配布用フォルダーに設置します。  
      前述の仮定に従い、各ファイルのUNCパスは
      
      * `\\fileserver\thinbridge\ThinBridgeEdge-v2.2.1.crx`
      * `\\fileserver\thinbridge\ThinBridgeChrome-v2.2.1.crx`
      
      となると仮定します。
   4. ファイル配布用フォルダー内の`manifest.xml`を「メモ帳」もしくは何らかのテキスト編集ツールで開き、ファイルが以下のような内容であることを確認します。
      
      ```xml
      <?xml version='1.0' encoding='UTF-8'?>
      <gupdate xmlns='http://www.google.com/update2/response' protocol='2.0'>
        <app appid='jcamehnjflombcdhafhiogbojgghefec'>
          <updatecheck
            codebase='（ThinBridgeEdge-vX.X.X.crxのURL）'
            version='（バージョン番号）' />
        </app>
        <app appid='XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'>
          <updatecheck
            codebase='（ThinBridgeChrome-vX.X.X.crxのURL）'
            version='（バージョン番号）' />
        </app>
      </gupdate>
      ```
   5. 1.4で開いたファイルの`codebase='`から`'`までの間の箇所を、1.3で設置した実際のcrxファイルのURLで置き換えます。  
      また、`version ='`から`'`までの間の箇所を、切り戻し先のThinBridge拡張機能のバージョンで置き換えます。  
      前述の仮定に従うと、ファイルのURLはUNCパスに基づいて
      
      * `file://fileserver/thinbridge/ThinBridgeEdge-v2.2.1.crx`
      * `file://fileserver/thinbridge/ThinBridgeChrome-v2.2.1.crx`
      
      となり、ThinBridge拡張機能バージョン2.2.1へ切り戻す場合の編集後の`manifest.xml`の内容は以下の要領となります。
      
      ```xml
      <?xml version='1.0' encoding='UTF-8'?>
      <gupdate xmlns='http://www.google.com/update2/response' protocol='2.0'>
        <app appid='jcamehnjflombcdhafhiogbojgghefec'>
          <updatecheck
            codebase='file://fileserver/thinbridge/ThinBridgeEdge-v2.2.1.crx'
            version='2.2.1' />
        </app>
        <app appid='XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'>
          <updatecheck
            codebase='file://fileserver/thinbridge/ThinBridgeChrome-v2.2.1.crx'
            version='2.2.1' />
        </app>
      </gupdate>
      ```
   6. 編集後の「manifest.xml」を上書き保存します。
   7. 1.3で配置した各ファイルおよび`manifest.xml`の「プロパティ」を開き、「セキュリティ」タブを選択して、当該ファイルが「Everyone」で読み取り可能な状態になっていることを確認します。  
      もしそのようになっていない場合は、以下の手順でファイルを「Everyone」で読み取り可能な状態に設定します。
      
      1. 「編集」ボタンをクリックします。
      2. 開かれたダイアログ内で「追加」ボタンをクリックします。
      3. 開かれたダイアログ内で「選択するオブジェクト名を入力してください」欄に「Everyone」と入力します。
      4. 「OK」ボタンを押してダイアログを閉じる操作を3回繰り返します。
   8. ADドメイン参加状態の任意の端末に一般ユーザーでログインし、ファイル配布用フォルダー内のファイルを読み取れることを確認します。
      
      * `manifest.xml`のURL（前述の仮定に従うと`file://fileserver/thinbridge/manifest.xml`）をEdgeで開き、1.5で設定した通りの内容を読み取れることを確認します。
      * Edge用crxのURL（前述の仮定に従うと`file://fileserver/thinbridge/ThinBridgeEdge-v2.2.1.crx`）をEdgeで開き、ファイルがダウンロード可能であることを確認します。
      * Chrome用crxのURL（前述の仮定に従うと`file://fileserver/thinbridge/ThinBridgeChrome-v2.2.1.crx`）をChromeで開き、ファイルがダウンロード可能であることを確認します。
2. **端末でThinBridge拡張機能が切り戻されることの確認**  
   以下の作業はすべて、ADドメイン参加状態の任意の端末にて、システム管理者または一般ユーザーが一般ユーザーアカウントで実施します。
   1. Edgeを起動します。
   2. Edgeのツールバー上にパズルピース型のボタンをクリックし、「拡張機能の管理」をクリックします。
   3. 拡張機能の管理画面が開かれますので、「開発者モード」を有効化します。
   4. 「インストール済の拡張機能」の見出し横に表示される「更新」ボタンをクリックします。
   5. 読み込み中を示すアニメーションが表示されますので、アニメーションが消えて「拡張機能が更新されました」というメッセージが表示されるまで待ちます。
   6. 「インストール済の拡張機能」の一覧にある「ThinBridge」の詳細情報を表示し、バージョンが切り戻し後のバージョンと一致していることを確認します。
   7. 「開発者モード」を無効化します。
   8. Chromeを起動します。
   9. Chromeのツールバー上にパズルピース型のボタンをクリックし、「拡張機能の管理」をクリックします。
   10. 拡張機能の管理画面が開かれますので、「デベロッパー モード」を有効化します。
   11. ページ上部に表示される「更新」ボタンをクリックします。
   12. ページ下部に「更新しています...」というメッセージが表示されますので、メッセージが消えるまで待ちます。
   13. 「すべての拡張機能」の一覧にある「ThinBridge」の詳細情報を表示し、バージョンが切り戻し後のバージョンと一致していることを確認します。
   14. 「デベロッパー モード」を無効化します。

以上の手順により、組織内サーバーに設置したバージョンのThinBridge拡張機能が、各端末に反映されます。

