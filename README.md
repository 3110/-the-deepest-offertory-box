# 世界一深い賽銭箱

世界一深い賽銭箱があると，ついお賽銭を入れたくなりませんか？

[![世界一深い賽銭箱](/images/the-deepest-offetory-box-youtube.png)](https://www.youtube.com/watch?v=967QOeop2Vo)

今回作ったしくみは，スピーカーを内蔵したATOM Echoと距離が測定できるToF Unitを使用し，距離の変化をトリガーとして何らかのリアクションを返す（今回の場合は音を鳴らす）しくみとして汎用的に使用できます。

将来的に他の距離が測れるユニット（Ultrasonic Unit）や全く別のユニットを使用したトリガーにも対応する予定です。

## 必要なもの

- [ATOM Echo](https://www.switch-science.com/products/6347)
- [ToF Unit](https://www.switch-science.com/products/5219)  
  ATOM Echoと接続するためのGROVEケーブルも付属しています。
- 賽銭箱  
  [セリア](https://www.seria-group.com/)で購入
- 超強粘着 粘着ゲル 両面テープ CROCODILE GRIP  
  [ダイソー](https://www.daiso-sangyo.co.jp/)で購入。ATOM EchoやToF Unitを賽銭箱に固定するのに使用。強めに貼れてはがせるので便利
- 音源ファイル（WAV）  
  動画で使用している音源は以下の2つのファイルを結合して1つのファイルにしています。  
  - [Springin' Sound Stock](https://www.springin.org/sound-stock/)の[演出](https://www.springin.org/sound-stock/category/staging/)にある「落下 2」
  - [Springin' Sound Stock](https://www.springin.org/sound-stock/)の[生活](https://www.springin.org/sound-stock/subcategory/money/)にある「お賽銭 1」

## コンパイル方法

VSCodeのPlatformIO IDEが必要です。Windows 11上で動くVSCodeで動作確認をしています。

## 音源ファイルの転送

使用する音源ファイル（WAV形式）を`sound-effect.wav`という名前で`data`フォルダに置いてください。SPIFFS（SPI Flash File System）に転送して使用します。

ファイルを置いたら，PlatformIO メニューから「Upload Filesystem Image」を選択するか，コマンドラインから`pio run --target uploadfs`を実行してSPIFFS にアップロードします。

## 使用方法

実際に使用する場所にATOM EchoとToFセンサーを設置してください。
![接続例](/images/assembled-image.png)

![設置例](/images/installed-image.png)

USBケーブルを挿して起動すれば，自動的にデフォルトの距離を測定してその平均値を閾値として設定し，待機します。

ATOM Echoのボタンを押すとATOM EchoのLEDが緑色に点灯し，距離の測定が始まります。測定距離が短くなる（今回の例ではお金が賽銭箱を通った）とトリガーが発火し，落下音がします。

### 測定距離の閾値の設定

測定距離の閾値とは，ToFセンサーの測定値がこの値以下になったらトリガーが発火し，リアクションを返す（今回の場合は音を鳴らす）値です。

初めて起動する場合，もしくはATOM Echoのボタンを押したまま起動した場合に閾値設定モードになります。デフォルトでは距離を10回測定（測定するたびにATOM EchoのLEDが青点滅する）し，その平均値を閾値とします。

閾値はATOM Echoの不揮発記憶装置（NVS: Non-Volatile Storage）に記録されるので，次に起動するときは記録された閾値を使うようになります。再度設定し直したい場合は，ATOM Echoのボタンを押しながら起動させてください。

ToF Unitは測定精度が±3%なので，閾値との±3%以内の距離の違いは誤差として扱います。

### 距離測定の有効化・無効化

起動時には距離測定が無効（ATOM EchoのボタンのLEDが消灯）になっています。ATOM Echoのボタンを押すと，LEDが緑色に点灯して測定を開始します。ATOM Echoのボタンを押すごとに有効・無効が切り替わります。

## 配布用ファームウェアの作成

M5Burnerで配布するファームウェアを作成するには，PlatformIOメニューにあるPROJECT TASKSからCustomの下にある「Generate User Custom」を選択します。

![Generate User Customの選択](/images/generate-user-custom.png)

ビルドが完了すると，`platformio.ini`の`env`セクションにある`custom_firmware_dir`で指定したディレクトリ（デフォルトは`firmware`）に配布用のファームウェア（`.bin`ファイル）が生成されます。

M5Burnerでの配布方法は[M5Burner v3の使いかた](https://zenn.dev/saitotetsuya/articles/m5stack_m5burner_v3)を参照してください。
