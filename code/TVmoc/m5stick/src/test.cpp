
#include "M5StickCPlus2.h"
#include <IRrecv.h>
#include <IRutils.h>

// 赤外線受信ピン
const uint16_t RECV_PIN = 33;

// IR受信オブジェクトを作成
IRrecv irrecv(RECV_PIN);
decode_results results;

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

void loop() {
  // 赤外線信号を受信した場合
  if (irrecv.decode(&results)) {
    // ディスプレイをクリア
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);

    // ディスプレイに受信情報を表示
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.println("IR Received!");
    M5.Lcd.setTextColor(WHITE);

    // プロトコル名を表示
    String protocol = typeToString(results.decode_type);
    M5.Lcd.printf("Type: %s\n", protocol.c_str());

    // 値を表示
    M5.Lcd.printf("Value: 0x%llX\n", (unsigned long long)results.value);

    // ビット数を表示
    M5.Lcd.printf("Bits: %d\n", results.bits);

    // 基本的な受信内容を表示
    Serial.println("=== IR Signal Received ===");
    Serial.println(resultToHumanReadableBasic(&results));

    // より詳細な情報を表示
    Serial.println("=== Detailed Information ===");
    Serial.println(resultToSourceCode(&results));

    // 生データ（16進数）も表示
    Serial.print("Raw Data (hex): ");
    for (uint16_t i = 1; i < results.rawlen; i++) {
      if (i % 16 == 1) Serial.print("\n");
      Serial.print(results.rawbuf[i] * kRawTick);
      if (i < results.rawlen - 1) Serial.print(", ");
    }
    Serial.println();

    Serial.println("========================");
    Serial.println();

    // 受信した信号を再度準備
    irrecv.resume();
  }

  delay(100);
}
