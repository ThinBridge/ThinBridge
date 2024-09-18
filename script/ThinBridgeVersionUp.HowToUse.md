# ThinBridgeVersionUp の使い方

「ThinBridgeVersionUp」は、ThinBridge のバージョンアップ用スクリプトである

## 機能の概要

- パラメータで指定したバージョン番号`$new_version` の値へ一括更新する

### バージョン情報の付与ルール

- `vx.y.z.b`とする。先頭の「v」に続けて、0 以上の 4 つの整数 x y z b を.（ピリオド）で区切って表記する
- x,y,z,b の値域はいずれも 0-99（数字 1 桁または 2 桁）とする

## 実行手順

1. エクスプローラー上で`script` フォルダを右クリックして「PowerShell ウィンドウをここで開く」を選択する
2. PowerShell ウィンドウで下記を実施する

```PowerShell
~\browserselector\script> powershell.exe -ExecutionPolicy Bypass -file .\ThinBridgeVersionUp.ps1 [$new_version]
```

- 引数なしで`.\ThinBridgeVersionUp.ps1` を実行した時は、リポジトリ内のバージョン情報の grep 結果を表示する
- `$new_version` は 0 以上の 4 つの整数 x y z b を .（ピリオド）で区切って指定する（例：`.\ThinBridgeVersionUp.ps1 4.2.1.0`）。「v」は不要
- 指定した`$new_version` が上記「バージョン情報の付与ルール」に違反している場合、エラーとする

### テストファイルの実行手順

PowerShell のテスティングフレームワーク「[Pester](https://pester.dev/)」にて実行する。手順は次のとおり

1. エクスプローラー上で`script` フォルダを右クリックして「PowerShell ウィンドウをここで開く」を選択する
2. PowerShell ウィンドウで下記を実施する

```PowerShell
~\browserselector\script> powershell.exe -ExecutionPolicy Bypass Invoke-Pester
```

フォルダ内のテストファイル`ThinBridgeVersionUp.Tests.ps1` が実行される

- 上記テストは、`script` 以外のパスから Invoke すると一部"Failed" となる（`script` を基準として更新対象ファイルの存在チェックを行っているため）

### 実行後の注意事項

- 当スクリプトによって変更したファイルのコミットの前に、diff を確認すること
