# 😴💤 Oton-Zzz

[![IMAGE ALT TEXT HERE](https://github.com/user-attachments/assets/b37cd706-0cfb-4f80-8dd5-6354e5a18b4a)
](https://www.youtube.com/watch?v=lA9EluZugD8)

> テレビをつけたまま寝てしまうオトンを救う、AI睡眠検知システム

## 🔧 使用技術



##  製品概要

### 💡 背景（製品開発のきっかけ、課題等）

「また電気つけっぱなしで寝て…」

オカンは電気代を節約したいと思っているのに、オトンがテレビをつけたまま寝てしまうため困っています。仕方なくオトンを怒っているものの、本当は自動的にテレビを消してくれる仕組みがあれば…という思いから、本システムを開発しました。

現代社会において、不要な電力消費は家計の負担だけでなく環境問題にもつながります。特に、テレビをつけたまま就寝してしまう行動は、多くの家庭で見られる無駄な電力消費の典型例です。

### 🎯 製品説明（具体的な製品の説明）

Oton-Zzzは、カメラとAIを活用して対象者の睡眠状態を自動検知し、テレビを自動的にオフにする電気代節約システムです。

**動作フロー：**
1. カメラで対象者の顔を継続的に監視
2. MediaPipeを使用してまぶたの状態を検出
3. 目が閉じた状態が5分間継続すると「睡眠状態」と判定
4. M5Stickに信号を送り、赤外線でテレビをオフに制御

### ✨ 特長

#### 1. 🧠 AIによる高精度な睡眠検知

MediaPipeの顔ランドマーク検出技術を活用し、まぶたの状態を高精度で判定します。単なる動き検出ではなく、顔の特徴点を追跡することで、テレビを見ながらうたた寝をした場合でも確実に検知できます。

#### 2. 📹 非侵襲的な監視システム

ウェアラブルデバイスなどの装着が不要で、カメラによる映像分析のみで睡眠状態を判定します。利用者は普段通りテレビを視聴するだけで、システムが自動的に動作します。

#### 3. 🔌 既存のテレビに後付け可能

M5StickとIRモジュールを使用した赤外線制御により、既存のテレビに対しても導入可能です。特別な改造や配線工事は不要で、リモコン信号と同じ方式で制御します。

## システム概要図

<img width="2040" height="1696" alt="image(1)" src="https://github.com/user-attachments/assets/bf1aec8e-384a-4b9f-a085-624f29c4ce31" />


## 🎬 デモ動画

[![デモ動画](https://img.youtube.com/vi/lA9EluZugD8/0.jpg)](https://www.youtube.com/watch?v=lA9EluZugD8)

システムの動作デモをご覧いただけます。実際の睡眠検知からテレビ消灯までの一連の流れを確認できます。


### 🎁 解決できること

- **💰 電気代の削減**: テレビをつけたまま寝てしまうことによる無駄な電力消費を防止
- **👨‍👩‍👧‍👦 家族間のストレス軽減**: 「またつけっぱなし！」と怒る必要がなくなり、家庭内の平和を維持
- **🌍 環境への配慮**: 不要な電力消費を削減し、CO2排出量の低減に貢献
- **😴 快適な睡眠環境**: テレビの音や光が睡眠を妨げることを防ぎ、より良い睡眠環境を提供

### 🚀 今後の展望

- **🎤 音声検知機能の追加**: いびき検出など、音声情報も組み合わせた多角的な睡眠判定
- **🏠 スマートホーム連携**: 照明やエアコンなど、他の家電とも連携した総合的な省エネシステムへの拡張
- **📚 学習機能の実装**: 個人の睡眠パターンを学習し、より精度の高い判定を実現
- **👥 複数人対応**: 家族全員を同時に監視し、全員が寝た時だけテレビを消すなどの細かい制御
- **📱 スマホアプリ開発**: 電気代削減効果の可視化や、設定のカスタマイズが可能なモバイルアプリ
- **💰 お手軽価格での実装**: 専用の高価なセンサーや機器を必要とせず、一般家庭でも手軽に導入できる価格帯を実現。ESP32マイコン(約500円)、IRモジュール(約300円)、Webカメラ(約2,000円)、その他部品(約700円)の合計約3,500円で実装可能。既存の高価な睡眠モニタリングシステム(数万円〜)と比較して大幅なコスト削減を実現

### 💪 注力したこと（こだわり等）

* **⏱️ 5分間の継続判定ロジック**: 単に目を閉じただけでテレビが消えてしまうと不便なため、5分間の継続判定を実装。ちょっと目をつぶっただけでは反応せず、本当に寝てしまった場合のみ動作します。
* **📡 M5Stick間の赤外線通信プロトコル**: リモコンとの互換性を保ちながら、独自の制御信号を実装。デモ環境でも実機環境でも動作するよう設計しました。
* **🎭 擬似テレビ環境の構築**: 実際のテレビがなくてもシステムの動作を確認できるよう、動画切り替えによる擬似環境を構築。プレゼンテーションやデモに最適化しました。

## 🛠️ 開発技術

### 💻 技術スタック一覧

<p align="center">
  <img src="https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white" alt="Python"/>
  <img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white" alt="C++"/>
  <img src="https://img.shields.io/badge/OpenCV-5C3EE8?style=for-the-badge&logo=opencv&logoColor=white" alt="OpenCV"/>
  <img src="https://img.shields.io/badge/MediaPipe-0097A7?style=for-the-badge&logo=google&logoColor=white" alt="MediaPipe"/>
  <img src="https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white" alt="Arduino"/>
  <img src="https://img.shields.io/badge/PlatformIO-FF7F00?style=for-the-badge&logo=platformio&logoColor=white" alt="PlatformIO"/>
  <img src="https://img.shields.io/badge/M5Stack-0D1117?style=for-the-badge&logo=espressif&logoColor=white" alt="M5Stack"/>
</p>

#### 🔌 API・データ

* **MediaPipe**: Googleが提供する機械学習ソリューション。顔のランドマーク検出とブレンドシェイプ解析に使用
* **OpenCV**: 映像処理とカメラ入力の取得に使用

#### 📚 フレームワーク・ライブラリ・モジュール

**🐍 Python側:**
* `opencv-python`: カメラ映像の取得と処理
* `mediapipe`: 顔認識とランドマーク検出
* `pyserial`: M5Stickとのシリアル通信

**⚙️ M5Stick側 (C++/Arduino):**
* `M5StickCPlus2`: M5StickC Plus2のハードウェア制御
* `IRremoteESP8266`: 赤外線信号の送受信制御

#### 🔧 デバイス

* **M5StickC Plus2** (2台): 赤外線送受信とシリアル通信を担当
* **Webカメラ**: 対象者の顔を撮影
* **PC** (2台, Windows): 睡眠検知処理とテレビ擬似環境
* **赤外線送受信モジュール**: テレビ制御用の赤外線通信
* **リモコン**: テレビ操作のリファレンス信号取得

### 🌟 独自技術

#### 🏆 ハッカソンで開発した独自機能・技術

* **睡眠状態の継続判定アルゴリズム**: MediaPipeの顔ランドマーク検出結果を時系列で分析し、5分間の継続的な目の閉じ状態を判定するロジックを実装
  - 実装ファイル: [`code/Oton_Zzz/python/main.py`](code/Oton_Zzz/python/main.py)

* **PC-M5Stick間のシリアル通信プロトコル**: 睡眠検知結果を確実にM5Stickに伝達するための独自プロトコル設計（"ALERT", "AWAKE", "OFF"コマンド）
  - 検出PC側: [`code/Oton_Zzz/python/main.py`](code/Oton_Zzz/python/main.py)
  - M5Stick側: [`code/Oton_Zzz/m5stick/src/main.cpp`](code/Oton_Zzz/m5stick/src/main.cpp)

* **M5Stick間の赤外線通信システム**: NEC方式の赤外線通信を使用した、M5Stick同士およびリモコンとの双方向通信の実装
  - IR送信制御: [`code/Oton_Zzz/m5stick/src/ir_controller.cpp`](code/Oton_Zzz/m5stick/src/ir_controller.cpp)
  - IR受信処理: [`code/TVmoc/m5stick/src/main.cpp`](code/TVmoc/m5stick/src/main.cpp)

* **テレビ擬似環境システム**: 動画の切り替えによってテレビのON/OFFを演出する、デモに最適化された擬似環境
  - 実装ファイル: [`code/TVmoc/python/main.py`](code/TVmoc/python/main.py)


## 📁 プロジェクト構成

```
os_2512/
├── code/
│   ├── Oton_Zzz/          # 睡眠検知・制御システム
│   │   ├── python/        # 睡眠検知AI
│   │   └── m5stick/       # IR送信制御
│   └── TVmoc/             # テレビ擬似環境
│       ├── python/        # 動画切替演出
│       └── m5stick/       # IR受信
├── utils/                 # 共通ユーティリティ
├── DEMO/                  # デモ用資料
├── 仕様書.md             # 詳細仕様書
└── README.md             # このファイル
```
