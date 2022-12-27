# Related issue

https://github.com/ThinBridge/ThinBridge/issues/18

# Steps to verify

## Preparation

1. Log into the verification environment.
2. Quit all running IE and Chrome.
3. Install ThinBridge with the installer.
4. Start `cmd.exe`.

## Internet Explorer

1. Run a command line: `"C:\Program Files (x86)\Internet Explorer\iexplore.exe" about:blank`
   * This should starts IE with a blank tab.
2. Type and enter a URL `https://powerbi.microsoft.com/ja-jp/` to IE's address bar.
   * This should load the page with IE.
   * Chrome should not be started.
3. Quit IE.
4. Run a command line: `"C:\Program Files\ThinBridge\ThinBridgeSetting.exe"`
5. Go to the section `URLリダイレクト全般設定` and uncheck `アドレスバーに表示されるURLのみを判定する`.
6. Click `設定保存`.
7. Click `OK`.
8. Run a command line: `"C:\Program Files (x86)\Internet Explorer\iexplore.exe" about:blank`
   * This should starts IE with a blank tab.
9. Type and enter a URL `https://powerbi.microsoft.com/ja-jp/` to IE's address bar.
   * This should load the page with IE.
   * Chrome should be started with a URL like `https://publisher.liveperson.net/iframe-le-tag/...` by ThinBridge.
10. Quit both IE and Chrome.
11. Run a command line: `"C:\Program Files\ThinBridge\ThinBridgeSetting.exe"`
12. Go to the section `URLリダイレクト全般設定` and check `アドレスバーに表示されるURLのみを判定する`.
13. Click `設定保存`.

## IE mode of Edge

1. Run a command line: `"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe" --profile-directory=Default edge://blank`
   * This should starts Edge with a blank tab.
2. Type and enter a URL `https://powerbi.microsoft.com/ja-jp/` to Edge's address bar.
   * This should load the page with Edge's IE mode tab.
   * Chrome should not be started.
3. Quit Edge.
4. Run a command line: `"C:\Program Files\ThinBridge\ThinBridgeSetting.exe"`
5. Go to the section `URLリダイレクト全般設定` and uncheck `アドレスバーに表示されるURLのみを判定する`.
6. Click `設定保存`.
7. Click `OK`.
8. Run a command line: `"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe" --profile-directory=Default edge://blank`
   * This should starts Edge with a blank tab.
9. Type and enter a URL `https://powerbi.microsoft.com/ja-jp/` to Edge's address bar.
   * This should load the page with Edge's IE mode tab.
   * Chrome should be started with a URL like `https://publisher.liveperson.net/iframe-le-tag/...` by ThinBridge.
10. Quit both Edge and Chrome.
11. Run a command line: `"C:\Program Files\ThinBridge\ThinBridgeSetting.exe"`
12. Go to the section `URLリダイレクト全般設定` and check `アドレスバーに表示されるURLのみを判定する`.
13. Click `設定保存`.

