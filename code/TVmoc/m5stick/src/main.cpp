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

// ★★★ NECプロトコルのボタン定義 ★★★
IRButton necButtons[] = {
  IRButton("TV_OFF", 0x11A00FF, 0xA0),
  IRButton("CH_1",   0x11A807F, 0xA8),
  IRButton("CH_2",   0x11AD22D, 0xAD),
  IRButton("CH_3",   0x11A52AD, 0xA5),
  IRButton("CH_4",   0x11A5AA5, 0xA5),
  IRButton("CH_5",   0x11A28D7, 0xA2),
  // IRButton("CH_6",   0x11A????, 0xA?)  // 配列に追加するだけ
  // IRButton("CH_7",   0x11A????, 0xA?)  // 配列に追加するだけ
  // IRButton("CH_8",   0x11A????, 0xA?)  // 配列に追加するだけ
  // IRButton("CH_9",   0x11A????, 0xA?)  // 配列に追加するだけ
  // IRButton("CH_10",   0x11A????, 0xA?)  // 配列に追加するだけ
  // IRButton("CH_11",   0x11A????, 0xA?)  // 配列に追加するだけ
  // IRButton("CH_12",   0x11A????, 0xA?)  // 配列に追加するだけ
};

// NECプロトコルオブジェクト
IRProtocol necProtocol("NEC", 0x11, necButtons, 6);

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

// ★★★ ボタン識別用の関数（プロトコルクラスを使用） ★★★
String identifyButton(uint64_t value, uint8_t command) {
  // まずValueで検索
  const char* buttonName = necProtocol.findButtonByValue(value);
  if (buttonName != nullptr) {
    return String(buttonName);
  }

  // Valueで見つからない場合、Commandで検索
  buttonName = necProtocol.findButtonByCommand(command);
  if (buttonName != nullptr) {
    return String(buttonName);
  }

  return "UNKNOWN";
}

void loop() {
  // 赤外線信号を受信した場合
  if (irrecv.decode(&results)) {

    // ★★★ クラスを使用した2段階フィルタ ★★★
    if (FILTER_ENABLED) {
      bool isMatch = false;

      // 1. まずアドレスとコマンドで判定（両方が0でない場合のみ有効）
      if (results.address != 0 && results.command != 0) {
        if (necProtocol.matchButton(results.address, results.command)) {
          isMatch = true;
        }
      }

      // 2. アドレス&コマンドで判定できなかった場合、Value全体で判定
      if (!isMatch) {
        if (necProtocol.matchButtonByValue(results.value)) {
          isMatch = true;
        }
      }

      // 3. どちらにも一致しなければ無視
      if (!isMatch) {
        irrecv.resume();  // 次の信号を受信準備
        return;  // 対象外の信号は無視
      }
    }

    // ボタンを識別
    String buttonName = identifyButton(results.value, results.command);

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
