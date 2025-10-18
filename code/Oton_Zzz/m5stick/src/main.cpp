#include "M5StickCPlus2.h"
#include "config.h"
#include "ir_controller.h"
#include "display_manager.h"
#include "button_handler.h"

// グローバルオブジェクト
IRController irController;
DisplayManager displayManager;
ButtonHandler buttonHandler;

// プログラムの状態
Mode currentMode = SEND_MODE;
TVstatus currentTVstatus = TV_OFF;

// 関数の前方宣言
void updateDisplay();
void handleSendSignal();
void handleRegisterMode();
void toggleMode();
void handleSerialCommand();
void handleSendModeMonitoring();

void toggleMode() {
  if (currentMode == SEND_MODE) {
    currentMode = REGISTER_MODE;
    irController.resetRegistration();
    Serial.println("Entered register mode - waiting for 3 identical signals...");
    Serial.println("Repeat signals will be automatically ignored");
  } else {
    currentMode = SEND_MODE;
    Serial.println("Returned to send mode");
  }
  updateDisplay();
}

void toggleTVstatus() {
  if (currentTVstatus == TV_OFF) {
    currentTVstatus = TV_ON;
    displayManager.showMessage("TV is now ON", 1500);
  } else {
    currentTVstatus = TV_OFF;
    displayManager.showMessage("TV is now OFF", 1500);
  }
  updateDisplay();
}

void updateDisplay() {
  if (currentMode == SEND_MODE) {
    displayManager.showSendMode(irController.isSignalRegistered(), irController.getRegisteredSignal());
  } else {
    displayManager.showRegisterMode(irController.getRegisterCount());
  }
}

void handleSendSignal() {
  if (irController.sendRegisteredSignal()) {
    displayManager.showSignalSentMessage();
    delay(1000);
  } else {
    if (!irController.isSignalRegistered()) {
      displayManager.showNoSignalMessage();
      delay(2000);
    } else {
      displayManager.showErrorMessage("Unsupported protocol!");
      delay(2000);
    }
  }
  updateDisplay();
}

void handleRegisterMode() {
  bool registrationComplete = irController.handleRegisterMode();
  
  if (registrationComplete) {
    Serial.println("Registration completed successfully!");
    currentMode = SEND_MODE;
    updateDisplay();
  } else if (irController.isRegistrationTimeout()) {
    updateDisplay();  // IRController内でリセット済み
  }
}

void handleSerialCommand() {
  if (!Serial.available() || currentMode != SEND_MODE) {
    return;
  }
  String command = Serial.readStringUntil('\n');
  command.trim();  // 前後の空白や改行を削除
  Serial.printf("Received command: '%s'\n", command.c_str());
  if (command == "OFF") {
    if (currentTVstatus == TV_OFF) {
      Serial.println("TV is already OFF");
      return;
    }
    Serial.println("OFF command received - sending IR signal");
    handleSendSignal();
  } else if (command == "ALERT") {
    Serial.println("ALERT command received - sending IR signal");
    // アラート画像を表示
  } else {
    Serial.printf("Unknown command: '%s'\n", command.c_str());
    Serial.println("Send 'HELP' for available commands");
  }
}

void handleSendModeMonitoring() {
  // 送信モード時に登録済み信号を監視
  if (irController.checkForRegisteredSignal()) {
    // 登録済み信号を受信した場合の処理
    toggleTVstatus();
  }
}

void setup() {
  M5.begin();
  Serial.begin(115200);
  
  // 各オブジェクトの初期化
  irController.begin();
  displayManager.begin();
  buttonHandler.begin();
  
  Serial.println("M5StickC Plus2 IR Remote Control");
  Serial.println("A: Send registered signal");
  Serial.println("B: Enter register mode");
  Serial.println("Serial Commands:");
  Serial.println("  TV_OFF - Send IR signal");
  Serial.println("  STATUS - Show status");
  Serial.println("  HELP   - Show commands");
  Serial.println("-------------------------");
  
  updateDisplay();
}

void loop() {
  buttonHandler.update();
  
  // シリアル通信でのコマンド処理
  handleSerialCommand();
  
  // Aボタン：登録された信号を送信（ボタンでも送信可能）
  if (buttonHandler.isButtonAPressed()) {
    if (currentMode == SEND_MODE) {
      toggleTVstatus();
    }
  }
  
  // Bボタン：登録モードの切り替え
  if (buttonHandler.isButtonBPressed()) {
    toggleMode();
  }
  
  // モード別の処理
  if (currentMode == REGISTER_MODE) {
    // 登録モード時の受信処理
    handleRegisterMode();
  } else {
    // 送信モード時の信号監視
    handleSendModeMonitoring();
  }
  
  delay(50);
}