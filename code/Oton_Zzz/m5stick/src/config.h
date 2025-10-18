#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>

// IRremoteESP8266ライブラリの定数
#ifndef kRawTick
#define kRawTick 2  // IRremoteESP8266ライブラリのデフォルト値
#endif

// 赤外線送信・受信ピン (M5StickC Plus2)
const uint16_t IR_LED_PIN = 19;
const uint16_t RECV_PIN = 33;

// タイミング設定
const unsigned long RECEIVE_TIMEOUT = 10000;  // 10秒のタイムアウト
const unsigned long REPEAT_IGNORE_TIME = 500;  // リピート信号を無視する時間（0.5秒）
const int REGISTRATION_ATTEMPTS = 3;  // 登録に必要な受信回数
const uint16_t MIN_SIGNAL_BITS = 20;  // 最小有効ビット数

// プログラムの状態
enum Mode {
  SEND_MODE,      // 送信モード（デフォルト）
  REGISTER_MODE   // 登録モード
};

// 信号保存用の構造体
struct IRSignal {
  decode_type_t protocol;
  uint64_t value;
  uint16_t bits;
  uint16_t* rawData;
  uint16_t rawLen;
  bool isRegistered;
  
  // コンストラクタ
  IRSignal() : protocol(UNKNOWN), value(0), bits(0), rawData(nullptr), rawLen(0), isRegistered(false) {}
};

// 色定義
#define BLACK     0x0000
#define WHITE     0xFFFF
#define RED       0xF800
#define GREEN     0x07E0
#define BLUE      0x001F
#define DARKGREY  0x4208

#endif // CONFIG_H