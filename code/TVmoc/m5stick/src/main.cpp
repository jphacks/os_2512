#include "M5StickCPlus2.h"
#include <IRrecv.h>
#include <IRutils.h>

// 赤外線受信ピン
const uint16_t RECV_PIN = 33;

// IR受信オブジェクトを作成
IRrecv irrecv(RECV_PIN);
decode_results results;

// ★★★ IRボタン情報を管理する構造体 ★★★
struct IRButton {
  const char* name;        // ボタン名
  uint64_t value;          // 完全なValue値
  uint8_t command;         // コマンドコード

  IRButton(const char* n, uint64_t v, uint8_t c)
    : name(n), value(v), command(c) {}
};

// ★★★ IRプロトコルを管理するクラス ★★★
class IRProtocol {
private:
  const char* protocolName;
  uint16_t address;
  IRButton* buttons;
  int buttonCount;

public:
  IRProtocol(const char* name, uint16_t addr, IRButton* btns, int count)
    : protocolName(name), address(addr), buttons(btns), buttonCount(count) {}

  // アドレスが一致するかチェック
  bool matchAddress(uint16_t addr) const {
    return addr == address;
  }

  // Value値からボタンを検索
  const char* findButtonByValue(uint64_t value) const {
    for (int i = 0; i < buttonCount; i++) {
      if (buttons[i].value == value) {
        return buttons[i].name;
      }
    }
    return nullptr;
  }

  // コマンドコードからボタンを検索
  const char* findButtonByCommand(uint8_t command) const {
    for (int i = 0; i < buttonCount; i++) {
      if (buttons[i].command == command) {
        return buttons[i].name;
      }
    }
    return nullptr;
  }

  // アドレス&コマンドで一致するか確認
  bool matchButton(uint16_t addr, uint8_t cmd) const {
    if (addr != address) return false;
    return findButtonByCommand(cmd) != nullptr;
  }

  // Valueで一致するか確認
  bool matchButtonByValue(uint64_t value) const {
    return findButtonByValue(value) != nullptr;
  }

  const char* getProtocolName() const { return protocolName; }
};

// ★★★ NECプロトコルのボタン定義（コメントアウト） ★★★
// IRButton necButtons[] = {
//   IRButton("TV_POWER", 0x11A00FF, 0xA0),
//   IRButton("CH_1",   0x11A807F, 0xA8),
//   IRButton("CH_2",   0x11A6897, 0xA6),
//   IRButton("CH_3",   0x11AD22D, 0xAD),
//   IRButton("CH_4",   0x11A52AD, 0xA5),
//   IRButton("CH_5",   0x11A5AA5, 0xA5),
//   IRButton("CH_6",   0x11A28D7, 0xA2),
//   IRButton("CH_7",   0x11AE817, 0xAE),
//   IRButton("CH_8",   0x11ACA35, 0xAC),
//   IRButton("CH_9",   0x11AD02F, 0xAD),
//   IRButton("CH_10",   0x11A4AB5, 0xA4),
//   IRButton("CH_11",   0x11A0AF5, 0xA0),
//   IRButton("CH_12",   0x11A8A75, 0xA8)
// };
// IRProtocol necProtocol("NEC", 0x11, necButtons, 13);

// ★★★ PANASONICプロトコルのボタン定義 ★★★
IRButton panasonicButtons[] = {
  IRButton("TV_POWER", 0x555AF148688B, 0x00),
  IRButton("TV_1",     0x555AF148724C, 0x00),
  IRButton("TV_2",     0x555AF148F244, 0x00),
  IRButton("TV_3",     0x555AF1480A43, 0x00),
  IRButton("TV_4",     0x555AF1488A4B, 0x00),
  IRButton("TV_5",     0x555AF1484A47, 0x00),
  IRButton("TV_6",     0x555AF148CA4F, 0x00),
  IRButton("TV_7",     0x555AF1482A41, 0x00),
  IRButton("TV_8",     0x555AF148AA49, 0x00),
  IRButton("TV_9",     0x555AF1486A45, 0x00),
  IRButton("TV_10",    0x555AF148EA4D, 0x00),
  IRButton("TV_11",    0x555AF1481A42, 0x00),
  IRButton("TV_12",    0x555AF1489A4A, 0x00)
};

// PANASONICプロトコルオブジェクト
IRProtocol panasonicProtocol("PANASONIC", 0x00, panasonicButtons, 13);

const bool FILTER_ENABLED = true;  // trueにすると特定の信号のみ検出

void setup() {
  M5.begin();
  Serial.begin(115200);

  // ディスプレイの初期設定
  M5.Lcd.setRotation(3);  // 横向きに設定
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);  // フォントサイズを大きく設定
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("IR Receiver Ready");

  irrecv.enableIRIn();  // IR受信を開始
}

// ★★★ ボタン識別用の関数（完全なValue値で検索） ★★★
String identifyButton(uint64_t value) {
  // Value値で検索
  const char* buttonName = panasonicProtocol.findButtonByValue(value);
  if (buttonName != nullptr) {
    return String(buttonName);
  }

  return "UNKNOWN";
}

void loop() {
  // 赤外線信号を受信した場合
  if (irrecv.decode(&results)) {

    // ★★★ 完全なValue値でフィルタリング ★★★
    if (FILTER_ENABLED) {
      // Value値が登録されているボタンと一致するかチェック
      if (!panasonicProtocol.matchButtonByValue(results.value)) {
        irrecv.resume();  // 次の信号を受信準備
        return;  // 登録されていない信号は無視
      }
    }

    // ボタンを識別（完全なValue値で検索）
    String buttonName = identifyButton(results.value);

    // ★★★ PCへ送信するシンプルなテキストメッセージ ★★★
    Serial.println(buttonName);

    // ディスプレイをクリア
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);

    // ディスプレイに受信情報を表示
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(3);
    M5.Lcd.println(buttonName);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);

    // プロトコル名を表示
    String protocol = typeToString(results.decode_type);
    M5.Lcd.printf("Type: %s\n", protocol.c_str());

    // 値を表示
    M5.Lcd.printf("Value:\n0x%llX\n", (unsigned long long)results.value);

    // 受信した信号を再度準備
    irrecv.resume();
  }

  delay(100);
}
