# 故障時の対応

## 🌟 3D プリンターの部品の交換

./stl ディレクトリに全パーツの STL データがあります。**TODO**

## 🌟 電子部品の交換

./bom.csv に部品表があります。**TODO**

## 🌟 マイコンの交換

./firmware ディレクトリに Arduino IDE 用のソースコードがあります。交換後 ./firmware/firmware.ino を Arduino IDE で開き、Raspberry Pi Pico W 向けにビルド、書き込みを行ってください。以下ビルド時に必要な情報です。

| 項目                 | 内容                                                                                                |
| -------------------- | --------------------------------------------------------------------------------------------------- |
| マイコン             | Raspberry Pi Pico W                                                                                 |
| ボードマネージャ URL | <https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json> |
| 依存ライブラリ       | <https://github.com/waspinator/AccelStepper>                                                        |

## 🌟 SD カードの故障、誤消去

以下のディレクトリ構成を SD カードに作成してください。

Web アプリ用のファイルは ./slicer ディレクトリにあります。ap.txt についは [ネットワーク設定](#ネットワーク設定) で説明しています。

```txt
SDカード
  ├── ap.txt
  └── app
      ├── index.html
      ├── style.css
      └── script.js
```
